// ============================================================================
// Mapa de pinos - Heltec WiFi LoRa 32 V2
// Conferido contra o PCB "Circuito_Transmissor_Dados_Boia_Biotema_V2" (06/11/2026)
// ============================================================================
//
// Reservados pela placa (não mexer):
//   LoRa SPI : SCK 5, MISO 19, MOSI 27, CS 18, RST 14, DIO0 26
//   OLED I2C : SDA 4, SCL 15, RST 16   -> barramento "Wire" (a lib Heltec configura)
//
// ADS1115 (0x48) e EEPROM 24LC512 (0x50) estão nos pinos SDA/SCL do header
// (GPIO 21/22), em OUTRO barramento -> "Wire1". NÃO use o GPIO 21 como Vext.
#define PIN_I2C_SDA     21
#define PIN_I2C_SCL     22

// GPS GY-NEO6MV2 (Serial1). Atenção: no PCB V2 é RX=13 / TX=12
// (o firmware antigo usava 12/13 invertido).
#define PIN_GPS_RX      13   // ESP32 RX  <- TX do GPS (net TX_GPS)
// O GPIO12 é pino de boot (precisa estar LOW ao gravar/ligar), por isso o TX do
// ESP32 para o GPS NAO e usado (-1): o firmware so le o GPS, nunca escreve nele.
// Se quiser garantir, solte o fio RX_GPS ao gravar o firmware.
#define PIN_GPS_TX      -1

// Sonda Modbus (Serial2), módulo RS485 de direção automática (sem DE/RE).
// PCB V2: net RX_MODBUS -> GPIO36 (ok, só entrada)
//         net TX_MODBUS -> GPIO37 (!!! só entrada, NÃO transmite)
// Correção: jumper do net TX_MODBUS (pino TX do U2) para o GPIO23 (livre no PCB).
#define PIN_SONDA_RX    36
#define PIN_SONDA_TX    23
#define PIN_SONDA_DE    -1

// Acqua Nativa (shields antigas, ainda NAO usadas no firmware).
// PCB V2: RX_ACQUA_NATIVA -> GPIO35 (ok p/ RX), TX_ACQUA_NATIVA -> GPIO34 (SO ENTRADA, nao transmite).
// GPIO34..39 sao input-only. Se for usar: jumper do TX p/ o GPIO17 e serial por software/UART mapeada.
#define PIN_ACQUA_RX    35
#define PIN_ACQUA_TX    17

// DS18B20 (temperatura interna/externa da caixa) - GPIO2 com pull-up 4k7
#define PIN_ONEWIRE     2

// EEPROM I2C externa: U9 = 24LC512 (512 kbit = 64 KB, página de 128 bytes; o esquemático ainda mostra 24LC256, mesmo pinout)
#define EEPROM_I2C_ADDR     0x50   // A0..A2 em GND (hoje estão FLUTUANDO no PCB!)
#define EEPROM_TAM_BYTES    65536UL
#define EEPROM_TAM_PAGINA   128
