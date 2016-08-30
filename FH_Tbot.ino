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

// WebPage
#include "WebPage.h"

extern "C" { 
   #include "user_interface.h" 
 } 

//////////////////////
// RGB LEDs //
//////////////////////
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
#define PIN D4
#define NUMPIXELS      6
NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(6,D4);//6 [pixels on pin d4]
int delayval = 500; // delay for half a second
boolean useBlinkers = true;
            


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

// stepper without PWM/speed input pins, don't use D0
const int motorLeftA  = D5;
const int motorLeftB  = D6;
const int motorRightA = D3;
const int motorRightB = D2;

motorController motors(motorLeftA,motorLeftB,motorRightA,motorRightB);

/*
// stepper with direction and speed pins, don't use D0 for speed
const int motorLeftDir  = D2;
const int motorLeftSpd  = D1;
const int motorRightDir = D5;
const int motorRightSpd = D6;

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
  //system_update_cpu_freq(160);        // set cpu to 160MHZ !
  initHardware();
  setupWiFi();
  HeartBeatTicker.attach_ms(500, CheckHeartBeat);
  Stop();
  //motors.setTrim(1.0,1.0);            // this setting is optional, it compensates for speed difference of motors eg (0.95,1.0), and it can reduce maximum speed of both eg (0.75,0.75);
  //motors.setSteeringSensitivity(0.5);  // this setting is optional
  motors.setPWMFrequency(50);           // this setting is optional, depending on power supply and H-Bridge this option will alter motor noise and torque.
  //motors.setMinimumSpeed(0.25);         // this setting is optional, default is 0.1(10%) to prevent motor from stalling at low speed
    
  strip.Begin();
  strip.Show(); // Initialize all pixels to 'off'
     //////////////////////////
 for(int i=0;i<NUMPIXELS;i++){

    // strip.Color takes RGB values, from 0,0,0 up to 255,255,255
    strip.SetPixelColor(i, RgbColor(0,40,0)); // Moderately bright green color.

  }
  strip.Show(); // This sends the updated pixel color to the hardware.
  //delay(delayval); // Delay for a period of time (in milliseconds).
    

  
  /////////////////////////////////////////////
  
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

    //Blinker Addon
    //Only operates if enabled in RGB options above
    if(useBlinkers == true){
      updateBlinkers(dX,dY);
    }

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
  Serial.begin(115200);
  delay(100);
  Serial.println(F("\r\n"));
  Serial.println(F("            FH@Tbot Serial Connected\r\n"));
  Serial.println(F("  Type \"FHTbot.com\" into your browser to connect. \r\n"));
  Serial.swap();
  //ping.begin(D7,D8,9600);
  ping.begin(Serial);
}
