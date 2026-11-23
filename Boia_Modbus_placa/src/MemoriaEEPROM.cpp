#include "MemoriaEEPROM.h"
#include "Pins.h"

MemoriaEEPROM::MemoriaEEPROM() {
    _tamanhoEEPROM = EEPROM_TAM_BYTES;
}

void MemoriaEEPROM::iniciar(TwoWire& wire) {
    // O Wire já deve ter sido iniciado (Wire.begin(SDA, SCL)) antes.
    _ok = _eeprom.begin(EEPROM_I2C_ADDR, wire);
    if (!_ok) {
        Serial.println("Erro: EEPROM I2C nao encontrada (endereco/fiacao?)");
        return;
    }
    _eeprom.setMemorySizeBytes(EEPROM_TAM_BYTES);
    _eeprom.setPageSizeBytes(EEPROM_TAM_PAGINA);

    _memoriaFlash.begin("boia_mem", false);
    _head = _memoriaFlash.getUInt("head", 0);
    _tail = _memoriaFlash.getUInt("tail", 0);

    // Se head/tail da flash não fazem sentido p/ esta EEPROM, zera a fila
    if (_head >= _tamanhoEEPROM || _tail >= _tamanhoEEPROM) {
        _head = _tail = 0;
        _memoriaFlash.putUInt("head", 0);
        _memoriaFlash.putUInt("tail", 0);
    }
    Serial.printf("EEPROM OK. Pendentes: %d\n", contarAtrasos());
}

void MemoriaEEPROM::guardarAtraso(PacoteBoia* payload) {
    if (!_ok) return;
    _eeprom.put(_head, *payload);
    _head += sizeof(PacoteBoia);
    if (_head + sizeof(PacoteBoia) > _tamanhoEEPROM) _head = 0;
    _memoriaFlash.putUInt("head", _head);
    Serial.println("-> Erro no LoRa! Pacote guardado na EEPROM.");
}

bool MemoriaEEPROM::temAtrasosPendentes() {
    return _ok && (_head != _tail);
}

void MemoriaEEPROM::lerAtraso(PacoteBoia* payload) {
    if (!_ok) return;
    _eeprom.get(_tail, *payload);
}

void MemoriaEEPROM::confirmarEnvioAtraso() {
    _tail += sizeof(PacoteBoia);
    if (_tail + sizeof(PacoteBoia) > _tamanhoEEPROM) _tail = 0;
    _memoriaFlash.putUInt("tail", _tail);
    Serial.println("-> Pacote atrasado da EEPROM enviado com sucesso!");
}

int MemoriaEEPROM::contarAtrasos() {
    if (!_ok) return 0;
    if (_head >= _tail) {
        return (_head - _tail) / sizeof(PacoteBoia);
    }
    return ((_tamanhoEEPROM - _tail) + _head) / sizeof(PacoteBoia);
}
