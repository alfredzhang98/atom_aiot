#ifndef _USER_ICM45686_H_
#define _USER_ICM45686_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Raw accelerometer and gyroscope data (LSB units).
 */
typedef struct {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
} imu_raw_t;

/**
 * Real-world accelerometer (g) and gyroscope (dps) data.
 */
typedef struct {
    float ax, ay, az;
    float gx, gy, gz;
} imu_real_t;

/**
 * Initialize the ICM-45686 sensor over SPI.
 * Must be called before any other imu_icm45686_* functions.
 * @return 0 on success, non-zero error code on failure.
 */
int imu_icm45686_init(void);

/**
 * Read the latest raw sensor data from the ICM-45686.
 * @param[out] raw  Pointer to imu_raw_t to be filled.
 * @return 0 on success, non-zero error code on failure.
 */
int imu_icm45686_get_latest_raw(imu_raw_t *raw);

/**
 * Read the latest real-world sensor data from the ICM-45686.
 * @param[out] real  Pointer to imu_real_t to be filled.
 * @return 0 on success, non-zero error code on failure.
 */
int imu_icm45686_get_latest_real(imu_real_t *real);

#ifdef __cplusplus
}
#endif

#endif // _USER_ICM45686_H_
