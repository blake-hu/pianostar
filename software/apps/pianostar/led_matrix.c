// LED Matrix Driver
// Displays characters on the LED matrix
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf_gpio.h"
#include "app_timer.h"

#include "led_matrix.h"
#include "font.h"
#include "microbit_v2.h"

#define DISPLAY_INTERVAL APP_TIMER_TICKS(1)

uint32_t rows[] = {LED_ROW1, LED_ROW2, LED_ROW3, LED_ROW4, LED_ROW5};
uint32_t cols[] = {LED_COL1, LED_COL2, LED_COL3, LED_COL4, LED_COL5};

static int cur_row = 0;
static bool led_states[5][5];

APP_TIMER_DEF(timer1);

void display_char(char letter)
{
  uint8_t *char_bitmap = font[(int)letter];
  for (int r = 0; r < 5; r++)
  {
    uint8_t row_data = char_bitmap[r];
    for (int c = 0; c < 5; c++)
    {
      int bit = (row_data >> c) & 1;
      led_states[r][c] = bit;
    }
  }
}

static void cycle_rows(void *_unused)
{
  nrf_gpio_pin_write(rows[cur_row], 0);
  cur_row = (cur_row + 1) % 5;

  for (int c = 0; c < 5; c++)
  {
    nrf_gpio_pin_write(cols[c], !led_states[cur_row][c]);
  }

  nrf_gpio_pin_write(rows[cur_row], 1);
}

void led_matrix_init(void)
{
  for (int i = 0; i < 5; i++)
  {
    nrf_gpio_pin_dir_set(cols[i], NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(rows[i], NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_clear(cols[i]);
    nrf_gpio_pin_clear(rows[i]);
  }

  app_timer_init();
  app_timer_create(&timer1, APP_TIMER_MODE_REPEATED, cycle_rows);

  app_timer_start(timer1, DISPLAY_INTERVAL, NULL);
}
