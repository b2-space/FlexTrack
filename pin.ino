#ifdef ENABLE_UPLINK

unsigned long PinOffAt=0;
int ControlPinNumber=0;

void SetupPins(void)
{
  int i;

  Serial.print("Init Output pins");
  
  for (i=0; i<sizeof(PinList); i++)
  {
    Serial.print(' ');
    Serial.print(PinList[i]);
    digitalWrite(PinList[i], 0);
    pinMode (PinList[i], OUTPUT);
  }
  
  Serial.println();
}

int PinIsValid(int Pin)
{
  int i;

  for (i=0; i<sizeof(PinList); i++)
  {
    if (PinList[i] == Pin)
    {
      return 1;
    }
  }
  
  return 0;
}

void ControlPin(int Pin, int Period)
{
  if (ControlPinNumber > 0)
  {
    Serial.print("Switching off pin ");
    Serial.println(ControlPinNumber);
    digitalWrite(ControlPinNumber, 0);
    ControlPinNumber = 0;
    PinOffAt = 0;    
  }
  
  if (PinIsValid(Pin) && (Period > 0) && (Period <= 60))
  {
    digitalWrite (Pin, 1);
    ControlPinNumber = Pin;
    Serial.print("Switching on pin ");
    Serial.println(ControlPinNumber);
    
    PinOffAt = millis() + (long)Period * 1000L;
  }
}

void CheckPins(void)
{
  if ((PinOffAt > 0) && (millis() >= PinOffAt))
  {
    digitalWrite(ControlPinNumber, 0);
    Serial.print("Switching off pin ");
    Serial.println(ControlPinNumber);

    ControlPinNumber = 0;
    PinOffAt = 0;
  }
}

#endif
