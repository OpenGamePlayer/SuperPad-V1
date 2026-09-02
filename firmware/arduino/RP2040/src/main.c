// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2022, Input Labs Oy.

#include "loop.h"

// NOTE (SuperPad-V1 Arduino port): on the Arduino platform the core provides
// `main()` (which calls setup()/loop()); this C entry point is disabled to
// avoid a duplicate symbol. Entry lives in SuperPadV1.ino instead.
#ifndef ARDUINO
int main() {
    #if defined DEVICE_ALPAKKA_V0
        loop_controller_init();
    #elif defined DEVICE_ALPAKKA_V1
        loop_controller_init();
    #elif defined DEVICE_DONGLE
        loop_dongle_init();
    #elif defined DEVICE_LLAMA
        loop_llama_init();
    #endif
}
#endif
