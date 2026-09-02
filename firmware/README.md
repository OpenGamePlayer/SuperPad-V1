# SuperPad-V1 firmware

Alpakka（Input Labs）固件多平台移植。

## 目录结构

```
firmware/
├── arduino/             # Arduino 平台（arduino-cli / Arduino IDE / earlephilhower arduino-pico 核心）
│   └── RP2040/          #   RP2040（Alpakka 官方目标：Raspberry Pi Pico）
├── keil/                # Keil MDK (uVision) 工程
│   └── RP2040/          #   RP2040（Alpakka 官方目标）
└── platformio_ide/      # PlatformIO IDE 工程
    └── RP2040/          #   RP2040（Alpakka 官方目标）
```

> 约定：每个开发平台目录下按**芯片型号**分子目录；新增芯片时照此新建（如 `keil/STM32F4xx/`、`platformio_ide/ESP32/`）。

## 平台速览

| 平台 | 目录 | 工具链 | 状态 |
| --- | --- | --- | --- |
| Arduino | `arduino/RP2040/` | arduino-cli / Arduino IDE 2.x + arduino-pico | ✅ 本机验证编译通过（UF2 已生成） |
| PlatformIO | `platformio_ide/RP2040/` | PlatformIO + maxgerhardt raspberrypi (picosdk) | ✅ 本机验证编译通过（`pio run`，elf/uf2/bin 已生成） |
| Keil MDK | `keil/RP2040/` | Keil uVision | 📋 指南已提供，需 Keil 环境实机验证 |

## Arduino（earlephilhower arduino-pico 核心）

源码：`arduino/RP2040/`，移植细节见 [PORT_TO_ARDUINO.md](./arduino/RP2040/PORT_TO_ARDUINO.md)。

```powershell
# Windows 一键构建（自动定位 arduino-cli 与核心）
cd arduino/RP2040
./build.ps1

# 或手动
arduino-cli compile --fqbn rp2040:rp2040:rpipico:usbstack=tinyusb \
  --build-property "compiler.c.extra_flags=<DEVICE_宏与 -I 路径>" \
  --build-property "compiler.cpp.extra_flags=<同上>" arduino/RP2040
```

要点：
- FQBN 用 **`usbstack=tinyusb`**（Adafruit TinyUSB），核心的裁剪库才含 HID/VENDOR 类。
- 已做三处移植适配：`.ino` 入口（setup/loop）、裁剪版 `pico/sleep`、`pico_compat/`（tinyusb HID 类 + 轻量 stdio）。
- 产物 `RP2040.ino.uf2` 拖入 BOOTSEL 模式的 Pico 即可烧录。

## PlatformIO（原生 pico-sdk）

源码：`platformio_ide/RP2040/`。

**官方 CMake 路线（本机已验证，Windows + ninja + pico-sdk 2.2.0）**：

```bash
cd platformio_ide/RP2040
cmake -S . -B build-cmake -DDEVICE=alpakka_v0 -DPICO_BOARD=pico -DPICO_NO_PICOTOOL=1 -G Ninja
cmake --build build-cmake     # 产物 build-cmake/alpakka.elf / .bin / .hex
```

> 依赖：pico-sdk + pico-extras 克隆到 `deps/`（已在 .gitignore 排除），ARM GNU 工具链 + ninja 在 PATH 中。

**PlatformIO `pio run` 路线**：`platformio.ini` 指向社区 fork
`maxgerhardt/platform-raspberrypi#develop`（`framework = picosdk`），该 fork 的
develop 分支提供原生 pico-sdk 支持；官方 `platformio/raspberrypi` 只支持
arduino framework。首次 `pio run` 会自动下载 fork 平台与工具链。

```bash
cd platformio_ide/RP2040
pio run                    # 编译 alpakka_v0（产物 .pio/build/alpakka_v0/firmware.elf / .uf2 / .bin）
pio run -t upload          # 烧录（Pico 需进入 BOOTSEL）
```

- 构建钩子 `scripts/gen_version.py` 会自动生成上游 `version.sh` 产出的 `src/headers/version.h`。
- 构建钩子 `scripts/build_tinyusb.py`：fork 的 picosdk.py 只会在启用 USB stdio
  （`PIO_STDIO_USB`）时编译 tinyusb，而 Alpakka 自带独立 USB 栈（`tusb_config.h`
  定义 CFG_TUD_HID/VENDOR、CFG_TUD_CDC=0），与 `pico_stdio_usb` 冲突。因此本工程用
  `PIO_STDIO_UART`（串口 stdio），并由该钩子手动编译 tinyusb + pico_fix，等价于
  picosdk.py 的 `build_tinyusb()`。
- pico-sdk 的 `pico_stdio_usb` 依赖 Alpakka 关闭的 CDC 类，故不启用（Alpakka 固件
  用 UART 日志 + 自管 USB 描述符）。
- `src/pico_extras/` 存放从 pico-extras 精简来的 `hardware_rosc` + `pico_sleep`
  （该 fork 的 pico-sdk 包未含 pico-extras 组件），PlatformIO 会随 `src/` 一并编译。
- 上游以 `-Werror` 编译；本移植 `-Wno-error`（GCC 9.2.1 下 config.c 有 format-truncation 告警）。

## Keil MDK（Windows）

源码：`keil/RP2040/`（与 PlatformIO 同源，另含上游 Makefile/CMake 供参考）。
RP2040 在 Keil 下的路径见 [PORT_TO_KEIL.md](./keil/RP2040/PORT_TO_KEIL.md)：
1. **CMake 生成 uVision 工程**（推荐）：ARM GNU 工具链 + RP2040 支持包 + CMake 导出 `.uvprojx`。
2. **手动建 Keil 原生工程**：装 RP2040 DFP，把 `src/` 加入工程，补 pico-sdk 头文件/库，AC6 适配。

## 构建宏对照（所有平台一致）

| 宏 | 值 | 目标 |
| --- | --- | --- |
| `DEVICE_ALPAKKA_V0` | 1 | Alpakka v0（Pico 单板） |
| `DEVICE_ALPAKKA_V1` | 1 | Alpakka v1（Marmota 模块） |
| `DEVICE_DONGLE` | 1 | 无线接收器 |
| `DEVICE_LLAMA` | 1 | ESP 刷写器 |
| `DEVICE_IS_ALPAKKA` | 1 | 所有 Alpakka 变体 |

## 状态

- [x] 源码已按 <开发平台>/<芯片型号>/ 结构列入仓库（arduino / keil / platformio_ide）
- [x] Arduino 平台：本机验证编译通过（RP2040，UF2 290KB）
- [x] PlatformIO：官方 CMake 路线本机验证编译通过（alpakka.elf 1.8MB）
- [x] PlatformIO：`pio run` 本机验证编译通过（firmware.elf / .uf2 / .bin）
- [x] Keil：移植指南（PORT_TO_KEIL.md）
- [ ] Keil：需 Keil MDK 环境实机验证