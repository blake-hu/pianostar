// LED Matrix Driver
// Displays characters on the LED matrix

#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

#include "piano_matrix.h"
#include "microbit_v2.h"

/**
 * pin/register config
 */
static uint32_t OUTPUT_PIN = 15;

static uint32_t READ_CHANNEL = 0;
static uint32_t WRITE_CHANNEL = 1;

/**
 * timing constants
 */
// these are in us
const float PERIOD_LEN = 1.25;
const float ONE_BIT_LEN = 0.8;
const float ZERO_BIT_LEN = 0.4;
const float RESET_LEN = 50;

const float TICKS_PER_us = 16; // 16 ticks/us
// const float TICKS_PER_us = 16000; // 16 ticks/us // TODO

const float f_PERIOD_TICKS = TICKS_PER_us * PERIOD_LEN;
const float f_ONE_BIT_TICKS = TICKS_PER_us * ONE_BIT_LEN;
const float f_ZERO_BIT_TICKS = TICKS_PER_us * ZERO_BIT_LEN;
const float f_RESET_TICKS = TICKS_PER_us * RESET_LEN;

#define PERIOD_TICKS (uint32_t)round(f_PERIOD_TICKS)
#define ONE_BIT_TICKS (uint32_t)round(f_ONE_BIT_TICKS)
#define ZERO_BIT_TICKS (uint32_t)round(f_ZERO_BIT_TICKS)
#define RESET_TICKS (uint32_t)round(f_RESET_TICKS)

/**
 * led matrix constants
 */
uint32_t NUM_LEDS = 10;     // TODO
uint32_t BITS_PER_LED = 24; // TODO
uint32_t *buffer;
volatile uint32_t current_bit = 0;
volatile uint32_t current_led = 0;
volatile bool writing = false;

/**
 * forward declarations
 */
void handle_period_end(void);
void handle_low_end(void);
void drive_high(void);
void drive_low(void);

/**
 * timer stuff
 */
void timer_init(void)
{
  // prescaler to 0
  NRF_TIMER3->PRESCALER = 0;
  NRF_TIMER4->PRESCALER = 0;

  // use big timers
  NRF_TIMER3->BITMODE = 3;
  NRF_TIMER4->BITMODE = 3;

  // timer mode
  NRF_TIMER3->MODE = 0;
  NRF_TIMER4->MODE = 0;

  // enable interrupts
  NRF_TIMER3->INTENSET = 1 << 17; // COMPARE[1]
  NRF_TIMER4->INTENSET = 1 << 17; // COMPARE[1]
  NVIC_EnableIRQ(TIMER3_IRQn);
  NVIC_EnableIRQ(TIMER4_IRQn);

  // clear timers
  NRF_TIMER3->TASKS_CLEAR = 1;
  NRF_TIMER4->TASKS_CLEAR = 1;

  // printf("Timers initialized\n");
}

uint32_t read_timer_3(void)
{
  // return value of internal counter for TIMER4
  NRF_TIMER3->TASKS_CAPTURE[READ_CHANNEL] = 1;
  return NRF_TIMER3->CC[READ_CHANNEL];
}
uint32_t read_timer_4(void)
{
  // return value of internal counter for TIMER4
  NRF_TIMER4->TASKS_CAPTURE[READ_CHANNEL] = 1;
  return NRF_TIMER4->CC[READ_CHANNEL];
}

void timer_3_start(uint32_t len)
{
  // printf("len: %ld\n", len);
  NRF_TIMER3->CC[WRITE_CHANNEL] = read_timer_3() + len;
  NRF_TIMER3->TASKS_START = 1;
  // printf("Timer 3 started\n");
}
void timer_4_start(uint32_t len)
{
  NRF_TIMER4->CC[WRITE_CHANNEL] = read_timer_4() + len;
  NRF_TIMER4->TASKS_START = 1;
  // printf("Timer 4 started\n");
}

void timer_3_stop(void)
{
  NRF_TIMER3->TASKS_STOP = 1;
  NRF_TIMER3->TASKS_CLEAR = 1;
}
void timer_4_stop(void)
{
  NRF_TIMER4->TASKS_STOP = 1;
  NRF_TIMER4->TASKS_CLEAR = 1;
}

// period timer
void TIMER3_IRQHandler(void)
{
  // This should always be the first line of the interrupt handler!
  // It clears the event so that it doesn't happen again
  NRF_TIMER3->EVENTS_COMPARE[WRITE_CHANNEL] = 0;

  printf("PERIOD FIRED\n");

  handle_period_end();
}

// low timer
void TIMER4_IRQHandler(void)
{
  // This should always be the first line of the interrupt handler!
  // It clears the event so that it doesn't happen again
  NRF_TIMER4->EVENTS_COMPARE[WRITE_CHANNEL] = 0;

  printf("LOW FIRED\n");

  handle_low_end();
}

/**
 * LED matrix stuff
 */
void led_matrix_init(void)
{
  // set gpio pin to output
  nrf_gpio_pin_dir_set(OUTPUT_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_clear(OUTPUT_PIN);
}

void drive_high(void)
{
  nrf_gpio_pin_write(OUTPUT_PIN, 1);
}
void drive_low(void)
{
  nrf_gpio_pin_write(OUTPUT_PIN, 0);
}

void handle_period_end(void)
{
  // there is another bit left to send
  if (current_bit < BITS_PER_LED || current_led < NUM_LEDS)
  {
    // set gpio pin high
    drive_high();

    // get next bit
    uint8_t next_bit = buffer[current_led] >> current_bit & 1;

    // start low timer for next bit
    if (next_bit)
    {
      timer_4_start(ONE_BIT_TICKS);
    }
    else
    {
      timer_4_start(ZERO_BIT_TICKS);
    }

    // start high timer for next bit
    timer_3_start(PERIOD_TICKS);

    // update current bit and/or current led
    if (current_bit < BITS_PER_LED)
    {
      current_bit++;
    }
    else
    {
      current_bit = 0;
      current_led++;
    }
  }
  else
  {
    drive_low();
    // stop timers
    timer_3_stop();
    timer_4_stop();
    writing = false;
  }
}

void handle_low_end(void)
{
  drive_low();
}

void display_buffer(uint32_t *buf)
{
  writing = true;
  drive_low();
  // set buffer
  buffer = buf;
  // print first 5 items in buffer
  // for (int i = 0; i < 5; i++)
  // {
  //   printf("Buffer[%d]: %lx\n", i, buffer[i]);
  // }
  // reset everything
  current_bit = 0;
  current_led = 0;
  // start timer
  timer_3_start(RESET_TICKS);
  // wait until done writing
  while (writing)
  {
    nrf_delay_ms(1);
  }
}
