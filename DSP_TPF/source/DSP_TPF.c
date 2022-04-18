/*
 * TRABAJO FINAL DSP (FCEFYN, UNC)
 * Autores:
 * 	-Gomez, Augusto
 * 	-Alaniz, Dario
 * 	-Ferraris, Domingo
 *
 * PINES:
 *  FRDM-k64F:
 * 	 I2C:
 * 	  -SDA : J2[18]
 * 	  -SCLK : J2[20]
 * 	 I2S:
 * 	  -MCLK : J1[7]
 * 	  -RX_BCLK : J1[9]
 * 	  -RX_D : J1[15]
 * 	  -RX_FS : J1[13]
 *
 * 	TWR-ELEV:
 * 	 I2C:
 * 	  -SDA : J8[8]
 * 	  -SCLK : J8[7]
 * 	 I2S:
 * 	  -MCLK: J8[21]
 * 	  -BLCK: J8[22]
 * 	  -RX_D: J8[24]
 *	  -FS:   J8[23]
 *
 *	 NOTAS:
 *	 Los datos del ADC son SIGNADOS.
 */

#ifndef MAIN_INCLUDE
#define MAIN_INCLUDE

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "arm_math.h"
#include "testFilter.h"
#include "LPFilter32K.h"

#endif /* MAIN_INCLUDE */

/* TODO: insert other include files here. */
#include "sgtl5000.h"

/* TODO: insert other definitions and declarations here. */
/* Defines */
#define BUFF_LEN 512U
#define BLOCKSIZE BUFF_LEN/4
#define NUMTAPS 12U

#define HALF_RANGE_Q31 2147483647.0f
#define FLOAT_TO_Q31(arg) (q31_t) (HALF_RANGE_Q31 * arg)
#define MAX_RANGE_Q31 (q31_t) 0x7FFFFFFF

#define DELAY_DEPTH 3200U // DelayDepht(in Samples) = DelayInTime*SR = 100 ms * 32KHz

/* Enum & structures */
typedef enum {
	kPING,
	kPONG,
} buff_to_process;

/* Variables */
q31_t pingPongIN[BUFF_LEN];
q31_t pingPongOUT[BUFF_LEN];

q31_t *pingIn = pingPongIN;
q31_t *pingOut = pingPongOUT;
q31_t *pongIn = &pingPongIN[BUFF_LEN/2];
q31_t *pongOut = &pingPongOUT[BUFF_LEN/2];

q31_t LInBuff[BLOCKSIZE], RInBuff[BLOCKSIZE];
q31_t LOutBuff[BLOCKSIZE], ROutBuff[BLOCKSIZE];

q31_t LInBuffScaledD[BLOCKSIZE];

q31_t delayFeedBack = FLOAT_TO_Q31(0.8f);
int32_t delayDepth = DELAY_DEPTH;
int32_t readIndex = 0;
int32_t writeIndex = 0;
q31_t delayFifo[DELAY_DEPTH];
q31_t delayWetLvl = FLOAT_TO_Q31(0.9f);
q31_t delayDryLvl = FLOAT_TO_Q31(0.9f);

buff_to_process rx_proc_buffer, tx_proc_buffer;

volatile uint8_t rx_buffer_full = 0;
volatile uint8_t tx_buffer_empty = 0;

q31_t firStateR[NUMTAPS + BLOCKSIZE];
q31_t firStateL[NUMTAPS + BLOCKSIZE];

arm_fir_instance_q31 fir_instanceR;
arm_fir_instance_q31 fir_instanceL;

uint16_t readValue;

sai_transfer_t rxFer, txFer;

uint8_t syncFlag = 1;

/* Function prototypes */
void debug_printf_registers();
void process_block();

/* TODO: Main */
int main(void) {
    /* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    /*-----------------ALGORITHM BEGIN-----------------*/
    /* I2S struct init */
    rxFer.dataSize = sizeof(pingPongIN)/2;
    txFer.dataSize = sizeof(pingPongOUT)/2;

    rxFer.data = (uint8_t *) pingIn;
    txFer.data = (uint8_t *) pingOut;

    /* TEST: LP FIR filter init */
    arm_fir_init_q31(&fir_instanceR, NUMTAPS, LPFilter32K, firStateR, BLOCKSIZE);
    arm_fir_init_q31(&fir_instanceL, NUMTAPS, LPFilter32K, firStateL, BLOCKSIZE);

    /* DELAY Setup */
//    delayDryLvl = MAX_RANGE_Q31 - delayWetLvl; // dry = 1 - wet
    readIndex = writeIndex - delayDepth; // despues reemplazar delayDepth
    if(readIndex < 0)
    	readIndex += DELAY_DEPTH;

    /* SGTL5000 codec init */
    sgtl5000_init_Line_in_AVC_HP_out_32K();

    SAI_TransferReceiveEDMA(I2S0_PERIPHERAL, &I2S0_SAI_Rx_eDMA_Handle, &rxFer);

    while(1) {
    	while (!(rx_buffer_full && tx_buffer_empty)) {}
    	GPIO_PinWrite(BOARD_GPIO_pin_GPIO, BOARD_GPIO_pin_PIN, 1U);
    	process_block();
    	GPIO_PinWrite(BOARD_GPIO_pin_GPIO, BOARD_GPIO_pin_PIN, 0U);
    }

    return 0;
}

void process_block() {
	q31_t *pRx, *pTx;

	if(rx_proc_buffer == kPING)
		pRx = pingIn;
	else
		pRx = pongIn;

	if(tx_proc_buffer == kPING)
		pTx = pingOut;
	else
		pTx = pongOut;

	/* Separate L and R channel samples */
	for(uint16_t i = 0U; i < BLOCKSIZE; i++) {
		RInBuff[i] = *pRx++;
		LInBuff[i] = *pRx++;
	}

	/*
	 * Apply Digital Effect
	 * TODO:
	 * Por ahora solo L channel
	 */

	/* Scale down input samples */
	arm_scale_q31(LInBuff, FLOAT_TO_Q31(0.5f), 0U, LInBuffScaledD, BLOCKSIZE); // scale = scaleFract * 2^shift

	for(uint16_t i = 0U; i < BLOCKSIZE; i++) {
		/* Read the output of the delay at readIndex */
		q31_t y = delayFifo[readIndex];
		q31_t aux, aux2, aux3;

		/* Write the input to the delay */
		arm_mult_q31(&delayFeedBack, &y, &aux, 1U);
		arm_add_q31(&aux, &LInBuffScaledD[i], &delayFifo[writeIndex], 1U); // x + delayFeedBack*y;

		/* Create the wet/dry mix and write to the output buffer, dry = 1 - wet */
		arm_mult_q31(&delayWetLvl, &y, &aux2, 1U);
		arm_mult_q31(&delayDryLvl, &LInBuffScaledD[i], &aux3, 1U);
		arm_add_q31(&aux2, &aux3, pTx, 1U); //delayWetLvl*y + (1.0 - delayWetLvl)*x;

		/* Copy L samples in R channel */
		arm_copy_q31(pTx, (pTx + 1), 1U);
		pTx = pTx + 2;

		/* Increment the pointers and wrap if necessary */
		writeIndex++;
		if(writeIndex >= delayDepth)
			writeIndex = 0;
		readIndex++;
		if(readIndex >= delayDepth)
			readIndex = 0;
	}

	rx_buffer_full = 0;
	tx_buffer_empty = 0;
}

void I2S_RX_eDMA_callback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData) {
	if(rxFer.data == (uint8_t *) pingIn) {
		rxFer.data = (uint8_t *) pongIn;
		rx_proc_buffer = kPING;
	}

	else {
		rxFer.data = (uint8_t *) pingIn;
		rx_proc_buffer = kPONG;
	}

//	GPIO_PortToggle(BOARD_GPIO_pin_GPIO, 1 << BOARD_GPIO_pin_PIN);
	rx_buffer_full = 1;
	SAI_TransferReceiveEDMA(I2S0_PERIPHERAL, &I2S0_SAI_Rx_eDMA_Handle, &rxFer);

	if (syncFlag) {
		SAI_TransferSendEDMA(I2S0_PERIPHERAL, &I2S0_SAI_Tx_eDMA_Handle, &txFer);
		syncFlag = 0;
	}

}

void I2S_TX_eDMA_callback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData) {
	if(txFer.data == (uint8_t *) pingOut) {
		txFer.data = (uint8_t *) pongOut;
		tx_proc_buffer = kPING;
	}

	else {
		txFer.data = (uint8_t *) pingOut;
		tx_proc_buffer = kPONG;
	}

//	GPIO_PortToggle(BOARD_GPIO_pin_GPIO, 1 << BOARD_GPIO_pin_PIN);
	tx_buffer_empty = 1;
	SAI_TransferSendEDMA(I2S0_PERIPHERAL, &I2S0_SAI_Tx_eDMA_Handle, &txFer);
}

void debug_printf_registers() {
	//PowerUp
	readValue = sgtl5000_read_register(SGTL5000_CHIP_LINREG_CTRL);
	PRINTF("get sgtl5000 SGTL5000_CHIP_LINREG_CTRL: %x\n", readValue);

	readValue = sgtl5000_read_register(SGTL5000_CHIP_ANA_POWER);
	PRINTF("get sgtl5000 SGTL5000_CHIP_ANA_POWER: %x\n", readValue);

	readValue = sgtl5000_read_register(SGTL5000_CHIP_DIG_POWER);
	PRINTF("get sgtl5000 SGTL5000_CHIP_DIG_POWER: %x\n", readValue);

	// IO Routing
	readValue = sgtl5000_read_register(SGTL5000_CHIP_SSS_CTRL);
	PRINTF("get sgtl5000 SGTL5000_CHIP_SSS_CTRL: %x\n", readValue);

	// Volume
	readValue = sgtl5000_read_register(SGTL5000_CHIP_MIC_CTRL);
	PRINTF("get sgtl5000 SGTL5000_CHIP_MIC_CTRL: %x\n", readValue);

	readValue = sgtl5000_read_register(SGTL5000_CHIP_ANA_CTRL);
	PRINTF("get sgtl5000 SGTL5000_CHIP_ANA_CTRL: %x\n", readValue);

	readValue = sgtl5000_read_register(SGTL5000_CHIP_ANA_HP_CTRL);
	PRINTF("get sgtl5000 SGTL5000_CHIP_ANA_HP_CTRL: %x\n", readValue);

	readValue = sgtl5000_read_register(SGTL5000_CHIP_ANA_STATUS);
	PRINTF("get sgtl5000 SGTL5000_CHIP_ANA_STATUS: %x\n", readValue);

	readValue = sgtl5000_read_register(SGTL5000_CHIP_ADCDAC_CTRL);
	PRINTF("get sgtl5000 SGTL5000_CHIP_ADCDAC_CTRL: %x\n", readValue);

	readValue = sgtl5000_read_register(SGTL5000_CHIP_DAC_VOL);
	PRINTF("get sgtl5000 SGTL5000_CHIP_DAC_VOL: %x\n", readValue);
}
