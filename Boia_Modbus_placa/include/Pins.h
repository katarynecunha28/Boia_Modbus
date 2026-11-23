#define PIN_I2C_SDA     21
#define PIN_I2C_SCL     22

// GPS GY-NEO6MV2 (Serial1).
#define PIN_GPS_RX      13   // ESP32 RX  <- TX do GPS (net TX_GPS)
#define PIN_GPS_TX      12

// Sonda Modbus (Serial2), módulo RS485 de direção automática (sem DE/RE).
#define PIN_SONDA_RX    23
#define PIN_SONDA_TX    17
#define PIN_SONDA_DE    -1

// Acqua Nativa (shields antigas, ainda NAO usadas no firmware).
#define PIN_ACQUA_RX    25
#define PIN_ACQUA_TX    32

// DS18B20 (temperatura interna/externa da caixa) - GPIO2 com pull-up 4k7
#define PIN_ONEWIRE     2

// EEPROM I2C 
#define EEPROM_I2C_ADDR     0x50   
#define EEPROM_TAM_BYTES    65536UL
#define EEPROM_TAM_PAGINA   128
