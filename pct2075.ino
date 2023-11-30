#ifdef PCT2075

#include <Adafruit_PCT2075.h>

#define CHECK_TEMPERATURE_TIME 200 // Sample time in [ms]

Adafruit_PCT2075 PCT2075_sensor;

bool SetupPCT2075() {
  PCT2075_sensor = Adafruit_PCT2075();
  if (!PCT2075_sensor.begin()) {
    Serial.println("Failed to communicate with PCT2075 temperature sensor");
    return false;
  } else {
    Serial.println("Detected PCT2075 temperature sensor");
    return true;
  }
}

void CheckPCT2075() {
  static unsigned int TemperatureTime;
  if (((millis() - TemperatureTime) > CHECK_TEMPERATURE_TIME)) {
    GPS.ExternalTemperature = PCT2075_sensor.getTemperature();

    TemperatureTime = millis();
  }
}
#endif
