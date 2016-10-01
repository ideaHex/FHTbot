/*
 * 
 * 
 * 
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
    encoderStepTimingBufferPosition[a] = 0;
    lastSampleDeltaT[a] = 0;
    steps[a] = 0;
    totalSteps[a] = 0;
    lastMicros[a] = micros();
    wheelSpeed[a] = 0;
    motorDirection[a] = forward;
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
void encoderMotorController::playNote(int note,int duration){
  analogWriteFreq(78000);
  delay(1);
  double noteFrequency = 1.0 / note ;
  noteFrequency *= 1000000.00;
  double noteHalfDuration = noteFrequency * 0.5; 
  double a=0;
  while (a<(duration/(noteFrequency/1000.0))){
    a++;
  analogWrite(motorAPin1,0);
  analogWrite(motorAPin2,1023);
  analogWrite(motorBPin1,0);
  analogWrite(motorBPin2,1023);
  delayMicroseconds(noteHalfDuration);
  analogWrite(motorAPin1,1023);
  analogWrite(motorAPin2,0);
  analogWrite(motorBPin1,1023);
  analogWrite(motorBPin2,0);
  delayMicroseconds(noteHalfDuration);
  }
  analogWriteFreq(PWMFrequency);
  analogWrite(motorAPin1,0);
  analogWrite(motorAPin2,0);
  analogWrite(motorBPin1,0);
  analogWrite(motorBPin2,0);
  delay(30);
}

void encoderMotorController::takeStep(int encoder){
  // debounce
  if (micros() - lastMicros[encoder] < debounceMinStepTime){
    return;                                         // this is a bounce, ignore it
  }
  lastMicros[encoder] = micros();
  // update grid
  updateGrid(encoder);
  
  // update heading + steps
    heading += ((encoder == 1) * motorDirection[encoder] * -anglePerStep ) + ( (encoder == 0 ) * motorDirection[encoder] * anglePerStep );
    if (heading > 360)heading -= 360.0;
    else if (heading < 0)heading += 360.0;
    totalSteps[encoder]++;
    steps[encoder]++;

  // TODO: update speed buffer / last pulse time ? // optional at this point

  // update targets

  // TODO: check bot targets if commandSetHasCommands == true
  
  if (botTargetDistance > 0){
    volatile long currentDistance = ((totalSteps[0] + totalSteps[1]) * 0.5) * distancePerStep;
    if ((botTargetDistance - currentDistance) >= 70){
      wheelTargetSpeed[0] = botmodeSpeed;
      wheelTargetSpeed[1] = botmodeSpeed;
    }
    if ((botTargetDistance - currentDistance) < 70){
      wheelTargetSpeed[0] = MIN_Speed;
      wheelTargetSpeed[1] = MIN_Speed;
    }
    if (currentDistance >= (botTargetDistance - distancePerStep)){
      botTargetDistance = 0;
      wheelTargetSpeed[0] = 0;
      wheelTargetSpeed[1] = 0;
      PWMA = 0;
      PWMB = 0;
      //targetHeading = heading;
      botTargetSpeed = 0;
      setMotorSpeed(); // STOP !!
      commandCompleted = true;
      //getNextCommand();
    }
  }
      double headingToTarget = targetHeading - heading;
      if (headingToTarget > 180)headingToTarget-=360;
      if (headingToTarget < -180)headingToTarget+=360;
  if (botTurnDirection == turnLeft || botTurnDirection == turnRight){ // stop overshoot
    if (makePositive(headingToTarget) < (anglePerStep * 0.5) ){
      Serial.println("\r\nT" + String(targetHeading) + "H" + String(heading));
      PWMA=0;
      PWMB=0;
      setMotorSpeed(); // STOP !!
      commandCompleted = true;
      //getNextCommand();
    }
  }
  /*
  if (botTurnDirection == none){
    if (makePositive(headingToTarget) < (anglePerStep * 0.5) ){
      if (wheelTargetSpeed[0] < wheelTargetSpeed[1]){
        wheelTargetSpeed[1] = wheelTargetSpeed[0];
        PWMB = PWMA;
        setMotorSpeed();
      }else{
        wheelTargetSpeed[0] = wheelTargetSpeed[1];
        PWMA = PWMB;
        setMotorSpeed();
      }
    }
  }*/
  
  /*
  if (targetHeading != -1.0){ 
      double headingToTarget = targetHeading - heading;
      if (headingToTarget > 180)headingToTarget-=360;
      if (headingToTarget < -180)headingToTarget+=360;
      Serial.println("H " + String(heading) + " TH " + String(targetHeading) + " d " + String(headingToTarget));

      if (makePositive(headingToTarget) < anglePerStep * 1.5){
        if (botTargetDistance == 0){
        targetHeading = -1.0;
        getNextCommand();
        }
        return;
      }
      double targetDegreesPerSecond; //
      if (botTurnDirection == none){
        if (headingToTarget > 0){targetDegreesPerSecond = MAX_heading_Change;
        }else{
          targetDegreesPerSecond = -MAX_heading_Change;
        }
      }else{
        if (makePositive(headingToTarget) > anglePerStep * 2.0){
          if (botTurnDirection == turnRight){
                targetDegreesPerSecond = MAX_heading_Change;
          }else{
                targetDegreesPerSecond = -MAX_heading_Change;
              }
        }else{botTurnDirection = none;}
      }
    //if ((targetHeading - heading)
    motorDirection[0] = botTargetDirection;
    motorDirection[1] = botTargetDirection;

  wheelTargetSpeed[0] = wheelTargetSpeed[0] + ( (targetDegreesPerSecond * motorDirection[0] * 0.5) * (distancePerStep/anglePerStep)/360000.0 )* 3600.0;
  wheelTargetSpeed[1] = wheelTargetSpeed[1] - ( (targetDegreesPerSecond * motorDirection[1] * 0.5) * (distancePerStep/anglePerStep)/360000.0 )* 3600.0;
    if (wheelTargetSpeed[0] > 0 ){
    motorDirection[0] = botTargetDirection;
  }
  if (wheelTargetSpeed[0] < 0){
    wheelTargetSpeed[0] = -wheelTargetSpeed[0];
    motorDirection[0] = -botTargetDirection;
  }
   if (wheelTargetSpeed[1] > 0 ){
    motorDirection[1] = botTargetDirection;
  }
    if (wheelTargetSpeed[1] < 0){
    wheelTargetSpeed[1] = -wheelTargetSpeed[1];
    motorDirection[1] = -botTargetDirection;
  }
  // check to see if target is below max
  if (wheelTargetSpeed[0] > MAX_Speed){
    wheelTargetSpeed[1] -= wheelTargetSpeed[0] - MAX_Speed;
    wheelTargetSpeed[0] = MAX_Speed;
  }
   if (wheelTargetSpeed[1] > MAX_Speed){
    wheelTargetSpeed[0] -= wheelTargetSpeed[1] - MAX_Speed;
    wheelTargetSpeed[1] = MAX_Speed;
  }

  //     TODO::  correct for either wheel not exceeding its min speed or 0
  
  //       should be able to set timer now so the target heading can be calculated in the next update. Target heading +- targetDegreesPerSecond / 1000ms * actual time in ms
  // temporary functions
    if ((wheelTargetSpeed[0] > 0) && (wheelTargetSpeed[0] < MIN_Speed) )wheelTargetSpeed[0] = MIN_Speed;
    if ((wheelTargetSpeed[1] > 0) && (wheelTargetSpeed[1] < MIN_Speed) )wheelTargetSpeed[1] = MIN_Speed;
    Serial.println(wheelTargetSpeed[1]);
    PID(); // to update direction && speed
  }*/
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

  //if (!lastX && !lastY) targetHeading = heading;                               // start from standstill, based on input not speed
  if (!lastX && !lastX && !Y && !X){// no driving at all
    targetHeading = heading; // dump any incomplete turning
  }
  lastX = X;
  lastY = Y;
  
  if (Y < 0){motorDirection[0] = forward; motorDirection[1] = forward; botTargetDirection = forward;}
  else if (Y > 0) {motorDirection[0] = reverse; motorDirection[1] = reverse; botTargetDirection = reverse;} // if Y = 0 coasting in same direction
  botTargetSpeed = 0;
  //botTargetSpeed = -1.0 * (Y !=0 ) * ( ((double(Y) / double(MAX_range)) * double(MAX_Speed - MIN_Speed) ) + double(MIN_Speed * (Y > 0)) - double(MIN_Speed * (Y < 0)));    // cannot drop below min speed if Y != 0
  if (Y)botTargetSpeed = ( double(makePositive(Y)) / double(MAX_range) ) * double(MAX_Speed - MIN_Speed) + double(MIN_Speed);
  wheelTargetSpeed[0] = botTargetSpeed;
  wheelTargetSpeed[1] = wheelTargetSpeed[0];

  //  calculate heading change in degrees per second from X and update target heading
  double headingToTarget = targetHeading - heading;
      if (headingToTarget > 180)headingToTarget-=360;
      if (headingToTarget < -180)headingToTarget+=360;
      if (makePositive(headingToTarget) < 100){
        targetDegreesPerSecond = double(X) / double(MAX_range) * MAX_heading_Change;
      }else{targetDegreesPerSecond = 0;}
  /*
  //  calculate new motor speeds from the above and feed into PID for PWM of each motor
  double distanceInOneSecond = botTargetSpeed / 3600.0; // km / sec, calculate the target distance traveled in a straight line at the targes speed for 1 second
   //       calculate the target distance each wheel needs to travel to get the new heading in 1 second
  double wheelADistance = distanceInOneSecond + ( (targetDegreesPerSecond * motorDirection[0] * 0.5) * (distancePerStep/anglePerStep)/360000.0 );//in km / sec distancePerDegreeChange
  double wheelBDistance = distanceInOneSecond - ( (targetDegreesPerSecond * motorDirection[1] * 0.5) * (distancePerStep/anglePerStep)/360000.0 );
  wheelTargetSpeed[0] = wheelADistance * 3600;// km/hr
  wheelTargetSpeed[1] = wheelBDistance * 3600;// km/hr

  // check direction so encoders know which way to count and motors know which way to go
  // TODO: when motors change direction let PID know so it dosen't over accelerate
    //       if motor reversed set motor direction
  if (wheelTargetSpeed[0] > 0 ){
    if(Y)motorDirection[0] = forward;  //  if(Y !=0 ) to stop change in direction every update
  }
  if (wheelTargetSpeed[0] < 0){
    wheelTargetSpeed[0] = -wheelTargetSpeed[0];
    if(Y)motorDirection[0] = reverse;
  }
   if (wheelTargetSpeed[1] > 0 ){
    if(Y)motorDirection[1] = forward;
  }
    if (wheelTargetSpeed[1] < 0){
    wheelTargetSpeed[1] = -wheelTargetSpeed[1];
    if(Y)motorDirection[1] = reverse;
  }
  // check to see if target is below max
  if (wheelTargetSpeed[0] > MAX_Speed){
    wheelTargetSpeed[1] -= wheelTargetSpeed[0] - MAX_Speed;
    wheelTargetSpeed[0] = MAX_Speed;
  }
   if (wheelTargetSpeed[1] > MAX_Speed){
    wheelTargetSpeed[0] -= wheelTargetSpeed[1] - MAX_Speed;
    wheelTargetSpeed[1] = MAX_Speed;
  }

  //     TODO::  correct for either wheel not exceeding its min speed or 0
  
  //       should be able to set timer now so the target heading can be calculated in the next update. Target heading +- targetDegreesPerSecond / 1000ms * actual time in ms
  // temporary functions
    if ((wheelTargetSpeed[0] > 0) && (wheelTargetSpeed[0] < MIN_Speed) )wheelTargetSpeed[0] = MIN_Speed;
    if ((wheelTargetSpeed[1] > 0) && (wheelTargetSpeed[1] < MIN_Speed) )wheelTargetSpeed[1] = MIN_Speed;
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
  commandSet.remove(0,5); // trimm data & comma from front of string
  commandSetHasCommands = true;
  getNextCommand();
}

boolean encoderMotorController::getNextCommand(){
  allStop();
  String Command;
  float value;
  if (commandSet.length() && commandSetHasCommands){ // has data left
    int indexOfComma = commandSet.indexOf(",");
    Command = commandSet.substring(0,indexOfComma);
    commandSet.remove(0,indexOfComma+1);
    int nextComma =  commandSet.indexOf(",");
    value = commandSet.substring(0,nextComma).toFloat();
    commandSet.remove(0,nextComma+1);
    delay(500);// to stop whiplash from changing direction
    processCommand(Command , value);
  }else{
    cancelCommandSet();
    return false;
  }
  // TODO: set targets for command here, direction speed distance etc..
  return true;
}
void encoderMotorController::processCommand(String command, double value){
  // process single character commands
  char oneChar[2];
  command.toCharArray(oneChar,2);
Serial.println("command processed " + String(oneChar));
  switch(int(oneChar[0])){
    case 'F':
    {
      botTargetDistance = value * 10 + ((totalSteps[0] + totalSteps[1]) * 0.5) * distancePerStep;
      Serial.println("\r\n bot total travel target " + String(botTargetDistance));
      if (targetHeading == -1){
      targetHeading = heading;
      }
      botTargetSpeed = botmodeSpeed;
      wheelTargetSpeed[0] = botmodeSpeed;
      wheelTargetSpeed[1] = wheelTargetSpeed[0];
      motorDirection[0] = forward;
      motorDirection[1] = forward;
      botTargetDirection = forward;
      botTurnDirection = none;
    }
    break;

    case 'B':
    {
      botTargetDistance = value * 10 + ((totalSteps[0] + totalSteps[1]) * 0.5) * distancePerStep;
      Serial.println("\r\n bot total travel target " + String(botTargetDistance));
      if (targetHeading == -1){
      targetHeading = heading;
      }
      botTargetSpeed = botmodeSpeed;
      wheelTargetSpeed[0] = botmodeSpeed;
      wheelTargetSpeed[1] = wheelTargetSpeed[0];
      motorDirection[0] = reverse;
      motorDirection[1] = reverse;
      botTargetDirection = reverse;
      botTurnDirection = none;
    }
    break;

    case 'L':
    {
      Serial.println("\r\n turn Left ");
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
      Serial.println("\r\n turn Right ");
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
    Serial.println("\r\n not single character command");
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
    commandSet.remove(0);
    commandSetHasCommands = false;
  }
}

void encoderMotorController::update(){
  unsigned long lastDeltaT = micros() - lastUpdateMicros;
  lastUpdateMicros = micros();
  botCurrentSpeed = ( (( double(steps[0] + steps[1]) * distancePerStep) * 0.5) / double(lastDeltaT)) * 3600; // km / hr
  wheelSpeed[0] = (double(steps[0]) * distancePerStep) / double(lastDeltaT) * 3600.0;
  wheelSpeed[1] = (double(steps[1]) * distancePerStep) / double(lastDeltaT) * 3600.0;
  
  steps[0] = 0;
  steps[1] = 0;
  
  if (commandCompleted && commandSetHasCommands){
    getNextCommand();
  }
  updateSteering(lastDeltaT);
  PID();
}

void encoderMotorController::allStop(){
      botTargetDistance = 0;
      wheelTargetSpeed[0] = 0;
      wheelTargetSpeed[1] = 0;
      PWMA = 0;
      PWMB = 0;
      //targetHeading = heading;// keep last heading to correct for any overshoot
      botTargetSpeed = 0;
      botTurnDirection = none;
      setMotorSpeed();
      targetDegreesPerSecond = 0;
      commandCompleted = false;
  // TODO: set all target flags to false
  //       set target speed to 0 so PID can slow to stop.
}

void encoderMotorController::setMotorSpeed(){
  analogWrite(motorAPin1,PWMA * (motorDirection[0] < 0) );
  analogWrite(motorAPin2,PWMA * (motorDirection[0] > 0) );
  analogWrite(motorBPin1,PWMB * (motorDirection[1] < 0) );
  analogWrite(motorBPin2,PWMB * (motorDirection[1] > 0) );
}

void encoderMotorController::PID(){
  //TODO: calculate PID for motor PWM baced on motor target speed
  // currently being called every 50 mS (20 Hz)
  // wheelSpeed[0];               // is current left motor speed in km / hr
  // wheelSpeed[1];               // is current right motor speed in km / hr
  // wheelTargetSpeed[0];         // is the left motor target speed
  // wheelTargetSpeed[1];         // is the right motor target speed
  // MAX_Speed;                   // is bot maximum speed
  // MIN_Speed;                   // minimum speed to prevent stall, takes about 50% more to overcome starting torque

// The following functions are a temporary fix for testing

      double errorA = wheelTargetSpeed[0] - wheelSpeed[0];
      double errorB = wheelTargetSpeed[1] - wheelSpeed[1];

      int PWMChange = 10;
      if (!wheelTargetSpeed[0] && !wheelTargetSpeed[1]){
          if (PWMA > PWMB){PWMB = PWMA;}else{PWMA = PWMB;}
          if (PWMA > 256){
            PWMA = 256;
            PWMB = 256;
          }

      if (PWMA < 200){
        //PWMChange = int(PWMChange * 0.5);
      }
    }
      if (errorA > 0.01 && PWMA < (1023 - PWMChange)){
        PWMA += PWMChange;
      }
      if (errorA < -0.01 && PWMA > PWMChange){
        PWMA -= PWMChange;
      }
      if (errorB > 0.01 && PWMB < (1023 - PWMChange)){
        PWMB += PWMChange;
      }
      if (errorB < -0.01 && PWMB > PWMChange){
        PWMB -= PWMChange;
      }
   if ((PWMA < minMotorSpeed * 1023) && (wheelTargetSpeed[0] < MIN_Speed)){
    PWMA = 0;
   }
   if ((PWMB < minMotorSpeed * 1023) && (wheelTargetSpeed[1] < MIN_Speed)){
    PWMB = 0;
   }
   if ((PWMA < minMotorSpeed * 1023) && (wheelTargetSpeed[0] >= MIN_Speed)){
    PWMA = minMotorSpeed * 1023 * 1.2; // start boost
   }
   if ((PWMB < minMotorSpeed * 1023) && (wheelTargetSpeed[1] >= MIN_Speed)){
    PWMB = minMotorSpeed * 1023 * 1.2;
   }
  setMotorSpeed();      // output pwm to motors range is 0 - 1023
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

    if (targetDegreesPerSecond != 0){
    double headingchange = (targetDegreesPerSecond / 1000000.0) * delatT;
    //targetDegreesPerSecond = 0;
    targetHeading += headingchange * botTargetDirection;
    if (targetHeading > 360)targetHeading -= 360;
    if (targetHeading < 0)targetHeading += 360;
    }
      double headingToTarget = targetHeading - heading;
      if (headingToTarget > 180)headingToTarget-=360;
      if (headingToTarget < -180)headingToTarget+=360;
      wheelTargetSpeed[0] = botTargetSpeed;
      wheelTargetSpeed[1] = botTargetSpeed;
      motorDirection[0] = botTargetDirection;
      motorDirection[1] = botTargetDirection;
      double changeDegreesPerSecond = 0;

      double thisMAXSpeed = distancePerDegreeChange * MAX_heading_Change * 0.0036;
      if (thisMAXSpeed < MIN_Speed)thisMAXSpeed = MIN_Speed;
      thisMAXSpeed = MIN_Speed;
      double positiveHeadingToTarget = makePositive(headingToTarget);
      
   if (makePositive(headingToTarget) > anglePerStep * 0.5 && (botTurnDirection != none)){ // change speed to correct heading for bot pivit
      //Serial.println("H" + String(heading) + "TH" + String(targetHeading) + "d" + String(headingToTarget));

      if (botTurnDirection == turnRight){
        if (positiveHeadingToTarget >= MAX_heading_Change){ // greater than can change in 1 second
            wheelTargetSpeed[0] = thisMAXSpeed;
            wheelTargetSpeed[1] = thisMAXSpeed;
            motorDirection[1] = -botTargetDirection;
        }else{// heading is less than max or is 0 change 
          double thisSpeed = thisMAXSpeed * (positiveHeadingToTarget / MAX_heading_Change);
          if (thisSpeed < MIN_Speed)thisSpeed = MIN_Speed;
            wheelTargetSpeed[0] = thisMAXSpeed;
            wheelTargetSpeed[1] = thisMAXSpeed;
            motorDirection[1] = -botTargetDirection;
        }
      }
      
      if (botTurnDirection == turnLeft){
        if (positiveHeadingToTarget >= MAX_heading_Change){ // greater than can change in 1 second
            wheelTargetSpeed[0] = thisMAXSpeed;
            wheelTargetSpeed[1] = thisMAXSpeed;
            motorDirection[0] = -botTargetDirection;
        }else{// heading is less than max or is 0 change 
          double thisSpeed = thisMAXSpeed * (positiveHeadingToTarget / MAX_heading_Change);
          if (thisSpeed < MIN_Speed)thisSpeed = MIN_Speed;
            wheelTargetSpeed[0] = thisMAXSpeed;
            wheelTargetSpeed[1] = thisMAXSpeed;
            motorDirection[0] = -botTargetDirection;
        }
      }
  }else{ // target angle reached
    if (botTurnDirection == turnLeft || botTurnDirection == turnRight){ // done
      //Serial.println("H" + String(heading) + "TH" + String(targetHeading) + "d" + String(headingToTarget));
      //getNextCommand();
    }
  }
   if (makePositive(headingToTarget) > anglePerStep * 2.5 && (botTurnDirection == none)){ // change speed to correct heading for bot drive
    if (botTurnDirection == none && botTargetSpeed){ // only when moving at least min speed
        //thisMAXSpeed *= 0.5;
        Serial.println("H" + String(heading) + "TH" + String(targetHeading) + "d" + String(headingToTarget));
        wheelTargetSpeed[0] = botTargetSpeed;
        wheelTargetSpeed[1] = botTargetSpeed;
        
        double thisSpeed = thisMAXSpeed;
          if (positiveHeadingToTarget < MAX_heading_Change){ // greater than can change in 1 second
              thisSpeed = thisMAXSpeed;//* (positiveHeadingToTarget / MAX_heading_Change);
            }
        if (headingToTarget > 0 ){ // need to turn right
            wheelTargetSpeed[0] += thisSpeed * botTargetDirection;
            wheelTargetSpeed[1] -= thisSpeed * botTargetDirection;
        }else{
            wheelTargetSpeed[0] -= thisSpeed * botTargetDirection;
            wheelTargetSpeed[1] += thisSpeed * botTargetDirection;
        }
        if (wheelTargetSpeed[0] < MIN_Speed){ // correct speeds
          wheelTargetSpeed[1] += MIN_Speed - wheelTargetSpeed[0];
          wheelTargetSpeed[0] = MIN_Speed;
        }
        if (wheelTargetSpeed[1] < MIN_Speed){ // correct speeds
          wheelTargetSpeed[0] += MIN_Speed - wheelTargetSpeed[1];
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
        //Serial.println("A" + String(wheelTargetSpeed[0]) + "B" + String(wheelTargetSpeed[1]));
      }
  }
}

