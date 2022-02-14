#ifndef XAAR128_H_
#define XAAR128_H_

#ifdef ARDUINO_AVR_MEGA2560
  // MEGA SPI: SCK: 52, MISO: 50, MOSI: 51

  #define nSS1          10  // Chip select for XARR 128 1st IC
  #define nSS2           7  // Chip select for XARR 128 2nd IC
  #define relayVHCH     49
  #define relayVHCL     48
  
  #define nCLK          11
  #define xVDD          23
  #define SENSOR_SS      3

#elif ARDUINO_AVR_UNO
  // UNO SPI: SCK: 13, MISO: 12, MOSI: 11
  
  #define nSS1          10  // Chip select for XARR 128 1st IC
  #define nSS2           7  // Chip select for XARR 128 2nd IC
  #define relayVHCH     19 
  #define relayVHCL     18 
  
  #define nCLK           9
  #define xVDD           4
  #define SENSOR_SS      3
  
#endif

// Output pins
//int PHO = 5;
#define nRESET           8
#define nFIRE            6

// Input pins
#define READY            2

class Xaar128 {

public:
  int readyState = 0;

  Xaar128();
  void init();
  void powerUp();
  void powerDown();
  void loadBuffer64(byte *val);
  void loadData(byte IMAGE1[16], byte IMAGE2[16]);
  bool fire();
};

#endif
