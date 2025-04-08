#include "spi_ad5940_port.h"

volatile static uint32_t ucInterrupted = 0; 
spi_device_handle_t spi_handle;

// ISR handler for external interrupt
static void IRAM_ATTR Ext_Int0_Handler(void* arg) {
    // uint32_t gpio_num = (uint32_t) arg;
    ucInterrupted = 1;
    // ESP_EARLY_LOGI("AD5940PORT", "GPIO[%d] intr, val: %d\n", gpio_num, gpio_get_level(gpio_num));
}

void AD5940_ReadWriteNBytes(unsigned char *pSendBuffer, unsigned char *pRecvBuff, unsigned long length) {
    if (spi_handle == NULL) {
        ESP_LOGE("AD5940PORT", "SPI device handle is not initialized");
        return;
    }

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       // Zero out the transaction
    t.length = length * 8;          // Length is in bits
    t.tx_buffer = pSendBuffer;      // Data to be sent
    t.rx_buffer = pRecvBuff;        // Buffer to hold received data

    // esp_err_t ret = spi_device_transmit(spi_handle, &t);
    esp_err_t ret = spi_device_polling_transmit(spi_handle, &t);
    if (ret != ESP_OK) {
        ESP_LOGE("AD5940PORT", "Failed to transmit SPI data: %s", esp_err_to_name(ret));
    } else {
        // ESP_LOGI("AD5940PORT", "SPI data transmitted successfully");
    }
}

// GPIO control functions for CS and RST
void AD5940_CsClr(void) {
    gpio_set_level(SPI_CS_AD5940_Pin, 0);
}

void AD5940_CsSet(void) {
    gpio_set_level(SPI_CS_AD5940_Pin, 1);
}

void AD5940_RstSet(void) {
    gpio_set_level(AD5940_ResetPin, 1);
}

void AD5940_RstClr(void) {
    gpio_set_level(AD5940_ResetPin, 0);
}

void AD5940_Delay10us(uint32_t time) {
    esp_rom_delay_us(time * 10);
}

uint32_t AD5940_GetMCUIntFlag(void) {
    return ucInterrupted;
}

uint32_t AD5940_ClrMCUIntFlag(void) {
    ucInterrupted = 0;
    return 1;
}

// SPI initialization function
void spi_init() {
    spi_bus_config_t buscfg = {
        .miso_io_num = SPI_MISO,
        .mosi_io_num = SPI_MOSI,
        .sclk_io_num = SPI_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096
    };

    esp_err_t ret = spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE("AD5940PORT", "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
        return;
    }

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = SPI_CLK,
        .mode = 0,                            // SPI mode 0
        .spics_io_num = SPI_CS_AD5940_Pin,    // CS pin
        .queue_size = 32,                      // Queue 7 transactions at a time
        .pre_cb = NULL,                       // Specify pre-transfer callback to handle D/C line
        .post_cb = NULL,
    };

    ret = spi_bus_add_device(HSPI_HOST, &devcfg, &spi_handle);
    if (ret != ESP_OK) {
        ESP_LOGE("AD5940PORT", "Failed to add SPI device: %s", esp_err_to_name(ret));
        spi_bus_free(HSPI_HOST);
        return;
    }
    ESP_LOGI("AD5940PORT", "SPI device initialized successfully");
}

// GPIO initialization function
void gpio_init() {
    esp_err_t ret;
    // Initialize GPIOs for CS and RST as outputs
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    // io_conf.pin_bit_mask = (1ULL << AD5940_ResetPin);
    io_conf.pin_bit_mask = (1ULL << SPI_CS_AD5940_Pin) | (1ULL << AD5940_ResetPin);
    io_conf.pull_down_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // Initialize INT pin as input with pull-up
    io_conf.intr_type = GPIO_INTR_NEGEDGE; // 上升沿触发中断
    io_conf.pin_bit_mask = (1ULL << AD5940_IntPin);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    // 安装 GPIO 中断服务
    ret = gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
    if (ret != ESP_OK) {
        ESP_LOGE("AD5940PORT", "Failed to install GPIO ISR service: %s", esp_err_to_name(ret));
        return;
    }
    // 添加中断处理程序
    ret = gpio_isr_handler_add(AD5940_IntPin, Ext_Int0_Handler, (void*) AD5940_IntPin);
    if (ret != ESP_OK) {
        ESP_LOGE("AD5940PORT", "Failed to add GPIO ISR handler: %s", esp_err_to_name(ret));
        return;
    }
}

uint32_t AD5940_MCUResourceInit(void *pCfg) {
    // Initialize SPI
    spi_init();
    // Initialize GPIO
    gpio_init();
    AD5940_CsSet();
    AD5940_RstSet();
    return 0;
}
