// Copyright 2015 Mike Tsao
//
// WebLight firmware
// https://github.com/sowbug/weblight

#if !defined(__SEQUENCER_H__)
#define __SEQUENCER_H_

#include <avr/io.h>

typedef enum {
  NONE = 0,
  LINEAR_RGB
} Transition;

typedef enum {
  COLOR = 0,
  TRANSITION = 1,
  PAUSE = 2,
  HALT = 3,
  SELECT = 4
} Command;

void HandleCOLOR(uint8_t r, uint8_t g, uint8_t b, uint16_t selection);
void HandleTRANSITION(Transition t, uint16_t duration_msec);
void HandlePAUSE(uint16_t duration_msec);
void HandleSELECT(uint16_t led_mask);
void HandleHALT();

uint8_t IsRecording();
void Record();
void Play();
void Stop();
void Save();
void Load();

void Run(uint16_t msec_since_last);

#endif // #if !defined(__SEQUENCER_H__)
