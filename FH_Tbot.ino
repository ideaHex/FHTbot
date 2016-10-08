/*
 * 
 * 
 */
//#pragma GCC optimize ("-O2") // O0 none, O1 Moderate optimization, 02, Full optimization, O3, as O2 plus attempts to vectorize loops, Os Optimize space
#include <ESP8266WiFi.h>
#include <FS.h>
#include <DNSServer.h>
#include <Ticker.h>
#include "EncoderMotorControl.h"
#include "RCW0006Ping.h"
#include <NeoPixelBus.h>
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
String getContentType(String);
void updateMotors();

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

encoderMotorController motors(motorLeftA,motorLeftB,motorRightA,motorRightB,motorLeftEncoder,motorRightEncoder);
Ticker motorControllerTicker;
WiFiServer server(80);
WiFiClient client;
DNSServer dnsServer;
Ticker HeartBeatTicker;
int distance = 300;
boolean driverAssist = false;
bool HeartBeatRcvd = false;
String closeConnectionHeader = "";
#define MAX_SRV_CLIENTS 10              // maximum client connections
WiFiClient serverClients[MAX_SRV_CLIENTS];
int currentClient = 0;
boolean pingOn = false;
int lastDistance = 300;

void Stop(void)
{
  motors.manualDrive(0,0);
  setColor(RgbColor(0,0,0));             // turn off led's to save power
  pingOn = false;                        // turn off ping to save power
}

void CheckHeartBeat(void)
{
  if (HeartBeatRcvd == true)
  {
    HeartBeatRcvd = false;
  }
  else
  {
    Stop();                             
  }
}

void setup()
{
  system_update_cpu_freq(160);          // set cpu to 80MHZ or 160MHZ !
  initHardware();
  setupWiFi();
  HeartBeatTicker.attach_ms(1000, CheckHeartBeat);
  closeConnectionHeader += F("HTTP/1.1 204 No Content\r\nConnection: Close\r\n\r\n");
}

void loop()
{
  // time dependant functions here
  if (pingOn){
   distance = getDistance();
   if (driverAssist){
      int testPing = makePositive(lastDistance - distance);
      if ( testPing < (0.15 * distance) && !testPing){ // less than 15% change to avoid spikes
        if (distance < 200){
          setColor(RgbColor(255,0,0));
          motors.manualDrive(0,500);
          }
      }
    }
   lastDistance = distance;
  }
   dnsServer.processNextRequest();
   // client functions here
  while (server.hasClient()){
    for(uint8_t i = 0; i < MAX_SRV_CLIENTS; i++){
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected()){
        if(serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        return;
      }
    }
    //no free/disconnected spot so reject
   // WiFiClient serverClient = server.available();
   // serverClient.stop();
  }
  //check clients for data
  String req = "";
  for(uint8_t i = currentClient; i < MAX_SRV_CLIENTS; i++){// start at current client to keep in order
    if (serverClients[i] && serverClients[i].connected()){
        if (serverClients[i].available()){
          req = serverClients[i].readStringUntil('\r');   // Read the first line of the request
          serverClients[i].flush();
          currentClient = i;
          break;
        }
    }
    currentClient = 0;
  }
 
  if (!req.length()){// empty request
      return;
      }
  HeartBeatRcvd = true;                                           // recieved data, must be connected
  //Serial.println("\r\n" + req);
  //Serial.println(client.readString());
  int indexOfX = req.indexOf("/X");
  int indexOfY = req.indexOf("/Y");
  if (indexOfX != -1 && indexOfY != -1){
    pingOn = true;
    if (req.indexOf("/HBDA") != -1)driverAssist = true;
    if (req.indexOf("/HBDM") != -1)driverAssist = false;
    serverClients[currentClient].write( closeConnectionHeader.c_str(),closeConnectionHeader.length() );
    yield();
    //Serial.print(F("Ram:"));
    //Serial.println(system_get_free_heap_size());
    String xOffset = req.substring(indexOfX + 2, indexOfX + 8);
    int dX = xOffset.toInt();
    String yOffset = req.substring(indexOfY + 2, indexOfY + 8);
    int dY = yOffset.toInt();
    // driver assist
    if (driverAssist){
      updateBlinkers(dX,dY);
      int testPing = makePositive(lastDistance - distance);
   //   if ( testPing < (0.15 * double(distance))){ // less than 15% change to avoid spikes . was  && !testPing
        if (distance < 500 && distance > 199 && dY < 0){
        setColor(RgbColor(90,105,95));
          dX = 700 ;//- distance;
          if (dY != -100 ){
            dY = -100;
          }
        }
        if (distance < 200){
         setColor(RgbColor(255,0,0));
          dY = 500;
        }
   //   }
    }else{
      pixelTest();
    }
    motors.manualDrive(dX,dY);
  }else{
        String fileString = req.substring(4, (req.length() - 9));
        //Serial.println("\r\n" + fileString);
          if (fileString.indexOf("/PlayCharge") != -1){
            serverClients[currentClient].write( closeConnectionHeader.c_str(),closeConnectionHeader.length() );
            yield();
            motors.playCharge();
            return;
          }
          if (fileString.indexOf("/PlayMarch") != -1){
            serverClients[currentClient].write( closeConnectionHeader.c_str(),closeConnectionHeader.length() );
            yield();
            motors.playMarch();
            return;
          }
          if (fileString.indexOf("/PlayMarioTheme") != -1){
            serverClients[currentClient].write( closeConnectionHeader.c_str(),closeConnectionHeader.length() );
            yield();
            motors.playMarioMainThem();
            return;
          }
          if (fileString.indexOf("/PlayMarioUnderworld") != -1){
            serverClients[currentClient].write( closeConnectionHeader.c_str(),closeConnectionHeader.length() );
            yield();
            motors.playMarioUnderworld();
            return;
          }
          if (fileString.indexOf("/HB") != -1){
            pingOn = false;
            driverAssist = false;
            HeartBeatRcvd = true;
            serverClients[currentClient].write( closeConnectionHeader.c_str(),closeConnectionHeader.length() );
            yield();
            return;
          }
          if (fileString.indexOf("data,") != -1){
              serverClients[currentClient].write( closeConnectionHeader.c_str(),closeConnectionHeader.length() );
              yield();
              fileString.remove(0,fileString.indexOf("data,"));
              fileString.trim();
              motors.startCommandSet(fileString);
              return;
          }
          if (fileString.indexOf("feedback") != -1){             // send feedback to drive webpage
                String s,h;
                s = F("<!DOCTYPE HTML><html><head><meta http-equiv='refresh' content='1'></head><body><script>");
                s += (";var dis=");
                s += distance;
                s += (";var kph=");
                s += motors.getSpeed();
                s += (";var movd=");
                s += motors.getTravel();
                s += (";var hed=");
                s += motors.getheading();
                s += (";</script></body></html>");
                h = F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: Close\r\ncontent-length: ");
                h += s.length();
                h += F("\r\n\r\n");
                String ss = h + s;
                serverClients[currentClient].write(ss.c_str(),ss.length());
                yield();
                return;
          }
          sendFile(fileString);
        }
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
    wifi_set_phy_mode(PHY_MODE_11B);            // Note: ESP8266 soft-AP only support bg.
    const char *pw = "";
    WiFi.softAP(AP_NameChar, pw , channel , 0 );
  }else{
    wifi_set_phy_mode(PHY_MODE_11N);
    WiFi.softAP(AP_NameChar, password , channel , 0 );
  }
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "FHTbot.com", apIP);//must use '.com, .org etc..' and cant use '@ or _ etc...' ! . Use "*" to divert all **VALID** names
  server.begin();
  server.setNoDelay(true);
}

void initHardware()
{
  Serial.begin(250000);
  Serial.println(F("\r\n"));
  Serial.println(F("            FH@Tbot Serial Connected\r\n"));
  Serial.print(F("\r\n  Your FHTbot Wifi connection is called "));
  Serial.println(AP_Name + " " + WiFi.softAPmacAddress());
  Serial.print(F("\r\n  Your password is "));
  Serial.println(password);
  Serial.println(F("\r\n  Type FHTbot.com into your browser to connect. \r\n"));
  SPIFFS.begin();
  delay(200);
  pingSetup();
  strip.Begin();
  strip.Show();
  smile();
  // setup motors and encoders
  attachInterrupt(motorLeftEncoder, motorLeftEncoderInterruptHandler , CHANGE);
  attachInterrupt(motorRightEncoder, motorRightEncoderInterruptHandler , CHANGE);
  motorControllerTicker.attach_ms(motors.updateFrequency, updateMotors);
}

void motorLeftEncoderInterruptHandler(){
  motors.takeStep(0);
}
void motorRightEncoderInterruptHandler(){
 motors.takeStep(1);
}

void updateMotors(){
  motors.update();
}

void sendFile(String path){
// get content type
if(path.endsWith("/")){ path += "index.html";}
String dataType = getContentType(path);
  
// check if theres a .gz'd version and send that instead
String gzPath = path + ".gz";
File theBuffer;
if (SPIFFS.exists(gzPath)){             // test to see if there is a .gz version of the file
  theBuffer = SPIFFS.open(gzPath, "r");
  path = gzPath;                        // got it, use this path
}else{                                  // not here so load the standard file
  theBuffer = SPIFFS.open(path, "r");
  if (!theBuffer){                      // this one dosn't exist either, abort.
    theBuffer.close();
    String notFound = F("HTTP/1.1 404 Not Found\r\nConnection: Close\r\n\r\n");
    serverClients[currentClient].write( notFound.c_str(),notFound.length() );
    yield();
    return; // failed to read file
  }
}

// make header
String s = F("HTTP/1.1 200 OK\r\ncache-control: max-age = 3600\r\ncontent-length: ");
    s += theBuffer.size();
    s += F("\r\ncontent-type: ");
    s += dataType;
    s += F("\r\nconnection: close"); // last one added X-Content-Type-Options: nosniff\r\n
  if (path.endsWith(".gz")){
    s += F("\r\nContent-Encoding: gzip\r\n\r\n");
  }else{
    s += F("\r\n\r\n");
  }
     // send the file
  if( !serverClients[currentClient].write(s.c_str(),s.length()) ){
    // failed to send
    theBuffer.close();
    return;
  }
  int bufferLength = theBuffer.size();
  if ( serverClients[currentClient].write(theBuffer,2920) <  bufferLength){
    // failed to send all file
  }
  yield();
  theBuffer.close();
}

String getContentType(String path){ // get content type
String dataType = F("text/html");
String lowerPath = path.substring(path.length()-4,path.length());
lowerPath.toLowerCase();

if(lowerPath.endsWith(".src")) lowerPath = lowerPath.substring(0, path.lastIndexOf("."));
else if(lowerPath.endsWith(".html")) dataType = F("text/html");
else if(lowerPath.endsWith(".htm")) dataType = F("text/html");
else if(lowerPath.endsWith(".png")) dataType = F("image/png");
else if(lowerPath.endsWith(".js")) dataType = F("application/javascript");
else if(lowerPath.endsWith(".css")) dataType = F("text/css");
else if(lowerPath.endsWith(".gif")) dataType = F("image/gif");
else if(lowerPath.endsWith(".jpg")) dataType = F("image/jpeg");
else if(lowerPath.endsWith(".ico")) dataType = F("image/x-icon");
else if(lowerPath.endsWith(".xml")) dataType = F("text/xml");
else if(lowerPath.endsWith(".pdf")) dataType = F("application/x-pdf");
else if(lowerPath.endsWith(".zip")) dataType = F("application/x-zip");
else if(lowerPath.endsWith(".gz")) dataType = F("application/x-gzip");
return dataType;
}
