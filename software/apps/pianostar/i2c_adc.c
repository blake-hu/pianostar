#include "nrf_twi_mngr.h"

static const uint8_t ADC_ADDRESS_0 = 0x48;
static const uint8_t ADC_ADDRESS_1 = 0x49;
static const uint8_t ADC_ADDRESS_2 = 0x4a;
static const uint8_t ADC_ADDRESS_3 = 0x4b;

// COMMAND BYTE
// MSB ---------------- LSB
// SD C2 C1 C0 PD1 PD0 X X
// - SD: single ended
// - PD: 1 1
static const uint8_t COMMAND_BASE = 0b10001100;
// Pointer to an initialized I2C instance to use for transactions
static const nrf_twi_mngr_t* i2c_manager = NULL;

static const uint8_t CHANNELS[8] = {
    0b000 << 4,
    0b100 << 4,
    0b001 << 4,
    0b101 << 4,
    0b010 << 4,
    0b110 << 4,
    0b011 << 4,
    0b111 << 4,
};

uint8_t read_adc(uint8_t adc_address, uint8_t adc_channel)
{
    uint8_t command_byte = COMMAND_BASE | CHANNELS[adc_channel];
    uint8_t rx_buf = 0;

    nrf_twi_mngr_transfer_t const read_transfer[] = {
        NRF_TWI_MNGR_WRITE(adc_address, &command_byte, 1, NRF_TWI_MNGR_NO_STOP),
        NRF_TWI_MNGR_READ(adc_address, &rx_buf, 1, 0)
    };

    ret_code_t result = nrf_twi_mngr_perform(i2c_manager, NULL, read_transfer, 2, NULL);
    if (result != NRF_SUCCESS) {
        // Likely error codes:
        //  NRF_ERROR_INTERNAL            (0x0003) - something is wrong with the driver itself
        //  NRF_ERROR_INVALID_ADDR        (0x0010) - buffer passed was in Flash instead of RAM
        //  NRF_ERROR_BUSY                (0x0011) - driver was busy with another transfer still
        //  NRF_ERROR_DRV_TWI_ERR_OVERRUN (0x8200) - data was overwritten during the transaction
        //  NRF_ERROR_DRV_TWI_ERR_ANACK   (0x8201) - i2c device did not acknowledge its address
        //  NRF_ERROR_DRV_TWI_ERR_DNACK   (0x8202) - i2c device did not acknowledge a data byte
        printf("I2C transaction failed! Error: %lX\n", result);
    }

    return rx_buf;
}
