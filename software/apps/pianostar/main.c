#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrfx_pwm.h"

#include "microbit_v2.h"
#include "pwm_speaker.h"

int main(void) {
  printf("Board started!\n");

  // initialize GPIO
  gpio_init();

  // initialize the PWM
  pwm_init();

  // compute the sine wave values
  // You should pass in COUNTERTOP-1 here as the maximum value
  compute_sine_wave((16000000 / (SAMPLING_FREQUENCY * 2)) - 1);

  while (true) {
    // Proof of concept: Alternate between 2 different octaves of C major chord

    clear_notes(); // Approach 1: Clear all notes and set individual notes
    add_note((pianostar_note_t){1046, 0.2});
    add_note((pianostar_note_t){1319, 0.2});
    add_note((pianostar_note_t){1568, 0.2});
    add_note((pianostar_note_t){1046, 0.2});
    add_note((pianostar_note_t){1319, 0.2});
    add_note((pianostar_note_t){1568, 0.2});
    play_updated_notes(); // Must call this to play updated notes
    nrf_delay_ms(200);
  }

  pwm_stop();
}
