#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include "heltec.h"
#include "Pins.h"
#include "Types.h"
#include "TemperatureSensors.h"
#include "SondaModbus.h"
#include "EnergiaADS.h"
#include "LocalizacaoGPS.h"
#include "MemoriaEEPROM.h"

#define SONDA_SLAVE_ID 21   // no read_sonda.py está 20; o teste em C++ que funciona usa 21

// Instâncias
SondaModbus moduloAgua(Serial2, PIN_SONDA_RX, PIN_SONDA_TX, PIN_SONDA_DE, SONDA_SLAVE_ID);
EnergiaADS moduloEnergia;
LocalizacaoGPS moduloGPS(Serial1, PIN_GPS_RX, PIN_GPS_TX);
MemoriaEEPROM moduloMemoria;
TemperatureSensors sensoresTemp(PIN_ONEWIRE);

PacoteBoia payloadAtual;
unsigned long ultimoEnvio = 0;
unsigned long ultimoDreno = 0;
const unsigned long intervaloDeEnvio = 30000;
const unsigned long intervaloDreno   = 3000;   // 1 pacote da EEPROM a cada 3 s
bool sondaOK = false;

// ====================================================================
// DISPLAY OLED
// ====================================================================
void atualizarDisplayOLED(bool envioLoRaOK) {
    Heltec.display->clear();
    Heltec.display->setFont(ArialMT_Plain_10);

    Heltec.display->drawString(0, 0, String("GPS: ") + (moduloGPS.temSinal()
        ? "OK sat " + String((int)payloadAtual.num_satellites) : "aguardando..."));
    Heltec.display->drawString(0, 10, "Mod " + String(payloadAtual.tensao_modulos, 2) +
                                      " Sol " + String(payloadAtual.tensao_solar, 1) +
                                      " Bat " + String(payloadAtual.tensao_bateria, 1));
    Heltec.display->drawString(0, 20, sondaOK
        ? "pH " + String(payloadAtual.ph, 2) + " OD " + String(payloadAtual.od_mgl, 2)
        : String("Sonda: FALHA"));
    Heltec.display->drawString(0, 30, sondaOK
        ? "T " + String(payloadAtual.temp_agua, 1) + " EC " + String(payloadAtual.ec, 0)
        : String(""));
    Heltec.display->drawString(0, 40, envioLoRaOK ? "LoRa: Enviado OK" : "LoRa: FALHOU!");
    Heltec.display->drawString(0, 50, moduloMemoria.disponivel()
        ? "Fila EEPROM: " + String(moduloMemoria.contarAtrasos()) + " pacotes"
        : String("EEPROM: nao encontrada"));
    Heltec.display->display();
}

// ====================================================================
// LoRa
// ====================================================================
bool enviarViaLoRa(PacoteBoia* p) {
    Heltec.LoRa.beginPacket();
    Heltec.LoRa.write((uint8_t*)p, sizeof(PacoteBoia));
    int status = Heltec.LoRa.endPacket();
    return (status == 1);
}

void setup() {
    WiFi.mode(WIFI_OFF);
    btStop();
    Serial.begin(115200);

    memset(&payloadAtual, 0, sizeof(payloadAtual));
    strcpy(payloadAtual.boia_id, "mara_2");

    Heltec.begin(true, true, true, true, 915E6);
    Heltec.display->clear();
    Heltec.display->drawString(0, 20, "Iniciando mara_2...");
    Heltec.display->display();

    // I2C dos sensores (ADS1115 + EEPROM) em Wire1, separado do OLED (Wire 4/15).
    // Depois do Heltec.begin(), que reconfigura o Wire para o OLED.
    Wire1.begin(PIN_I2C_SDA, PIN_I2C_SCL);

    sensoresTemp.begin();
    moduloGPS.iniciar();
    moduloAgua.iniciar();
    moduloMemoria.iniciar(Wire1);
    moduloEnergia.iniciar();

    Heltec.LoRa.idle();
    Heltec.LoRa.setSpreadingFactor(7);
    Heltec.LoRa.setSignalBandwidth(125E3);
    Heltec.LoRa.setCodingRate4(5);
    Heltec.LoRa.enableCrc();
    Heltec.LoRa.setTxPower(14, RF_PACONFIG_PASELECT_PABOOST);
}

void loop() {
    moduloGPS.escutarSatelites();

    if (millis() - ultimoEnvio >= intervaloDeEnvio) {
        ultimoEnvio = millis();
        payloadAtual.contador++;

        sensoresTemp.read();
        payloadAtual.temp_int = sensoresTemp.getInterna();
        payloadAtual.temp_ext = NAN;   // só há um DS18B20 no PCB; campo mantido para não mudar o pacote

        sondaOK = moduloAgua.lerTodosOsParametros(&payloadAtual);

        moduloGPS.escutarSatelites();   // esvazia o buffer do GPS após a leitura Modbus (bloqueante)
        moduloGPS.lerCoordenadas(&payloadAtual);
        moduloEnergia.lerTensoes(&payloadAtual);

        bool sucessoEnvio = enviarViaLoRa(&payloadAtual);
        if (!sucessoEnvio) {
            moduloMemoria.guardarAtraso(&payloadAtual);
        }
        atualizarDisplayOLED(sucessoEnvio);
        ultimoDreno = millis();
        return;
    }

    // Reenvia pacotes atrasados guardados na EEPROM, um de cada vez
    if (moduloMemoria.temAtrasosPendentes() && millis() - ultimoDreno >= intervaloDreno) {
        ultimoDreno = millis();
        PacoteBoia atrasado;
        moduloMemoria.lerAtraso(&atrasado);
        if (enviarViaLoRa(&atrasado)) {
            moduloMemoria.confirmarEnvioAtraso();
            atualizarDisplayOLED(true);
        }
    }
}
