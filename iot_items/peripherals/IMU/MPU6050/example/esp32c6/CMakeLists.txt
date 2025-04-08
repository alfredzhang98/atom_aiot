idf_component_register(SRCS "MPU6050.cpp"
                      "MPU6050_6Axis_MotionApps20.cpp"
                      INCLUDE_DIRS "."
                      PRIV_REQUIRES driver esp_timer
                      REQUIRES I2Cdev)
