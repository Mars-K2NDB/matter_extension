/*
 * PB00 falling edge -> ISR: PWM off + shared protection latch (OvercurrentProtector).
 */

#include "ShortCircuitProtector.h"

#include "OvercurrentProtector.h"
#include "short_circuit_protect_config.h"

#include <lib/support/logging/CHIPLogging.h>

#include "sl_clock_manager.h"
#include "sl_gpio.h"
#include "sl_status.h"

int32_t ShortCircuitProtector::sIntNo            = SL_GPIO_INTERRUPT_UNAVAILABLE;
bool ShortCircuitProtector::sInterruptEnabled = false;

namespace {

sl_gpio_t ShortCircuitGpio()
{
    return {
        .port = SHORT_CIRCUIT_GPIO_PORT,
        .pin  = SHORT_CIRCUIT_GPIO_PIN,
    };
}

void ShortCircuitGpioIsr(uint8_t intNo, void * context)
{
    (void) intNo;
    (void) context;
    ShortCircuitProtector::HandleInterrupt();
}

} // namespace

void ShortCircuitProtector::HandleInterrupt()
{
    DisableInterrupt();
    OvercurrentProtector::TripFromIsr();
}

void ShortCircuitProtector::EnableInterrupt()
{
    if (sIntNo == SL_GPIO_INTERRUPT_UNAVAILABLE || sInterruptEnabled)
    {
        return;
    }

    const sl_status_t status = sl_gpio_enable_interrupts(1U << static_cast<uint32_t>(sIntNo));
    if (status == SL_STATUS_OK)
    {
        sInterruptEnabled = true;
    }
}

void ShortCircuitProtector::DisableInterrupt()
{
    if (sIntNo == SL_GPIO_INTERRUPT_UNAVAILABLE || !sInterruptEnabled)
    {
        return;
    }

    (void) sl_gpio_disable_interrupts(1U << static_cast<uint32_t>(sIntNo));
    sInterruptEnabled = false;
}

bool ShortCircuitProtector::IsPinOk()
{
    bool level = false;
    const sl_gpio_t gpio = ShortCircuitGpio();
    if (sl_gpio_get_pin_input(&gpio, &level) != SL_STATUS_OK)
    {
        return false;
    }
    return level == (SHORT_CIRCUIT_GPIO_OK_LEVEL != 0);
}

void ShortCircuitProtector::OnRecover()
{
    EnableInterrupt();
}

void ShortCircuitProtector::Init()
{
    if (sIntNo != SL_GPIO_INTERRUPT_UNAVAILABLE)
    {
        return;
    }

    (void) sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_GPIO);

    const sl_gpio_t gpio = ShortCircuitGpio();
    (void) sl_gpio_set_pin_mode(&gpio, SL_GPIO_MODE_INPUT_PULL, 1);

    int32_t intNo = SL_GPIO_INTERRUPT_UNAVAILABLE;
    const sl_status_t status =
        sl_gpio_configure_external_interrupt(&gpio, &intNo, SL_GPIO_INTERRUPT_FALLING_EDGE, ShortCircuitGpioIsr, nullptr);
    if (status != SL_STATUS_OK)
    {
        ChipLogError(AppServer, "ShortCircuit: PB%u.%u IRQ setup failed (%u)", static_cast<unsigned>(SHORT_CIRCUIT_GPIO_PORT),
                     static_cast<unsigned>(SHORT_CIRCUIT_GPIO_PIN), static_cast<unsigned>(status));
        return;
    }

    sIntNo = intNo;
    EnableInterrupt();
    ChipLogProgress(AppServer, "ShortCircuit: PB%u.%u falling-edge detect armed (int %ld)",
                    static_cast<unsigned>(SHORT_CIRCUIT_GPIO_PORT), static_cast<unsigned>(SHORT_CIRCUIT_GPIO_PIN),
                    static_cast<long>(sIntNo));
}
