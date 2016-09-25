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
  }
  lastMicros = micros();
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
  // TODO: debounce

  // TODO: update grid
  
  // TODO: update headding + steps
    totalSteps[encoder]++;
    steps[encoder]++;

  // TODO: update speed buffer / last pulse time ?
}

void encoderMotorController::manualDrive(int X, int Y){
  cancelCommandSet();
  // TODO: cancel any outstanding turtle / auto mode commands targets etc...
  
  if (X > MAX_range)X = MAX_range;
  if (X < -MAX_range)X = -MAX_range;
  if (Y > MAX_range)Y = MAX_range;
  if (Y < -MAX_range)Y = -MAX_range;

  if (Y < 0){motorDirection[0] = forward; motorDirection[1] = forward;}
  else if (Y > 0) {motorDirection[0] = reverse; motorDirection[1] = reverse;} // if Y = 0 coasting in same direction

  botTargetSpeed = double(Y) / double(MAX_range);

  // TODO: calculate headding change in degrees per second from X and update target heading

  // TODO: calculate new motor speeds from the above and feed into PID for PWM of each motor

  // TODO: update PWM of the motors
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
    // error, no data found
    return;
  }
  commandSet.remove(0,5); // trimm data & comma from front of string
  commandSetHasCommands = true;
}

boolean encoderMotorController::getNextCommand(){
  // TODO: call commandSetHasCommands = getNextCommand(); update function
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
  unsigned long lastDeltaT = micros() - lastMicros;
  botCurrentSpeed = ((( (double(steps[0]) + double(steps[1]) ) * distancePerStep) * 0.5) / double(lastDeltaT)) / 3600.0; // km / hr

  steps[0] = 0;
  steps[1] = 0;
  lastMicros = micros();
}

