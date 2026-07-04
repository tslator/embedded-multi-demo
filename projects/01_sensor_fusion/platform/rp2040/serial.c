#include <stdio.h>

#include "platform_serial.h"

void platform_serial_print(char const * const text)
{
    printf("%s", text);
}

void platform_serial_println(char const * const text)
{
    printf("%s\n", text);
}
