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

// void TIMER2_IRQHandler(void)
// {
//   // This should always be the first line of the interrupt handler!
//   // It clears the event so that it doesn't happen again
//   NRF_TIMER2->EVENTS_COMPARE[1] = 0;

//   printf("TIMER 2 FIRED!\n");
// }

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
    buffer[i] = 0xFFFFFF;
  }
  display_buffer(buffer);
  // printf("%d\n", writing);
  // nrf_delay_ms(1000);

  // NRF_TIMER2->PRESCALER = 0;
  // NRF_TIMER2->BITMODE = 3;
  // NRF_TIMER2->MODE = 0;
  // NRF_TIMER2->INTENSET = 0x1 << 17;
  // NVIC_EnableIRQ(TIMER2_IRQn);
  // NRF_TIMER2->TASKS_CLEAR = 1;

  // NRF_TIMER2->TASKS_CAPTURE[0] = 1;
  // uint32_t timer_val = NRF_TIMER0->CC[0] + 60000;
  // uint32_t timer_val = 5;
  // printf("Timer val: %ld\n", timer_val);
  // NRF_TIMER2->CC[1] = timer_val;

  // NRF_TIMER2->TASKS_START = 1;

  // printf("chicken\n");

  // loop forever
  while (1)
  {
    printf("Looping\n");
    // printf("Timer 3: %ld\n", read_timer_3());
    // printf("Timer 4: %ld\n", read_timer_4());

    // NRF_TIMER2->TASKS_CAPTURE[0] = 1;
    // printf("Timer 2: %ld\n", NRF_TIMER2->CC[0]);

    nrf_delay_ms(1000);
  }
}
