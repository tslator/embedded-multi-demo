#include <stdint.h>
#include <stdbool.h>

#include "platform/platform.h"
#include "platform/platform_delay.h"
#include "platform/platform_serial.h"
#include "drivers.h"
#include "app.h"
#include "led.h"

static uint16_t loop_delay = 255;
static volatile uint32_t count = 0;


void app_init(void)
{
}

void app_config(app_config_t config)
{
    loop_delay = config.loop_delay;
}

void app_update_count(void)
{
    platform_fifo_push_u32(count);
    count++;
    if (count > 10)
    {
        count = 0;
    }
}

void app_update_telemetry(void)
{
    platform_serial_println("Looping ...");
    led_toggle();
}

void app_throttle_loop(void)
{
    platform_delay_ms(loop_delay);
}

void app_stop(void)
{

}

void app_shutdown(void)
{

}
