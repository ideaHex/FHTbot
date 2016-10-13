/*
 * 
 * 
 * 
 * 
 */
#include <ESP8266WiFi.h>
#include "botTemp.h"

#define MAX_SAMPLES 10

long sampleBuffer = 0;
int currentTempSampleNumber = 0;
int temperature = 0;
double beta = 4050; // 25 - 50 = 4050. 25 - 80 = 4101.  25-85 = 4108.  25-100 = 4131.
double resistance = 33.0;

void updateTemperature(){
  if (currentTempSampleNumber < MAX_SAMPLES){
    sampleBuffer += analogRead(A0);
    currentTempSampleNumber++;
  }else{
    double averageSamples = double(double(sampleBuffer) / double(MAX_SAMPLES));
    currentTempSampleNumber = 0;
    sampleBuffer = 0;
    temperature = beta / (log(((1025.0 * resistance / averageSamples) - 33.0) / 33.0) + (beta / 298.0)) - 273.0;
  }
}

int getCurrentTemperature(){
  return temperature;
}

