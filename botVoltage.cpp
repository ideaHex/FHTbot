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
#include "botVoltage.h"

#define MAX_SAMPLES 10

long sampleBuffer = 0;
int currentVoltageSampleNumber = 0;
float Voltage = 0;

void updateVoltage(){
  if (currentVoltageSampleNumber < MAX_SAMPLES){
    sampleBuffer += analogRead(A0);
    currentVoltageSampleNumber++;
  }else{
    double averageSamples = double(double(sampleBuffer) / double(MAX_SAMPLES));
    currentVoltageSampleNumber = 0;
    sampleBuffer = 0;
    Voltage = (averageSamples / 910.0) * 6.0; // 944 default
  }
}

float getCurrentVoltage(){
  return Voltage;
}

