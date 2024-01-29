#include <Wire.h>

// #define I2C_SLAVE_DEBUG

typedef enum {
  // Register           // Addr   // R/W  // Type
  REG_LATITUDE          = 0x01,   // R       Float
  REG_LONGITUDE         = 0x02,   // R       Float
  REG_ALTITUDE          = 0x03,   // R       Long
  REG_FIX               = 0x04,   // R       Byte
  REG_SATELLITES        = 0x05,   // R       Byte
  REG_TIME              = 0x06,   // R       char[6] (hhmmss)
  REG_UPLINK_STR_LEN    = 0x10,   // R       size_t
  REG_UPLINK_STR        = 0x11,   // RW      string
  REG_DOWNLINK_STR_LEN  = 0x20,   // R       size_t
  REG_DOWNLINK_STR      = 0x21,   // R       string
} registers;

uint8_t register_address = 0x00;

void onRequest()
{
  uint8_t empty_string[1] = {'\0'};
  uint8_t string_length;
#ifdef I2C_SLAVE_DEBUG
  Serial.printf("onRequest - register: 0x%02X: ", register_address);
#endif
  switch (register_address) {
    case REG_LATITUDE:
      Wire.write((uint8_t *)&GPS.Latitude, 4);
#ifdef I2C_SLAVE_DEBUG
      Serial.printf("send Latitude %d\r\n", GPS.Latitude);
#endif
      break;
    case REG_LONGITUDE:
      Wire.write((uint8_t *)&GPS.Longitude, 4);
#ifdef I2C_SLAVE_DEBUG
      Serial.printf("send Longitude %d\r\n", GPS.Longitude);
#endif
      break;
    case REG_ALTITUDE:
      Wire.write((uint8_t *)&GPS.Altitude, 4);
#ifdef I2C_SLAVE_DEBUG
      Serial.printf("send Altitude %d\r\n", GPS.Altitude);
#endif
      break;
    case REG_FIX:
      Wire.write((uint8_t *)&GPS.FixType, 1);
#ifdef I2C_SLAVE_DEBUG
      Serial.printf("send Fix %d\r\n", GPS.FixType);
#endif
      break;
    case REG_SATELLITES:
    {
      unsigned char satellites = GPS.Satellites;
      Wire.write((uint8_t *)&satellites, 1);
#ifdef I2C_SLAVE_DEBUG
      Serial.printf("send Satellites %d\r\n", satellites);
#endif
      break;
    }
    case REG_TIME:
    {
      char time_str[6+1];
      sprintf(time_str,"%02d%02d%02d", GPS.Hours, GPS.Minutes, GPS.Seconds);
      Wire.write((uint8_t *)time_str, strlen(time_str));
#ifdef I2C_SLAVE_DEBUG
      Serial.printf("send Time %s\r\n", time_str);
#endif
      break;
    }
    case REG_UPLINK_STR_LEN:
      string_length = strlen(GPS.UplinkText);
      Wire.write(&string_length, 1);
#ifdef I2C_SLAVE_DEBUG
      Serial.printf("send uplink str length %d\r\n", strlen(GPS.UplinkText));
#endif
      break;
    case REG_UPLINK_STR:
      if (*GPS.UplinkText) {
        Wire.write((uint8_t *)&GPS.UplinkText, strlen(GPS.UplinkText));
#ifdef I2C_SLAVE_DEBUG
        Serial.printf("send uplink str %s\r\n", GPS.UplinkText);
#endif
        GPS.UplinkText[0] = '\0'; // mark as read
      } else {
        Wire.write(empty_string, 1);
#ifdef I2C_SLAVE_DEBUG
        Serial.println("no uplink str");
#endif
      }
      break;
    case REG_DOWNLINK_STR_LEN:
      string_length = strlen(GPS.ExtraFields);
      Wire.write(&string_length, 1);
#ifdef I2C_SLAVE_DEBUG
      Serial.printf("send downlink str length %d\r\n", strlen(GPS.ExtraFields));
#endif
      break;
    case REG_DOWNLINK_STR:
      if (*GPS.ExtraFields) {
        Wire.write((uint8_t *)&GPS.ExtraFields, strlen(GPS.ExtraFields));
#ifdef I2C_SLAVE_DEBUG
        Serial.printf("send downlink str %s\r\n", GPS.UplinkText);
#endif
      } else {
        Wire.write(empty_string, 1);
#ifdef I2C_SLAVE_DEBUG
        Serial.println("no downlink str");
#endif
      }
      break;
    default:
      Wire.write(empty_string, 1);
      // invalid register address
#ifdef I2C_SLAVE_DEBUG
      Serial.println("Invalid");
#endif
      break;
  }
}

void onReceive(int len)
{
  uint8_t empty_string[1] = {'\0'};
  int i=0;
  uint8_t string_length;
#ifdef I2C_SLAVE_DEBUG
  Serial.printf("onReceive[%dB], ", len);
#endif
  register_address = Wire.read(); // read register address sent by master
#ifdef I2C_SLAVE_DEBUG
  Serial.printf("register: 0x%02X", register_address);
#endif
  // Note: due to esp32 i2c bug, i2c buffer for request has to be previously written with Wire.slaveWrite
  switch (register_address) {
    case REG_LATITUDE:
      Wire.slaveWrite((uint8_t *)&GPS.Latitude, 4);
#ifdef I2C_SLAVE_DEBUG
      if (Wire.available()) {
        Serial.println("Invalid");
      }
#endif
      break;
    case REG_LONGITUDE:
      Wire.slaveWrite((uint8_t *)&GPS.Longitude, 4);
#ifdef I2C_SLAVE_DEBUG
      if (Wire.available()) {
        Serial.println("Invalid");
      }
#endif
      break;
    case REG_ALTITUDE:
      Wire.slaveWrite((uint8_t *)&GPS.Altitude, 4);
#ifdef I2C_SLAVE_DEBUG
      if (Wire.available()) {
        Serial.println("Invalid");
      }
#endif
      break;
    case REG_FIX:
      Wire.slaveWrite((uint8_t *)&GPS.FixType, 1);
#ifdef I2C_SLAVE_DEBUG
      if (Wire.available()) {
        Serial.println("Invalid");
      }
#endif
      break;
    case REG_SATELLITES:
    {
      unsigned char satellites = GPS.Satellites;
      Wire.slaveWrite((uint8_t *)&satellites, 1);
#ifdef I2C_SLAVE_DEBUG
      if (Wire.available()) {
        Serial.println("Invalid");
      }
#endif
      break;
    }
    case REG_TIME:
    {
      char time_str[6+1];
      sprintf(time_str,"%02d%02d%02d", GPS.Hours, GPS.Minutes, GPS.Seconds);
      Wire.slaveWrite((uint8_t *)time_str, strlen(time_str));
#ifdef I2C_SLAVE_DEBUG
      if (Wire.available()) {
        Serial.println("Invalid");
      }
#endif
      break;
    }
    case REG_UPLINK_STR_LEN:
      string_length = strlen(GPS.UplinkText);
      Wire.slaveWrite(&string_length, 1);
#ifdef I2C_SLAVE_DEBUG
      if (Wire.available()) {
        Serial.println("Invalid");
      }
#endif
      break;
    case REG_UPLINK_STR:
      if (*GPS.UplinkText) {
        Wire.slaveWrite((uint8_t *)&GPS.UplinkText, strlen(GPS.UplinkText));
      } else {
        Wire.slaveWrite(empty_string, 1);
      }
#ifdef I2C_SLAVE_DEBUG
      if (Wire.available()) {
        Serial.println("Invalid");
      }
#endif
      break;
    case REG_DOWNLINK_STR_LEN:
      string_length = strlen(GPS.ExtraFields);
      Wire.slaveWrite(&string_length, 1);
#ifdef I2C_SLAVE_DEBUG
      if (Wire.available()) {
        Serial.println("Invalid");
      }
#endif
      break;
    case REG_DOWNLINK_STR:
      if (*GPS.ExtraFields) {
        Wire.slaveWrite((uint8_t *)&GPS.ExtraFields, strlen(GPS.ExtraFields));
      } else {
        Wire.slaveWrite(empty_string, 1);
      }
      // Proccess incoming string
      while(Wire.available())
      {
        GPS.ExtraFields[i++] = Wire.read();
      }
      GPS.ExtraFields[i] = '\0';
#ifdef I2C_SLAVE_DEBUG
      Serial.println(GPS.ExtraFields);
#endif
      break;
    default:
      Wire.slaveWrite(empty_string, 1);
      // invalid register address
      break;
  }
#ifdef I2C_SLAVE_DEBUG
  Serial.println();
#endif
}

void SetupSlave()
{
  Wire.end();
  Serial.println("Setting up I2C slave");
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  Wire.begin((uint8_t)I2C_SLAVE_ADDR);  // , I2C_SLAVE_SDA_PIN, I2C_SLAVE_SCL_PIN);
}
