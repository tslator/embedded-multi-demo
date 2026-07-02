#ifndef DRIVER_IMU_H
#define DRIVER_IMU_H

#include "imu_types.h"

int *_init(imu_driver_t* instance, const imu_config_t* config);
int *_start(imu_driver_t* instance);
int *_read(imu_driver_t* instance, imu_sample_t* sample);
void *_stop(imu_driver_t* instance);

#endif