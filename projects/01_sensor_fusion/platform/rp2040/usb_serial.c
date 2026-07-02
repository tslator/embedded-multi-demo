#include <stdio.h>

#include "platform_serial.h"

void serial_print(char const * const text)
{
    printf("%s", text);
}

void serial_println(char const * const text)
{
    printf("%s\n", text);
}
