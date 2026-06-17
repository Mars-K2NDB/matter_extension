/*
 * User flash partition: power-cycle factory reset and saved light color state.
 */

#include "device_user_flash.h"

#include "AppConfig.h"
#include "light_output.h"

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/reporting/reporting.h>
#include <app/server/Server.h>
#include <clusters/LevelControl/AttributeIds.h>
#include <clusters/OnOff/AttributeIds.h>
#include <em_device.h>
#include <em_msc.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceLayer.h>
#include <protocols/interaction_model/StatusCode.h>
#include <system/SystemClock.h>

#include <cstring>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::DeviceLayer;
using namespace chip::Protocols::InteractionModel;

// One flash page immediately below the NVM3 region (linker_nvm_begin is always defined).
extern "C" char linker_nvm_begin;

namespace {

constexpr uint32_t kMagicPowerCycle = 0x50435231u; // "PCR1"
constexpr uint32_t kMagicLightState = 0x4C475431u; // "LGT1"
constexpr uint16_t kLightStateVersion = 1;

constexpr uint32_t kPowerCycleWindowSec = 10;
constexpr uint8_t kPowerCycleTriggerCount = 5;

constexpr size_t kUserFlashSize     = FLASH_PAGE_SIZE;
constexpr size_t kLightRecordOffset = 128;

/** Default Matter level when flash record has no brightness (legacy). */
constexpr uint8_t kDefaultRestoredLevel = 254;

struct PowerCycleRecord
{
    uint32_t magic;
    uint8_t count;
    uint8_t reserved[3];
};

struct SavedLightStateRecord
{
    uint32_t magic;
    uint16_t version;
    uint8_t color_mode;
    uint8_t brightness;
    uint16_t ct_mireds;
    uint8_t hue;
    uint8_t saturation;
    uint8_t reserved[7];
};

// RAM scratch for read-modify-write (never put 8 KB structs on the App task stack).
static uint8_t flash_scratch_[kUserFlashSize];

static device_user_flash::SavedLightState cached_light_state_;
static bool cached_light_valid_           = false;
static bool allow_flash_save_             = false;
static bool suppress_attribute_flash_save_ = false;

uint8_t* UserFlashBase()
{
    return reinterpret_cast<uint8_t*>(&linker_nvm_begin) - kUserFlashSize;
}

bool IsUserFlashInFlashRange()
{
    const uintptr_t addr = reinterpret_cast<uintptr_t>(UserFlashBase());
    return (addr >= 0x08000000U) && (addr < 0x08200000U);
}

void LoadScratchFromFlash()
{
    std::memcpy(flash_scratch_, UserFlashBase(), kUserFlashSize);
}

CHIP_ERROR CommitScratchToFlash()
{
    uint32_t* const base = reinterpret_cast<uint32_t*>(UserFlashBase());
    MSC_Init();
    if (MSC_ErasePage(base) != mscReturnOk)
    {
        MSC_Deinit();
        ChipLogError(DeviceLayer, "User flash: MSC_ErasePage failed at 0x%08lx", reinterpret_cast<unsigned long>(base));
        return CHIP_ERROR_INTERNAL;
    }
    if (MSC_WriteWord(base, flash_scratch_, kUserFlashSize) != mscReturnOk)
    {
        MSC_Deinit();
        ChipLogError(DeviceLayer, "User flash: MSC_WriteWord failed at 0x%08lx", reinterpret_cast<unsigned long>(base));
        return CHIP_ERROR_INTERNAL;
    }
    MSC_Deinit();
    return CHIP_NO_ERROR;
}

void ReadPowerCycleRecord(PowerCycleRecord & pc)
{
    LoadScratchFromFlash();
    std::memcpy(&pc, flash_scratch_, sizeof(pc));
}

void WritePowerCycleRecord(const PowerCycleRecord& pc)
{
    LoadScratchFromFlash();
    std::memcpy(flash_scratch_, &pc, sizeof(pc));
    const CHIP_ERROR err = CommitScratchToFlash();
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(DeviceLayer, "Power-cycle reset: failed to save count=%u: %" CHIP_ERROR_FORMAT, pc.count, err.Format());
    }
}

void ReadLightRecord(SavedLightStateRecord & rec)
{
    LoadScratchFromFlash();
    std::memcpy(&rec, flash_scratch_ + kLightRecordOffset, sizeof(rec));
}

CHIP_ERROR WriteLightRecord(const SavedLightStateRecord& rec)
{
    LoadScratchFromFlash();
    std::memcpy(flash_scratch_ + kLightRecordOffset, &rec, sizeof(rec));
    return CommitScratchToFlash();
}

void PersistCachedLightStateToFlash()
{
    if (!cached_light_valid_)
    {
        return;
    }

    SavedLightStateRecord rec{};
    std::memset(&rec, 0, sizeof(rec));
    rec.magic      = kMagicLightState;
    rec.version    = kLightStateVersion;
    rec.color_mode  = static_cast<uint8_t>(cached_light_state_.color_mode);
    rec.brightness = cached_light_state_.brightness;
    rec.ct_mireds   = cached_light_state_.ct_mireds;
    rec.hue        = cached_light_state_.hue;
    rec.saturation = cached_light_state_.saturation;

    const CHIP_ERROR err = WriteLightRecord(rec);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(DeviceLayer, "Light state: failed to write flash (ct=%u level=%u): %" CHIP_ERROR_FORMAT,
                     cached_light_state_.ct_mireds, cached_light_state_.brightness, err.Format());
    }
    else
    {
        ChipLogDetail(DeviceLayer, "Light state: saved to flash (ct=%u level=%u mode=%u)", cached_light_state_.ct_mireds,
                      cached_light_state_.brightness, static_cast<unsigned>(cached_light_state_.color_mode));
    }
}

void OnPowerCycleWindowTimer(System::Layer* /*layer*/, void* /*ctx*/)
{
    PowerCycleRecord pc{};
    ReadPowerCycleRecord(pc);
    const uint8_t prevCount = pc.count;
    if (pc.magic != kMagicPowerCycle)
    {
        return;
    }
    pc.count = 0;
    WritePowerCycleRecord(pc);
    ChipLogProgress(DeviceLayer, "Power-cycle reset: %us window expired, count cleared (%u -> 0)", kPowerCycleWindowSec,
                  prevCount);
}

void SchedulePowerCycleWindowExpiry()
{
    PlatformMgr().LockChipStack();
    auto & systemLayer = DeviceLayer::SystemLayer();
    systemLayer.CancelTimer(OnPowerCycleWindowTimer, nullptr);
    const CHIP_ERROR err =
        systemLayer.StartTimer(System::Clock::Seconds32(kPowerCycleWindowSec), OnPowerCycleWindowTimer, nullptr);
    PlatformMgr().UnlockChipStack();
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(DeviceLayer, "Power-cycle reset: failed to start %us window timer: %" CHIP_ERROR_FORMAT,
                     kPowerCycleWindowSec, err.Format());
    }
}

device_user_flash::SavedLightState RecordToCached(const SavedLightStateRecord& rec)
{
    device_user_flash::SavedLightState out;
    out.color_mode  = static_cast<device_user_flash::LightColorMode>(rec.color_mode);
    out.brightness = rec.brightness;
    out.ct_mireds   = rec.ct_mireds;
    out.hue        = rec.hue;
    out.saturation = rec.saturation;
    return out;
}

void ConnectivityEventHandler(const ChipDeviceEvent* event, intptr_t /*arg*/)
{
    if (event->Type != DeviceEventType::kThreadConnectivityChange)
    {
        return;
    }
    if (event->ThreadConnectivityChange.Result != ConnectivityChange::kConnectivity_Established)
    {
        return;
    }
    device_user_flash::ReportCurrentLightEffect(LIGHT_ENDPOINT);
}

} // namespace

namespace device_user_flash {

void Init()
{
    TEMPORARY_RETURN_IGNORED PlatformMgr().AddEventHandler(ConnectivityEventHandler, 0);
}

void EnablePersistedLightStateSave()
{
    allow_flash_save_ = true;
}

bool ProcessPowerCycleReset()
{
    if (!IsUserFlashInFlashRange())
    {
        ChipLogError(DeviceLayer, "Power-cycle reset: user flash at 0x%08lx is not in flash",
                     reinterpret_cast<unsigned long>(UserFlashBase()));
        return false;
    }

    PowerCycleRecord pc{};
    ReadPowerCycleRecord(pc);

    if (pc.magic != kMagicPowerCycle)
    {
        std::memset(&pc, 0, sizeof(pc));
        pc.magic = kMagicPowerCycle;
        pc.count = 0;
    }

    pc.count++;

    if (pc.count >= kPowerCycleTriggerCount)
    {
        pc.count = 0;
        WritePowerCycleRecord(pc);
        ChipLogProgress(DeviceLayer, "Power-cycle reset: count reached %u, scheduling factory reset", kPowerCycleTriggerCount);
        Server::GetInstance().ScheduleFactoryReset();
        return true;
    }

    WritePowerCycleRecord(pc);
    SchedulePowerCycleWindowExpiry();
    return false;
}

void LoadSavedLightState()
{
    SavedLightStateRecord rec{};
    ReadLightRecord(rec);
    if (rec.magic != kMagicLightState || rec.version != kLightStateVersion)
    {
        cached_light_valid_ = false;
        ChipLogProgress(DeviceLayer, "Light state: no valid flash record (magic=0x%08lx ver=%u)",
                        static_cast<unsigned long>(rec.magic), rec.version);
        return;
    }

    cached_light_state_ = RecordToCached(rec);
    cached_light_valid_ = true;
    ChipLogProgress(DeviceLayer, "Light state: loaded from flash (ct=%u level=%u mode=%u)", cached_light_state_.ct_mireds,
                  cached_light_state_.brightness, static_cast<unsigned>(cached_light_state_.color_mode));
}

bool HasPersistedLightState()
{
    return cached_light_valid_;
}

const SavedLightState& GetCachedLightState()
{
    return cached_light_state_;
}

void ApplyCachedLightStateToMatter(EndpointId endpoint)
{
    if (!cached_light_valid_)
    {
        return;
    }

    suppress_attribute_flash_save_ = true;

    const SavedLightState& s = cached_light_state_;

    const uint8_t level_to_apply = (s.brightness > 0) ? s.brightness : kDefaultRestoredLevel;
    LevelControl::Attributes::CurrentLevel::Set(endpoint, level_to_apply);

    // Boot default: off, but level restored for next on (dimmable endpoint has no Color Control).
    OnOff::Attributes::OnOff::Set(endpoint, false);

    suppress_attribute_flash_save_ = false;

    ChipLogProgress(DeviceLayer, "Light state: applied to Matter (level=%u on=0)", level_to_apply);
}

void SaveLightStateFromMatter(EndpointId endpoint)
{
    SavedLightState s = cached_light_valid_ ? cached_light_state_ : SavedLightState{};

    app::DataModel::Nullable<uint8_t> level;
    if (LevelControl::Attributes::CurrentLevel::Get(endpoint, level) == Status::Success && !level.IsNull() && level.Value() > 0)
    {
        s.brightness = level.Value();
    }

    cached_light_state_ = s;
    cached_light_valid_   = true;

    PersistCachedLightStateToFlash();
}

void UpdateLightStateFromAttributeChange(EndpointId endpoint, ClusterId cluster_id, AttributeId attribute_id)
{
    if (!allow_flash_save_ || suppress_attribute_flash_save_)
    {
        return;
    }

    const bool relevant =
        (cluster_id == LevelControl::Id && attribute_id == LevelControl::Attributes::CurrentLevel::Id);

    if (!relevant)
    {
        return;
    }

    PlatformMgr().LockChipStack();
    SaveLightStateFromMatter(endpoint);
    PlatformMgr().UnlockChipStack();
}

void ReportCurrentLightEffect(EndpointId endpoint)
{
    PlatformMgr().LockChipStack();
    MatterReportingAttributeChangeCallback(ConcreteAttributePath(endpoint, OnOff::Id, OnOff::Attributes::OnOff::Id));
    MatterReportingAttributeChangeCallback(
        ConcreteAttributePath(endpoint, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id));
    PlatformMgr().UnlockChipStack();
    ChipLogProgress(AppServer, "Light state: reported attributes to subscribers after reconnect");
}

void PrepareLevelControlForOnOffRestore(EndpointId endpoint)
{
    if (LevelControl::Attributes::OnLevel::SetNull(endpoint) == Status::Success)
    {
        ChipLogProgress(DeviceLayer, "LevelControl: OnLevel cleared (use CurrentLevel on OnOff)");
    }
}

} // namespace device_user_flash
