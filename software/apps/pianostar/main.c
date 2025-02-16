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

  pwm_play();

  while (true) {
    // Proof of concept: Alternate between 2 different octaves of C major chord

    clear_notes(); // Approach 1: Clear all notes and set individual notes
    pianostar_note_t *note0 = add_note((pianostar_note_t){1046, 1});
    pianostar_note_t *note1 = add_note((pianostar_note_t){1319, 1});
    pianostar_note_t *note2 = add_note((pianostar_note_t){1568, 1});
    play_updated_notes(); // Must call this to play updated notes

    nrf_delay_ms(500);

    delete_note(note0); // Approach 2: Clear individual notes
    delete_note(note1);
    delete_note(note2);
    add_note((pianostar_note_t){523, 1});
    add_note((pianostar_note_t){659, 1});
    add_note((pianostar_note_t){784, 1});
    play_updated_notes();

    nrf_delay_ms(500);
  }

  pwm_stop();
}
