#ifndef TELEMETRY_SVC_H
#define TELEMETRY_SVC_H

#include "telemetry_svc_types.h"

void telemetry_svc_init(void);
void telemetry_svc_publish(void);
void telemetry_svc_stop(void);

#endif