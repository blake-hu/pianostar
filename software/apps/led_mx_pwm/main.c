// PWM Square wave tone app
//
// Use PWM to play a tone over the speaker using a square wave

#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrfx_pwm.h"

#include "microbit_v2.h"

/**
 * output pin
 */
#define OUTPUT_PIN 15

/**
 * timing constants
 */
// these are in us
const float PERIOD_LEN = 1.25;
const float ONE_BIT_LEN = 0.8;
const float ZERO_BIT_LEN = 0.4;
const float RESET_LEN = 55;

const float TICKS_PER_us = 16; // ticks/us

#define PERIOD_TICKS (uint32_t)round(TICKS_PER_us * PERIOD_LEN)
#define ONE_BIT_TICKS (uint32_t)round(TICKS_PER_us * ONE_BIT_LEN)
#define ZERO_BIT_TICKS (uint32_t)round(TICKS_PER_us * ZERO_BIT_LEN)
#define RESET_TICKS (uint32_t)round(TICKS_PER_us * RESET_LEN)


// PWM configuration
static const nrfx_pwm_t PWM_INST = NRFX_PWM_INSTANCE(0);

// Holds duty cycle values to trigger PWM toggle
nrf_pwm_values_common_t sequence_data[1] = {0};

// Sequence structure for configuring DMA
nrf_pwm_sequence_t pwm_sequence = {
    .values.p_common = sequence_data,
    .length = 1,
    .repeats = 0,
    .end_delay = 0,
};

static void pwm_init(void)
{
  // Initialize the PWM
  // SPEAKER_OUT is the output pin, mark the others as NRFX_PWM_PIN_NOT_USED
  // Set the clock to 500 kHz, count mode to Up, and load mode to Common
  // The Countertop value doesn't matter for now. We'll set it in play_tone()
  nrfx_pwm_config_t pwm_config = {
      .output_pins = {OUTPUT_PIN, NRFX_PWM_PIN_NOT_USED, NRFX_PWM_PIN_NOT_USED, NRFX_PWM_PIN_NOT_USED},
      .base_clock = NRF_PWM_CLK_16MHz,
      .count_mode = NRF_PWM_MODE_UP,
      .top_value = 100, // anything for now
      .load_mode = NRF_PWM_LOAD_COMMON,
      .step_mode = NRF_PWM_STEP_AUTO};

  nrfx_pwm_init(&PWM_INST, &pwm_config, NULL);
}

static void play_tone(uint16_t frequency)
{
  // Stop the PWM (and wait until its finished)
  nrfx_pwm_stop(&PWM_INST, true);

  // Set a countertop value based on desired tone frequency
  // You can access it as NRF_PWM0->COUNTERTOP
  NRF_PWM0->COUNTERTOP = 500000 / frequency;

  // Modify the sequence data to be a 25% duty cycle
  sequence_data[0] = 0.5 * 500000 / frequency;

  // Start playback of the samples and loop indefinitely
  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, frequency, NRFX_PWM_FLAG_STOP);
}

int main(void)
{
  printf("Board started!\n");

  // initialize PWM
  pwm_init();

  // Play the A4 tone for one second
  play_tone(440);
  nrf_delay_ms(1000);

  // Play the C#5 tone for one second
  play_tone(554);
  nrf_delay_ms(1000);

  // Play the E5 tone for one second
  play_tone(659);
  nrf_delay_ms(1000);

  // Play the A5 tone for one second
  play_tone(880);
  nrf_delay_ms(1000);

  // Stop all noises
  nrfx_pwm_stop(&PWM_INST, true);
}