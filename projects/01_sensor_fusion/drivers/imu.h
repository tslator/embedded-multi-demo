#ifndef DRIVER_IMU_H
#define DRIVER_IMU_H

#include "imu_types.h"

int imu_init(imu_driver_t* instance, const imu_config_t* config);
int imu_start(imu_driver_t* instance);
int imu_read(imu_driver_t* instance, imu_sample_t* sample);
void imu_stop(imu_driver_t* instance);

#endif