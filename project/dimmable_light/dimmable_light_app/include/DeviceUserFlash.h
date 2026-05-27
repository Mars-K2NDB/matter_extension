/*
 * User flash partition: power-cycle factory reset and saved light color state.
 */

#pragma once

#include <cstdint>
#include <lib/core/CHIPError.h>
#include <lib/core/DataModelTypes.h>

namespace DeviceUserFlash {

/** Color mode values (aligned with five-lamp product conventions). */
enum class LightColorMode : uint8_t
{
    kOff = 0,
    kCt  = 1,
    kHsv = 2,
    kXy  = 3,
};

struct SavedLightState
{
    LightColorMode colorMode = LightColorMode::kCt;
    uint8_t        brightness = 0; // Matter CurrentLevel (1-254), 0 = unset
    uint16_t       ctMireds   = 0;
    uint8_t        hue        = 0;
    uint8_t        saturation = 0;
};

/** Register connectivity event handler (call after App event queue exists). */
void Init();

/** Allow attribute-change callbacks to persist light state (call at end of InitLight). */
void EnablePersistedLightStateSave();

/**
 * Power-cycle reset: 5 boots within 10 s triggers factory reset.
 * @return true if factory reset was scheduled (caller may return early).
 */
bool ProcessPowerCycleReset();

/** Load saved color from flash into RAM cache (does not turn the lamp on). */
void LoadSavedLightState();

/** True after LoadSavedLightState() found a valid LGT1 record in flash. */
bool HasPersistedLightState();

const SavedLightState & GetCachedLightState();

/** Write Matter level/on-off from cache; forces OnOff off (caller holds chip stack lock). */
void ApplyCachedLightStateToMatter(chip::EndpointId endpoint);

/** Persist current Matter color/level attributes (caller holds chip stack lock). */
void SaveLightStateFromMatter(chip::EndpointId endpoint);

/** Update cache from attribute-change callback and write flash. */
void UpdateLightStateFromAttributeChange(chip::EndpointId endpoint, chip::ClusterId clusterId,
                                         chip::AttributeId attributeId);

/** Report current light attributes to subscribed commissioners. */
void ReportCurrentLightEffect(chip::EndpointId endpoint);

} // namespace DeviceUserFlash
