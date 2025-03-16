// LED Matrix app
//
// Display messages on the LED matrix

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"

#include "piano_matrix.h"
#include "microbit_v2.h"

static uint32_t buffer[256];

int main(void)
{
  printf("\n");
  printf("Board started!\n");

  // init
  led_matrix_init();
  timer_init();

  // turn everything on
  for (int i = 0; i < 256; i++)
  {
    buffer[i] = 0x0000FF;
  }
  display_buffer(buffer);

  // loop forever
  while (1)
  {
    printf("Looping\n");

    nrf_delay_ms(1000);
  }
}
