#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint16_t loop_delay;
    bool heartbeat_led_enabled;
} app_config_t;


#endif