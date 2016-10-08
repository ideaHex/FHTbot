
/*
 * This is a non blocking ping library for the ESP8266 it is capable of 100 pings
 * a second with millimeter accuracy without oversampling.
 * 
 * The documentation recommends not pinging faster the 60ms to avoid echo's creating 
 * false readings, however I have tested at 10ms at close range (<200mm) without problems
 * 
 * 
 */
 
#include <ESP8266WiFi.h>
#include "RCW0006Ping.h"

#define TRIGGER D0                                            // Trigger pin
#define ECHO D8                                               // Echo Pin
volatile long pingTime;                                       // time taken to complete ping in uS
volatile int pingDistance = -1;                               // distance in mm
volatile int lastPingDistance = -1;                           // distance in mm

#define MAX_TEST_DISTANCE 5000                                 // in mm
volatile int maxTimeNeeded = (MAX_TEST_DISTANCE / 10.0 * 58.0) / 1000.0;   // in ms
volatile long previousMillis = millis();                       // used for timeout for no ping received
volatile int minimumDelay = 60;                                // to prevent false echo's default 24

  // use getDistance() to trigger next pin, the distance is held in lastPingDistance

void pingSetup(){
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  if (maxTimeNeeded < minimumDelay) maxTimeNeeded = minimumDelay;
}
void startup(){                                                 // ready now, wait for distance
  previousMillis = millis();
  pingTime = micros();
  detachInterrupt(ECHO);
  attachInterrupt(ECHO, calculateDistance, FALLING);
}
void calculateDistance(){
  unsigned long duration = micros() - pingTime - 5;             // added 50 uS to calibrate SQ4 sensor
  pingDistance = int((duration/2.0) / 29.1 * 10.0);
}
void triggerPing(){

  digitalWrite(TRIGGER, LOW);                                   // start low 2uS
  delayMicroseconds(2); 
  
  digitalWrite(TRIGGER, HIGH);                                  // datasheet states a uS 10  pulse to start
  delayMicroseconds(10); 
  
  digitalWrite(TRIGGER, LOW);   

  attachInterrupt(ECHO, startup, RISING);                       // when ready echo goes high
}

int getDistance(){                                              // returns distance or last distance or -1 if not available
  if (pingDistance != -1 && millis() - previousMillis > minimumDelay){
      //Serial.printf("Distance: %d mm \r\n", pingDistance);
      lastPingDistance = pingDistance;                          // last real distance
      pingDistance = -1;
      triggerPing();
    }
    if (millis() - previousMillis > maxTimeNeeded){
      previousMillis = millis();                                // to stop false starts
      pingDistance = -1;
      triggerPing();
    }
    return lastPingDistance;
}

