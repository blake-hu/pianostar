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
    clear_notes();
    add_note((pianostar_note_t){261, 0.2});
    play_updated_notes();
    nrf_delay_ms(500);

    clear_notes();
    add_note((pianostar_note_t){330, 0.2});
    play_updated_notes();
    nrf_delay_ms(500);

    clear_notes();
    add_note((pianostar_note_t){392, 0.2});
    play_updated_notes();
    nrf_delay_ms(500);

    clear_notes();
    add_note((pianostar_note_t){261, 0.2});
    add_note((pianostar_note_t){330, 0.2});
    add_note((pianostar_note_t){392, 0.2});
    play_updated_notes();
    nrf_delay_ms(500);
  }

  pwm_stop();
}
