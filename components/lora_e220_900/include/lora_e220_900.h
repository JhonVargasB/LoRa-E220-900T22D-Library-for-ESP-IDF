#ifndef LORA_E220_900_H
#define LORA_E220_900_H
#include "esp_err.h"
#include "lora_e220_900_defs.h"

esp_err_t lora_e220_900_init(const lora_e220_900_settings_t *cfg);
int lora_e220_receive(uint8_t *buffer, uint16_t max_len, uint32_t timeout_ms);
esp_err_t lora_e220_send_fixed(uint16_t address,
                               uint8_t channel,
                               const uint8_t *data,
                               uint16_t length);
esp_err_t lora_e220_send(const uint8_t *data, uint16_t length);

#endif






