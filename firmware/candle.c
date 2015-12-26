// Copyright 2015 Mike Tsao
//
// WebLight firmware
// https://github.com/sowbug/weblight

#include "candle.h"

#include <stdlib.h>

#include "led_control.h"

// http://burtleburtle.net/bob/rand/smallprng.html
typedef uint32_t u4;
typedef struct ranctx { u4 a; u4 b; u4 c; u4 d; } ranctx;
ranctx rctx;

#define rot(x, k) (((x)<<(k))|((x)>>(32-(k))))
u4 ranval(ranctx *x) {
  u4 e = x->a - rot(x->b, 27);
  x->a = x->b ^ rot(x->c, 17);
  x->b = x->c + x->d;
  x->c = x->d + e;
  x->d = e + x->a;
  return x->d;
}

void raninit(ranctx *x, u4 seed) {
  u4 i;
  x->a = 0xf1ea5eed, x->b = x->c = x->d = seed;
  for (i = 0; i < 20; ++i) {
    (void)ranval(x);
  }
}

// http://stackoverflow.com/a/109025
int NumberOfSetBits(uint32_t n) {
  n = n - ((n >> 1) & 0x55555555);
  n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
  return (((n + (n >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

void CandleInit() {
  raninit(&rctx, TCNT1);
}

void CandleRun(uint16_t msec_since_last) {
    static uint8_t candle_delay = 0;
    if (candle_delay++ >= 3) {
      candle_delay = 0;
      int i;
      for (i = 0; i < GetLEDCount(); i++) {
        uint32_t bits = NumberOfSetBits(ranval(&rctx));
        bits = abs(16 - bits) / 2 + 4;
        if (bits >= 8) { bits = 8; }
        SetLED(i, 128, (1 << (8 - bits)) + 64, (1 << (8 - bits)) / 4);
      }
    }
}
