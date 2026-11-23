#pragma once
#include <Arduino.h>
#include <OneWire.h>
#include <DS18B20.h>

// Um único DS18B20 (temperatura da caixa) no pino 1-Wire.
class TemperatureSensors {
  public:
    TemperatureSensors(uint8_t pin);

    void begin();
    void read();

    float getInterna();   // temperatura do DS18B20 (NAN se falhar)

  private:
    OneWire _oneWire;
    DS18B20 _ds;
    float _tempInt;
};
