#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

typedef void (*CORE1_FUNCTION)(void);

void platform_init();
void platform_core1_start(CORE1_FUNCTION func);
void platform_fifo_push_u32(uint32_t value);
uint32_t platform_fifo_pop_u32_blocking();

#endif