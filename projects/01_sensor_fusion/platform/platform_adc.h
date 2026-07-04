#ifndef PLATFORM_ADC_H
#define PLATFORM_ADC_H

#include <stdint.h>

void platform_adc_init(void);
void platform_adc_select_channel(uint8_t channel);
uint16_t platform_adc_read_raw12bit(void);

#endif