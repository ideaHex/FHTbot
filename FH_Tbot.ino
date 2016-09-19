/*
 * 
 * 
 */

#include <ESP8266WiFi.h>
#include "FS.h"
#include <DNSServer.h>
#include <Ticker.h>
#include "MotorController.h"
#include "US100Ping.h"
#include <NeoPixelBus.h>
#include "Encoder.h"
#include "NeoPixelAnimations.h"

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
void sendFile(File);
void loadIndexPage(void);

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
const int motorRightEncoder = D7;

motorController motors(motorLeftA,motorLeftB,motorRightA,motorRightB);

WiFiServer server(80);
WiFiClient client;
DNSServer dnsServer;
Ticker HeartBeatTicker;
US100Ping ping;
int clientTimeout = 150;
bool clientStopped = true;
unsigned long nextClientTimeout = 0;
int temperature = 0;
int distance = 0;
boolean driverAssist = false;
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
    Stop();                             // Serial.println("Connection lost STOP!!!!!!");
  }
}

void setup()
{
  system_update_cpu_freq(160);          // set cpu to 80MHZ or 160MHZ !
  initHardware();
  setupWiFi();
  HeartBeatTicker.attach_ms(500, CheckHeartBeat);
  //motors.setTrim(1.0,1.0);            // this setting is optional, it compensates for speed difference of motors eg (0.95,1.0), and it can reduce maximum speed of both eg (0.75,0.75);
  //motors.setSteeringSensitivity(0.9);  // this setting is optional
  motors.setPWMFrequency(40);           // this setting is optional, depending on power supply and H-Bridge this option will alter motor noise and torque.
  motors.setMinimumSpeed(0.08);         // this setting is optional, default is 0.1(10%) to prevent motor from stalling at low speed

  motors.playNote(NOTE_C5,200);
  motors.playNote(NOTE_E5,200);
  motors.playNote(NOTE_G5,200);
  motors.playNote(NOTE_A5,400);
  motors.playNote(NOTE_G5,200);
  motors.playNote(NOTE_A5,800);
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
   }
   if (ping.gotDistance()){
    distance = ping.getDistance();
    if (driverAssist){
      if (distance < 200){
         setColor(RgbColor(255,0,0));
         motors.update(0,50);
        }
    }
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
    String xOffset = req.substring(indexOfX + 2, indexOfX + 8);
    int dX = xOffset.toInt();
    String yOffset = req.substring(indexOfY + 2, indexOfY + 8);
    int dY = yOffset.toInt();
    
    HeartBeatRcvd = true;               // recieved data, must be connected
    // driver assist
    if (driverAssist){
      updateBlinkers(dX,dY);
      if (distance < 450 && dY < 0){
      setColor(RgbColor(70,85,75));
        dX = 500 - distance;
        if (dY < -40 ){
          //dX = 0;
          dY = -40;
        }
      }
    }else{
      pixelTest();
    }
    motors.update(dX,dY);
  }else{
        String fileString = req.substring(4, (req.length() - 9));
        
        if (req.indexOf("GET / HTTP/1.1") != -1){         // start page
            loadIndexPage();
            return;
          }
         if (req.indexOf("DriverAssist") != -1){
              File dataFile = SPIFFS.open("/Start.html", "r");
              sendFile(dataFile);
              dataFile.close();
              driverAssist = true;
              return;
          }
          if (req.indexOf("feedback") != -1){             // send feedback to drive webpage
                String s;
                s = ("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<meta http-equiv='refresh' content='1'><!DOCTYPE HTML>\r\n<html>\r\n<body><script>");
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
                return;
          }
          File dataFile = SPIFFS.open(fileString, "r");
            if(dataFile){                                 // if the file exists
              sendFile(dataFile);
              dataFile.close();
              lastMicrosTime = micros();
              maxLoopTime = 0;
              return;
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
  Serial.begin(9600);               // 9600 to work with US-100
  Serial.println(F("\r\n"));
  Serial.println(F("            FH@Tbot Serial Connected\r\n"));
  Serial.println(F("  Type \"FHTbot.com\" into your browser to connect. \r\n"));
  SPIFFS.begin();
  delay(200);
  //Serial.swap();
  ping.begin(Serial);
  strip.Begin();
  strip.Show();
  smile();
  motors.addEncoders(motorLeftEncoder,motorRightEncoder);
  attachInterrupt(motorLeftEncoder, motorLeftEncoderInterruptHandler , CHANGE);
  attachInterrupt(motorRightEncoder, motorRightEncoderInterruptHandler , CHANGE);
}

void sendFile(File theBuffer){ // breaks string into packets
  int bufferLength = theBuffer.size();
  if (bufferLength < 2920){
    client.write(theBuffer,bufferLength);
    //yield();
    return;
  }
  while (bufferLength > 2920){
    client.write(theBuffer,2920);
    bufferLength -= 2920;
    //yield();
  }
  if (bufferLength > 0){
    client.write(theBuffer,bufferLength);
    //yield();
  }
}
void loadIndexPage(){
     driverAssist = false;
     File dataFile = SPIFFS.open("/index.html", "r");
     sendFile(dataFile);
     dataFile.close();
}
void motorLeftEncoderInterruptHandler(){
  motors.encoderA_Step();
  motors.run();
}
void motorRightEncoderInterruptHandler(){
  motors.encoderB_Step();
  motors.run();
}

