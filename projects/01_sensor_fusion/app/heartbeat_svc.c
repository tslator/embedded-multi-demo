#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <memory.h>
#include "heartbeat_svc.h"
#include "platform/platform.h"
#include "platform/platform_serial.h"



static volatile bool hold_in_reset = true;

void app_heartbeat(void)
{
    char msg[64];
    memset(msg, 0, sizeof(msg));

    while (hold_in_reset) {}

    while (true)
    {
        uint32_t value = platform_fifo_pop_u32_blocking();
        int n = snprintf(msg, sizeof(msg), "Heartbeat got: %lu", value);
        assert(n == sizeof(msg));
        platform_serial_println(msg);
        // serial_println("looping ...");
    }

}

void app_heartbeat_start(void)
{
    hold_in_reset = false;
}
