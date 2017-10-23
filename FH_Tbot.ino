/*
Copyright 2017, Tilden Groves, Alexander Battarbee.

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

#pragma GCC optimize("-O2") // O0 none, O1 Moderate optimization, 02, Full
                            // optimization, O3, as O2 plus attempts to
                            // vectorize loops, Os Optimize space
#include "EncoderMotorControl.h"
#include "NeoPixelAnimations.h"
#include "RCW0006Ping.h"
#include "botTemp.h"
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <NeoPixelBus.h>
#include <Ticker.h>
#include <WebSockets.h>
#include <WebSocketsClient.h>
#include <WebSocketsServer.h>

extern "C" {
#include "user_interface.h"
}

//////////////////////
// WiFi Definitions //
//////////////////////

const char *password = "12345678"; // This is the Wifi Password (only numbers
                                   // and letters,  not . , |)
String AP_Name = "FHTbot"; // This is the Wifi Name(SSID), some numbers will be
                           // added for clarity (mac address)
bool enableCompatibilityMode = false; // turn on compatibility mode for older
                                      // devices, spacifically sets no
                                      // encryption and 11B wifi standard

void setupWiFi(void);
void setupWebsocket(void);
void initHardware(void);
void sendFile(String);
String getContentType(String);
void updateMotors();
void updTemp();
void testBumper();
void leftBumperHitFunction();
void leftBumperReset();
void rightBumperHitFunction();
void rightBumperReset();
void checkBoredBot();
void motorLeftEncoderInterruptHandler();
void motorRightEncoderInterruptHandler();
void updateClient();

/////////////////////
// Pin Definitions //
/////////////////////

// D4 is used for neoPixelBus (TXD1)
// D0 is used to trigger ping
// D8 is used for echo of ping
#define D9 3 // D9 & D10 aren't defined so define them here
#define D10 1
const int motorLeftA = D6;
const int motorLeftB = D5;
const int motorRightA = D2;
const int motorRightB = D3;
const int motorLeftEncoder = D7;
const int motorRightEncoder = D1;
const int leftBumper = D9;
const int rightBumper = D10;

encoderMotorController motors(motorLeftA, motorLeftB, motorRightA, motorRightB,
                              motorLeftEncoder, motorRightEncoder);
Ticker motorControllerTicker;
Ticker tempTicker;
Ticker lBH; // left bumper hit reverse timer
Ticker rBH; // right bumper hit reverse timer
WiFiServer server(80);
WiFiClient client;
WebSocketsServer webSocket = WebSocketsServer(81);
DNSServer dnsServer;
Ticker HeartBeatTicker;
int distance = 500;
boolean driverAssist = false;
bool HeartBeatRcvd = false;
String closeConnectionHeader = "";
#define MAX_SRV_CLIENTS 10 // maximum client connections
WiFiClient serverClients[MAX_SRV_CLIENTS];
int currentClient = 0;
boolean pingOn = false;
long nextBoredBotEvent = 0;
int boredBotTimeout = 60000; // in ms
boolean leftBumperHit = false;
boolean rightBumperHit = false;
boolean autoMode = false; // drive mode with no client connected
long autoModeNextUpdate = 0;
long autoModeNextEvent = 0;
#define Diag                           // if not defined disables serial communication after initial feedback
long timerPing;
short updateRound = 0;

void Stop(void) {
  motors.manualDrive(0, 0);
  setColor(RgbColor(0, 0, 0)); // turn off LED's to save power
  pingOn = false;              // turn off ping to save power
  driverAssist = false;
}

void CheckHeartBeat(void) {
  if (HeartBeatRcvd == true) {
    HeartBeatRcvd = false;
    nextBoredBotEvent = millis() + boredBotTimeout; // reset bored bot timer
  } else {
    Stop();
  }
}

void setup() {
  system_update_cpu_freq(80); // set cpu to 80MHZ or 160MHZ !
  system_phy_set_max_tpw(10);  // 0 - 82 radio TX power
  initHardware();
  setupWiFi();
  setupWebsocket();
  HeartBeatTicker.attach_ms(991, CheckHeartBeat);
  closeConnectionHeader +=
      F("HTTP/1.1 204 No Content\r\nConnection: Close\r\n\r\n");
  nextBoredBotEvent = millis() + boredBotTimeout;
}

void loop() {
  // time dependant functions here
  checkBoredBot();
  checkAutoMode();
  if (pingOn) {
    getDistance(); // ping pulse/update function must be called to ping
    distance = getMedian();
    if (driverAssist) {
      if (distance < 200) { // too close bounce back
        setColor(RgbColor(255, 0, 0));
        motors.manualDrive(0, 500);
      }
    }
  }

  if (driverAssist) {
    testBumper();
  }
   dnsServer.processNextRequest();      // update DNS requests

  //Loop WebSocket Server
  webSocket.loop();
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
    // no free/disconnected spot so reject
    // WiFiClient serverClient = server.available();
    // serverClient.stop();
  }
  // check clients for data
  String req = "";
  for(uint8_t i = currentClient; i < MAX_SRV_CLIENTS; i++){// start at current client to keep in order
    if (serverClients[i] && serverClients[i].connected()){
        if (serverClients[i].available()){
          req = serverClients[i].readStringUntil('\r');   // Read the first line of the request
          //Serial.println((serverClients[i].readString()+"\r\n"));
          serverClients[i].flush();
          currentClient = i;
          break;
        }
    }
    currentClient = 0;
  }
  executeRequest(req);
}
/**
 * A faster version of exeReq for websockets.
 * Features WS response and a shorter command list.
 */
void WSRequest(String req, int clientNum) {
  // Empty request tripwire
  if (!req.length()) {
    // empty request
    return;
  }
  HeartBeatRcvd = true;
  if (req.indexOf("/HB") != -1) {
    // Fast Catch and Release, HB handled above.
    updateClient();
    yield();
    return;
  }
  // Serial.println("\r\n" + req);
  int indexOfX = req.indexOf("/X");
  int indexOfY = req.indexOf("/Y");
  if (indexOfX != -1 && indexOfY != -1) {
    pingOn = true;
    int indexOfHB = req.indexOf("/DA");
    if (indexOfHB != -1){
      driverAssist = true;
      }
    if (req.indexOf("/DM") != -1){
      driverAssist = false;
      }
    // serverClients[currentClient].write(
    // closeConnectionHeader.c_str(),closeConnectionHeader.length() );
    yield();
    String xOffset = req.substring(indexOfX + 2, indexOfX + 8);
    int dX = xOffset.toInt();
    String yOffset = req.substring(indexOfY + 2, indexOfY + 8);
    int dY = yOffset.toInt();
    #ifdef Diag
    Serial.println("dX"+String(dX)+"dY"+String(dY));
    #endif
    // driver assist
    if (driverAssist) {
      updateBlinkers(dX, dY);
      if (distance < 450 && distance > 199 && dY < 0) {
        setColor(RgbColor(90, 105, 95));
        motors.hardRightTurn();
        dX = 500;
        dY = -100;
      }
      if (distance < 200) {
        setColor(RgbColor(255, 0, 0));
        dY = 500;
      }
      updateClient();
    } else {
      pixelTest();
    }
    motors.manualDrive(dX, dY);
  }else{
    //TODO REVIEW THESE FOR FALSE ACTIVATION LS PROVED TO HIT
  if (req.indexOf("data,") != -1) {
    // serverClients[currentClient].write(
    // closeConnectionHeader.c_str(),closeConnectionHeader.length() );
    yield();
    req.remove(0, req.indexOf("data,"));
    req.trim();
    motors.startCommandSet(req);
    return;
  }
  if (req.indexOf("save,") != -1) {
    //File pushed by turtle mode to be saved
    //save,fileName,xmlstring
    #ifdef Diag
    Serial.println("Save Request:" + req);
    #endif
    String dataString = req.substring(req.indexOf(","));
    int titleComma = dataString.indexOf(",");
    String fileName = "/T/" + dataString.substring(0,titleComma);
    File dataFile = SPIFFS.open(fileName,"w");
    if(dataFile){
      //Write to file
      dataFile.print(dataString.substring(titleComma));
      dataFile.close();
      //respond to client
      webSocket.sendTXT(clientNum, "File Saved Successfully");
    }else{
      #ifdef Diag
      Serial.println("Failed to create file");
      #endif
      //respond to client
      webSocket.sendTXT(clientNum, "Failed to create file. May already exist.");
    }
  }
  //Listing directory contents
  if(req.indexOf("ls,")!= 1){
     //load dir object
     Dir dir = SPIFFS.openDir(req.substring(req.indexOf(",")));
     String out = "";
     while(dir.next()){
      File f = dir.openFile("r");
      //Compiling comma delimited list of file names
      out += dir.fileName();
     }
     #ifdef Diag
     Serial.println("output:" + out);
     #endif
     webSocket.sendTXT(clientNum, out);
  }
  }
}
void executeRequest(String req) {
  if (!req.length()) { // empty request
    return;
  }
  HeartBeatRcvd = true; // received data, must be connected
  if (autoMode) {
    autoMode = false; // disable autoMode if client sends packet
    HeartBeatRcvd = false;
    Stop();
  }
  // Serial.println("\r\n" + req);
  int indexOfX = req.indexOf("/X");
  int indexOfY = req.indexOf("/Y");
  if (indexOfX != -1 && indexOfY != -1) {
    pingOn = true;
    if (req.indexOf("/HBDA") != -1)
      driverAssist = true;
    if (req.indexOf("/HBDM") != -1)
      driverAssist = false;
    serverClients[currentClient].write(closeConnectionHeader.c_str(),
                                       closeConnectionHeader.length());
    yield();
    String xOffset = req.substring(indexOfX + 2, indexOfX + 8);
    int dX = xOffset.toInt();
    String yOffset = req.substring(indexOfY + 2, indexOfY + 8);
    int dY = yOffset.toInt();
    // driver assist
    if (driverAssist) {
      updateBlinkers(dX, dY);
      if (distance < 450 && distance > 199 && dY < 0) {
        setColor(RgbColor(90, 105, 95));
        motors.hardRightTurn();
        dX = 500;
        dY = -100;
      }
      if (distance < 200) {
        setColor(RgbColor(255, 0, 0));
        dY = 500;
      }
    } else {
      pixelTest();
    }
    motors.manualDrive(dX, dY);
  } else {
    String fileString = req.substring(4, (req.length() - 9));
    // Serial.println("\r\n" + fileString);
    if (fileString.indexOf("/HB") != -1) {
      pingOn = false;
      driverAssist = false;
      HeartBeatRcvd = true;
      serverClients[currentClient].write(closeConnectionHeader.c_str(),
                                         closeConnectionHeader.length());
      yield();
      return;
    }
    if (fileString.indexOf("data,") != -1) {
      serverClients[currentClient].write(closeConnectionHeader.c_str(),
                                         closeConnectionHeader.length());
      yield();
      fileString.remove(0, fileString.indexOf("data,"));
      fileString.trim();
      motors.startCommandSet(fileString);
      return;
    }
    if (fileString.indexOf("feedback") !=
        -1) { // send feedback to drive web page
      int temperature = getCurrentTemperature();
      String s, h;
      s = F("<!DOCTYPE HTML><html><head><meta http-equiv='refresh' "
            "content='1'></head><body><script>");
      s += (";var tmp=");
      s += temperature;
      s += (";var dis=");
      s += distance;
      s += (";var kph=");
      s += motors.getSpeed();
      s += (";var movd=");
      s += motors.getTravel();
      s += (";var hed=");
      s += motors.getheading();
      s += (";</script></body></html>");
      h = F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: "
            "Close\r\ncontent-length: ");
      h += s.length();
      h += F("\r\n\r\n");
      String ss = h + s;
      serverClients[currentClient].write(ss.c_str(), ss.length());
      yield();
      return;
    }
    if (fileString.indexOf("write,") != -1) {
      serverClients[currentClient].write(closeConnectionHeader.c_str(),
                                         closeConnectionHeader.length());
      yield();
      fileString.remove(0, fileString.indexOf("write,"));
      fileString.trim();
      // TODO Save File

      return;
    }
    if (fileString.indexOf("/PlayCharge") != -1) {
      serverClients[currentClient].write(closeConnectionHeader.c_str(),
                                         closeConnectionHeader.length());
      yield();
      motors.playCharge();
      return;
    }
    if (fileString.indexOf("/PlayMarch") != -1) {
      serverClients[currentClient].write(closeConnectionHeader.c_str(),
                                         closeConnectionHeader.length());
      yield();
      motors.playMarch();
      return;
    }
    if (fileString.indexOf("/PlayMarioTheme") != -1) {
      serverClients[currentClient].write(closeConnectionHeader.c_str(),
                                         closeConnectionHeader.length());
      yield();
      motors.playMarioMainThem();
      return;
    }
    if (fileString.indexOf("/PlayMarioUnderworld") != -1) {
      serverClients[currentClient].write(closeConnectionHeader.c_str(),
                                         closeConnectionHeader.length());
      yield();
      motors.playMarioUnderworld();
      return;
    }
    sendFile(fileString);
  }
}
/**
 * Handles Websocket reception, deciding what needs to be done vs the type of
 * message.
 */
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                    size_t payLength) {

  #ifdef Diag
  Serial.println("WEBSOCKET EVENT");
  Serial.println(millis() - timerPing);
  #endif
  timerPing = millis();
  switch (type) {
  case WStype_DISCONNECTED: {
    // perform disconnection events (i.e. send bot to idle.)
    IPAddress ip = webSocket.remoteIP(num);
    #ifdef Diag
    Serial.println(String(num) + " Client from " + ip[0] + "." + ip[1] + "." + ip[2] + "." + ip[3] + " Has Disconnected " + "\n");
    #endif
    // Stop Bot
    Stop();
  } break;
  case WStype_CONNECTED: {
    // HANDLE NEW CLIENT CONNECTION
    IPAddress ip = webSocket.remoteIP(num);
    #ifdef Diag
    Serial.println(String(num) + " Connected from " + ip[0] + "." + ip[1] +
                   "." + ip[2] + "." + ip[3] + " url: " + String(*payload) +
                   "\n");
    #endif
    // ACK connection to client
    webSocket.sendTXT(num, "Connected to FH_Tbot");
  } break;
  case WStype_TEXT: {
    // Perform actions based on a good payload.
    #ifdef Diag
    Serial.println("Starting charstream to char array conversion");
    #endif
    char A[payLength + 1];
    char *strncpy(char *A, const char *payload, size_t payLength);
    A[payLength] = '\0';
    String b((char *)payload);
    #ifdef Diag
    Serial.println(String(num) + "get Text: " + b +
                   " length: " + String(payLength) + "\n");
    #endif
    HeartBeatRcvd = true;
    WSRequest(b, num);
    // Feedback
  } break;
  }
}

void setupWiFi() {
  WiFi.mode(WIFI_AP);

  // Create a unique name by appending the MAC address to the AP Name

  AP_Name = AP_Name + " " + WiFi.softAPmacAddress();
  char AP_NameChar[AP_Name.length() + 1];
  AP_Name.toCharArray(AP_NameChar, AP_Name.length() + 1);

  /*
   * struct softap_config {
      uint8 ssid[32];
      uint8 password[64];
      uint8 ssid_len;
      uint8 channel; // support 1 ~ 13
      uint8 authmode; // Don’t support AUTH_WEP in SoftAP mode
      uint8 ssid_hidden; // default 0
      uint8 max_connection; // default 4, max 4
      uint16 beacon_interval; // 100 ~ 60000 ms, default 100
    };
   */
  // setup AP, start DNS server, start Web server

  int channel = random(1, 13 + 1); // have to add 1 or will be 1 - 12
  const byte DNS_PORT = 53;
  IPAddress subnet(255, 255, 255, 0);
  IPAddress apIP(192, 168, 1, 1);
  WiFi.softAPConfig(apIP, apIP, subnet);
  if (enableCompatibilityMode) {
    wifi_set_phy_mode(PHY_MODE_11B); // Note: ESP8266 soft-AP only support bg.
    const char *pw = "";
    WiFi.softAP(AP_NameChar, pw, channel, 0);
  } else {
    wifi_set_phy_mode(PHY_MODE_11N);
    WiFi.softAP(AP_NameChar, password, channel, 0);
  }
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP); // default FHTbot.com  //must use '.com,
                                        // .org etc..' and cant use '@ or _
                                        // etc...' ! . Use "*" to divert all
                                        // **VALID** names
  server.begin();
  server.setNoDelay(true);
}

void setupWebsocket() {
  // start websocket
  #ifdef Diag
  Serial.println("Establishing Websocket Server");
  #endif
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void initHardware() {
  Serial.begin(250000);
  Serial.println(F("\r\n"));
  Serial.println(F("            FHTbot Serial Connected\r\n"));
  Serial.println(F("\r\n         Disable any form of assisted WIFI\r\n"));
  Serial.print(F("\r\n  Your FHTbot Wifi connection is called "));
  Serial.println(AP_Name + " " + WiFi.softAPmacAddress());
  Serial.print(F("\r\n  Your password is "));
  Serial.println(password);
  Serial.println(
      F("\r\n  Type FHTbot.com into your browser after you connect. \r\n"));
  SPIFFS.begin();
  delay(200);
  #ifndef Diag
  Serial.end(); // disable serial interface
  #endif
  pingSetup();
  strip.Begin();
  strip.Show();
  smile();
  // setup motors and encoders
  attachInterrupt(motorLeftEncoder, motorLeftEncoderInterruptHandler, CHANGE);
  attachInterrupt(motorRightEncoder, motorRightEncoderInterruptHandler, CHANGE);
  motorControllerTicker.attach_ms(motors.updateFrequency,
                                  updateMotors); // attach motor update timer
  tempTicker.attach_ms(200, updTemp); // attach temperature sample timer
  #ifndef Diag
  pinMode(leftBumper, INPUT_PULLUP);
  pinMode(rightBumper, INPUT_PULLUP);
  // test for startup auto mode
  if (digitalRead(leftBumper) == LOW) { // test for startup auto mode
    autoMode = true;
    motors.playCharge();
    autoModeNextUpdate = millis() + 300;
    autoModeNextEvent = millis() + random(10000, 30000);
  };
  attachInterrupt(leftBumper, leftBumperHitFunction, FALLING);
  attachInterrupt(rightBumper, rightBumperHitFunction, FALLING);
  #endif
}
void leftBumperHitFunction() {
  if (!leftBumperHit) {
    lBH.once_ms(500, leftBumperReset);
    // setColor(RgbColor(255,0,0));
  }
  leftBumperHit = true;
}
void leftBumperReset() { leftBumperHit = false; }
void rightBumperHitFunction() {
  if (!rightBumperHit) {
    rBH.once_ms(500, rightBumperReset);
    // setColor(RgbColor(255,0,0));
  }
  rightBumperHit = true;
}
void rightBumperReset() { rightBumperHit = false; }

void motorLeftEncoderInterruptHandler() { motors.takeStep(0); }
void motorRightEncoderInterruptHandler() { motors.takeStep(1); }

void updateMotors() { motors.update(); }

void sendFile(String path) {
  // get content type
  if (path.endsWith("/")) {
    path += "index.html";
  }
  String dataType = getContentType(path);

  // check if theres a .gz'd version and send that instead
  String gzPath = path + ".gz";
  File theBuffer;
  if (SPIFFS.exists(
          gzPath)) { // test to see if there is a .gz version of the file
    theBuffer = SPIFFS.open(gzPath, "r");
    path = gzPath; // got it, use this path
  } else {         // not here so load the standard file
    theBuffer = SPIFFS.open(path, "r");
    if (!theBuffer) { // this one doesn't exist either, abort.
      // Serial.println(path + "Does Not Exist");
      theBuffer.close();
      String notFound = F("HTTP/1.1 404 Not Found\r\nConnection: "
                          "Close\r\ncontent-length: 0\r\n\r\n");
      serverClients[currentClient].write(notFound.c_str(), notFound.length());
      // serverClients[currentClient].write(
      // closeConnectionHeader.c_str(),closeConnectionHeader.length() );
      yield();
      return; // failed to read file
    }
  }

  // make header
  String s =
      F("HTTP/1.1 200 OK\r\ncache-control: max-age = 3600\r\ncontent-length: ");
  s += theBuffer.size();
  s += F("\r\ncontent-type: ");
  s += dataType;
  s += F("\r\nconnection: close"); // last one added X-Content-Type-Options:
                                   // nosniff\r\n
  if (path.endsWith(".gz")) {
    s += F("\r\nContent-Encoding: gzip\r\n\r\n");
  } else {
    s += F("\r\n\r\n");
  }
  // send the file
  if (!serverClients[currentClient].write(s.c_str(), s.length())) {
    // failed to send
    theBuffer.close();
    return;
  }
  int bufferLength = theBuffer.size();
  if (serverClients[currentClient].write(theBuffer, 2920) < bufferLength) {
    // failed to send all file
  }
  yield();
  theBuffer.close();
}

String getContentType(String path) { // get content type
  String dataType = F("text/html");
  String lowerPath = path.substring(path.length() - 4, path.length());
  lowerPath.toLowerCase();

  if (lowerPath.endsWith(".src"))
    lowerPath = lowerPath.substring(0, path.lastIndexOf("."));
  else if (lowerPath.endsWith(".gz"))
    dataType = F("application/x-gzip");
  else if (lowerPath.endsWith(".html"))
    dataType = F("text/html");
  else if (lowerPath.endsWith(".htm"))
    dataType = F("text/html");
  else if (lowerPath.endsWith(".png"))
    dataType = F("image/png");
  else if (lowerPath.endsWith(".js"))
    dataType = F("application/javascript");
  else if (lowerPath.endsWith(".css"))
    dataType = F("text/css");
  else if (lowerPath.endsWith(".gif"))
    dataType = F("image/gif");
  else if (lowerPath.endsWith(".jpg"))
    dataType = F("image/jpeg");
  else if (lowerPath.endsWith(".ico"))
    dataType = F("image/x-icon");
  else if (lowerPath.endsWith(".svg"))
    dataType = F("image/svg+xml");
  else if (lowerPath.endsWith(".mp3"))
    dataType = F("audio/mpeg");
  else if (lowerPath.endsWith(".wav"))
    dataType = F("audio/wav");
  else if (lowerPath.endsWith(".ogg"))
    dataType = F("audio/ogg");
  else if (lowerPath.endsWith(".xml"))
    dataType = F("text/xml");
  else if (lowerPath.endsWith(".pdf"))
    dataType = F("application/x-pdf");
  else if (lowerPath.endsWith(".zip"))
    dataType = F("application/x-zip");

  return dataType;
}

void checkBoredBot() {
  if (millis() > nextBoredBotEvent) { // bored bot event called here
    nextBoredBotEvent =
        millis() + boredBotTimeout * 0.5; // reset bored bot timer
    // different events to be put here
    int events = 4;
    int pickedEvent = random(1, (events + 1));
    switch (pickedEvent) {

    case 1: // play vroom and bright light
      setColor(RgbColor(80, 80, 80));
      motors.playVroom();
      break;

    case 2: // random colors
      for (int a = 0; a < 50; a++) {
        pixelTest();
        delay(20);
      }
      break;

    case 3: // blinker rotation
      for (int a = 0; a < 4; a++) {
        delay(100);
        updateBlinkers(0, -1);
        delay(100);
        updateBlinkers(60, -1);
        delay(100);
        updateBlinkers(0, 1);
        delay(100);
        updateBlinkers(-60, -1);
      }
      break;

    case 4: // smile
      smile();
      break;
    }
  }
}

void updTemp() { updateTemperature(); }

void testBumper() {
#ifndef Diag
  if (!leftBumperHit && rightBumperHit) {
    motors.manualDrive(-250, 500);
    motors.hardLeftTurn();
  }
  if (!rightBumperHit && leftBumperHit) {
    motors.manualDrive(250, 500);
    motors.hardRightTurn(); // in reverse right becomes left
  }
  if (rightBumperHit && leftBumperHit) {
    motors.manualDrive(0, 500);
  }
#endif
}

void checkAutoMode() {
  if (autoMode) {
    HeartBeatRcvd = true;
    pingOn = true;
    driverAssist = true;
    int dX = -1;
    int dY = -500;
    if (millis() > autoModeNextUpdate) {
      updateBlinkers(dX, dY);
      autoModeNextUpdate = millis() + 500;
      if (distance < 450 && distance > 199 && dY < 0) {
        setColor(RgbColor(90, 105, 95));
        motors.hardRightTurn();
        dX = 500;
        dY = -100;
        autoModeNextUpdate = millis() + 2000;
      }
      if (distance < 200) {
        setColor(RgbColor(255, 0, 0));
        dY = 500;
        autoModeNextUpdate = millis() + 1500;
      }
      motors.manualDrive(dX, dY);
    }
    if (millis() > autoModeNextEvent) {
      autoModeNextEvent = millis() + random(10000, 30000);
      setColor(RgbColor(30, 105, 35));
      motors.hardRightTurn();
      dX = 500;
      dY = -100;
      motors.manualDrive(dX, dY);
      autoModeNextUpdate = millis() + 3000;
      // String data = "data,L,120,";
      // motors.startCommandSet(data);
    }
  }
}
/**
 * Updates websocket client with distance/temperature information.
 * TODO CHANGE TO CHAR ARRAY
 */
void updateClient() {

  String s = "/T";
  s.concat(getCurrentTemperature());
  s.concat(",/D");
  s.concat(distance);
  webSocket.sendTXT(0, s);
  #ifdef Diag
  Serial.println("Return Message: " + s);
  #endif
}
