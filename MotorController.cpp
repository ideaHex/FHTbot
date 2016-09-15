/*
 * 
 * Remember on the ESP8266 pin 16, also called D0 has no PWM output so don't use it for speed/pwm
 * This is a work in progress
 */

#include "MotorController.h"
#include "Encoder.h"

// constructor for h bridge with enable pins for PWM
motorController::motorController(uint8_t motorA_pin_1 , uint8_t motorA_pin_2 , uint8_t PWM_A_pin,uint8_t motorB_pin_1 , uint8_t motorB_pin_2,uint8_t PWM_B_pin){
  analogWriteFreq(PWMFrequency); //Theoretical max frequency is 80000000/range, range = 1024 so 78Khz here
  mPA1 = motorA_pin_1;
  mPA2 = motorA_pin_2;
  mPB1 = motorB_pin_1;
  mPB2 = motorB_pin_2;
  PWMA = PWM_A_pin;
  PWMB = PWM_B_pin;
  useEnablePins = true;
  pinMode(mPA1,OUTPUT);
  pinMode(mPA2,OUTPUT);
  pinMode(mPB1,OUTPUT);
  pinMode(mPB2,OUTPUT);
  pinMode(PWMA,OUTPUT);
  pinMode(PWMB,OUTPUT);
  lastX = 0;
  lastY = 0;
}
// constructor for h bridge without enable pins for PWM
// can also be used for h bridge with direction and speed(PWM) pins using (directionA,Unused pin,SpeedA,directionB,Unused pin,SpeedB);
motorController::motorController(uint8_t motorA_pin_1 , uint8_t motorA_pin_2 ,uint8_t motorB_pin_1 , uint8_t motorB_pin_2){
  analogWriteFreq(PWMFrequency); //Theoretical max frequency is 80000000/range, range = 1024 so 78Khz here
  mPA1 = motorA_pin_1;
  mPA2 = motorA_pin_2;
  mPB1 = motorB_pin_1;
  mPB2 = motorB_pin_2;
  useEnablePins = false;
  pinMode(mPA1,OUTPUT);
  pinMode(mPA2,OUTPUT);
  pinMode(mPB1,OUTPUT);
  pinMode(mPB2,OUTPUT);
  lastX = 0;
  lastY = 0;
}
void motorController::update(int X, int Y){
  if (X > MAX_range)X = MAX_range;
  if (X < -MAX_range)X = -MAX_range;
  if (Y > MAX_range)Y = MAX_range;
  if (Y < -MAX_range)Y = -MAX_range;
  startBoost(&X,&Y); // to overcome initial torque needed to start motors
  //int A = Y - int(steeringSensitivity*float(X*(Y<=0) - X*(Y>0))); // to fix steering when backwards *(Y<0) + X*(Y>0)
  //int B = Y + int(steeringSensitivity*float(X*(Y<=0) - X*(Y>0)));
  //int A = Y - int(steeringSensitivity*float(X*(Y<=0)*(X<0) - X*(Y>0)*(X<0))); // to fix steering when backwards *(Y<0) + X*(Y>0)
  //int B = Y + int(steeringSensitivity*float(X*(Y<=0)*(X>0) - X*(Y>0))*(X>0));
  int A = Y;//int(Y * float( ((float(MAX_range/steeringSensitivity - makePositive(X))) / float(MAX_range/steeringSensitivity))*(X<=0) )) + Y*(X>0);
  int B = Y;//int(Y * float( ((float(MAX_range/steeringSensitivity - makePositive(X))) / float(MAX_range/steeringSensitivity))*(X>=0) )) + Y*(X<0);
  if (A < 0){motorADirection = forward;}else{if (A > 0)motorADirection = reverse;} // if A = 0 coasting in same direction
  if (B < 0){motorBDirection = forward;}else{if (B > 0)motorBDirection = reverse;} // if B = 0 coasting in same direction
  float float_MAX_range = float(MAX_range);
  float minTurn = 0.7;//float(makePositive(Y))/float_MAX_range - 0.2;
  float steeringA = float( (float(float_MAX_range/steeringSensitivity - float(makePositive(X))) / float(float_MAX_range/steeringSensitivity))*float(X<=0) ) + 1.0*(X>0);
  float steeringB = float( (float(float_MAX_range/steeringSensitivity - float(makePositive(X))) / float(float_MAX_range/steeringSensitivity))*float(X>=0) ) + 1.0*(X<0);
  if (steeringA < minTurn) steeringA = minTurn; // limit turning to prevent spinout at high speed
  if (steeringB < minTurn) steeringB = minTurn;
  if (!useEnablePins){// assuming LOW LOW to h bridge is OFF and HIGH HIGH is Break
    
    if (!reverseMotorADirection){
      analogWrite(mPA1, getPWM1(A,trimA)*steeringA);
      analogWrite(mPA2, getPWM2(A,trimA)*steeringA);
    }else{
      analogWrite(mPA1, getPWM2(A,trimA)*steeringA);
      analogWrite(mPA2, getPWM1(A,trimA)*steeringA);
    }
    //Serial.printf("Motor A: 1 %d , 2 %d \r\n",int(getPWM1(A)*trimA),int(getPWM2(A)*trimA));
    //Serial.printf("Motor B: 1 %d , 2 %d \r\n",int(getPWM1(B)*trimB),int(getPWM2(B)*trimB));
    
    if (!reverseMotorBDirection){
      analogWrite(mPB1, getPWM1(B,trimB)*steeringB);
      analogWrite(mPB2, getPWM2(B,trimB)*steeringB);
    }else{
      analogWrite(mPB1, getPWM2(B,trimB)*steeringB);
      analogWrite(mPB2, getPWM1(B,trimB)*steeringB);
    }
    
  }else{ // PWM pins connected to H-Bridge enable
    
    if (!reverseMotorADirection){
      digitalWrite(mPA1, (A<0));
      digitalWrite(mPA2, (A>0));
    }else{
      digitalWrite(mPA1, (A>0));
      digitalWrite(mPA2, (A<0));
    }
    if (A<0) A *= -1;
    analogWrite(PWMA, getPWM1(A,trimA)*steeringA);
    //Serial.printf("Motor A PWM: %d  \r\n", getPWM1(A,trimA)); 
    if (!reverseMotorBDirection){
      digitalWrite(mPB1, (B>0));
      digitalWrite(mPB2, (B<0));
    }else{
      digitalWrite(mPB1, (B<0));
      digitalWrite(mPB2, (B>0));
    }
    if (B<0) B *= -1;
    analogWrite(PWMB, getPWM1(B,trimB)*steeringB);
    //Serial.printf("Motor B PWM: %d \r\n", getPWM1(B,trimB));
  }
}
// maximum input expected from controller input ie 0 to Range
void motorController::setRange(int newRange){
  MAX_range = newRange;
}

int motorController::getPWM1(int A, float trimN){
  int result = (A>0) * ( ((float(float(A)/float(MAX_range))*PWMWriteRange)* (A<MAX_range)) + ((A>=MAX_range) * PWMWriteRange) );
  result = int(checkMinimumSpeed(result, trimN) * trimN);
  return result;
}

int motorController::getPWM2(int A, float trimN){
  int result = (A<0) * ( ((float(float(-A)/float(MAX_range))*PWMWriteRange)* ((-A)<MAX_range)) + (((-A)>=MAX_range) * PWMWriteRange) );
  result = int(checkMinimumSpeed(result, trimN) * trimN);
  return result;
}

void motorController::reverseMotorA(){
  reverseMotorADirection = !reverseMotorADirection;
}

void motorController::reverseMotorB(){
    reverseMotorBDirection = !reverseMotorBDirection;
}

// trim output for motor A & B to compensate for slightly different motors or reduce max - speed
// 0 - 1, 0 = stop 1 = 100 %
void motorController::setTrim(float trimForMotorA ,float trimForMotorB){
  trimA = checkNormal(trimForMotorA);
  trimB = checkNormal(trimForMotorB);
  if (trimA<minMotorSpeed)trimA=minMotorSpeed;
  if (trimB<minMotorSpeed)trimB=minMotorSpeed;
}
// reduce turning speed
void motorController::setSteeringSensitivity(float sensitivity){
  steeringSensitivity = checkNormal(sensitivity);
}

float motorController::checkNormal(float normal){
  if (normal > 1)normal = 1.0;
  if (normal < 0)normal = 0.0;
  return normal;
}
void motorController::setPWMFrequency(int frequency){
  PWMFrequency = frequency;
  analogWriteFreq(PWMFrequency);
}
void motorController::setPWMFrequency(int frequency, int range){
  PWMWriteRange = range;
  PWMFrequency = frequency;
  analogWriteRange(PWMWriteRange);
  analogWriteFreq(PWMFrequency);
}
int motorController::checkMinimumSpeed(int PWM, float trimN){
  if (PWM > 0)PWM = int(PWM*(1.0-minMotorSpeed/trimN) + PWMWriteRange * minMotorSpeed /trimN);
  if (PWM > PWMWriteRange)PWM = PWMWriteRange;
  return PWM;
}
// motor speed expressed as a value of 0.0 - 1.0
void motorController::setMinimumSpeed(float minimumMotorSpeed){
  minMotorSpeed = checkNormal(minimumMotorSpeed);
  if (trimA<minMotorSpeed)trimA=minMotorSpeed;
  if (trimB<minMotorSpeed)trimB=minMotorSpeed;
}
int motorController::makePositive(int number){
  if (number < 0){
    number = -number;
  }
  return number;
}
float motorController::makePositive(float number){
  if (number < 0){
    number = -number;
  }
  return number;
}
void motorController::startBoost(int *X ,int *Y){
  unsigned long currentMillis = millis();
  if (lastY == 0 || boostEndTime > currentMillis){
    float startSpeed = MAX_range * minMotorSpeed * 2.0;
    if ( makePositive(*Y) < startSpeed && *Y != 0){
      if (*Y > 0){
        *Y = startSpeed;
      }
      else{
        *Y = -startSpeed;
      }
      if (boostEndTime < currentMillis) boostEndTime = currentMillis + boostDuration;
    }
  }
  lastX = *X;
  lastY = *Y;
}
void motorController::addEncoders(uint8_t A,uint8_t B){
  encoderA.begin(A , encoderWheelSlots);
  encoderB.begin(B , encoderWheelSlots);
}
void motorController::run(){
  if (encoderA.run()){
      if (motorADirection == forward)heading += anglePerStep;
      else heading -= anglePerStep;
    if (heading > 360)heading -= 360.0;
    else if (heading < 0)heading += 360.0;
   }
   if (encoderB.run()){
      if (motorBDirection == forward) heading -= anglePerStep;
      else heading += anglePerStep;
    if (heading > 360)heading -= 360.0;
    else if (heading < 0)heading += 360.0;
   }
}
double motorController::getheading(){
  return heading;
}
double motorController::getSpeed(){
  double velocity = (encoderA.getAngularVelocity() + encoderB.getAngularVelocity()) / 2.0; // average between both wheels
         velocity = float(velocity * ((wheelDiameter * PI) / 360.0) * 0.0036);  // convert to Kph
         return velocity;
}
double motorController::getTravel(){
   int steps = (encoderA.getSteps() + encoderB.getSteps()) / 2.0;
   int travel = int(steps * distancePerStep );
       return travel;
}
double motorController::getAcceleration(){
  double acceleration = (encoderA.getAngularAcceleration() + encoderB.getAngularAcceleration()) / 2.0;
         acceleration = double(acceleration * ((wheelDiameter * PI) / 360.0) / 1000.0);
         return acceleration;
}
void motorController::playNote(int note,int duration){
  setPWMFrequency(78000);
  yield();
  update(0,-1);
  delay(20);
  double noteFrequency = 1.0 / note ;
  noteFrequency *= 1000000.00;
  double noteHalfDuration = noteFrequency * 0.5; 
  double a=0;
  while (a<(duration/(noteFrequency/1000.0))){
    a++;
  update(0,1000);
  delayMicroseconds(noteHalfDuration);
  update(0,-1000);
  delayMicroseconds(noteHalfDuration);
  }
  update(0,0);
  yield();
  setPWMFrequency(PWMFrequency);
}

