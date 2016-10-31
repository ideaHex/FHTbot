/*
Copyright 2016, Tilden Groves.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
/*
 * This is a non blocking ping library for the ESP8266.
 * 
 * The documentation recommends not pinging faster the 60ms to avoid echo's creating 
 * false readings
 *
 */
 
#include <ESP8266WiFi.h>
#include "RCW0006Ping.h"

#define TRIGGER D0                                            // Trigger pin
#define ECHO D8                                               // Echo Pin
volatile long pingTime;                                       // time taken to complete ping in uS
volatile int pingDistance = -1;                               // distance in mm
volatile int lastPingDistance = -1;                           // distance in mm

#define MAX_TEST_DISTANCE 4500                                // in mm
#define MAX_MEDIAN_SAMPLES 13                                 // use an odd number
volatile int currentMedianSample;
volatile int medianBuffer[MAX_MEDIAN_SAMPLES];
volatile int medianDistance = 500;
volatile int maxTimeNeeded = int((double(MAX_TEST_DISTANCE) / 10.0 * 58.0) / 1000.0);   // in ms
volatile long previousMillis = millis();                       // used for timeout for no ping received
volatile int minimumDelay = 24;                                // to prevent false echo's
volatile long currentTimeout;

void pingSetup(){
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  if (maxTimeNeeded < minimumDelay) maxTimeNeeded = minimumDelay;
  resetFilter();
}
void startup(){                                                 // ready now, wait for distance
  previousMillis = millis();
  pingTime = micros();
  detachInterrupt(ECHO);
  attachInterrupt(ECHO, calculateDistance, FALLING);
}
void calculateDistance(){
  unsigned long duration = micros() - pingTime - 5;             // added 50 uS to calibrate SQ4 sensor
  pingDistance = int((duration/2.0) / 2.91);                    // full equation int((duration/2.0) / 29.1 * 10.0);
  addToFilter(pingDistance);
}
void addToFilter(int pD){
  currentMedianSample++;
    for (int a = 0 ; a < MAX_MEDIAN_SAMPLES; a++){
        if (medianBuffer[a] < pD){
          for (int b = MAX_MEDIAN_SAMPLES - 1; b > a; b--){
            medianBuffer[b] = medianBuffer[b - 1];
          }
          medianBuffer[a] = pD;
          break;
        }
    }
    if (currentMedianSample == MAX_MEDIAN_SAMPLES){
      medianDistance = medianBuffer[int(MAX_MEDIAN_SAMPLES / 2.0)];
//      for (int a = 0; a < MAX_MEDIAN_SAMPLES; a++){
//      Serial.print(" "+String(medianBuffer[a]));
//      }
//      Serial.println();
//      Serial.println("D"+String(medianDistance));
      resetFilter();
    }
}
void resetFilter(){
  for (int a =0 ; a < MAX_MEDIAN_SAMPLES; a++){
    medianBuffer[a]=-1;
  }
  currentMedianSample = 0;
}
int getMedian(){
  return medianDistance;
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
  currentTimeout = millis() - previousMillis;
  if (pingDistance != -1 && currentTimeout > minimumDelay){
      //Serial.printf("Distance: %d mm \r\n", pingDistance);
      lastPingDistance = pingDistance;                          // last real distance
      pingDistance = -1;
      triggerPing();
    }else if (currentTimeout > maxTimeNeeded){
      previousMillis = millis();                                // to stop false starts
      pingDistance = -1;
      triggerPing();
    }
    return lastPingDistance;
}

