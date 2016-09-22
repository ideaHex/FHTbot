/*
 * 
 * 
 * 
 */


#ifndef encoderMotorControl_h
#define encoderMotorControl_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class encoderMotorController {

  public:
  encoderMotorController(uint8_t , uint8_t  , uint8_t ,uint8_t  , uint8_t ,uint8_t );       //constructor
  void playNote(int,int);                                                                   // Play notes through motors
  void manualDrive(int, int);                                                               // manual drive mode intput
  void update();                                                                            // use ticker to call this every updateFrequency from loop function
  int updateFrequency = 50;                                                                 // update frequency in milli seconds

  private:

  #define MAX_STEP_TIMING_BUFFER 200
  #define TIME_OUT 100000                         // encoder time out in micro seconds
  int MAX_range = 500;                            // maximum input from controller, higher values will be capped
  float MAX_Speed = 2;                            // in KPH
  float minMotorSpeed = 0.12;                     // as a normal (range from 0.0 to 1.0)
  void motorAStep();                              // process interrupt change
  void motorBStep();
  void reverseMotorA();
  void reverseMotorB();

  uint8_t motorAPin1;
  uint8_t motorAPin2;
  uint8_t motorBPin1;
  uint8_t motorBPin2;
  unsigned long lastMicros;                      // time of last update
  int encoderStepTiming[2][MAX_STEP_TIMING_BUFFER]; // micro seconds of each step
  int encoderStepTimingBufferPosition[2];
  unsigned long steps[2];
  unsigned long totalSteps[2];                       
  unsigned long debounceMinStepTime = 2000;       // minimum step time in micro seconds
  double lastSampleDeltaT[2];

  double encoderWheelSlots = 20;
  float wheelDiameter = 64.6;                     // in mm
  double axleLength = 93.8;                       // distance between wheel centers in mm
  double axleCircumference = (axleLength * 2.0) * PI;
  double distancePerStep = (wheelDiameter * PI) / (encoderWheelSlots * 2.0);
  double anglePerStep = (distancePerStep / axleCircumference) * 360.0;
  double heading = 0.0;
  int PWMFrequency = 40;                        //Theoretical max frequency is 80000000/range, range = 1023 so 78Khz here
  int PWMWriteRange = 1023;                       // 1023 is default for 10 bit,the maximum value can be ~ frequency * 1000 /45. For example, 1KHz PWM, duty range is 0 ~ 22222
  int lastX = 0, lastY = 0;
  #define forward 1
  #define reverse -1
  int motorDirection[2];
  unsigned long boostEndTime;
  int boostDuration = 150;                        // in mS
  double botTargetSpeed = 0.0;                    // in KPH
  double wheelTargetSpeed[2] = {0,0};             // in KPH
  double botCurrentSpeed;                         // in KPH
  double targetHeading = 0.0;                     // in degrees
  long botTargetDistance = 0;                     // in mm
  long wheelTargetDistance[2] = {0,0};            // in mm
  long botTargetSteps = 0;
  long wheelTargetSteps[2] = {0,0};
  double gridX = 0.0;                             // grid coords
  double gridY = 0.0;
  String commandSet;                              // string to hold incomming commands
  boolean commandSetHasCommands = false;
  
  // private functions
  float checkNormal(float);
  int makePositive(int);
  void step(int);
  void updateGrid(int);
  void startCommandSet(String);
  boolean getNextCommand();
  void cancelCommandSet();
  
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
