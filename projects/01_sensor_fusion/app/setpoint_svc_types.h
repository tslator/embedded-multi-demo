#ifndef SETPOINT_SVC_TYPES_H
#define SETPOINT_SVC_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint32_t timestamp_us;
    uint16_t raw_12bit;
    uint16_t millivolt;
    bool valid;
} setpoint_input_t;

#endif