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

- `main/maintx.c`: transmitter example. It configures the local E220 address as `0x0001` and sends a telemetry-like `data_t` structure to address `0x0002` on channel `0x17`.
- `main/mainrx.c`: receiver example. It configures the local E220 address as `0x0002`, waits for a `data_t` packet, and prints the received fields to the ESP-IDF log.
- `main/main.c`: earlier single-file test example kept as a reference.
- `main/CMakeLists.txt`: selects which main program is compiled. It currently builds `maintx.c`.
- `components/lora_e220_900/lora_e220_900.c`: basic E220 driver implementation.
- `components/lora_e220_900/include/lora_e220_900.h`: public component API.
- `components/lora_e220_900/include/lora_e220_900_defs.h`: pins, commands, registers, and configuration enums.

## Current Configuration

Both TX and RX examples initialize the E220 with:

- Channel: `0x17`
- Module UART: `9600 8N1`
- Air rate: `2.4 kbps`
- TX power: `22 dBm`
- Transmission mode: fixed
- Encryption key: `0x0000`

The transmitter uses local address `0x0001` and sends packets to destination address `0x0002`. The receiver uses local address `0x0002`.

The active program is selected in `main/CMakeLists.txt`:

```cmake
idf_component_register(SRCS "maintx.c"
                    INCLUDE_DIRS ".")
```

To build the receiver firmware instead, change `maintx.c` to `mainrx.c` and rebuild/flash the board.

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

## TX/RX Test Programs

This project has two main application files for testing two boards:

- Flash `main/maintx.c` to the transmitter board.
- Flash `main/mainrx.c` to the receiver board.
- Keep both modules on the same channel, UART settings, air rate, and encryption key.
- Use different E220 addresses for each node. The current examples use `0x0001` for TX and `0x0002` for RX.

The transmitted payload is a C structure:

```c
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
```

Because the payload is sent as raw binary data, both boards must use the same `data_t` definition and compatible compiler/target settings.

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
    .transmission_method = LORA_FIXED_MODE,
    .lbt = LORA_LBT_DISABLE,
    .wor_cycle = LORA_WOR_2000MS,
    .crypt_key = 0x0000,
};

esp_err_t ret = lora_e220_900_init(&cfg);
```

Send data in fixed mode:

```c
data_t data = {
    .address = 0x0002,
    .channel = 0x17,
};

lora_e220_send_fixed(data.address,
                     data.channel,
                     (const uint8_t *)&data,
                     sizeof(data));
```

Receive data:

```c
uint8_t rx[128];
int len = lora_e220_receive(rx, sizeof(rx), 1000);

if (len == sizeof(data_t)) {
    data_t data_rx;
    memcpy(&data_rx, rx, sizeof(data_t));
    ESP_LOGI("RX", "Altitude: %.2f", data_rx.altitud);
}
```

## Important Notes

- GPIO43 and GPIO44 map to D6/TX and D7/RX on the XIAO ESP32S3 Sense.
- If the ESP-IDF console is using UART0, avoid sharing `UART_NUM_0` with the LoRa module. In that case, use `UART_NUM_1` with the same GPIO43/GPIO44 pins, or configure the console to use USB Serial/JTAG.
- The current TX/RX examples configure the module in fixed mode. For transparent mode, use `LORA_TRANSPARENT_MODE` and `lora_e220_send()`.
- If you change the baud rate configured in the E220, also update the ESP32S3 UART configuration.

## Project Status

This project is a functional base for testing point-to-point binary telemetry with two E220-900 modules. The current `main/CMakeLists.txt` builds the transmitter example by default.
