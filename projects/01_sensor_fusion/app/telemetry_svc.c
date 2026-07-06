#include <stdio.h>
#include <stdarg.h>
#include "telemetry_svc_types.h"
#include "heartbeat_svc.h"
#include "setpoint_svc.h"
#include "platform_serial.h"

static telemetry_sink_t serial_telemetry_sink;

static bool telemetry_append(char *buffer, size_t capacity, size_t *used, char const *format, ...)
{
    if ((buffer == NULL) || (used == NULL) || (format == NULL) || (*used >= capacity))
    {
        return false;
    }

    va_list args;
    va_start(args, format);
    int written = vsnprintf(&buffer[*used], capacity - *used, format, args);
    va_end(args);

    if ((written < 0) || ((size_t)written >= (capacity - *used)))
    {
        return false;
    }

    *used += (size_t)written;
    return true;
}

static bool serial_write(uint8_t const *data, size_t len)
{
    if ((data == NULL) || (len == 0u))
    {
        return false;
    }

    char buffer[256];
    size_t copy_len = (len < (sizeof(buffer) - 1u)) ? len : (sizeof(buffer) - 1u);

    for (size_t i = 0u; i < copy_len; ++i)
    {
        buffer[i] = (char)data[i];
    }

    buffer[copy_len] = '\0';
    platform_serial_println(buffer);

    return (copy_len == len);
}

void telemetry_svc_init(void)
{
    serial_telemetry_sink.write = serial_write;
}

void telemetry_svc_publish(void)
{
    telemetry_snapshot_t telemetry;
    heartbeat_svc_get_last(&telemetry.heartbeat);
    setpoint_svc_get_last(&telemetry.setpoint);

    char buffer[512];
    size_t used = 0;

    bool formatted = true;
    formatted &= telemetry_append(buffer, sizeof(buffer), &used, "========== TELEMETRY BEGIN ==========" "\n");
    formatted &= telemetry_append(buffer, sizeof(buffer), &used, "heartbeat.last_counter=%lu" "\n", telemetry.heartbeat.last_counter);
    formatted &= telemetry_append(buffer, sizeof(buffer), &used, "heartbeat.last_timestamp_us=%lu" "\n", telemetry.heartbeat.last_timestamp_us);
    formatted &= telemetry_append(buffer, sizeof(buffer), &used, "heartbeat.last_delta_us=%lu" "\n", telemetry.heartbeat.last_delta_us);
    formatted &= telemetry_append(buffer, sizeof(buffer), &used, "heartbeat.received_count=%lu" "\n", telemetry.heartbeat.received_count);
    formatted &= telemetry_append(buffer, sizeof(buffer), &used, "heartbeat.sequence_error_count=%lu" "\n", telemetry.heartbeat.sequence_error_count);
    formatted &= telemetry_append(buffer, sizeof(buffer), &used, "heartbeat.valid=%u" "\n", telemetry.heartbeat.valid ? 1u : 0u);
    formatted &= telemetry_append(buffer, sizeof(buffer), &used, "setpoint.timestamp_us=%lu" "\n", telemetry.setpoint.timestamp_us);
    formatted &= telemetry_append(buffer, sizeof(buffer), &used, "setpoint.raw_12bit=%u" "\n", (unsigned)telemetry.setpoint.raw_12bit);
    formatted &= telemetry_append(buffer, sizeof(buffer), &used, "setpoint.millivolt=%u" "\n", (unsigned)telemetry.setpoint.millivolt);
    formatted &= telemetry_append(buffer, sizeof(buffer), &used, "setpoint.valid=%u" "\n", telemetry.setpoint.valid ? 1u : 0u);
    formatted &= telemetry_append(buffer, sizeof(buffer), &used, "========== TELEMETRY END ==========" "\n");

    if (!formatted)
    {
        int fallback_chars = snprintf(buffer, sizeof(buffer), "========== TELEMETRY BEGIN ==========" "\n"
            "telemetry.format_error=1" "\n"
            "========== TELEMETRY END ==========" "\n");
        used = (fallback_chars > 0) ? (size_t)fallback_chars : 0u;
    }

    bool success = serial_telemetry_sink.write((uint8_t const *)buffer, used);
    if (!success)
    {
        // update attempts, success, failure counters
    }
}

void telemetry_svc_stop(void)
{

}