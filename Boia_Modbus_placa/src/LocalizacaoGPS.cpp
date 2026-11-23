#include "LocalizacaoGPS.h"
#include <time.h> 

LocalizacaoGPS::LocalizacaoGPS(HardwareSerial& porta, int8_t rx, int8_t tx) {
    _portaSerial = &porta;
    _rxPin = rx;
    _txPin = tx;
}

TinyGPSPlus& LocalizacaoGPS::getGPS() {
    return _gps;
}

void LocalizacaoGPS::iniciar() {
    _portaSerial->begin(9600, SERIAL_8N1, _rxPin, _txPin);
    Serial.println("Módulo GPS Iniciado.");
}

void LocalizacaoGPS::escutarSatelites() {
    while (_portaSerial->available() > 0) {
        _gps.encode(_portaSerial->read());
    }
}

void LocalizacaoGPS::lerCoordenadas(PacoteBoia* payload) {

    if (_gps.location.isValid()) {
        payload->lat = _gps.location.lat();
        payload->lng = _gps.location.lng();
        payload->alt = _gps.altitude.meters();
        payload->hdop = _gps.hdop.value();
        payload->num_satellites = _gps.satellites.value();
 
    } else {
        payload->lat = NAN;
        payload->lng = NAN;
        payload->alt = NAN;
        payload->hdop = NAN;
        payload->num_satellites = NAN;

    }

    if (_gps.date.isValid() && _gps.time.isValid()) {
        struct tm t;
        t.tm_year = _gps.date.year() - 1900;
        t.tm_mon  = _gps.date.month() - 1;
        t.tm_mday = _gps.date.day();
        t.tm_hour = _gps.time.hour();
        t.tm_min  = _gps.time.minute();
        t.tm_sec  = _gps.time.second();
        t.tm_isdst = 0;

        payload->timestamp = mktime(&t);
    }


}

bool LocalizacaoGPS::temSinal() {
    return _gps.location.isValid() &&
           _gps.satellites.value() >= 2;
}