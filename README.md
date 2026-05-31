# Lora_Test1

Firmware de prueba para usar un modulo LoRa E220-900 con una placa Seeed Studio XIAO ESP32S3 Sense usando ESP-IDF.

## Hardware

- Placa: Seeed Studio XIAO ESP32S3 Sense
- Modulo LoRa: EBYTE E220-900
- Framework: ESP-IDF

## Conexiones

| Senal E220 | GPIO ESP32S3 | Pin XIAO |
| ---------- | ------------ | -------- |
| TXD        | GPIO44       | D7 / RX  |
| RXD        | GPIO43       | D6 / TX  |
| M0         | GPIO2        | D1       |
| M1         | GPIO3        | D2       |
| AUX        | GPIO1        | D0       |
| VCC        | 3V3          | 3V3      |
| GND        | GND          | GND      |

Nota: TXD del E220 debe ir al RX del ESP32S3, y RXD del E220 debe ir al TX del ESP32S3.

## Archivos principales

- `main/main.c`: configura el modulo y contiene el bucle principal de prueba.
- `components/lora_e220_900/lora_e220_900.c`: implementacion del driver basico del E220.
- `components/lora_e220_900/include/lora_e220_900.h`: API publica del componente.
- `components/lora_e220_900/include/lora_e220_900_defs.h`: pines, comandos, registros y enums de configuracion.

## Configuracion actual

El ejemplo inicializa el E220 con:

- Direccion: `0x0001`
- Canal: `0x17`
- UART del modulo: `9600 8N1`
- Air rate: `2.4 kbps`
- Potencia: `22 dBm`
- Metodo de transmision: transparente
- Clave de cifrado: `0x0000`

La configuracion se define en `main/main.c` dentro de `app_main()`.

## Compilar

Desde una terminal con ESP-IDF cargado:

```powershell
idf.py build
```

## Flashear

Reemplaza `COMx` por el puerto serial correspondiente:

```powershell
idf.py -p COMx flash
```

## Monitor serial

```powershell
idf.py -p COMx monitor
```

Para salir del monitor: `Ctrl+]`.

## Uso basico de la API

Inicializar el modulo:

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

Enviar en modo transparente:

```c
const char *msg = "Hola desde ESP32";
lora_e220_send((const uint8_t *)msg, strlen(msg));
```

Recibir:

```c
uint8_t rx_buffer[128];
int len = lora_e220_receive(rx_buffer, sizeof(rx_buffer) - 1, 1000);

if (len > 0) {
    rx_buffer[len] = '\0';
    ESP_LOGI("MAIN", "Recibido: %s", rx_buffer);
}
```

## Notas importantes

- GPIO43 y GPIO44 corresponden a D6/TX y D7/RX en la XIAO ESP32S3 Sense.
- Si el proyecto usa la consola de ESP-IDF por UART0, evita compartir `UART_NUM_0` con el modulo LoRa. En ese caso es recomendable usar `UART_NUM_1` con los mismos pines GPIO43/GPIO44, o configurar la consola por USB Serial/JTAG.
- El codigo actual configura el modulo en modo transparente. Para usar `lora_e220_send_fixed()`, cambia `transmission_method` a `LORA_FIXED_MODE`.
- Si cambias el baudrate configurado en el E220, asegúrate de actualizar tambien la configuracion UART del ESP32S3.

