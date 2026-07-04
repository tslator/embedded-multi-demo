#include "adc.h"
#include "platform_adc.h"

adc_status_t adc_init(adc_driver_t* const instance, adc_config_t const * const config)
{
    platform_adc_init();
    return 0;
}

adc_status_t adc_start(adc_driver_t* const instance)
{
    return 0;
}

adc_status_t adc_read(adc_driver_t* const instance, adc_sample_t* const sample)
{

    return 0;
}

void adc_stop(adc_driver_t* const instance)
{

}