#ifndef DRIVER_ADC_H
#define DRIVER_ADC_H

#include "adc_types.h"

int adc_init(adc_driver_t* instance, adc_config_t* config);
int adc_start(adc_driver_t* instance);
int adc_read(adc_driver_t* instance, adc_sample_t* sample);
void adc_stop(adc_driver_t* instance);

#endif