/*
 * sgtl5000.c
 *
 *  Created on: 13 mar. 2022
 *      Author: Augusto R Gomez
 */
#include "sgtl5000.h"

void sgtl5000_read_register(uint16_t *rBuff, uint16_t addr) {
	i2c_master_transfer_t masterXfer;

    masterXfer.slaveAddress = I2C_SLAVE_ADDR;
    masterXfer.direction = kI2C_Read;
    masterXfer.subaddress  = (uint32_t) addr;
    masterXfer.subaddressSize = 2;
    masterXfer.data = (uint8_t *) rBuff;
    masterXfer.dataSize = 2;
    masterXfer.flags = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(I2C0_PERIPHERAL, &masterXfer);

    //Swap top Byte with lower one
    uint16_t aux = *rBuff;
    *rBuff = (aux << 8U) | (aux >> 8U);
}

void sgtl5000_write_register(uint16_t *wBuff, uint16_t addr) {
    //Swap top Byte with lower one
	uint16_t aux = *wBuff;
	*wBuff = (aux << 8U) | (aux >> 8U);

	i2c_master_transfer_t masterXfer;

	masterXfer.slaveAddress = I2C_SLAVE_ADDR;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress  = (uint32_t) addr;
	masterXfer.subaddressSize = 2;
	masterXfer.data = (uint8_t *) wBuff;
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
	sgtl5000_read_register(&dummy, addr);

	//apply mask
	dummy &= mask;

	//update old bits
	dummy |= (newBits << bitShift);

	//write new register
	sgtl5000_write_register(&dummy, addr);
}

void sgtl5000_power_up() {
}

void sgtl5000_test(uint16_t *pW, uint16_t *pR) {
	/*
	 * Pruebas de lectura/escritura
	 */
	//leer DEVID
	sgtl5000_read_register(pR, SGTL5000_CHIP_ID);
	PRINTF("get sgtl5000 ID: %x\n", *pR);

	//leer test analogico 1
	sgtl5000_read_register(pR, SGTL5000_CHIP_ANA_TEST1);
	PRINTF("get sgtl5000 CHIP_ANA_TEST1: %x\n", *pR);

	//escribir
	*pW = 0x1234;

	sgtl5000_write_register(pW, SGTL5000_CHIP_ANA_TEST1);

	//leer de nuevo
	sgtl5000_read_register(pR, SGTL5000_CHIP_ANA_TEST1);
	PRINTF("get sgtl5000 CHIP_ANA_TEST1: %x\n", *pR);

	//modificar registro test ana 1
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_TEST1, 0xF, 4U, 8U);

	//leer de nuevo
	sgtl5000_read_register(pR, SGTL5000_CHIP_ANA_TEST1);
	PRINTF("get sgtl5000 CHIP_ANA_TEST1: %x\n", *pR);
}
