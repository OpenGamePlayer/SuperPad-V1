# SuperPad-V1 / Alpakka — PlatformIO post-extra script
#
# 背景：maxgerhardt platform-raspberrypi (picosdk) 的 picosdk.py 只会在
# LIB_PICO_STDIO_USB（PIO_STDIO_USB）时编译 tinyusb 源码；而 Alpakka 固件
# 自带独立 USB 栈（tusb_config.h 定义 CFG_TUD_HID/VENDOR、CFG_TUD_CDC=0），
# 若启用 PIO_STDIO_USB 会与 pico_stdio_usb 冲突（Alpakka 的 tusb_config 劫持
# CDC 描述符宏）。因此这里手动编译 tinyusb（与 picosdk.py 的 build_tinyusb()
# 等价），让 Alpakka 的 hid.c / tusb_config.c 拿到 tud_hid_* 等符号。
#
# 参考 picosdk.py#build_tinyusb()：
#   CPPPATH+=lib/tinyusb/src
#   CPPDEFINES+=CFG_TUSB_DEBUG=0, CFG_TUSB_MCU=OPT_MCU_RP2040,
#               CFG_TUSB_OS=OPT_OS_PICO, PICO_RP2040_USB_*_FIX=1
#   BuildSources($BUILD_DIR/PicoSDKTinyUSB, <tinyusb src>, "+<*> -<portable> +<portable/raspberrypi>")

import os
Import("env")


def build_tinyusb():
    platform = env.PioPlatform()
    framework_dir = platform.get_package_dir("framework-picosdk")
    tinyusb_src = os.path.join(framework_dir, "lib", "tinyusb", "src")
    print("SuperPad-V1: manually building TinyUSB from %s" % tinyusb_src)
    env.Append(
        CPPPATH=[tinyusb_src],
        CPPDEFINES=[
            ("CFG_TUSB_DEBUG", 0),
            ("CFG_TUSB_MCU", "OPT_MCU_RP2040"),
            ("CFG_TUSB_OS", "OPT_OS_PICO"),
            ("PICO_RP2040_USB_DEVICE_UFRAME_FIX", 1),
            ("PICO_RP2040_USB_DEVICE_ENUMERATION_FIX", 1),
        ],
    )
    env.BuildSources(
        os.path.join("$BUILD_DIR", "PicoSDKTinyUSB"),
        tinyusb_src,
        "+<*> -<portable> +<portable/raspberrypi>",
    )
    # dcd_rp2040_irq references rp2040_usb_device_enumeration_fix, which
    # pico-sdk ships in pico_fix/rp2040_usb_device_enumeration. build_tinyusb()
    # in picosdk.py also compiles pico_fix for this reason.
    pico_fix_dir = os.path.join(framework_dir, "src", "rp2_common", "pico_fix")
    env.BuildSources(
        os.path.join("$BUILD_DIR", "PicoSDKPicoFix"),
        pico_fix_dir,
        "+<rp2040_usb_device_enumeration>",
    )


build_tinyusb()