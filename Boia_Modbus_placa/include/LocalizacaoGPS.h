#pragma once
#include <Arduino.h>
#include <TinyGPS++.h>
#include "Types.h"

class LocalizacaoGPS {
private:
    TinyGPSPlus _gps;
    HardwareSerial* _portaSerial;
    int8_t _rxPin;
    int8_t _txPin;

public:
    LocalizacaoGPS(HardwareSerial& porta, int8_t rx, int8_t tx);
    TinyGPSPlus& getGPS();
    void iniciar();
    void escutarSatelites(); 
    bool temSinal(); 
    void lerCoordenadas(PacoteBoia* payload);

};