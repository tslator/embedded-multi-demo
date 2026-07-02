#include "platform/gpio_led.h"


void led_init(void)
{
    gpio_led_init();
}

void led_toggle()
{
    gpio_led_toggle();
}