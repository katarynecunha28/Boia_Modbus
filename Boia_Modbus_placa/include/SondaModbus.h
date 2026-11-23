#pragma once
#include <Arduino.h>
#include <ModbusMaster.h>
#include "Types.h"

// Sonda AcquaProbe-AQ via Modbus RTU (9600 8N1).
// Lógica copiada do teste que funciona (BoiaModbus_teste_cpp_04-08-2026).
class SondaModbus {
private:
    ModbusMaster _node;
    HardwareSerial& _serial;
    int8_t _rxPin, _txPin, _dePin;
    uint8_t _slaveId;
    bool ativar();
    static float regsParaFloat(uint16_t hi, uint16_t lo);

public:
    SondaModbus(HardwareSerial& serial, int8_t rx, int8_t tx, int8_t dePin, uint8_t slaveId);
    void iniciar();
    // Retorna true se a leitura funcionou; em falha preenche os campos com NAN.
    bool lerTodosOsParametros(PacoteBoia* payload);
};
