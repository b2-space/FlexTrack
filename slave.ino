#ifdef I2C_SLAVE_ADDR

#include <Wire.h>

typedef enum {
  // Register           // Addr   // R/W  // Type
  REG_LATITUDE          = 0x01,   // R       Float
  REG_LONGITUDE         = 0x02,   // R       Float
  REG_ALTITUDE          = 0x03,   // R       Long
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
  Serial.printf("onRequest - register: 0x%02X: ", register_address);
  switch (register_address) {
    case REG_LATITUDE:
      Wire.write((uint8_t *)&GPS.Latitude, 4);
      Serial.printf("send Latitude %d\r\n", GPS.Latitude);
      break;
    case REG_LONGITUDE:
      Wire.write((uint8_t *)&GPS.Longitude, 4);
      Serial.printf("send Longitude %d\r\n", GPS.Longitude);
      break;
    case REG_ALTITUDE:
      Wire.write((uint8_t *)&GPS.Altitude, 4);
      Serial.printf("send Altitude %d\r\n", GPS.Altitude);
      break;
    case REG_UPLINK_STR_LEN:
      string_length = strlen(GPS.UplinkText);
      Wire.write(&string_length, 1);
      Serial.printf("send uplink str length %d\r\n", strlen(GPS.UplinkText));
      break;
    case REG_UPLINK_STR:
      if (*GPS.UplinkText) {
        Wire.write((uint8_t *)&GPS.UplinkText, strlen(GPS.UplinkText));
        Serial.printf("send uplink str %s\r\n", GPS.UplinkText);
        GPS.UplinkText[0] = '\0'; // mark as read
      } else {
        Wire.write(empty_string, 1);
        Serial.println("no uplink str");
      }
      break;
    case REG_DOWNLINK_STR_LEN:
      string_length = strlen(GPS.ExtraFields);
      Wire.write(&string_length, 1);
      Serial.printf("send downlink str length %d\r\n", strlen(GPS.ExtraFields));
      break;
    case REG_DOWNLINK_STR:
      if (*GPS.ExtraFields) {
        Wire.write((uint8_t *)&GPS.ExtraFields, strlen(GPS.ExtraFields));
        Serial.printf("send downlink str %s\r\n", GPS.UplinkText);
      } else {
        Wire.write(empty_string, 1);
        Serial.println("no downlink str");
      }
      break;
    default:
      Wire.write(empty_string, 1);
      // invalid register address
      Serial.println("Invalid");
      break;
  }
}

void onReceive(int len)
{
  uint8_t empty_string[1] = {'\0'};
  int i=0;
  uint8_t string_length;
  Serial.printf("onReceive[%dB], ", len);
  register_address = Wire.read(); // read register address sent by master
  Serial.printf("register: 0x%02X", register_address);
  // Note: due to esp32 i2c bug, i2c buffer for request has to be previously written with Wire.slaveWrite
  switch (register_address) {
    case REG_LATITUDE:
      Wire.slaveWrite((uint8_t *)&GPS.Latitude, 4);
      if (Wire.available()) {
        Serial.println("Invalid");
      }
      break;
    case REG_LONGITUDE:
      Wire.slaveWrite((uint8_t *)&GPS.Longitude, 4);
      if (Wire.available()) {
        Serial.println("Invalid");
      }
      break;
    case REG_ALTITUDE:
      Wire.slaveWrite((uint8_t *)&GPS.Altitude, 4);
      if (Wire.available()) {
        Serial.println("Invalid");
      }
      break;
    case REG_UPLINK_STR_LEN:
      string_length = strlen(GPS.UplinkText);
      Wire.slaveWrite(&string_length, 1);
      if (Wire.available()) {
        Serial.println("Invalid");
      }
      break;
    case REG_UPLINK_STR:
      if (*GPS.UplinkText) {
        Wire.slaveWrite((uint8_t *)&GPS.UplinkText, strlen(GPS.UplinkText));
      } else {
        Wire.slaveWrite(empty_string, 1);
      }
      if (Wire.available()) {
        Serial.println("Invalid");
      }
      break;
    case REG_DOWNLINK_STR_LEN:
      string_length = strlen(GPS.ExtraFields);
      Wire.slaveWrite(&string_length, 1);
      if (Wire.available()) {
        Serial.println("Invalid");
      }
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
      Serial.println(GPS.ExtraFields);
      break;
    default:
      Wire.slaveWrite(empty_string, 1);
      // invalid register address
      break;
  }
  Serial.println();
}

void SetupSlave()
{
  Serial.println("Setting up I2C slave");
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  Wire.begin((uint8_t)I2C_SLAVE_ADDR);  // , I2C_SLAVE_SDA_PIN, I2C_SLAVE_SCL_PIN);
}


#endif
