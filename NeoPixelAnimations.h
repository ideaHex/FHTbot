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

;
#pragma GCC optimize ("-O2")
#include <NeoPixelBus.h>

NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1800KbpsMethod> strip(6, D4);

int makePositive(int);

void pixelTest(){
    byte brightness = 25; // max is 255
    strip.SetPixelColor(0, RgbColor(random(0,brightness), random(0,brightness), random(0,brightness)));
    strip.SetPixelColor(1, RgbColor(random(0,brightness), random(0,brightness), random(0,brightness)));
    strip.SetPixelColor(2, RgbColor(random(0,brightness), random(0,brightness), random(0,brightness)));
    strip.SetPixelColor(3, RgbColor(random(0,brightness), random(0,brightness), random(0,brightness)));
    strip.SetPixelColor(4, RgbColor(random(0,brightness), random(0,brightness), random(0,brightness)));
    strip.SetPixelColor(5, RgbColor(random(0,brightness), random(0,brightness), random(0,brightness)));
    strip.Show();
}
void smile(){
    int a = 35, b = 0;
    strip.SetPixelColor(0, RgbColor(0,0,0));
    strip.SetPixelColor(1, RgbColor(0,0,0));
    strip.SetPixelColor(2, RgbColor(0,0,0));
    strip.SetPixelColor(3, RgbColor(a,0,0));
    strip.SetPixelColor(4, RgbColor(a,0,0));
    strip.SetPixelColor(5, RgbColor(a,0,0));
    strip.Show();
    delay(2000);
    for ( b = 0; b <= a ; b++){
    strip.SetPixelColor(0, RgbColor(b*(b<0.5*a)+(a-b)*(b>=0.5*a),b,0));
    strip.SetPixelColor(2, RgbColor(b*(b<0.5*a)+(a-b)*(b>=0.5*a),b,0));
    strip.SetPixelColor(3, RgbColor(a-b,0,0));
    strip.SetPixelColor(4, RgbColor(a-b,b,0));
    strip.SetPixelColor(5, RgbColor(a-b,0,0));
    strip.Show();
    delay(4+a-b);
    }
	delay(200);
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

/**
 * Flashes forward LEDs to indicate direction of travel.
 * Should scale to larger LED counts (assuming two rows).
 * Stupid LED Trick mostly.
 * param int indexOfX: x axis on Joystick, left or right.
 * param int indexOfY: y axis on Joystick, forward or back.
 */
void updateBlinkers(int indexOfX, int indexOfY){
  const int NUMPIXELS = 6;
  //Creates array for pixels
  RgbColor px[NUMPIXELS];
  for(int i = 0; i < NUMPIXELS; i++){
    px[i] = RgbColor(0,0,0);
  }
  //Pick which variable is greater in magnitude.
    //If X>Y the result should be 0
    if (makePositive(indexOfX) > 30 ){
      if(indexOfX < 0 ){
        //Update Left
        px[2] = RgbColor(80,40,0);
        px[5] = RgbColor(80,40,0);
      }else if (indexOfX > 0){
        //update Right
        px[0] =  RgbColor(80,40,0);
        px[3] = RgbColor(80,40,0);
      }else{
        //Equals 0 no indication        
      }      
    }else{
      if(indexOfY < 0 ){
        px[0] = RgbColor(80,40,0);
        px[1] = RgbColor(80,40,0);
        px[2] = RgbColor(80,40,0);
      }else if (indexOfY > 0){    
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
    }
    strip.Show(); // This sends the updated pixel color to the hardware.
}
int makePositive(int number){
  if (number < 0){
    number = -number;
  }
  return number;
}
