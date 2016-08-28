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
void pixelTest(void);

/////////////////////
// Pin Definitions //
/////////////////////

// D4 is used for neoPixelBus (TXD1)

// stepper without PWM/speed input pins, don't use D0
const int motorLeftA  = D5;
const int motorLeftB  = D6;
const int motorRightA = D1;
const int motorRightB = D2;
const int motorLeftEncoder = D3;
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
WiFiServer server(80);                  // http only https is 443
WiFiClient client;
DNSServer dnsServer;
Ticker HeartBeatTicker;
US100Ping ping;
byte clientTimeout = 150;
bool clientStopped = true;
unsigned long nextClientTimeout = 0;
NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(6, D4); // 6 pixels, output pin D4, function ignores pin
encoder encoderA;
encoder encoderB;

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

void setup()
{
  system_update_cpu_freq(80);        // set cpu to 80MHZ or 160MHZ !
  initHardware();
  setupWiFi();
  HeartBeatTicker.attach_ms(500, CheckHeartBeat);
  motors.setTrim(0.95,1.0);            // this setting is optional, it compensates for speed difference of motors eg (0.95,1.0), and it can reduce maximum speed of both eg (0.75,0.75);
  //motors.setSteeringSensitivity(0.9);  // this setting is optional
  motors.setPWMFrequency(60);           // this setting is optional, depending on power supply and H-Bridge this option will alter motor noise and torque.
  motors.setMinimumSpeed(0.07);         // this setting is optional, default is 0.1(10%) to prevent motor from stalling at low speed
}
int temperature = 0;
int distance = 0;
void loop()
{
  // time dependant functions here
   encoderA.run();
   encoderB.run();
   ping.run();
   if (ping.gotTemperature()){
    temperature = ping.getTemperature();
    //Serial.printf("Temperature: %d C \r\n", temperature);
   }
   if (ping.gotDistance()){
    distance = ping.getDistance();
    if (distance < 130){
        motors.update(0,80);
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
    if (distance < 450 && dY < 0){
      dX = 500 - distance;
      if (dY < -40 ){
        //dX = 0;
        dY = -40;
      }
    }
    motors.update(dX,dY);
    
    //Serial.print(F("DX: "));
    //Serial.print(dX);
    //Serial.print(F(" DY: "));
    //Serial.println(dY);
    //Serial.print(F("Free Ram: "));
    //Serial.println(system_get_free_heap_size());

  }else{
        if (req.indexOf("GET / HTTP/1.1") != -1){
          //req = client.readString();
          //Serial.println(req);
          //Serial.println(F("Sending Page"));
          if (req.indexOf("Chrome") != -1){
            //client.print("<html><head></head><body>Your browser is not fully supported</body></html>");
          }
            int dataLength = strlen_P(HTML_text);
            client.write_P(HTML_text , dataLength);
            delay(1);                   // to improve compatability with some browsers
          }
          if (req.indexOf("feedback") != -1){
                String s;
                s = F("HTTP/1.1 200 OK\r\n"); 
                s += F("Content-Type: text/html\r\n\r\n");
                s += F("<meta http-equiv='refresh' content='1' />");
                s += F("<!DOCTYPE HTML>\r\n<html>\r\n");
              //  s += F("<head><style> body{color : White; text-shadow: 1px 1px Black;font-size: 20px; -webkit-user-select : none; -moz-user-select  : none; -khtml-user-select : none; -ms-user-select : none; user-select: none; -o-user-select:none; overflow:hidden;}</style></head>");
                s += F("<body>");
                s += F("<script> var tmp = '");//
                s += String(temperature);
                s += F("';var dis = '");
                s += String(distance);
                s += F("';");
                s += F("var kph = '");
                s += String( float(encoderA.getAngularVelocity() * ((70 * PI) / 360) * 0.0036) );
                s += F("';");
                s += F("</script></body></html>\n");
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
  //ping.begin(D6,D7,9600);
  delay(200);
  Serial.swap();
  ping.begin(Serial);
  strip.Begin();
  strip.Show();
  smile();
  encoderA.begin(motorLeftEncoder , 40);
  encoderB.begin(motorRightEncoder , 40);
}

void pixelTest(){
  // this resets all the neopixels to an off state
    byte brightness = 50; // max is 255
    strip.SetPixelColor(0, RgbColor(random(0,brightness), random(0,brightness), random(0,brightness)));
    strip.SetPixelColor(1, RgbColor(random(0,brightness), random(0,brightness), random(0,brightness)));
    strip.SetPixelColor(2, RgbColor(random(0,brightness), random(0,brightness), random(0,brightness)));
    strip.SetPixelColor(3, RgbColor(random(0,brightness), random(0,brightness), random(0,brightness)));
    strip.SetPixelColor(4, RgbColor(random(0,brightness), random(0,brightness), random(0,brightness)));
    strip.SetPixelColor(5, RgbColor(random(0,brightness), random(0,brightness), random(0,brightness)));
    strip.Show();
}
void smile(){
    int a = 25, b = 0;
    strip.SetPixelColor(0, RgbColor(0,0,0));
    strip.SetPixelColor(1, RgbColor(0,0,0));
    strip.SetPixelColor(2, RgbColor(0,0,0));
    strip.SetPixelColor(3, RgbColor(a,0,0));
    strip.SetPixelColor(4, RgbColor(a,0,0));
    strip.SetPixelColor(5, RgbColor(a,0,0));
    strip.Show();
    delay(3000);
    for ( b = 0; b <= a ; b++){
    strip.SetPixelColor(0, RgbColor(b*(b<0.5*a)+(a-b)*(b>=0.5*a),b,0));
    strip.SetPixelColor(2, RgbColor(b*(b<0.5*a)+(a-b)*(b>=0.5*a),b,0));
    strip.SetPixelColor(3, RgbColor(a-b,0,0));
    strip.SetPixelColor(4, RgbColor(a-b,b,0));
    strip.SetPixelColor(5, RgbColor(a-b,0,0));
    strip.Show();
    delay(4+a-b);
    }
}

