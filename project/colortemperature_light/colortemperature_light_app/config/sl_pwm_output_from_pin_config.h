/*
 * PWM OUTPUT_PORT/PIN fallbacks when Pin Tool only emits PERIPHERAL in sl_pwm_init_pwm*_config.h.
 * GPIO routing follows pin_config.h: TIMER0 CC0 -> PB4 (cool), TIMER1 CC0 -> PB5 (warm).
 * If Pin Tool later defines SL_PWM_PWMx_OUTPUT_*, those take precedence (#ifndef).
 */

#ifndef SL_PWM_OUTPUT_FROM_PIN_CONFIG_H
#define SL_PWM_OUTPUT_FROM_PIN_CONFIG_H

#include "pin_config.h"

#ifndef SL_PWM_PWM0_OUTPUT_CHANNEL
#define SL_PWM_PWM0_OUTPUT_CHANNEL 0
#endif
#ifndef SL_PWM_PWM0_OUTPUT_PORT
#define SL_PWM_PWM0_OUTPUT_PORT TIMER0_CC0_PORT
#endif
#ifndef SL_PWM_PWM0_OUTPUT_PIN
#define SL_PWM_PWM0_OUTPUT_PIN TIMER0_CC0_PIN
#endif

#ifndef SL_PWM_PWM1_OUTPUT_CHANNEL
#define SL_PWM_PWM1_OUTPUT_CHANNEL 0
#endif
#ifndef SL_PWM_PWM1_OUTPUT_PORT
#define SL_PWM_PWM1_OUTPUT_PORT TIMER1_CC0_PORT
#endif
#ifndef SL_PWM_PWM1_OUTPUT_PIN
#define SL_PWM_PWM1_OUTPUT_PIN TIMER1_CC0_PIN
#endif

#endif // SL_PWM_OUTPUT_FROM_PIN_CONFIG_H
