#ifndef SYSTEM_TYPES_H
#define SYSTEM_TYPES_H

#include <stdint.h>
#include "platform_types.h"

typedef enum
{
    SYS_OK
} system_status_t;

typedef enum
{
    HEARTBEAT
} system_role_t;

typedef struct 
{
    uint8_t adc_channel;
    system_role_t role;

} system_config_t;

#endif