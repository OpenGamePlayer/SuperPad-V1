/*
 * pico/stdio compat — SuperPad-V1 Arduino 移植
 *
 * earlephilhower arduino-pico 核心自带 newlib stdio 钩子（_write/_read）
 * 和 stdio_flush（见 cores/rp2040/posix.cpp / main.cpp），因此 pico-sdk 的
 * 完整 pico_stdio 库不能直接使用（会与核心的 stdio_flush 重复定义）。
 *
 * 这里提供 Alpakka 固件实际用到、但核心未提供的三个 pico stdio 符号：
 *   - stdio_init_all()           —— no-op（Arduino 核心已初始化串口）
 *   - stdio_uart_init()          —— no-op（调试输出走核心的 DEBUG_RP2040_PORT）
 *   - stdio_getchar_timeout_us() —— 返回 -1（PICO_ERROR_TIMEOUT），
 *                                   Arduino 平台无 pico stdio 输入驱动。
 *
 * printf/scanf 系列由 newlib 提供并路由到核心的 _write/_read。
 */

#include <stdint.h>
#include "pico/stdio.h"
#include "pico/stdio_uart.h"

bool stdio_init_all(void) {
    // Arduino 核心已在 setup 前初始化好串口（Serial），无需再做。
    // 返回 false 表示"核心的 stdio 已由 Arduino 接管，pico stdio 未安装任何驱动"。
    return false;
}

void stdio_uart_init(void) {
    // no-op：调试输出由核心的 _write（DEBUG_RP2040_PORT）处理。
}

int stdio_getchar_timeout_us(uint32_t timeout_us) {
    // arduino-pico 没有 pico stdio 输入通道；返回超时（-1）。
    // uart.c 的 getchar_timeout_us() 将据此认为无输入。
    (void) timeout_us;
    return -1;  // PICO_ERROR_TIMEOUT
}
