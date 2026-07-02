#include <stdint.h>
#include <stdbool.h>

#include "platform/platform.h"
#include "platform/platform_delay.h"
#include "platform/platform_serial.h"
#include "app.h"
#include "led.h"

static uint16_t led_delay = 255;

void app_init(void)
{
    platform_init();
}

void app_config(Config config)
{
    led_delay = config.led_delay;
}

void app_run(void)
{
    serial_println("Starting loop ...");
    while (true) 
    {
        serial_println("Looping ...");
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
