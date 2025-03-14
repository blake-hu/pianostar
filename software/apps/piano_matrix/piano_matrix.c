// LED Matrix Driver
// Displays characters on the LED matrix

#include <stdbool.h>
#include <stdio.h>

#include "nrf_gpio.h"
#include "app_timer.h"

#include "led_matrix.h"
#include "font.h"
#include "microbit_v2.h"

uint32_t OUTPUT_PIN = 15;

APP_TIMER_DEF(timer1);

uint32_t PERIOD_TICKS = 1000;
uint32_t ONE_BIT_TICKS = 500;
uint32_t ZERO_BIT_TICKS = 100;

void led_matrix_init(void)
{
  // set gpio pin to output
  nrf_gpio_pin_dir_set(OUTPUT_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_clear(OUTPUT_PIN);
}

void drive_high(void)
{
  // set gpio pin high
  nrf_gpio_pin_write(OUTPUT_PIN, 1);
}

void timer_init(void)
{
  // set up new timer to drive pin high every X ticks
  app_timer_init();
  app_timer_create(&timer1, APP_TIMER_MODE_REPEATED, drive_high);
  app_timer_start(timer1, PERIOD_TICKS, NULL);
}

void write_bit(uint8_t bit)
{
  // write bit to gpio pin
}

void write_24_bits(uint32_t bits)
{
  // 0000s at the top
  // write 24 bits to gpio pin
}
