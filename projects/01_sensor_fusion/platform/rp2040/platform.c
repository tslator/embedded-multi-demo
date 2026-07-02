#include <pico/stdlib.h>

#include "platform.h"
#include "platform_led.h"

void platform_init()
{
    stdio_init_all();
    platform_led_init();
}