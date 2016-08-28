/*
 * 
 * 
 */

#ifndef Encoder_h
#define Encoder_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class encoder {

  public:
  encoder();
  void begin(uint8_t,uint8_t);
  void run();
  void resetEncoder(); // when direction is stopped / reversed clear data
  double getAngularVelocity();

  private:

  #define MAX_STEP_TIMING_BUFFER 10
  
  uint8_t encoderPin;
  uint8_t slotsPerRevolution;
  unsigned long lastMicros;
  boolean lastState;
  double angularVelocity;
  double instantaniousAngularVelocity;
  double angularAcceleration;                    // degrees per second
  double angularResolution;                      // degrees per step
  int encoderStepTiming[MAX_STEP_TIMING_BUFFER]; // milli seconds of each step
  int encoderStepTimingBufferPosition;
  // private encoder functions

  
};

#endif
