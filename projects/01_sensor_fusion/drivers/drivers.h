#ifndef DRIVERS_H
#define DRIVERS_H

#include <stdbool.h>
#include <stdint.h>

#include "system_types.h"

typedef struct
{
	uint32_t timestamp_us;
	uint16_t raw_12bit;
	uint16_t millivolt;
	bool valid;
} drivers_adc_sample_t;

void drivers_init(void);
bool drivers_adc_config(system_adc_config_t const *config);
bool drivers_adc_sample(drivers_adc_sample_t *sample);
void drivers_adc_stop(void);

#endif