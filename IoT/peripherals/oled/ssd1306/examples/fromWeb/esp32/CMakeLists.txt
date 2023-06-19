set(srcs "spi_oled_main.c"
    "oled_port.c"
    "oled.c"
    )

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
					)
