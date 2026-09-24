// ==========================================
// infrarrojos.ino
// ==========================================
#include <QTRSensors.h>

QTRSensors qtr;

const uint8_t SensorCount = 8;

// Ajusta estos pines a los pines digitales libres 
const uint8_t sensorPins[SensorCount] = {12, 13, 14, 15, 16, 17, 18, 19};

uint16_t sensorValues[SensorCount];

// Umbral para determinar si un sensor lee la línea negra
// Ajustar este valor tras calibrar (típicamente entre 500 y 1000)
const uint16_t UMBRAL_NEGRO = 600; 

void setupInfrarrojos() {
  qtr.setTypeRC();
  qtr.setSensorPins(sensorPins, SensorCount);
  
  for (uint16_t i = 0; i < 100; i++) {
    qtr.calibrate();
    delay(10);
  }
}

bool sensorIR_detectaLinea() {
  qtr.read(sensorValues);
  for (uint8_t i = 0; i < SensorCount; i++) {
    if (sensorValues[i] > UMBRAL_NEGRO) {
      return true; // Se detectó la línea negra
    }
  }
  return false;
}

int calcularErrorLinea() {
  qtr.read(sensorValues);

  int error = 0;


  if (sensorValues[3] > UMBRAL_NEGRO) error = -1;
  else if (sensorValues[2] > UMBRAL_NEGRO) error = -2;
  else if (sensorValues[1] > UMBRAL_NEGRO) error = -3;
  else if (sensorValues[0] > UMBRAL_NEGRO) error = -4;


  if (sensorValues[4] > UMBRAL_NEGRO) error = 1;
  else if (sensorValues[5] > UMBRAL_NEGRO) error = 2;
  else if (sensorValues[6] > UMBRAL_NEGRO) error = 3;
  else if (sensorValues[7] > UMBRAL_NEGRO) error = 4;


  return error;
}