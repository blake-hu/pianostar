#include "i2c_adc.h"

// Pointer to an initialized I2C instance to use for transactions
NRF_TWI_MNGR_DEF(i2c_manager, 1, 0);

static const uint8_t ADC_ADDRESS[NUM_ADC] = {
    0x48,
    0x49,
    0x4a,
    0x4b,
};

static const uint8_t CHANNELS[NUM_ADC_CHANNELS] = {
    0b000 << 4, 0b100 << 4, 0b001 << 4, 0b101 << 4,
    0b010 << 4, 0b110 << 4, 0b011 << 4, 0b111 << 4,
};

// COMMAND BYTE
// MSB ---------------- LSB
// SD C2 C1 C0 PD1 PD0 X X
// - SD: single ended
// - PD: 1 1
static const uint8_t COMMAND_BASE = 0b10001100;

void i2c_init() {
  // Initialize I2C peripheral and driver
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  // WARNING!!
  // These are NOT the correct pins for external I2C communication.
  // If you are using QWIIC or other external I2C devices, the are
  // connected to EDGE_P19 (a.k.a. I2C_QWIIC_SCL) and EDGE_P20 (a.k.a.
  // I2C_QWIIC_SDA)
  i2c_config.scl = I2C_QWIIC_SCL;
  i2c_config.sda = I2C_QWIIC_SDA;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  i2c_config.interrupt_priority = 0;
  nrf_twi_mngr_init(&i2c_manager, &i2c_config);
}

uint8_t i2c_adc_read(uint8_t adc_idx, uint8_t adc_channel_idx) {
  uint8_t adc_address = ADC_ADDRESS[adc_idx];
  uint8_t command_byte = COMMAND_BASE | CHANNELS[adc_channel_idx];
  uint8_t rx_buf = 0;

  nrf_twi_mngr_transfer_t const read_transfer[] = {
      NRF_TWI_MNGR_WRITE(adc_address, &command_byte, 1, NRF_TWI_MNGR_NO_STOP),
      NRF_TWI_MNGR_READ(adc_address, &rx_buf, 1, 0)};

  ret_code_t result =
      nrf_twi_mngr_perform(&i2c_manager, NULL, read_transfer, 2, NULL);
  if (result != NRF_SUCCESS) {
    printf("I2C transaction failed! Error: %lX\n", result);
  }

  return rx_buf;
}
