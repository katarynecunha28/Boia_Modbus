#include "SondaModbus.h"

#define SONDA_BAUD       9600
#define SONDA_NUM_FLOATS 10

// Callbacks de direção RS485 (só usados se existir pino DE/RE)
static int8_t s_dePin = -1;
static void preTx()  { if (s_dePin >= 0) digitalWrite(s_dePin, HIGH); }
static void postTx() { if (s_dePin >= 0) digitalWrite(s_dePin, LOW); }

SondaModbus::SondaModbus(HardwareSerial& serial, int8_t rx, int8_t tx, int8_t dePin, uint8_t slaveId)
    : _serial(serial), _rxPin(rx), _txPin(tx), _dePin(dePin), _slaveId(slaveId) {}

void SondaModbus::iniciar() {
    s_dePin = _dePin;
    if (_dePin >= 0) {
        pinMode(_dePin, OUTPUT);
        digitalWrite(_dePin, LOW);
        _node.preTransmission(preTx);
        _node.postTransmission(postTx);
    }
    _serial.begin(SONDA_BAUD, SERIAL_8N1, _rxPin, _txPin);
    _node.begin(_slaveId, _serial);

    if (ativar()) Serial.println("Sonda ativada (HR 21/22 = 0xFDFD).");
    else          Serial.println("[Sonda] Falha ao ativar - vai tentar de novo na leitura.");
}

// Escreve 0xFDFD nos holding registers 21 e 22 para ativar a sonda
bool SondaModbus::ativar() {
    _node.setTransmitBuffer(0, 0xFDFD);
    _node.setTransmitBuffer(1, 0xFDFD);
    uint8_t r = _node.writeMultipleRegisters(21, 2);
    if (r != _node.ku8MBSuccess) {
        Serial.printf("[Sonda] Erro ao ativar: 0x%02X\n", r);
        return false;
    }
    return true;
}

// Dois registradores big-endian por palavra, float IEEE754 little-endian
float SondaModbus::regsParaFloat(uint16_t hi, uint16_t lo) {
    union { uint8_t b[4]; float f; } c;
    c.b[0] = hi >> 8;
    c.b[1] = hi & 0xFF;
    c.b[2] = lo >> 8;
    c.b[3] = lo & 0xFF;
    return c.f;
}

bool SondaModbus::lerTodosOsParametros(PacoteBoia* p) {
    uint8_t r = _node.readHoldingRegisters(0, SONDA_NUM_FLOATS * 2);

    if (r != _node.ku8MBSuccess) {
        Serial.printf("[Sonda] Falha na leitura: 0x%02X\n", r);
        p->od_sat = p->od_mgl = p->temp_agua = p->od_temp = p->ph = p->orp = NAN;
        p->ec = p->salinidade = p->tds = p->grav_esp = p->turbidez = NAN;
        ativar();   // pode ter reiniciado / perdido a ativação
        return false;
    }

    float v[SONDA_NUM_FLOATS];
    for (int i = 0; i < SONDA_NUM_FLOATS; i++) {
        v[i] = regsParaFloat(_node.getResponseBuffer(i * 2), _node.getResponseBuffer(i * 2 + 1));
    }

    // Ordem dos registradores da sonda (HR 0-19)
    p->od_sat     = v[0];   // OD %
    p->od_mgl     = v[1];   // OD mg/L
    p->temp_agua  = v[2];   // Temperatura °C
    p->od_temp    = v[2];   // mantido p/ compatibilidade com o servidor
    p->ph         = v[3];
    p->orp        = v[4];
    p->ec         = v[5];   // uS/cm
    p->salinidade = v[6];   // PSU
    p->tds        = v[7];   // mg/L
    p->grav_esp   = v[8];
    p->turbidez   = v[9];   // NTU

    Serial.printf("[Sonda] pH %.2f | OD %.2f mg/L (%.1f%%) | T %.2f C | ORP %.0f mV | EC %.1f\n",
                  p->ph, p->od_mgl, p->od_sat, p->temp_agua, p->orp, p->ec);
    return true;
}
