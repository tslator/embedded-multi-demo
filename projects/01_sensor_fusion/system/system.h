#ifndef SYSTEM_H
#define SYSTEM_H

#include "system_types.h"

system_status_t system_init();
system_config_t system_get_default_config(void);
system_status_t system_config(system_config_t config);
void system_start(void);
void system_stop(void);

#endif