/*
 * 
 * 
 */

#include "US100Ping.h"
#include <SoftwareSerial.h>
US100Ping::US100Ping(){
}

void US100Ping::begin(uint8_t RX, uint8_t TX, int baud){
  //swSer = new SoftwareSerial(RX, TX); // (RX, TX, false, 256) RX,TX,invert,bufferSize
  swSer->begin(baud);
  delay(100);                                     // Give Software Serial time to start
  startTemperature();
  nextMillisPing = millis() + pingInterval;
}

void US100Ping::begin(HardwareSerial& s){
  swSer = &s;
  startTemperature();
  nextMillisPing = millis() + pingInterval;
}

void US100Ping::run(){
// update timers
  currentMillis = millis();
  if (currentMillis > nextMillisGlobal){
  
    nextMillisGlobal = currentMillis + globalUpdateSpeed;
  if (!readingPing && !readingTemp){          // not reading anything so start next action
    
    if (currentMillis > nextMillisTemp){
      startTemperature();
    }else{
      if (currentMillis > nextMillisPing){
        startPing();
      }
    }
    
  }

  if (readingPing){
    if (currentMillis > (nextMillisPing + sensorTimeout - pingInterval)){//timed out
          pingReady = false;
          readingPing = false;
    }
    if(swSer->available() >= 2)                         //when receive 2 bytes 
    {
        readingPing = false;
        unsigned int HighLen = swSer->read();           //High byte of distance
        unsigned int LowLen  = swSer->read();           //Low byte of distance
        distance  = HighLen*256 + LowLen;               //Calculate the distance
        if((distance > 1) && (distance < 10000))        //normal distance should between 1mm and 10000mm (1mm, 10m)
        {
          pingReady = true;
        }else{
          distance = 0;
          pingReady = false;
        }
    }
  }

  if (readingTemp){
    if (currentMillis > (nextMillisTemp + sensorTimeout - tempInterval)){//timed out
          tempReady = false;
          readingTemp = false;
    }
    if(swSer->available() >= 1)                         //when receive 1 byte
    {
        readingTemp = false;
        temp = swSer->read()-45;                        //Temperature
        tempReady = true;
    }
  }
  }
}
void US100Ping::startPing(){
    swSer->flush();                                     // clear receive buffer of serial port
    swSer->write(0X55);                                 // trig US-100 begin to measure the distance
    readingPing = true;
    nextMillisPing = millis() + pingInterval;
}

int US100Ping::getDistance(){
  if (pingReady){
    pingReady = false;
    return distance;
  }else{
    return 0;
  }
}

int US100Ping::getTemperature(){
  if (tempReady){
    tempReady = false;
    return temp;
  }
  return 0;
}

void US100Ping::startTemperature(){
    swSer->flush();                                     // clear receive buffer of serial port
    swSer->write(0X50);                                 // trig US-100 begin to measure temperature
    readingTemp = true;
    nextMillisTemp = millis() + tempInterval;
}

bool US100Ping::gotTemperature(){
  return tempReady;
}

bool US100Ping::gotDistance(){
  return pingReady;
}

void US100Ping::enableRx(bool on){
//  swSer->enableRx(on);
}

