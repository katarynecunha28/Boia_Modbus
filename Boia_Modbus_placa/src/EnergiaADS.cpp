#include "EnergiaADS.h"

EnergiaADS::EnergiaADS() {
    // ====================================================================
    // AJUSTE DOS DIVISORES DE TENSÃO
    // Altere estes valores conforme os resistores usados no seu hardware
    // Exemplo: (R1 + R2) / R2
    // ====================================================================
    _fatorSolar   = 7.5; // Ex: Para ler até 24V GAIN_ONE
    _fatorBateria = 4.3;  // Ex: Para ler até 14V GAIN_ONE 
    _fatorModulos = 1.0; //Para ler 3.3V GAIN_ONE
    _fatorHeltec = 1.45; //para ler 5V com GAIN_TWOTHIRDS
}

void EnergiaADS::iniciar() {
    // O endereço padrão do ADS1115 é 0x48 (pino ADDR no GND)
    if (!_ads.begin(0x48, &Wire1)) {
        Serial.println("[ADC Erro] ADS1115 não encontrado no barramento I2C!");
    } else {
        // GAIN_ONE: Fundo de escala de +/- 4.096V
        // 1 bit = 0.125mV
        _ads.setGain(GAIN_ONE);
        Serial.println("Módulo de Energia (ADS1115) Pronto.");
    }
}

void EnergiaADS::lerTensoes(PacoteBoia* payload) {
    // Realiza a leitura dos canais analógicos
    // A0 = Painel Solar | A1 = Bateria | A2 = MODULOS
    int16_t rawSolar = _ads.readADC_SingleEnded(0);
    int16_t rawBat   = _ads.readADC_SingleEnded(1);
    int16_t rawModulos = _ads.readADC_SingleEnded(2);
    int16_t rawHeltec   = _ads.readADC_SingleEnded(3);


    // Converte os bits brutos em Volts (considerando o ganho GAIN_ONE)
    float voltsA0 = _ads.computeVolts(rawSolar);
    float voltsA1 = _ads.computeVolts(rawBat);
    float voltsA2 = _ads.computeVolts(rawModulos);
    float voltsA3 = _ads.computeVolts(rawHeltec);


    // Aplica o fator do divisor de tensão para saber a voltagem real da fonte
    payload->tensao_solar   = voltsA0 * _fatorSolar;
    payload->tensao_bateria = voltsA1 * _fatorBateria;
    payload->tensao_modulos = voltsA2;
    payload->tensao_heltec =  voltsA3 * _fatorHeltec;
  
    // Validação de segurança
    if (payload->tensao_modulos < 3.0) {
        Serial.println("[ALERTA] Modulo Baixa energia!");
    }
}