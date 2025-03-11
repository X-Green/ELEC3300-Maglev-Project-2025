//
// Created by eeasee on 2025/3/11.
//

#pragma once

#define configASSERT(x) \
    if (!(x))           \
        __asm volatile("bkpt ");

