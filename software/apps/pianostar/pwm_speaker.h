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
#define BUFFER_SIZE 24000
#define REPEATS 0
#define NUM_VOLUME_LEVELS 6

typedef struct {
  uint16_t frequency;
  float volume;
} pianostar_note_t;

void gpio_init(void);

void pwm_init(void);

void pwm_stop();

void compute_sine_wave(uint16_t max_value);

void play_updated_notes();

pianostar_note_t *add_note(pianostar_note_t new_note);

bool delete_note(pianostar_note_t *note);

bool set_note_volume(pianostar_note_t *note, float volume);

void clear_notes();

uint8_t toggle_volume(void);

uint8_t get_volume(void);

void normalize_note_volume(void);
