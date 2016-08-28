/*
 * 
 * 
 */

 #include "Encoder.h"

 encoder::encoder(){
 }
 
void encoder::begin(uint8_t EncoderPin,uint8_t encoderWheelSlots){
  encoderPin = EncoderPin;
  pinMode(encoderPin, INPUT);
  slotsPerRevolution = encoderWheelSlots;
  angularResolution = 360.0 / slotsPerRevolution;
  resetEncoder();
}

void encoder::run(){
  boolean encoderCurrentState = digitalRead(encoderPin);
  unsigned long currentMicros = micros();
  if (encoderCurrentState != lastState){ // new step
    lastState = encoderCurrentState;
    encoderStepTiming[encoderStepTimingBufferPosition] = int(currentMicros - lastMicros);
    // TODO: check timing of previous steps to test for missed steps
    instantaniousAngularVelocity = double( angularResolution / ( (encoderStepTiming[encoderStepTimingBufferPosition]) / 1000000.0) ); // degrees per second
    lastMicros = currentMicros;
    //Serial.printf("a: %d \r\n",int(instantaniousAngularVelocity));
    encoderStepTimingBufferPosition++;
    if (encoderStepTimingBufferPosition == MAX_STEP_TIMING_BUFFER){ // cyclic buffer
      encoderStepTimingBufferPosition = 0;
      double velocityCount = 0;
      for (int a = 0; a < MAX_STEP_TIMING_BUFFER; a++){
        velocityCount += encoderStepTiming[a];
      }
      angularVelocity = angularResolution / (double(velocityCount / double(MAX_STEP_TIMING_BUFFER)) / 1000000.0);
      //Serial.printf("av: %d deg/s \r\n" , int(velocityCount) );
      double kph = (angularVelocity * ((70 * PI) / 360)) * 0.0036;
      String speedKPH = "Av speed: ";
      speedKPH += kph;
      speedKPH += " Kph";
      //Serial.println(speedKPH);
    }
  }
}

void encoder::resetEncoder(){
  lastState = digitalRead(encoderPin);
  lastMicros = micros();
  encoderStepTimingBufferPosition = 0;
  angularVelocity = 0;
  for (int a = 0; a < MAX_STEP_TIMING_BUFFER; a++){ // clear buffer
    encoderStepTiming[a] = 0;
  }
}

double encoder::getAngularVelocity(){
  return angularVelocity;
}

