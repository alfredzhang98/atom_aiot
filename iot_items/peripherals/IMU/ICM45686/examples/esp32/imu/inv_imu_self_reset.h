#ifndef INV_IMU_SELF_RESET_H
#define INV_IMU_SELF_RESET_H

#include "imu/inv_imu_driver.h"

/**
 * @brief 自定义实现的软复位函数，直接操作寄存器，尝试绕过原始驱动的复位问题
 * 
 * @param s 设备实例
 * @return int 0表示成功，负值表示错误
 */
int inv_imu_self_reset(inv_imu_device_t *s);

#endif /* INV_IMU_SELF_RESET_H */
