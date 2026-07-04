#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "app.h"
#include "drivers.h"
#include "platform.h"
#include "platform_adc.h"
#include "system.h"
#include "system_types.h"
#include "app_types.h"
#include "scheduler.h"
#include "heartbeat_svc.h"


static system_config_t default_config = {
    .adc_channel = 0,
    .heartbeat_svc = ROLE_HEARTBEAT
};

static system_config_t *sys_config = nullptr;

system_status_t system_init()
{
    platform_init();
    drivers_init();
    app_init();

    return SYS_OK;
}

system_status_t system_config(system_config_t *config)
{
    sys_config = config == nullptr ? &default_config : config;
    assert(sys_config != nullptr);
    assert(sys_config->adc_channel < 8);
    assert(sys_config->heartbeat_svc.role == ROLE_HEARTBEAT);

    drivers_config(sys_config->adc_channel);

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

    assert(sys_config != nullptr);

    switch (sys_config->heartbeat_svc.role)
    {
        case ROLE_HEARTBEAT:
        default:
            platform_core1_start(app_heartbeat);
            app_heartbeat_start();
            break;
    }

    sched_run();
}

void system_stop(void)
{
}
