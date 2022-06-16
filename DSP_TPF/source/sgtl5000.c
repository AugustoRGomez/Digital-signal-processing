/*
 * sgtl5000.c
 *
 *  Created on: 13 mar. 2022
 *      Author: Augusto R Gomez
 *
 *  Codec init config sequence should follow the next order to avoid pops and glitching sounds:
 *  -sgtl5000_power_up();
 *  -sgtl5000_IO_routing();
 *  -sgtl5000_dap_setup();
 *  -sgtl5000_clk_setup();
 *  -sgtl5000_i2s_setup();
 *  -sgtl5000_volume_setup();
 *
 *  You can modify each function as wish.
 *  Also, you can just execute sgtl5000_init_xx_32K() init functions for a quick configuration.
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
	// NOTE: This next Write call is needed ONLY if VDDD is
	// externally driven
	// Turn off startup power supplies to save power (Clear bit 12 and 13)
	//Write CHIP_ANA_POWER 0x4260
	// NOTE: The next 2 Write calls is needed only if both VDDA and
	// VDDIO power supplies are less than 3.1V.
	// Enable the internal oscillator for the charge pump (Set bit 11)
//	sgtl5000_write_register(0x0800, SGTL5000_CHIP_CLK_TOP_CTRL);
	// Enable charge pump (Set bit 11)
//	Write CHIP_ANA_POWER 0x4A60
	// NOTE: The next modify call is only needed if both VDDA and
	// VDDIO are greater than 3.1V
	// Configure the charge pump to use the VDDIO rail (set bit 5 and bit 6)
	sgtl5000_modify_register(SGTL5000_CHIP_LINREG_CTRL, 0b11, 2U, 5U);

	/*------ Reference Voltage and Bias Current Configuration----------*/
	// NOTE: The value written in the next 2 Write calls is dependent
	// on the VDDA voltage value.
	// Set ground, ADC, DAC reference voltage (bits 8:4). The value should
	// be set to VDDA/2. This example assumes VDDA = 1.8V. VDDA/2 = 0.9V.
	// The bias current should be set to 50% of the nominal value (bits 3:1) 0x01E0
	sgtl5000_write_register(0x01E0, SGTL5000_CHIP_REF_CTRL);
	// Set LINEOUT reference voltage to VDDIO/2 (1.65V) (bits 5:0) and bias current (bits 11:8) to
	// the recommended value of 0.36mA for 10kOhm load with 1nF capacitance
	//Write CHIP_LINE_OUT_CTRL 0x0322

	/*----------------Other Analog Block Configurations------------------*/
	// Configure slow ramp up rate to minimize pop (bit 0)
	sgtl5000_modify_register(SGTL5000_CHIP_REF_CTRL, 0b1, 1U, 0U);
	// Enable short detect mode for headphone left/right
	// and center channel and set short detect current trip level
	// to 75mA
	sgtl5000_write_register(0x1106, SGTL5000_CHIP_SHORT_CTRL);
	// Enable Zero-cross detect if needed for HP_OUT (bit 5) and ADC (bit 1) (0x0133)
	sgtl5000_write_register(0x0133, SGTL5000_CHIP_ANA_CTRL);

	/*----------------Power up Inputs/Outputs/Digital Blocks-------------*/
	// Power up LINEOUT(nop), CAPELESS_MODE, HP, ADC, DAC, CHARGEPUMP, ADC_mono(nop)
	sgtl5000_write_register(0x6AFB, SGTL5000_CHIP_ANA_POWER); // poner una B para desactivar CAPLESS 0x6AFB
	// Power up desired digital blocks
	// DAC, ADC, I2S_IN, I2S_OUT, DAP (0x0063)
	sgtl5000_write_register(0x0063, SGTL5000_CHIP_DIG_POWER);
	// Desactivar DAP
	sgtl5000_modify_register(SGTL5000_CHIP_DIG_POWER, 0b0, 1U, 4U); // bit 4

	//--------------------Set LINEOUT Volume Level-----------------------*/
	// Set the LINEOUT volume level based on voltage reference (VAG)
	// values using this formula
	// Value = (int)(40*log(VAG_VAL/LO_VAGCNTRL) + 15)
	// Assuming VAG_VAL and LO_VAGCNTRL is set to 0.9V and 1.65V respectively, the
	// left LO volume (bits 12:8) and right LO volume (bits 4:0) value should be set
	// to 5
	// Write CHIP_LINE_OUT_VOL 0x0505

}

void sgtl5000_clk_setup() {
	/* MCLK & SampleCLK */
	// 0x0008 Configure SYS_FS clock to 48 kHz, MCLK_FREQ to 256*Fs
	// 0x0000 Configure SYS_FS clock to 32 kHz, MCLK_FREQ to 256*Fs
	sgtl5000_write_register(0x0000, SGTL5000_CHIP_CLK_CTRL);
}

void sgtl5000_IO_routing() {
//	// Example 1: I2S_IN -> DAP -> DAC -> LINEOUT, HP_OUT
//	// Route I2S_IN to DAP
//	Modify CHIP_SSS_CTRL->DAP_SELECT 0x0001 // bits 7:6
//	// Route DAP to DAC
//	Modify CHIP_SSS_CTRL->DAC_SELECT 0x0003 // bits 5:4
//	// Select DAC as the input to HP_OUT
//	Modify CHIP_ANA_CTRL->SELECT_HP 0x0000 // bit 6

	// Example 2: LINE_IN -> ADC -> I2S_OUT, I2S_IN -> DAC -> HP_OUT
	// Set ADC input to LINE_IN
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b1, 1U, 2U); // bit 2
	// Route ADC to I2S_OUT, I2S_IN to DAC
	sgtl5000_write_register(0x0010, SGTL5000_CHIP_SSS_CTRL);
	// Set DAC output to HP_OUT
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 0U, 6U); // bit 6

//	// Example 3: LINE_IN -> ADC -> I2S_OUT, I2S_IN -> DAP -> DAC -> HP_OUT
//	// Set ADC input to LINE_IN
//	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b1, 1U, 2U); // bit 2
//	// Route ADC to I2S_OUT, I2S_IN to DAP and DAP to DAC
//	sgtl5000_write_register(0x0070, SGTL5000_CHIP_SSS_CTRL);
//	// Set DAC output to HP_OUT
//	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 0U, 6U); // bit 6

//	// Example 4: LINEIN -> HP_OUT
//	// Select LINEIN as the input to HP_OUT
//	//sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b1, 1U, 6U);

//	//Example 5: MIC_IN -> ADC -> DAC -> HP
//	//MIC_IN -> ADC
//	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 2U); // bit 2
//	//ADC -> DAC
//	sgtl5000_write_register(0x0000, SGTL5000_CHIP_SSS_CTRL); // bit 5:4
//	//DAC -> HP
//	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 6U); // bits 6

//	// Example 6: MIC_IN -> ADC -> DAP -> DAC -> HP
//	// MIC_IN -> ADC
//	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 2U); // bit 2
//	// ADC -> DAP -> DAC
//	sgtl5000_write_register(0x0030, SGTL5000_CHIP_SSS_CTRL); // bit 5:4
//	// DAC -> HP
//	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 6U); // bits 6

//	// Example 7: LINE_IN -> ADC -> DAP -> DAC -> HP
//	// LINE_IN -> ADC
//	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b1, 1U, 2U); // bit 2
//	// ADC -> DAP -> DAC
//	sgtl5000_write_register(0x0030, SGTL5000_CHIP_SSS_CTRL); // bit 5:4
//	// DAC -> HP
//	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 6U); // bits 6

}

void sgtl5000_dap_setup() {
	// Enable DAP block
	// NOTE: DAP will be in a pass-through mode if none of DAP
	// sub-blocks are enabled.
//	sgtl5000_modify_register(SGTL5000_CHIP_DAP_CTRL, 0b1, 1U, 0U);
	// 7-Band PEQ Mode
//	// Select 7-Band PEQ mode and enable 7 PEQ filters
//	Write DAP_AUDIO_EQ 0x0001
//	Write DAP_PEQ 0x0007
	// Tone Control mode
//	sgtl5000_write_register(0x0002, SGTL5000_DAP_AUDIO_EQ);
//	// 5-Band GEQ Mode
//	sgtl5000_write_register(0x0003, SGTL5000_DAP_AUDIO_EQ);

	//Change Treble band (9.9KHz) to -10dB
//	sgtl5000_write_register(0x0007, SGTL5000_DAP_AUDIO_EQ_BASS_BAND4);
}

void sgtl5000_i2s_setup() {
	// Configure codec as Slave
	// 0x0000 32bits SCLK= 64*Fs (best perfomance)
	// 0x0080 32bits SCLK= 64*Fs SCLK falling edge
	// 0x0170 16bits SCLK= 32*Fs SCLK falling edge
 	sgtl5000_write_register(0x0000, SGTL5000_CHIP_I2S_CTRL);
}

void sgtl5000_volume_setup() {
	/*---------------- Input Volume Control---------------------*/
	// Configure ADC left and right analog volume to desired default.
	// Example shows volume of 0dB
	sgtl5000_write_register(0x0000, SGTL5000_CHIP_ANA_ADC_CTRL);

	// Configure MIC gain if needed. Example shows gain of 20dB (D)
	// Configure MIC bias voltage (C)
	// Configure MIC bias resistor (B)
	sgtl5000_write_register(0x0000, SGTL5000_CHIP_MIC_CTRL); // 0xABCD

	/*---------------- Volume and Mute Control---------------------*/
	// Configure HP_OUT left and right volume to minimum, unmute
	// HP_OUT and ramp the volume up to desired volume.
	sgtl5000_write_register(0x7F7F, SGTL5000_CHIP_ANA_HP_CTRL);
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 4U); // bit 4

	// Code assumes that left and right volumes are set to same value
	// So it only uses the left volume for the calculations
	uint8_t currentVol = 0x7F;
	uint8_t finalVol = 0x18;
	uint8_t numSteps = abs(currentVol - finalVol);
	uint16_t currentVolLR;

	// Ramp up
	if (numSteps > 0U) {
		for(uint16_t i = 0U; i < numSteps; i++) {
			--currentVol;
			currentVolLR = (currentVol << 8) | (currentVol);
			sgtl5000_write_register(currentVolLR, SGTL5000_CHIP_ANA_HP_CTRL);
		}
	}

	// LINEOUT and DAC volume control
	//sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 8U); // bit 8
	// Configure DAC left and right digital volume. Example shows
	// volume of 0dB
	sgtl5000_write_register(0x3C3C, SGTL5000_CHIP_DAC_VOL);
	sgtl5000_write_register(0x0200, SGTL5000_CHIP_ADCDAC_CTRL);

	// Unmute ADC
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 0U); // bit 0
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

	//modificar 3 bits
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_TEST1, 0x4, 3U, 8U);

	//leer de nuevo
	readVal = sgtl5000_read_register(SGTL5000_CHIP_ANA_TEST1);
	PRINTF("get sgtl5000 CHIP_ANA_TEST1: %x\n", readVal);

	//modificar 2 bits
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_TEST1, 0b10, 2U, 0U);

	//leer de nuevo
	readVal = sgtl5000_read_register(SGTL5000_CHIP_ANA_TEST1);
	PRINTF("get sgtl5000 CHIP_ANA_TEST1: %x\n", readVal);

	if(readVal == 0x1c36)
		PRINTF("No errors detected. Test passed");
	else
		PRINTF("An error occurred. Test failed.");
}

void sgtl5000_init_Line_in_AVC_HP_out_32K() {
	/*--------------- Power Supply Configuration------------------------*/
	// NOTE: This next 2 Write calls is needed ONLY if VDDD is
	// internally driven by the chip
	// Configure VDDD level to 1.2V (bits 3:0)
	sgtl5000_write_register(0x0008, SGTL5000_CHIP_LINREG_CTRL);
	// Power up internal linear regulator (Set bit 9)
	sgtl5000_write_register(0x7260, SGTL5000_CHIP_ANA_POWER);
	// NOTE: The next modify call is only needed if both VDDA and
	// VDDIO are greater than 3.1V
	// Configure the charge pump to use the VDDIO rail (set bit 5 and bit 6)
	sgtl5000_modify_register(SGTL5000_CHIP_LINREG_CTRL, 0b11, 2U, 5U);

	/*------ Reference Voltage and Bias Current Configuration-----------*/
	// NOTE: The value written in the next 2 Write calls is dependent
	// on the VDDA voltage value.
	// Set ground, ADC, DAC reference voltage (bits 8:4). The value should
	// be set to VDDA/2. This example assumes VDDA = 1.8V. VDDA/2 = 0.9V.
	// The bias current should be set to 50% of the nominal value (bits 3:1)
	sgtl5000_write_register(0x01E0, SGTL5000_CHIP_REF_CTRL);

	/*----------------Other Analog Block Configurations-----------------*/
	// Configure slow ramp up rate to minimize pop (bit 0)
	sgtl5000_modify_register(SGTL5000_CHIP_REF_CTRL, 0b1, 1U, 0U);
	// Enable short detect mode for headphone left/right
	// and center channel and set short detect current trip level
	// to 75mA
	sgtl5000_write_register(0x1106, SGTL5000_CHIP_SHORT_CTRL);
	// Enable Zero-cross detect if needed for HP_OUT (bit 5) and ADC (bit 1)
	sgtl5000_write_register(0x0133, SGTL5000_CHIP_ANA_CTRL);

	/*----------------Power up Inputs/Outputs/Digital Blocks------------*/
	// Power up HP, ADC, DAC, CHARGEPUMP
	sgtl5000_write_register(0x6AFB, SGTL5000_CHIP_ANA_POWER);
	// Power up desired digital blocks
	// DAC, ADC, I2S_IN, I2S_OUT, DAP
	sgtl5000_write_register(0x0073, SGTL5000_CHIP_DIG_POWER);

	/*-------------------------I/O routing------------------------------*/
	// LINE_IN -> ADC -> I2S_OUT -> DAP -> I2S_IN -> DAC -> HP_OUT
	// Set ADC input to LINE_IN
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b1, 1U, 2U); // bit 2
	// Route ADC to I2S_OUT, I2S_IN to DAP, DAP to DAC
	sgtl5000_write_register(0x0070, SGTL5000_CHIP_SSS_CTRL);
	// Set DAC output to HP_OUT
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 0U, 6U); // bit 6

	/*-------------------------DAP setup--------------------------------*/
	sgtl5000_write_register(0x0001, SGTL5000_CHIP_DAP_CTRL); //SGTL5000_DAP_CONTROL
	sgtl5000_write_register(0x0003, SGTL5000_DAP_AUDIO_EQ); //SGTL5000_DAP_AUDIO_EQ
	sgtl5000_write_register(0x0A40, SGTL5000_DAP_AVC_THRESHOLD); //SGTL5000_DAP_AVC_THRESHOLD
	sgtl5000_write_register(0x0014, SGTL5000_DAP_AVC_ATTACK); //SGTL5000_DAP_AVC_ATTACK
	sgtl5000_write_register(0x0028, SGTL5000_DAP_AVC_DECAY); //SGTL5000_DAP_AVC_DECAY
	sgtl5000_write_register(0x0001, SGTL5000_CHIP_DAP_AVC_CTRL); //SGTL5000_DAP_AVC_CTRL

	/*-------------------------CLK setup--------------------------------*/
	/* MCLK & SampleCLK */
	// 0x0008 Configure SYS_FS clock to 48 kHz, MCLK_FREQ to 256*Fs
	// 0x0000 Configure SYS_FS clock to 32 kHz, MCLK_FREQ to 256*Fs
	sgtl5000_write_register(0x0000, SGTL5000_CHIP_CLK_CTRL);

	/*-------------------------I2S setup--------------------------------*/
	/* Configure codec as Slave */
	// 0x0000 32bits SCLK= 64*Fs (best perfomance)
	// 0x0080 32bits SCLK= 64*Fs SCLK falling edge
	// 0x0170 16bits SCLK= 32*Fs SCLK falling edge
	sgtl5000_write_register(0x0000, SGTL5000_CHIP_I2S_CTRL);

	/*---------------- Input Volume Control---------------------*/
	// Configure ADC left and right analog volume to desired default.
	// Example shows volume of 0dB
	sgtl5000_write_register(0x0000, SGTL5000_CHIP_ANA_ADC_CTRL);

	// Configure MIC gain if needed (D)
	// Configure MIC bias voltage (C)
	// Configure MIC bias resistor (B)
	sgtl5000_write_register(0x0000, SGTL5000_CHIP_MIC_CTRL); // Desactivado

	/*---------------- Volume and Mute Control--------------------------*/
	// Configure HP_OUT left and right volume to minimum, unmute
	// HP_OUT and ramp the volume up to desired volume.
	sgtl5000_write_register(0x7F7F, SGTL5000_CHIP_ANA_HP_CTRL);
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 4U); // bit 4

	// Code assumes that left and right volumes are set to same value
	// So it only uses the left volume for the calculations
	uint8_t currentVol = 0x7F;
	uint8_t finalVol = 0x18;
	uint8_t numSteps = abs(currentVol - finalVol);
	uint16_t currentVolLR;

	// Ramp up
	if (numSteps > 0U) {
		for(uint16_t i = 0U; i < numSteps; i++) {
			--currentVol;
			currentVolLR = (currentVol << 8) | (currentVol);
			sgtl5000_write_register(currentVolLR, SGTL5000_CHIP_ANA_HP_CTRL);
		}
	}

	// Configure DAC left and right digital volume. Example shows
	// volume of 0dB
	sgtl5000_write_register(0x3C3C, SGTL5000_CHIP_DAC_VOL);
	sgtl5000_write_register(0x0200, SGTL5000_CHIP_ADCDAC_CTRL);

	// Unmute ADC
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 0U); // bit 0
}

void sgtl5000_init_Line_in_HP_out_32K() {
	/*--------------- Power Supply Configuration------------------------*/
	// NOTE: This next 2 Write calls is needed ONLY if VDDD is
	// internally driven by the chip
	// Configure VDDD level to 1.2V (bits 3:0)
	sgtl5000_write_register(0x0008, SGTL5000_CHIP_LINREG_CTRL);
	// Power up internal linear regulator (Set bit 9)
	sgtl5000_write_register(0x7260, SGTL5000_CHIP_ANA_POWER);
	// NOTE: The next modify call is only needed if both VDDA and
	// VDDIO are greater than 3.1V
	// Configure the charge pump to use the VDDIO rail (set bit 5 and bit 6)
	sgtl5000_modify_register(SGTL5000_CHIP_LINREG_CTRL, 0b11, 2U, 5U);

	/*------ Reference Voltage and Bias Current Configuration-----------*/
	// NOTE: The value written in the next 2 Write calls is dependent
	// on the VDDA voltage value.
	// Set ground, ADC, DAC reference voltage (bits 8:4). The value should
	// be set to VDDA/2. This example assumes VDDA = 1.8V. VDDA/2 = 0.9V.
	// The bias current should be set to 50% of the nominal value (bits 3:1)
	sgtl5000_write_register(0x01E0, SGTL5000_CHIP_REF_CTRL);

	/*----------------Other Analog Block Configurations-----------------*/
	// Configure slow ramp up rate to minimize pop (bit 0)
	sgtl5000_modify_register(SGTL5000_CHIP_REF_CTRL, 0b1, 1U, 0U);
	// Enable short detect mode for headphone left/right
	// and center channel and set short detect current trip level
	// to 75mA
	sgtl5000_write_register(0x1106, SGTL5000_CHIP_SHORT_CTRL);
	// Enable Zero-cross detect if needed for HP_OUT (bit 5) and ADC (bit 1)
	sgtl5000_write_register(0x0133, SGTL5000_CHIP_ANA_CTRL);

	/*----------------Power up Inputs/Outputs/Digital Blocks------------*/
	// Power up HP, ADC, DAC, CHARGEPUMP
	sgtl5000_write_register(0x6AFB, SGTL5000_CHIP_ANA_POWER);
	// Power up desired digital blocks
	// DAC, ADC, I2S_IN, I2S_OUT, DAP
	sgtl5000_write_register(0x0073, SGTL5000_CHIP_DIG_POWER);

	/*-------------------------I/O routing------------------------------*/
	// Example 2: LINE_IN -> ADC -> I2S_OUT, I2S_IN -> DAC -> HP_OUT
	// Set ADC input to LINE_IN
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b1, 1U, 2U); // bit 2
	// Route ADC to I2S_OUT, I2S_IN to DAC
	sgtl5000_write_register(0x0010, SGTL5000_CHIP_SSS_CTRL);
	// Set DAC output to HP_OUT
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 0U, 6U); // bit 6

	/*-------------------------DAP setup--------------------------------*/
	/*-------------------------CLK setup--------------------------------*/
	/* MCLK & SampleCLK */
	// 0x0008 Configure SYS_FS clock to 48 kHz, MCLK_FREQ to 256*Fs
	// 0x0000 Configure SYS_FS clock to 32 kHz, MCLK_FREQ to 256*Fs
	sgtl5000_write_register(0x0000, SGTL5000_CHIP_CLK_CTRL);

	/*-------------------------I2S setup--------------------------------*/
	/* Configure codec as Slave */
	// 0x0000 32bits SCLK= 64*Fs (best perfomance)
	// 0x0080 32bits SCLK= 64*Fs SCLK falling edge
	// 0x0170 16bits SCLK= 32*Fs SCLK falling edge
	sgtl5000_write_register(0x0000, SGTL5000_CHIP_I2S_CTRL);

	/*---------------- Input Volume Control---------------------*/
	// Configure ADC left and right analog volume to desired default.
	// Example shows volume of 0dB
	sgtl5000_write_register(0x0000, SGTL5000_CHIP_ANA_ADC_CTRL);

	// Configure MIC gain if needed (D)
	// Configure MIC bias voltage (C)
	// Configure MIC bias resistor (B)
	sgtl5000_write_register(0x0000, SGTL5000_CHIP_MIC_CTRL); // Desactivado

	/*---------------- Volume and Mute Control--------------------------*/
	// Configure HP_OUT left and right volume to minimum, unmute
	// HP_OUT and ramp the volume up to desired volume.
	sgtl5000_write_register(0x7F7F, SGTL5000_CHIP_ANA_HP_CTRL);
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 4U); // bit 4

	// Code assumes that left and right volumes are set to same value
	// So it only uses the left volume for the calculations
	uint8_t currentVol = 0x7F;
	uint8_t finalVol = 0x18;
	uint8_t numSteps = abs(currentVol - finalVol);
	uint16_t currentVolLR;

	// Ramp up
	if (numSteps > 0U) {
		for(uint16_t i = 0U; i < numSteps; i++) {
			--currentVol;
			currentVolLR = (currentVol << 8) | (currentVol);
			sgtl5000_write_register(currentVolLR, SGTL5000_CHIP_ANA_HP_CTRL);
		}
	}

	// Configure DAC left and right digital volume. Example shows
	// volume of 0dB
	sgtl5000_write_register(0x3C3C, SGTL5000_CHIP_DAC_VOL);
	sgtl5000_write_register(0x0200, SGTL5000_CHIP_ADCDAC_CTRL);

	// Unmute ADC
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 0U); // bit 0
}

void sgtl5000_init_MIC_in_AVC_HP_Line_out_32K() {
	/*--------------- Power Supply Configuration------------------------*/
	// NOTE: This next 2 Write calls is needed ONLY if VDDD is
	// internally driven by the chip
	// Configure VDDD level to 1.2V (bits 3:0)
	sgtl5000_write_register(0x0008, SGTL5000_CHIP_LINREG_CTRL);
	// Power up internal linear regulator (Set bit 9)
	sgtl5000_write_register(0x7260, SGTL5000_CHIP_ANA_POWER);
	// NOTE: The next modify call is only needed if both VDDA and
	// VDDIO are greater than 3.1V
	// Configure the charge pump to use the VDDIO rail (set bit 5 and bit 6)
	sgtl5000_modify_register(SGTL5000_CHIP_LINREG_CTRL, 0b11, 2U, 5U);

	/*------ Reference Voltage and Bias Current Configuration-----------*/
	// NOTE: The value written in the next 2 Write calls is dependent
	// on the VDDA voltage value.
	// Set ground, ADC, DAC reference voltage (bits 8:4). The value should
	// be set to VDDA/2. This example assumes VDDA = 1.8V. VDDA/2 = 0.9V.
	// The bias current should be set to 50% of the nominal value (bits 3:1)
	sgtl5000_write_register(0x01E0, SGTL5000_CHIP_REF_CTRL);

	// Set LINEOUT reference voltage to VDDIO/2 (1.65V) (bits 5:0) and bias current (bits 11:8) to
	// the recommended value of 0.36mA for 10kOhm load with 1nF capacitance
	sgtl5000_write_register(0x0322, SGTL5000_CHIP_LINE_OUT_CTRL);

	/*----------------Other Analog Block Configurations-----------------*/
	// Configure slow ramp up rate to minimize pop (bit 0)
	sgtl5000_modify_register(SGTL5000_CHIP_REF_CTRL, 0b1, 1U, 0U);
	// Enable short detect mode for headphone left/right
	// and center channel and set short detect current trip level
	// to 75mA
	sgtl5000_write_register(0x1106, SGTL5000_CHIP_SHORT_CTRL);
	// Enable Zero-cross detect if needed for HP_OUT (bit 5) and ADC (bit 1)
	sgtl5000_write_register(0x0133, SGTL5000_CHIP_ANA_CTRL);

	/*----------------Power up Inputs/Outputs/Digital Blocks------------*/
	// Power up HP, ADC, DAC, CHARGEPUMP
	sgtl5000_write_register(0x6AFB, SGTL5000_CHIP_ANA_POWER);
	// Power up LINE_out
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_POWER, 0b1, 1U, 0U);
	// Power up desired digital blocks
	// DAC, ADC, I2S_IN, I2S_OUT, DAP
	sgtl5000_write_register(0x0073, SGTL5000_CHIP_DIG_POWER);

	/*--------------------Set LINEOUT Volume Level-----------------------*/
	// Set the LINEOUT volume level based on voltage reference (VAG)
	// values using this formula
	// Value = (int)(40*log(VAG_VAL/LO_VAGCNTRL) + 15)
	// Assuming VAG_VAL and LO_VAGCNTRL is set to 0.9V and 1.65V respectively, the
	// left LO volume (bits 12:8) and right LO volume (bits 4:0) value should be set
	// to 5
	sgtl5000_write_register(0x0505, SGTL5000_CHIP_LINE_OUT_VOL);

	/*-------------------------I/O routing------------------------------*/
	// LINE_IN -> ADC -> DAP -> I2S_OUT -> I2S_IN -> DAC -> HP_OUT & LINE_OUT
	// Set ADC input to MIC_IN
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 2U); // bit 2
	// Route ADC to DAP, DAP to I2S_OUT, I2S_IN to DAC
	sgtl5000_write_register(0x0013, SGTL5000_CHIP_SSS_CTRL);
	// Set DAC output to HP_OUT
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 0U, 6U); // bit 6

	/*-------------------------DAP setup--------------------------------*/
	sgtl5000_write_register(0x0001, SGTL5000_CHIP_DAP_CTRL); //SGTL5000_DAP_CONTROL
	sgtl5000_write_register(0x0003, SGTL5000_DAP_AUDIO_EQ); //SGTL5000_DAP_AUDIO_EQ
	sgtl5000_write_register(0x0A40, SGTL5000_DAP_AVC_THRESHOLD); //SGTL5000_DAP_AVC_THRESHOLD
	sgtl5000_write_register(0x0014, SGTL5000_DAP_AVC_ATTACK); //SGTL5000_DAP_AVC_ATTACK
	sgtl5000_write_register(0x0028, SGTL5000_DAP_AVC_DECAY); //SGTL5000_DAP_AVC_DECAY
	sgtl5000_write_register(0x0001, SGTL5000_CHIP_DAP_AVC_CTRL); //SGTL5000_DAP_AVC_CTRL

	/*-------------------------CLK setup--------------------------------*/
	/* MCLK & SampleCLK */
	// 0x0008 Configure SYS_FS clock to 48 kHz, MCLK_FREQ to 256*Fs
	// 0x0000 Configure SYS_FS clock to 32 kHz, MCLK_FREQ to 256*Fs
	sgtl5000_write_register(0x0000, SGTL5000_CHIP_CLK_CTRL);

	/*-------------------------I2S setup--------------------------------*/
	/* Configure codec as Slave */
	// 0x0000 32bits SCLK= 64*Fs (best perfomance)
	// 0x0080 32bits SCLK= 64*Fs SCLK falling edge
	// 0x0170 16bits SCLK= 32*Fs SCLK falling edge
	sgtl5000_write_register(0x0000, SGTL5000_CHIP_I2S_CTRL);

	/*---------------- Input Volume Control---------------------*/
	// Configure ADC left and right analog volume to desired default.
	// Example shows volume of 0dB
	sgtl5000_write_register(0x0000, SGTL5000_CHIP_ANA_ADC_CTRL);

	// Configure MIC gain if needed (D)
	// Configure MIC bias voltage (C)
	// Configure MIC bias resistor (B)
	sgtl5000_write_register(0x3B2, SGTL5000_CHIP_MIC_CTRL);

	/*---------------- Volume and Mute Control--------------------------*/
	// Configure HP_OUT left and right volume to minimum, unmute
	// HP_OUT and ramp the volume up to desired volume.
	sgtl5000_write_register(0x7F7F, SGTL5000_CHIP_ANA_HP_CTRL);
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 4U); // bit 4

	// Code assumes that left and right volumes are set to same value
	// So it only uses the left volume for the calculations
	uint8_t currentVol = 0x7F;
	uint8_t finalVol = 0x10;
	uint8_t numSteps = abs(currentVol - finalVol);
	uint16_t currentVolLR;

	// Ramp up
	if (numSteps > 0U) {
		for(uint16_t i = 0U; i < numSteps; i++) {
			--currentVol;
			currentVolLR = (currentVol << 8) | (currentVol);
			sgtl5000_write_register(currentVolLR, SGTL5000_CHIP_ANA_HP_CTRL);
		}
	}

	// Configure DAC left and right digital volume. Example shows
	// volume of 0dB
	sgtl5000_write_register(0x3C3C, SGTL5000_CHIP_DAC_VOL);
	sgtl5000_write_register(0x0200, SGTL5000_CHIP_ADCDAC_CTRL);

	// LINEOUT and DAC volume control
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 8U);

	// Unmute ADC
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 0U); // bit 0
}

void sgtl5000_init_Line_in_AVC_Line_out_32K() {
	/*--------------- Power Supply Configuration------------------------*/
	// NOTE: This next 2 Write calls is needed ONLY if VDDD is
	// internally driven by the chip
	// Configure VDDD level to 1.2V (bits 3:0)
	sgtl5000_write_register(0x0008, SGTL5000_CHIP_LINREG_CTRL);
	// Power up internal linear regulator (Set bit 9)
	sgtl5000_write_register(0x7260, SGTL5000_CHIP_ANA_POWER);
	// NOTE: The next modify call is only needed if both VDDA and
	// VDDIO are greater than 3.1V
	// Configure the charge pump to use the VDDIO rail (set bit 5 and bit 6)
	sgtl5000_modify_register(SGTL5000_CHIP_LINREG_CTRL, 0b11, 2U, 5U);

	/*------ Reference Voltage and Bias Current Configuration-----------*/
	// NOTE: The value written in the next 2 Write calls is dependent
	// on the VDDA voltage value.
	// Set ground, ADC, DAC reference voltage (bits 8:4). The value should
	// be set to VDDA/2. This example assumes VDDA = 1.8V. VDDA/2 = 0.9V.
	// The bias current should be set to 50% of the nominal value (bits 3:1)
	sgtl5000_write_register(0x01E0, SGTL5000_CHIP_REF_CTRL);

	// Set LINEOUT reference voltage to VDDIO/2 (1.65V) (bits 5:0) and bias current (bits 11:8) to
	// the recommended value of 0.36mA for 10kOhm load with 1nF capacitance
	sgtl5000_write_register(0x0322, SGTL5000_CHIP_LINE_OUT_CTRL);

	/*----------------Other Analog Block Configurations-----------------*/
	// Configure slow ramp up rate to minimize pop (bit 0)
	sgtl5000_modify_register(SGTL5000_CHIP_REF_CTRL, 0b1, 1U, 0U);
	// Enable short detect mode for headphone left/right
	// and center channel and set short detect current trip level
	// to 75mA
	sgtl5000_write_register(0x1106, SGTL5000_CHIP_SHORT_CTRL);
	// Enable Zero-cross detect if needed for HP_OUT (bit 5) and ADC (bit 1)
	sgtl5000_write_register(0x0133, SGTL5000_CHIP_ANA_CTRL);

	/*----------------Power up Inputs/Outputs/Digital Blocks------------*/
	// Power up HP, ADC, DAC, CHARGEPUMP
	sgtl5000_write_register(0x6AFB, SGTL5000_CHIP_ANA_POWER);
	// Power up LINE_out
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_POWER, 0b1, 1U, 0U);

	// Power up desired digital blocks
	// DAC, ADC, I2S_IN, I2S_OUT, DAP
	sgtl5000_write_register(0x0073, SGTL5000_CHIP_DIG_POWER);

	/*--------------------Set LINEOUT Volume Level-----------------------*/
	// Set the LINEOUT volume level based on voltage reference (VAG)
	// values using this formula
	// Value = (int)(40*log(VAG_VAL/LO_VAGCNTRL) + 15)
	// Assuming VAG_VAL and LO_VAGCNTRL is set to 0.9V and 1.65V respectively, the
	// left LO volume (bits 12:8) and right LO volume (bits 4:0) value should be set
	// to 5
	sgtl5000_write_register(0x0505, SGTL5000_CHIP_LINE_OUT_VOL);

	/*-------------------------I/O routing------------------------------*/
	// Example 2: LINE_IN -> ADC -> I2S_OUT, I2S_IN -> DAC -> HP_OUT
	// Set ADC input to LINE_IN
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b1, 1U, 2U); // bit 2
	// Route ADC to I2S_OUT, I2S_IN to DAC
	sgtl5000_write_register(0x0010, SGTL5000_CHIP_SSS_CTRL);
	// Set DAC output to HP_OUT
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 0U, 6U); // bit 6

	/*-------------------------DAP setup--------------------------------*/
	/*-------------------------CLK setup--------------------------------*/
	/* MCLK & SampleCLK */
	// 0x0008 Configure SYS_FS clock to 48 kHz, MCLK_FREQ to 256*Fs
	// 0x0000 Configure SYS_FS clock to 32 kHz, MCLK_FREQ to 256*Fs
	sgtl5000_write_register(0x0000, SGTL5000_CHIP_CLK_CTRL);

	/*-------------------------I2S setup--------------------------------*/
	/* Configure codec as Slave */
	// 0x0000 32bits SCLK= 64*Fs (best perfomance)
	// 0x0080 32bits SCLK= 64*Fs SCLK falling edge
	// 0x0170 16bits SCLK= 32*Fs SCLK falling edge
	sgtl5000_write_register(0x0000, SGTL5000_CHIP_I2S_CTRL);

	/*---------------- Input Volume Control---------------------*/
	// Configure ADC left and right analog volume to desired default.
	// Example shows volume of 0dB
	sgtl5000_write_register(0x0000, SGTL5000_CHIP_ANA_ADC_CTRL);

	// Configure MIC gain if needed (D)
	// Configure MIC bias voltage (C)
	// Configure MIC bias resistor (B)
	sgtl5000_write_register(0x0000, SGTL5000_CHIP_MIC_CTRL); // Desactivado

	/*---------------- Volume and Mute Control--------------------------*/
	// Configure HP_OUT left and right volume to minimum, unmute
	// HP_OUT and ramp the volume up to desired volume.
	sgtl5000_write_register(0x7F7F, SGTL5000_CHIP_ANA_HP_CTRL);
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 4U); // bit 4

	// Code assumes that left and right volumes are set to same value
	// So it only uses the left volume for the calculations
	uint8_t currentVol = 0x7F;
	uint8_t finalVol = 0x18;
	uint8_t numSteps = abs(currentVol - finalVol);
	uint16_t currentVolLR;

	// Ramp up
	if (numSteps > 0U) {
		for(uint16_t i = 0U; i < numSteps; i++) {
			--currentVol;
			currentVolLR = (currentVol << 8) | (currentVol);
			sgtl5000_write_register(currentVolLR, SGTL5000_CHIP_ANA_HP_CTRL);
		}
	}

	// Configure DAC left and right digital volume. Example shows
	// volume of 0dB
	sgtl5000_write_register(0x3C3C, SGTL5000_CHIP_DAC_VOL);
	sgtl5000_write_register(0x0200, SGTL5000_CHIP_ADCDAC_CTRL);

	// LINEOUT and DAC volume control
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 8U);

	// Unmute ADC
	sgtl5000_modify_register(SGTL5000_CHIP_ANA_CTRL, 0b0, 1U, 0U); // bit 0
}
