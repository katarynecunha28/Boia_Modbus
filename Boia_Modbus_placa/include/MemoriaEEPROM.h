#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_External_EEPROM.h>
#include <Preferences.h>
#include "Types.h"

// Fila circular de pacotes não enviados numa EEPROM I2C externa.
// head/tail ficam na flash do ESP32 (Preferences) para sobreviver a reset.
class MemoriaEEPROM {
private:
    ExternalEEPROM _eeprom;
    Preferences _memoriaFlash;
    uint32_t _head = 0;
    uint32_t _tail = 0;
    uint32_t _tamanhoEEPROM = 0;
    bool _ok = false;

public:
    MemoriaEEPROM();
    void iniciar(TwoWire& wire = Wire);
    bool disponivel() { return _ok; }
    int contarAtrasos();
    void guardarAtraso(PacoteBoia* payload);
    bool temAtrasosPendentes();
    void lerAtraso(PacoteBoia* payload);
    void confirmarEnvioAtraso();
};
