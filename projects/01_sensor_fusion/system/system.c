#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include "app.h"
#include "drivers.h"
#include "platform.h"
#include "system.h"
#include "system_types.h"
#include "app_types.h"
#include "scheduler.h"
#include "heartbeat_svc.h"
#include "nullptr_compat.h"


static system_config_t default_config = {
    .adc = {
        .channel = 0u,
        .vref_millivolt = 3300u,
        .oversample_count = 1u,
        .as_millivolt = true
    },
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
    assert(sys_config->heartbeat_svc.role == ROLE_HEARTBEAT);
    assert(drivers_adc_config(&sys_config->adc));

    app_config_t app_config_data = {
        .loop_delay = 500
    };

    app_config(app_config_data);

    return SYS_OK;
}

void system_start(void)
{
    assert(sys_config != nullptr);

    switch (sys_config->heartbeat_svc.role)
    {
        case ROLE_HEARTBEAT:
        default:
            platform_core1_start(heartbeat_svc_core1_loop);
            break;
    }

    sched_run();
}

void system_stop(void)
{
}
