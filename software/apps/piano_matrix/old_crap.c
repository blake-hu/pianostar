// // LED Matrix Driver
// // Displays characters on the LED matrix

// #include <stdbool.h>
// #include <stdio.h>

// #include "nrfx_i2s.h"

// #include "led_matrix.h"
// #include "microbit_v2.h"

// #define NLEDS 256
// #define RESET_BITS 6
// #define I2S_BUFFER_SIZE 3 * NLEDS + RESET_BITS

// #define MY_I2C_SDOUT_PIN 6

// static uint32_t m_buffer_tx[I2S_BUFFER_SIZE];

// static void data_handler(nrfx_i2s_buffers_t const *p_released, uint32_t status)
// {
//   // Non-NULL value in 'p_data_to_send' indicates that the driver needs
//   // a new portion of data to send.

//   return;
// }

// void led_matrix_init()
// {
//   // initialize I2S
//   nrfx_i2s_config_t config = NRFX_I2S_DEFAULT_CONFIG;
//   config.sdin_pin = I2S_CONFIG_SDIN_PIN;
//   config.sdout_pin = MY_I2C_SDOUT_PIN;
//   config.mck_setup = NRF_I2S_MCK_32MDIV10; // 32 MHz / 10 = 3.2 MHz
//   config.ratio = NRF_I2S_RATIO_32X;        // LRCK = MCK / 32
//   config.channels = NRF_I2S_CHANNELS_STEREO;

//   nrfx_err_t err = nrfx_i2s_init(&config, data_handler);
//   APP_ERROR_CHECK(err);
// }

// void set_leds()
// {
//   // OFF = 1000 = 0x8
//   // ON = 1110 = 0xe

//   // turn everything on
//   for (int i = 0; i < NLEDS; i++)
//   {
//     m_buffer_tx[i] = 0xeeeeeeee;
//   }

//   // reset
//   for (int i = 3 * NLEDS; i < I2S_BUFFER_SIZE; i++)
//   {
//     m_buffer_tx[i] = 0;
//   }
// }

// void start_transfer()
// {
//   nrfx_i2s_buffers_t const initial_buffers = {
//       .p_tx_buffer = m_buffer_tx,
//       .p_rx_buffer = NULL};
//   nrfx_err_t err = nrfx_i2s_start(&initial_buffers, I2S_BUFFER_SIZE, 0);
//   APP_ERROR_CHECK(err);
// }

// void stop_transfer()
// {
//   nrfx_i2s_stop();
// }