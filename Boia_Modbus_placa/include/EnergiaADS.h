#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include "Types.h"

class EnergiaADS {
private:
    Adafruit_ADS1115 _ads;
    float _fatorSolar;
    float _fatorBateria;
    float _fatorModulos;
    float _fatorHeltec;

public:
    EnergiaADS();
    void iniciar();
    void lerTensoes(PacoteBoia* payload);
};