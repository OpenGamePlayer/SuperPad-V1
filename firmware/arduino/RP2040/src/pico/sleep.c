/*
 * pico/sleep.c — 裁剪版（SuperPad-V1 Arduino 移植）
 *
 * 从 pico-extras 的 pico_sleep/sleep.c 精简而来（BSD-3-Clause，
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.）。
 *
 * 仅保留 SuperPad-V1 固件用到的函数：
 *   - sleep_run_from_dormant_source（sleep_run_from_xosc 的底层实现）
 *   - sleep_goto_dormant_until_pin（sleep_goto_dormant_until_edge_high 的底层实现）
 *   - sleep_power_up
 * 依赖 pico-extras aon_timer 的 sleep_goto_sleep_until / sleep_goto_dormant_until
 * 及其它定时休眠 API 已省略。
 */

#include <stdio.h>
#include <inttypes.h>

#include "pico.h"
#include "pico/stdlib.h"
#include "pico/stdio_uart.h"
#include "pico/sleep.h"

#include "hardware/pll.h"
#include "hardware/regs/clocks.h"
#include "hardware/clocks.h"
#include "hardware/watchdog.h"
#include "hardware/xosc.h"
#include "hardware/rosc.h"
#include "hardware/regs/io_bank0.h"
// For __wfi
#include "hardware/sync.h"
#include "pico/runtime_init.h"

#ifndef __riscv
// For scb_hw so we can enable deep sleep
#include "hardware/structs/scb.h"
#endif

static dormant_source_t _dormant_source;

bool dormant_source_valid(dormant_source_t dormant_source)
{
    switch (dormant_source) {
        case DORMANT_SOURCE_XOSC:
            return true;
        case DORMANT_SOURCE_ROSC:
            return true;
        default:
            return false;
    }
}

// In order to go into dormant mode we need to be running from a stoppable clock source:
// either the xosc or rosc with no PLLs running. This means we disable the USB and ADC clocks
// and all PLLs
void sleep_run_from_dormant_source(dormant_source_t dormant_source) {
    assert(dormant_source_valid(dormant_source));
    _dormant_source = dormant_source;

    uint src_hz;
    uint clk_ref_src;
    switch (dormant_source) {
        case DORMANT_SOURCE_XOSC:
            src_hz = XOSC_HZ;
            clk_ref_src = CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC;
            break;
        case DORMANT_SOURCE_ROSC:
            src_hz = 6500 * KHZ; // todo
            clk_ref_src = CLOCKS_CLK_REF_CTRL_SRC_VALUE_ROSC_CLKSRC_PH;
            break;
        default:
            hard_assert(false);
    }

    // CLK_REF = XOSC or ROSC
    clock_configure(clk_ref,
                    clk_ref_src,
                    0, // No aux mux
                    src_hz,
                    src_hz);

    // CLK SYS = CLK_REF
    clock_configure(clk_sys,
                    CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF,
                    0, // Using glitchless mux
                    src_hz,
                    src_hz);

    // CLK ADC = 0MHz
    clock_stop(clk_adc);
    clock_stop(clk_usb);

#if PICO_RP2040
    // CLK RTC = ideally XOSC (12MHz) / 256 = 46875Hz but could be rosc
    uint clk_rtc_src = (dormant_source == DORMANT_SOURCE_XOSC) ?
                       CLOCKS_CLK_RTC_CTRL_AUXSRC_VALUE_XOSC_CLKSRC :
                       CLOCKS_CLK_RTC_CTRL_AUXSRC_VALUE_ROSC_CLKSRC_PH;

    clock_configure(clk_rtc,
                    0, // No GLMUX
                    clk_rtc_src,
                    src_hz,
                    46875);
#endif

    // CLK PERI = clk_sys. Used as reference clock for Peripherals. No dividers so just select and enable
    clock_configure(clk_peri,
                    0,
                    CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
                    src_hz,
                    src_hz);

    pll_deinit(pll_sys);
    pll_deinit(pll_usb);

    // Assuming both xosc and rosc are running at the moment
    if (dormant_source == DORMANT_SOURCE_XOSC) {
        // Can disable rosc
        rosc_disable();
    } else {
        // Can disable xosc
        xosc_disable();
    }

    // Reconfigure uart with new clocks
    // NOTE (SuperPad-V1 Arduino port): upstream calls setup_default_uart() here,
    // which pico-sdk provides via pico_stdlib; the arduino-pico core does not
    // ship it. stdio_uart_init() re-initialises the UART stdio, which is the
    // relevant behaviour after a clock change.
    stdio_uart_init();
}

static void processor_deep_sleep(void) {
    // Enable deep sleep at the proc
#ifndef __riscv
    scb_hw->scr |= ARM_CPU_PREFIXED(SCR_SLEEPDEEP_BITS);
#endif
}

static void _go_dormant(void) {
    assert(dormant_source_valid(_dormant_source));

    if (_dormant_source == DORMANT_SOURCE_XOSC) {
        xosc_dormant();
    } else {
        rosc_set_dormant();
    }
}

void sleep_goto_dormant_until_pin(uint gpio_pin, bool edge, bool high) {
    bool low = !high;
    bool level = !edge;

    // Configure the appropriate IRQ at IO bank 0
    assert(gpio_pin < NUM_BANK0_GPIOS);

    uint32_t event = 0;

    if (level && low) event = IO_BANK0_DORMANT_WAKE_INTE0_GPIO0_LEVEL_LOW_BITS;
    if (level && high) event = IO_BANK0_DORMANT_WAKE_INTE0_GPIO0_LEVEL_HIGH_BITS;
    if (edge && high) event = IO_BANK0_DORMANT_WAKE_INTE0_GPIO0_EDGE_HIGH_BITS;
    if (edge && low) event = IO_BANK0_DORMANT_WAKE_INTE0_GPIO0_EDGE_LOW_BITS;

    gpio_init(gpio_pin);
    gpio_set_input_enabled(gpio_pin, true);
    gpio_set_dormant_irq_enabled(gpio_pin, event, true);

    _go_dormant();
    // Execution stops here until woken up

    // Clear the irq so we can go back to dormant mode again if we want
    gpio_acknowledge_irq(gpio_pin, event);
    gpio_set_input_enabled(gpio_pin, false);
}

// To be called after waking up from sleep/dormant mode to restore system clocks properly
void sleep_power_up(void)
{
    // Re-enable the ring oscillator, which will essentially kickstart the proc.
    // NOTE: newer pico-sdk renamed rosc_enable() to rosc_restart(); the arduino-pico
    // core ships the newer API.
    rosc_restart();

    // Reset the sleep enable register so peripherals and other hardware can be used
    clocks_hw->sleep_en0 |= ~(0u);
    clocks_hw->sleep_en1 |= ~(0u);

    // Restore all clocks
    clocks_init();

    // UART needs to be reinitialised with the new clock frequencies for stable output.
    // (See note above about setup_default_uart vs stdio_uart_init on arduino-pico.)
    stdio_uart_init();
}
