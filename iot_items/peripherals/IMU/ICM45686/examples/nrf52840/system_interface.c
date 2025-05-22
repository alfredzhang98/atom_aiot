/* Standard includes */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <zephyr/drivers/uart.h>

#include <zephyr/sys_clock.h>  // 必须包含

#include <zephyr/logging/log.h>
 
/* IMU drivers */
#include "imu/inv_imu_defs.h" /* For error codes */
#include "system_interface.h"

/* 
 * Board 
 */
int si_board_init(void)
{
	return 0;
}

/* 
 * UART 
 */
 /* 获取 UART 设备 */
 #define UART_DEV_NODE DT_NODELABEL(uart0)  // 确保与设备树一致
 static const struct device *uart_dev = DEVICE_DT_GET(UART_DEV_NODE);
 
int si_config_uart_for_print(inv_uart_num_t id, int level)
{
	(void)id;
	(void)level;
	return 0;
}

int si_config_uart_for_bin(inv_uart_num_t id)
{
	(void)id;
	return 0;
}

int si_get_uart_command(inv_uart_num_t id, char *cmd)
{
    (void)id; // 目前只支持一个 UART，忽略 id
    if (!cmd) {
        return -EINVAL; // 无效参数
    }

    if (!device_is_ready(uart_dev)) {
        return -ENODEV; // 设备不可用
    }

    int ret = uart_poll_in(uart_dev, (unsigned char *)cmd);
    // if (ret < 0) {
    //     return ret; // 没有数据可读时返回错误码
    // }

    return 0; // 成功读取字符
}

/* 
 * I/O for IMU device 
 */

 /* 获取 SPI 设备 */
#define SPI_DEV_NODE DT_NODELABEL(spi1)  // 确保与设备树一致
const struct device *spi_dev = DEVICE_DT_GET(SPI_DEV_NODE);

/* SPI 配置 */
static struct spi_config spi_cfg = {
    .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPHA | SPI_MODE_CPOL,
    .frequency = 4000000, // 4 MHz
    .slave     = 0,
};

/* 片选信号 */
struct spi_cs_control cs_ctrl = {
    .gpio = GPIO_DT_SPEC_GET(DT_NODELABEL(spi1), cs_gpios),
    .delay = 0,
};

int si_io_imu_init(inv_imu_serif_type_t serif_type)
{
	(void)serif_type;
	spi_cfg.cs = cs_ctrl;
	return 0;
}

/* IMU 相关寄存器 */
#define ICM45686_WHO_AM_I_REG  0x72
#define ICM45686_WHO_AM_I_VAL  0xE9

int si_io_imu_read_reg(uint8_t reg, uint8_t *buf, uint32_t len)
{
    if (!device_is_ready(spi_dev)) {
        printk("Error: SPI device not ready!\n");
        return -1;
    }

    uint8_t tx_buf[1] = {reg | 0x80};  // 读操作，第7位设为1
    uint8_t rx_buf[len + 1];           // 第一个字节是无效的，占位

    struct spi_buf tx_spi_buf = {.buf = tx_buf, .len = sizeof(tx_buf)};
    struct spi_buf rx_spi_buf = {.buf = rx_buf, .len = sizeof(rx_buf)};

    struct spi_buf_set tx_set = {.buffers = &tx_spi_buf, .count = 1};
    struct spi_buf_set rx_set = {.buffers = &rx_spi_buf, .count = 1};

    int ret = spi_transceive(spi_dev, &spi_cfg, &tx_set, &rx_set);
    if (ret < 0) {
        printk("SPI transceive failed: %d\n", ret);
        return ret;
    }

    memcpy(buf, &rx_buf[1], len);  // 去掉第一个占位符字节

    return 0;
}

int si_io_imu_write_reg(uint8_t reg, const uint8_t *buf, uint32_t len)
{
    if (!device_is_ready(spi_dev)) {
        printk("Error: SPI device not ready!\n");
        return -1;
    }

    uint8_t tx_buf[1 + len];  // 1字节的寄存器地址 + 数据
    tx_buf[0] = reg & 0x7F;   // 写操作，第7位设为0
    memcpy(&tx_buf[1], buf, len);

    struct spi_buf tx_spi_buf = {.buf = tx_buf, .len = sizeof(tx_buf)};
    struct spi_buf_set tx_set = {.buffers = &tx_spi_buf, .count = 1};

    int ret = spi_write(spi_dev, &spi_cfg, &tx_set);
    if (ret < 0) {
        printk("SPI write failed: %d\n", ret);
        return ret;
    }

    return 0;
}


/* 
 * Timers 
 */

 static uint64_t start_time = 0;
 
// 定时器回调函数
void timer_callback(struct k_timer *timer_id)
{
    // 这个回调函数可以用于其他的定时任务处理
}
 
// 定时器结构体
static struct k_timer my_timer;

int si_init_timers()
{
    // printk("Initializing timers...\n");
 
    // 获取系统启动时的时间戳
    start_time = k_uptime_get();  // 获取系统启动以来的毫秒数
 
    // 初始化定时器
    k_timer_init(&my_timer, timer_callback, NULL);
 
    // printk("Timer initialized successfully\n");
 
    return 0;
}

void si_sleep_us(uint32_t us)
{
    k_busy_wait(us); 
}

uint64_t si_get_time_us()
{
    uint32_t cycles = sys_clock_cycle_get_32();
    // printk("cycles = %llu\n", cycles);

    uint32_t time_us = k_cyc_to_us_near64(cycles);
    // printk("time_us = %llu\n", time_us);

    return time_us;
}

/*
 * GPIO
 */

/* GPIO 设备 */
#define IMU_INT1_PIN  27  // 修改为实际连接的 IMU INT1 GPIO 引脚
#define IMU_INT1_PORT DT_NODELABEL(gpio0)  // nRF52840 默认 GPIO0

static const struct device *gpio_dev;
static struct gpio_callback imu_int_cb;

/* 全局回调指针 */
static void (*user_int_cb)(void *, unsigned int) = NULL;

/* 中断处理包装函数 */
static void int_cb_wrapper(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    (void)dev;
    (void)pins;

    if (user_int_cb) {
        user_int_cb(NULL, SI_GPIO_INT1);  // 调用用户的中断回调
    }
}

/**
 * @brief 初始化 GPIO 并配置中断
 * @param int_num: 需要初始化的 GPIO 中断编号
 * @param int_cb: 触发时执行的回调函数
 * @return 0: 成功, 负数: 失败
 */
int si_init_gpio_int(unsigned int_num, void (*int_cb)(void *context, unsigned int int_num))
{
    int ret;

    /* 仅支持 SI_GPIO_INT1 */
    if (int_num != SI_GPIO_INT1) {
        printk("Error: Unsupported interrupt number %d\n", int_num);
        return -EINVAL;
    }

    /* 保存用户回调 */
    user_int_cb = int_cb;

    /* 获取 GPIO 设备 */
    gpio_dev = DEVICE_DT_GET(IMU_INT1_PORT);
    if (!device_is_ready(gpio_dev)) {
        printk("Error: GPIO device not ready\n");
        return -ENODEV;
    }

    /* 先禁用中断，防止误触发 */
    gpio_pin_interrupt_configure(gpio_dev, IMU_INT1_PIN, GPIO_INT_DISABLE);

    /* 配置 GPIO 为输入模式，并启用下拉 */
    ret = gpio_pin_configure(gpio_dev, IMU_INT1_PIN, GPIO_INPUT | GPIO_PULL_DOWN);
    if (ret < 0) {
        printk("Error configuring GPIO pin\n");
        return ret;
    }

    /* 监听上升沿（脉冲） */
    ret = gpio_pin_interrupt_configure(gpio_dev, IMU_INT1_PIN, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret < 0) {
        printk("Error configuring interrupt\n");
        return ret;
    }

    /* 绑定中断回调 */
    gpio_init_callback(&imu_int_cb, int_cb_wrapper, BIT(IMU_INT1_PIN));
    gpio_add_callback(gpio_dev, &imu_int_cb);

    // printk("IMU INT1 interrupt initialized with pull-down and rising edge trigger\n");
    return 0;
}

int si_start_gpio_fsync(uint32_t freq, void (*fsync_timer_cb)(void *context))
{
	(void)freq;
	(void)fsync_timer_cb;
	return 0;
}

int si_stop_gpio_fsync()
{
	return 0;
}

void si_toggle_gpio_fsync(void)
{
}

/*
 * Common
 */

void si_disable_irq()
{
}

void si_enable_irq()
{
}

/*
 * Error codes
 */
int si_print_error_if_any(int rc)
{
	if (rc != 0) {
		switch (rc) {
		case INV_IMU_ERROR:
			INV_MSG(INV_MSG_LEVEL_ERROR, "Unspecified error (%d)", rc);
			break;
		case INV_IMU_ERROR_TRANSPORT:
			INV_MSG(INV_MSG_LEVEL_ERROR, "Error occurred at transport level (%d)", rc);
			break;
		case INV_IMU_ERROR_TIMEOUT:
			INV_MSG(INV_MSG_LEVEL_ERROR, "Action did not complete in the expected time window (%d)",
			        rc);
			break;
		case INV_IMU_ERROR_BAD_ARG:
			INV_MSG(INV_MSG_LEVEL_ERROR, "Invalid argument provided (%d)", rc);
			break;
		case INV_IMU_ERROR_EDMP_BUF_EMPTY:
			INV_MSG(INV_MSG_LEVEL_ERROR, "EDMP buffer is empty (%d)", rc);
			break;
		default:
			INV_MSG(INV_MSG_LEVEL_ERROR, "Unknown error (%d)", rc);
			break;
		}
	}

	return rc;
}

/*
 * Message
 */
void inv_msg(int level, const char *str, ...)
{
    // 只在日志级别不为 OFF 时进行输出
    if (level == INV_MSG_LEVEL_OFF) {
        return;
    }
 
    // 可变参数列表
    va_list args;
   
    // 根据日志级别选择不同的前缀
    const char *level_str = "";
    switch (level) {
        case INV_MSG_LEVEL_ERROR:
            level_str = "[ERROR] ";
            break;
        case INV_MSG_LEVEL_WARNING:
            level_str = "[WARNING] ";
            break;
        case INV_MSG_LEVEL_INFO:
            level_str = "[INFO] ";
            break;
        case INV_MSG_LEVEL_VERBOSE:
            level_str = "[VERBOSE] ";
            break;
        case INV_MSG_LEVEL_DEBUG:
            level_str = "[DEBUG] ";
            break;
        default:
            level_str = "[UNKNOWN] ";
            break;
    }
 
    // // 发送日志级别前缀
    // for (size_t i = 0; level_str[i] != '\0'; i++) {
    //     uart_poll_out(uart_dev, level_str[i]);
    // }
 
    // 初始化可变参数列表
    va_start(args, str);
 
    // 格式化消息
    char msg_buffer[256];  // 假设最大消息长度为 256 字节
    vsnprintf(msg_buffer, sizeof(msg_buffer), str, args);
   
    // 发送格式化后的消息
    for (size_t i = 0; msg_buffer[i] != '\0'; i++) {
        uart_poll_out(uart_dev, msg_buffer[i]);
    }
 
    // 清理可变参数列表
    va_end(args);
 
    // 发送换行符
    uart_poll_out(uart_dev, '\r');  // 先回到行首
    uart_poll_out(uart_dev, '\n');  // 然后换行
   
}

int si_read_gpio(int pin)
{
    if (!device_is_ready(gpio_dev)) {
        printk("[ERROR] GPIO device not ready!\n");
        return -1;
    }

    int value = gpio_pin_get(gpio_dev, pin);
    printk("[DEBUG] si_read_gpio(%d) = %d\n", pin, value);
    return value;
}