/*
 *	LMS Adaptative Filter
 *	Active Noise Cancellation
 *	Pagina 287- DSP_CORTEX_M4
 *	Pines:
 *		DAC: J4-11
*/

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "arm_math.h"
#include "planta_fir.h"
#include <math.h>

/*
 * Algunos valores de MU en formato q15
 * 0.5 -> 0x4000
 * 0.1 -> 0x0CCC
 * 0.05 -> 0x0666
 * 0.01 -> 0x0147
 * 0.005 -> 0x00A3
 * 0.001 -> 0x0020
 */

#define OUT_LMS //que muestra el DAC: OUT_ERROR, OUT_REF, OUT_LMS, OUT_FIR, OUT_SN, OUT_NOISE
#define HALF_RANGE_UINT_16 32767U
#define NUMTAPS 30U
#define BLOCKSIZE 1U
#define MU 0x0147
#define SAMPLES_NUM 256U
#define SAMPLES_PERIOD 100U
#define INPUT_POWER 1U //hasta 15 -> minimo valor no nulo 3 9 12 15

#define FREQ 700
#define SR 8000
#define SINE_BUFF_LEN 12 //SR/FREQ

q15_t inBuff[BLOCKSIZE];
q15_t refBuff[BLOCKSIZE];
q15_t outBuff[BLOCKSIZE];
q15_t errBuff[BLOCKSIZE];
q15_t lmsCoeff[NUMTAPS];
q15_t lmsState[NUMTAPS + BLOCKSIZE];
q15_t *pOut = outBuff;
q15_t *pErr = errBuff;

q15_t firState[NUMTAPS + BLOCKSIZE];
q15_t firOutBuff[BLOCKSIZE];
q15_t *pFirOut = firOutBuff;

float32_t inSignalF32[SINE_BUFF_LEN];
q15_t inSignalQ15[SINE_BUFF_LEN];

arm_lms_instance_q15 lms_instance;
arm_fir_instance_q15 fir_instance;

int samplesCnt = 0;

int main(void) {
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    arm_lms_init_q15(&lms_instance, NUMTAPS, lmsCoeff, lmsState, MU, BLOCKSIZE, 0U);
    arm_fir_init_q15(&fir_instance, NUMTAPS, planta_fir, firState, BLOCKSIZE);
    PIT_StartTimer(PIT_PERIPHERAL, PIT_CHANNEL_0);

    //Sine Wave table generation
    for(int i=0; i<SINE_BUFF_LEN; i++) {
		inSignalF32[i] = 0.5f * sinf(2.0f * PI * FREQ * i / SR);
		inSignalQ15[i] = (q15_t) (32768.0f * inSignalF32[i]);
    }

    while (1) {}

    return 0 ;
}

/* CH0 de PIT usado para controlar la tasa de actualización del DAC */
void PIT_CHANNEL_0_IRQHANDLER(void) {
	uint32_t intStatus;
	/* Reading all interrupt flags of status register */
	intStatus = PIT_GetStatusFlags(PIT_PERIPHERAL, PIT_CHANNEL_0);
	PIT_ClearStatusFlags(PIT_PERIPHERAL, PIT_CHANNEL_0, intStatus);

	/* Processing */
	q15_t rndVal = ((q15_t) rand()) >> INPUT_POWER; //divido por potencias de dos, limito la "amplitud" del ruido
	q15_t signalNoise = inSignalQ15[samplesCnt] + rndVal;

	arm_fir_q15(&fir_instance, &rndVal, firOutBuff, BLOCKSIZE);
	arm_lms_q15(&lms_instance, firOutBuff, &signalNoise, outBuff, errBuff, BLOCKSIZE);

	if(samplesCnt == SINE_BUFF_LEN-1)
		samplesCnt = 0;
	else
		samplesCnt++;

	/* DAC Output: La función DAC_setBufferValue toma los 12 bits menos significativos,
	 * por eso se shiftea 4 veces para que incluya la parte más significativa */
#ifdef OUT_REF
	DAC_SetBufferValue(DAC0_PERIPHERAL, 0u, ((uint16_t) ((inSignalQ15[samplesCnt] + HALF_RANGE_UINT_16) >> 4)));
#endif
#ifdef OUT_FIR
	DAC_SetBufferValue(DAC0_PERIPHERAL, 0u, ((uint16_t) ((*pFirOut + HALF_RANGE_UINT_16) >> 4)));
#endif
#ifdef OUT_ERROR
	DAC_SetBufferValue(DAC0_PERIPHERAL, 0u, ((uint16_t) ((*pErr + HALF_RANGE_UINT_16) >> 4)));
#endif
#ifdef OUT_LMS
	DAC_SetBufferValue(DAC0_PERIPHERAL, 0u, ((uint16_t) ((*pOut + HALF_RANGE_UINT_16) >> 4)));
#endif
#ifdef OUT_SN
	DAC_SetBufferValue(DAC0_PERIPHERAL, 0u, ((uint16_t) ((signalNoise + HALF_RANGE_UINT_16) >> 4)));
#endif
#ifdef OUT_NOISE
	DAC_SetBufferValue(DAC0_PERIPHERAL, 0u, ((uint16_t) ((rndVal + HALF_RANGE_UINT_16) >> 4)));
#endif


	/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
	 Store immediate overlapping exception return operation might vector to incorrect interrupt. */
	#if defined __CORTEX_M && (__CORTEX_M == 4U)
	__DSB();
	#endif
}
