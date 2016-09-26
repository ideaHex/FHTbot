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

void encoderMotorController::playNote(int note,int duration){
  analogWriteFreq(78000);
  yield();
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
void encoderMotorController::motorAStep(){
  step(0);
}

void encoderMotorController::motorBStep(){
  step(1);
}

void encoderMotorController::step(int encoder){
  // debounce
  if (micros() - lastMicros[encoder] < debounceMinStepTime){
    return;                                         // this is a bounce, ignore it
  }
  lastMicros[encoder] = micros();
  // update grid
  updateGrid(encoder);
  
  // update heading + steps
    heading += (encoder * motorDirection[encoder] * -anglePerStep ) + ( (encoder + 1) * motorDirection[encoder] * anglePerStep );
    if (heading > 360)heading -= 360.0;
    else if (heading < 0)heading += 360.0;
    totalSteps[encoder]++;
    steps[encoder]++;

  // TODO: update speed buffer / last pulse time ? // optional at this point
}

void encoderMotorController::manualDrive(int X, int Y){
  if (commandSetHasCommands){
    cancelCommandSet();                                                        // cancel any outstanding turtle / auto mode commands targets etc...
    targetHeading = heading;
  }

  if (X > MAX_range)X = MAX_range;
  if (X < -MAX_range)X = -MAX_range;
  if (Y > MAX_range)Y = MAX_range;
  if (Y < -MAX_range)Y = -MAX_range;

  if (!lastX && !lastY) targetHeading = heading;                               // start from standstill, based on input not speed
  lastX = X;
  lastY = Y;

  if (Y < 0){motorDirection[0] = forward; motorDirection[1] = forward;}
  else if (Y > 0) {motorDirection[0] = reverse; motorDirection[1] = reverse;} // if Y = 0 coasting in same direction

  botTargetSpeed = (makePositive(Y) > 0) * ( (double(Y) / double(MAX_range) * (MAX_Speed - MIN_Speed) ) + MIN_Speed );    // cannot drop below min speed if Y != 0

  // TODO: calculate heading change in degrees per second from X and update target heading
  double targetDegreesPerSecond = double(X) / double(MAX_range) * MAX_heading_Change;
  
  // TODO: calculate new motor speeds from the above and feed into PID for PWM of each motor
  double distanceInOneSecond = botTargetSpeed / 3600.0; // km / sec, calculate the target distance traveled in a straight line at the targes speed for 1 second
   //       calculate the target distance each wheel needs to travel to get the new heading in 1 second
  double wheelADistance = distanceInOneSecond + ( (targetDegreesPerSecond * motorDirection[0] * 0.5) * distancePerDegreeChange );//in km / sec
  double wheelBDistance = distanceInOneSecond - ( (targetDegreesPerSecond * motorDirection[1] * 0.5) * distancePerDegreeChange );
  wheelTargetSpeed[0] = wheelADistance * 3600.0;// km/hr
  wheelTargetSpeed[1] = wheelBDistance * 3600.0;// km/hr
  // check direction so encoders know which way to count and motors know which way to go
  if (wheelTargetSpeed[0] < 0){
    wheelTargetSpeed[0] = -wheelTargetSpeed[0];
    motorDirection[0] = !motorDirection[0];
  }
    if (wheelTargetSpeed[1] < 0){
    wheelTargetSpeed[1] = -wheelTargetSpeed[1];
    motorDirection[1] = !motorDirection[1];
  }
  // check to see if target is below max
  if (wheelTargetSpeed[0] > MAX_Speed){
    wheelTargetSpeed[1] -= MAX_Speed - wheelTargetSpeed[0];
    wheelTargetSpeed[0] = MAX_Speed;
  }
   if (wheelTargetSpeed[1] > MAX_Speed){
    wheelTargetSpeed[0] -= MAX_Speed - wheelTargetSpeed[1];
    wheelTargetSpeed[1] = MAX_Speed;
  }
  //       correct for either wheel not exceeding its min speed or 0
  
  //       should be able to set timer now so the target heading can be calculated in the next update. Target heading +- targetDegreesPerSecond / 1000ms * actual time in ms
  //       if motor reversed set motor direction
  
  
  // TODO: update PWM of the motors through PID ?? or wait for update ?
}

  void encoderMotorController::reverseMotorA(){
    // swap pins
    uint8_t holder;
    holder = motorAPin1;
    motorAPin1 = motorAPin2;
    motorAPin2 = holder;
  }
  
  void encoderMotorController::reverseMotorB(){
        // swap pins
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
  if (commandSet.indexOf("data") == -1){
    // error, no data found or invalid string
    return;
  }
  commandSet.remove(0,5); // trimm data & comma from front of string
  commandSetHasCommands = true;
}

boolean encoderMotorController::getNextCommand(){
  String Command;
  int value;
  if (commandSet.length()){ // has data left
    Command = commandSet.substring(0,1);
    commandSet.remove(0,2);
    int nextComma =  commandSet.indexOf(",");
    value = commandSet.substring(0,nextComma).toInt();
    commandSet.remove(0,nextComma+1);
  }else{
    return false;
  }
  // TODO: set targets for command here, direction speed distance etc..
  
  return true;
}

void encoderMotorController::cancelCommandSet(){
  commandSet.remove(0);
  commandSetHasCommands = 0;
}

void encoderMotorController::update(){
  lastUpdateMicros = micros();
  unsigned long lastDeltaT = micros() - lastUpdateMicros;
  botCurrentSpeed = ((( (double(steps[0]) + double(steps[1]) ) * distancePerStep) * 0.5) / double(lastDeltaT)) / 3600.0; // km / hr
  wheelSpeed[0] = double(steps[0]) * distancePerStep / double(lastDeltaT) / 3600.0;
  wheelSpeed[1] = double(steps[1]) * distancePerStep / double(lastDeltaT) / 3600.0;
  
  steps[0] = 0;
  steps[1] = 0;
  
  // TODO:  if current command done call commandSetHasCommands = getNextCommand();
  //        assume done if error is less than 1% ?
  
}

void encoderMotorController::allStop(){
  // TODO: set all target flags to false
  //       set target speed to 0 so PID can slow to stop.
}

void encoderMotorController::setMotorSpeed(int PWMA, int PWMB){
  analogWrite(motorAPin1,PWMA * (motorDirection[0] < 0) );
  analogWrite(motorAPin2,PWMA * (motorDirection[0] > 0) );
  analogWrite(motorBPin1,PWMB * (motorDirection[1] > 0) );
  analogWrite(motorBPin2,PWMB * (motorDirection[1] < 0) );
}

void encoderMotorController::PID(){
  int PWMA,PWMB;
  //TODO: calculate PID for motor PWM baced on motor target speed
  // currently being called every 50 mS (20 Hz)
  // wheelSpeed[0];               // is current left motor speed in km / hr
  // wheelSpeed[1];               // is current right motor speed in km / hr
  // wheelTargetSpeed[0];         // is the left motor target speed
  // wheelTargetSpeed[1];         // is the right motor target speed
  // MAX_Speed;                   // is bot maximum speed
  // MIN_Speed;                   // minimum speed to prevent stall, takes about 50% more to overcome starting torque

  setMotorSpeed(PWMA, PWMB);      // output pwm to motors range is 0 - 1023
}

