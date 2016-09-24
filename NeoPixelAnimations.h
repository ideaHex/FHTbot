#include <NeoPixelBus.h>

NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(6, D4);
int makePositive(int);

void pixelTest(){
    byte brightness = 20; // max is 255
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
  int NUMPIXELS = 6;
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
        //px[(NUMPIXELS/2 - 1)] = RgbColor(255,127,0);
        //px[NUMPIXELS - 1] =  RgbColor(255,127,0);
        px[2] = RgbColor(80,40,0);
        px[5] = RgbColor(80,40,0);
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
      //delay(delayval); // Delay for a period of time (in milliseconds).
    }
    strip.Show(); // This sends the updated pixel color to the hardware.
}
int makePositive(int number){
  if (number < 0){
    number = -number;
  }
  return number;
}
void sendingFilePixels(int at){
  if (at > 5){
    at = 0;
  }
  for (int a = 0; a < 6; a++){
    if ( at == a){
      strip.SetPixelColor(a, RgbColor(0,40,0));
    }else{
      strip.SetPixelColor(a, RgbColor(0,0,0));
    }
  }
  strip.Show();
}

