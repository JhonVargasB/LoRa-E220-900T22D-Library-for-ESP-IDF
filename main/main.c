#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "lora_e220_900.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    lora_e220_900_settings_t cfg = {
        .address = 0x0001,
        .channel = 0x17,

        .uart_baud = LORA_UART_BAUD_9600,
        .uart_parity = LORA_UART_PARITY_8N1,
        .air_rate = LORA_AIR_RATE_2K4,

        .sub_packet = LORA_SUB_PACKET_200,
        .rssi_noise = LORA_RSSI_NOISE_DISABLE,
        .tx_power = LORA_TX_POWER_22DBM,

        .rssi_byte = LORA_RSSI_BYTE_DISABLE,
        .transmission_method = LORA_TRANSPARENT_MODE,
        .lbt = LORA_LBT_DISABLE,
        .wor_cycle = LORA_WOR_2000MS,

        .crypt_key = 0x0000
    };

    ESP_LOGI(TAG, "Inicializando LoRa E220...");

    esp_err_t ret = lora_e220_900_init(&cfg);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error inicializando/configurando LoRa: %s",
                 esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "LoRa configurado correctamente");

    uint8_t rx_buffer[128];

    while (1)
    {


        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}