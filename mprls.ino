#ifdef MPRLS

#include <float.h>
#include <Wire.h>
#include "Adafruit_MPRLS.h"

#define CHECK_PRESSURE_TIME 200 // Sample time in [ms]

// You dont *need* a reset and EOC pin for most uses, so we set to -1 and don't connect
#define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
#define EOC_PIN    -1  // set to any GPIO pin to read end-of-conversion by pin
Adafruit_MPRLS mpr = Adafruit_MPRLS(RESET_PIN, EOC_PIN);

/**
 * @brief Calculate altitude from a given pressure given a standard atmosphere profile
 *
 * @param pressure current pressure in Pa
 * @return altitude in m
 */
static double CalculateAltitude(double pressure) {
    double Pb = 0.0, Tb = 0.0, Hb = 0.0, Lb = 0.0, H;

    if (pressure > 22632.10) {
        Pb = 101325.00;
        Tb = 288.15;
        Hb = 0;
        Lb = -0.0065;
    } else if ((22632.10 >= pressure) && (pressure > 5474.89)) {
        Pb = 22632.10;
        Tb = 216.65;
        Hb = 11000;
        Lb = 0;
    } else if (5474.89 >= pressure) {
        Pb = 5474.89;
        Tb = 216.65;
        Hb = 20000;
        Lb = 0.001;
    }

    if (Lb != 0) {
        H = Hb + (Tb / Lb) * (pow((pressure / Pb), ((-8.3144598 * Lb) / (9.80665 * 0.0289644))) - 1);
    } else {
        H = Hb - (8.3144598 / (9.80665 * 0.0289644)) * Tb * log(pressure / Pb);
    }

    return H;
}

bool SetupMPRLS() {
  GPS.PressureMin = FLT_MAX;
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
    if (GPS.Pressure < GPS.PressureMin) {
      GPS.PressureMin = GPS.Pressure;
      GPS.MaximumBaroAltitude = CalculateAltitude(GPS.PressureMin * 100.0);
    }

    PressureTime = millis();
  }
}
#endif
