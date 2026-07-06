#include "adc.h"
#include "platform_adc.h"
#include "platform_delay.h"
#include "nullptr_compat.h"

static uint16_t adc_raw_to_millivolt(uint16_t raw_12bit, uint16_t vref_millivolt)
{
    uint32_t scaled = ((uint32_t)raw_12bit * (uint32_t)vref_millivolt) + 2047u;
    return (uint16_t)(scaled / 4095u);
}

adc_status_t adc_init(adc_driver_t* const instance, adc_config_t const * const config)
{
    if ((instance == nullptr) || (config == nullptr))
    {
        return ADC_ERR_BAD_ARG;
    }

    if ((config->channel >= 8u) ||
        (config->vref_millivolt == 0u) ||
        (config->oversample_count == 0u))
    {
        return ADC_ERR_BAD_ARG;
    }

    platform_adc_init();
    platform_adc_select_channel(config->channel);

    instance->initialized = true;
    instance->started = false;
    instance->config = *config;
    instance->last_sample.timestamp_us = 0u;
    instance->last_sample.raw_12bit = 0u;
    instance->last_sample.millivolt = 0u;
    instance->last_sample.valid = false;

    return ADC_OK;
}

adc_status_t adc_start(adc_driver_t* const instance)
{
    if (instance == nullptr)
    {
        return ADC_ERR_BAD_ARG;
    }

    if (!instance->initialized)
    {
        return ADC_ERR_NOT_INIT;
    }

    instance->started = true;
    return ADC_OK;
}

adc_status_t adc_read(adc_driver_t* const instance, adc_sample_t* const sample)
{
    if ((instance == nullptr) || (sample == nullptr))
    {
        return ADC_ERR_BAD_ARG;
    }

    if (!instance->initialized)
    {
        return ADC_ERR_NOT_INIT;
    }

    if (!instance->started)
    {
        return ADC_ERR_NOT_STARTED;
    }

    uint32_t accumulator = 0u;

    for (uint8_t read_count = 0u; read_count < instance->config.oversample_count; ++read_count)
    {
        accumulator += platform_adc_read_raw12bit();
    }

    sample->timestamp_us = platform_time_now();
    sample->raw_12bit = (uint16_t)(accumulator / (uint32_t)instance->config.oversample_count);
    sample->millivolt = instance->config.as_millivolt
        ? adc_raw_to_millivolt(sample->raw_12bit, instance->config.vref_millivolt)
        : 0u;
    sample->valid = true;

    instance->last_sample = *sample;

    return ADC_OK;
}

void adc_stop(adc_driver_t* const instance)
{
    if (instance != nullptr)
    {
        instance->started = false;
    }
}