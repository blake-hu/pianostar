// PWM Speaker
//
// Play multiple notes at the same time

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrfx_pwm.h"

#include "microbit_v2.h"

#define PIANOSTAR_MAX_NOTES 8
#define SINE_BUFFER_SIZE 5000
#define SAMPLING_FREQUENCY 32000 // 16 kHz sampling rate
#define BUFFER_SIZE 32000        // one second worth of data
#define REPEATS 0

typedef struct {
  uint16_t frequency;
  float volume;
} pianostar_note_t;

void gpio_init(void);

void pwm_init(void);

void pwm_play();

void pwm_stop();

void compute_sine_wave(uint16_t max_value);

void play_updated_notes();

pianostar_note_t *add_note(pianostar_note_t new_note);

bool delete_note(pianostar_note_t *note);

bool set_note_volume(pianostar_note_t *note, float volume);

void clear_notes();
