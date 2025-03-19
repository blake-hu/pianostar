#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app_timer.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrfx_pwm.h"
#include "nrfx_timer.h"

#include "capacitive_touch.h"
#include "i2c_adc.h"
#include "led_matrix.h"
#include "microbit_v2.h"
#include "notes.h"
#include "nrfx_gpiote.h"
#include "pwm_speaker.h"
#include "saadc_fsr.h"

#define NUM_OCTAVE_SETS 2

const float V_TO_VOL_SCALE = 1.0 / 3.6; // voltage to volume
const float PLAY_THRESHOLD = 1.0;

const char volume_levels[NUM_VOLUME_LEVELS] = {'0', '1', '2', '3', '4', 'S'};

uint8_t active_octave_set = 0;

const uint16_t ADC_KEYMAP[NUM_OCTAVE_SETS][NUM_ADC][NUM_ADC_CHANNELS] = {
    {
        {B5, As5, A5, Gs5, G5, Fs5, F5, NO_NOTE},
        {E5, Ds5, D5, Cs5, C5, NO_NOTE, NO_NOTE, NO_NOTE},
        {B4, As4, A4, Gs4, G4, Fs4, F4, NO_NOTE},
        {E4, Ds4, D4, Cs4, C4, NO_NOTE, NO_NOTE, NO_NOTE},
    },
    {
        {B7, As7, A7, Gs7, G7, Fs7, F7, NO_NOTE},
        {E7, Ds7, D7, Cs7, C7, NO_NOTE, NO_NOTE, NO_NOTE},
        {B6, As6, A6, Gs6, G6, Fs6, F6, NO_NOTE},
        {E6, Ds6, D6, Cs6, C6, NO_NOTE, NO_NOTE, NO_NOTE},
    },
};

void on_touch(void) {
  uint8_t volume = toggle_volume();
  display_char(volume_levels[volume]);
}

void gpio_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
  if (pin == BTN_B) {
    active_octave_set = (active_octave_set + 1) % NUM_OCTAVE_SETS;
  }
}

int main(void) {
  printf("Board started!\n");

  // capacitive touch
  nrfx_gpiote_init();
  app_timer_init();
  capacitive_touch_init(on_touch);

  // for led matrix display
  led_matrix_init();
  display_char(volume_levels[get_volume()]);

  // for FSRs
  i2c_init();

  // for PWM speaker
  gpio_init();
  pwm_init();
  compute_sine_wave((16000000 / (SAMPLING_FREQUENCY * 2)) - 1);

  // for switching octaves
  nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
  nrfx_gpiote_in_init(BTN_B, &in_config, gpio_handler);
  nrfx_gpiote_in_event_enable(BTN_B, true);

  while (1) {
    clear_notes();

    for (int adc = 0; adc < NUM_ADC; adc++) {
      for (int chan = 0; chan < NUM_ADC_CHANNELS; chan++) {
        uint16_t freq = ADC_KEYMAP[active_octave_set][adc][chan];
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
