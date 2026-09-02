# RP2040 → Arduino 平台移植说明

> 本目录 `arduino/RP2040/` 是 Alpakka 官方固件源码的 **Arduino 平台移植**。
> 可在 **Arduino IDE 2.x** 或 **arduino-cli** 中编译为 Raspberry Pi Pico 可直接烧录的 UF2。
> 与本仓 `platformio_ide/`、`keil/` 目录为同源多平台移植。

## 为什么需要专门移植（背景）

上游 Alpakka 固件是 **原生 pico-sdk + CMake** 工程，直接把它放进 Arduino 编译会
遇到三处结构性问题，本移植逐一解决：

1. **入口模型差异**：Arduino 核心自带 `main()`，调用 `setup()`/`loop()`。
   - `src/main.c` 的 `int main()` 用 `#ifndef ARDUINO` 屏蔽（见 ARDUINO 宏）。
   - `RP2040.ino` 提供 `setup()`（调用 `loop_controller_init`）与 `loop()`
     （调用 `loop_controller_task`）。
   - `src/loop.c` 中的 `loop_run()`（无限循环）在 ARDUINO 下不调用。

2. **pico_sleep 缺失**：`power.c` 用到 pico-extras 的 `pico/sleep.h`
   （`sleep_goto_dormant_until_edge_high` 等），而 arduino-pico 核心**不带
   pico-extras**。移植做法：本地 `src/pico/` 提供**裁剪版** sleep 实现，只保留
   本固件用到的函数（`sleep_run_from_xosc` / `sleep_goto_dormant_until_pin` /
   `sleep_power_up`），去掉依赖 aon_timer 的部分。原实现 BSD-3-Clause 版权保留在文件头。

3. **tinyusb / pico_stdio 被核心裁剪**：arduino-pico 的预编译 `libpico.a` 只含
   它自己需要的 tinyusb CDC 类和 stdio 钩子，**不含 HID/VENDOR 类与
   pico_stdio 全量实现**。移植处理：
   - `src/pico_compat/class/hid/`：补入 tinyusb 的 `hid_device.c/h`（HID 类）——
     与 libpico.a 中已有的 usbd/dcd/vendor 配合。
   - `src/pico_compat/stdio_compat.c`：轻量 stdio 适配（`stdio_init_all` 等
     no-op；`_write`/`_read`/`stdio_flush` 由核心提供）。

## 编译方法

### 方式 A：一键脚本（Windows，推荐）

```powershell
cd firmware/arduino/RP2040
./build.ps1
```

脚本自动定位 arduino-cli 与 rp2040 核心，产物在
`%LOCALAPPDATA%\arduino\sketches\...\RP2040.ino.uf2`。

### 方式 B：arduino-cli 手动

```bash
arduino-cli core install rp2040:rp2040
arduino-cli compile \
  --fqbn rp2040:rp2040:rpipico:usbstack=tinyusb \
  --build-property "compiler.cpp.extra_flags=-DDEVICE_ALPAKKA_V0=1 -DDEVICE_IS_ALPAKKA=1 -I src -I src/headers -Wno-error" \
  --build-property "compiler.c.extra_flags=同上" \
  firmware/arduino/RP2040
```

要点：
- FQBN 必须带 `usbstack=tinyusb`（Adafruit TinyUSB），否则核心缺 HID 类。
- `-I src` 用于 `#include <pico/sleep.h>`（本地裁剪版）；如果编译器找不到
  `hardware/rosc.h`，把核心的 `.../pico-sdk/src/rp2_common/hardware_rosc/include`
  加入 `-I`（见 build.ps1）。

### 方式 C：Arduino IDE 2.x

1. 安装 "Raspberry Pi Pico/RP2040/RP2350" 板卡（earlephilhower 核心）。
2. 打开 `firmware/arduino/RP2040/RP2040.ino`。
3. 板型选 **Raspberry Pi Pico**，USB 栈选 **Adafruit TinyUSB**。
4. 在 `board.local.txt` 或 IDE 的 "Compiler options" 中加：
   `-DDEVICE_ALPAKKA_V0=1 -DDEVICE_IS_ALPAKKA=1 -I src -I src/headers`
   （IDE 方式下 `-I` 路径写 sketch 目录相对路径即可）。

## 烧录

将 `RP2040.ino.uf2` 拖入 BOOTSEL 模式的 Pico（或 `arduino-cli upload`）。

## 已知限制

| 项 | 说明 |
| --- | --- |
| USB 栈 | 走 Adafruit TinyUSB，XInput/WEBUSB 描述符源自 Alpakka 的 tusb_config.h，与实际 PC 端的兼容性需实测 |
| 休眠 | 裁剪版 pico_sleep 保留常用函数；定时休眠（aon_timer）未移植 |
| stdio 输入 | `stdio_getchar_timeout_us` 返回 -1（无输入通道）；仅 UART 输出 |
| 版本头 | `src/headers/version.h` 若缺失，按 `#define VERSION "0.1.0"` 手动创建，或参考 platformio 的 `scripts/gen_version.py` |

## 许可

Alpakka 固件 **GPL-2.0**；裁剪的 pico_sleep 为 **BSD-3-Clause**（保留原版权头）。