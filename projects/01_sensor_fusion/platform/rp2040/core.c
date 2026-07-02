#include <pico/stdlib.h>
#include <pico/multicore.h>


#include "platform.h"

void platform_init()
{
    stdio_init_all();
}

void platform_core1_start(CORE1_FUNCTION func)
{
    multicore_launch_core1(func);
}

void platform_fifo_push_u32(uint32_t value)
{
    multicore_fifo_push_blocking(value);
}

uint32_t platform_fifo_pop_u32_blocking()
{
    return multicore_fifo_pop_blocking();
}
