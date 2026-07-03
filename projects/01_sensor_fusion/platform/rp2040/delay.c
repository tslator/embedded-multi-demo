#include <pico/stdlib.h>
#include <stdint.h>

#include "platform_delay.h"

void platform_delay_ms(uint32_t delay)
{
    sleep_ms(delay);
}
