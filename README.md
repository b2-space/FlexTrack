## FlexTrack 

Flexible Arduino-based tracker software for RTTY, LoRa and APRS.

This fork is configured for Habduino V4.X boards. 

This code provides simultaneous (where legal and supported by the hardware installed) transmission of radio telemetry via RTTY and APRS on Habduino Arduino addon boards for High Altitude Ballooning.

## Libraries

In Arduino add esp32 boards and select *T-Beam*.

This program works on several T-Beam models: 433MHz/868MHz, T-Beam 1.1/2.
It will automatically detect the power supply (it differs from T-Beam 1.1 to 1.2) and select the appropriate code using LilyGo XPowersLib.

To build FlexTrack for use with a HABDuino or other tracker using I2C communications, you need to install this library into your Arduino IDE:

[https://github.com/rambo/I2C](https://github.com/rambo/I2C) 

## Flashing

In Arduino select the COM port of the T-Beam and click on Upload.
Otherwise locate the binary file (can be generated via *Export Compiled Binary* in Arduino) and then use flashing/prog.bat and follow the instructions for flashing the compiled binary into the selected COM port.

## Configuration

Use USB serial communication for debugging telemetry, and reading/modifying setting through commands.
Most configuration is available through a GUI called FlextrakConfig.exe.
Apart from that, there is a ~LPn command which sets the LoRa Power to a number of mW.

## Disclaimer

The FlexTrack code is provided as is with no guarantees of performance or operation. 

If you decide to use this code under a balloon it’s your responsibility to ensure you comply with the local legislation and laws regarding meteorological balloon launching and radio transmission in the air. 
The Radiometrix NTX2B 434Mhz is NOT license exempt in the United States of America and does need a radio amateur license.

Use of APRS requires a radio amateur license in all countries and a number of countries don’t permit the airborne use of APRS under any circumstances. 

It is YOUR responsibility to ensure Habduino hardware and code is used safely and legally please review the safety section on the website. 

## Further Reading on High Altitude Ballooning

Please read this http://www.daveakerman.com/?p=1732

## License

The hardware design & code for Habduino is released under a Creative Commons License 3.0 Attribution-ShareAlike License : http://creativecommons.org/licenses/by-sa/3.0/
