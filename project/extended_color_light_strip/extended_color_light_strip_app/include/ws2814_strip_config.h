/*
 * WS2814 SPI 灯带参数（对齐 Telink RDWD5137A board.overlay + WS2814 V1.4 时序）。
 *
 * 协议要点（规格书 WS2814，参见 LCSC 数据手册）：
 * - 单线 NZR，每颗 32bit，发送顺序 WRGB、高位先发。
 * - 0 码：T0H 220~380ns，T0L 580ns~1us；1 码：T1H 580ns~1us，T1L 580ns~1us。
 * - 位周期 TDATA >= 1.25us；帧间 RES：低电平 >280us。
 * - SPI 模拟：Mode1、MSB，逻辑 0/1 用 0xC0/0xF0（每逻辑位 8 个 SPI 位）。
 *
 * EFR32MG24 EUSART1 时钟来自 EM01GRPACLK（本工程 39MHz HFXO）。同步 SPI 实际频率为
 * ref / floor(ref/requested)，须使 requested 能整除 ref，否则时序会整体偏快（例如请求
 * 6.67MHz 时硬件实为 7.8MHz，T1H/T1L 低于规格下限）。
 *
 * 线序：规格书为 WRGB；与 Telink color-mapping 一致。
 */
#pragma once

#ifndef WS2814_LED_COUNT
#define WS2814_LED_COUNT 69
#endif

/** EUSART 同步 SPI 参考时钟（与 SL_CLOCK_MANAGER_HFXO_FREQ / HFXO_FREQ 一致） */
#ifndef WS2814_EUSART_REF_CLK_HZ
#define WS2814_EUSART_REF_CLK_HZ 39000000UL
#endif

/**
 * 须与 sl_spidrv_eusart_SPI_config.h 中 SL_SPIDRV_EUSART_SPI_BITRATE 一致。
 * 39MHz 下取 3000000 -> 实际 3.0MHz，每个 logic bit 占 4 个 SPI 位，约 1.33us，满足 T0/T1。
 */
#ifndef WS2814_SPI_BITRATE_HZ
#define WS2814_SPI_BITRATE_HZ 3000000UL
#endif

/** EUSART 分频后的 MOSI 位时钟（Hz） */
#define WS2814_SPI_ACTUAL_BITRATE_HZ \
    ((WS2814_EUSART_REF_CLK_HZ) / ((WS2814_EUSART_REF_CLK_HZ) / (WS2814_SPI_BITRATE_HZ)))

#ifndef WS2814_SPI_ONE_FRAME
#define WS2814_SPI_ONE_FRAME 0xF0
#endif

#ifndef WS2814_SPI_ZERO_FRAME
#define WS2814_SPI_ZERO_FRAME 0xC0
#endif

/** 规格书 RES 低电平最短时间 (us) */
#ifndef WS2814_RESET_TIME_US
#define WS2814_RESET_TIME_US 280U
#endif

/** 每 SPI 字节在总线上的时间 (ns)：8 bit / 实际 MOSI 时钟 */
#define WS2814_NS_PER_SPI_BYTE \
    ((8000000000ULL + (WS2814_SPI_ACTUAL_BITRATE_HZ) - 1ULL) / (WS2814_SPI_ACTUAL_BITRATE_HZ))

/** 每个 WS2814 逻辑位周期 (ns)：4 个 SPI 位 */
#define WS2814_NS_PER_LOGIC_BIT \
    ((4000000000ULL + (WS2814_SPI_ACTUAL_BITRATE_HZ) - 1ULL) / (WS2814_SPI_ACTUAL_BITRATE_HZ))

/** 满足 RES>=280us 所需的 0x00 尾部字节数（含约 5% 余量） */
#ifndef WS2814_SPI_RESET_BYTES
#define WS2814_SPI_RESET_BYTES                                                                                   \
    ((((WS2814_RESET_TIME_US) * 1000ULL * 120ULL / 100ULL) + WS2814_NS_PER_SPI_BYTE - 1ULL) / WS2814_NS_PER_SPI_BYTE)
#endif

/** 全黑帧重复发送次数，避免级联尾端灯珠残留上一帧数据 */
#ifndef WS2814_OFF_FRAME_REPEAT
#define WS2814_OFF_FRAME_REPEAT 2U
#endif

/**
 * 1：SPI 空闲时将 MOSI（EUSART1 TX / PC9）解除外设路由并配置为 GPIO 推挽输出低。
 */
#ifndef WS2814_SPI_MOSI_IDLE_GPIO_LOW
#define WS2814_SPI_MOSI_IDLE_GPIO_LOW 1
#endif

/**
 * 帧头 0x00：上电/帧间 MOSI 可能非低，首颗会误锁存；参考 ws2812 SPI 驱动在数据前发若干 0x00。
 */
#ifndef WS2814_SPI_LEAD_BYTES
#define WS2814_SPI_LEAD_BYTES 4U
#endif

#define WS2814_BYTES_PER_CHANNEL 4U
#define WS2814_CHANNELS_PER_LED 4U
#define WS2814_BYTES_PER_LED     (WS2814_BYTES_PER_CHANNEL * WS2814_CHANNELS_PER_LED)

#define WS2814_FRAME_PAYLOAD_OFFSET (WS2814_SPI_LEAD_BYTES)
#define WS2814_FRAME_PAYLOAD_BYTES  ((WS2814_LED_COUNT) * WS2814_BYTES_PER_LED)
#define WS2814_FRAME_RESET_OFFSET   (WS2814_FRAME_PAYLOAD_OFFSET + WS2814_FRAME_PAYLOAD_BYTES)
#define WS2814_FRAME_TOTAL_BYTES    (WS2814_FRAME_RESET_OFFSET + (WS2814_SPI_RESET_BYTES))

/** 四路 RGBW 混光映射到 SPI 灯带（色温 -> R,G,B,W） */
#ifndef RGBW_4CH_MIX_5CH
#define RGBW_4CH_MIX_5CH 1
#endif

#ifndef STRIP_MIN_BRIGHTNESS_PERCENT
#define STRIP_MIN_BRIGHTNESS_PERCENT 5
#endif

#ifndef STRIP_LEVEL_MAX
#define STRIP_LEVEL_MAX 1024
#endif

/**
 * 灯带输出渐变（开关/亮度/颜色 smoothstep + 定时器）。
 * 0：关闭渐变，属性变更后立即输出（当前默认）。
 * 1：开启开关/亮度/颜色 smoothstep 渐变。
 */
#ifndef STRIP_FADE_ENABLED
#define STRIP_FADE_ENABLED 0
#endif
