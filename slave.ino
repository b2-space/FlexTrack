#ifdef I2C_SLAVE_ADDR

#include <Wire.h>

void onRequest()
{
  if (*GPS.UplinkText)
  {
    Wire.write(GPS.UplinkText);
    Serial.print("onRequest - send '");
    Serial.print(GPS.UplinkText);
    Serial.println("'");
    GPS.UplinkText[0] = '\0';
  }
  else
  {
    Wire.write(0);
    Serial.println("onRequest - nothing to send");
  }
}

void onReceive(int len)
{
  int i=0;
  
  Serial.printf("onReceive[%d]: ", len);
  while(Wire.available())
  {
    GPS.ExtraFields[i++] = Wire.read();
  }
  GPS.ExtraFields[i] = '\0';    
  
  Serial.println(GPS.ExtraFields);
}

void SetupSlave()
{
  Serial.println("Setting up I2C slave");
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  Wire.begin((uint8_t)I2C_SLAVE_ADDR);  // , I2C_SLAVE_SDA_PIN, I2C_SLAVE_SCL_PIN);
}


#endif
