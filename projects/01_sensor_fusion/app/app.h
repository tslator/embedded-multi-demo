/*
The application provides an high-level interface to the operation of the sensor fusion application.  It supplies the following accessors:

    app_init - establishes initial conditions for software layers
    app_config - configures software layers in preparation for execution
    app_run - operates the application
    app_stop - stops the application (typically only for testing purposes)
    app_shutdown - shuts down the application in the event of exception condition
*/
#ifndef APP_H
#define APP_H

#include "app_types.h"

void app_init(void);
void app_config(app_config_t config);
void app_run(void);
void app_stop(void);
void app_shutdown(void);

#endif