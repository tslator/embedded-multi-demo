#ifndef DRIVERS_H
#define DRIVERS_H

#include <stdint.h>

void drivers_init(void);
void drivers_config(uint8_t adc_channel);

#endif