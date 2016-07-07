


#include "MotorController.h"


// constructor for h bridge with enable pins for PWM
motorController::motorController(uint8_t motorA_pin_1 , uint8_t motorA_pin_2 , uint8_t PWM_A_pin,uint8_t motorB_pin_1 , uint8_t motorB_pin_2,uint8_t PWM_B_pin){
  mPA1 = motorA_pin_1;
  mPA2 = motorA_pin_2;
  mPB1 = motorB_pin_1;
  mPB2 = motorB_pin_2;
  PWMA = PWM_A_pin;
  PWMB = PWM_B_pin;
  MAX_range = 500;
  useEnablePins = true;
  pinMode(mPA1,OUTPUT);
  pinMode(mPA2,OUTPUT);
  pinMode(mPB1,OUTPUT);
  pinMode(mPB2,OUTPUT);
  pinMode(PWMA,OUTPUT);
  pinMode(PWMB,OUTPUT);
}
// constructor for h bridge without enable pins for PWM
motorController::motorController(uint8_t motorA_pin_1 , uint8_t motorA_pin_2 ,uint8_t motorB_pin_1 , uint8_t motorB_pin_2){
  mPA1 = motorA_pin_1;
  mPA2 = motorA_pin_2;
  mPB1 = motorB_pin_1;
  mPB2 = motorB_pin_2;
  PWMA = -1;
  PWMB = -1;
  MAX_range = 500;
  useEnablePins = false;
  pinMode(mPA1,OUTPUT);
  pinMode(mPA2,OUTPUT);
  pinMode(mPB1,OUTPUT);
  pinMode(mPB2,OUTPUT);
}
void motorController::update(int X, int Y){
    int A = Y - X*(Y<=0) + X*(Y>0); // to fix steering when backwards *(Y<0) + X*(Y>0)
    int B = Y + X*(Y<=0) - X*(Y>0);
  if (!useEnablePins){// assuming LOW LOW to h bridge is OFF and HIGH HIGH is Break
    analogWrite(mPA1,getPWM1(A));
    analogWrite(mPA2,getPWM2(A));
    Serial.printf("Motor A: 1 %d , 2 %d \r\n",getPWM1(A),getPWM2(A));
    Serial.printf("Motor B: 1 %d , 2 %d \r\n",getPWM1(B),getPWM2(B));
    // invert motor B ?
    analogWrite(mPB1,getPWM1(B));
    analogWrite(mPB2,getPWM2(B));
  }else{ // PWM pins connected to H-Bridge enable
    digitalWrite(mPA1, (A<0));
    digitalWrite(mPA2, (A>0));
    if (A<0) A *= -1;
    analogWrite(PWMA,getPWM1(A));
    // invert motor B ?
    digitalWrite(mPB1, (B>0));
    digitalWrite(mPB2, (B<0));
    if (B<0) B *= -1;
    analogWrite(PWMB,getPWM1(B));
  }
}

void motorController::range(int newRange){
  MAX_range = newRange;
}

int motorController::getPWM1(int A){
  int result = (A>0) * ( ((float(float(A)/float(MAX_range))*1023)* (A<MAX_range)) + ((A>=MAX_range) * 1023) );
  return result;
}

int motorController::getPWM2(int A){
  int result = (A<0) * ( ((float(float(A*-1)/float(MAX_range))*1023)* ((A*-1)<MAX_range)) + (((A*-1)>=MAX_range) * 1023) );
  return result;
}

void reverseMotorA(){
  
}

void reverseMotorB(){
    
}

