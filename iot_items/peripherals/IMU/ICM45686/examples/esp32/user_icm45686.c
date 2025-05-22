#include "user_icm45686.h"
#include "system_interface.h"
#include "imu/inv_imu_driver.h"
#include <string.h>

// IMU device handle
static inv_imu_device_t imu_dev;

int imu_icm45686_init(void)
{
    int rc = 0;

    // Board and transport init
    rc |= si_board_init();
    rc |= si_config_uart_for_print(SI_UART_ID_FTDI, INV_MSG_LEVEL_ERROR);
    rc |= si_init_gpio_int(SI_GPIO_INT1, NULL);
    rc |= si_init_timers();
    rc |= si_io_imu_init(UI_SPI4);

    // Link driver transport
    imu_dev.transport.read_reg   = si_io_imu_read_reg;
    imu_dev.transport.write_reg  = si_io_imu_write_reg;
    imu_dev.transport.serif_type = UI_SPI4;
    imu_dev.transport.sleep_us   = si_sleep_us;

    // Delay for power-up
    si_sleep_us(3000);

    // Reset and verify WHOAMI
    uint8_t whoami = 0;
    rc |= inv_imu_get_who_am_i(&imu_dev, &whoami);
    if(rc || whoami != INV_IMU_WHOAMI)
        return -1;
    rc |= inv_imu_soft_reset(&imu_dev);

    // Configure IMU default settings
    // Set full-scale ranges
    rc |= inv_imu_set_accel_fsr(&imu_dev, ACCEL_CONFIG0_ACCEL_UI_FS_SEL_4_G);
    rc |= inv_imu_set_gyro_fsr(&imu_dev, GYRO_CONFIG0_GYRO_UI_FS_SEL_2000_DPS);
    // Set ODR to 50Hz
    rc |= inv_imu_set_accel_frequency(&imu_dev, ACCEL_CONFIG0_ACCEL_ODR_50_HZ);
    rc |= inv_imu_set_gyro_frequency(&imu_dev, GYRO_CONFIG0_GYRO_ODR_50_HZ);
    // Set bandwidth = ODR/4 (Low-Noise)
    rc |= inv_imu_set_accel_ln_bw(&imu_dev, IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_4);
    rc |= inv_imu_set_gyro_ln_bw(&imu_dev, IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_4);
    // Enable sensors in Low-Noise
    rc |= inv_imu_set_accel_mode(&imu_dev, PWR_MGMT0_ACCEL_MODE_LN);
    rc |= inv_imu_set_gyro_mode(&imu_dev, PWR_MGMT0_GYRO_MODE_LN);

    return rc;
}

int imu_icm45686_get_latest_raw(imu_raw_t *raw)
{
    if(!raw)
        return -1;

    inv_imu_sensor_data_t d;
    int rc = inv_imu_get_register_data(&imu_dev, &d);
    if(rc)
        return rc;

    raw->ax = d.accel_data[0];
    raw->ay = d.accel_data[1];
    raw->az = d.accel_data[2];
    raw->gx = d.gyro_data[0];
    raw->gy = d.gyro_data[1];
    raw->gz = d.gyro_data[2];

    return 0;
}

int imu_icm45686_get_latest_real(imu_real_t *real)
{
    if(!real)
        return -1;

    imu_raw_t raw;
    int rc = imu_icm45686_get_latest_raw(&raw);
    if(rc)
        return rc;

    // Constants matching init config
    const float accel_fsr = 4.0f;      // g
    const float gyro_fsr  = 2000.0f;   // dps
    const float lsb16     = 32768.0f;

    real->ax = (raw.ax * accel_fsr) / lsb16;
    real->ay = (raw.ay * accel_fsr) / lsb16;
    real->az = (raw.az * accel_fsr) / lsb16;
    real->gx = (raw.gx * gyro_fsr)  / lsb16;
    real->gy = (raw.gy * gyro_fsr)  / lsb16;
    real->gz = (raw.gz * gyro_fsr)  / lsb16;

    return 0;
}
