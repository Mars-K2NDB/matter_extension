#!/usr/bin/env bash
# Normalize hand-written Matter light app sources (Google C++ style).
# Usage: normalize_light_project.sh <app_dir> <driver_src_basename>
# Example: normalize_light_project.sh dimmable_light/dimmable_light_app single_pwm_driver

set -euo pipefail

APP_DIR="${1:?app dir required}"
DRIVER_BASE="${2:?driver basename required}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
APP_PATH="${ROOT}/${APP_DIR}"
INC="${APP_PATH}/include"
SRC="${APP_PATH}/src"

if [[ ! -d "${INC}" || ! -d "${SRC}" ]]; then
  echo "missing include/ or src/ under ${APP_PATH}" >&2
  exit 1
fi

rename_if() {
  local dir="$1" old="$2" new="$3"
  if [[ -f "${dir}/${old}" ]]; then
    mv "${dir}/${old}" "${dir}/${new}"
  fi
}

# --- file renames (snake_case) ---
rename_if "${INC}" DeviceUserFlash.h device_user_flash.h
rename_if "${SRC}" DeviceUserFlash.cpp device_user_flash.cpp
rename_if "${INC}" LightOutput.h light_output.h
rename_if "${INC}" OvercurrentProtector.h overcurrent_protector.h
rename_if "${SRC}" OvercurrentProtector.cpp overcurrent_protector.cpp
rename_if "${INC}" ShortCircuitProtector.h short_circuit_protector.h
rename_if "${SRC}" ShortCircuitProtector.cpp short_circuit_protector.cpp
rename_if "${INC}" VoltageAdcDriver.h voltage_adc_driver.h
rename_if "${SRC}" VoltageAdcDriver.cpp voltage_adc_driver.cpp

# driver-specific (PascalCase -> snake_case filename)
for pair in \
  "SinglePwmDriver:single_pwm_driver" \
  "RgbcwPwmDriver:rgbcw_pwm_driver" \
  "CtDualPwmDriver:ct_dual_pwm_driver" \
  "CtPwmVersion:ct_pwm_version"; do
  old="${pair%%:*}"
  new="${pair##*:}"
  rename_if "${INC}" "${old}.h" "${new}.h"
  rename_if "${SRC}" "${old}.cpp" "${new}.cpp"
done

apply_common_sed() {
  local f="$1"
  sed -i \
    -e 's|"DeviceUserFlash\.h"|"device_user_flash.h"|g' \
    -e 's|"LightOutput\.h"|"light_output.h"|g' \
    -e 's|"OvercurrentProtector\.h"|"overcurrent_protector.h"|g' \
    -e 's|"ShortCircuitProtector\.h"|"short_circuit_protector.h"|g' \
    -e 's|"VoltageAdcDriver\.h"|"voltage_adc_driver.h"|g' \
    -e 's|"SinglePwmDriver\.h"|"single_pwm_driver.h"|g' \
    -e 's|"RgbcwPwmDriver\.h"|"rgbcw_pwm_driver.h"|g' \
    -e 's|"CtDualPwmDriver\.h"|"ct_dual_pwm_driver.h"|g' \
    -e 's|"CtPwmVersion\.h"|"ct_pwm_version.h"|g' \
    -e 's/namespace DeviceUserFlash/namespace device_user_flash/g' \
    -e 's/} \/\/ namespace DeviceUserFlash/} \/\/ namespace device_user_flash/g' \
    -e 's/namespace LightOutput/namespace light_output/g' \
    -e 's/} \/\/ namespace LightOutput/} \/\/ namespace light_output/g' \
    -e 's/DeviceUserFlash::/device_user_flash::/g' \
    -e 's/LightOutput::/light_output::/g' \
    -e 's/ResolveLevelForPwmLocked/ResolveLevelForCluster/g' \
    -e 's/ResolveLevelForPwm(/ResolveLevelForCluster(/g' \
    -e 's/clusterLevel/cluster_level/g' \
    -e 's/ctMireds/ct_mireds/g' \
    -e 's/colorMode/color_mode/g' \
    -e 's/clusterId/cluster_id/g' \
    -e 's/attributeId/attribute_id/g' \
    -e 's/attributePath/attribute_path/g' \
    -e 's/timerCbArg/timer_cb_arg/g' \
    -e 's/restartFade/restart_fade/g' \
    -e 's/appState/app_state/g' \
    -e 's/durationMs/duration_ms/g' \
    -e 's/levelToApply/level_to_apply/g' \
    -e 's/wasOn/was_on/g' \
    -e 's/kEventType_CtPwm/kEventType_LightStrip/g' \
    -e 's/CtPwmEventKind/LightStripEventKind/g' \
    -e 's/kCtPwmOn/kOnOff/g' \
    -e 's/kCtPwmLevel/kLevel/g' \
    -e 's/kCtPwmCt/kColorTemp/g' \
    -e 's/CtPwmEvent/light_strip_event/g' \
    -e 's/PwmEventHandler/LightStripEventHandler/g' \
    -e 's/PostPwmEvent/PostLightStripEvent/g' \
    -e 's/sOutputsReady/outputs_ready_/g' \
    -e 's/sHueSatApplyScheduled/hue_sat_apply_scheduled_/g' \
    -e 's/sHueSatApplyEndpoint/hue_sat_apply_endpoint_/g' \
    -e 's/sXyApplyScheduled/xy_apply_scheduled_/g' \
    -e 's/sXyApplyEndpoint/xy_apply_endpoint_/g' \
    -e 's/\.Kind/.kind/g' \
    -e 's/\.On/.on/g' \
    -e 's/\.Level/.level/g' \
    -e 's/\.CtMireds/.ct_mireds/g' \
    -e 's/System::Layer \*/System::Layer*/g' \
    -e 's/void \*/void*/g' \
    -e 's/uint8_t \*/uint8_t*/g' \
    -e 's/uint32_t \*/uint32_t*/g' \
    -e 's/AppEvent \*/AppEvent*/g' \
    -e 's/const AppEvent &/const AppEvent\&/g' \
    -e 's/const SavedLightState &/const SavedLightState\&/g' \
    -e 's/const PowerCycleRecord &/const PowerCycleRecord\&/g' \
    -e 's/const SavedLightStateRecord &/const SavedLightStateRecord\&/g' \
    -e 's/const ChipDeviceEvent \*/const ChipDeviceEvent*/g' \
    -e 's/bool &/bool\&/g' \
    -e 's/uint8_t &/uint8_t\&/g' \
    -e 's/uint16_t &/uint16_t\&/g' \
    -e 's/ConcreteAttributePath &/ConcreteAttributePath\&/g' \
    "${f}"
}

apply_flash_static_sed() {
  local f="$1"
  sed -i \
    -e 's/sCachedLightState/cached_light_state_/g' \
    -e 's/sCachedLightValid/cached_light_valid_/g' \
    -e 's/sAllowFlashSave/allow_flash_save_/g' \
    -e 's/sSuppressAttributeFlashSave/suppress_attribute_flash_save_/g' \
    -e 's/sFlashScratch/flash_scratch_/g' \
    "${f}"
}

apply_overcurrent_static_sed() {
  local f="$1"
  sed -i \
    -e 's/sPollActive/poll_active_/g' \
    -e 's/IsFaultActive/IsFaultActive/g' \
    -e 's/sFaultActive/fault_active_/g' \
    -e 's/Ifault_active_/IsFaultActive/g' \
    -e 's/sSnapValid/snap_valid_/g' \
    -e 's/sSnapOn/snap_on_/g' \
    -e 's/sSnapLevel/snap_level_/g' \
    -e 's/sSnapCtMireds/snap_ct_mireds_/g' \
    -e 's/sMatterOffPending/matter_off_pending_/g' \
    -e 's/sAvgMillivolts/avg_millivolts_/g' \
    -e 's/sRecoveryMs/recovery_ms_/g' \
    -e 's/sSampleRing/sample_ring_/g' \
    -e 's/sSampleCount/sample_count_/g' \
    -e 's/sSampleIndex/sample_index_/g' \
    "${f}"
}

apply_single_pwm_static_sed() {
  local f="$1"
  sed -i \
    -e 's/SinglePwmDriver::sOn/SinglePwmDriver::on_/g' \
    -e 's/SinglePwmDriver::sLevel/SinglePwmDriver::level_/g' \
    -e 's/SinglePwmDriver::sPwmStarted/SinglePwmDriver::pwm_started_/g' \
    -e 's/SinglePwmDriver::sRouteDisabled/SinglePwmDriver::route_disabled_/g' \
    -e 's/SinglePwmDriver::sPreFaultSaved/SinglePwmDriver::pre_fault_saved_/g' \
    -e 's/SinglePwmDriver::sPreFaultOn/SinglePwmDriver::pre_fault_on_/g' \
    -e 's/SinglePwmDriver::sPreFaultLevel/SinglePwmDriver::pre_fault_level_/g' \
    -e 's/SinglePwmDriver::sDisplayDuty/SinglePwmDriver::display_duty_/g' \
    -e 's/SinglePwmDriver::sFadeStartDuty/SinglePwmDriver::fade_start_duty_/g' \
    -e 's/SinglePwmDriver::sFadeTargetDuty/SinglePwmDriver::fade_target_duty_/g' \
    -e 's/SinglePwmDriver::sFadeStep/SinglePwmDriver::fade_step_/g' \
    -e 's/SinglePwmDriver::sFadeStepsTotal/SinglePwmDriver::fade_steps_total_/g' \
    -e 's/SinglePwmDriver::sFadeActive/SinglePwmDriver::fade_active_/g' \
    -e 's/\bsOn\b/on_/g' \
    -e 's/\bsLevel\b/level_/g' \
    -e 's/\bsPwmStarted\b/pwm_started_/g' \
    -e 's/\bsRouteDisabled\b/route_disabled_/g' \
    -e 's/\bsPreFaultSaved\b/pre_fault_saved_/g' \
    -e 's/\bsPreFaultOn\b/pre_fault_on_/g' \
    -e 's/\bsPreFaultLevel\b/pre_fault_level_/g' \
    -e 's/\bsDisplayDuty\b/display_duty_/g' \
    -e 's/\bsFadeStartDuty\b/fade_start_duty_/g' \
    -e 's/\bsFadeTargetDuty\b/fade_target_duty_/g' \
    -e 's/\bsFadeStep\b/fade_step_/g' \
    -e 's/\bsFadeStepsTotal\b/fade_steps_total_/g' \
    -e 's/\bsFadeActive\b/fade_active_/g' \
    "${f}"
}

apply_ct_dual_static_sed() {
  local f="$1"
  sed -i \
    -e 's/CtDualPwmDriver::sOn/CtDualPwmDriver::on_/g' \
    -e 's/CtDualPwmDriver::sLevel/CtDualPwmDriver::level_/g' \
    -e 's/CtDualPwmDriver::sCtMireds/CtDualPwmDriver::ct_mireds_/g' \
    -e 's/CtDualPwmDriver::sPwmStarted/CtDualPwmDriver::pwm_started_/g' \
    -e 's/CtDualPwmDriver::sRouteDisabled/CtDualPwmDriver::route_disabled_/g' \
    -e 's/CtDualPwmDriver::sPreFaultSaved/CtDualPwmDriver::pre_fault_saved_/g' \
    -e 's/CtDualPwmDriver::sPreFaultOn/CtDualPwmDriver::pre_fault_on_/g' \
    -e 's/CtDualPwmDriver::sPreFaultLevel/CtDualPwmDriver::pre_fault_level_/g' \
    -e 's/CtDualPwmDriver::sPreFaultCtMireds/CtDualPwmDriver::pre_fault_ct_mireds_/g' \
    -e 's/CtDualPwmDriver::sLastOnValid/CtDualPwmDriver::last_on_valid_/g' \
    -e 's/CtDualPwmDriver::sLastOnLevel/CtDualPwmDriver::last_on_level_/g' \
    -e 's/CtDualPwmDriver::sLastOnCtMireds/CtDualPwmDriver::last_on_ct_mireds_/g' \
    -e 's/CtDualPwmDriver::sDisplayCoolDuty/CtDualPwmDriver::display_cool_duty_/g' \
    -e 's/CtDualPwmDriver::sDisplayWarmDuty/CtDualPwmDriver::display_warm_duty_/g' \
    -e 's/CtDualPwmDriver::sFadeStartBrightness/CtDualPwmDriver::fade_start_brightness_/g' \
    -e 's/CtDualPwmDriver::sFadeTargetBrightness/CtDualPwmDriver::fade_target_brightness_/g' \
    -e 's/CtDualPwmDriver::sFadeStartWarmRatioFp/CtDualPwmDriver::fade_start_warm_ratio_fp_/g' \
    -e 's/CtDualPwmDriver::sFadeTargetWarmRatioFp/CtDualPwmDriver::fade_target_warm_ratio_fp_/g' \
    -e 's/CtDualPwmDriver::sFadeKind/CtDualPwmDriver::fade_kind_/g' \
    -e 's/CtDualPwmDriver::sFadeStep/CtDualPwmDriver::fade_step_/g' \
    -e 's/CtDualPwmDriver::sFadeStepsTotal/CtDualPwmDriver::fade_steps_total_/g' \
    -e 's/CtDualPwmDriver::sFadeActive/CtDualPwmDriver::fade_active_/g' \
    -e 's/\bsOn\b/on_/g' \
    -e 's/\bsLevel\b/level_/g' \
    -e 's/\bsCtMireds\b/ct_mireds_/g' \
    -e 's/\bsPwmStarted\b/pwm_started_/g' \
    -e 's/\bsRouteDisabled\b/route_disabled_/g' \
    -e 's/\bsPreFaultSaved\b/pre_fault_saved_/g' \
    -e 's/\bsPreFaultOn\b/pre_fault_on_/g' \
    -e 's/\bsPreFaultLevel\b/pre_fault_level_/g' \
    -e 's/\bsPreFaultCtMireds\b/pre_fault_ct_mireds_/g' \
    -e 's/\bsLastOnValid\b/last_on_valid_/g' \
    -e 's/\bsLastOnLevel\b/last_on_level_/g' \
    -e 's/\bsLastOnCtMireds\b/last_on_ct_mireds_/g' \
    -e 's/\bsDisplayCoolDuty\b/display_cool_duty_/g' \
    -e 's/\bsDisplayWarmDuty\b/display_warm_duty_/g' \
    -e 's/\bsFadeStartBrightness\b/fade_start_brightness_/g' \
    -e 's/\bsFadeTargetBrightness\b/fade_target_brightness_/g' \
    -e 's/\bsFadeStartWarmRatioFp\b/fade_start_warm_ratio_fp_/g' \
    -e 's/\bsFadeTargetWarmRatioFp\b/fade_target_warm_ratio_fp_/g' \
    -e 's/\bsFadeKind\b/fade_kind_/g' \
    -e 's/\bsFadeStep\b/fade_step_/g' \
    -e 's/\bsFadeStepsTotal\b/fade_steps_total_/g' \
    -e 's/\bsFadeActive\b/fade_active_/g' \
    "${f}"
}

apply_rgbcw_static_sed() {
  local f="$1"
  sed -i \
    -e 's/sRuntime/runtime_/g' \
    -e 's/sFault/fault_/g' \
    -e 's/sLastOn/last_on_/g' \
    -e 's/sDisplay/display_/g' \
    -e 's/sFade/fade_/g' \
    -e 's/\.useCt/.use_ct/g' \
    -e 's/stepsTotal/steps_total/g' \
    "${f}"
}

for f in "${INC}"/*.{h,hpp} "${SRC}"/*.cpp; do
  [[ -f "${f}" ]] || continue
  apply_common_sed "${f}"
  apply_flash_static_sed "${f}"
  apply_overcurrent_static_sed "${f}"
done

if [[ -f "${SRC}/single_pwm_driver.cpp" ]]; then
  for f in "${INC}/single_pwm_driver.h" "${SRC}/single_pwm_driver.cpp"; do
    apply_single_pwm_static_sed "${f}"
  done
fi

if [[ -f "${SRC}/ct_dual_pwm_driver.cpp" ]]; then
  for f in "${INC}/ct_dual_pwm_driver.h" "${SRC}/ct_dual_pwm_driver.cpp"; do
    apply_ct_dual_static_sed "${f}"
  done
fi

if [[ -f "${SRC}/rgbcw_pwm_driver.cpp" ]]; then
  for f in "${INC}/rgbcw_pwm_driver.h" "${SRC}/rgbcw_pwm_driver.cpp"; do
    apply_rgbcw_static_sed "${f}"
  done
fi

# slcp + cmake
for cfg in "${APP_PATH}"/*.slcp "${APP_PATH}"/cmake_gcc/*.cmake "${APP_PATH}"/cmake_llvm/*.cmake "${APP_PATH}"/cmake_iar/*.cmake; do
  [[ -f "${cfg}" ]] || continue
  sed -i \
    -e 's/DeviceUserFlash/device_user_flash/g' \
    -e 's/LightOutput/light_output/g' \
    -e 's/OvercurrentProtector/overcurrent_protector/g' \
    -e 's/ShortCircuitProtector/short_circuit_protector/g' \
    -e 's/VoltageAdcDriver/voltage_adc_driver/g' \
    -e 's/SinglePwmDriver/single_pwm_driver/g' \
    -e 's/RgbcwPwmDriver/rgbcw_pwm_driver/g' \
    -e 's/CtDualPwmDriver/ct_dual_pwm_driver/g' \
    -e 's/CtPwmVersion/ct_pwm_version/g' \
    "${cfg}"
done

echo "normalized ${APP_PATH}"
