#include <stdint.h>
#include <stdbool.h>

#include "platform/delay.h"
#include "app.h"
#include "led.h"

static uint16_t led_delay = 255;

void app_init(void)
{
    led_init();
}

void app_config(Config config)
{
    led_delay = config.led_delay;
}

void app_run(void)
{
    while (true) 
    {
        led_toggle();
        delay_ms(led_delay);
        led_toggle();
        delay_ms(led_delay);
    }
}

void app_stop(void)
{

}

void app_shutdown(void)
{

}
