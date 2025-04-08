# Instructions
spi_ad5940_port.h
spi_ad5940_port.c
There two are the interface related to idf (version is idf 5.3)

---
This is also need to link to the [ad5940 lib](https://github.com/alfredzhang98/ad5940lib/tree/842574b93aabd96f5bec68c3e3e00cec099351f2)


---

Make sure the cmake include realted libs as below


```
idf_component_register(SRCS "BodyImpedance.c"
                       "spi_ad5940_port.c"
                       "ad5940lib/ad5940.c"
                       INCLUDE_DIRS "." "ad5940lib/"
                       PRIV_REQUIRES driver esp_timer)
```
