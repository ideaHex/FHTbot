/*
 * 
 * 
 */

 
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <Ticker.h>
#include "MotorController.h"
#include "US100Ping.h"
#include <NeoPixelBus.h>
#include "Encoder.h"
#include "NeoPixelAnimations.h"

// WebPage
#include "WebPage.h"
#include "Images.h"
#include "WebPage_StartPage.h"
#include "WebPage_CreditsPage.h"
#include "WebPage_AboutPage.h"
#include "WebPage_ProgramMode.h"

extern "C" { 
   #include "user_interface.h" 
 } 


//////////////////////
// WiFi Definitions //
//////////////////////
const char *password = "12345678";      // This is the Wifi Password (only numbers and letters,  not . , |)
String AP_Name = "FH@Tbot";             // This is the Wifi Name(SSID), some numbers will be added for clarity (mac address)
bool enableCompatibilityMode = false;   // turn on compatibility mode for older devices, spacifically sets no encryption and 11B wifi standard

void setupWiFi(void);
void initHardware(void);

/////////////////////
// Pin Definitions //
/////////////////////

// D4 is used for neoPixelBus (TXD1)

// stepper without PWM/speed input pins, don't use D0
const int motorLeftA  = D5;
const int motorLeftB  = D6;
const int motorRightA = D3;
const int motorRightB = D2;
const int motorLeftEncoder = D1;
const int motorRightEncoder = D0;

motorController motors(motorLeftA,motorLeftB,motorRightA,motorRightB);

/*
// stepper with direction and speed pins, don't use D0 for speed
const int motorLeftDir  = D3;
const int motorLeftSpd  = D2;
const int motorRightDir = D7;
const int motorRightSpd = D8;

motorController motors(motorLeftDir,D5,motorLeftSpd,motorRightDir,D4,motorRightSpd); 
*/
/*
// stepper with 2 motor pins and a enable pin per motor, don't use D0 for speed
const int motorLeftA    =  D1;
const int motorLeftB    =  D0;
const int motorLeftSpd  =  D2;
const int motorRightA   =  D3;
const int motorRightB   =  D4;
const int motorRightSpd =  D5;

motorController motors(motorLeftA,motorLeftB,motorLeftSpd,motorRightA,motorRightB,motorRightSpd); 
*/
WiFiServer server(80);
WiFiClient client;
DNSServer dnsServer;
Ticker HeartBeatTicker;
US100Ping ping;
int clientTimeout = 150;
bool clientStopped = true;
unsigned long nextClientTimeout = 0;
//NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(6, D4); // 6 pixels, output pin D4, function ignores pin
int temperature = 0;
int distance = 0;

bool HeartBeatRcvd = false;

void Stop(void)
{
  motors.update(0,0);
}

void CheckHeartBeat(void)
{
  if (HeartBeatRcvd == true)
  {
    HeartBeatRcvd = false;
  }
  else
  {
    Stop();                          // Serial.println("Connection lost STOP!!!!!!");
  }
}

/**
 * Flashes forward LEDs to indicate direction of travel.
 * Should scale to larger LED counts (assuming two rows).
 * Stupid LED Trick mostly.
 * param int indexOfX: x axis on Joystick, left or right.
 * param int indexOfY: y axis on Joystick, forward or back.
 */
void updateBlinkers(int indexOfX, int indexOfY){
  //Creates array for pixels
  RgbColor px[NUMPIXELS];
  for(int i = 0; i < NUMPIXELS; i++){
    px[i] = RgbColor(0,0,0);
  }
  //Pick which variable is greater in magnitude.
    //If X>Y the result should be 0
    if((indexOfX % indexOfY) == 0){
      if(indexOfX < 0 ){
        //Update Left
        //px[(NUMPIXELS/2 - 1)] = RgbColor(255,127,0);
        //px[NUMPIXELS - 1] =  RgbColor(255,127,0);
        px[2] = RgbColor(80,40,0);
        px[5] =  RgbColor(80,40,0);
      }else if (indexOfX > 0){
        //update Right
        //px[0] =  RgbColor(255,127,0);
        //px[(NUMPIXELS/2)] = RgbColor(255,127,0);
        px[0] =  RgbColor(80,40,0);
        px[3] = RgbColor(80,40,0);
      }else{
        //Equals 0 no indication        
      }      
    }else{
      if(indexOfY < 0 ){
        //px[(NUMPIXELS/2) - 2] = RgbColor(255,127,0);
        //px[(NUMPIXELS/2) - 1] = RgbColor(255,127,0);
        //px[(NUMPIXELS/2)] = RgbColor(255,127,0);
        px[0] = RgbColor(80,40,0);
        px[1] = RgbColor(80,40,0);
        px[2] = RgbColor(80,40,0);
      }else if (indexOfY > 0){
        //px[(NUMPIXELS) - 2] = RgbColor(255,127,0);
        //px[(NUMPIXELS) - 1] = RgbColor(255,127,0);
        //px[(NUMPIXELS)] = RgbColor(255,127,0);        
        px[3] = RgbColor(80,40,0);
        px[4] = RgbColor(80,40,0);
        px[5] = RgbColor(80,40,0);
      }else{
        //Equals 0 no indication
      }  
    }
    
    for(int i = 0; i< NUMPIXELS; i++){
      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      strip.SetPixelColor(i, px[i]); // Orange colour.
      strip.Show(); // This sends the updated pixel color to the hardware.
      //delay(delayval); // Delay for a period of time (in milliseconds).
    }
}

void setup()
{
  system_update_cpu_freq(160);        // set cpu to 80MHZ or 160MHZ !
  initHardware();
  setupWiFi();
  HeartBeatTicker.attach_ms(500, CheckHeartBeat);
  //motors.setTrim(1.0,1.0);            // this setting is optional, it compensates for speed difference of motors eg (0.95,1.0), and it can reduce maximum speed of both eg (0.75,0.75);
  //motors.setSteeringSensitivity(0.9);  // this setting is optional
  motors.setPWMFrequency(40);           // this setting is optional, depending on power supply and H-Bridge this option will alter motor noise and torque.
  motors.setMinimumSpeed(0.08);         // this setting is optional, default is 0.1(10%) to prevent motor from stalling at low speed
}
unsigned long maxLoopTime = 0;
unsigned long lastMicrosTime;
void loop()
{
  if (micros() - lastMicrosTime > maxLoopTime)maxLoopTime = micros() - lastMicrosTime;
  lastMicrosTime = micros();
  // time dependant functions here
   motors.run();
   ping.run();
   if (ping.gotTemperature()){
    temperature = ping.getTemperature();
    //Serial.printf("Temperature: %d C \r\n", temperature);
   }
   if (ping.gotDistance()){
    distance = ping.getDistance();
    if (distance < 200){
       // setColor(RgbColor(255,0,0));
       // motors.update(0,50);
      }
    //Serial.printf("distance: %d mm \r\n", distance);
   }
   dnsServer.processNextRequest();
   // client functions here
   if (clientStopped){
    client = server.available();
    clientStopped = false;
    nextClientTimeout = millis() + clientTimeout;
   }else{
    if (!client.connected() || millis() > nextClientTimeout){
      client.stop();
      clientStopped = true;
    }
   }
   
  if (!client.available())
  {
    return;
  }
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  //Serial.println(req);
  //Serial.println(client.readString());
  int indexOfX = req.indexOf("/X");
  int indexOfY = req.indexOf("/Y");
  if (indexOfX != -1 && indexOfY != -1){
    pixelTest();
    String xOffset = req.substring(indexOfX + 2, indexOfX + 8);
    int dX = xOffset.toInt();
    String yOffset = req.substring(indexOfY + 2, indexOfY + 8);
    int dY = yOffset.toInt();
    
    HeartBeatRcvd = true;               // recieved data, must be connected
    // driver assist
    /*
    if (distance < 450 && dY < 0){
    setColor(RgbColor(70,85,75));
      dX = 500 - distance;
      if (dY < -40 ){
        //dX = 0;
        dY = -40;
      }
    }*/
    motors.update(dX,dY);

        
    //Serial.print(F("DX: "));
    //Serial.print(dX);
    //Serial.print(F(" DY: "));
    //Serial.println(dY);
    //Serial.print(F("Free Ram: "));
    //Serial.println(system_get_free_heap_size());

    //Blinker Addon
    //Only operates if enabled in RGB options above
    if(useBlinkers == true){
      updateBlinkers(dX,dY);
    }
  }else{
        if (req.indexOf("GET / HTTP/1.1") != -1){         // start page
          int dataLength = strlen_P(startPage);
            delay(1);
            client.write_P(startPage , dataLength);
            delay(1);                   
            dataLength = strlen_P(hackerspaceImage);
            client.write_P(hackerspaceImage , dataLength);
            delay(1);
            dataLength = strlen_P(startPage1);
            client.write_P(startPage1 , dataLength);
            delay(1);                                     // to improve compatability with some browsers
          }
          if (req.indexOf("Credits") != -1){ // credits page
          int dataLength = strlen_P(creditsPage);
            delay(1);
            client.write_P(creditsPage , dataLength);
            delay(1);                   
            dataLength = strlen_P(hackerspaceImage);
            client.write_P(hackerspaceImage , dataLength);
            delay(1);
            dataLength = strlen_P(creditsPage1);
            client.write_P(creditsPage1 , dataLength);
            delay(1);                                     // to improve compatability with some browsers
          }
          if (req.indexOf("Prog") != -1){ // credits page
          int dataLength = strlen_P(programMode);
            delay(1);
            client.write_P(programMode , dataLength);
            delay(1);                                     // to improve compatability with some browsers
          }
          if (req.indexOf("About") != -1){ // credits page
          int dataLength = strlen_P(aboutPage);
            delay(1);
            client.write_P(aboutPage , dataLength);
            delay(1);                   
            dataLength = strlen_P(hackerspaceImage);
            client.write_P(hackerspaceImage , dataLength);
            delay(1);
            dataLength = strlen_P(aboutPage1);
            client.write_P(aboutPage1 , dataLength);
            delay(1);                                     // to improve compatability with some browsers
          }
          if (req.indexOf("Start") != -1){                // free drive mode
          //req = client.readString();
          //Serial.println(req);
          //Serial.println(F("Sending Page"));
          if (req.indexOf("Chrome") != -1){
            //client.print("<html><head></head><body>Your browser is not fully supported</body></html>");
          }
            int dataLength = strlen_P(HTML_text);
            delay(1);
            client.write_P(HTML_text , dataLength);
            delay(1);                   
            dataLength = strlen_P(hackerspaceImage);
            client.write_P(hackerspaceImage , dataLength);
            delay(1);
            dataLength = strlen_P(HTML_text1);
            client.write_P(HTML_text1 , dataLength);
            delay(1);                                     // to improve compatability with some browsers
            lastMicrosTime = micros();
            maxLoopTime = 0;
          }
          if (req.indexOf("feedback") != -1){
                String s;
                s = ("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<meta http-equiv='refresh' content='3'><!DOCTYPE HTML>\r\n<html>\r\n<body><script>");
                s += ("var tmp=");
                s += temperature;
                s += (";var dis=");
                s += distance;
                s += (";var kph=");
                s += motors.getSpeed();
                s += (";var movd=");
                s += motors.getTravel();
                s += (";var acl=");
                s += motors.getAcceleration();
                s += (";var hed=");
                s += motors.getheading();
                s += (";var mlt=");
                s += maxLoopTime;
                s += (";</script></body></html>\n");
                client.print(s);
          }
        }
         client.flush();
}

void setupWiFi()
{
  WiFi.mode(WIFI_AP);
 
  // Create a unique name by appending the MAC address to the AP Name

  AP_Name = AP_Name + " " + WiFi.softAPmacAddress();
  char AP_NameChar[AP_Name.length() + 1];
  AP_Name.toCharArray(AP_NameChar,AP_Name.length() + 1);

  // setup AP, start DNS server, start Web server

  int channel = random(1,13);
  const byte DNS_PORT = 53;
  IPAddress subnet(255, 255, 255, 0);
  IPAddress apIP(192, 168, 1, 1);
  WiFi.softAPConfig(apIP, apIP, subnet);
  if (enableCompatibilityMode){
    wifi_set_phy_mode(PHY_MODE_11B);    // Note: ESP8266 soft-AP only support bg.
    const char *pw = "";
    WiFi.softAP(AP_NameChar, pw , channel , 0 );
  }else{
    wifi_set_phy_mode(PHY_MODE_11N);
    WiFi.softAP(AP_NameChar, password , channel , 0 );
  }
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "FHTbot.com", apIP);//must use '.com, .org etc..' and cant use '@ or _ etc...' ! . Use "*" to divert all **VALID** names
  server.begin();
  
  //WiFi.printDiag(Serial);
}

void initHardware()
{
  Serial.begin(9600);
  Serial.println(F("\r\n"));
  Serial.println(F("            FH@Tbot Serial Connected\r\n"));
  Serial.println(F("  Type \"FHTbot.com\" into your browser to connect. \r\n"));
  delay(200);
  Serial.swap();
  ping.begin(Serial);
  strip.Begin();
  strip.Show();
  smile();
  motors.addEncoders(motorLeftEncoder,motorRightEncoder);
}


void sound(){
  motors.update(0,-1);
  delay(1);
  motors.setPWMFrequency(78000);
  double a=0;
  while (a<1000){
    a++;
  motors.update(0,1000);
  delayMicroseconds(250);
  motors.update(0,-1000);
  delayMicroseconds(250);
  }
  motors.update(0,0);
  delay(10);
  //motors.setPWMFrequency(78000);
  a=0;
  while (a<500){
    a++;
  motors.update(0,1000);
  delayMicroseconds(608);
  motors.update(0,-1000);
  delayMicroseconds(608);
  }
  motors.update(0,0);
  delay(10);
 // motors.setPWMFrequency(78000);
  a=0;
  while (a<1000){
    a++;
  motors.update(0,1000);
  delayMicroseconds(357);//714
  motors.update(0,-1000);
  delayMicroseconds(357);
  }
  motors.update(0,0);
  delay(10);
  for (a = 100; a < 3000; a+=1){
  motors.setPWMFrequency(a);
  motors.update(0,1000);
  delayMicroseconds(100);
  motors.update(0,-1000);
  delayMicroseconds(100);
  }
  delay(0);
  for ( a= 3000; a > 100; a-=0.50){
  motors.setPWMFrequency(a);
  motors.update(0,1000);
  delayMicroseconds(50);
  motors.update(0,-1000);
  delayMicroseconds(50);
  }
  motors.update(0,0);
}

