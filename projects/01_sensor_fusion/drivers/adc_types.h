#ifndef DRIVER_ADC_TYPES_H
#define DRIVER_ADC_TYPES_H


enum {
    ADC_OK,
    ADC_ERR_NOT_INIT,
    ADC_ERR_NOT_STARTED,
    ADC_ERR_BAD_ARG,
    ADC_ERR_PLATFORM
} adc_status_t;

typedef struct {} adc_driver_t;
typedef struct {} adc_config_t;
typedef struct {} adc_sample_t;

#endif