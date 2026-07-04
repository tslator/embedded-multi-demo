#ifndef DRIVER_ADC_TYPES_H
#define DRIVER_ADC_TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    ADC_OK,
    ADC_ERR_NOT_INIT,
    ADC_ERR_NOT_STARTED,
    ADC_ERR_BAD_ARG,
    ADC_ERR_PLATFORM
} adc_status_t;

typedef struct 
{
    uint8_t channel;
    uint8_t vref_millivolt;
    uint8_t oversample_count;
    bool as_millivolt;
} adc_config_t;

typedef struct 
{
    uint32_t timestamp_us;
    uint16_t raw_12bit;
    uint16_t millivolt;
    bool valid;
} adc_sample_t;

typedef struct 
{
    bool initialized;
    bool started;
    adc_config_t config;
    adc_sample_t last_sample;
} adc_driver_t;

#endif