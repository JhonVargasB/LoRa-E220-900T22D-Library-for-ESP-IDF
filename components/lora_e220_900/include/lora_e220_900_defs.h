#ifndef LORA_E220_900_DEFS_H
#define LORA_E220_900_DEFS_H
#include <stdint.h>

//Definiciones de HW
#define LORA_UART_PORT UART_NUM_1
#define LORA_TX_PIN GPIO_NUM_43
#define LORA_RX_PIN GPIO_NUM_44
#define LORA_M0_PIN GPIO_NUM_2
#define LORA_M1_PIN GPIO_NUM_3
#define LORA_AUX_PIN GPIO_NUM_4


#define COMMAND_SET 0xC0
#define COMMAND_READ 0xC1
#define COMMAND_SET_TEMP 0xC2
#define COMMAND_RESPONSE 0xC1
#define COMMAND_ERROR 0xFF

// Definicion de Registros
#define LORA_ADDH 0x00 // Default 0
#define LORA_ADDL 0x01 // Default 0
#define LORA_REG0 0x02
#define LORA_REG1 0x03
#define LORA_REG2 0x04
#define LORA_REG3 0x05
#define LORA_CRYPT_H 0x06
#define LORA_CRYPT_L 0x07

// 7 6 5 UART Serial Port Rate (bps)
typedef enum
{
    LORA_UART_BAUD_1200   = 0,
    LORA_UART_BAUD_2400   = 1,
    LORA_UART_BAUD_4800   = 2,
    LORA_UART_BAUD_9600   = 3,
    LORA_UART_BAUD_19200  = 4,
    LORA_UART_BAUD_38400  = 5,
    LORA_UART_BAUD_57600  = 6,
    LORA_UART_BAUD_115200 = 7

} lora_uart_baud_t;

// 4 3 Serial Parity Bit
typedef enum
{
    LORA_UART_PARITY_8N1 = 0,
    LORA_UART_PARITY_8O1 = 1,
    LORA_UART_PARITY_8E1 = 2

} lora_uart_parity_t;

// 2 1 0 Air Data Rate (kbps)
typedef enum
{
    LORA_AIR_RATE_2K4  = 2,
    LORA_AIR_RATE_4K8  = 3,
    LORA_AIR_RATE_9K6  = 4,
    LORA_AIR_RATE_19K2 = 5,
    LORA_AIR_RATE_38K4 = 6,
    LORA_AIR_RATE_62K5 = 7

} lora_air_rate_t;

// 7 6 Sub Packet Setting
typedef enum
{
    LORA_SUB_PACKET_200 = 0,
    LORA_SUB_PACKET_128 = 1,
    LORA_SUB_PACKET_64  = 2,
    LORA_SUB_PACKET_32  = 3

} lora_sub_packet_t;

// 5 RSSI ambient Noise Enable
typedef enum
{
    LORA_RSSI_NOISE_DISABLE = 0,
    LORA_RSSI_NOISE_ENABLE  = 1

} lora_rssi_noise_t;

// 4 3 2 Reserve

// 1 0  Transmission Power
typedef enum
{
    LORA_TX_POWER_22DBM = 0,
    LORA_TX_POWER_17DBM = 1,
    LORA_TX_POWER_13DBM = 2,
    LORA_TX_POWER_10DBM = 3

} lora_tx_power_t;

// 7 Enable RSSI Byte
typedef enum
{
    LORA_RSSI_BYTE_DISABLE = 0,
    LORA_RSSI_BYTE_ENABLE  = 1

} lora_rssi_byte_t;

// 6 Transmission method
typedef enum
{
    LORA_TRANSPARENT_MODE = 0,
    LORA_FIXED_MODE       = 1

} lora_transmission_method_t;

// 5 Reserve

// 4 LBT Enable
typedef enum
{
    LORA_LBT_DISABLE = 0,
    LORA_LBT_ENABLE  = 1

} lora_lbt_t;

// 3 Reserve

// 2 1 0 WOR Cycle
typedef enum
{
    LORA_WOR_500MS  = 0,
    LORA_WOR_1000MS = 1,
    LORA_WOR_1500MS = 2,
    LORA_WOR_2000MS = 3,
    LORA_WOR_2500MS = 4,
    LORA_WOR_3000MS = 5,
    LORA_WOR_3500MS = 6,
    LORA_WOR_4000MS = 7

} lora_wor_cycle_t;



typedef enum
{
    LORA_MODE_NORMAL = 0,      // M1=0, M0=0
    LORA_MODE_WOR_TX = 1,      // M1=0, M0=1
    LORA_MODE_WOR_RX = 2,      // M1=1, M0=0
    LORA_MODE_CONFIG = 3       // M1=1, M0=1

} lora_e220_mode_t;


typedef struct
{
    uint16_t address;
    uint8_t channel;

    lora_uart_baud_t uart_baud;
    lora_uart_parity_t uart_parity;
    lora_air_rate_t air_rate;

    lora_sub_packet_t sub_packet;
    lora_rssi_noise_t rssi_noise;
    lora_tx_power_t tx_power;

    lora_rssi_byte_t rssi_byte;
    lora_transmission_method_t transmission_method;
    lora_lbt_t lbt;
    lora_wor_cycle_t wor_cycle;

    uint16_t crypt_key;

} lora_e220_900_settings_t;



#endif
