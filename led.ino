#include <Wire.h>
#include "XPowersLib.h"

extern XPowersLibInterface *PMU;

unsigned long NextLEDs=0;

// LED modes:
// ON at startup
// 4Hz till GPS lock
// 1Hz with GPS lock
// OFF above 1000m (for power save)

void ControlLED(int Mode)
{
  static int OldMode=XPOWERS_CHG_LED_OFF;

  if (PMU && (Mode != OldMode))
  {
    PMU->setChargingLedMode(Mode);

    OldMode = Mode;
  }
}

void SetupLEDs(void)
{
  ControlLED(XPOWERS_CHG_LED_OFF);
}

void CheckLEDs(void)
{
  if (millis() >= NextLEDs)
  {
    if (GPS.Altitude > 1000)
    {
      ControlLED(XPOWERS_CHG_LED_OFF);
    }
    else if (GPS.Satellites >= 4)
    {
      ControlLED(XPOWERS_CHG_LED_BLINK_1HZ);
    }
    else
    {
      ControlLED(XPOWERS_CHG_LED_BLINK_4HZ);
    }      
    
    NextLEDs = millis() + 1000L;
  }
}
