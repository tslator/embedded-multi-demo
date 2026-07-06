#include "unity.h"

#include "adc.h"
#include "mock_platform_adc.h"
#include "mock_platform_delay.h"

static adc_driver_t driver;
static adc_config_t config;

void setUp(void)
{
    driver = (adc_driver_t){0};
    config = (adc_config_t){
        .channel = 2u,
        .vref_millivolt = 3300u,
        .oversample_count = 2u,
        .as_millivolt = true
    };
}

void tearDown(void)
{
}

void test_adc_init_rejects_invalid_arguments(void)
{
    TEST_ASSERT_EQUAL(ADC_ERR_BAD_ARG, adc_init(nullptr, &config));
    TEST_ASSERT_EQUAL(ADC_ERR_BAD_ARG, adc_init(&driver, nullptr));

    config.oversample_count = 0u;
    TEST_ASSERT_EQUAL(ADC_ERR_BAD_ARG, adc_init(&driver, &config));
}

void test_adc_init_configures_platform_and_caches_config(void)
{
    platform_adc_init_Expect();
    platform_adc_select_channel_Expect(config.channel);

    TEST_ASSERT_EQUAL(ADC_OK, adc_init(&driver, &config));
    TEST_ASSERT_TRUE(driver.initialized);
    TEST_ASSERT_FALSE(driver.started);
    TEST_ASSERT_EQUAL_UINT16(config.vref_millivolt, driver.config.vref_millivolt);
    TEST_ASSERT_EQUAL_UINT8(config.oversample_count, driver.config.oversample_count);
}

void test_adc_read_requires_started_driver(void)
{
    adc_sample_t sample = {0};

    TEST_ASSERT_EQUAL(ADC_ERR_NOT_INIT, adc_read(&driver, &sample));

    platform_adc_init_Expect();
    platform_adc_select_channel_Expect(config.channel);
    TEST_ASSERT_EQUAL(ADC_OK, adc_init(&driver, &config));

    TEST_ASSERT_EQUAL(ADC_ERR_NOT_STARTED, adc_read(&driver, &sample));
}

void test_adc_read_averages_samples_and_converts_to_millivolts(void)
{
    adc_sample_t sample = {0};

    platform_adc_init_Expect();
    platform_adc_select_channel_Expect(config.channel);
    TEST_ASSERT_EQUAL(ADC_OK, adc_init(&driver, &config));
    TEST_ASSERT_EQUAL(ADC_OK, adc_start(&driver));

    platform_adc_read_raw12bit_ExpectAndReturn(4095u);
    platform_adc_read_raw12bit_ExpectAndReturn(4095u);
    platform_time_now_ExpectAndReturn(1234u);

    TEST_ASSERT_EQUAL(ADC_OK, adc_read(&driver, &sample));
    TEST_ASSERT_TRUE(sample.valid);
    TEST_ASSERT_EQUAL_UINT32(1234u, sample.timestamp_us);
    TEST_ASSERT_EQUAL_UINT16(4095u, sample.raw_12bit);
    TEST_ASSERT_EQUAL_UINT16(3300u, sample.millivolt);
    TEST_ASSERT_EQUAL_UINT16(sample.raw_12bit, driver.last_sample.raw_12bit);
}

void test_adc_stop_clears_started_flag(void)
{
    platform_adc_init_Expect();
    platform_adc_select_channel_Expect(config.channel);
    TEST_ASSERT_EQUAL(ADC_OK, adc_init(&driver, &config));
    TEST_ASSERT_EQUAL(ADC_OK, adc_start(&driver));

    adc_stop(&driver);

    TEST_ASSERT_FALSE(driver.started);
}