/*
 * 
 * 
 */

#ifndef US100Ping_h
#define US100Ping_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <SoftwareSerial.h>

class US100Ping {

  public:
  US100Ping();
  void begin(uint8_t,uint8_t,int);              // software serial begin function(crashes wifi when recieving data at the same time client data is recieved 04/08/2016)
  void begin(HardwareSerial& s);                // hardware serial begin function
  void run();
  int getDistance();
  int getTemperature();
  bool gotDistance();
  bool gotTemperature();
  void enableRx(bool on);

  private:
  //SoftwareSerial *swSer;
  HardwareSerial *swSer;
  
  void startPing();
  void startTemperature();

  byte pingInterval = 24;                       // minimum time between pings
  byte globalUpdateSpeed = 1;                   // limit cpu overhead
  int tempInterval = 5000;                      // time between tests for temperature
  byte sensorTimeout = 100;                     // sensor timeout dont touch
  unsigned long nextMillisPing = millis() + pingInterval;
  unsigned long nextMillisTemp = millis() + tempInterval;
  unsigned long nextMillisGlobal = millis() + globalUpdateSpeed;
  bool pingReady = false;                       // true if it has a valid distance
  bool tempReady = false;                       // true if it has a valid temperature
  bool readingPing = false;                     // true when in the middle of a reading
  bool readingTemp = false;                     // true when in the middle of a reading
  int temp = 0;
  int distance = 0;
  unsigned long currentMillis = millis();
};




#endif
