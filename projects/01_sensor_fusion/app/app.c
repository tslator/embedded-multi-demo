#include <stdint.h>
#include <stdbool.h>

#include "platform.h"
#include "platform_delay.h"
#include "platform_serial.h"
#include "drivers.h"
#include "app.h"
#include "setpoint_svc.h"
#include "heartbeat_svc.h"
#include "telemetry_svc.h"

static uint16_t loop_delay = 255;


void app_init(void)
{
    setpoint_svc_init();
    heartbeat_svc_init();
}

void app_config(app_config_t config)
{
    loop_delay = config.loop_delay;
    heartbeat_svc_set_led_enabled(config.heartbeat_led_enabled);
}

void app_setpoint_update(void)
{
    bool result = setpoint_svc_update();
    if (!result)
    {
        // log update failure
    }
}

void app_heartbeat_publish(void)
{
    bool result = heartbeat_svc_publish_next();
    if (!result)
    {
        // log publish failure
    }
}

void app_telemetry_publish(void)
{
    telemetry_svc_publish();
}

void app_throttle_loop(void)
{
    platform_delay_ms(loop_delay);
}

void app_stop(void)
{
    setpoint_svc_stop();
    heartbeat_svc_stop();
}

void app_shutdown(void)
{
    app_stop();
}
