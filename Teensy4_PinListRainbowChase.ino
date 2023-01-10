/*  OctoWS2811 Teensy4_PinList.ino - Demonstrate use of any pins
    http://www.pjrc.com/teensy/td_libs_OctoWS2811.html

  With Teensy 4.x, you can use any group of pins.  You are not limited
  to only 8 fixed pins as with Teensy 3.x.  This example shows how to
  use only 4 pins, which are the 4 pins of the Octo28 shield which do
  not conflict with the Audio shield.

  Required Connections
  --------------------
    pin 2:  LED Strip #1
    pin 14: LED strip #2
    pin 6:  LED strip #5
    pin 5:  LED strip #8
*/

#include <OctoWS2811.h>

// Any group of digital pins may be used
const int numPins = 4;
// byte pinList[numPins] = {2, 14, 6, 5};
byte pinList[numPins] = {0, 1, 22, 23};

const int ledsPerStrip = 250;


// These buffers need to be large enough for all the pixels.
// The total number of pixels is "ledsPerStrip * numPins".
// Each pixel needs 3 bytes, so multiply by 3.  An "int" is
// 4 bytes, so divide by 4.  The array is created using "int"
// so the compiler will align it to 32 bit memory.
const int bytesPerLED = 3;  // change to 4 if using RGBW
DMAMEM int displayMemory[ledsPerStrip * numPins * bytesPerLED / 4];
int drawingMemory[ledsPerStrip * numPins * bytesPerLED / 4];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config, numPins, pinList);

int rainbowColors[180];
int BRIGHTNESS = 25; // 0-100

void setup() {
  for (int i=0; i<180; i++) {
    int hue = i * 2;
    int saturation = 100;
    int lightness = BRIGHTNESS;
    // pre-compute the 180 rainbow colors
    rainbowColors[i] = makeColor(hue, saturation, lightness);
  }
  leds.begin();
  leds.show();
}

void loop() {
  rainbow(10, 2500);
}

// phaseShift is the shift between each row.  phaseShift=0
// causes all rows to show the same colors moving together.
// phaseShift=180 causes each row to be the opposite colors
// as the previous.
//
// cycleTime is the number of milliseconds to shift through
// the entire 360 degrees of the color wheel:
// Red -> Orange -> Yellow -> Green -> Blue -> Violet -> Red
//
void rainbow(int phaseShift, int cycleTime)
{
  int color, x, wait;

  wait = cycleTime * 1000 / ledsPerStrip;
  for (color=0; color < 180; color++) {
    for (x=0; x < leds.numPixels(); x++) {
      int index = (color + x) % 180;
      leds.setPixel(x, rainbowColors[index]);
    }
    leds.show();
    delayMicroseconds(wait);
  }
}



// Convert HSL (Hue, Saturation, Lightness) to RGB (Red, Green, Blue)
//
//   hue:        0 to 359 - position on the color wheel, 0=red, 60=orange,
//                            120=yellow, 180=green, 240=blue, 300=violet
//
//   saturation: 0 to 100 - how bright or dull the color, 100=full, 0=gray
//
//   lightness:  0 to 100 - how light the color is, 100=white, 50=color, 0=black
//
int makeColor(unsigned int hue, unsigned int saturation, unsigned int lightness)
{
  unsigned int red, green, blue;
  unsigned int var1, var2;

  if (hue > 359) hue = hue % 360;
  if (saturation > 100) saturation = 100;
  if (lightness > 100) lightness = 100;

  // algorithm from: http://www.easyrgb.com/index.php?X=MATH&H=19#text19
  if (saturation == 0) {
    red = green = blue = lightness * 255 / 100;
  } else {
    if (lightness < 50) {
      var2 = lightness * (100 + saturation);
    } else {
      var2 = ((lightness + saturation) * 100) - (saturation * lightness);
    }
    var1 = lightness * 200 - var2;
    red = h2rgb(var1, var2, (hue < 240) ? hue + 120 : hue - 240) * 255 / 600000;
    green = h2rgb(var1, var2, hue) * 255 / 600000;
    blue = h2rgb(var1, var2, (hue >= 120) ? hue - 120 : hue + 240) * 255 / 600000;
  }
  return (red << 16) | (green << 8) | blue;
}

unsigned int h2rgb(unsigned int v1, unsigned int v2, unsigned int hue)
{
  if (hue < 60) return v1 * 60 + (v2 - v1) * hue;
  if (hue < 180) return v2 * 60;
  if (hue < 240) return v1 * 60 + (v2 - v1) * (240 - hue);
  return v1 * 60;
}

// alternate code:
// http://forum.pjrc.com/threads/16469-looking-for-ideas-on-generating-RGB-colors-from-accelerometer-gyroscope?p=37170&viewfull=1#post37170
