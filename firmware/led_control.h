#if !defined(__LED_CONTROL_H__)
#define __LED_CONTROL_H__

#include <avr/io.h>

uint8_t GetLEDCount();
void SetLED(uint8_t i, uint8_t r, uint8_t g, uint8_t b);
void SetLEDs(uint8_t r, uint8_t g, uint8_t b);
void LEDsOff();

void StatusBlink(uint8_t count);

// None of the other functions talk over the wire. This one does.
void UpdateLEDs();

typedef struct RgbColor {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} RgbColor;

typedef struct HsvColor {
  unsigned char h;
  unsigned char s;
  unsigned char v;
} HsvColor;

// Note that both of these use values ranging from 0 to 255. This is
// typical for RGB, but for HSV this means that the usual [0, 360)
// range for hue, and [0, 1] or percentage range for saturation and
// brightness all need to be scaled.
RgbColor HsvToRgb(HsvColor hsv);
HsvColor RgbToHsv(RgbColor rgb);

#endif  // #if !defined(__LED_CONTROL_H__)
