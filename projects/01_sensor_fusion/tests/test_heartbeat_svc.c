#include "unity.h"

#include "heartbeat_svc.h"
#include "heartbeat_svc_types.h"
#include "mock_platform.h"
#include "mock_platform_delay.h"

void setUp(void)
{
    heartbeat_svc_init();
}

void tearDown(void)
{
}

void test_heartbeat_svc_get_last_returns_false_when_sample_pointer_is_null(void)
{
    TEST_ASSERT_FALSE(heartbeat_svc_get_last(nullptr));
}

void test_heartbeat_svc_get_last_returns_false_before_first_sample(void)
{
    heartbeat_status_t sample = {0};

    TEST_ASSERT_FALSE(heartbeat_svc_get_last(&sample));
}

void test_heartbeat_svc_process_value_caches_first_sample(void)
{
    heartbeat_status_t sample = {0};

    platform_time_now_ExpectAndReturn(100u);

    TEST_ASSERT_TRUE(heartbeat_svc_process_value(7u));
    TEST_ASSERT_TRUE(heartbeat_svc_get_last(&sample));
    TEST_ASSERT_EQUAL_UINT32(7u, sample.last_counter);
    TEST_ASSERT_EQUAL_UINT32(100u, sample.last_timestamp_us);
    TEST_ASSERT_EQUAL_UINT32(0u, sample.last_delta_us);
    TEST_ASSERT_EQUAL_UINT32(1u, sample.received_count);
    TEST_ASSERT_EQUAL_UINT32(0u, sample.sequence_error_count);
    TEST_ASSERT_TRUE(sample.valid);
}

void test_heartbeat_svc_process_value_tracks_delta_and_sequence_errors(void)
{
    heartbeat_status_t sample = {0};

    platform_time_now_ExpectAndReturn(100u);
    TEST_ASSERT_TRUE(heartbeat_svc_process_value(10u));

    platform_time_now_ExpectAndReturn(150u);
    TEST_ASSERT_TRUE(heartbeat_svc_process_value(12u));

    TEST_ASSERT_TRUE(heartbeat_svc_get_last(&sample));
    TEST_ASSERT_EQUAL_UINT32(12u, sample.last_counter);
    TEST_ASSERT_EQUAL_UINT32(150u, sample.last_timestamp_us);
    TEST_ASSERT_EQUAL_UINT32(50u, sample.last_delta_us);
    TEST_ASSERT_EQUAL_UINT32(2u, sample.received_count);
    TEST_ASSERT_EQUAL_UINT32(1u, sample.sequence_error_count);
}

void test_heartbeat_svc_stop_prevents_processing_updates(void)
{
    heartbeat_svc_stop();

    TEST_ASSERT_FALSE(heartbeat_svc_process_value(1u));
}