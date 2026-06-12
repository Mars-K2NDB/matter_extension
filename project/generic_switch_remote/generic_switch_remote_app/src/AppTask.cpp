#include "AppTask.h"
#include "AppConfig.h"
#include "AppEvent.h"

#include <app/clusters/switch-server/switch-server.h>
#include <app/server/Server.h>
#if CHIP_CONFIG_ENABLE_ICD_SERVER
#include <app/icd/server/ICDNotifier.h>
#endif
#include <lib/support/CodeUtils.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/PlatformManager.h>
#include <platform/silabs/platformAbstraction/SilabsPlatform.h>

#include "FreeRTOS.h"
#include "sl_clock_manager.h"
#include "sl_gpio.h"
#include "sl_simple_button.h"
#include "sl_simple_button_btn0_config.h"
#include "sl_simple_button_config.h"
#include "sl_simple_button_instances.h"
#include "timers.h"

using namespace chip;
using namespace chip::DeviceLayer;
using namespace chip::DeviceLayer::Silabs;

namespace {

constexpr EndpointId kEndpoint1Id = 1;
constexpr EndpointId kEndpoint2Id = 2;
constexpr EndpointId kEndpoint3Id = 3;
constexpr EndpointId kEndpoint4Id = 4;
constexpr uint8_t kSwitchIdlePosition    = 0;
constexpr uint8_t kSwitchPressedPosition = 1;
constexpr size_t kSwitchEndpointCount    = 4;
constexpr uint32_t kLongPressMs          = 5000;
constexpr uint32_t kMultiPressWindowMs   = 500;

// autogen 顺序: btn0, c0, c1, c2, c3
constexpr uint8_t APP_RESET_BUTTON    = 0;
constexpr uint8_t APP_SWITCH_BUTTON_0 = 1;
constexpr uint8_t APP_SWITCH_BUTTON_1 = 2;
constexpr uint8_t APP_SWITCH_BUTTON_2 = 3;
constexpr uint8_t APP_SWITCH_BUTTON_3 = 4;

constexpr uint8_t KEY_LED = 1;

enum class SwitchEmitEvent : uint8_t
{
    kInitialPress = 0,
    kLongPress,
    kShortRelease,
    kLongRelease,
    kMultiPressOngoing,
    kMultiPressComplete,
};

struct SwitchEndpointState
{
    bool isPressed         = false;
    bool longPressEmitted  = false;
    bool multipressOngoing = false;
    uint8_t multiPressCount = 0;
};

struct SwitchEventWork
{
    EndpointId endpointId;
    SwitchEmitEvent event;
    uint8_t position;
    uint8_t count;
};

SwitchEndpointState sSwitchState[kSwitchEndpointCount];
uint8_t sKeyDownMask = 0;

uint8_t sPrevButtonState[SL_SIMPLE_BUTTON_COUNT] = {};

osTimerId_t sLongPressTimer[kSwitchEndpointCount]  = {};
osTimerId_t sMultiPressTimer[kSwitchEndpointCount] = {};

LEDWidget sKeyLED;

bool IsButtonPressed(uint8_t btnAction)
{
    return btnAction == static_cast<uint8_t>(SilabsPlatform::ButtonAction::ButtonPressed);
}

EndpointId EndpointForButton(uint8_t button)
{
    switch (button)
    {
    case APP_SWITCH_BUTTON_0:
        return kEndpoint1Id;
    case APP_SWITCH_BUTTON_1:
        return kEndpoint2Id;
    case APP_SWITCH_BUTTON_2:
        return kEndpoint3Id;
    case APP_SWITCH_BUTTON_3:
        return kEndpoint4Id;
    default:
        return kEndpoint1Id;
    }
}

uint8_t KeyIndexForButton(uint8_t button)
{
    switch (button)
    {
    case APP_SWITCH_BUTTON_0:
        return 0;
    case APP_SWITCH_BUTTON_1:
        return 1;
    case APP_SWITCH_BUTTON_2:
        return 2;
    case APP_SWITCH_BUTTON_3:
        return 3;
    default:
        return 0;
    }
}

void UpdateKeyLed()
{
    sKeyLED.Set(sKeyDownMask != 0);
}

void KeyIndicatorOnPress(uint8_t keyIndex)
{
    if (keyIndex >= 4)
    {
        return;
    }
    sKeyDownMask |= static_cast<uint8_t>(1u << keyIndex);
    UpdateKeyLed();
}

void KeyIndicatorOnRelease(uint8_t keyIndex)
{
    if (keyIndex >= 4)
    {
        return;
    }
    sKeyDownMask &= static_cast<uint8_t>(~(1u << keyIndex));
    UpdateKeyLed();
}

uint8_t EndpointIndex(EndpointId endpointId)
{
    if (endpointId < kEndpoint1Id || endpointId > kEndpoint4Id)
    {
        return 0;
    }
    return static_cast<uint8_t>(endpointId - kEndpoint1Id);
}

const char * SwitchEventName(SwitchEmitEvent event)
{
    switch (event)
    {
    case SwitchEmitEvent::kInitialPress:
        return "InitialPress";
    case SwitchEmitEvent::kLongPress:
        return "LongPress";
    case SwitchEmitEvent::kShortRelease:
        return "ShortRelease";
    case SwitchEmitEvent::kLongRelease:
        return "LongRelease";
    case SwitchEmitEvent::kMultiPressOngoing:
        return "MultiPressOngoing";
    case SwitchEmitEvent::kMultiPressComplete:
        return "MultiPressComplete";
    default:
        return "Unknown";
    }
}

void EmitSwitchEvent(EndpointId endpointId, SwitchEmitEvent event, uint8_t position, uint8_t count = 0)
{
    auto * work = Platform::New<SwitchEventWork>();
    if (work == nullptr)
    {
        return;
    }
    work->endpointId = endpointId;
    work->event      = event;
    work->position   = position;
    work->count      = count;

    TEMPORARY_RETURN_IGNORED PlatformMgr().ScheduleWork(
        [](intptr_t context) {
            auto * w        = reinterpret_cast<SwitchEventWork *>(context);
            auto * cluster  = app::Clusters::Switch::FindClusterOnEndpoint(w->endpointId);
            if (cluster == nullptr)
            {
                SILABS_LOG("Switch cluster missing on endpoint %u", w->endpointId);
                Platform::Delete(w);
                return;
            }

            std::optional<EventNumber> eventNumber;
            switch (w->event)
            {
            case SwitchEmitEvent::kInitialPress:
                if (cluster->SetCurrentPosition(kSwitchPressedPosition) == CHIP_NO_ERROR)
                {
                    eventNumber = cluster->OnInitialPress(w->position);
                }
                break;
            case SwitchEmitEvent::kLongPress:
                eventNumber = cluster->OnLongPress(w->position);
                break;
            case SwitchEmitEvent::kShortRelease:
                if (cluster->SetCurrentPosition(kSwitchIdlePosition) == CHIP_NO_ERROR)
                {
                    eventNumber = cluster->OnShortRelease(w->position);
                }
                break;
            case SwitchEmitEvent::kLongRelease:
                if (cluster->SetCurrentPosition(kSwitchIdlePosition) == CHIP_NO_ERROR)
                {
                    eventNumber = cluster->OnLongRelease(w->position);
                }
                break;
            case SwitchEmitEvent::kMultiPressOngoing:
                eventNumber = cluster->OnMultiPressOngoing(w->position, w->count);
                break;
            case SwitchEmitEvent::kMultiPressComplete:
                eventNumber = cluster->OnMultiPressComplete(w->position, w->count);
                break;
            default:
                break;
            }

            if (eventNumber.has_value())
            {
                SILABS_LOG("Matter ep%u %s pos=%u cnt=%u", w->endpointId, SwitchEventName(w->event), w->position, w->count);
            }
            else
            {
                SILABS_LOG("Matter ep%u %s failed", w->endpointId, SwitchEventName(w->event));
            }

            Platform::Delete(w);
        },
        reinterpret_cast<intptr_t>(work));
}

void HandleLongPressTimeout(uint8_t epIndex)
{
    if (epIndex >= kSwitchEndpointCount)
    {
        return;
    }

    SwitchEndpointState & state = sSwitchState[epIndex];
    if (!state.isPressed || state.longPressEmitted || state.multipressOngoing)
    {
        return;
    }

    state.longPressEmitted = true;
    EmitSwitchEvent(static_cast<EndpointId>(kEndpoint1Id + epIndex), SwitchEmitEvent::kLongPress, kSwitchPressedPosition);
}

void HandleMultiPressTimeout(uint8_t epIndex)
{
    if (epIndex >= kSwitchEndpointCount)
    {
        return;
    }

    SwitchEndpointState & state = sSwitchState[epIndex];
    state.multipressOngoing   = false;

    const uint8_t pressCount = (state.multiPressCount >= 1) ? state.multiPressCount : 1;
    EmitSwitchEvent(static_cast<EndpointId>(kEndpoint1Id + epIndex), SwitchEmitEvent::kMultiPressComplete,
                    kSwitchPressedPosition, pressCount);
    state.multiPressCount   = 0;
}

void LongPressTimerEventHandler(AppEvent * aEvent)
{
    const uint8_t epIndex = static_cast<uint8_t>(reinterpret_cast<intptr_t>(aEvent->TimerEvent.Context));
    HandleLongPressTimeout(epIndex);
}

void MultiPressTimerEventHandler(AppEvent * aEvent)
{
    const uint8_t epIndex = static_cast<uint8_t>(reinterpret_cast<intptr_t>(aEvent->TimerEvent.Context));
    HandleMultiPressTimeout(epIndex);
}

void LongPressTimerCallback(void * argument)
{
    AppEvent timerEvent;
    timerEvent.Type               = AppEvent::kEventType_Timer;
    timerEvent.TimerEvent.Context = argument;
    timerEvent.Handler            = LongPressTimerEventHandler;
    AppTask::GetAppTask().PostEvent(&timerEvent);
}

void MultiPressTimerCallback(void * argument)
{
    AppEvent timerEvent;
    timerEvent.Type               = AppEvent::kEventType_Timer;
    timerEvent.TimerEvent.Context = argument;
    timerEvent.Handler            = MultiPressTimerEventHandler;
    AppTask::GetAppTask().PostEvent(&timerEvent);
}

void HandleSwitchPress(EndpointId endpointId)
{
    const uint8_t epIndex     = EndpointIndex(endpointId);
    SwitchEndpointState & state = sSwitchState[epIndex];

    state.isPressed = true;

    if (state.multipressOngoing)
    {
        if (sMultiPressTimer[epIndex] != nullptr)
        {
            (void) osTimerStop(sMultiPressTimer[epIndex]);
        }
        state.multiPressCount++;
        EmitSwitchEvent(endpointId, SwitchEmitEvent::kInitialPress, kSwitchPressedPosition);
        EmitSwitchEvent(endpointId, SwitchEmitEvent::kMultiPressOngoing, kSwitchPressedPosition, state.multiPressCount);
        return;
    }

    if (sLongPressTimer[epIndex] != nullptr)
    {
        (void) osTimerStart(sLongPressTimer[epIndex], pdMS_TO_TICKS(kLongPressMs));
    }
    EmitSwitchEvent(endpointId, SwitchEmitEvent::kInitialPress, kSwitchPressedPosition);
}

void HandleSwitchRelease(EndpointId endpointId)
{
    const uint8_t epIndex     = EndpointIndex(endpointId);
    SwitchEndpointState & state = sSwitchState[epIndex];

    state.isPressed = false;
    if (sLongPressTimer[epIndex] != nullptr)
    {
        (void) osTimerStop(sLongPressTimer[epIndex]);
    }

    if (state.longPressEmitted)
    {
        state.longPressEmitted = false;
        if (sMultiPressTimer[epIndex] != nullptr)
        {
            (void) osTimerStop(sMultiPressTimer[epIndex]);
        }
        state.multipressOngoing = false;
        state.multiPressCount   = 0;
        EmitSwitchEvent(endpointId, SwitchEmitEvent::kLongRelease, kSwitchPressedPosition);
        return;
    }

    EmitSwitchEvent(endpointId, SwitchEmitEvent::kShortRelease, kSwitchPressedPosition);

    if (!state.multipressOngoing)
    {
        state.multiPressCount = 1;
    }
    state.multipressOngoing = true;
    if (sMultiPressTimer[epIndex] != nullptr)
    {
        (void) osTimerStart(sMultiPressTimer[epIndex], pdMS_TO_TICKS(kMultiPressWindowMs));
    }
}

void InitSwitchTimers()
{
    for (uint8_t i = 0; i < kSwitchEndpointCount; i++)
    {
        if (sLongPressTimer[i] == nullptr)
        {
            sLongPressTimer[i] =
                osTimerNew(LongPressTimerCallback, osTimerOnce, reinterpret_cast<void *>(static_cast<intptr_t>(i)), nullptr);
        }
        if (sMultiPressTimer[i] == nullptr)
        {
            sMultiPressTimer[i] =
                osTimerNew(MultiPressTimerCallback, osTimerOnce, reinterpret_cast<void *>(static_cast<intptr_t>(i)), nullptr);
        }
    }
}

void SetButtonGpioMode(const sl_button_t * handle, sl_gpio_mode_t mode, bool pullUp)
{
    if (handle == nullptr || handle->context == nullptr)
    {
        return;
    }

    auto * ctx = static_cast<sl_simple_button_context_t *>(handle->context);
    sl_gpio_t gpio = {
        .port = ctx->port,
        .pin  = ctx->pin,
    };
    sl_gpio_set_pin_mode(&gpio, mode, pullUp ? 1U : 0U);
}

// 5 路按键若都用 GPIO 外部中断，PB02 与 PC02 等会争用同一 int 线；统一轮询并分引脚配置
void InitAllButtons()
{
    sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_GPIO);

    const sl_button_t * btn0 = SL_SIMPLE_BUTTON_INSTANCE(APP_RESET_BUTTON);
    SetButtonGpioMode(btn0, SL_GPIO_MODE_INPUT, false);
    if (btn0 != nullptr)
    {
        sl_button_init(btn0);
    }

    for (uint8_t i = APP_SWITCH_BUTTON_0; i <= APP_SWITCH_BUTTON_3; i++)
    {
        const sl_button_t * handle = SL_SIMPLE_BUTTON_INSTANCE(i);
        SetButtonGpioMode(handle, SL_GPIO_MODE_INPUT_PULL, true);
        if (handle != nullptr)
        {
            sl_button_init(handle);
        }
    }

    for (uint8_t i = 0; i < SL_SIMPLE_BUTTON_COUNT; i++)
    {
        const sl_button_t * handle = SL_SIMPLE_BUTTON_INSTANCE(i);
        if (handle == nullptr)
        {
            continue;
        }
        sl_button_poll_step(handle);
        sPrevButtonState[i] = sl_button_get_state(handle);
    }
}

void PollAllButtons()
{
    for (uint8_t i = 0; i < SL_SIMPLE_BUTTON_COUNT; i++)
    {
        const sl_button_t * handle = SL_SIMPLE_BUTTON_INSTANCE(i);
        if (handle == nullptr)
        {
            continue;
        }

        sl_button_poll_step(handle);
        const uint8_t state = sl_button_get_state(handle);
        if (state == sPrevButtonState[i])
        {
            continue;
        }

        sPrevButtonState[i] = state;
        AppTask::ButtonEventHandler(i, state);
    }
}

} // namespace

AppTask AppTask::sAppTask;

void AppTask::EarlyHardwareInit()
{
    chip::DeviceLayer::Silabs::GetPlatform().SetButtonsCb(AppTask::ButtonEventHandler);
    InitAllButtons();
    LEDWidget::InitGpio();
    sKeyLED.Init(KEY_LED);
    sKeyLED.Set(false);
    InitSwitchTimers();
}

CHIP_ERROR AppTask::AppInit()
{
    // 轮询检测到边沿后在任务上下文 PostEvent
    chip::DeviceLayer::Silabs::GetPlatform().SetButtonsCb(AppTask::ButtonEventHandler);
    return CHIP_NO_ERROR;
}

CHIP_ERROR AppTask::StartAppTask()
{
    return BaseApplication::StartAppTask(AppTaskMain);
}

void AppTask::AppTaskMain(void * pvParameter)
{
    AppEvent event;
    osMessageQueueId_t sAppEventQueue = *(static_cast<osMessageQueueId_t *>(pvParameter));

    CHIP_ERROR err = sAppTask.Init();
    if (err != CHIP_NO_ERROR)
    {
        SILABS_LOG("AppTask.Init() failed");
        appError(err);
    }

#if !(defined(CHIP_CONFIG_ENABLE_ICD_SERVER) && CHIP_CONFIG_ENABLE_ICD_SERVER)
    sAppTask.StartStatusLEDTimer();
#endif

    SILABS_LOG("Generic Switch App Task started");

#if CHIP_CONFIG_ENABLE_ICD_SERVER
    constexpr TickType_t kAppTaskIdleMs = 50;
#else
    constexpr TickType_t kAppTaskIdleMs = 10;
#endif

    while (true)
    {
        PollAllButtons();

        osStatus_t eventReceived = osMessageQueueGet(sAppEventQueue, &event, NULL, pdMS_TO_TICKS(kAppTaskIdleMs));
        while (eventReceived == osOK)
        {
            sAppTask.DispatchEvent(&event);
            eventReceived = osMessageQueueGet(sAppEventQueue, &event, NULL, 0);
        }
    }
}

void AppTask::ButtonEventHandler(uint8_t button, uint8_t btnAction)
{
    AppEvent button_event           = {};
    button_event.Type               = AppEvent::kEventType_Button;
    button_event.ButtonEvent.Action = btnAction;

    // 与 onoff_plug 相同：BTN0 仅走 BaseApplication::ButtonHandler，不控制 LED1
    if (button == APP_RESET_BUTTON)
    {
        SILABS_LOG("BTN0 %s", IsButtonPressed(btnAction) ? "pressed" : "released");
        button_event.Handler = BaseApplication::ButtonHandler;
        sAppTask.PostEvent(&button_event);
        return;
    }

    if (button < APP_SWITCH_BUTTON_0 || button > APP_SWITCH_BUTTON_3)
    {
        return;
    }

    SILABS_LOG("Switch key %u %s", button, IsButtonPressed(btnAction) ? "pressed" : "released");

#if CHIP_CONFIG_ENABLE_ICD_SERVER
    if (IsButtonPressed(btnAction))
    {
        TEMPORARY_RETURN_IGNORED PlatformMgr().ScheduleWork(
            [](intptr_t) { chip::app::ICDNotifier::GetInstance().NotifyNetworkActivityNotification(); });
    }
#endif

    const uint8_t keyIndex = KeyIndexForButton(button);
    if (IsButtonPressed(btnAction))
    {
        KeyIndicatorOnPress(keyIndex);
    }
    else
    {
        KeyIndicatorOnRelease(keyIndex);
    }

    button_event.Type                    = AppEvent::kEventType_Switch;
    button_event.SwitchEvent.ButtonIndex = button;
    button_event.SwitchEvent.Action      = btnAction;
    button_event.SwitchEvent.EndpointId  = EndpointForButton(button);
    button_event.Handler                 = SwitchActionEventHandler;
    sAppTask.PostEvent(&button_event);
}

void AppTask::SwitchActionEventHandler(AppEvent * aEvent)
{
    VerifyOrReturn(aEvent->Type == AppEvent::kEventType_Switch);

    const EndpointId endpointId = static_cast<EndpointId>(aEvent->SwitchEvent.EndpointId);

    if (IsButtonPressed(aEvent->SwitchEvent.Action))
    {
        HandleSwitchPress(endpointId);
    }
    else
    {
        HandleSwitchRelease(endpointId);
    }
}
