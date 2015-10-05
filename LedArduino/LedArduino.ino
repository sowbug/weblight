#include <DigiUSB.h>
#include <Adafruit_NeoPixel.h>

#define PIN 0
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);

int get_input() {
  int color;
  int lastRead;
  // when there are no characters to read, or the character isn't a newline
  while (true) { // loop forever
    if (DigiUSB.available()) {
      // something to read
      lastRead = DigiUSB.read();
      DigiUSB.write(lastRead);

      if (lastRead == '\n') {
        break; // when we get a newline, break out of loop
      } else {
        color = lastRead;
      }
    }
    // refresh the usb port for 10 milliseconds
    DigiUSB.delay(10);
  }
  return color;
}

void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void changeColor(int c) {
  if (c == 'r') {
    colorWipe(strip.Color(255, 0, 0), 5); // Red
  }
  if (c == 'g') {
    colorWipe(strip.Color(0, 255, 0), 5); // Green
  }
  if (c == 'b') {
    colorWipe(strip.Color(0, 0, 255), 5); // Blue
  }
  if (c == 'p') {
    colorWipe(strip.Color(219, 0, 202), 5); // Emily's purple
  }
  if (c == 'x') {
    rainbowCycle(5);
  }
  if (c == 'o') {
    colorWipe(strip.Color(0, 0, 0), 5); // Off
  }
  if (c == 'f') {
    bool cb = false;
    for (int i = 0; i < 20; i++) {
      int c_bright;
      int c_dim;
      cb = !cb;
      if (cb) {
        c_bright = strip.Color(255, 0, 0);
        c_dim = strip.Color(128, 0, 0);
      } else {
        c_bright = strip.Color(255, 0, 0);
        c_dim = strip.Color(128, 0, 128);
      }
      strip.setPixelColor(0, c_bright);
      strip.show();
      delay(5);
      strip.setPixelColor(0, c_dim);
      strip.show();
      delay(5);
      strip.setPixelColor(0, strip.Color(0, 0, 0));
      strip.show();
      delay(500);
    }
  }
}

void setup() {
  strip.begin();
  strip.show();
  DigiUSB.begin();
}

void loop() {
  // print output
  DigiUSB.println("Waiting for input...");
  // get input
  int color = get_input();
  if (color != 0) {
    changeColor(color);
  }
}

