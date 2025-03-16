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
#define OUTPUT_PIN 15

#define READ_CHANNEL 0
#define WRITE_CHANNEL 1

/**
 * timing constants
 */
// these are in us
const float PERIOD_LEN = 1.25;
const float ONE_BIT_LEN = 0.8;
const float ZERO_BIT_LEN = 0.4;
const float RESET_LEN = 55;

const float TICKS_PER_us = 16; // ticks/us
// const float TICKS_PER_us = 16000; // ticks/ms
// const float TICKS_PER_us = 16000000; // 16 ticks/s

#define PERIOD_TICKS (uint32_t)round(TICKS_PER_us * PERIOD_LEN)
#define ONE_BIT_TICKS (uint32_t)round(TICKS_PER_us * ONE_BIT_LEN)
#define ZERO_BIT_TICKS (uint32_t)round(TICKS_PER_us * ZERO_BIT_LEN)
#define RESET_TICKS (uint32_t)round(TICKS_PER_us * RESET_LEN)

/**
 * buffer
 */
#define BUFFER_SIZE 48
uint32_t interrupts_at[BUFFER_SIZE] = {
  880, 893, // 1
  900, 913, // 1
  920, 933, // 1
  940, 953, // 1
  960, 973, // 1
  980, 993, // 1
  1000, 1013, // 1
  1020, 1033, // 1
  1040, 1046, // 0
  1060, 1066, // 0
  1080, 1086, // 0
  1100, 1106, // 0
  1120, 1126, // 0
  1140, 1146, // 0
  1160, 1166, // 0
  1180, 1186, // 0
  1200, 1206, // 0
  1220, 1226, // 0
  1240, 1246, // 0
  1260, 1266, // 0
  1280, 1286, // 0
  1300, 1306, // 0
  1320, 1326, // 0
  1340, 1346, // 0
};
static uint32_t current_idx = 0;
static bool pin_state = 0;
static bool writing = false;

/**
 * debugging
 */
#define DEBUG_BUF_SIZE 200
uint32_t print_buf[DEBUG_BUF_SIZE];
uint32_t print_idx = 0;


/**
 * forward declarations
 */
void drive_low(void);
void drive_high(void);

/**
 * timer utils
 */
void timer_init(void)
{
  // prescaler to 0
  NRF_TIMER3->PRESCALER = 0;

  // use big timers
  NRF_TIMER3->BITMODE = 3;

  // timer mode
  NRF_TIMER3->MODE = 0;

  // enable interrupts
  NRF_TIMER3->INTENSET = 1 << 17; // COMPARE[1]
  NVIC_EnableIRQ(TIMER3_IRQn);

  // clear timers
  NRF_TIMER3->TASKS_CLEAR = 1;
}

uint32_t read_timer(void)
{
  // return value of internal counter for TIMER3
  NRF_TIMER3->TASKS_CAPTURE[READ_CHANNEL] = 1;
  return NRF_TIMER3->CC[READ_CHANNEL];
}

void start_next_timer(void)
{
  print_buf[print_idx++] = read_timer();
  print_buf[print_idx++] = interrupts_at[current_idx];
  // printf("current timer: %ld\n", read_timer());
  // printf("setting timer for: %ld\n", interrupts_at[current_idx]);
  NRF_TIMER3->CC[WRITE_CHANNEL] = interrupts_at[current_idx];
  NRF_TIMER3->TASKS_START = 1;
}

void stop_timer(void)
{
  NRF_TIMER3->TASKS_STOP = 1;
  NRF_TIMER3->TASKS_CLEAR = 1;
}

void TIMER3_IRQHandler(void)
{
  // This should always be the first line of the interrupt handler!
  // It clears the event so that it doesn't happen again
  NRF_TIMER3->EVENTS_COMPARE[WRITE_CHANNEL] = 0;

  print_buf[print_idx++] = read_timer();
  // printf("Fired at %ld\n", read_timer());

  // toggle pin
  if (pin_state)
  {
    drive_low();
  }
  else
  {
    drive_high();
  }

  // update states
  current_idx++;
  if (current_idx < BUFFER_SIZE)
  {
    start_next_timer();
  }
  else
  {
    stop_timer();
    writing = false;
  }
}

/**
 * LED matrix utils
 */
void led_matrix_init(void)
{
  // set gpio pin to output
  nrf_gpio_pin_dir_set(OUTPUT_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_clear(OUTPUT_PIN);
}

void drive_high(void)
{
  pin_state = 1;
  nrf_gpio_pin_write(OUTPUT_PIN, 1);
}
void drive_low(void)
{
  pin_state = 0;
  nrf_gpio_pin_write(OUTPUT_PIN, 0);
}

/**
 * write the buffer to output
 * must init led matrix and timer first
 */
void display_buffer(void)
{
  writing = true;
  printf("Period ticks: %ld\n", PERIOD_TICKS);
  printf("One bit ticks: %ld\n", ONE_BIT_TICKS);
  printf("Zero bit ticks: %ld\n", ZERO_BIT_TICKS);
  printf("Reset ticks: %ld\n", RESET_TICKS);
  drive_low();
  // reset everything
  current_idx = 0;
  // start timer
  start_next_timer();
  // wait until done writing
  nrf_delay_us(100);
  // while (writing) {
  //   nrf_delay_us(50);
  // }
  // print out print buffer
  for (uint32_t i = 0; i < print_idx; i += 3)
  {
    printf("At time %ld, set timer for time %ld\n", print_buf[i], print_buf[i + 1]);
    printf("Fired at %ld\n", print_buf[i + 2]);
  }
}
