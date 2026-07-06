#ifndef SETPOINT_SVC
#define SETPOINT_SVC

#include "setpoint_svc_types.h"

void setpoint_svc_init(void);
bool setpoint_svc_update(void);
bool setpoint_svc_get_last(setpoint_input_t *sample);
void setpoint_svc_stop(void);

#endif
