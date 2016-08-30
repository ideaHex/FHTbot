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
  angularResolution = 360.0 / slotsPerRevolution / 2.0;
  resetEncoder();
}

void encoder::run(){
  boolean encoderCurrentState = digitalRead(encoderPin);
  unsigned long currentMicros = micros();
  int deltaMicros = int(currentMicros - lastMicros);
  if (deltaMicros > TIME_OUT){
      resetEncoder();
      return;
    }
  
  if (encoderCurrentState != lastState){ // new step
    steps++;
    lastState = encoderCurrentState;
    encoderStepTiming[encoderStepTimingBufferPosition] = deltaMicros;
    // TODO: check timing of previous steps to test for missed steps
    instantaniousAngularVelocity = double( angularResolution / ( (encoderStepTiming[encoderStepTimingBufferPosition]) / 1000000.0) ); // degrees per second
    lastMicros = currentMicros;
    encoderStepTimingBufferPosition++;
    if (encoderStepTimingBufferPosition == MAX_STEP_TIMING_BUFFER){
      encoderStepTimingBufferPosition = 0;
      double velocityCount = 0;
      for (int a = 0; a < MAX_STEP_TIMING_BUFFER; a++){
        velocityCount += encoderStepTiming[a];
      } 
      double lastAngularVelocity = angularVelocity;
      angularVelocity = angularResolution / (double(velocityCount / double(MAX_STEP_TIMING_BUFFER)) / 1000000.0);// averaged over the buffer duration
      angularAcceleration = (angularVelocity - lastAngularVelocity) / ((velocityCount + lastVelocityCount) / 1000000.0);// dw/dt    degrees per second^2
      lastVelocityCount = velocityCount;
    }
  }
}

void encoder::resetEncoder(){
  lastState = digitalRead(encoderPin);
  lastMicros = micros();
  encoderStepTimingBufferPosition = 0;
  angularVelocity = 0;
  angularAcceleration = 0;
  lastVelocityCount = 0;
  for (int a = 0; a < MAX_STEP_TIMING_BUFFER; a++){ // clear buffer
    encoderStepTiming[a] = 0;
  }
  steps = 0;
}

double encoder::getAngularVelocity(){
  return angularVelocity;
}

unsigned long encoder::getSteps(){
  return steps;
}

double encoder::getAngularAcceleration(){
  return angularAcceleration;
}
