#pragma once
#include <Arduino.h>

typedef struct {
  char boia_id[16];
  uint32_t contador;
  uint32_t timestamp;

  // Água (Modbus)
  float ph;
  float od_temp;      // = temperatura da sonda (a sonda tem um único sensor de temp.)
  float od_mgl;
  float od_sat;       // OD em %
  float ec;           // uS/cm
  float orp;          // mV
  float temp_agua;    // °C

  // Energia (ADS1115)
  float tensao_bateria;
  float tensao_solar;
  float tensao_modulos;
  float tensao_heltec;

  // Localização (GPS)
  float lat;
  float lng;
  float num_satellites;
  float hdop;
  float alt;

  // Temperatura interna e externa da caixa
  float temp_int;
  float temp_ext;

  //  Novos parâmetros da sonda 
  float salinidade;   // PSU
  float tds;          // mg/L
  float grav_esp;     // gravidade específica
  float turbidez;     // NTU
} PacoteBoia;
