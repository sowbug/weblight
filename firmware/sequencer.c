// Copyright 2015 Mike Tsao
//
// weblight

#include "sequencer.h"

#include "eeprom.h"
#include "led_control.h"

#define TRUE (1==1)
#define FALSE (!TRUE)

#define PROGRAM_SIZE_MAX (48)
uint8_t opcodes[PROGRAM_SIZE_MAX];
uint8_t oi = 0;
uint8_t opcode_count = 0;
uint8_t is_playing = FALSE;
uint8_t is_recording = FALSE;
Transition current_transition = NONE;
uint16_t current_transition_duration_msec = 0;
uint16_t pause_duration_msec = 0;

uint8_t IsRecording() { return is_recording; }

static void VerifySequenceCapacity(uint8_t opcodes) {
  if (opcode_count + opcodes >= PROGRAM_SIZE_MAX) {
    // This will wake up the watchdog.
    StatusBlink(255);
  }
}

void HandleCOLOR(uint8_t r, uint8_t g, uint8_t b) {
  if (is_recording) {
    VerifySequenceCapacity(4);
    opcodes[opcode_count++] = COLOR;
    opcodes[opcode_count++] = r;
    opcodes[opcode_count++] = g;
    opcodes[opcode_count++] = b;
  } else {
    SetLEDs(r, g, b);
  }
}

void HandleTRANSITION(Transition t, uint16_t duration_msec) {
  if (is_recording) {
    VerifySequenceCapacity(3);
    opcodes[opcode_count++] = TRANSITION;
    opcodes[opcode_count++] = duration_msec >> 8;
    opcodes[opcode_count++] = duration_msec & 0xff;
  } else {
    current_transition = t;
    current_transition_duration_msec = duration_msec;
  }
}

void HandlePAUSE(uint16_t duration_msec) {
  if (is_recording) {
    VerifySequenceCapacity(3);
    opcodes[opcode_count++] = PAUSE;
    opcodes[opcode_count++] = duration_msec >> 8;
    opcodes[opcode_count++] = duration_msec & 0xff;
  } else {
    pause_duration_msec = duration_msec;
  }
}

void HandleHALT() {
  if (is_recording) {
    VerifySequenceCapacity(1);
    opcodes[opcode_count++] = HALT;
  }
}

void Record() {
  Stop();
  opcode_count = 0;
  is_recording = TRUE;
}

void Play() {
  Stop();
  is_playing = TRUE;
}

void Stop() {
  oi = 0;
  is_playing = FALSE;
  is_recording = FALSE;
}

static void AdvanceSequencePointer(uint8_t opcodes) {
  oi += opcodes;
  if (oi >= opcode_count) {
      oi = 0;
  }
}

void Run(uint16_t msec_since_last) {
  if (is_recording) {
    return;
  }
  if (!is_playing) {
    return;
  }
  if (opcode_count == 0) {
    return;
  }

  // Are we in the middle of a pause?
  if (pause_duration_msec) {
    if (pause_duration_msec > msec_since_last) {
      pause_duration_msec -= msec_since_last;
    } else {
      pause_duration_msec = 0;
    }
    if (pause_duration_msec) {
      // Still happening
      return;
    }
    // Done. Handle next command.
  }

  // Are we in the middle of a transition?
  if (current_transition_duration_msec) {
    if (current_transition_duration_msec > msec_since_last) {
      current_transition_duration_msec -= msec_since_last;
    } else {
      current_transition_duration_msec = 0;
    }
    if (current_transition_duration_msec) {
      // Still happening
      return;
    }
    // Done. Handle next command.
  }

  switch (opcodes[oi]) {
  case COLOR:
    HandleCOLOR(opcodes[oi + 1], opcodes[oi + 2], opcodes[oi + 3]);
    AdvanceSequencePointer(4);
    break;
  case TRANSITION:
    HandleTRANSITION(opcodes[oi + 1], (opcodes[oi + 2] << 8) | opcodes[oi + 3]);
    AdvanceSequencePointer(4);
    break;
  case PAUSE:
    HandlePAUSE((opcodes[oi + 1] << 8) | opcodes[oi + 2]);
    AdvanceSequencePointer(3);
    break;
  case HALT:
    HandleHALT();
    // Don't advance. Just spin.
    break;
  }
}

void Save() {
  WriteLightProgram(opcodes, opcode_count);
}

void Load() {
  opcode_count = ReadLightProgram(opcodes, PROGRAM_SIZE_MAX);
}
