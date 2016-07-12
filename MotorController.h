/*
 * 
 * 
 */

#ifndef motorController_h
#define motorController_h
//#include <stdint.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class motorController {

  public:
  motorController(uint8_t , uint8_t  , uint8_t ,uint8_t  , uint8_t ,uint8_t );
  motorController(uint8_t , uint8_t  , uint8_t ,uint8_t );
  void update(int,int);
  void setRange(int);
  void reverseMotorA();
  void reverseMotorB();
  void setTrim(float,float);
  void setSteeringSensitivity(float);
  void setPWMFrequency(int);
  void setMinimumSpeed(float);
  
  private:
  uint8_t mPA1;
  uint8_t mPA2;
  uint8_t mPB1;
  uint8_t mPB2;
  uint8_t PWMA;
  uint8_t PWMB;
  int MAX_range = 500;
  bool useEnablePins;
  int getPWM1(int);
  int getPWM2(int);
  float trimA = 1.0;
  float trimB = 1.0;
  float steeringSensitivity = 1.0;
  bool reverseMotorADirection = false;
  bool reverseMotorBDirection = false;
  int PWMFrequency = 15000; //Theoretical max frequency is 80000000/range, range = 1024 so 78Khz here
  float minMotorSpeed = 0.1;
  // private functions
  float checkNormal(float);
  int checkMinimumSpeed(int);
};


#endif
