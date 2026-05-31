# Lora_Test1

Test firmware for using an EBYTE E220-900 LoRa module with a Seeed Studio XIAO ESP32S3 Sense board using ESP-IDF.

## Hardware

- Board: Seeed Studio XIAO ESP32S3 Sense
- LoRa module: EBYTE E220-900
- Framework: ESP-IDF

## Wiring

| E220 Signal | ESP32S3 GPIO | XIAO Pin |
| ----------- | ------------ | -------- |
| TXD         | GPIO44       | D7 / RX  |
| RXD         | GPIO43       | D6 / TX  |
| M0          | GPIO2        | D1       |
| M1          | GPIO3        | D2       |
| AUX         | GPIO1        | D0       |
| VCC         | 3V3          | 3V3      |
| GND         | GND          | GND      |

Note: E220 TXD must be connected to ESP32S3 RX, and E220 RXD must be connected to ESP32S3 TX.

## Main Files

- `main/main.c`: configures the module and contains the main test loop.
- `components/lora_e220_900/lora_e220_900.c`: basic E220 driver implementation.
- `components/lora_e220_900/include/lora_e220_900.h`: public component API.
- `components/lora_e220_900/include/lora_e220_900_defs.h`: pins, commands, registers, and configuration enums.

## Current Configuration

The example initializes the E220 with:

- Address: `0x0001`
- Channel: `0x17`
- Module UART: `9600 8N1`
- Air rate: `2.4 kbps`
- TX power: `22 dBm`
- Transmission mode: transparent
- Encryption key: `0x0000`

The configuration is defined in `main/main.c` inside `app_main()`.

## Build

From a terminal with ESP-IDF loaded:

```powershell
idf.py build
```

## Flash

Replace `COMx` with the correct serial port:

```powershell
idf.py -p COMx flash
```

## Serial Monitor

```powershell
idf.py -p COMx monitor
```

Exit the monitor with `Ctrl+]`.

## Basic API Usage

Initialize the module:

```c
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
    .crypt_key = 0x0000,
};

esp_err_t ret = lora_e220_900_init(&cfg);
```

Send data in transparent mode:

```c
const char *msg = "Hello from ESP32";
lora_e220_send((const uint8_t *)msg, strlen(msg));
```

Receive data:

```c
uint8_t rx_buffer[128];
int len = lora_e220_receive(rx_buffer, sizeof(rx_buffer) - 1, 1000);

if (len > 0) {
    rx_buffer[len] = '\0';
    ESP_LOGI("MAIN", "Received: %s", rx_buffer);
}
```

## Important Notes

- GPIO43 and GPIO44 map to D6/TX and D7/RX on the XIAO ESP32S3 Sense.
- If the ESP-IDF console is using UART0, avoid sharing `UART_NUM_0` with the LoRa module. In that case, use `UART_NUM_1` with the same GPIO43/GPIO44 pins, or configure the console to use USB Serial/JTAG.
- The current code configures the module in transparent mode. To use `lora_e220_send_fixed()`, change `transmission_method` to `LORA_FIXED_MODE`.
- If you change the baud rate configured in the E220, also update the ESP32S3 UART configuration.

## Project Status

This project is a functional base for initializing and testing communication with the E220-900. The send/receive loop in `main/main.c` is currently commented out to make step-by-step testing easier.
