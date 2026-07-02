#include <pico/stdlib.h>

#include "gpio_led.h"

void gpio_led_init()
{
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}

void gpio_led_toggle()
{
    bool value = gpio_get(PICO_DEFAULT_LED_PIN);
    value = value ? false : true;
    gpio_put(PICO_DEFAULT_LED_PIN, value);
}