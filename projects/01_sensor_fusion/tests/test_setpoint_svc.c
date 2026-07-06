#include "unity.h"

#include "setpoint_svc.h"
#include "setpoint_svc_types.h"
#include "mock_drivers.h"

void setUp(void)
{
    setpoint_svc_init();
}

void tearDown(void)
{
}

void test_setpoint_svc_get_last_returns_false_when_sample_pointer_is_null(void)
{
    TEST_ASSERT_FALSE(setpoint_svc_get_last(nullptr));
}

void test_setpoint_svc_update_caches_latest_driver_sample(void)
{
    setpoint_input_t sample = {0};
    drivers_adc_sample_t driver_sample = {
        .timestamp_us = 250u,
        .raw_12bit = 1024u,
        .millivolt = 825u,
        .valid = true
    };

    drivers_adc_sample_ExpectAnyArgsAndReturn(true);
    drivers_adc_sample_ReturnThruPtr_sample(&driver_sample);

    TEST_ASSERT_TRUE(setpoint_svc_update());
    TEST_ASSERT_TRUE(setpoint_svc_get_last(&sample));
    TEST_ASSERT_EQUAL_UINT32(driver_sample.timestamp_us, sample.timestamp_us);
    TEST_ASSERT_EQUAL_UINT16(driver_sample.raw_12bit, sample.raw_12bit);
    TEST_ASSERT_EQUAL_UINT16(driver_sample.millivolt, sample.millivolt);
}

void test_setpoint_svc_update_resets_cached_sample_on_driver_failure(void)
{
    setpoint_input_t sample = {
        .timestamp_us = 1u,
        .raw_12bit = 1u,
        .millivolt = 1u,
        .valid = true
    };

    drivers_adc_sample_ExpectAnyArgsAndReturn(false);

    TEST_ASSERT_FALSE(setpoint_svc_update());
    TEST_ASSERT_FALSE(setpoint_svc_get_last(&sample));
    TEST_ASSERT_EQUAL_UINT32(0u, sample.timestamp_us);
    TEST_ASSERT_EQUAL_UINT16(0u, sample.raw_12bit);
    TEST_ASSERT_EQUAL_UINT16(0u, sample.millivolt);
    TEST_ASSERT_FALSE(sample.valid);
}

void test_setpoint_svc_stop_stops_underlying_driver_service(void)
{
    drivers_adc_stop_Expect();

    setpoint_svc_stop();
}
