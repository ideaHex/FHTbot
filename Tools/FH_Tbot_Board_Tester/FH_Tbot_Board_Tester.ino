/*
Copyright 2017, Tilden Groves.

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
#pragma GCC optimize("-O2")
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include "RCW0006Ping.h"
#include "botVoltage.h"
#include <NeoPixelBus.h>

//////////////////////
// WiFi Definitions //
//////////////////////

const char *password = "12345678"; // This is the Wifi Password (only numbers and letters,  not . , |)
String AP_Name = "FHTbot"; // This is the Wifi Name(SSID), some numbers will be added for clarity (mac address)
bool enableCompatibilityMode = false; // turn on compatibility mode for older devices, specifically sets no encryption and 11B wifi standard

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

///////////////////////////
// Remaining Definitions //
///////////////////////////

volatile bool leftInterruptTriggered = false;
volatile bool rightInterruptTriggered = false;
NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(6, D4);
WiFiServer server(80);
DNSServer dnsServer;

void motorLeftEncoderInterruptHandler(void);
void motorRightEncoderInterruptHandler(void);
void setColor(RgbColor);
void setupWiFi(void);

void setup() {
	Serial.begin(2000000);
	setupWiFi();
	pingSetup();
	Serial.println("\r\n");
	Serial.println("Board tester installed and running");
	Serial.println("Plug WeMos into PCB and power with batteries");
	Serial.println("Each function will change the LED colours");
	Serial.println("Encoders will also run motors if attached");
	Serial.println("* If a second colour appears during start-up there is a problem with the voltage sensor");
	delay(100);
	Serial.end(); // disable serial interface
	pinMode(leftBumper,INPUT_PULLUP);
	pinMode(rightBumper,INPUT_PULLUP);
	pinMode(motorLeftA,OUTPUT);
	pinMode(motorLeftB,OUTPUT);
	pinMode(motorRightA,OUTPUT);
	pinMode(motorRightB,OUTPUT);
	strip.Begin();
	strip.Show();
	attachInterrupt(motorLeftEncoder, motorLeftEncoderInterruptHandler , CHANGE);
	attachInterrupt(motorRightEncoder, motorRightEncoderInterruptHandler , CHANGE);
	delay(100);
	setColor(RgbColor(20, 30, 10));
	delay(2000);
	setColor(RgbColor(0, 0, 0));
	resetVoltageFilter();
	updateVoltage();
    float voltage = getCurrentVoltage();
	if (voltage<4 || voltage>7){
		setColor(RgbColor(255, 30, 10));
		delay(6000);
		setColor(RgbColor(0, 0, 0));
	}
}

void loop() {
	getDistance(); // ping pulse/update function must be called to ping
	int distance = getMedian();
	if (distance < 100 && distance > 0){
		setColor(RgbColor(80, 0, 80));
		delay(55);
		setColor(RgbColor(0, 0, 0));
  }
	if(digitalRead(leftBumper) == LOW || digitalRead(rightBumper) == LOW){
		setColor(RgbColor(0, 80, 80));
		delay(500);
		setColor(RgbColor(0, 0, 0));
  }
  	if (rightInterruptTriggered){
		setColor(RgbColor(0, 80, 0));
		digitalWrite(motorRightA,LOW);
		digitalWrite(motorRightB,HIGH);
		delay(1000);
		digitalWrite(motorRightA,LOW);
		digitalWrite(motorRightB,LOW);
		setColor(RgbColor(0, 0, 0));
		delay(1000);
		rightInterruptTriggered = false;
	}
	if (leftInterruptTriggered){
		setColor(RgbColor(80, 0, 0));
		digitalWrite(motorLeftA,LOW);
		digitalWrite(motorLeftB,HIGH);
		delay(1000);
		digitalWrite(motorLeftA,LOW);
		digitalWrite(motorLeftB,LOW);
		setColor(RgbColor(0, 0, 0));
		delay(1000);
		leftInterruptTriggered = false;
	}
}

void motorLeftEncoderInterruptHandler(){
		leftInterruptTriggered = true;
}
void motorRightEncoderInterruptHandler(){
		rightInterruptTriggered = true;
}

void setColor(RgbColor color){
    strip.SetPixelColor(0, color);
    strip.SetPixelColor(1, color);
    strip.SetPixelColor(2, color);
    strip.SetPixelColor(3, color);
    strip.SetPixelColor(4, color);
    strip.SetPixelColor(5, color);
    strip.Show();
}

void setupWiFi(){
  WiFi.mode(WIFI_AP);
  // Create a unique name by appending the MAC address to the AP Name
  AP_Name = AP_Name + " " + WiFi.softAPmacAddress();
  char AP_NameChar[AP_Name.length() + 1];
  AP_Name.toCharArray(AP_NameChar, AP_Name.length() + 1);
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
  dnsServer.start(DNS_PORT, "*", apIP); // default FHTbot.com  //must use '.com, .org etc..' and cant use '@ or _ etc...' ! . Use "*" to divert all **VALID** names
  server.begin();
  server.setNoDelay(true);
}