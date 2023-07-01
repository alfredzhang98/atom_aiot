set(srcs "main.c"
    "oled_ssd1306.c"
    "ssd1306_spi_interface.c"
    )

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ".")

