/*
 * IADC scan entry 0 on PA00 (Pin Tool: IADC0 SCAN0POS), see voltage_adc_config.h.
 */

#include "VoltageAdcDriver.h"

#include "overcurrent_protect_config.h"
#include "OvercurrentProtector.h"
#include "pin_config.h"
#include "voltage_adc_config.h"

#include <em_gpio.h>
#include <em_iadc.h>

#include "sl_clock_manager.h"
#include "sl_gpio.h"
#include "sl_status.h"

#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceLayer.h>
#include <silabs_utils.h>
#include <system/SystemClock.h>

bool VoltageAdcDriver::sInitialized    = false;
bool VoltageAdcDriver::sPeriodicActive = false;

namespace {

constexpr uint8_t kScanEntryId = VOLTAGE_ADC_SCAN_ENTRY_ID;

constexpr uint32_t kClkSrcAdcHz = 2000000U;
constexpr uint32_t kClkAdcHz    = 1000000U;

uint32_t sRefMillivolts = 1210U;

GPIO_Port_TypeDef PortFromSlPort(uint8_t port)
{
    switch (port)
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

sl_status_t AllocateAnalogBus(GPIO_Port_TypeDef port, unsigned int pin)
{
#if (GPIO_PA_COUNT > 0)
    if (port == gpioPortA)
    {
        if ((pin % 2U) == 0U)
        {
            if ((GPIO->ABUSALLOC & _GPIO_ABUSALLOC_AEVEN0_MASK) == GPIO_ABUSALLOC_AEVEN0_TRISTATE)
            {
                GPIO->ABUSALLOC |= GPIO_ABUSALLOC_AEVEN0_ADC0;
            }
            else if ((GPIO->ABUSALLOC & _GPIO_ABUSALLOC_AEVEN1_MASK) == GPIO_ABUSALLOC_AEVEN1_TRISTATE)
            {
                GPIO->ABUSALLOC |= GPIO_ABUSALLOC_AEVEN1_ADC0;
            }
            else
            {
                return SL_STATUS_ALLOCATION_FAILED;
            }
        }
        else
        {
            if ((GPIO->ABUSALLOC & _GPIO_ABUSALLOC_AODD0_MASK) == GPIO_ABUSALLOC_AODD0_TRISTATE)
            {
                GPIO->ABUSALLOC |= GPIO_ABUSALLOC_AODD0_ADC0;
            }
            else if ((GPIO->ABUSALLOC & _GPIO_ABUSALLOC_AODD1_MASK) == GPIO_ABUSALLOC_AODD1_TRISTATE)
            {
                GPIO->ABUSALLOC |= GPIO_ABUSALLOC_AODD1_ADC0;
            }
            else
            {
                return SL_STATUS_ALLOCATION_FAILED;
            }
        }
        return SL_STATUS_OK;
    }
#endif
#if (GPIO_PB_COUNT > 0)
    if (port == gpioPortB)
    {
        if ((pin % 2U) == 0U)
        {
            if ((GPIO->BBUSALLOC & _GPIO_BBUSALLOC_BEVEN0_MASK) == GPIO_BBUSALLOC_BEVEN0_TRISTATE)
            {
                GPIO->BBUSALLOC |= GPIO_BBUSALLOC_BEVEN0_ADC0;
            }
            else if ((GPIO->BBUSALLOC & _GPIO_BBUSALLOC_BEVEN1_MASK) == GPIO_BBUSALLOC_BEVEN1_TRISTATE)
            {
                GPIO->BBUSALLOC |= GPIO_BBUSALLOC_BEVEN1_ADC0;
            }
            else
            {
                return SL_STATUS_ALLOCATION_FAILED;
            }
        }
        else
        {
            if ((GPIO->BBUSALLOC & _GPIO_BBUSALLOC_BODD0_MASK) == GPIO_BBUSALLOC_BODD0_TRISTATE)
            {
                GPIO->BBUSALLOC |= GPIO_BBUSALLOC_BODD0_ADC0;
            }
            else if ((GPIO->BBUSALLOC & _GPIO_BBUSALLOC_BODD1_MASK) == GPIO_BBUSALLOC_BODD1_TRISTATE)
            {
                GPIO->BBUSALLOC |= GPIO_BBUSALLOC_BODD1_ADC0;
            }
            else
            {
                return SL_STATUS_ALLOCATION_FAILED;
            }
        }
        return SL_STATUS_OK;
    }
#endif
#if (GPIO_PC_COUNT > 0 || GPIO_PD_COUNT > 0)
    if (port == gpioPortC || port == gpioPortD)
    {
        if ((pin % 2U) == 0U)
        {
            if ((GPIO->CDBUSALLOC & _GPIO_CDBUSALLOC_CDEVEN0_MASK) == GPIO_CDBUSALLOC_CDEVEN0_TRISTATE)
            {
                GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDEVEN0_ADC0;
            }
            else if ((GPIO->CDBUSALLOC & _GPIO_CDBUSALLOC_CDEVEN1_MASK) == GPIO_CDBUSALLOC_CDEVEN1_TRISTATE)
            {
                GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDEVEN1_ADC0;
            }
            else
            {
                return SL_STATUS_ALLOCATION_FAILED;
            }
        }
        else
        {
            if ((GPIO->CDBUSALLOC & _GPIO_CDBUSALLOC_CDODD0_MASK) == GPIO_CDBUSALLOC_CDODD0_TRISTATE)
            {
                GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDODD0_ADC0;
            }
            else if ((GPIO->CDBUSALLOC & _GPIO_CDBUSALLOC_CDODD1_MASK) == GPIO_CDBUSALLOC_CDODD1_TRISTATE)
            {
                GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDODD1_ADC0;
            }
            else
            {
                return SL_STATUS_ALLOCATION_FAILED;
            }
        }
        return SL_STATUS_OK;
    }
#endif
    return SL_STATUS_INVALID_PARAMETER;
}

void FlushScanFifo()
{
    while ((IADC0->STATUS & IADC_STATUS_SCANFIFODV) != 0)
    {
        (void) IADC_pullScanFifoData(IADC0);
    }
}

bool WaitScanDataValid()
{
    constexpr uint32_t kMaxPoll = 500000U;
    for (uint32_t i = 0; i < kMaxPoll; ++i)
    {
        if ((IADC0->STATUS & IADC_STATUS_SCANFIFODV) != 0)
        {
            return true;
        }
    }
    return false;
}

} // namespace

void VoltageAdcDriver::Init()
{
    if (sInitialized)
    {
        return;
    }

#if defined(IADC0_SCAN0POS_PORT) && defined(IADC0_SCAN0POS_PIN)
    static_assert(VOLTAGE_ADC_GPIO_PORT == IADC0_SCAN0POS_PORT, "VOLTAGE_ADC_GPIO_PORT must match IADC0_SCAN0POS");
    static_assert(VOLTAGE_ADC_GPIO_PIN == IADC0_SCAN0POS_PIN, "VOLTAGE_ADC_GPIO_PIN must match IADC0_SCAN0POS");
#endif

    const GPIO_Port_TypeDef gpioPort = PortFromSlPort(VOLTAGE_ADC_GPIO_PORT);
    const unsigned int gpioPin     = VOLTAGE_ADC_GPIO_PIN;

    if (sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_GPIO) != SL_STATUS_OK)
    {
        ChipLogError(AppServer, "VoltageAdc: GPIO clock enable failed");
        return;
    }

    const sl_status_t busStatus = AllocateAnalogBus(gpioPort, gpioPin);
    if (busStatus != SL_STATUS_OK)
    {
        ChipLogError(AppServer, "VoltageAdc: analog bus alloc failed (%u)", static_cast<unsigned>(busStatus));
        return;
    }

    GPIO_PinModeSet(gpioPort, gpioPin, gpioModeDisabled, 0);

    if (sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_IADC0) != SL_STATUS_OK)
    {
        ChipLogError(AppServer, "VoltageAdc: IADC0 clock enable failed");
        return;
    }

    IADC_Init_t init             = IADC_INIT_DEFAULT;
    IADC_AllConfigs_t allConfigs = IADC_ALLCONFIGS_DEFAULT;
    init.srcClkPrescale          = IADC_calcSrcClkPrescale(IADC0, kClkSrcAdcHz, 0);
    init.warmup                  = iadcWarmupKeepWarm;
    allConfigs.configs[0].adcClkPrescale =
        IADC_calcAdcClkPrescale(IADC0, kClkAdcHz, 0, iadcCfgModeNormal, init.srcClkPrescale);
#if VOLTAGE_ADC_REFERENCE_VDDX
    allConfigs.configs[0].reference = iadcCfgReferenceVddx;
    allConfigs.configs[0].vRef      = VOLTAGE_ADC_VREF_MV;
#else
    allConfigs.configs[0].reference = iadcCfgReferenceInt1V2;
    allConfigs.configs[0].vRef      = 1210;
#endif
    sRefMillivolts = allConfigs.configs[0].vRef;
    IADC_init(IADC0, &init, &allConfigs);

    IADC_ScanTable_t scanTable = IADC_SCANTABLE_DEFAULT;
    scanTable.entries[kScanEntryId].negInput     = iadcNegInputGnd;
    scanTable.entries[kScanEntryId].posInput     = IADC_portPinToPosInput(gpioPort, static_cast<uint8_t>(gpioPin));
    scanTable.entries[kScanEntryId].configId     = 0;
    scanTable.entries[kScanEntryId].compare      = false;
    scanTable.entries[kScanEntryId].includeInScan = true;

    IADC_InitScan_t initScan = IADC_INITSCAN_DEFAULT;
    IADC_initScan(IADC0, &initScan, &scanTable);

    FlushScanFifo();
    sInitialized = true;
    (void) ReadMillivolts();
    SILABS_LOG("VoltageAdc scan entry %u OK (port %u pin %u, SCAN0POS)", kScanEntryId, VOLTAGE_ADC_GPIO_PORT,
               VOLTAGE_ADC_GPIO_PIN);
}

bool VoltageAdcDriver::TryReadMillivolts(uint32_t & millivoltsOut)
{
    if (!sInitialized)
    {
        return false;
    }

    FlushScanFifo();
    IADC_command(IADC0, iadcCmdStartScan);

    if (!WaitScanDataValid())
    {
        ChipLogError(AppServer, "VoltageAdc: scan conversion timeout");
        return false;
    }

    const IADC_Result_t result = IADC_pullScanFifoResult(IADC0);
    const uint32_t raw12       = static_cast<uint32_t>(result.data) & 0x0FFFU;
    const uint32_t adcMv       = (raw12 * sRefMillivolts) / 4095U;

    millivoltsOut = (adcMv * VOLTAGE_ADC_INPUT_SCALE_NUM) / VOLTAGE_ADC_INPUT_SCALE_DEN;
    return true;
}

uint32_t VoltageAdcDriver::ReadMillivolts()
{
    uint32_t mv = 0;
    if (!TryReadMillivolts(mv))
    {
        return 0;
    }
    return mv;
}

void VoltageAdcDriver::OnPeriodicSampleTimer(chip::System::Layer * layer, void * appState)
{
    (void) layer;
    (void) appState;

    if (!sInitialized || !sPeriodicActive)
    {
        return;
    }

    uint32_t mv = 0;
    const bool valid = TryReadMillivolts(mv);
    OvercurrentProtector::OnAdcSample(mv, valid);

    if (sPeriodicActive)
    {
        (void) chip::DeviceLayer::SystemLayer().StartTimer(chip::System::Clock::Milliseconds32(OVERCURRENT_SAMPLE_MS),
                                                           OnPeriodicSampleTimer, nullptr);
    }
}

void VoltageAdcDriver::StartPeriodicSampling()
{
    if (!sInitialized || sPeriodicActive)
    {
        return;
    }

    // SystemLayer::StartTimer requires the CHIP stack lock; InitLightImpl runs from AppInit without it.
    chip::DeviceLayer::PlatformMgr().LockChipStack();
    sPeriodicActive = true;
    (void) chip::DeviceLayer::SystemLayer().StartTimer(chip::System::Clock::Milliseconds32(OVERCURRENT_SAMPLE_MS),
                                                       OnPeriodicSampleTimer, nullptr);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
}
void VoltageAdcDriver::StopPeriodicSampling()
{
    if (!sPeriodicActive)
    {
        return;
    }

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    sPeriodicActive = false;
    chip::DeviceLayer::SystemLayer().CancelTimer(OnPeriodicSampleTimer, nullptr);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
}
