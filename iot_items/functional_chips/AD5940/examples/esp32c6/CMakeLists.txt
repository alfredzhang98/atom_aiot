idf_component_register(SRCS "BodyImpedance.c"
                       "spi_ad5940_port.c"
                       "ad5940lib/ad5940.c"
                       INCLUDE_DIRS "." "ad5940lib/"
                       PRIV_REQUIRES driver esp_timer)