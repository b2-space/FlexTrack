#ifdef LSM6DSO32

#include <math.h>
#include <Adafruit_LSM6DSO32.h>

#define CHECK_IMU_TIME 1000 // Sample time in [ms]

// IMU configuration parameters
#define IMU_DEFAULT_ACCEL_RANGE LSM6DSO32_ACCEL_RANGE_32_G
#define IMU_DEFAULT_GYRO_RANGE  LSM6DS_GYRO_RANGE_2000_DPS
#define IMU_DEFAULT_ACCEL_ODS   LSM6DS_RATE_104_HZ
#define IMU_DEFAULT_GYRO_ODS    LSM6DS_RATE_104_HZ

Adafruit_LSM6DSO32 dso32;


void ConfigDSO32() {
  dso32.setAccelRange(IMU_DEFAULT_ACCEL_RANGE);
  Serial.print("Accelerometer range set to: ");
  switch (dso32.getAccelRange()) {
  case LSM6DSO32_ACCEL_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case LSM6DSO32_ACCEL_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case LSM6DSO32_ACCEL_RANGE_16_G:
    Serial.println("+-16G");
    break;
  case LSM6DSO32_ACCEL_RANGE_32_G:
    Serial.println("+-32G");
    break;
  }

  dso32.setGyroRange(IMU_DEFAULT_GYRO_RANGE);
  Serial.print("Gyro range set to: ");
  switch (dso32.getGyroRange()) {
  case LSM6DS_GYRO_RANGE_125_DPS:
    Serial.println("125 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_250_DPS:
    Serial.println("250 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_500_DPS:
    Serial.println("500 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_1000_DPS:
    Serial.println("1000 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_2000_DPS:
    Serial.println("2000 degrees/s");
    break;
  case ISM330DHCX_GYRO_RANGE_4000_DPS:
    break; // unsupported range for the DSO32
  }

  dso32.setAccelDataRate(IMU_DEFAULT_ACCEL_ODS);
  Serial.print("Accelerometer data rate set to: ");
  switch (dso32.getAccelDataRate()) {
  case LSM6DS_RATE_SHUTDOWN:
    Serial.println("0 Hz");
    break;
  case LSM6DS_RATE_12_5_HZ:
    Serial.println("12.5 Hz");
    break;
  case LSM6DS_RATE_26_HZ:
    Serial.println("26 Hz");
    break;
  case LSM6DS_RATE_52_HZ:
    Serial.println("52 Hz");
    break;
  case LSM6DS_RATE_104_HZ:
    Serial.println("104 Hz");
    break;
  case LSM6DS_RATE_208_HZ:
    Serial.println("208 Hz");
    break;
  case LSM6DS_RATE_416_HZ:
    Serial.println("416 Hz");
    break;
  case LSM6DS_RATE_833_HZ:
    Serial.println("833 Hz");
    break;
  case LSM6DS_RATE_1_66K_HZ:
    Serial.println("1.66 KHz");
    break;
  case LSM6DS_RATE_3_33K_HZ:
    Serial.println("3.33 KHz");
    break;
  case LSM6DS_RATE_6_66K_HZ:
    Serial.println("6.66 KHz");
    break;
  }

  dso32.setGyroDataRate(IMU_DEFAULT_ACCEL_ODS);
  Serial.print("Gyro data rate set to: ");
  switch (dso32.getGyroDataRate()) {
  case LSM6DS_RATE_SHUTDOWN:
    Serial.println("0 Hz");
    break;
  case LSM6DS_RATE_12_5_HZ:
    Serial.println("12.5 Hz");
    break;
  case LSM6DS_RATE_26_HZ:
    Serial.println("26 Hz");
    break;
  case LSM6DS_RATE_52_HZ:
    Serial.println("52 Hz");
    break;
  case LSM6DS_RATE_104_HZ:
    Serial.println("104 Hz");
    break;
  case LSM6DS_RATE_208_HZ:
    Serial.println("208 Hz");
    break;
  case LSM6DS_RATE_416_HZ:
    Serial.println("416 Hz");
    break;
  case LSM6DS_RATE_833_HZ:
    Serial.println("833 Hz");
    break;
  case LSM6DS_RATE_1_66K_HZ:
    Serial.println("1.66 KHz");
    break;
  case LSM6DS_RATE_3_33K_HZ:
    Serial.println("3.33 KHz");
    break;
  case LSM6DS_RATE_6_66K_HZ:
    Serial.println("6.66 KHz");
    break;
  }
}

bool SetupDSO32() {
  if (!dso32.begin_I2C()) {
    Serial.println("Failed to communicate with LSM6DSO32 IMU sensor");
    return false;
  } else {
    Serial.println("Detected LSM6DSO32 IMU sensor");
    ConfigDSO32();
    return true;
  }
}

void CheckDSO32() {
  static unsigned int IMUTime;
  if (((millis() - IMUTime) > CHECK_IMU_TIME)) {
    // Get a new normalized sensor event
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    dso32.getEvent(&accel, &gyro, &temp);

#ifndef PCT2075
    GPS.ExternalTemperature = temp.temperature;
#endif

    // TODO: acceleration seem to be negative value for all axes

    // /* Display the results (acceleration is measured in m/s^2) */
    // Serial.printf("IMU Acceleration (m/s^2): x:%f y:%f z:%f\n",
    //   accel.acceleration.x, accel.acceleration.y, accel.acceleration.z);
    
    // Store magnitude of the acceleration vector
    GPS.Acceleration = sqrt(accel.acceleration.x * accel.acceleration.x +
      accel.acceleration.y * accel.acceleration.y + accel.acceleration.z * accel.acceleration.z);

    // /* Display the results (rotation is measured in rad/s) */
    // Serial.printf("IMU Rotation (rad/s): x:%f y:%f z:%f\n",
    //   gyro.gyro.x, gyro.gyro.y, gyro.gyro.z);
    
    // Store magnitude of the angular velocity
    GPS.Rotation = sqrt(gyro.gyro.x * gyro.gyro.x + gyro.gyro.y * gyro.gyro.y + gyro.gyro.z * gyro.gyro.z);

    IMUTime = millis();
  }
}
#endif
