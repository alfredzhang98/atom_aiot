/* system_interface.c – ESP‑IDF board glue for ICM‑45686 driver
 * provides si_*  functions expected by InvenSense examples           */
#include "system_interface.h"
#include <string.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"   // for esp_rom_delay_us()
#include "imu/inv_imu_defs.h"

/* ---------- user config pins from sdkconfig ---------------------------- */
#ifndef IMU_CS
#define IMU_CS    CONFIG_IMU_CS
#endif
#ifndef IMU_SCLK
#define IMU_SCLK  CONFIG_IMU_SCLK
#endif
#ifndef IMU_MOSI
#define IMU_MOSI  CONFIG_IMU_MOSI
#endif
#ifndef IMU_MISO
#define IMU_MISO  CONFIG_IMU_MISO
#endif
#ifndef IMU_INT
#define IMU_INT   CONFIG_IMU_INT
#endif

static const char *TAG = "ICM45686_IF";
static spi_device_handle_t imu_spi = NULL;

static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

// GPIO ISR callback storage
static void (*user_int_cb)(void*, unsigned) = NULL;
static unsigned       user_int_num;

static void IRAM_ATTR gpio_isr_handler(void* arg){
    if(user_int_cb) user_int_cb(NULL, user_int_num);
}

int si_board_init(void)
{
    // No board-specific init needed for ESP32
    return 0;
}

int si_config_uart_for_print(inv_uart_num_t id, int level)
{
    // Use ESP_LOG with appropriate level
    esp_log_level_set(TAG, level);
    ESP_LOGI(TAG, "Logging initialized at level %d", level);
    return 0;
}

int si_config_uart_for_bin(inv_uart_num_t id)
{
    // Binary UART not used when logging via ESP_LOG
    ESP_LOGW(TAG, "Binary UART disabled in this interface");
    return 0;
}

int si_get_uart_command(inv_uart_num_t id, char *cmd)
{
    // Implement if command input via UART is required
    return 0;
}

int si_io_imu_init(inv_imu_serif_type_t serif_type)
{
    if(serif_type != UI_SPI4)
        return INV_IMU_ERROR_BAD_ARG;

    spi_bus_config_t buscfg = {
        .mosi_io_num = IMU_MOSI,
        .miso_io_num = IMU_MISO,
        .sclk_io_num = IMU_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 64
    };
    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 10 * 1000 * 1000,
        .mode           = 0,
        .spics_io_num   = IMU_CS,
        .queue_size     = 1,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &devcfg, &imu_spi));
    ESP_LOGI(TAG, "SPI device for ICM-45686 configured");
    return 0;
}

int si_io_imu_read_reg(uint8_t reg, uint8_t *buf, uint32_t len)
{
    if(!imu_spi) return INV_IMU_ERROR;

    uint8_t *tx = heap_caps_malloc(len + 1, MALLOC_CAP_DMA);
    uint8_t *rx = heap_caps_malloc(len + 1, MALLOC_CAP_DMA);
    if(!tx || !rx) {
        free(tx); free(rx);
        return INV_IMU_ERROR;
    }

    tx[0] = reg | 0x80;
    memset(tx + 1, 0, len);

    spi_transaction_t t = {
        .length    = 8 * (len + 1),
        .tx_buffer = tx,
        .rx_buffer = rx
    };
    esp_err_t ret = spi_device_transmit(imu_spi, &t);
    if(ret != ESP_OK){
        free(tx); free(rx);
        return INV_IMU_ERROR_TRANSPORT;
    }

    memcpy(buf, rx + 1, len);
    free(tx); free(rx);
    return 0;
}

int si_io_imu_write_reg(uint8_t reg, const uint8_t *buf, uint32_t len)
{
    if(!imu_spi) return INV_IMU_ERROR;

    uint8_t *tx = heap_caps_malloc(len + 1, MALLOC_CAP_DMA);
    if(!tx) return INV_IMU_ERROR;

    tx[0] = reg & 0x7F;
    memcpy(tx + 1, buf, len);

    spi_transaction_t t = {
        .length    = 8 * (len + 1),
        .tx_buffer = tx,
        .rx_buffer = NULL
    };
    esp_err_t ret = spi_device_transmit(imu_spi, &t);
    free(tx);
    return (ret == ESP_OK ? 0 : INV_IMU_ERROR_TRANSPORT);
}

int si_init_timers(void)
{
    // esp_timer is already running
    return 0;
}

void si_sleep_us(uint32_t us)
{
    esp_rom_delay_us(us);
}

uint64_t si_get_time_us(void)
{
    return esp_timer_get_time();
}

int si_init_gpio_int(unsigned int_num, void (*int_cb)(void *context, unsigned int_num))
{
    user_int_cb  = int_cb;
    user_int_num = int_num;

    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << IMU_INT,
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_NEGEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(IMU_INT, gpio_isr_handler, NULL));
    return 0;
}

int si_start_gpio_fsync(uint32_t freq, void (*fsync_timer_cb)(void *context)) { return 0; }
int si_stop_gpio_fsync(void) { return 0; }
void si_toggle_gpio_fsync(void) {}

void si_disable_irq(void) { portENTER_CRITICAL(&spinlock); }
void si_enable_irq(void)  { portEXIT_CRITICAL(&spinlock); }

int si_print_error_if_any(int rc)
{
    if(rc){
        ESP_LOGE(TAG, "Driver error %d", rc);
    }
    return rc;
}
