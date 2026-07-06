#include "unity.h"

#include "drivers.h"
#include "mock_adc.h"
#include "mock_platform_led.h"

static adc_status_t capture_adc_config(adc_driver_t * const instance, adc_config_t const * const config, int cmock_num_calls)
{
    (void)instance;
    (void)cmock_num_calls;

    TEST_ASSERT_NOT_NULL(config);
    TEST_ASSERT_EQUAL_UINT8(3u, config->channel);
    TEST_ASSERT_EQUAL_UINT16(3300u, config->vref_millivolt);
    TEST_ASSERT_EQUAL_UINT8(4u, config->oversample_count);
    TEST_ASSERT_TRUE(config->as_millivolt);

    return ADC_OK;
}

void setUp(void)
{
}

void tearDown(void)
{
}

void test_drivers_init_initializes_led_platform(void)
{
    platform_led_init_Expect();

    drivers_init();
}

void test_drivers_adc_config_initializes_and_starts_adc_driver(void)
{
    system_adc_config_t config = {
        .channel = 3u,
        .vref_millivolt = 3300u,
        .oversample_count = 4u,
        .as_millivolt = true
    };

    adc_init_StubWithCallback(capture_adc_config);
    adc_start_ExpectAnyArgsAndReturn(ADC_OK);

    TEST_ASSERT_TRUE(drivers_adc_config(&config));
}

void test_drivers_adc_config_rejects_invalid_configuration(void)
{
    system_adc_config_t config = {
        .channel = 3u,
        .vref_millivolt = 3300u,
        .oversample_count = 4u,
        .as_millivolt = true
    };

    TEST_ASSERT_FALSE(drivers_adc_config(nullptr));

    config.channel = 8u;
    TEST_ASSERT_FALSE(drivers_adc_config(&config));

    config.channel = 3u;
    config.vref_millivolt = 0u;
    TEST_ASSERT_FALSE(drivers_adc_config(&config));

    config.vref_millivolt = 3300u;
    config.oversample_count = 0u;
    TEST_ASSERT_FALSE(drivers_adc_config(&config));
}

void test_drivers_adc_sample_reads_and_translates_cached_driver_sample(void)
{
    system_adc_config_t config = {
        .channel = 1u,
        .vref_millivolt = 3300u,
        .oversample_count = 1u,
        .as_millivolt = true
    };
    drivers_adc_sample_t sample = {0};
    adc_sample_t driver_sample = {
        .timestamp_us = 42u,
        .raw_12bit = 777u,
        .millivolt = 626u,
        .valid = true
    };

    adc_init_ExpectAnyArgsAndReturn(ADC_OK);
    adc_start_ExpectAnyArgsAndReturn(ADC_OK);
    TEST_ASSERT_TRUE(drivers_adc_config(&config));

    adc_read_ExpectAnyArgsAndReturn(ADC_OK);
    adc_read_ReturnThruPtr_sample(&driver_sample);

    TEST_ASSERT_TRUE(drivers_adc_sample(&sample));
    TEST_ASSERT_EQUAL_UINT32(driver_sample.timestamp_us, sample.timestamp_us);
    TEST_ASSERT_EQUAL_UINT16(driver_sample.raw_12bit, sample.raw_12bit);
    TEST_ASSERT_EQUAL_UINT16(driver_sample.millivolt, sample.millivolt);
}

void test_drivers_adc_stop_stops_active_driver(void)
{
    system_adc_config_t config = {
        .channel = 1u,
        .vref_millivolt = 3300u,
        .oversample_count = 1u,
        .as_millivolt = true
    };

    adc_init_ExpectAnyArgsAndReturn(ADC_OK);
    adc_start_ExpectAnyArgsAndReturn(ADC_OK);
    TEST_ASSERT_TRUE(drivers_adc_config(&config));

    adc_stop_ExpectAnyArgs();
    drivers_adc_stop();
}