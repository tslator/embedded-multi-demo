#ifndef DRIVER_ENCODER_H
#define DRIVER_ENCODER_H

#include "encoder_types.h"

int encoder_init(encoder_driver_t* instance, const encoder_config_t* config);
int encoder_start(encoder_driver_t* instance);
int encoder_read(encoder_driver_t* instance, encoder_sample_t* sample);
void encoder_stop(encoder_driver_t* instance);

#endif