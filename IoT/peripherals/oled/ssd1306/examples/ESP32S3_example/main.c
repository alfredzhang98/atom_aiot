#include <oled_ssd1306.h>

void app_main(void)
{
    printf("Hello world!\n");
	oledSSD1603Setup();
	oledTest();

	// while (1){

    //     vTaskDelay(100 / portTICK_PERIOD_MS);

    // }
}
