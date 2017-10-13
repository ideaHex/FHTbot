/*
Copyright 2017, Tilden Groves.

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

double currentVoltageMedianSample;
double medianVoltageBuffer[MAX_VOLTAGE_MEDIAN_SAMPLES];
float Voltage = 6;

void updateVoltage(){
	double reading = (double(analogRead(A0)) / 910.0) * 6.0;
	addToVoltageFilter(reading);
}

float getCurrentVoltage(){
  return Voltage;
}

void addToVoltageFilter(double pD){
  currentVoltageMedianSample++;
    for (int a = 0 ; a < MAX_VOLTAGE_MEDIAN_SAMPLES; a++){
        if (medianVoltageBuffer[a] < pD){
          for (int b = MAX_VOLTAGE_MEDIAN_SAMPLES - 1; b > a; b--){
            medianVoltageBuffer[b] = medianVoltageBuffer[b - 1];
          }
          medianVoltageBuffer[a] = pD;
          break;
        }
    }
    if (currentVoltageMedianSample == MAX_VOLTAGE_MEDIAN_SAMPLES){
      Voltage = medianVoltageBuffer[int(MAX_VOLTAGE_MEDIAN_SAMPLES / 2.0)];
      resetVoltageFilter();
    }
}
void resetVoltageFilter(){
  for (int a =0 ; a < MAX_VOLTAGE_MEDIAN_SAMPLES; a++){
    medianVoltageBuffer[a]=-1;
  }
  currentVoltageMedianSample = 0;
}
