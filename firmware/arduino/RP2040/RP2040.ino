// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2022, Input Labs Oy.  (SuperPad-V1 Arduino port)
//
// SuperPad-V1 / Alpakka Arduino entry point.
//
// The upstream Alpakka firmware is a C project using a custom `main()` that
// calls loop_controller_init() and then blocks in an infinite loop
// (loop_run()). On Arduino the core provides `main()` and we implement
// setup()/loop() instead:
//   - setup():  perform controller init (loop_controller_init)
//   - loop():   run one controller tick (loop_controller_task)
// The infinite `loop_run()` in loop.c is compiled out under ARDUINO.
//
// Build with:
//   arduino-cli compile --fqbn rp2040:rp2040:rpipico \
//     --build-property "compiler.cpp.extra_flags=-DDEVICE_ALPAKKA_V0=1 -DDEVICE_IS_ALPAKKA=1 -I src/headers" \
//     --build-property "compiler.c.extra_flags=-DDEVICE_ALPAKKA_V0=1 -DDEVICE_IS_ALPAKKA=1 -I src/headers" \
//     .
//
// Or open this folder (SuperPadV1.ino) in Arduino IDE 2.x with the
// "Raspberry Pi Pico/RP2040/RP2350" core (earlephilhower) installed, board
// set to "Raspberry Pi Pico", and the DEVICE_* macros added to
// Tools > Compiler flags / platform.local.txt.

extern "C" {
#include "loop.h"
}

void setup() {
    #if defined DEVICE_ALPAKKA_V0 || defined DEVICE_ALPAKKA_V1
        loop_controller_init();
    #elif defined DEVICE_DONGLE
        loop_dongle_init();
    #elif defined DEVICE_LLAMA
        loop_llama_init();
    #else
        #error "No device selected: define DEVICE_ALPAKKA_V0 / DEVICE_ALPAKKA_V1 / DEVICE_DONGLE / DEVICE_LLAMA"
    #endif
}

void loop() {
    #if defined DEVICE_ALPAKKA_V0 || defined DEVICE_ALPAKKA_V1
        loop_controller_task();
    #elif defined DEVICE_DONGLE
        loop_dongle_task();
    #endif
}
