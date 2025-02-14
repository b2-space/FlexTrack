
/*-------------------------------------------------------------------------------------------------------\
|                                                                                                        |
| FlexTrack specifically for the TTGO T-Beam which has ESP32, UBlox NEO-6M and LoRa transceiver          |
|                                                                                                        |
| Compared with usual AVR FlexTrack, it has landing prediction and EEPROM configuration.                 |
|                                                                                                        |
\*------------------------------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------------------------------

// CONFIGURATION SECTION.  CHANGE AS REQUIRED.

#define I2C_SLAVE_SDA_PIN           21
#define I2C_SLAVE_SCL_PIN           22
#define I2C_SLAVE_ADDR              0x16        // Comment out to disable I2C slave

// #define OLED                                // Enable OLED

// Optional devices - uncomment if present
// #define BMP085         // Also works with BMP180
// #define BNO085

// Options
#define CUTDOWN             25      // This pin made active to cut down from balloon
#define ENABLE_UPLINK               // Enables uplink code, for which you need to set TDM mode for transmission/reception
#define HEARTBEAT_LED       4       // On-board LED on GPIO4
#define USER_BUTTON         38      // User butotn on GPIO38

#define LED_TX_TIME_MS      (unsigned int)50
#define LED_TX_GPS_TIME_MS  (unsigned int)200
#define LED_RX_TIME_MS      (unsigned int)500

// Pin list
unsigned char PinList[] = {2, 4, 13};

// PRODUCT INFO
#define   VERSION     "V1.48"
#define   PRODUCT     "FlexTrack"
#define   DESCRIPTION "T-Beam with B2Space Mods, adaptative AXP and (opt) I2C slave comms"

// FIXED CONFIG

#define SIG_1   'D'
#define SIG_2   'G'

#define LORA_TIME_INDEX      2
#define LORA_TIME_MUTLIPLER  2
#define LORA_TIME_OFFSET     1
#define LORA_PACKET_TIME    500
#define LORA_OFFSET           0         // Frequency to add in kHz to make Tx frequency accurate

#define LORA_CALL_FREQ     433.650
#define LORA_CALL_MODE     5        


// HARDWARE DEFINITION FOR TTGO T-BEAM

#define OLED_SDA                21
#define OLED_SCL                22
#define OLED_ADDRESS            0x3C
#define SCREEN_WIDTH            128
#define SCREEN_HEIGHT           64


// Libraries

#include <EEPROM.h>

#ifdef OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#endif

#ifdef BMP085
#include <Adafruit_BMP085.h>
#endif

#ifdef BNO085
#include <Adafruit_BNO08x.h>
#include <sh2.h>
#include <sh2_err.h>
#include <sh2_hal.h>
#include <sh2_SensorValue.h>
#include <sh2_util.h>
#include <shtp.h>
#endif

//------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------

// HARDWARE DEFINITION

#define LORA_NSS           18                // Comment out to disable LoRa code
#define LORA_RESET         14                // Comment out if not connected
#define LORA_DIO0          26                
#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI

//------------------------------te------------------------------------------------------------------------

// OLED
#ifdef OLED
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
#endif

// Cutdown ...
#ifdef CUTDOWN
  #define CUTDOWN_FIELD_FORMAT    ",%d"
  #define CUTDOWN_FIELD_LIST       ,GPS.CutdownStatus
#else
  #define CUTDOWN_FIELD_FORMAT    
  #define CUTDOWN_FIELD_LIST          
#endif

// IMU ...
#ifdef BNO085
  #define IMU_FIELD_FORMAT    ",%d,%d,%d"
  #define IMU_FIELD_LIST       ,GPS.Heading, GPS.Pitch, GPS.Roll

#else
  #define IMU_FIELD_FORMAT    
  #define IMU_FIELD_LIST          
#endif


// List of variables/expressions for extra fields. Make empty if no such fields.

#define EXTRA_FIELD_FORMAT    ""
#define EXTRA_FIELD_LIST           

#define SENTENCE_LENGTH       200        // Extend if needed
#define PAYLOAD_LENGTH         16
#define FIELDLIST_LENGTH       24
#define COMMAND_BUFFER_LENGTH  80


//------------------------------------------------------------------------------------------------------
//
//  Globals

typedef enum {
  LORA_POWER_2dBm  =  2,
  LORA_POWER_3dBm  =  3,
  LORA_POWER_4dBm  =  4,
  LORA_POWER_5dBm  =  5,
  LORA_POWER_6dBm  =  6,
  LORA_POWER_7dBm  =  7,
  LORA_POWER_8dBm  =  8,
  LORA_POWER_9dBm  =  9,
  LORA_POWER_10dBm = 10,
  LORA_POWER_11dBm = 11,
  LORA_POWER_12dBm = 12,
  LORA_POWER_13dBm = 13,
  LORA_POWER_14dBm = 14,
  LORA_POWER_15dBm = 15,
  LORA_POWER_16dBm = 16,
  LORA_POWER_17dBm = 17,
  LORA_POWER_20dBm = 20
} LoRa_power;

struct TSettings
{
  // Common
  char PayloadID[PAYLOAD_LENGTH];
  char FieldList[FIELDLIST_LENGTH];
  bool I2CSlave;

  // GPS
  unsigned int FlightModeAltitude;
  int GPSDynamicModel;
  
  // LoRa
  float LoRaFrequency;
//  unsigned char Implicit;             // 1=Implicit, 0=Explicit
//  unsigned char ErrorCoding;
//  unsigned char Bandwidth;
//  unsigned char SpreadingFactor;
//  unsigned char LowDataRateOptimize;
  int           LoRaMode;
  unsigned int  LoRaCycleTime;
  int           LoRaSlot;
  unsigned int  CallingCount;
  int           LoRaRepeatSlot1;
  int           LoRaRepeatSlot2;
  char          UseBinaryMode;
  char          BinaryNode;
  LoRa_power    LoRaPower;
  bool          LoRaNoGPSSync;

  // Cutdown
  long          CutdownAltitude;
  unsigned int  CutdownPeriod;

  // Uplink
  int           EnableUplink;
  char          UplinkCode[16];

  // Prediction
  float         CDA;
  float         PayloadWeight;
  long          LandingAltitude;

  // RTTY
  float         RTTYFrequency;
  int           RTTYBaudRate;
  int           RTTYAudioShift;
  unsigned int  RTTYCount;
  unsigned int  RTTYEvery;
  unsigned int  RTTYPreamble;
} Settings;

bool I2CSlave;

#define EEPROM_SIZE sizeof(Settings)

struct TBinaryPacket
{
	uint8_t 	PayloadIDs;
	uint16_t	Counter;
	uint16_t	BiSeconds;
	float		Latitude;
	float		Longitude;
	int32_t  	Altitude;
};  //  __attribute__ ((packed));

typedef enum {fmIdle, fmLaunched, fmDescending, fmLanding, fmLanded} TFlightMode;

struct TGPS
{
  int Hours, Minutes, Seconds;
  unsigned long SecondsInDay;					// Time in seconds since midnight
  float Longitude, Latitude;
  long Altitude, MinimumAltitude, MaximumAltitude, PreviousAltitude;
  unsigned int Satellites;
  byte FixType;
  byte psm_status;
  float InternalTemperature;
  float BatteryVoltage;
  float ExternalTemperature;
  float Pressure;
  float AscentRate;
  unsigned int BoardCurrent;
  unsigned int errorstatus;
  byte GPSFlightMode;
  TFlightMode FlightMode;
  byte PowerMode;
  int CutdownStatus;
  float PredictedLatitude;
  float PredictedLongitude;
  float CDA;
  int UseHostPosition;
  int TimeTillLanding;
  float PredictedLandingSpeed;
  int LastPacketRSSI;
  int LastPacketSNR;
  int ReceivedCommandCount;
  char LastReceivedCommand[16];
  long CutdownAltitude;
  unsigned int CutdownPeriod;
  int Heading, Pitch, Roll;
  char ExtraFields[128];
  char UplinkText[33];
  unsigned int DataSentTime;
  unsigned int DataReceivedTime;
  unsigned int DataSentLEDOnTime;
  unsigned int DataReceivedLEDOnTime;
} GPS;


unsigned int SentenceCounter=0;

//------------------------------------------------------------------------------------------------------

void setup()
{
  // Serial port(s)

  setCpuFrequencyMhz(80);
  
  Serial.begin(38400);

  // Setup LED
  pinMode(HEARTBEAT_LED, OUTPUT);
  digitalWrite(HEARTBEAT_LED, LOW);

  // Setup User Button
  pinMode(USER_BUTTON, INPUT);

  delay(1000);
  
  Serial.println();
  Serial.println();
  Serial.printf("%s: %s %s\n", PRODUCT, DESCRIPTION, VERSION); 
  Serial.println();

  EEPROM.begin(EEPROM_SIZE);  

  if ((EEPROM.read(0) == SIG_1) && (EEPROM.read(1) == SIG_2))
  {
    // Store current (default) settings
    Serial.println("Loading settings from EEPROM");
    LoadSettings();
  }
  else
  {
    Serial.println("Placing default settings in EEPROM");
    SetDefaults();
  }

  I2CSlave = Settings.I2CSlave;
  
  Serial.print("Payload ID ");
  Serial.println(Settings.PayloadID);

  GPS.UplinkText[0] = '\0';
  GPS.ExtraFields[0] = '\0';

  GPS.DataSentLEDOnTime = LED_TX_TIME_MS;
  GPS.DataReceivedLEDOnTime = LED_RX_TIME_MS;
        
#ifdef CUTDOWN
  GPS.CutdownAltitude = Settings.CutdownAltitude;
  
  SetupCutdown();
#endif

  if (!axp_begin()) {
    Serial.println("AXP error");
    while(1);
  } else {
    axp_config();
    axp_print();
  }

#ifdef OLED
  SetupOLED();
#endif

#ifdef BNO085
  SetupBNO085();
#endif

  SetupGPS();
  
  SetupLoRa();

#ifdef WIREBUS
  // Setupds18b20();
#endif

#ifdef BMP085
  SetupBMP085();
#endif

  SetupPrediction();

#ifdef ENABLE_UPLINK               // Enables uplink, for which you need to set TDM mode for transmission/reception
  SetupPins();
#endif

  SetupLEDs();

if (I2CSlave) {
  SetupSlave();
}

  digitalWrite(HEARTBEAT_LED, HIGH); // Turn off LED after setup
}


void loop()
{  
  CheckGPS();

#ifdef CUTDOWN
  CheckCutdown();
#endif
  
#ifdef ENABLE_UPLINK               // Enables uplink, for which you need to set TDM mode for transmission/reception
  CheckPins();
#endif

  CheckLoRa();

if (!I2CSlave) {
  CheckLEDs();

  axp_check();
}

#ifdef WIREBUS
  Checkds18b20();
#endif

#ifdef BMP085
  CheckBMP085();
#endif

#ifdef BNO085
  CheckBNO085();
#endif

  CheckHost();

  CheckPrediction();

  // Turn off LED after LoRa TX + delay
  if (GPS.DataSentTime && ((millis() - GPS.DataSentTime) > GPS.DataSentLEDOnTime)) {
    GPS.DataSentTime = 0;
    digitalWrite(HEARTBEAT_LED, HIGH);
  }
  // Turn off LED after LoRa RX + delay
  if (GPS.DataReceivedTime && ((millis() - GPS.DataReceivedTime) > GPS.DataReceivedLEDOnTime)) {
    GPS.DataReceivedTime = 0;
    digitalWrite(HEARTBEAT_LED, HIGH);
  }
}

void CheckHost(void)
{
  static char Line[COMMAND_BUFFER_LENGTH];
  static unsigned int Length=0;
  char Character;

  while (Serial.available())
  { 
    Character = Serial.read();

    if (Character == '~')
    {
      Line[0] = Character;
      Length = 1;
    }
    else if (Character == '\r')
    {
      Line[Length] = '\0';
      ProcessCommand(Line+1);
      Length = 0;
    }
    else if (Length >= sizeof(Line))
    {
      Length = 0;
    }
    else if (Length > 0)
    {
      Line[Length++] = Character;
    }
  }
}

void ProcessCommand(char *Line)
{
  int OK = 0;

  if (Line[0] == 'G')
  {
    OK = ProcessGPSCommand(Line+1);
  }
  else if (Line[0] == 'C')
  {
    OK = ProcessCommonCommand(Line+1);
  }
  else if (Line[0] == 'L')
  {
    OK = ProcessLORACommand(Line+1);
  }
  else if (Line[0] == 'R')
  {
    OK = ProcessRTTYCommand(Line+1);
  }
  else if (Line[0] == 'P')
  {
    OK = ProcessPredictionCommand(Line+1);
  }
  else if (Line[0] == 'F')
  {
    OK = ProcessFieldCommand(Line+1);
  }

  if (OK)
  {
    Serial.println("*");
    #ifdef OLED
      ShowSettings();
    #endif
  }
  else
  {
    Serial.println("?");
  }
}

int ProcessFieldCommand(char *Line)
{
  int OK = 0;

  if (Line[0] == 'P')
  {
    GPS.PreviousAltitude = GPS.Altitude;
    sscanf(Line+1,"%f,%f,%ld", &GPS.Latitude, &GPS.Longitude, &GPS.Altitude);
    GPS.UseHostPosition = 5;
    GPS.AscentRate = GPS.AscentRate * 0.7 + (GPS.Altitude - GPS.PreviousAltitude) * 0.3;
    OK = 1;
  }
  
  return OK;
}

void SetDefaults(void)
{
  // Common settings
  strcpy(Settings.PayloadID, "TTGO");

  const static char DefaultFieldList[] = "0123456CD";
  strcpy(Settings.FieldList, (char *)DefaultFieldList);

  Settings.I2CSlave = true;

  // GPS Settings
  Settings.FlightModeAltitude = 2000;
  Settings.GPSDynamicModel = 0;

  // LoRa Settings
  Settings.LoRaFrequency = 434.454;
  Settings.LoRaMode = 2;
  Settings.LoRaCycleTime = 5;
  Settings.LoRaSlot = 1;
  Settings.CallingCount = 0;
  Settings.LoRaRepeatSlot1 = -1;
  Settings.LoRaRepeatSlot2 = -1;
  Settings.UseBinaryMode = 0;
  Settings.BinaryNode = 0;
  Settings.LoRaPower = LORA_POWER_13dBm;
  Settings.LoRaNoGPSSync = false;

  // Cutdown Settings
  Settings.CutdownAltitude = 0;     // Disables cutdown
  Settings.CutdownPeriod = 5000;    // ms

  // Prediction Settings
  Settings.CDA = 0.7;
  Settings.PayloadWeight = 1.0;
  Settings.LandingAltitude = 100;

  // RTTY Settings
  Settings.RTTYFrequency = 434.400;
  Settings.RTTYBaudRate = 300;
  Settings.RTTYAudioShift = 488;
  Settings.RTTYCount = 0;
  Settings.RTTYEvery = 3;
  Settings.RTTYPreamble = 8;
  
  // Uplink Settings
  Settings.EnableUplink = 0;
  Settings.UplinkCode[0] = 0;
  
  SaveSettings();
}

void LoadSettings(void)
{
  unsigned int i;
  unsigned char *ptr;

  ptr = (unsigned char *)(&Settings);
  for (i=0; i<sizeof(Settings); i++, ptr++)
  {
    *ptr = EEPROM.read(i+2);
  }
}

void SaveSettings(void)
{
  unsigned int i;
  unsigned char *ptr;
  
  // Signature
  EEPROM.write(0, SIG_1);
  EEPROM.write(1, SIG_2);

  // Settings
  ptr = (unsigned char *)(&Settings);
  for (i=0; i<sizeof(Settings); i++, ptr++)
  {
    EEPROM.write(i+2, *ptr);
  }

  EEPROM.commit();
}

int ProcessCommonCommand(char *Line)
{
  int OK = 0;

  if (Line[0] == 'P')
  {
    // Store payload ID
    if (strlen(Line+1) < PAYLOAD_LENGTH)
    {
      strcpy(Settings.PayloadID, Line+1);
      OK = 1;
    }
  }
  else if (Line[0] == 'F')
  {
    // Store Field List
    if (strlen(Line+1) < FIELDLIST_LENGTH)
    {
      strcpy(Settings.FieldList, Line+1);
      OK = 1;
    }
  }
  else if (Line[0] == 'R')
  {
    // Reset to default settings
    SetDefaults();
    OK = 1;
  }
  else if (Line[0] == 'S')
  {
    // Save settings to flash
    SaveSettings();
    OK = 1;
  }
  else if (Line[0] == 'E')
  {
    SendSettings();
    OK = 1;
  }
  else if (Line[0] == 'V')
  {
    Serial.printf("VER=%s\n", VERSION);
    OK = 1;
  }
  else if (Line[0] == 'C')
  {
    Serial.printf("PROD=%s\n", PRODUCT);
    OK = 1;
  }
  else if (Line[0] == 'D')
  {
    Serial.printf("DESC=%s\n", DESCRIPTION);
    OK = 1;
  }
  else if (Line[0] == 'A')
  {
    // Cutdown Altitude   
    Settings.CutdownAltitude = atol(Line+1);
    OK = 1;
  }
  else if (Line[0] == 'T')
  {
    // Cutdown Time  
    Settings.CutdownPeriod = atoi(Line+1);
    OK = 1;
  }
  else if (Line[0] == 'I')
  {
    // I2C Slave
    Settings.I2CSlave = (atoi(Line+1) > 0);
    Serial.printf("I2C slave %s. Reset to apply\n",
      Settings.I2CSlave ? "enabled" : "disabled");
    OK = 1;
  }

  return OK;
}

int ProcessLORACommand(char *Line)
{
  int OK = 0;

  if (Line[0] == 'F')
  {
    // New frequency
    double Frequency;
    
    Frequency = atof(Line+1);
    if ((Frequency >= 400) && (Frequency < 1000))
    {
      Settings.LoRaFrequency = Frequency;
      setupRFM98(Settings.LoRaFrequency, Settings.LoRaMode);
      OK = 1;
    }
  }
  else if (Line[0] == 'M')
  {
    // Preset Mode
    int LoRaMode = atoi(Line+1);
    
    if ((LoRaMode >= 0) && (LoRaMode <= 5))
    {
      Settings.LoRaMode = LoRaMode;
      setupRFM98(Settings.LoRaFrequency, Settings.LoRaMode);
      OK = 1;
    }
  }
  else if (Line[0] == 'C')
  {
    // Calling mode count
    int CallingCount = atoi(Line+1);
    
    if ((CallingCount >= 0) && (CallingCount <= 100))
    {
      Settings.CallingCount = CallingCount;
      OK = 1;
    }
  }
  else if (Line[0] == 'T')
  {
    int LoRaCycleTime = atoi(Line+1);
    
    if ((LoRaCycleTime >= 0) && (LoRaCycleTime <= 60))
    {
      Settings.LoRaCycleTime = LoRaCycleTime;
      OK = 1;
    }
  }
  else if (Line[0] == 'O')
  {
    int LoRaSlot = atoi(Line+1);
    
    if ((LoRaSlot >= -1) && (LoRaSlot < 60))
    {
      Settings.LoRaSlot = LoRaSlot;
      OK = 1;
    }
  }
  else if (Line[0] == 'K')
  {
    Settings.EnableUplink = atoi(Line+1);
    OK = 1;
  }
  else if (Line[0] == 'U')
  {
    strncpy(Settings.UplinkCode, Line+1, sizeof(Settings.UplinkCode));
    OK = 1;
    Serial.printf("Password = '%s'\n", Settings.UplinkCode);
  }
  else if (Line[0] == '1')
  {
    Settings.LoRaRepeatSlot1 = atoi(Line+1);
    OK = 1;
  }
  else if (Line[0] == '2')
  {
    Settings.LoRaRepeatSlot2 = atoi(Line+1);
    OK = 1;
  }
  else if (Line[0] == 'Y')
  {
    Settings.UseBinaryMode = atoi(Line+1);
    OK = 1;
  }
  else if (Line[0] == 'N')
  {
    Settings.BinaryNode = atoi(Line+1);
    OK = 1;
  }
  else if (Line[0] == 'P')
  {
    int power = atoi(Line+1);
    switch (power) {
      case LORA_POWER_2dBm:
      case LORA_POWER_3dBm:
      case LORA_POWER_4dBm:
      case LORA_POWER_5dBm:
      case LORA_POWER_6dBm:
      case LORA_POWER_7dBm:
      case LORA_POWER_8dBm:
      case LORA_POWER_9dBm:
      case LORA_POWER_10dBm:
      case LORA_POWER_11dBm:
      case LORA_POWER_12dBm:
      case LORA_POWER_13dBm:
      case LORA_POWER_14dBm:
      case LORA_POWER_15dBm:
      case LORA_POWER_16dBm:
      case LORA_POWER_17dBm:
      case LORA_POWER_20dBm:
        Settings.LoRaPower = (LoRa_power)power;
        Serial.print("LoRa power set to: ");
        Serial.println(power);
        OK = 1;
        break;
      default:
        // Unrecognized power setting
        Serial.print("LoRa power NOT recognized: ");
        Serial.println(power);
        OK = 0;
        break;
    }
  }
  else if (Line[0] == 'G')
  {
    int nogpssync = atoi(Line+1);
    if(nogpssync==0)
    {
      Settings.LoRaNoGPSSync = false; 
      Serial.print("LoRa set synchronization without gps to off");
      OK=1;
    }
    else if(nogpssync==1)
    {
      Settings.LoRaNoGPSSync = true; 
      Serial.print("LoRa set synchronization without gps to on");
      OK=1;
    }
    else
    {
      Serial.print("LoRa noGpssynchronization NOT recognized: ");
      Serial.println(nogpssync);
      OK=0;
    }
  }
  return OK;
}

int ProcessPredictionCommand(char *Line)
{
  int OK = 0;

  if (Line[0] == 'C')
  {
    // CDA
    float CDA = atof(Line+1);
    
    if ((CDA > 0) && (CDA <= 10.0))
    {
      Settings.CDA = CDA;
      OK = 1;
    }
  }
  else if (Line[0] == 'W')
  {
    // Payload Weight
    float PayloadWeight = atof(Line+1);
    
    if ((PayloadWeight > 0) && (PayloadWeight <= 10.0))
    {
      Settings.PayloadWeight = PayloadWeight;
      OK = 1;
    }
  }
  else if (Line[0] == 'L')
  {
    // Landing Altitude
    int LandingAltitude = atol(Line+1);
    
    if ((LandingAltitude >= 0) && (LandingAltitude <= 20))
    {
      Settings.LandingAltitude = LandingAltitude;
      OK = 1;
    }
  }

  return OK;
}

int ProcessGPSCommand(char *Line)
{
  int OK = 0;
  
  if (Line[0] == 'F')
  {
    // Flight mode altitude
    unsigned int Altitude;
    
    Altitude = atoi(Line+1);
    
    if (Altitude < 8000)
    {
      Settings.FlightModeAltitude = Altitude;
      OK = 1;
    }
  }
  else if (Line[0] == 'M')
  {
    Settings.GPSDynamicModel = atoi(Line+1);
    OK = 1;
  }

  return OK;
}

int ProcessRTTYCommand(char *Line)
{
  int OK = 0;

  if (Line[0] == 'F')
  {
    // New frequency
    double Frequency;
    
    Frequency = atof(Line+1);
    if ((Frequency >= 400) && (Frequency < 1000))
    {
      Settings.RTTYFrequency = Frequency;
      OK = 1;
    }
  }
  else if (Line[0] == 'B')
  {
    int BaudRate = atoi(Line+1);
    
    if ((BaudRate >= 50) && (BaudRate <= 1200))
    {
      Settings.RTTYBaudRate = BaudRate;
      OK = 1;
    }
  }
  else if (Line[0] == 'A')
  {
    int RTTYAudioShift = atoi(Line+1);
    
    if ((RTTYAudioShift >= 100) && (RTTYAudioShift <= 1200))
    {
      Settings.RTTYAudioShift = RTTYAudioShift;
      OK = 1;
    }
  }
  else if (Line[0] == 'C')
  {
    int RTTYCount = atoi(Line+1);
    
    if ((RTTYCount >= 0) && (RTTYCount <= 5))
    {
      Settings.RTTYCount = RTTYCount;
      OK = 1;
    }
  }
  else if (Line[0] == 'E')
  {
    int RTTYEvery = atoi(Line+1);
    
    if ((RTTYEvery > 0) && (RTTYEvery <= 100))
    {
      Settings.RTTYEvery = RTTYEvery;
      OK = 1;
    }
  }
  else if (Line[0] == 'P')
  {
    int RTTYPreamble = atoi(Line+1);
    
    if ((RTTYPreamble >= 4) && (RTTYPreamble <= 100))
    {
      Settings.RTTYPreamble = RTTYPreamble;
      OK = 1;
    }
  }

  return OK;
}

void SendSettings(void)
{
  Serial.printf("CP=%s\n", Settings.PayloadID);
  Serial.printf("CF=%s\n", Settings.FieldList);

  Serial.printf("CA=%ld\n", Settings.CutdownAltitude);
  Serial.printf("CT=%u\n", Settings.CutdownPeriod);

  Serial.printf("GF=%u\n", Settings.FlightModeAltitude);
  Serial.printf("GM=%d\n", Settings.GPSDynamicModel);

  Serial.printf("LF=%.4f\n", Settings.LoRaFrequency);
  Serial.printf("LM=%u\n", Settings.LoRaMode);

  Serial.printf("LT=%u\n", Settings.LoRaCycleTime);
  Serial.printf("LO=%u\n", Settings.LoRaSlot);
  Serial.printf("L1=%d\n", Settings.LoRaRepeatSlot1);
  Serial.printf("L2=%d\n", Settings.LoRaRepeatSlot2);
  
  Serial.printf("LB=%d\n", Settings.UseBinaryMode);
  Serial.printf("LN=%d\n", Settings.BinaryNode);

  Serial.printf("LC=%u\n", Settings.CallingCount);

  Serial.printf("LK=%u\n", Settings.EnableUplink);
  Serial.printf("LU=%s\n", Settings.UplinkCode);

  Serial.printf("LP=%d\n", Settings.LoRaPower);

  Serial.printf("PC=%.1f\n", Settings.CDA);
  Serial.printf("PW=%.2f\n", Settings.PayloadWeight);
  Serial.printf("PL=%ld\n", Settings.LandingAltitude);
  
  Serial.printf("RF=%.4f\n", Settings.RTTYFrequency);
  Serial.printf("RB=%d\n", Settings.RTTYBaudRate);
  Serial.printf("RA=%d\n", Settings.RTTYAudioShift);
  Serial.printf("RC=%u\n", Settings.RTTYCount);
  Serial.printf("RE=%u\n", Settings.RTTYEvery);
  Serial.printf("RP=%u\n", Settings.RTTYPreamble);
}
