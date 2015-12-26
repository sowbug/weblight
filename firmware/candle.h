// Copyright 2015 Mike Tsao
//
// WebLight firmware
// https://github.com/sowbug/weblight

#if !defined(__CANDLE_H__)
#define __CANDLE_H_

#include <avr/io.h>

void CandleInit();
void CandleRun(uint16_t msec_since_last);

#endif  // #if !defined(__CANDLE_H__)
