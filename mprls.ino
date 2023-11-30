#ifdef MPRLS

#include <Wire.h>
#include "Adafruit_MPRLS.h"

#define CHECK_PRESSURE_TIME 200 // Sample time in [ms]

// You dont *need* a reset and EOC pin for most uses, so we set to -1 and don't connect
#define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
#define EOC_PIN    -1  // set to any GPIO pin to read end-of-conversion by pin
Adafruit_MPRLS mpr = Adafruit_MPRLS(RESET_PIN, EOC_PIN);

bool SetupMPRLS() {
  if (!mpr.begin()) {
    Serial.println("Failed to communicate with MPRLS pressure sensor");
    return false;
  } else {
    Serial.println("Detected MPRLS pressure sensor");
    return true;
  }
}

void CheckMPRLS() {
  static unsigned int PressureTime;
  if (((millis() - PressureTime) > CHECK_PRESSURE_TIME)) {
    GPS.Pressure = mpr.readPressure();

    PressureTime = millis();
  }
}
#endif
