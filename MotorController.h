


#ifndef motorController_h
#define motorController_h
#include <stdint.h>

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
  void range(int);

  private:
  uint8_t mPA1;
  uint8_t mPA2;
  uint8_t mPB1;
  uint8_t mPB2;
  uint8_t PWMA;
  uint8_t PWMB;
  int MAX_range;
  bool useEnablePins;
  int getPWM1(int);
  int getPWM2(int);
  
};




#endif
