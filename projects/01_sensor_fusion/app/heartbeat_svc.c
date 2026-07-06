#include <memory.h>

#include "heartbeat_svc.h"
#include "platform.h"
#include "platform_delay.h"
#include "nullptr_compat.h"
#include "led.h"


static heartbeat_status_t heartbeat_status;
static bool heartbeat_running = false;
static uint32_t heartbeat_counter = 0u;
static bool heartbeat_led_enabled = true;

void heartbeat_svc_init(void)
{
    memset(&heartbeat_status, 0, sizeof(heartbeat_status));
    heartbeat_running = true;
    heartbeat_counter = 0u;
}

void heartbeat_svc_set_led_enabled(bool enabled)
{
    heartbeat_led_enabled = enabled;
}

bool heartbeat_svc_publish_next(void)
{
    if (!heartbeat_running)
    {
        return false;
    }

    platform_fifo_push_u32(heartbeat_counter);
    heartbeat_counter++;

    if (heartbeat_counter > 10u)
    {
        heartbeat_counter = 0u;
    }

    return true;
}

bool heartbeat_svc_process_value(uint32_t counter_value)
{
    if (!heartbeat_running)
    {
        return false;
    }

    uint32_t now_us = platform_time_now();

    if (heartbeat_status.valid)
    {
        uint32_t expected_next = heartbeat_status.last_counter + 1u;
        if (counter_value != expected_next)
        {
            heartbeat_status.sequence_error_count++;
        }

        heartbeat_status.last_delta_us = now_us - heartbeat_status.last_timestamp_us;
    }
    else
    {
        heartbeat_status.last_delta_us = 0u;
    }

    heartbeat_status.last_counter = counter_value;
    heartbeat_status.last_timestamp_us = now_us;
    heartbeat_status.received_count++;
    heartbeat_status.valid = true;

    if (heartbeat_led_enabled)
    {
        led_toggle();
    }

    return true;
}

bool heartbeat_svc_get_last(heartbeat_status_t *sample)
{
    if (sample == nullptr)
    {
        return false;
    }

    *sample = heartbeat_status;
    return heartbeat_status.valid;
}

void heartbeat_svc_stop(void)
{
    heartbeat_running = false;
    heartbeat_counter = 0u;
}

void heartbeat_svc_core1_loop(void)
{
    heartbeat_svc_init();

    while (true)
    {
        uint32_t counter_value = platform_fifo_pop_u32_blocking();
        (void)heartbeat_svc_process_value(counter_value);
    }
}
