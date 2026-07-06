#include <memory.h>
#include "app.h"
#include "drivers.h"
#include "setpoint_svc.h"
#include "drivers.h"

static setpoint_input_t setpoint_input;

void setpoint_svc_init(void)
{
    memset(&setpoint_input, 0, sizeof(setpoint_input));
}

bool setpoint_svc_update(void)
{
    drivers_adc_sample_t driver_sample;

    if (!drivers_adc_sample(&driver_sample))
    {
        setpoint_svc_init();
        return false;
    }

    setpoint_input.timestamp_us = driver_sample.timestamp_us;
    setpoint_input.raw_12bit = driver_sample.raw_12bit;
    setpoint_input.millivolt = driver_sample.millivolt;
    setpoint_input.valid = driver_sample.valid;

    return setpoint_input.valid;
}

bool setpoint_svc_get_last(setpoint_input_t *sample)
{
    if (sample == nullptr)
    {
        return false;
    }

    *sample = setpoint_input;
    return setpoint_input.valid;
}

void setpoint_svc_stop(void)
{
    drivers_adc_stop();
}