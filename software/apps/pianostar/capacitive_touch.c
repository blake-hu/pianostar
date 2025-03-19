// Capacitive touch library
// Detects touch status of the logo touchpad

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app_timer.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "nrfx_timer.h"

#include "microbit_v2.h"

#include "capacitive_touch.h"

#define CC_RISE_TIME 0
#define CC_TIMEOUT 1
#define RISE_TIME_THRESHOLD 100   // microseconds
#define CAP_TOUCH_LOOP_PERIOD 100 // ticks (32768 ticks per second)

// function to call when we register a touch
void (*touch_callback)(void);

// how many times we detect a touch before we register it
uint32_t touch_count = 0;
#define TOUCH_COUNT_THRESHOLD 40

// forward declarations
static void start_capacitive_test(void);

// High-speed timer for timeout detection
static nrfx_timer_t TIMER_RISE = NRFX_TIMER_INSTANCE(0);
APP_TIMER_DEF(CAP_TOUCH_LOOP_TIMER);

static void disable_all_interrupts() {
  nrfx_gpiote_in_event_disable(TOUCH_LOGO);
  nrfx_timer_pause(&TIMER_RISE);
}

// Callback function for GPIO interrupts
// touch inactive
static void gpio_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
  // Disable the GPIO interrupt event so you don't get multiple spurious events
  disable_all_interrupts();

  // uint32_t time = nrfx_timer_capture(&TIMER_RISE, NRF_TIMER_CC_CHANNEL0);

  touch_count = 0;
}

// touch active
static void timer_handler(nrf_timer_event_t event, void *context) {
  disable_all_interrupts();
  
  // uint32_t time = nrfx_timer_capture(&TIMER_RISE, NRF_TIMER_CC_CHANNEL0);

  if (touch_count < TOUCH_COUNT_THRESHOLD) {
    touch_count++;
  }
  else if (touch_count == TOUCH_COUNT_THRESHOLD) {
    touch_callback(); // only call once
    touch_count++;
  }
  // do not increment past TOUCH_COUNT_THRESHOLD + 1
}

void app_timer_handler(void *_unused) { start_capacitive_test(); }

// Helper function for starting a test
// A "capacitive test" consists of:
//  1. Driving the pad low
//  2. Changing to an input and allowing it to float high
//  3. Either triggering a GPIO interrupt when it becomes high or timing out
static void start_capacitive_test(void) {
  // set pin as input and clear it
  nrf_gpio_cfg(TOUCH_LOGO, NRF_GPIO_PIN_DIR_OUTPUT,
               NRF_GPIO_PIN_INPUT_DISCONNECT, NRF_GPIO_PIN_NOPULL,
               NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
  nrf_gpio_pin_clear(TOUCH_LOGO);

  // clear and resume timeout timer
  nrfx_timer_clear(&TIMER_RISE);
  nrfx_timer_resume(&TIMER_RISE);

  // re-initialize and enable GPIO interrupt event
  nrfx_gpiote_in_config_t in_config =
      NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(true); // high-accuracy mode
  nrfx_gpiote_in_init(TOUCH_LOGO, &in_config, gpio_handler);
  nrfx_gpiote_in_event_enable(TOUCH_LOGO, true); // enable interrupts
}

void start_cap_touch_timer_loop(void) {
  app_timer_init();
  app_timer_create(&CAP_TOUCH_LOOP_TIMER, APP_TIMER_MODE_REPEATED,
                   app_timer_handler);
  app_timer_start(CAP_TOUCH_LOOP_TIMER, CAP_TOUCH_LOOP_PERIOD, NULL);
}

// Starts continuously measuring capacitive touch for the logo
// Function returns immediately without blocking
void capacitive_touch_init(void (*on_touch)(void)) {
  touch_callback = on_touch;
  
  // configure high-speed timer
  // timer should be 1 MHz and 32-bit
  nrfx_timer_config_t timer_config = {
      .frequency = NRF_TIMER_FREQ_1MHz,
      .mode = NRF_TIMER_MODE_TIMER,
      .bit_width = NRF_TIMER_BIT_WIDTH_32,
      .interrupt_priority = 4,
      .p_context = NULL,
  };
  nrfx_timer_init(&TIMER_RISE, &timer_config, timer_handler);
  nrfx_timer_compare(&TIMER_RISE, CC_TIMEOUT, RISE_TIME_THRESHOLD, true);

  // enable, but pause the timer
  nrfx_timer_enable(&TIMER_RISE);
  nrfx_timer_pause(&TIMER_RISE);

  // start the touch test
  start_cap_touch_timer_loop();
}

