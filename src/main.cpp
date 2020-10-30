#include <Arduino.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN 2
#define NUMPIXELS 2

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


const int MODE_RAINBOW = 1;
const int MODE_SINGLE_COLOR = 2;
const int MODE_FLASH = 3;

const long MODE_LENGTH[]={300, 60000, 1000};

int mode = 2;
long modeLength = 0;
long lastModeChange = 0;

int step = 0;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  //while (!Serial);
  Serial.println("Started");

  pixels.begin();
}


// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait)
{
    // Hue of first pixel runs 5 complete loops through the color wheel.
    // Color wheel has a range of 65536 but it's OK if we roll over, so
    // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
    // means we'll make 5*65536/256 = 1280 passes through this outer loop:
    for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256)
    {
        for (int i = 0; i < pixels.numPixels(); i++)
        { // For each pixel in strip...
            // Offset pixel hue by an amount to make one full revolution of the
            // color wheel (range of 65536) along the length of the strip
            // (strip.numPixels() steps):
            int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
            // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
            // optionally add saturation and value (brightness) (each 0 to 255).
            // Here we're using just the single-argument hue variant. The result
            // is passed through strip.gamma32() to provide 'truer' colors
            // before assigning to each pixel:
            pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
        }
        pixels.show(); // Update strip with new contents
        delay(wait);  // Pause for a moment
    }
}

void flash(int wait) {
  Serial.printf("step %d \n",step);
  if(step == 0) {
    pixels.clear();
    step = 1;
  }
  else {
    pixels.fill(pixels.Color(255, 255, 255));   
    step = 0;
  }
  pixels.show();  
  delay(wait);
  
}
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void singleColor() {
  int randomColor = pixels.Color(random(32,254), random(32,254), random(32,254));
  Serial.printf("random color");
  pixels.clear();
  pixels.fill(randomColor);
  pixels.show();
  delay(5000);
}


void loop() {
   long now = millis();
  switch (mode) {
    case MODE_RAINBOW:
      rainbow(random(8,20));      
    break;
    case MODE_SINGLE_COLOR:      
      singleColor();
    break;
    case MODE_FLASH:
      flash(300); 
    break;
  }
  if(now > lastModeChange + modeLength) {
    int lastMode = mode;
    if(lastMode != 1)
      mode = 1;
    else {
      while(lastMode == mode)
        mode = random(1, 4);
    }
      
    lastModeChange = millis();
    modeLength = MODE_LENGTH[mode-1];
    step=0;
    Serial.printf("mode changed to %d modeLength %d\n" ,mode, modeLength);
  }

}
