# RP2040 → Keil MDK 移植指南

> 本目录 `keil/RP2040/` 是 Alpakka 官方固件源码（与 `platformio_ide/RP2040/` 同源，保留上游 Makefile/CMake 便于参考）。本指南说明如何把它在 **Keil MDK (uVision)** 中编译。

## 背景：为什么不能直接打开

- Alpakka 官方固件基于 **Raspberry Pi Pico SDK**，官方构建方式为 **CMake + ARM GNU 工具链**（见上游 `CMakeLists.txt`、`Makefile`）。
- Keil MDK 的 uVision 使用自家 AC5/AC6 编译器和自己的工程格式（`.uvprojx`），**不能直接打开** pico-sdk 的 CMake 工程。
- 因此需要"导入"而非"打开"。下面给出两条路径，推荐路径 1。

## 前置条件

- **Keil MDK**（建议 5.37+）
- **Raspberry Pi Pico 器件支持包**：uVision 中 `Pack Installer` → 搜索 `Raspberry Pi Pico SDK` / `RP2040` 设备包并安装（Keil 官方提供 RP2040 支持）
- **Python 3 + Git**（用于拉取 pico-sdk 和运行辅助脚本）

## 路径 1（推荐）：CMake 生成 uVision 工程

RP2040 的 pico-sdk 官方已提供对 Keil 的桥接支持（`pico_set_binary_type` / 由社区工具生成 `.uvprojx`），基本流程：

1. 准备依赖：
   ```bash
   cd keil/RP2040
   ./scripts/install.sh      # 拉取 deps/pico-sdk、pico-extras、picotool、arm 工具链（上游官方脚本）
   ```
2. 用 CMake 配置，生成面向 Keil 的工程文件：
   ```bash
   mkdir build-keil && cd build-keil
   cmake .. -DDEVICE=alpakka_v0 -DPICO_BOARD=pico -G "NMake Makefiles"  # 或 VS 生成器
   ```
   （若你的 pico-sdk 版本带 uVision 生成支持，会在 build 目录产出 `.uvprojx`。）
3. 用 Keil uVision 打开生成的 `.uvprojx`，选择 `AC6` 编译器，编译 `alpakka_v0` 目标。

> 说明：上游官方并未在仓库中内置 `.uvprojx`，此路径依赖 pico-sdk / 社区对 Keil 的桥接。若你的 pico-sdk 版本无内置支持，请走路径 2。

## 路径 2：Keil 原生工程（工作量较大）

1. 安装 **RP2040 器件包（DFP）** 后，在 uVision 新建空工程，芯片选 `RP2040`（Cortex-M0+）。
2. 将 `src/` 下全部 `.c`（含 `profiles/`、`thumbstick/`）加入工程；`src/headers/` 加入 Include 路径。
3. 拷贝 pico-sdk 的 `pico-sdk/src/rp2_common` 相关源码到工程（或链接其 include 路径），并补充 tinyusb、hardware_* 等库。
4. 用 AC6 编译，处理头文件与宏（`DEVICE_ALPAKKA_V0=1`、`DEVICE_IS_ALPAKKA=1`）。

## 宏定义对照（两个路径都需要）

| 宏 | 值 | 目标 |
| --- | --- | --- |
| `DEVICE_ALPAKKA_V0` | 1 | alpakka_v0 |
| `DEVICE_IS_ALPAKKA` | 1 | 所有 Alpakka 变体 |

## 常见问题

| 现象 | 处理 |
| --- | --- |
| 找不到 `pico/xxx.h` | 加入 pico-sdk 的 include 路径，或先跑 `scripts/install.sh` |
| 找不到 `version.h` | 手动创建 `src/headers/version.h`，内容 `#define VERSION "0.1.0"`（构建钩子会自动生成） |
| 链接 tinyusb 报错 | 确认 pico-sdk 的 tinyusb 子模块已初始化（`git submodule update --init`） |
| `-Werror` 告警变错误 | Keil 工程中关闭 treat warnings as errors，或逐个修 |

## 状态

- [ ] 本机 Keil 编译验证（需 Keil MDK 环境）
