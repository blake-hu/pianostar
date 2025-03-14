// LED Matrix Driver
// Displays characters on the LED matrix

#include <stdbool.h>
#include <stdio.h>

#include "nrf_gpio.h"
#include "app_timer.h"

#include "led_matrix.h"
#include "font.h"
#include "microbit_v2.h"

uint32_t PERIOD_TICKS = 1000;

void led_matrix_init(void) {
  // set gpio pin to output

}

void timer_init(void) {
  // set up new timer to drive pin high every X ticks
}

void write_bit(uint8_t bit) {
  // write bit to gpio pin
}

void write_24_bits(uint32_t bits) {
  // 0000s at the top
  // write 24 bits to gpio pin
}
