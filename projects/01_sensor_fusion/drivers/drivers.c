#include "drivers.h"

#include "platform_led.h"
#include "platform_adc.h"

void drivers_init(void)
{
    platform_led_init();
}

void drivers_config(uint8_t adc_channel)
{
    platform_adc_init();
    platform_adc_select_channel(adc_channel);
}
