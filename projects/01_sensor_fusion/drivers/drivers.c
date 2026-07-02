#include "drivers.h"

#include "platform_led.h"

void drivers_init(void)
{
    platform_led_init();
}
