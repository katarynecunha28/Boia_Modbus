#include "TemperatureSensors.h"

TemperatureSensors::TemperatureSensors(uint8_t pin)
: _oneWire(pin),
  _ds(&_oneWire),
  _tempInt(NAN)
{
}

void TemperatureSensors::begin() {
    _ds.begin();
}

void TemperatureSensors::read() {
    _ds.requestTemperatures();
    float t = _ds.getTempC();
    // A lib retorna valores muito negativos (-127/-128) quando o sensor não responde
    _tempInt = (t > -100.0f) ? t : NAN;
}

float TemperatureSensors::getInterna() { return _tempInt; }
