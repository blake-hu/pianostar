// Capacitive touch library
// Detects touch status of the logo touchpad

#include <stdint.h>
#include <stdio.h>

#include "app_timer.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "nrfx_timer.h"

#include "microbit_v2.h"

#include "capacitive_touch.h"

APP_TIMER_DEF(my_timer);

static nrfx_timer_t TOUCH_TIMER = NRFX_TIMER_INSTANCE(0);

// function we want to call when detect a touch
void (*call_on_touch)(void);

// whether it is being touched rn
static bool touch_active = false;

// Callback function for GPIO interrupts
static void gpio_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
  disable_interrupts();
  touch_active = false;
}

// this function is called if someone is touching it
static void timer_handler(nrf_timer_event_t event, void *context)
{
  disable_interrupts();
  if (!touch_active)
  {
    call_on_touch();
  }
  touch_active = true;
}

// Helper function for starting a test
// A "capacitive test" consists of:
//  1. Driving the pad low
//  2. Changing to an input and allowing it to float high
//  3. Either triggering a GPIO interrupt when it becomes high or timing out
static void start_capacitive_test(void *_unused)
{
  // set pin as input and clear it
  nrf_gpio_cfg(TOUCH_LOGO, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
               NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
  nrf_gpio_pin_clear(TOUCH_LOGO);

  // clear and resume timeout timer
  nrfx_timer_clear(&TOUCH_TIMER);
  nrfx_timer_resume(&TOUCH_TIMER);

  // re-initialize and enable GPIO interrupt event
  nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(true); // high-accuracy mode
  nrfx_gpiote_in_init(TOUCH_LOGO, &in_config, gpio_handler);
  nrfx_gpiote_in_event_enable(TOUCH_LOGO, true); // enable interrupts
};

// Starts continuously measuring capacitive touch for the logo
// Function returns immediately without blocking
void capacitive_touch_init(void (*on_touch)(void))
{
  // configure high-speed timer
  // timer should be 1 MHz and 32-bit
  nrfx_timer_config_t timer_config = {
      .frequency = NRF_TIMER_FREQ_1MHz,
      .mode = NRF_TIMER_MODE_TIMER,
      .bit_width = NRF_TIMER_BIT_WIDTH_32,
      .interrupt_priority = 4,
      .p_context = NULL
  };
  nrfx_timer_init(&TOUCH_TIMER, &timer_config, timer_handler);
  call_on_touch = on_touch;

  // enable, but pause the timer
  nrfx_timer_enable(&TOUCH_TIMER);
  nrfx_timer_pause(&TOUCH_TIMER);

  // interrupts for stuff
  nrfx_timer_compare(&TOUCH_TIMER, NRF_TIMER_CC_CHANNEL1, 500, true);

  // init app timer
  app_timer_init();
  app_timer_create(&my_timer, APP_TIMER_MODE_REPEATED, start_capacitive_test);
  app_timer_start(my_timer, 6000, NULL);
};

void disable_interrupts(void)
{
  nrfx_gpiote_in_event_disable(TOUCH_LOGO);
  nrfx_timer_pause(&TOUCH_TIMER);
};
