/*
 * pico/sleep.h — 裁剪版（SuperPad-V1 Arduino 移植）
 *
 * 上游 power.c 需要 pico-extras 的 pico_sleep 模块。Arduino 平台
 * （earlephilhower arduino-pico 核心）自带 pico-sdk 但不含 pico-extras，
 * 因此这里提供一个精简实现：只保留本固件用到的
 *   sleep_run_from_xosc / sleep_goto_dormant_until_edge_high / sleep_power_up
 * 依赖 aon_timer 的 sleep_goto_sleep_until / sleep_goto_dormant_until 被省略。
 *
 * 原实现版权 (c) 2020 Raspberry Pi (Trading) Ltd., BSD-3-Clause。
 * 实现见同目录 sleep.c。
 */
#ifndef _PICO_SLEEP_H_
#define _PICO_SLEEP_H_

#include "pico.h"
#include "hardware/rosc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DORMANT_SOURCE_NONE,
    DORMANT_SOURCE_XOSC,
    DORMANT_SOURCE_ROSC,
    DORMANT_SOURCE_LPOSC, // rp2350 only
} dormant_source_t;

void sleep_run_from_dormant_source(dormant_source_t dormant_source);

static inline void sleep_run_from_xosc(void) {
    sleep_run_from_dormant_source(DORMANT_SOURCE_XOSC);
}

static inline void sleep_run_from_rosc(void) {
    sleep_run_from_dormant_source(DORMANT_SOURCE_ROSC);
}

void sleep_goto_dormant_until_pin(uint gpio_pin, bool edge, bool high);

static inline void sleep_goto_dormant_until_edge_high(uint gpio_pin) {
    sleep_goto_dormant_until_pin(gpio_pin, true, true);
}

static inline void sleep_goto_dormant_until_level_high(uint gpio_pin) {
    sleep_goto_dormant_until_pin(gpio_pin, false, true);
}

void sleep_power_up(void);

#ifdef __cplusplus
}
#endif

#endif
