#include "drivers.h"

#include <string.h>

#include "adc.h"
#include "nullptr_compat.h"
#include "platform_led.h"

static adc_driver_t adc_driver;
static volatile bool adc_ready = false;

static adc_config_t drivers_make_adc_config(system_adc_config_t const * const config)
{
    adc_config_t driver_config = {
        .channel = config->channel,
        .vref_millivolt = config->vref_millivolt,
        .oversample_count = config->oversample_count,
        .as_millivolt = config->as_millivolt
    };

    return driver_config;
}

void drivers_init(void)
{
    memset(&adc_driver, 0, sizeof(adc_driver));
    adc_ready = false;
    platform_led_init();
}

bool drivers_adc_config(system_adc_config_t const *config)
{
    bool is_null_config = config == nullptr;
    bool is_invalid_channel = config->channel >= 8u;
    bool is_invalid_vref = config->vref_millivolt == 0u;
    bool is_invalid_oversample_count = config->oversample_count == 0u;

    if (is_null_config || is_invalid_channel || is_invalid_vref || is_invalid_channel)
    {
        return false;
    }

    adc_config_t driver_config = drivers_make_adc_config(config);

    adc_ready = false;

    if ((adc_init(&adc_driver, &driver_config) != ADC_OK) ||
        (adc_start(&adc_driver) != ADC_OK))
    {
        return false;
    }

    adc_ready = true;
    return true;
}

bool drivers_adc_sample(drivers_adc_sample_t *sample)
{
    adc_sample_t driver_sample;

    if ((sample == nullptr) || !adc_ready)
    {
        return false;
    }

    if (adc_read(&adc_driver, &driver_sample) != ADC_OK)
    {
        memset(sample, 0, sizeof(*sample));
        return false;
    }

    sample->timestamp_us = driver_sample.timestamp_us;
    sample->raw_12bit = driver_sample.raw_12bit;
    sample->millivolt = driver_sample.millivolt;
    sample->valid = driver_sample.valid;

    return sample->valid;
}

void drivers_adc_stop(void)
{
    if (adc_ready)
    {
        adc_stop(&adc_driver);
        adc_ready = false;
    }
}
