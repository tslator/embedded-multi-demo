#include "app.h"
#include "drivers.h"
#include "platform.h"
#include "platform_adc.h"
#include "system.h"
#include "system_types.h"
#include "app_types.h"
#include "scheduler.h"

static system_config_t const sys_config = {
    .adc_channel = 0,
    .role = HEARTBEAT
};


system_status_t system_init()
{
    platform_init();
    drivers_init();
    app_init();

    return SYS_OK;
}

system_config_t system_get_default_config(void)
{
    return sys_config;
}

system_status_t system_config(system_config_t config)
{
    switch (config.role)
    {
        case HEARTBEAT:
            platform_core1_start(app_heartbeat);
            break;
    }
    
    drivers_config(config.adc_channel);

    app_config_t app_config_data = {
        .loop_delay = 500
    };

    app_config(app_config_data);

    return SYS_OK;
}

void system_start(void)
{
    // start drivers needed on Core 0
    // launch Core 1 via platform primitive
    // enter main

    sched_run();
}

void system_stop(void)
{
}
