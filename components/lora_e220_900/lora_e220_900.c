#include <stdio.h>
#include <string.h>
#include "lora_e220_900.h"

#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const static char *TAG = "lora";
static uint8_t lora_build_reg0(const lora_e220_900_settings_t *cfg)
{
    return ((cfg->uart_baud & 0x07) << 5) |
           ((cfg->uart_parity & 0x03) << 3) |
           ((cfg->air_rate & 0x07) << 0);
}

static uint8_t lora_build_reg1(const lora_e220_900_settings_t *cfg)
{
    return ((cfg->sub_packet & 0x03) << 6) |
           ((cfg->rssi_noise & 0x01) << 5) |
           ((cfg->tx_power & 0x03) << 0);
}

static uint8_t lora_build_reg3(const lora_e220_900_settings_t *cfg)
{
    return ((cfg->rssi_byte & 0x01) << 7) |
           ((cfg->transmission_method & 0x01) << 6) |
           ((cfg->lbt & 0x01) << 4) |
           ((cfg->wor_cycle & 0x07) << 0);
}

static esp_err_t lora_hw_init(void)
{

    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    esp_err_t ret = uart_driver_install(LORA_UART_PORT, 1024, 1024, 0, NULL, 0);

    if (ret != ESP_OK)
        return ret;

    ret = uart_param_config(LORA_UART_PORT, &uart_config);

    if (ret != ESP_OK)
        return ret;

    ret = uart_set_pin(LORA_UART_PORT, LORA_TX_PIN, LORA_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    if (ret != ESP_OK)
        return ret;

    gpio_config_t o_Config = {
        .pin_bit_mask = (1ULL << LORA_M0_PIN) | (1ULL << LORA_M1_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};

    gpio_config_t i_Config = {
        .pin_bit_mask = (1ULL << LORA_AUX_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE

    };

    ret = gpio_config(&o_Config);
    if (ret != ESP_OK)
        return ret;
    ret = gpio_config(&i_Config);
    if (ret != ESP_OK)
        return ret;

    gpio_set_level(LORA_M0_PIN, 0);
    gpio_set_level(LORA_M1_PIN, 0);
    uart_flush_input(LORA_UART_PORT);
    return ESP_OK;
}

static esp_err_t lora_e220_uart_write(const uint8_t *data, uint16_t length)
{
    if (data == NULL || length == 0)
        return ESP_ERR_INVALID_ARG;

    int written = uart_write_bytes(LORA_UART_PORT,
                                   (const char *)data,
                                   length);

    if (written != length)
    {
        return ESP_FAIL;
    }

    return ESP_OK;
}

static esp_err_t lora_e220_uart_read(uint8_t *data,
                                     uint16_t length,
                                     uint32_t timeout_ms)
{
    if (data == NULL || length == 0)
        return ESP_ERR_INVALID_ARG;

    int read = uart_read_bytes(
        LORA_UART_PORT,
        data,
        length,
        pdMS_TO_TICKS(timeout_ms));

    if (read != length)
        return ESP_ERR_TIMEOUT;

    return ESP_OK;
}

static esp_err_t lora_e220_write_registers(const lora_e220_900_settings_t *cfg)
{
    if (cfg == NULL)
        return ESP_ERR_INVALID_ARG;

    uint8_t buffer[11];
    uint8_t response[11];

    buffer[0] = COMMAND_SET;
    buffer[1] = LORA_ADDH;
    buffer[2] = 0x08;
    buffer[3] = (uint8_t)(cfg->address >> 8);
    buffer[4] = (uint8_t)(cfg->address & 0xFF);
    buffer[5] = lora_build_reg0(cfg);
    buffer[6] = lora_build_reg1(cfg);
    buffer[7] = cfg->channel;
    buffer[8] = lora_build_reg3(cfg);
    buffer[9] = (uint8_t)(cfg->crypt_key >> 8);
    buffer[10] = (uint8_t)(cfg->crypt_key & 0xFF);

    ESP_LOGI(TAG, "TX:");
    for (int i = 0; i < sizeof(buffer); i++)
        ESP_LOGI(TAG, "[%02d] = 0x%02X", i, buffer[i]);

    esp_err_t ret = lora_e220_uart_write(buffer, sizeof(buffer));
    if (ret != ESP_OK)
        return ret;

    ret = lora_e220_uart_read(response, sizeof(response), 1000);
    if (ret != ESP_OK)
        return ret;

    ESP_LOGI(TAG, "RX:");
    for (int i = 0; i < sizeof(response); i++)
        ESP_LOGI(TAG, "[%02d] = 0x%02X", i, response[i]);

    if (response[0] != COMMAND_RESPONSE)
        return ESP_FAIL;

    for (int i = 1; i < sizeof(buffer); i++)
    {
        if (response[i] != buffer[i])
            return ESP_FAIL;
    }

    return ESP_OK;
}


static esp_err_t lora_e220_set_mode(lora_e220_mode_t mode)
{
    switch (mode)
    {
    case LORA_MODE_NORMAL:
        gpio_set_level(LORA_M1_PIN, 0);
        gpio_set_level(LORA_M0_PIN, 0);
        break;

    case LORA_MODE_WOR_TX:
        gpio_set_level(LORA_M1_PIN, 0);
        gpio_set_level(LORA_M0_PIN, 1);
        break;

    case LORA_MODE_WOR_RX:
        gpio_set_level(LORA_M1_PIN, 1);
        gpio_set_level(LORA_M0_PIN, 0);
        break;

    case LORA_MODE_CONFIG:
        gpio_set_level(LORA_M1_PIN, 1);
        gpio_set_level(LORA_M0_PIN, 1);
        break;

    default:
        return ESP_ERR_INVALID_ARG;
    }

    vTaskDelay(pdMS_TO_TICKS(2));

    return ESP_OK;
}

static esp_err_t lora_e220_wait_aux(uint32_t timeout_ms)
{
    TickType_t start_time = xTaskGetTickCount();

    while (gpio_get_level(LORA_AUX_PIN) == 0)
    {
        if ((xTaskGetTickCount() - start_time) >
            pdMS_TO_TICKS(timeout_ms))
        {
            return ESP_ERR_TIMEOUT;
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }

    return ESP_OK;
}

esp_err_t lora_e220_900_init(const lora_e220_900_settings_t *cfg)
{
    if (cfg == NULL)
        return ESP_ERR_INVALID_ARG;

    esp_err_t ret = lora_hw_init();
    if (ret != ESP_OK)
        return ret;
    ret = lora_e220_set_mode(LORA_MODE_CONFIG);
    if (ret != ESP_OK)
        return ret;

    ret = lora_e220_wait_aux(1000);
    if (ret != ESP_OK)
        return ret;

    ret = lora_e220_write_registers(cfg);
    if (ret != ESP_OK)
        return ret;

    ret = lora_e220_wait_aux(1000);
    if (ret != ESP_OK)
        return ret;

    ret = lora_e220_set_mode(LORA_MODE_NORMAL);
    if (ret != ESP_OK)
        return ret;

    return lora_e220_wait_aux(1000);
}

// Enviar datos en modo normal
esp_err_t lora_e220_send(const uint8_t *data, uint16_t length)
{
    if (data == NULL || length == 0)
        return ESP_ERR_INVALID_ARG;

    esp_err_t ret = lora_e220_wait_aux(1000);
    if (ret != ESP_OK) return ret;

    ret = lora_e220_uart_write(data, length);
    if (ret != ESP_OK) return ret;

    return lora_e220_wait_aux(1000);
}


esp_err_t lora_e220_send_fixed(uint16_t address,
                               uint8_t channel,
                               const uint8_t *data,
                               uint16_t length)
{
    if (data == NULL || length == 0)
        return ESP_ERR_INVALID_ARG;

    uint8_t tx_buffer[3 + length];

    tx_buffer[0] = (uint8_t)(address >> 8);
    tx_buffer[1] = (uint8_t)(address & 0xFF);
    tx_buffer[2] = channel;

    memcpy(&tx_buffer[3], data, length);

    esp_err_t ret = lora_e220_wait_aux(1000);
    if (ret != ESP_OK) return ret;

    ret = lora_e220_uart_write(tx_buffer, sizeof(tx_buffer));
    if (ret != ESP_OK) return ret;

    return lora_e220_wait_aux(1000);
}


int lora_e220_receive(uint8_t *buffer, uint16_t max_len, uint32_t timeout_ms)
{
    if (buffer == NULL || max_len == 0)
        return -1;

    return uart_read_bytes(
        LORA_UART_PORT,
        buffer,
        max_len,
        pdMS_TO_TICKS(timeout_ms));
}

