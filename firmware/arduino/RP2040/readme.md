# Alpakka Firmware

> **SuperPad-V1 移植说明**：本目录已适配 Arduino（earlephilhower arduino-pico 核心）。
> - 编译：`./build.ps1`（Windows 一键，详见 [PORT_TO_ARDUINO.md](PORT_TO_ARDUINO.md)）。
> - 产物：`RP2040.ino.uf2`，拖入 BOOTSEL 模式的 Pico 即可烧录。
> - 以下是上游原始 README（Makefile 路线参考）。

*Alpakka controller reference firmware (for Raspberry Pi Pico)*
## Project links
- [Alpakka Manual](https://inputlabs.io/devices/alpakka/manual).
- [Alpakka Firmware](https://github.com/inputlabs/alpakka_firmware). _(you are here)_
- [Alpakka PCB](https://github.com/inputlabs/alpakka_pcb).
- [Alpakka 3D-print](https://github.com/inputlabs/alpakka_case).
- [Input Labs Roadmap](https://github.com/orgs/inputlabs/projects/2/views/2).

## Supported developer operative systems
- GNU/Linux (and MacOS) - See [Development in Linux](https://inputlabs.io/devices/alpakka/manual/dev_unix).
- Windows - See [Development in Windows](https://inputlabs.io/devices/alpakka/manual/dev_windows).

## System dependencies
With `apt`, `rpm`, `pacman`, `brew`, or the equivalent package manager of your system, install:
- **gcc**
- **git**
- **cmake**

## Project dependencies
- `make install`: Download and configure dependencies automatically.

## Compilation targets
- `alpakka_v0`: Alpakka v0.x.x with Raspberry Pico.
- `alpakka_v1`: Alpakka v1.x.x with Marmota module.
- `dongle`: Wireless dongle.
- `llama`: ESP wireless module flasher (ESP-LLAMA repository).

Example usage:
```
DEVICE=alpakka_v1 make
```

## Development commands
- `DEVICE=<device> make`: Build compilation environment and build executables.
- `make rebuild`: Build executables again using cache (faster).
- `make load`: Load built .uf2 file into the Pico (requires bootsel mode or active session).
- `make reload`: Do both `rebuild` and `load` commands (for dev convenience).
- `make clean`: Delete previous build files.
- `make session`: Connect to UART serial stdio, and display controller log.

While having an active session:
- `make restart`: Restart the controller.
- `make bootsel`: Put the controller in bootsel mode.
- `make calibrate`: Calibrate thumbstick and IMUs.
- `make format`: Format NVM sector and reset to initial values.
- `make test`: Start a semi-manual testing procedure for the buttons and axis.

## Devkit button
- Single press: Restart the controller.
- Double press: Put the controller in bootsel mode.

Read the [Alpakka developer manual](https://inputlabs.io/devices/alpakka/manual/dev) for details about the **devkit** and more.
