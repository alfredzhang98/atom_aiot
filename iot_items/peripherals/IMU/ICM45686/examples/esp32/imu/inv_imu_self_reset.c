#include "imu/inv_imu_driver.h"
#include "imu/inv_imu_defs.h"
#include "esp_log.h"

/**
 * @brief 自定义实现的软复位函数，直接操作寄存器，尝试绕过原始驱动的复位问题
 * 
 * @param s 设备实例
 * @return int 0表示成功，负值表示错误
 */
int inv_imu_self_reset(inv_imu_device_t *s)
{
    int status = INV_IMU_OK;
    uint8_t reg_misc2 = 0;
    uint8_t int_status = 0;
    int timeout_ms = 100; // 100ms timeout
    
    ESP_LOGI("IMU_RESET", "开始执行手动软复位");
    
    // 1. 读取REG_MISC2寄存器的当前值
    status = s->transport.read_reg(REG_MISC2, &reg_misc2, 1);
    if (status != 0) {
        ESP_LOGE("IMU_RESET", "读取REG_MISC2失败: %d", status);
        return INV_IMU_ERROR_TRANSPORT;
    }
    
    // 2. 设置soft_rst位（第0位）
    reg_misc2 |= 0x01;
    
    // 3. 写回寄存器触发复位
    status = s->transport.write_reg(REG_MISC2, &reg_misc2, 1);
    if (status != 0) {
        ESP_LOGE("IMU_RESET", "写入REG_MISC2失败: %d", status);
        return INV_IMU_ERROR_TRANSPORT;
    }
    
    ESP_LOGI("IMU_RESET", "触发软复位指令已发送");
    
    // 4. 等待复位完成 - 至少1ms
    s->transport.sleep_us(5000);
    
    // 5. 等待RESET_DONE中断标志
    while (timeout_ms > 0) {
        // 读取INT1_STATUS0寄存器，检查reset_done位（第7位）
        status = s->transport.read_reg(INT1_STATUS0, &int_status, 1);
        if (status != 0) {
            ESP_LOGW("IMU_RESET", "读取INT1_STATUS0失败: %d, 重试中...", status);
            s->transport.sleep_us(1000);
            timeout_ms--;
            continue;
        }
        
        // 检查reset_done位是否置位
        if (int_status & 0x80) {  // INT1_STATUS0的第7位是reset_done
            ESP_LOGI("IMU_RESET", "复位完成！INT1_STATUS0=0x%02x", int_status);
            return INV_IMU_OK;
        }
        
        s->transport.sleep_us(1000);  // 等待1ms再次检查
        timeout_ms--;
    }
    
    ESP_LOGE("IMU_RESET", "等待复位完成超时");
    return INV_IMU_ERROR_TIMEOUT;
}
