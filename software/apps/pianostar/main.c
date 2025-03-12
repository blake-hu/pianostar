#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrfx_pwm.h"

#include "i2c_adc.h"
#include "microbit_v2.h"
#include "notes.h"
#include "pwm_speaker.h"
#include "saadc_fsr.h"

const float V_TO_VOL_SCALE = 1.0 / 3.6;
const float PLAY_THRESHOLD = 1.0;

const uint16_t ADC_KEYMAP[NUM_ADC][NUM_ADC_CHANNELS] = {
    {B5, As5, A5, Gs5, G5, Fs5, F5, NO_NOTE},
    {E5, Ds5, D5, Cs5, C5, NO_NOTE, NO_NOTE, NO_NOTE},
    {B4, As4, A4, Gs4, G4, Fs4, F4, NO_NOTE},
    {E4, Ds4, D4, Cs4, C4, NO_NOTE, NO_NOTE, NO_NOTE},
};

int main(void) {
  printf("Board started!\n");

  // init ADC
  adc_init();

  i2c_init();

  // initialize GPIO
  gpio_init();

  // initialize the PWM
  pwm_init();

  // compute the sine wave values
  // You should pass in COUNTERTOP-1 here as the maximum value
  compute_sine_wave((16000000 / (SAMPLING_FREQUENCY * 2)) - 1);

  while (1) {

    clear_notes();

    for (int adc = 0; adc < NUM_ADC; adc++) {
      for (int chan = 0; chan < NUM_ADC_CHANNELS; chan++) {
        uint16_t freq = ADC_KEYMAP[adc][chan];
        if (freq == NO_NOTE) {
          break;
        }
        uint8_t value = i2c_adc_read(adc, chan);
        float voltage = ((float)value / 255) * 3.6;
        if (voltage > PLAY_THRESHOLD) {
          pianostar_note_t note = {freq, voltage * V_TO_VOL_SCALE};
          add_note(note);
          printf("%u\n", freq);
        }
      }
    }

    play_updated_notes();
    nrf_delay_ms(50);
  }

  pwm_stop();
}
