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
    ROLE_HEARTBEAT
} system_service_role_t;

typedef struct 
{
    system_service_role_t role;
    CORE1_FUNCTION service_func;
} system_service_t;

typedef struct 
{
    uint8_t adc_channel;
    system_service_t heartbeat_svc;
} system_config_t;

#endif