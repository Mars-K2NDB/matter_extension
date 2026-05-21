/*
 * Short-circuit detection on PB00 (falling edge). Shares OvercurrentProtector fault latch.
 */

#pragma once

#include <cstdint>

class ShortCircuitProtector
{
public:
    static void Init();
    static bool IsPinOk();
    static void OnRecover();
    static void HandleInterrupt();

private:
    static void EnableInterrupt();
    static void DisableInterrupt();

    static int32_t sIntNo;
    static bool sInterruptEnabled;
};

