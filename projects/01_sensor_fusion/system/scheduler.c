#include <stdbool.h>
#include "scheduler.h"
#include "app.h"

void sched_run(void)
{
    // Run the application
    while (true) 
    {
        app_update_count();
        app_update_telemetry();
        app_throttle_loop();
    }
}