#include <stdlib.h>
#include <assert.h>
#include "system.h"
#include "app.h"

int main()
{
    system_status_t status = system_init();
    assert(status == SYS_OK);

    status = system_config(nullptr);
    assert(status == SYS_OK);

    system_start();
    system_stop();
}