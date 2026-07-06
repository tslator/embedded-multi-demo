#ifndef SYSTEM_TYPES_H
#define SYSTEM_TYPES_H

#include <stdbool.h>
#include <stdint.h>

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
    bool led_enabled;
} system_service_t;

typedef struct
{
    uint8_t channel;
    uint16_t vref_millivolt;
    uint8_t oversample_count;
    bool as_millivolt;
} system_adc_config_t;

typedef struct 
{
    system_adc_config_t adc;
    system_service_t heartbeat_svc;
} system_config_t;

#endif