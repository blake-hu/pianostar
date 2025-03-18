// PWM to control the external led matrix
//

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
#define OUTPUT_PIN EDGE_P15

/**
 * timing constants
 */
// these are in us
#define PERIOD_LEN 1.25
#define ONE_BIT_LEN 0.8
#define ZERO_BIT_LEN 0.4
#define RESET_LEN 55

#define TICKS_PER_us 16 // ticks/us

#define PERIOD_TICKS 20 // 1.25 * 16
#define ONE_BIT_TICKS 13 | 0x8000 // 0.8 * 16
#define ZERO_BIT_TICKS 6 | 0x8000 // 0.4 * 16
#define RESET_TICKS 880 // 55 * 16

/**
 * PWM stuff
 */
// PWM instance
static const nrfx_pwm_t PWM_INST = NRFX_PWM_INSTANCE(0);

// sequence: immediately pull low
nrf_pwm_values_common_t seq_reset[1] = {0};

// sequence: FF0000 = green
nrf_pwm_values_common_t seq_one_green[24] = {
  ONE_BIT_TICKS, ONE_BIT_TICKS, ONE_BIT_TICKS, ONE_BIT_TICKS,
  ONE_BIT_TICKS, ONE_BIT_TICKS, ONE_BIT_TICKS, ONE_BIT_TICKS,
  ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS,
  ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS,
  ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS,
  ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS,
};

// sequence: 00FF00 = red
nrf_pwm_values_common_t seq_one_red[24] = {
  ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS,
  ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS,
  ONE_BIT_TICKS, ONE_BIT_TICKS, ONE_BIT_TICKS, ONE_BIT_TICKS,
  ONE_BIT_TICKS, ONE_BIT_TICKS, ONE_BIT_TICKS, ONE_BIT_TICKS,
  ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS,
  ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS,
};

// sequence: 0000FF = blue
nrf_pwm_values_common_t seq_one_blue[24] = {
  ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS,
  ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS,
  ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS,
  ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS, ZERO_BIT_TICKS,
  ONE_BIT_TICKS, ONE_BIT_TICKS, ONE_BIT_TICKS, ONE_BIT_TICKS,
  ONE_BIT_TICKS, ONE_BIT_TICKS, ONE_BIT_TICKS, ONE_BIT_TICKS,
};

static void gpio_init(void)
{
  // Initialize the GPIO
  nrf_gpio_cfg_output(OUTPUT_PIN);
  nrf_gpio_pin_clear(OUTPUT_PIN);
}

static void pwm_init(void)
{
  // Initialize the PWM
  nrfx_pwm_config_t pwm_config = {
      .output_pins = {OUTPUT_PIN, 
                      NRFX_PWM_PIN_NOT_USED, 
                      NRFX_PWM_PIN_NOT_USED, 
                      NRFX_PWM_PIN_NOT_USED},
      .base_clock = NRF_PWM_CLK_16MHz,
      .count_mode = NRF_PWM_MODE_UP,
      .top_value = PERIOD_TICKS,
      .load_mode = NRF_PWM_LOAD_COMMON,
      .step_mode = NRF_PWM_STEP_AUTO};

  nrfx_pwm_init(&PWM_INST, &pwm_config, NULL);
}

static void send_reset_sequence()
{
  // Stop the PWM (and wait until current playback is finished)
  nrfx_pwm_stop(&PWM_INST, true);

  // adjust countertop
  NRF_PWM0->COUNTERTOP = RESET_TICKS;

  // keep it low for whole period
  nrf_pwm_sequence_t reset = {
      .values.p_common = seq_reset,
      .length = 1,
      .repeats = 0,
      .end_delay = 0,
  };
  nrfx_pwm_simple_playback(&PWM_INST, &reset, 1, NRFX_PWM_FLAG_STOP);
}

static void send_led_sequence(nrf_pwm_values_common_t* seq_led, uint16_t len, uint16_t playback_count)
{
  // Stop the PWM (and wait until current playback is finished)
  nrfx_pwm_stop(&PWM_INST, true);
  
  // set countertop to one peroid
  NRF_PWM0->COUNTERTOP = PERIOD_TICKS;

  // light one led
  nrf_pwm_sequence_t one_led = {
      .values.p_common = seq_led,
      .length = len,
      .repeats = 0,
      .end_delay = 0,
  };
  nrfx_pwm_simple_playback(&PWM_INST, &one_led, playback_count, NRFX_PWM_FLAG_STOP);
}

int main(void)
{
  printf("Board started!\n");

  // init
  gpio_init();
  pwm_init();

  // reset
  send_reset_sequence();
  while (!nrfx_pwm_is_stopped(&PWM_INST))
  {
    nrf_delay_us(10);
  }

  // print what we will display
  for (int i = 0; i < 24; i++)
  {
    printf("%x\n", seq_one_red[i]);
  }
  
  // display
  send_led_sequence(seq_one_red, 24, 20);
  while (!nrfx_pwm_is_stopped(&PWM_INST))
  {
    nrf_delay_us(10);
  }
  printf("LED flushed\n");

  // Stop
  nrfx_pwm_stop(&PWM_INST, true);
}

// TODO: event handler