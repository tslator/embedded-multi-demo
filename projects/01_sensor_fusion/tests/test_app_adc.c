#include "unity.h"

#include "app.h"
#include "mock_heartbeat_svc.h"
#include "mock_setpoint_svc.h"
#include "mock_platform.h"
#include "mock_platform_delay.h"
#include "mock_platform_serial.h"
#include "mock_led.h"
#include "mock_telemetry_svc.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_app_init_initializes_setpoint_service(void)
{
    setpoint_svc_init_Expect();
    heartbeat_svc_init_Expect();

    app_init();
}

void test_app_config_updates_loop_delay(void)
{
    app_config_t config = {
        .loop_delay = 10u,
        .heartbeat_led_enabled = true
    };

    heartbeat_svc_set_led_enabled_Expect(true);

    app_config(config);
}

void test_app_setpoint_update_invokes_setpoint_service(void)
{
    setpoint_svc_update_ExpectAndReturn(true);

    app_setpoint_update();
}

void test_app_stop_stops_setpoint_service(void)
{
    setpoint_svc_stop_Expect();
    heartbeat_svc_stop_Expect();

    app_stop();
}