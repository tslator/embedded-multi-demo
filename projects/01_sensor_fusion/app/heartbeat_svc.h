#ifndef HEARTBEAT_SVC_H
#define HEARTBEAT_SVC_H

#include <stdbool.h>
#include <stdint.h>

#include "heartbeat_svc_types.h"

void heartbeat_svc_init(void);
void heartbeat_svc_set_led_enabled(bool enabled);
bool heartbeat_svc_publish_next(void);
bool heartbeat_svc_process_value(uint32_t counter_value);
bool heartbeat_svc_get_last(heartbeat_status_t *sample);
void heartbeat_svc_stop(void);

void heartbeat_svc_core1_loop(void);

#endif