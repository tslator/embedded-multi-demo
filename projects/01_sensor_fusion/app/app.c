#include <stdint.h>
#include <stdbool.h>
#include <memory.h>
#include <stdio.h>

#include "platform/platform.h"
#include "platform/platform_delay.h"
#include "platform/platform_serial.h"
#include "drivers.h"
#include "app.h"
#include "led.h"

static uint16_t led_delay = 255;

static volatile uint32_t count = 0;


static void heartbeat_func()
{
    char msg[64];
    memset(msg, 0, sizeof(msg));
    while (true)
    {
        uint32_t value = platform_fifo_pop_u32_blocking();
        int n = snprintf(msg, sizeof(msg), "Heartbeat got: %lu", value);
        serial_println(msg);
        // serial_println("looping ...");
    }

}


void app_init(void)
{
    platform_init();
    drivers_init();
    platform_core1_start(heartbeat_func);
}

void app_config(Config config)
{
    led_delay = config.led_delay;
}

void app_run(void)
{
    while (true) 
    {
        platform_fifo_push_u32(count);
        count++;
        if (count > 10)
        {
            count = 0;
        }
        serial_println("Looping ...");
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
