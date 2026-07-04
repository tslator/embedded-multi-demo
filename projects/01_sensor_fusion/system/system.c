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


static system_config_t default_config = {
    .adc_channel = 0,
    .heartbeat_svc = {ROLE_HEARTBEAT, app_heartbeat}
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
    system_config_t *source_config = nullptr;
    size_t target_config_size = 0;

    if (config != nullptr)
    {
        source_config = config;
        target_config_size = sizeof(*config);
    }
    else
    {
        source_config = &default_config;
        target_config_size = sizeof(default_config);
    }

    sys_config = malloc(target_config_size);
    assert(sys_config != nullptr);
    memset(sys_config, 0, target_config_size);
    memcpy(sys_config, source_config, target_config_size);

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

    platform_core1_start(sys_config->heartbeat_svc.service_func);

    app_heartbeat_start();
    sched_run();
}

void system_stop(void)
{
}
