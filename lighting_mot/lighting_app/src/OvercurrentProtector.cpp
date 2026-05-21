/*
 * Overcurrent protection: ADC average -> PWM off, Matter OnOff off, latch until cleared.
 */

#include "OvercurrentProtector.h"

#include "AppConfig.h"
#include "CtDualPwmDriver.h"
#include "overcurrent_protect_config.h"
#include "VoltageAdcDriver.h"

#include <app/clusters/on-off-server/on-off-server.h>
#include <app/reporting/reporting.h>
#include <clusters/OnOff/AttributeIds.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceLayer.h>

#include <em_gpio.h>
#include <sl_gpio.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::DeviceLayer;

bool OvercurrentProtector::sFaultActive    = false;
uint32_t OvercurrentProtector::sAvgMillivolts = 0;
uint32_t OvercurrentProtector::sRecoveryMs   = 0;
uint32_t OvercurrentProtector::sSampleRing[OVERCURRENT_AVG_SAMPLES] = {};
uint8_t OvercurrentProtector::sSampleCount = 0;
uint8_t OvercurrentProtector::sSampleIndex = 0;

namespace {

#if defined(OVERCURRENT_GPIO_OK_PORT) && defined(OVERCURRENT_GPIO_OK_PIN)
GPIO_Port_TypeDef GpioOkPort()
{
    switch (OVERCURRENT_GPIO_OK_PORT)
    {
    case SL_GPIO_PORT_A:
        return gpioPortA;
    case SL_GPIO_PORT_B:
        return gpioPortB;
    case SL_GPIO_PORT_C:
        return gpioPortC;
    case SL_GPIO_PORT_D:
        return gpioPortD;
    default:
        return gpioPortA;
    }
}
#endif

void SetMatterOnOff(bool on)
{
    PlatformMgr().LockChipStack();
    OnOffServer::Instance().setOnOffValue(LIGHT_ENDPOINT, on ? 1U : 0U, false);
    MatterReportingAttributeChangeCallback(
        ConcreteAttributePath(LIGHT_ENDPOINT, OnOff::Id, OnOff::Attributes::OnOff::Id));
    PlatformMgr().UnlockChipStack();
}

} // namespace

void OvercurrentProtector::Init()
{
    sFaultActive    = false;
    sAvgMillivolts  = 0;
    sRecoveryMs     = 0;
    sSampleCount    = 0;
    sSampleIndex    = 0;
}

void OvercurrentProtector::UpdateAverage(uint32_t millivolts)
{
    if (sSampleCount < OVERCURRENT_AVG_SAMPLES)
    {
        sSampleRing[sSampleCount] = millivolts;
        ++sSampleCount;
    }
    else
    {
        sSampleRing[sSampleIndex] = millivolts;
        sSampleIndex              = static_cast<uint8_t>((sSampleIndex + 1U) % OVERCURRENT_AVG_SAMPLES);
    }

    uint32_t sum = 0;
    for (uint8_t i = 0; i < sSampleCount; ++i)
    {
        sum += sSampleRing[i];
    }
    sAvgMillivolts = sum / sSampleCount;
}

bool OvercurrentProtector::IsGpioOk()
{
#if defined(OVERCURRENT_GPIO_OK_PORT) && defined(OVERCURRENT_GPIO_OK_PIN)
    const unsigned int level = GPIO_PinInGet(GpioOkPort(), OVERCURRENT_GPIO_OK_PIN);
    return level == OVERCURRENT_GPIO_OK_LEVEL;
#else
    return VoltageAdcDriver::IsInitialized();
#endif
}

void OvercurrentProtector::Trip()
{
    if (sFaultActive)
    {
        return;
    }

    sFaultActive  = true;
    sRecoveryMs   = 0;

    CtDualPwmDriver::ForceOffForFault();

    SetMatterOnOff(false);

    ChipLogError(AppServer, "Overcurrent TRIP: avg=%lu mV > %u mV", static_cast<unsigned long>(sAvgMillivolts),
                 OVERCURRENT_TRIP_MV);
}

void OvercurrentProtector::Recover()
{
    if (!sFaultActive)
    {
        return;
    }

    sFaultActive = false;
    sRecoveryMs  = 0;

    CtDualPwmDriver::Init();
    SetMatterOnOff(true);
    CtDualPwmDriver::SyncFromMatterEndpoint(LIGHT_ENDPOINT);

    ChipLogProgress(AppServer, "Overcurrent RECOVER: avg=%lu mV <= %u mV for %u ms", static_cast<unsigned long>(sAvgMillivolts),
                    OVERCURRENT_CLEAR_MV, OVERCURRENT_RECOVERY_MS);
}

void OvercurrentProtector::OnAdcSample(uint32_t millivolts, bool sampleValid)
{
    if (!sampleValid)
    {
        sRecoveryMs = 0;
        return;
    }

    UpdateAverage(millivolts);

    if (sFaultActive)
    {
        if (sAvgMillivolts <= OVERCURRENT_CLEAR_MV && IsGpioOk())
        {
            sRecoveryMs += OVERCURRENT_SAMPLE_MS;
            if (sRecoveryMs >= OVERCURRENT_RECOVERY_MS)
            {
                Recover();
            }
        }
        else
        {
            sRecoveryMs = 0;
        }
        return;
    }

    if (sAvgMillivolts > OVERCURRENT_TRIP_MV)
    {
        Trip();
    }
}
