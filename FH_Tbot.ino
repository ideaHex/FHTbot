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
void sendFile(File);

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
    if (distance < 200){
       // setColor(RgbColor(255,0,0));
       // motors.update(0,50);
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
  }else{
        if (req.indexOf("GET / HTTP/1.1") != -1){         // start page
          int dataLength = strlen_P(startPage);
            yield();
            client.write_P(startPage , dataLength);
            yield();
            dataLength = strlen_P(hackerspaceImage);
            client.write_P(hackerspaceImage , dataLength);
            yield();
            dataLength = strlen_P(startPage1);
            client.write_P(startPage1 , dataLength);
            delay(1);                                     // to improve compatability with some browsers
          }
          String fileString = req.substring(4, (req.length() - 9));
          if (fileString.indexOf("png") != -1){
            File dataFile = SPIFFS.open(fileString, "r");
            sendFile(dataFile);
            dataFile.close();
          }
          if (req.indexOf("Credits") != -1){              // credits page
          int dataLength = strlen_P(creditsPage);
            yield();
            client.write_P(creditsPage , dataLength);
            yield();                   
            dataLength = strlen_P(hackerspaceImage);
            client.write_P(hackerspaceImage , dataLength);
            yield();
            dataLength = strlen_P(creditsPage1);
            client.write_P(creditsPage1 , dataLength);
            delay(1);                                     // to improve compatability with some browsers
          }
          if (req.indexOf("Prog") != -1){ // credits page
          int dataLength = strlen_P(programMode);
            yield();
            client.write_P(programMode , dataLength);
            delay(1);                                     // to improve compatability with some browsers
          }
          if (req.indexOf("About") != -1){ // credits page
          int dataLength = strlen_P(aboutPage);
            yield();
            client.write_P(aboutPage , dataLength);
            yield();                   
            dataLength = strlen_P(hackerspaceImage);
            client.write_P(hackerspaceImage , dataLength);
            yield();
            dataLength = strlen_P(aboutPage1);
            client.write_P(aboutPage1 , dataLength);
            delay(1);                                     // to improve compatability with some browsers
          }
          if (req.indexOf("Start") != -1){                // free drive mode
            boolean Chrome = false;
            if (req.indexOf("Chrome") != -1){
              Chrome = true;
            //client.print("<html><head></head><body>Your browser is not fully supported</body></html>");
            }
            int dataLength = strlen_P(HTML_text);
            yield();
            client.write_P(HTML_text , dataLength);
            yield();
            if (!Chrome){
            dataLength = strlen_P(hackerspaceImage);
            client.write_P(hackerspaceImage , dataLength);
            }else{
              dataLength = strlen_P(hackerspaceImageChrome);
              client.write_P(hackerspaceImageChrome , dataLength);
            }
            yield();
            dataLength = strlen_P(HTML_text1);
            client.write_P(HTML_text1 , dataLength);
            delay(1);                                     // to improve compatability with some browsers
            lastMicrosTime = micros();
            maxLoopTime = 0;
          }
          if (req.indexOf("feedback") != -1){
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
  SPIFFS.begin();
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
void sendFile(File theBuffer){ // breaks string into packets
  int bufferLength = theBuffer.size();
  if (bufferLength < 2920){
    client.write(theBuffer,bufferLength);
    return;
  }
  while (bufferLength > 2920){
    client.write(theBuffer,2920);
    bufferLength -= 2920;
  }
  if (bufferLength > 0){
    client.write(theBuffer,bufferLength);
  }
}

