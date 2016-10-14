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

