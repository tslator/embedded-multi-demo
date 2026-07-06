#ifndef HEARTBEAT_SVC_TYPES_H
#define HEARTBEAT_SVC_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint32_t last_counter;
    uint32_t last_timestamp_us;
    uint32_t last_delta_us;
    uint32_t received_count;
    uint32_t sequence_error_count;
    bool valid;
} heartbeat_status_t;

#endif