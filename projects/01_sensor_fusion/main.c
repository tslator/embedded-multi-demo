#include "app.h"

Config config = {
    .led_delay = 500
};

int main()
{
    app_init();
    app_config(config);
    app_run();
}