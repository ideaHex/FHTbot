/*
 * 
 * 
 */

 
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <Ticker.h>
#include "MotorController.h"

// WebPage
#include "WebPage.h"

extern "C" { 
   #include "user_interface.h" 
 } 


//////////////////////
// WiFi Definitions //
//////////////////////
const char *password = "12345678";    // This is the Wifi Password (only numbers and letters,  not . , |)
String AP_Name = "FH@Tbot";        // This is the Wifi Name(SSID), some numbers will be added for clarity
#define testCaptivePortal false

void setupWiFi(void);
void initHardware(void);

/////////////////////
// Pin Definitions //
/////////////////////

//const int motorLeftA  = D2;
//const int motorLeftB  = D3;
//const int motorRightA = D4;
//const int motorRightB = D5;
//
//motorController motors(motorLeftA,motorLeftB,motorRightA,motorRightB);
const int motorLeftDir  = D2; 
const int motorLeftSpd  = D3; 
const int motorRightDir = D0; 
const int motorRightSpd = D1; 
 
motorController motors(motorLeftDir,D5,motorLeftSpd,motorRightDir,D4,motorRightSpd); 


WiFiServer server(80); // http only https is 443
DNSServer dnsServer;
Ticker HeartBeatTicker;

bool HeartBeatRcvd = false;

void Stop(void)
{
  motors.update(0,0);
  //Serial.println("STOP");
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
 //   Serial.println("STOP!!!!!!");
  }
}

void setup()
{
  initHardware();
  setupWiFi();
  server.begin();
  HeartBeatTicker.attach_ms(500, CheckHeartBeat);
  Stop();
}

void loop()
{
   dnsServer.processNextRequest();
   WiFiClient client = server.available();
  if (!client)
  {
          return;
  }
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  //Serial.println(client.readString());
  client.flush();

  if (req.indexOf("/X") != -1 && req.indexOf("/Y") != -1){
    String xOffset = req.substring(req.indexOf("/X")+2, req.indexOf("/X")+8);
    int dX = xOffset.toInt();
    Serial.print(F("DX: "));
    Serial.print(dX);

    String yOffset = req.substring(req.indexOf("/Y")+2, req.indexOf("/Y")+8);
    int dY = yOffset.toInt();
    Serial.print(F(" DY: "));
    Serial.println(dY);
    
    motors.update(dX,dY);
    
    Serial.print(F("Free Ram: "));
    Serial.println(system_get_free_heap_size());

    HeartBeatRcvd = true; // recieved data, must be connected
    
   byte max_delay = 150;
   while (client.connected() && max_delay>0){
      delay(1);
      max_delay--;
   }
   //Serial.printf("Timeout delay: %d \r\n", max_delay);
   client.stop();
   
  }else{
          client.write_P(HTML_text,strlen_P(HTML_text));
        }
  delay(1);
}

void setupWiFi()
{
  WiFi.mode(WIFI_AP);
 
  // Create a unique name by appending the MAC address to the AP Name

  String macID = WiFi.softAPmacAddress();
  AP_Name = AP_Name + " " +macID;

  char AP_NameChar[AP_Name.length() + 1];
  AP_Name.toCharArray(AP_NameChar,AP_Name.length() + 1);

  int channel = random(1,13);
  const byte DNS_PORT = 53;
  IPAddress subnet(255, 255, 255, 0);
  IPAddress apIP(192, 168, 1, 1);
  WiFi.softAPConfig(apIP, apIP, subnet);
  WiFi.softAP(AP_NameChar, password , channel , 0 );
   // set all traffic to 192.168.1.1 automatically
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  if (testCaptivePortal)
  dnsServer.start(DNS_PORT, "*", apIP);
}

void initHardware()
{
  Serial.begin(115200);
  delay(100);
  Serial.println("");
  Serial.println("FH@Tbot Serial Connected");
}
