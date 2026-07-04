#ifndef DRIVER_ADC_H
#define DRIVER_ADC_H

#include "adc_types.h"

adc_status_t adc_init(adc_driver_t * const instance, adc_config_t const * const config);
adc_status_t adc_start(adc_driver_t* const instance);
adc_status_t adc_read(adc_driver_t* const instance, adc_sample_t * const sample);
void adc_stop(adc_driver_t* const instance);

#endif