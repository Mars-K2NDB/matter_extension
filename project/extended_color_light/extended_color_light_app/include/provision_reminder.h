/*
 * Provisioning reminder: cool-white 500 ms blink, max 60 s, interruptible by light switch.
 */

#pragma once

namespace provision_reminder {

/** Register connectivity handler (idempotent). */
void Init();

/** True when Thread/Wi-Fi network credentials are stored. */
bool IsDeviceProvisioned();

/** Start blink when device is not yet provisioned (call after PWM outputs are ready). */
void TryStartOnBoot();

/** Stop blink when the light-switch button is pressed; caller continues normal toggle. */
void OnLightSwitchPressed();

/** True while the provisioning reminder overlay is driving PWM output. */
bool IsActive();

} // namespace provision_reminder
