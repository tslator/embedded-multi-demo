#include <pico/stdlib.h>

#include "platform_delay.h"

void delay_ms(int delay)
{
    sleep_ms(delay);
}
