/*
 * 
 * 
 */

#ifndef motorController_h
#define motorController_h

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
  void setPWMFrequency(int,int);
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
  float trimA = 1.0;
  float trimB = 1.0;
  float steeringSensitivity = 1.0;
  bool reverseMotorADirection = false;
  bool reverseMotorBDirection = false;
  int PWMFrequency = 1000; //Theoretical max frequency is 80000000/range, range = 1023 so 78Khz here
  int PWMWriteRange = 1023; // 1023 is default for 10 bit,the maximum value can be ~ frequency * 1000 /45. For example, 1KHz PWM, duty range is 0 ~ 22222
  float minMotorSpeed = 0.1;
  
  // private functions
  int getPWM1(int,float);
  int getPWM2(int,float);
  float checkNormal(float);
  int checkMinimumSpeed(int,float);
  
};


#endif
