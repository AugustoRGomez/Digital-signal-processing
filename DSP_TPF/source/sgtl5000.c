/*
 * sgtl5000.c
 *
 *  Created on: 13 mar. 2022
 *      Author: Augusto R Gomez
 */
#include "sgtl5000.h"

uint16_t sgtl5000_read_register(uint16_t addr) {
	uint16_t aux;
	i2c_master_transfer_t masterXfer;

    masterXfer.slaveAddress = I2C_SLAVE_ADDR;
    masterXfer.direction = kI2C_Read;
    masterXfer.subaddress  = (uint32_t) addr;
    masterXfer.subaddressSize = 2;
    masterXfer.data = (uint8_t *) &aux;
    masterXfer.dataSize = 2;
    masterXfer.flags = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(I2C0_PERIPHERAL, &masterXfer);

    //Swap top Byte with lower one
    aux = (aux << 8U) | (aux >> 8U);

    return aux;
}

void sgtl5000_write_register(uint16_t wBuff, uint16_t addr) {
    //Swap top Byte with lower one
	uint16_t aux = wBuff;
	aux = (aux << 8U) | (aux >> 8U);

	i2c_master_transfer_t masterXfer;

	masterXfer.slaveAddress = I2C_SLAVE_ADDR;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress  = (uint32_t) addr;
	masterXfer.subaddressSize = 2;
	masterXfer.data = (uint8_t *) &aux;
	masterXfer.dataSize = 2;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	I2C_MasterTransferBlocking(I2C0_PERIPHERAL, &masterXfer);

	//Pause time
	for(uint32_t i = 0U; i < WAIT_TIME; i++)
	{
		__NOP();
	}
}

void sgtl5000_modify_register(uint16_t addr, uint16_t newBits, uint8_t bitLen, uint8_t bitShift) {
	uint16_t mask = 0xFFFF;
	uint16_t dummy;

	//create mask
	for(uint8_t i = 0U; i < bitLen; i++) {
		mask &= ~(0b1 << (i + bitShift));
	}

	//read first
	dummy = sgtl5000_read_register(addr);

	//apply mask
	dummy &= mask;

	//update old bits
	dummy |= (newBits << bitShift);

	//write new register
	sgtl5000_write_register(dummy, addr);
}

void sgtl5000_power_up() {
	/*--------------- Power Supply Configuration----------------*/

	// NOTE: This next 2 Write calls is needed ONLY if VDDD is
	// internally driven by the chip
	// Configure VDDD level to 1.2V (bits 3:0)
	sgtl5000_write_register(0x0008, SGTL5000_CHIP_LINREG_CTRL);
	// Power up internal linear regulator (Set bit 9)
	sgtl5000_write_register(0x7260, SGTL5000_CHIP_ANA_POWER);

//	// NOTE: This next Write call is needed ONLY if VDDD is
//	// externally driven
//	// Turn off startup power supplies to save power (Clear bit 12 and 13)
//	Write CHIP_ANA_POWER 0x4260
//
//	// NOTE: The next Write calls is needed only if both VDDA and
//	// VDDIO power supplies are less than 3.1V.
//	// Enable the internal oscillator for the charge pump (Set bit 11)
//	Write CHIP_CLK_TOP_CTRL 0x0800
//	// Enable charge pump (Set bit 11)
//	Write CHIP_ANA_POWER 0x4A60
//	// NOTE: The next modify call is only needed if both VDDA and
//	// VDDIO are greater than 3.1 V
//	// Configure the charge pump to use the VDDIO rail (set bit 5 and
//	bit 6)
//	Write CHIP_LINREG_CTRL 0x006C
//	//---- Reference Voltage and Bias Current Configuration----
//	// NOTE: The value written in the next 2 Write calls is dependent
//	// on the VDDA voltage value.
//	// Set ground, ADC, DAC reference voltage (bits 8:4). The value
//	should
//	// be set to VDDA/2. This example assumes VDDA = 1.8 V.
//	VDDA/2 = 0.9 V.
//	// The bias current should be set to 50% of the nominal value (bits
//	3:1)
//	Write CHIP_REF_CTRL 0x004E
//	// Set LINEOUT reference voltage to VDDIO/2 (1.65 V) (bits 5:0)
//	and bias current (bits 11:8) to the recommended value of 0.36 mA
//	for 10 kOhm load with 1.0 nF capacitance
//	Write CHIP_LINE_OUT_CTRL 0x0322
//	//------------Other Analog Block Configurations--------------
//	// Configure slow ramp up rate to minimize pop (bit 0)
//	Write CHIP_REF_CTRL 0x004F
//	// Enable short detect mode for headphone left/right
//	// and center channel and set short detect current trip level
//	// to 75 mA
//	Write CHIP_SHORT_CTRL 0x1106
//	// Enable Zero-cross detect if needed for HP_OUT (bit 5) and ADC
//	(bit 1)
//	Write CHIP_ANA_CTRL 0x0133
//	//------------Power up Inputs/Outputs/Digital Blocks---------
//	// Power up LINEOUT, HP, ADC, DAC
//	Write CHIP_ANA_POWER 0x6AFF
//	// Power up desired digital blocks
//	// I2S_IN (bit 0), I2S_OUT (bit 1), DAP (bit 4), DAC (bit 5),
//	// ADC (bit 6) are powered on
//	Write CHIP_DIG_POWER 0x0073
//	//----------------Set LINEOUT Volume Level-------------------
//	// Set the LINEOUT volume level based on voltage reference
//	(VAG)
//	// values using this formula
//	// Value = (int)(40*log(VAG_VAL/LO_VAGCNTRL) + 15)
//	// Assuming VAG_VAL and LO_VAGCNTRL is set to 0.9 V and
//	1.65 V respectively, the // left LO vol (bits 12:8) and right LO
//	volume (bits 4:0) value should be set // to 5
//	Write CHIP_LINE_OUT_VOL 0x0505
}

void sgtl5000_RW_test() {
	/*
	 * Pruebas de lectura/escritura
	 */
	//leer DEVID
	uint16_t readVal = sgtl5000_read_register(SGTL5000_CHIP_ID);
	PRINTF("get sgtl5000 ID: %x\n", readVal);

	//leer test analogico 1
	readVal = sgtl5000_read_register(SGTL5000_CHIP_ANA_TEST1);
	PRINTF("get sgtl5000 CHIP_ANA_TEST1: %x\n", readVal);

	//escribir
	sgtl5000_write_register(0x1234, SGTL5000_CHIP_ANA_TEST1);

	//leer de nuevo
	readVal = sgtl5000_read_register(SGTL5000_CHIP_ANA_TEST1);
	PRINTF("get sgtl5000 CHIP_ANA_TEST1: %x\n", readVal);

	//modificar registro test ana 1
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_TEST1, 0xF, 4U, 8U);

	//leer de nuevo
	readVal = sgtl5000_read_register(SGTL5000_CHIP_ANA_TEST1);
	PRINTF("get sgtl5000 CHIP_ANA_TEST1: %x\n", readVal);
}
