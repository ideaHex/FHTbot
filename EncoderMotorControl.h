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

#pragma GCC optimize ("-O2")
#ifndef encoderMotorControl_h
#define encoderMotorControl_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#endif

class encoderMotorController {

  public:
  encoderMotorController(uint8_t,  uint8_t,  uint8_t,  uint8_t,  uint8_t,  uint8_t);        //constructor
  void playNote(int, double);                                                               // Play notes through motors
  void manualDrive(int, int);                                                               // manual drive mode input
  void update();                                                                            // use ticker to call this every updateFrequency from loop function
  float updateFrequencyScaler = 0.75;
  int updateFrequency = 30 * updateFrequencyScaler;                                         // update frequency in milli seconds
  void startCommandSet(String);                                                             // get command set from string
  void cancelCommandSet();
  void takeStep(int);
  void hardRightTurn();
  void hardLeftTurn();

  double getheading();
  double getSpeed();
  double getTravel();
  double getAcceleration();
  void reverseMotorA();
  void reverseMotorB();
  void playMarch();
  void playCharge();
  void playMarioMainThem();
  void playMarioUnderworld();
  void playVroom();

  private:

  #define MAX_STEP_TIMING_BUFFER 50
  #define TIME_OUT 100000                              // encoder time out in micro seconds
  int MAX_range = 500;                                 // maximum input from controller, higher values will be capped
  volatile double MAX_Speed = 1.7 ;                    // in KPH
  volatile double MIN_Speed = 0.32; //.37                   // minimum speed in KPH
  volatile double BASE_MIN_Speed = MIN_Speed;
  volatile double BASE_MAX_Speed = MAX_Speed;
  volatile float minMotorSpeed = MIN_Speed / MAX_Speed;// as a normal (range from 0.0 to 1.0)
  volatile double botmodeSpeed = ((MAX_Speed - MIN_Speed) * 0.4) + MIN_Speed;
  volatile unsigned int timeOfFirstStep[2];            // first step in sample
  volatile unsigned int timeOfLastStep[2];             //step before current
  volatile unsigned int timeOfCurrentStep[2];
  volatile double minCalculatedSpeed = MIN_Speed * 0.08;// if speed drops 92% below minimum speed is considered 0 KPH
  uint8_t motorAPin1;
  uint8_t motorAPin2;
  uint8_t motorBPin1;
  uint8_t motorBPin2;
  volatile unsigned long lastMicros[2];                // time of last step
  unsigned long lastUpdateMicros;                      // time of last update
  volatile long steps[2];
  volatile long totalSteps[2];
  double lastSampleDeltaT[2];
  int PWMFrequency = 40;                               // Theoretical max frequency is 80000000/range, range = 1023 so 78Khz here
  int PWMWriteRange = 1023;                            // 1023 is default for 10 bit,the maximum value can be ~ frequency * 1000 /45. For example, 1KHz PWM, duty range is 0 ~ 22222
  double encoderWheelSlots = 20;                       //THIS CONTROLS THE NUMBER OF WINDOWS ON THE ENCODER WHEELS
  float wheelDiameter = 64.93592;                      // in mm
  double axleLength = 90.55;                           // distance between wheel centres in mm, last version was 93.8
  double axleCircumference = (axleLength * 2.0) * PI;
  volatile double distancePerStep = (wheelDiameter * PI) / (encoderWheelSlots * 2.0);
  volatile double anglePerStep = (distancePerStep / axleCircumference) * 360.0; // heading change angle per step
  double distancePerDegreeChange = axleCircumference / 360.0;   // distance a wheel traveled to alter heading 1 degree
  volatile long minCalculatedSpeedTimePerStep = long(distancePerStep / (minCalculatedSpeed/3600.0));
  volatile double startPWMBoost = minMotorSpeed * PWMWriteRange * 1.5; // add default:35% above minimum speed by default
  volatile unsigned long debounceMinStepTime[2];       // minimum step time in micro seconds
  volatile boolean boostOn[2];
  volatile double heading = 0.0;
  volatile double lastError[2];
  volatile double MAX_heading_Change = 110.0;          // in degrees per second
  volatile double minPWM = minMotorSpeed * PWMWriteRange * 0.85;
  int lastX = 0;
  int lastY = 0;
  const int forward = 1;
  const int reverse = -1;
  const int turnLeft = 1;
  const int turnRight = -1;
  const int none = 0;
  volatile int motorDirection[2];
  volatile int botTargetDirection = forward;
  volatile int botTurnDirection = none;
  volatile double botTargetSpeed = 0.0;                // in KPH
  volatile double wheelTargetSpeed[2];                 // in KPH
  volatile double botCurrentSpeed;                     // in KPH
  volatile double wheelSpeed[2];
  volatile double targetHeading = 0.0;                 // in degrees
  volatile long botTargetDistance = 0;                 // in mm
  long wheelTargetDistance[2];                         // in mm
  long botTargetSteps = 0;
  long wheelTargetSteps[2];
  double gridX = 0.0;                                  // grid coords
  double gridY = 0.0;
  String commandSet;                                   // string to hold incoming commands
  boolean commandSetHasCommands = false;
  volatile boolean commandCompleted = false;
  volatile boolean slow = false;
  volatile int PWMA = 0;
  volatile int PWMB = 0;
  double targetDegreesPerSecond = 0;
  volatile long nextCommandMillis = 0;
  volatile long delaybetweenCommands = 300;             // default 350

  // private functions
  float checkNormal(float);
  int makePositive(int);
  double makePositive(double);
  void updateGrid(int);
  boolean getNextCommand();
  void processCommand(String , double);
  void allStop();
  void setMotorSpeed();
  void setMotorSpeed(int,int);
  void PID();
  void updateSteering(long);
  void updateBPM(double);
  void increaseMinSpeed(int);
  void motorBreak();

  // DURATION OF THE NOTES
  double BPM = 104;
  double Qnote = 60000/BPM;                                // quarter 1/4
  double Hnote = 2*Qnote;                                  // half 2/4
  double Enote = Qnote/2;                                  // eighth 1/8
  double Snote = Qnote/4;                                  // sixteenth 1/16
  double Wnote = 4*Qnote;                                  // whole 4/4

};

/*************************************************
  * Public Constant Notes.
  *************************************************/

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978


#endif


