# IMU driver component CMakeLists.txt

set(IMU_SRCS 
    "inv_imu_driver.c"
    "inv_imu_driver_advanced.c"
    "inv_imu_driver_aux1.c"
    "inv_imu_edmp.c"
    "inv_imu_selftest.c"
    "inv_imu_transport.c"
   
)

set(IMU_INCLUDES
    "."
)

idf_component_register(
    SRCS ${IMU_SRCS}
    INCLUDE_DIRS ${IMU_INCLUDES}
    # If this component depends on other components, list them here
    # REQUIRES other_component1 other_component2
)