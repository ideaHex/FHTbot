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

boolean encoder::run(){
  //boolean encoderCurrentState = boolean(digitalRead(encoderPin));
  unsigned long currentMicros = micros();
  unsigned long deltaMicros = currentMicros - lastMicros;
  if (deltaMicros > TIME_OUT){
      resetEncoder();
      return false;
    }
  
  //if (encoderCurrentState != lastState && deltaMicros > debounceMinStepTime){ // new step
   if ( interuptChanged && deltaMicros > debounceMinStepTime){ // new step
    interuptChanged = false;
    steps++;
    sampleSteps++;
    lastState = !lastState;//encoderCurrentState;
    encoderStepTiming[encoderStepTimingBufferPosition] = deltaMicros;
    // TODO: check timing of previous steps to test for missed steps
    instantaniousAngularVelocity = double( angularResolution / ( (encoderStepTiming[encoderStepTimingBufferPosition]) / 1000000.0) ); // degrees per second
    lastMicros = currentMicros;
    encoderStepTimingBufferPosition++;
    if (encoderStepTimingBufferPosition >= MAX_STEP_TIMING_BUFFER || millis() >= nextUpdate){
      double sampleDeltaT = 0.0;
      for (int a = 0; a < encoderStepTimingBufferPosition; a++){
        sampleDeltaT += encoderStepTiming[a];
      } 
      double lastAngularVelocity = angularVelocity;
      angularVelocity = angularResolution / (double(sampleDeltaT / double(encoderStepTimingBufferPosition)) / 1000000.0);// averaged over the buffer duration
      angularAcceleration = (angularVelocity - lastAngularVelocity) / ((sampleDeltaT + lastSampleDeltaT) / 1000000.0);// dw/dt    degrees per second^2
      lastSampleDeltaT = sampleDeltaT;
      encoderStepTimingBufferPosition = 0;
      lastSampleSteps = sampleSteps;
      sampleSteps = 0;
      nextUpdate = millis() + updateFrequency;
      return true;
    }
    return true;
  }
  interuptChanged = false;
  return false;
}

void encoder::resetEncoder(){
  lastState = boolean(digitalRead(encoderPin));
  lastMicros = micros();
  encoderStepTimingBufferPosition = 0;
  angularVelocity = 0;
  angularAcceleration = 0;
  lastSampleDeltaT = 0;
  steps = 0;
  sampleSteps = 0;
  lastSampleSteps = 0;
  nextUpdate = millis() + updateFrequency;
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
int encoder::getSampleSteps(){
  return lastSampleSteps;
}
void encoder::pinChanged(){
  interuptChanged = true;
}

