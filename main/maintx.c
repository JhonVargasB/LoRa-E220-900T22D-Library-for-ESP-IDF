#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "lora_e220_900.h"

static const char *TAG = "MAIN";
typedef struct
{
    uint16_t address;
    uint8_t channel;
    float tiempo;
    float altitud;
    float presion;
    float temperatura;
    float Latitud;
    float Longitud;
    float voltaje;
    uint8_t estado;
} data_t;

lora_e220_900_settings_t cfg;

data_t data;

void app_main(void)
{

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
        data.tiempo += 1.0;
        data.altitud += 0.5;
        data.presion -= 0.8;
        data.temperatura += 0.1;
        data.voltaje -= 0.01;
        data.estado = !data.estado;

        esp_err_t ret = lora_e220_send_fixed(
            data.address,
            data.channel,
            (const uint8_t *)&data,
            sizeof(data_t));

        if (ret == ESP_OK)
            ESP_LOGI("TX", "Estructura enviada: %d bytes", sizeof(data_t));
        else
            ESP_LOGE("TX", "Error enviando: %s", esp_err_to_name(ret));

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

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
    .transmission_method = LORA_FIXED_MODE,
    .lbt = LORA_LBT_DISABLE,
    .wor_cycle = LORA_WOR_2000MS,

    .crypt_key = 0x0000};

data_t data = {
    .address = 0x0002,
    .channel = 0x17,
    .tiempo = 0,
    .altitud = 100.0,
    .presion = 68950.0,
    .temperatura = 18.0,
    .Latitud = -13.5167,
    .Longitud = -71.9781,
    .voltaje = 4.20,
    .estado = 1};