#include <stdlib.h>
#include <assert.h>
#include "system.h"
#include "app.h"

int main()
{
    system_status_t status = system_init();
    assert(status == SYS_OK);

    system_config_t config = system_get_default_config();
    status = system_config(config);
    assert(status == SYS_OK);

    system_start();
    system_stop();
}