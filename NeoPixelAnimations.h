#include <NeoPixelBus.h>

NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(6, D4);

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
