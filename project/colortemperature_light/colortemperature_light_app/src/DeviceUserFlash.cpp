/*
 * User flash partition: power-cycle factory reset and saved light color state.
 */

#include "DeviceUserFlash.h"

#include "AppConfig.h"
#include "LightOutput.h"

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/reporting/reporting.h>
#include <app/server/Server.h>
#include <clusters/ColorControl/AttributeIds.h>
#include <clusters/ColorControl/Enums.h>
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
    uint8_t colorMode;
    uint8_t brightness;
    uint16_t ctMireds;
    uint8_t hue;
    uint8_t saturation;
    uint8_t reserved[7];
};

// RAM scratch for read-modify-write (never put 8 KB structs on the App task stack).
static uint8_t sFlashScratch[kUserFlashSize];

static DeviceUserFlash::SavedLightState sCachedLightState;
static bool sCachedLightValid           = false;
static bool sAllowFlashSave             = false;
static bool sSuppressAttributeFlashSave = false;

uint8_t * UserFlashBase()
{
    return reinterpret_cast<uint8_t *>(&linker_nvm_begin) - kUserFlashSize;
}

bool IsUserFlashInFlashRange()
{
    const uintptr_t addr = reinterpret_cast<uintptr_t>(UserFlashBase());
    return (addr >= 0x08000000U) && (addr < 0x08200000U);
}

void LoadScratchFromFlash()
{
    std::memcpy(sFlashScratch, UserFlashBase(), kUserFlashSize);
}

CHIP_ERROR CommitScratchToFlash()
{
    uint32_t * const base = reinterpret_cast<uint32_t *>(UserFlashBase());
    MSC_Init();
    if (MSC_ErasePage(base) != mscReturnOk)
    {
        MSC_Deinit();
        ChipLogError(DeviceLayer, "User flash: MSC_ErasePage failed at 0x%08lx", reinterpret_cast<unsigned long>(base));
        return CHIP_ERROR_INTERNAL;
    }
    if (MSC_WriteWord(base, sFlashScratch, kUserFlashSize) != mscReturnOk)
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
    std::memcpy(&pc, sFlashScratch, sizeof(pc));
}

void WritePowerCycleRecord(const PowerCycleRecord & pc)
{
    LoadScratchFromFlash();
    std::memcpy(sFlashScratch, &pc, sizeof(pc));
    const CHIP_ERROR err = CommitScratchToFlash();
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(DeviceLayer, "Power-cycle reset: failed to save count=%u: %" CHIP_ERROR_FORMAT, pc.count, err.Format());
    }
}

void ReadLightRecord(SavedLightStateRecord & rec)
{
    LoadScratchFromFlash();
    std::memcpy(&rec, sFlashScratch + kLightRecordOffset, sizeof(rec));
}

CHIP_ERROR WriteLightRecord(const SavedLightStateRecord & rec)
{
    LoadScratchFromFlash();
    std::memcpy(sFlashScratch + kLightRecordOffset, &rec, sizeof(rec));
    return CommitScratchToFlash();
}

void PersistCachedLightStateToFlash()
{
    if (!sCachedLightValid)
    {
        return;
    }

    SavedLightStateRecord rec{};
    std::memset(&rec, 0, sizeof(rec));
    rec.magic      = kMagicLightState;
    rec.version    = kLightStateVersion;
    rec.colorMode  = static_cast<uint8_t>(sCachedLightState.colorMode);
    rec.brightness = sCachedLightState.brightness;
    rec.ctMireds   = sCachedLightState.ctMireds;
    rec.hue        = sCachedLightState.hue;
    rec.saturation = sCachedLightState.saturation;

    const CHIP_ERROR err = WriteLightRecord(rec);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(DeviceLayer, "Light state: failed to write flash (ct=%u level=%u): %" CHIP_ERROR_FORMAT,
                     sCachedLightState.ctMireds, sCachedLightState.brightness, err.Format());
    }
    else
    {
        ChipLogDetail(DeviceLayer, "Light state: saved to flash (ct=%u level=%u mode=%u)", sCachedLightState.ctMireds,
                      sCachedLightState.brightness, static_cast<unsigned>(sCachedLightState.colorMode));
    }
}

void OnPowerCycleWindowTimer(System::Layer * /*layer*/, void * /*ctx*/)
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

DeviceUserFlash::SavedLightState RecordToCached(const SavedLightStateRecord & rec)
{
    DeviceUserFlash::SavedLightState out;
    out.colorMode  = static_cast<DeviceUserFlash::LightColorMode>(rec.colorMode);
    out.brightness = rec.brightness;
    out.ctMireds   = rec.ctMireds;
    out.hue        = rec.hue;
    out.saturation = rec.saturation;
    return out;
}

void ConnectivityEventHandler(const ChipDeviceEvent * event, intptr_t /*arg*/)
{
    if (event->Type != DeviceEventType::kThreadConnectivityChange)
    {
        return;
    }
    if (event->ThreadConnectivityChange.Result != ConnectivityChange::kConnectivity_Established)
    {
        return;
    }
    DeviceUserFlash::ReportCurrentLightEffect(LIGHT_ENDPOINT);
}

} // namespace

namespace DeviceUserFlash {

void Init()
{
    TEMPORARY_RETURN_IGNORED PlatformMgr().AddEventHandler(ConnectivityEventHandler, 0);
}

void EnablePersistedLightStateSave()
{
    sAllowFlashSave = true;
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
        sCachedLightValid = false;
        ChipLogProgress(DeviceLayer, "Light state: no valid flash record (magic=0x%08lx ver=%u)",
                        static_cast<unsigned long>(rec.magic), rec.version);
        return;
    }

    sCachedLightState = RecordToCached(rec);
    sCachedLightValid = true;
    ChipLogProgress(DeviceLayer, "Light state: loaded from flash (ct=%u level=%u mode=%u)", sCachedLightState.ctMireds,
                  sCachedLightState.brightness, static_cast<unsigned>(sCachedLightState.colorMode));
}

bool HasPersistedLightState()
{
    return sCachedLightValid;
}

const SavedLightState & GetCachedLightState()
{
    return sCachedLightState;
}

void ApplyCachedLightStateToMatter(EndpointId endpoint)
{
    if (!sCachedLightValid)
    {
        return;
    }

    sSuppressAttributeFlashSave = true;

    const SavedLightState & s = sCachedLightState;

    const uint8_t levelToApply = (s.brightness > 0) ? s.brightness : kDefaultRestoredLevel;
    LevelControl::Attributes::CurrentLevel::Set(endpoint, levelToApply);

    uint16_t ctMireds = s.ctMireds;
    if (ctMireds < LightOutput::kCtMinMireds || ctMireds > LightOutput::kCtMaxMireds)
    {
        ctMireds = LightOutput::kDefaultCtMireds;
    }
    ColorControl::Attributes::ColorTemperatureMireds::Set(endpoint, ctMireds);

    switch (s.colorMode)
    {
    case LightColorMode::kCt:
        ColorControl::Attributes::ColorMode::Set(endpoint, ColorControl::ColorModeEnum::kColorTemperatureMireds);
        ColorControl::Attributes::EnhancedColorMode::Set(endpoint, ColorControl::EnhancedColorModeEnum::kColorTemperatureMireds);
        break;
    case LightColorMode::kHsv:
        ColorControl::Attributes::CurrentHue::Set(endpoint, s.hue);
        ColorControl::Attributes::CurrentSaturation::Set(endpoint, s.saturation);
        ColorControl::Attributes::ColorMode::Set(endpoint, ColorControl::ColorModeEnum::kCurrentHueAndCurrentSaturation);
        ColorControl::Attributes::EnhancedColorMode::Set(endpoint,
                                                         ColorControl::EnhancedColorModeEnum::kCurrentHueAndCurrentSaturation);
        break;
    case LightColorMode::kXy:
        ColorControl::Attributes::ColorMode::Set(endpoint, ColorControl::ColorModeEnum::kCurrentXAndCurrentY);
        ColorControl::Attributes::EnhancedColorMode::Set(endpoint, ColorControl::EnhancedColorModeEnum::kCurrentXAndCurrentY);
        break;
    default:
        ColorControl::Attributes::ColorMode::Set(endpoint, ColorControl::ColorModeEnum::kColorTemperatureMireds);
        ColorControl::Attributes::EnhancedColorMode::Set(endpoint, ColorControl::EnhancedColorModeEnum::kColorTemperatureMireds);
        break;
    }

    // Boot default: off, but level/CT restored for next on.
    OnOff::Attributes::OnOff::Set(endpoint, false);

    sSuppressAttributeFlashSave = false;

    ChipLogProgress(DeviceLayer, "Light state: applied to Matter (ct=%u level=%u on=0)", ctMireds, levelToApply);
}

void SaveLightStateFromMatter(EndpointId endpoint)
{
    SavedLightState s = sCachedLightValid ? sCachedLightState : SavedLightState{};

    app::DataModel::Nullable<uint8_t> level;
    if (LevelControl::Attributes::CurrentLevel::Get(endpoint, level) == Status::Success && !level.IsNull() && level.Value() > 0)
    {
        s.brightness = level.Value();
    }

    uint16_t ct = 0;
    if (ColorControl::Attributes::ColorTemperatureMireds::Get(endpoint, &ct) == Status::Success)
    {
        s.ctMireds = ct;
    }

    ColorControl::ColorModeEnum mode = ColorControl::ColorModeEnum::kColorTemperatureMireds;
    if (ColorControl::Attributes::ColorMode::Get(endpoint, &mode) == Status::Success)
    {
        switch (mode)
        {
        case ColorControl::ColorModeEnum::kCurrentHueAndCurrentSaturation:
            s.colorMode = LightColorMode::kHsv;
            break;
        case ColorControl::ColorModeEnum::kCurrentXAndCurrentY:
            s.colorMode = LightColorMode::kXy;
            break;
        default:
            s.colorMode = LightColorMode::kCt;
            break;
        }
    }

    uint8_t hue = 0;
    uint8_t sat = 0;
    if (ColorControl::Attributes::CurrentHue::Get(endpoint, &hue) == Status::Success)
    {
        s.hue = hue;
    }
    if (ColorControl::Attributes::CurrentSaturation::Get(endpoint, &sat) == Status::Success)
    {
        s.saturation = sat;
    }

    sCachedLightState = s;
    sCachedLightValid   = true;

    PersistCachedLightStateToFlash();
}

void UpdateLightStateFromAttributeChange(EndpointId endpoint, ClusterId clusterId, AttributeId attributeId)
{
    if (!sAllowFlashSave || sSuppressAttributeFlashSave)
    {
        return;
    }

    const bool relevant = (clusterId == LevelControl::Id && attributeId == LevelControl::Attributes::CurrentLevel::Id) ||
        (clusterId == ColorControl::Id &&
         (attributeId == ColorControl::Attributes::ColorTemperatureMireds::Id ||
          attributeId == ColorControl::Attributes::ColorMode::Id ||
          attributeId == ColorControl::Attributes::EnhancedColorMode::Id ||
          attributeId == ColorControl::Attributes::CurrentHue::Id ||
          attributeId == ColorControl::Attributes::CurrentSaturation::Id));

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
    MatterReportingAttributeChangeCallback(
        ConcreteAttributePath(endpoint, ColorControl::Id, ColorControl::Attributes::ColorTemperatureMireds::Id));
    MatterReportingAttributeChangeCallback(ConcreteAttributePath(endpoint, ColorControl::Id, ColorControl::Attributes::ColorMode::Id));
    MatterReportingAttributeChangeCallback(
        ConcreteAttributePath(endpoint, ColorControl::Id, ColorControl::Attributes::EnhancedColorMode::Id));
    MatterReportingAttributeChangeCallback(ConcreteAttributePath(endpoint, ColorControl::Id, ColorControl::Attributes::CurrentHue::Id));
    MatterReportingAttributeChangeCallback(
        ConcreteAttributePath(endpoint, ColorControl::Id, ColorControl::Attributes::CurrentSaturation::Id));
    PlatformMgr().UnlockChipStack();
    ChipLogProgress(AppServer, "Light state: reported attributes to subscribers after reconnect");
}

} // namespace DeviceUserFlash
