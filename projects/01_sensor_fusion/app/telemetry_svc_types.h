#ifndef TELEMETRY_SVC_TYPES_H
#define TELEMETRY_SVC_TYPES_H

#include <stddef.h>
#include "setpoint_svc_types.h"
#include "heartbeat_svc_types.h"


typedef struct
{
    setpoint_input_t setpoint;
    heartbeat_status_t heartbeat;
} telemetry_snapshot_t;

typedef struct
{
    bool (*write)(uint8_t const *data, size_t len);
} telemetry_sink_t;

#endif