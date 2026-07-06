#include <stdbool.h>
#include "scheduler.h"
#include "app.h"

void sched_run(void)
{
    // Run the application
    while (true) 
    {
        app_setpoint_update();
        app_heartbeat_publish();
        app_telemetry_publish();
        app_throttle_loop();
    }
}