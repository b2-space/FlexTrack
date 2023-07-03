#ifdef CUTDOWN

unsigned long CutdownOffAt=0;

void SetupCutdown(void)
{
    digitalWrite(CUTDOWN, 0);
    pinMode(CUTDOWN, OUTPUT);
    digitalWrite(CUTDOWN, 0);
}

void CutdownNow(unsigned long Period)
{
  Serial.println("CUTDOWN ON");
  digitalWrite(CUTDOWN, 1);
  CutdownOffAt = millis() + Period;
}

void CheckCutdown(void)
{
  // Don't do anything unless we have GPS
  if (GPS.Satellites >= 4)
  {
    // Arm ?
    
    if ((GPS.Altitude > 2000) && (GPS.CutdownStatus == 0))
    {
      GPS.CutdownStatus = 1;      // Armed
    }

    // Trigger only if armed
    if (GPS.CutdownStatus == 1)
    {
      // ALTITUDE TEST
      if ((GPS.CutdownAltitude > 2000) && (GPS.Altitude > GPS.CutdownAltitude))
      {
        GPS.CutdownStatus = 2;      // Altitude trigger
        CutdownNow(GPS.CutdownPeriod);
      }
    }
  }

  if ((CutdownOffAt > 0) && (millis() >= CutdownOffAt))
  {
    digitalWrite(CUTDOWN, 0);
    Serial.println("CUTDOWN OFF");
    CutdownOffAt = 0;
  }
}

#endif
