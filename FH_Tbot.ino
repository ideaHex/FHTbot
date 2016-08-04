/*
 * 
 * 
 */

 
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <Ticker.h>
#include "MotorController.h"
#include "US100Ping.h"

// WebPage
#include "WebPage.h"

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
/*
// stepper without PWM/speed input pins, don't use D0
const int motorLeftA  = D2;
const int motorLeftB  = D3;
const int motorRightA = D4;
const int motorRightB = D5;

motorController motors(motorLeftA,motorLeftB,motorRightA,motorRightB);
*/

// stepper with direction and speed pins, don't use D0 for speed
const int motorLeftDir  = D3;
const int motorLeftSpd  = D2;
const int motorRightDir = D7;
const int motorRightSpd = D8;

motorController motors(motorLeftDir,D5,motorLeftSpd,motorRightDir,D4,motorRightSpd); 

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
WiFiServer server(80);                  // http only https is 443
WiFiClient client;
DNSServer dnsServer;
Ticker HeartBeatTicker;
US100Ping ping;
byte clientTimeout = 150;
bool clientStopped = true;
unsigned long nextClientTimeout = 0;

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
    Stop();
                                        // Serial.println("Connection lost STOP!!!!!!");
  }
}

void setup()
{
  //system_update_cpu_freq(160);        // set cpu to 160MHZ !
  initHardware();
  setupWiFi();
  HeartBeatTicker.attach_ms(500, CheckHeartBeat);
  Stop();
  //motors.setTrim(1.0,1.0);            // this setting is optional, it compensates for speed difference of motors eg (0.95,1.0), and it can reduce maximum speed of both eg (0.75,0.75);
  motors.setSteeringSensitivity(0.25);  // this setting is optional
  motors.setPWMFrequency(50);           // this setting is optional, depending on power supply and H-Bridge this option will alter motor noise and torque.
  motors.setMinimumSpeed(0.05);         // this setting is optional, default is 0.1(10%) to prevent motor from stalling at low speed
}

void loop()
{
  // time dependant functions here
   ping.run();
   if (ping.gotTemperature()) Serial.printf("Temperature: %d C \r\n", ping.getTemperature());
   if (ping.gotDistance()) Serial.printf("distance: %d mm \r\n", ping.getDistance());
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
  client.flush();
  int indexOfX = req.indexOf("/X");
  int indexOfY = req.indexOf("/Y");
  if (indexOfX != -1 && indexOfY != -1){
    
    String xOffset = req.substring(indexOfX + 2, indexOfX + 8);
    int dX = xOffset.toInt();
    String yOffset = req.substring(indexOfY + 2, indexOfY + 8);
    int dY = yOffset.toInt();
    
    motors.update(dX,dY);
    
    //Serial.print(F("DX: "));
    //Serial.print(dX);
    //Serial.print(F(" DY: "));
    //Serial.println(dY);
    //Serial.print(F("Free Ram: "));
    //Serial.println(system_get_free_heap_size());

    HeartBeatRcvd = true;               // recieved data, must be connected

  }else{
        if (req.indexOf("GET / HTTP/1.1") != -1){
            Serial.println(F("Sending Page"));
            client.write_P(HTML_text,strlen_P(HTML_text));
            delay(1);                   // to improve compatability with some browsers
          }
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
  delay(100);
  Serial.println(F("\r\n"));
  Serial.println(F("            FH@Tbot Serial Connected\r\n"));
  Serial.println(F("  Type \"FHTbot.com\" into your browser to connect. \r\n"));
  //ping.begin(D6,D7,9600);
  ping.begin(Serial);
}
