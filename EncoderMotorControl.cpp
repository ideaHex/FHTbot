/*
Copyright 2016, Tilden Groves.

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

#include "EncoderMotorControl.h"

encoderMotorController::encoderMotorController(uint8_t motorA_pin_1 , uint8_t motorA_pin_2 ,uint8_t motorB_pin_1 , uint8_t motorB_pin_2,uint8_t encoderA_pin_1,uint8_t encoderA_pin_2){
  analogWriteFreq(PWMFrequency); //Theoretical max frequency is 80000000/range, range = 1024 so 78Khz here
  motorAPin1 = motorA_pin_1;
  motorAPin2 = motorA_pin_2;
  motorBPin1 = motorB_pin_1;
  motorBPin2 = motorB_pin_2;
  pinMode(motorAPin1,OUTPUT);
  pinMode(motorAPin2,OUTPUT);
  pinMode(motorBPin1,OUTPUT);
  pinMode(motorBPin2,OUTPUT);
  lastX = 0;
  lastY = 0;
  for (int a = 0; a < 2; a++){
    lastSampleDeltaT[a] = 0;
    steps[a] = 0;
    totalSteps[a] = 0;
    lastMicros[a] = micros();
    wheelSpeed[a] = 0;
    motorDirection[a] = forward;
    timeOfFirstStep[a] = 0;
    timeOfLastStep[a] = 0;
    timeOfCurrentStep[a] = 0;
    wheelTargetSpeed[a] = 0;
    boostOn[a] = false;
    debounceMinStepTime[a] = (distancePerStep / (MAX_Speed/3600.0)) * 0.5;
    lastError[a] = 0;
  }

}
 float encoderMotorController::checkNormal(float normal){
  if (normal > 1)normal = 1.0;
  if (normal < 0)normal = 0.0;
  return normal;
 }

 int encoderMotorController::makePositive(int number){
  if (number < 0){
    number = -number;
  }
  return number;
}
 double encoderMotorController::makePositive(double number){
  if (number < 0){
    number = -number;
  }
  return number;
}
void encoderMotorController::playNote(int note,double duration){
  if (note >= NOTE_B0){ 
  delay(1);
  double noteFrequency = 1.0 / note ;
  noteFrequency *= 1000000.00;
  double noteHalfDuration = noteFrequency * 0.5; 
  double a=0;
    while (a<(duration/(noteFrequency/1000.0))){
    a++;
  digitalWrite(motorAPin1,0);
  digitalWrite(motorAPin2,HIGH);
  digitalWrite(motorBPin1,0);
  digitalWrite(motorBPin2,HIGH);
  delayMicroseconds(noteHalfDuration);
  digitalWrite(motorAPin1,HIGH);
  digitalWrite(motorAPin2,0);
  digitalWrite(motorBPin1,HIGH);
  digitalWrite(motorBPin2,0);
  delayMicroseconds(noteHalfDuration);
  }
  analogWrite(motorAPin1,0);
  analogWrite(motorAPin2,0);
  analogWrite(motorBPin1,0);
  analogWrite(motorBPin2,0);
  delay(10);
  }else{
    delay(duration + 11);
  }
}

void encoderMotorController::takeStep(int encoder){
  // debounce
  if ((micros() - lastMicros[encoder]) < debounceMinStepTime[encoder]){
//    if ((micros() - lastMicros[encoder]) > 1000){
//    Serial.println();
//    Serial.println((micros() - lastMicros[encoder]));
//    }
    return;                                         // this is a bounce, ignore it
  }
  lastMicros[encoder] = micros();
  // TODO: update grid
  //updateGrid(encoder);
  
  // update heading + steps
  if (!commandCompleted){// ignore shake
    heading += ((encoder == 1) * motorDirection[encoder] * -anglePerStep ) + ( (encoder == 0 ) * motorDirection[encoder] * anglePerStep );
    if (heading > 360)heading -= 360.0;
    else if (heading < 0)heading += 360.0;
    totalSteps[encoder]++;
    steps[encoder]++;
  }
  // Stop overshoot on start / boost
  if (boostOn[encoder] == true){ // first step after start
      wheelSpeed[encoder] = MIN_Speed;
      if (encoder == 0){
        PWMA = minMotorSpeed * PWMWriteRange;
      }else{
        PWMB = minMotorSpeed * PWMWriteRange;
      }
      boostOn[encoder] = false;
    }

  // update speed buffer / last pulse time 
  if ((micros() - timeOfCurrentStep[encoder]) > minCalculatedSpeedTimePerStep){  // beyond max time so reset minCalculatedSpeedTimePerStep
    timeOfLastStep[encoder] = micros();                   // reset timers
    timeOfCurrentStep[encoder] = timeOfLastStep[encoder];
    timeOfFirstStep[encoder] = timeOfLastStep[encoder];
    wheelSpeed[encoder] = 0;
  }else{
    timeOfLastStep[encoder] = timeOfCurrentStep[encoder];
    timeOfCurrentStep[encoder] = micros();
  }

  // update targets
  
  if (botTargetDistance > 0){
    volatile long currentDistance = ((totalSteps[0] + totalSteps[1]) * 0.5) * distancePerStep;
    if ((botTargetDistance - currentDistance) >= 100){
      wheelTargetSpeed[0] = botmodeSpeed;
      wheelTargetSpeed[1] = botmodeSpeed;
    }
    if ((botTargetDistance - currentDistance) < 100){
      double botRemainingDistance = double(botTargetDistance - currentDistance);
      botTargetSpeed = MIN_Speed + (botmodeSpeed - MIN_Speed)*(botRemainingDistance / 100.0);
    }
    if (currentDistance >= (botTargetDistance - distancePerStep*4.0)){
      botTargetDistance = 0;
      wheelTargetSpeed[0] = 0;
      wheelTargetSpeed[1] = 0;
      PWMA = 0;
      PWMB = 0;
      botTargetSpeed = 0;
      setMotorSpeed();
      commandCompleted = true;
    }
  }
      volatile double headingToTarget = targetHeading - heading;
      if (headingToTarget > 180)headingToTarget-=360;
      if (headingToTarget < -180)headingToTarget+=360;
    if (botTurnDirection == turnLeft || botTurnDirection == turnRight){ // stop overshoot
      if (makePositive(headingToTarget) < (anglePerStep * 4.5 * (MIN_Speed / BASE_MIN_Speed) * 2.0) ){
        if (BASE_MIN_Speed != MIN_Speed && !commandCompleted){
          //Serial.println("Slow" + String(PWMA)+ "Heading" + String(heading) + "WS" + String(wheelSpeed[0])+ "Target" + String(targetHeading));
          slow = true;
          PWMA= (BASE_MIN_Speed / BASE_MAX_Speed) * PWMWriteRange * 1.0;//0.5
          PWMB= (BASE_MIN_Speed / BASE_MAX_Speed) * PWMWriteRange * 1.0;
          if (wheelSpeed[encoder] < MIN_Speed)slow = false; // prevent stall
          setMotorSpeed();
        }else{
            if (!commandCompleted){
              //Serial.println("Slow" + String(PWMA)+ "Heading" + String(heading) + "WS" + String(wheelSpeed[0])+ "Target" + String(targetHeading));
              slow = true;
              if (PWMA > (minMotorSpeed * PWMWriteRange) * 1.0) PWMA = minMotorSpeed * PWMWriteRange * 1.0;
              if (PWMB > (minMotorSpeed * PWMWriteRange) * 1.0) PWMB = minMotorSpeed * PWMWriteRange * 1.0;
              if (wheelSpeed[encoder] < MIN_Speed)slow = false; // prevent stall
              setMotorSpeed();
            }
        }
        if (commandCompleted){
          motorBreak();
          if(nextCommandMillis)nextCommandMillis += 100; // bot is shaking, give it more time to stop
          //Serial.println("Bot rocking");
        }
        if (makePositive(headingToTarget) < (anglePerStep * 0.5) && !commandCompleted){// 0.5
          analogWrite(motorAPin1,(minMotorSpeed * PWMWriteRange) * 0.5 * (motorDirection[0] > 0) );
          analogWrite(motorAPin2,(minMotorSpeed * PWMWriteRange) * 0.5 * (motorDirection[0] < 0) );
          analogWrite(motorBPin1,(minMotorSpeed * PWMWriteRange) * 0.5 * (motorDirection[1] > 0) );
          analogWrite(motorBPin2,(minMotorSpeed * PWMWriteRange) * 0.5 * (motorDirection[1] < 0) );
          PWMA=0;
          PWMB=0;
          slow = false;
          wheelTargetSpeed[0] = 0;
          wheelTargetSpeed[1] = 0;
          //setMotorSpeed();
          //motorBreak();
          commandCompleted = true;
          //Serial.println("Stop, Heading" + String(heading) + "Target" + String(targetHeading));
        }
      }
  }
  
  if (botTurnDirection == none && botTargetSpeed > 0){
//    if (makePositive(headingToTarget) < (anglePerStep * 0.5) ){
//      wheelTargetSpeed[0] = botTargetSpeed;
//      wheelTargetSpeed[1] = botTargetSpeed;
//      PID();
//    }
      //Serial.println("HTT" + String(headingToTarget) + "H" + String(heading) + "TH" + String(targetHeading));
  }
}

void encoderMotorController::manualDrive(int X, int Y){
  if (commandSetHasCommands){
    cancelCommandSet();                                                        // cancel any outstanding turtle / auto mode commands targets etc...
  }
  botTurnDirection = none;                                                     // turn to closest heading
  if (X > MAX_range)X = MAX_range;
  if (X < -MAX_range)X = -MAX_range;
  if (Y > MAX_range)Y = MAX_range;
  if (Y < -MAX_range)Y = -MAX_range;
  // drive steering function
  if (!X && !Y){ // just got 0,0 so stop
    boostOn[0] = false;
    boostOn[1] = false;
  }
  if(X){
    double headingChange = (double(X) / double(MAX_range)) * MAX_heading_Change * double(botTargetDirection);
      if (makePositive(headingChange) > 10){
        targetHeading = heading + headingChange;
      }else{
        targetHeading += headingChange; // if heading is small to prevent reverse rotation
      }
      if (targetHeading > 360)targetHeading -= 360.0;
      if (targetHeading < 0)targetHeading += 360.0;
  }
  
  if (!lastX && !lastX && !Y && !X){// no driving at all.start from standstill, based on input not speed
      targetHeading = heading; // dump any incomplete turning
  }
    /*
    if (lastX > 0 && X < 0){ // change direction
      targetHeading = heading; // dump any incomplete turning
    }
    if (lastX < 0 && X > 0){
      targetHeading = heading; // dump any incomplete turning
    }*/
    if ((lastY > 0 && Y < 0) || (lastY < 0 && Y > 0) ){// just reversed direction
      for (int a=0; a<2 ; a++){
        boostOn[a] = false;
        steps[a] = 0;
        timeOfLastStep[a] = micros();
        timeOfCurrentStep[a] = timeOfLastStep[a];
        timeOfFirstStep[a] = timeOfLastStep[a];
        motorDirection[a] = -motorDirection[a];
      }
      PWMA = PWMWriteRange * 0.5;
      PWMB = PWMWriteRange * 0.5;
      setMotorSpeed();
    }
  lastX = X;
  lastY = Y;
  if (!Y)targetHeading = heading; // not moving forward dump incomplete turning
  if (Y < 0){motorDirection[0] = forward; motorDirection[1] = forward; botTargetDirection = forward;}
  else if (Y > 0) {motorDirection[0] = reverse; motorDirection[1] = reverse; botTargetDirection = reverse;} // if Y = 0 coasting in same direction
  botTargetSpeed = 0;
  if (Y)botTargetSpeed = ( double(makePositive(Y)) / double(MAX_range) ) * double(MAX_Speed - MIN_Speed) + double(MIN_Speed);
  wheelTargetSpeed[0] = botTargetSpeed;
  wheelTargetSpeed[1] = wheelTargetSpeed[0];

  //  TODO: calculate heading change in degrees per second from X and update target heading
  /*
  double headingToTarget = targetHeading - heading;
      if (headingToTarget > 180)headingToTarget-=360;
      if (headingToTarget < -180)headingToTarget+=360;
      if (makePositive(headingToTarget) < 140.0){
        targetDegreesPerSecond = double(X) / double(MAX_range) * MAX_heading_Change;
      }else{targetDegreesPerSecond = 0;}
  */

}

  void encoderMotorController::reverseMotorA(){    // swap pins
    uint8_t holder;
    holder = motorAPin1;
    motorAPin1 = motorAPin2;
    motorAPin2 = holder;
  }
  
  void encoderMotorController::reverseMotorB(){    // swap pins
    uint8_t holder;
    holder = motorBPin1;
    motorBPin1 = motorBPin2;
    motorBPin2 = holder;
  }

void encoderMotorController::updateGrid(int encoder){
  //gridX += (0.5 * distancePerStep * motorDirection[encoder]) * cos(heading + anglePerStep);
  //gridY += (0.5 * distancePerStep * motorDirection[encoder]) * sin(heading + anglePerStep);
}

void encoderMotorController::startCommandSet(String theCommandSet){
  commandSet = theCommandSet;
  if (commandSet.indexOf("data") == -1){ // error, no data found or invalid string
    cancelCommandSet();
    return;
  }
  commandSet.remove(0,5);                // trimm data & comma from front of string
  commandSetHasCommands = true;
  nextCommandMillis = 0;
  targetHeading = heading;
  getNextCommand();
}

boolean encoderMotorController::getNextCommand(){
  allStop();
  botTurnDirection = none;
  String Command;
  float value;
  if (commandSet.length() && commandSetHasCommands){ // has data left
    int indexOfComma = commandSet.indexOf(",");
    Command = commandSet.substring(0,indexOfComma);
    commandSet.remove(0,indexOfComma+1);
    int nextComma =  commandSet.indexOf(",");
    value = commandSet.substring(0,nextComma).toFloat();
    commandSet.remove(0,nextComma+1);
    processCommand(Command , value);
  }else{
    cancelCommandSet();
    return false;
  }
  return true;
}
void encoderMotorController::processCommand(String command, double value){
  // process single character commands
  char oneChar[2];
  command.toCharArray(oneChar,2);
  switch(int(oneChar[0])){
    case 'F':
    {
      botTargetDistance = value * 10 + ((totalSteps[0] + totalSteps[1]) * 0.5) * distancePerStep;
      botTargetSpeed = botmodeSpeed;
      wheelTargetSpeed[0] = botTargetSpeed;
      wheelTargetSpeed[1] = botTargetSpeed;
      motorDirection[0] = forward;
      motorDirection[1] = forward;
      botTargetDirection = forward;
      botTurnDirection = none;
    }
    break;

    case 'B':
    {
      botTargetDistance = value * 10 + ((totalSteps[0] + totalSteps[1]) * 0.5) * distancePerStep;
      botTargetSpeed = botmodeSpeed;
      wheelTargetSpeed[0] = botTargetSpeed;
      wheelTargetSpeed[1] = botTargetSpeed;
      motorDirection[0] = reverse;
      motorDirection[1] = reverse;
      botTargetDirection = reverse;
      botTurnDirection = none;
    }
    break;

    case 'L':
    {
      botTargetSpeed = 0.0;
      targetHeading = targetHeading - value;
      if (targetHeading < 0)targetHeading += 360;
      motorDirection[0] = forward;
      motorDirection[1] = forward;
      botTargetDirection = forward;
      botTurnDirection = turnLeft;
    }
    break;

    case 'R':
    {
      botTargetSpeed = 0.0;
      targetHeading = targetHeading + value;
      if (targetHeading > 360)targetHeading -= 360;
      motorDirection[0] = forward;
      motorDirection[1] = forward;
      botTargetDirection = forward;
      botTurnDirection = turnRight;
    }
    break;
    default:
    break;
  }
  // process longer commands
  if(command.indexOf("AS") != -1){ // all stop
    cancelCommandSet();
  }
}
void encoderMotorController::cancelCommandSet(){
  if (commandSetHasCommands){ // only cancel if running
    allStop();
    botTurnDirection = none;
    commandSet.remove(0);
    commandSetHasCommands = false;
    targetHeading = heading;
  }
}

void encoderMotorController::update(){
  unsigned long lastDeltaT = micros() - lastUpdateMicros;
  unsigned long lastDeltaTA = timeOfCurrentStep[0] - timeOfFirstStep[0];
  unsigned long lastDeltaTB = timeOfCurrentStep[1] - timeOfFirstStep[1];
  lastUpdateMicros = micros(); 
  if (steps[0] > 0)wheelSpeed[0] = (double(steps[0]) * distancePerStep) / double(lastDeltaTA) * 3600.0; // km / hr
  if (steps[1] > 0)wheelSpeed[1] = (double(steps[1]) * distancePerStep) / double(lastDeltaTB) * 3600.0;
  botCurrentSpeed = (wheelSpeed[0] + wheelSpeed[1]) * 0.5;
  //Serial.println("AS" + String(wheelSpeed[0]) + "BS" + String(wheelSpeed[1]));
  //TODO: dynamically adjust debounce baced on wheel speed
  //if (wheelSpeed[0]>MIN_Speed)debounceMinStepTime[0] = (distancePerStep / (wheelSpeed[0]/3600.0)) * 0.5;//wheelSpeed[0]/wheelTargetSpeed[0]
  //if (wheelSpeed[1]>MIN_Speed)debounceMinStepTime[1] = (distancePerStep / (wheelSpeed[1]/3600.0)) * 0.5;

  if ((micros() - timeOfCurrentStep[0]) > minCalculatedSpeedTimePerStep){  // beyond max time so reset minCalculatedSpeedTimePerStep
    timeOfLastStep[0] = micros();                   // reset timers
    timeOfCurrentStep[0] = timeOfLastStep[0];
    timeOfFirstStep[0] = timeOfLastStep[0];
    wheelSpeed[0] = 0;
    if (boostOn[0]){
      increaseMinSpeed(0);
    }
    //Serial.println("STOP0" + String(boostOn[0]));
  }
    if ((micros() - timeOfCurrentStep[1]) > minCalculatedSpeedTimePerStep){
    timeOfLastStep[1] = micros();
    timeOfCurrentStep[1] = timeOfLastStep[1];
    timeOfFirstStep[1] = timeOfLastStep[1];
    wheelSpeed[1] = 0;
    if (boostOn[1]){
      increaseMinSpeed(1);
    }
    //Serial.println("STOP1" + String(boostOn[1]));
  }
if (steps[0] > 0)timeOfFirstStep[0] = timeOfCurrentStep[0];
if (steps[1] > 0)timeOfFirstStep[1] = timeOfCurrentStep[1];
  steps[0] = 0;
  steps[1] = 0;
  
  if (commandCompleted && commandSetHasCommands){
    if (!nextCommandMillis){
      nextCommandMillis = millis() + delaybetweenCommands;
      allStop();
      commandCompleted = true; // just to keep flag
    }
    if (millis() > nextCommandMillis && nextCommandMillis){
      nextCommandMillis = 0;
      getNextCommand();
    }
  }
  updateSteering(lastDeltaT);
  PID();
}

void encoderMotorController::allStop(){
  //       set all target flags to false
  //       set target speed to 0 so PID can slow to stop.
      botTargetDistance = 0;
      wheelTargetSpeed[0] = 0;
      wheelTargetSpeed[1] = 0;
      PWMA = 0;
      PWMB = 0;
      botTargetSpeed = 0;
      setMotorSpeed();
      targetDegreesPerSecond = 0;
      commandCompleted = false;
      slow = false;
      boostOn[0] = false;
      boostOn[1] = false;
}

void encoderMotorController::setMotorSpeed(){
  if (commandCompleted && botTurnDirection != none){
    motorBreak();
  }else{
  //Serial.println("PWMA" + String(PWMA) + "PWMB" + String(PWMB));
  analogWrite(motorAPin1,PWMA * (motorDirection[0] < 0) );
  analogWrite(motorAPin2,PWMA * (motorDirection[0] > 0) );
  analogWrite(motorBPin1,PWMB * (motorDirection[1] < 0) );
  analogWrite(motorBPin2,PWMB * (motorDirection[1] > 0) );
  }
}
void encoderMotorController::setMotorSpeed(int newPWMA, int newPWMB){
  analogWrite(motorAPin1,newPWMA * (motorDirection[0] < 0) );
  analogWrite(motorAPin2,newPWMA * (motorDirection[0] > 0) );
  analogWrite(motorBPin1,newPWMB * (motorDirection[1] < 0) );
  analogWrite(motorBPin2,newPWMB * (motorDirection[1] > 0) );
}
void encoderMotorController::motorBreak(){
  analogWrite(motorAPin1, PWMWriteRange);
  analogWrite(motorAPin2, PWMWriteRange);
  analogWrite(motorBPin1, PWMWriteRange);
  analogWrite(motorBPin2, PWMWriteRange);
  //Serial.println("BREAK");
}
void encoderMotorController::PID(){
      // calculate error
      double errorA = wheelTargetSpeed[0] - wheelSpeed[0];
      double errorB = wheelTargetSpeed[1] - wheelSpeed[1];
      // convert error to PWM
      double maxPWMChange = 100.0;
      int PWMChangeIncreaseA = int(maxPWMChange * (makePositive(errorA) / MAX_Speed));
      int PWMChangeIncreaseB = int(maxPWMChange * (makePositive(errorB) / MAX_Speed));
      int PWMChangeDecreaseA = int(double(PWMChangeIncreaseA) * 0.5);
      int PWMChangeDecreaseB = int(double(PWMChangeIncreaseB) * 0.5);
      if (PWMChangeIncreaseA > PWMWriteRange)PWMChangeIncreaseA = 0;
      if (PWMChangeIncreaseB > PWMWriteRange)PWMChangeIncreaseB = 0;
      if (PWMChangeDecreaseA > PWMWriteRange)PWMChangeDecreaseA = 0;
      if (PWMChangeDecreaseB > PWMWriteRange)PWMChangeDecreaseB = 0;
      //TODO: Prevent overshoot
      /*
      if ((makePositive(lastError[0]) - makePositive(errorA)) > 0){
        PWMChangeIncreaseA = -PWMChangeIncreaseA;
        PWMChangeDecreaseA = -PWMChangeDecreaseA;
      }
      if ((makePositive(lastError[1]) - makePositive(errorB)) > 0){
        PWMChangeIncreaseB = -PWMChangeIncreaseB;
        PWMChangeDecreaseB = -PWMChangeDecreaseB;
      }
      */
      lastError[0] = errorA;
      lastError[1] = errorB;
      // special limits for turning in turtle mode
      if (botTurnDirection == turnLeft || botTurnDirection == turnRight){
        PWMChangeIncreaseA = 7;
        PWMChangeIncreaseB = 7;
        PWMChangeDecreaseA = 2;
        PWMChangeDecreaseB = 2;
        if (commandCompleted || slow){
          if(commandCompleted)motorBreak();
          return;
        }
      }
       // slow to stop
      if (!wheelTargetSpeed[0] && !wheelTargetSpeed[1]){
          if (PWMA > PWMB){PWMB = PWMA;}else{PWMA = PWMB;}
          if (PWMA > 256){
            PWMA = 256;
            PWMB = 256;
          }

      if (PWMA <= 256 && PWMA > 8){
        PWMChangeDecreaseA = 8;
        PWMChangeDecreaseB = 8;
      }
    }
      // Start boost to overcome starting torque
      if ((PWMA == 0) && (wheelTargetSpeed[0] >= MIN_Speed)){
        PWMA = startPWMBoost; // start boost
        timeOfLastStep[0] = micros();
        timeOfCurrentStep[0] = timeOfLastStep[0];
        timeOfFirstStep[0] = timeOfLastStep[0];
        boostOn[0] = true;
        errorA = 0;
      }
      if ((PWMB == 0) && (wheelTargetSpeed[1] >= MIN_Speed)){
        PWMB = startPWMBoost;
        timeOfLastStep[1] = micros();
        timeOfCurrentStep[1] = timeOfLastStep[1];
        timeOfFirstStep[1] = timeOfLastStep[1];
        boostOn[1] = true;
        errorB = 0;
      }
      //if (boostOn[0])errorA = 0;
      //if (boostOn[1])errorB = 0;
      // update power to motor
      if (errorA > 0.1){
        PWMA += PWMChangeIncreaseA;
        if (PWMA > PWMWriteRange)PWMA = PWMWriteRange;
      }
      if (errorA < -0.15){
        PWMA -= PWMChangeDecreaseA;
        if (PWMA < 0)PWMA = 0;
        if (wheelTargetSpeed[0] > 0 && PWMA < ((minMotorSpeed * 0.5) * PWMWriteRange)){ // prevent stall
          PWMA = minMotorSpeed * PWMWriteRange * 0.5;
        }
      }
      if (errorB > 0.1){
        PWMB += PWMChangeIncreaseB;
        if (PWMB > PWMWriteRange)PWMB = PWMWriteRange;
      }
      if (errorB < -0.15){
        PWMB -= PWMChangeDecreaseB;
        if (PWMB < 0)PWMB = 0;
        if (wheelTargetSpeed[1] > 0 && PWMB < ((minMotorSpeed * 0.5) * PWMWriteRange)){ // prevent stall
          PWMB = minMotorSpeed * PWMWriteRange * 0.5;
        }
      }
      // stop if target speed is below minimum
   if ((PWMA < minMotorSpeed * PWMWriteRange) && (wheelTargetSpeed[0] < BASE_MIN_Speed)){
    PWMA = 0;
   }
   if ((PWMB < minMotorSpeed * PWMWriteRange) && (wheelTargetSpeed[1] < BASE_MIN_Speed)){
    PWMB = 0;
   }
   if (!wheelTargetSpeed[0] && !wheelSpeed[0]){
    PWMA = 0;
   }
   if (!wheelTargetSpeed[1] && !wheelSpeed[1]){
    PWMB = 0;
   }
   // test for stalled motor, if battery is low enough the motor can stop at full PWM !
   if ((!boostOn[0] || PWMA == PWMWriteRange) && !wheelSpeed[0] && wheelTargetSpeed[0] && startPWMBoost != PWMWriteRange - 1){ // if motor isnt in start mode and the wheel isn't turning
    increaseMinSpeed(0);
   }
   if ((!boostOn[1] || PWMB == PWMWriteRange) && !wheelSpeed[1] && wheelTargetSpeed[1] && startPWMBoost != PWMWriteRange - 1){
    increaseMinSpeed(1);
   }
   //Serial.println("PWMA" + String(PWMA) + "PWMB" + String(PWMB));
      // send pwm to motors
   setMotorSpeed();
}
double encoderMotorController::getSpeed(){
  return botCurrentSpeed;
}
double encoderMotorController::getheading(){
  return heading;
}
double encoderMotorController::getTravel(){
  return (totalSteps[0]+totalSteps[1]) / 2.0 * distancePerStep;
}
double encoderMotorController::getAcceleration(){
  return 0;
}
void encoderMotorController::updateSteering(long delatT){
  // TODO: update heading baced on degrees per second
  /*
    if (targetDegreesPerSecond != 0){
    //double headingchange = (targetDegreesPerSecond / 1000000.0) * delatT;
    //targetDegreesPerSecond = 0;
   // targetHeading += headingchange * double(botTargetDirection);
    if (targetHeading > 360)targetHeading -= 360;
    if (targetHeading < 0)targetHeading += 360;
    }
    */
      double headingToTarget = targetHeading - heading;
      if (headingToTarget > 180)headingToTarget-=360;
      else if (headingToTarget < -180)headingToTarget+=360;
      wheelTargetSpeed[0] = botTargetSpeed;
      wheelTargetSpeed[1] = botTargetSpeed;
      motorDirection[0] = botTargetDirection;
      motorDirection[1] = botTargetDirection;
      //double changeDegreesPerSecond = 0;

      //double thisMAXSpeed = distancePerDegreeChange * MAX_heading_Change * 0.0036;
      //if (thisMAXSpeed < MIN_Speed)thisMAXSpeed = MIN_Speed;
      double positiveHeadingToTarget = makePositive(headingToTarget); 
      //positiveHeadingToTarget >= anglePerStep  &&     
   if (botTurnDirection != none){ // change speed to correct heading for bot pivit
      if (botTurnDirection == turnRight){
        if (PWMA > startPWMBoost && !boostOn[0]){ // help prevent overshoot
          PWMA = startPWMBoost;
        }
        if (PWMB > startPWMBoost && !boostOn[1]){
          PWMB = startPWMBoost;
        }

        if (positiveHeadingToTarget >= MAX_heading_Change){ // greater than can change in 1 second
            wheelTargetSpeed[0] = MIN_Speed;
            wheelTargetSpeed[1] = MIN_Speed;
            motorDirection[1] = -botTargetDirection;
        }else{// heading is less than max or is 0 change 
            wheelTargetSpeed[0] = MIN_Speed;
            wheelTargetSpeed[1] = MIN_Speed;
            motorDirection[1] = -botTargetDirection;
        }
      }
      
      if (botTurnDirection == turnLeft){
        if (PWMA > startPWMBoost && !boostOn[0]){ // help prevent overshoot
          PWMA = startPWMBoost;
        }
        if (PWMB > startPWMBoost && !boostOn[1]){
          PWMB = startPWMBoost;
        }
        if (positiveHeadingToTarget >= MAX_heading_Change){ // greater than can change in 1 second
            wheelTargetSpeed[0] = MIN_Speed;
            wheelTargetSpeed[1] = MIN_Speed;
            motorDirection[0] = -botTargetDirection;
        }else{// heading is less than max or is 0 change 
          //double thisSpeed = thisMAXSpeed * (positiveHeadingToTarget / MAX_heading_Change);
         // if (thisSpeed < MIN_Speed)thisSpeed = MIN_Speed;
            wheelTargetSpeed[0] = MIN_Speed;
            wheelTargetSpeed[1] = MIN_Speed;
            motorDirection[0] = -botTargetDirection;
        }
      }
      if (commandCompleted){
      wheelTargetSpeed[0] = 0;
      wheelTargetSpeed[1] = 0;
    }
    }
   if ((positiveHeadingToTarget > (anglePerStep * 0.5)) && (botTurnDirection == none)){ // change speed to correct heading for bot drive
    if (botTargetSpeed){  // only when moving at least min speed
        wheelTargetSpeed[0] = botTargetSpeed;
        wheelTargetSpeed[1] = botTargetSpeed;
        double scale = positiveHeadingToTarget / MAX_heading_Change;
        if (scale < 0.3)scale = 0.3;
        double thisSpeed = botTargetSpeed * 0.22 * scale;     //thisMAXSpeed;
          //if (positiveHeadingToTarget < 5 ){         // greater than can change in 1 second
             // thisSpeed = botTargetSpeed * 0.22 * 0.3;
         //   }
        if (headingToTarget > 0 ){ // need to turn right
            wheelTargetSpeed[0] += (thisSpeed * double(botTargetDirection));
            wheelTargetSpeed[1] -= (thisSpeed * double(botTargetDirection));
        }else{
            wheelTargetSpeed[0] -= (thisSpeed * double(botTargetDirection));
            wheelTargetSpeed[1] += (thisSpeed * double(botTargetDirection));
        }
        if (wheelTargetSpeed[0] < MIN_Speed){ // correct speeds
         // wheelTargetSpeed[1] += (MIN_Speed - wheelTargetSpeed[0]);
          wheelTargetSpeed[0] = MIN_Speed;
        }
        if (wheelTargetSpeed[1] < MIN_Speed){ // correct speeds
         // wheelTargetSpeed[0] += MIN_Speed - wheelTargetSpeed[1];
          wheelTargetSpeed[1] = MIN_Speed;
        }
        if (wheelTargetSpeed[0] > MAX_Speed){
          wheelTargetSpeed[1] -= wheelTargetSpeed[0] - MAX_Speed;
          wheelTargetSpeed[0] = MAX_Speed;
        }
        if (wheelTargetSpeed[1] > MAX_Speed){
          wheelTargetSpeed[0] -= wheelTargetSpeed[1] - MAX_Speed;
          wheelTargetSpeed[1] = MAX_Speed;
        }
      }
  }
}
void encoderMotorController::playMarch(){
  updateBPM(104);
  int notes[]{NOTE_A3,NOTE_A3,NOTE_A3,NOTE_F3,NOTE_C4,NOTE_A3,NOTE_F3,NOTE_C4,NOTE_A3,NOTE_E4,NOTE_E4,NOTE_E4,NOTE_F4,NOTE_C4,NOTE_GS3,NOTE_F3,NOTE_C4,NOTE_A3,NOTE_A4,NOTE_A3,NOTE_A3,
  NOTE_A4,NOTE_GS4,NOTE_G4,NOTE_FS4,NOTE_F4,NOTE_FS4,1,NOTE_AS3,NOTE_DS4,NOTE_D4,NOTE_CS4,NOTE_C4,NOTE_B3,NOTE_C4,1,NOTE_F3,NOTE_GS3,NOTE_F3,NOTE_A3,NOTE_C4,NOTE_A3,NOTE_C4,NOTE_E4,
  NOTE_A4,NOTE_A3,NOTE_A3,NOTE_A4,NOTE_GS4,NOTE_G4,NOTE_FS4,NOTE_F4,NOTE_FS4,1,NOTE_AS3,NOTE_DS4,NOTE_D4,NOTE_CS4,NOTE_C4,NOTE_B3,NOTE_C4,1,NOTE_F3,NOTE_GS3,NOTE_F3,NOTE_C4,NOTE_A3,
  NOTE_F3,NOTE_C4,NOTE_A3,0};
  double durations[]{ Qnote,Qnote,Qnote,Enote+Snote,Snote,Qnote,Enote+Snote,Snote,Hnote,Qnote,Qnote,Qnote,Enote+Snote,Snote,Qnote,Enote+Snote,Snote,Hnote,Qnote,Enote+Snote,Snote,Qnote,
  Enote+Snote,Snote,Snote,Snote,Enote,Enote,Enote,Qnote,Enote+Snote,Snote,Snote,Snote,Enote,Enote,Enote,Qnote,Enote+Snote,Snote,Qnote,Enote+Snote,Snote,Hnote,
  Qnote,Enote+Snote,Snote,Qnote,Enote+Snote,Snote,Snote,Snote,Enote,Enote,Enote,Qnote,Enote+Snote,Snote,Snote,Snote,Enote,Enote,Enote,Qnote,Enote+Snote,Snote,Qnote,
  Enote+Snote,Snote,Hnote,0};
  int note = 0;
  while (notes[note]){
    playNote(notes[note],durations[note]);
    note++;
  }
}
void encoderMotorController::playCharge(){
  updateBPM(68);
  playNote(NOTE_G5,Snote);
  playNote(NOTE_C6,Snote);
  playNote(NOTE_E6,Snote);
  playNote(NOTE_G6,Enote);
  playNote(NOTE_E6,Snote);
  playNote(NOTE_G6,Qnote);
}
void encoderMotorController::playVroom(){
  for(int a = NOTE_A1; a < NOTE_F3; a++){ // v - room
    playNote(a,4);
  }
}

void encoderMotorController::playMarioMainThem(){//Mario main them
  int notes[] = { // from the website http://www.princetronics.com/supermariothemesong/
  NOTE_E7, NOTE_E7, 1, NOTE_E7,
  1, NOTE_C7, NOTE_E7, 1,
  NOTE_G7, 1, 1,  1,
  NOTE_G6, 1, 1, 1,
 
  NOTE_C7, 1, 1, NOTE_G6,
  1, 1, NOTE_E6, 1,
  1, NOTE_A6, 1, NOTE_B6,
  1, NOTE_AS6, NOTE_A6, 1,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 1, NOTE_F7, NOTE_G7,
  1, NOTE_E7, 1, NOTE_C7,
  NOTE_D7, NOTE_B6, 1, 1,
 
  NOTE_C7, 1, 1, NOTE_G6,
  1, 1, NOTE_E6, 1,
  1, NOTE_A6, 1, NOTE_B6,
  1, NOTE_AS6, NOTE_A6, 1,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 1, NOTE_F7, NOTE_G7,
  1, NOTE_E7, 1, NOTE_C7,
  NOTE_D7, NOTE_B6, 1, 1, 0
};
double durations[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};
  int note = 0;
  while (notes[note]){
    playNote(notes[note],(1000 / durations[note]*1.5) );
    note++;
  }
}
void encoderMotorController::playMarioUnderworld(){//Underworld melody
int notes[] = {// from the website http://www.princetronics.com/supermariothemesong/
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 1,
  1,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 1,
  1,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 1,
  1,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 1,
  1, NOTE_DS4, NOTE_CS4, NOTE_D4,
  NOTE_CS4, NOTE_DS4,
  NOTE_DS4, NOTE_GS3,
  NOTE_G3, NOTE_CS4,
  NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,
  NOTE_AS3, NOTE_A3, NOTE_GS3,
  1, 1, 1, 0
};
double durations[] = {
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  6, 18, 18, 18,
  6, 6,
  6, 6,
  6, 6,
  18, 18, 18, 18, 18, 18,
  10, 10, 10,
  10, 10, 10,
  3, 3, 3
};
    int note = 0;
  while (notes[note]){
    playNote(notes[note],(1000 / durations[note]*1.5) );
    note++;
  }
}
void encoderMotorController::updateBPM(double thisBPM){
  // DURATION OF THE NOTES 
BPM = thisBPM;
Qnote = 60000/BPM;                                // quarter 1/4 
Hnote = 2*Qnote;                                  // half 2/4
Enote = Qnote/2;                                  // eighth 1/8
Snote = Qnote/4;                                  // sixteenth 1/16
Wnote = 4*Qnote;                                  // whole 4/4
}

void encoderMotorController::hardRightTurn(){     // emergency turn
  PWMA = PWMWriteRange;
  PWMB = minMotorSpeed * PWMWriteRange;
  setMotorSpeed();
}
void encoderMotorController::hardLeftTurn(){     // emergency turn
  PWMB = PWMWriteRange;
  PWMA = minMotorSpeed * PWMWriteRange;
  setMotorSpeed();
}

void encoderMotorController::increaseMinSpeed(int wheel){
     if (!wheel){ // stop wheel completely so next cycle it will get a surge in boost mode
     PWMA = 0;
     }else{
     PWMB = 0;
     }
    timeOfLastStep[wheel] = micros();                             // reset timers
    timeOfCurrentStep[wheel] = timeOfLastStep[wheel];
    timeOfFirstStep[wheel] = timeOfLastStep[wheel];
    wheelSpeed[wheel] = 0;
    startPWMBoost += 50;                                          // increase the start boost to try to stop it next start
    if (startPWMBoost >= PWMWriteRange){
      startPWMBoost = PWMWriteRange - 1;
    }
      MIN_Speed += (BASE_MAX_Speed * 0.05);
      MAX_Speed -= (BASE_MAX_Speed * 0.05);
      if (MAX_Speed < 1.2)MAX_Speed = 1.2;
      if (MIN_Speed > 0.85)MIN_Speed = 0.85;
      if (MIN_Speed > MAX_Speed)MIN_Speed = MAX_Speed;
      minMotorSpeed = MIN_Speed / MAX_Speed;
      botmodeSpeed = ((MAX_Speed - MIN_Speed) * 0.5) + MIN_Speed;
      if (MIN_Speed > (MAX_Speed - (MAX_Speed * 0.1))){
      MIN_Speed = MAX_Speed - (MAX_Speed * 0.1);
    }
    //Serial.println("New Min Speed" + String(MIN_Speed) + "New Max Speed" + String(MAX_Speed));
}

