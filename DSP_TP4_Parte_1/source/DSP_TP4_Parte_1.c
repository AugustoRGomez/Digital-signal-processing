/*
 *	LMS Adaptative Filter
 *	Signal prediction
 *	Pagina 287- DSP_CORTEX_M4
 *	DAC: J4-11
*/

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "arm_math.h"
#include "f_imp_q15.h"
#include "planta_fir.h"
#include "prbs_gen.h" //borrar

/*
 * Algunos valores de MU en formato q15
 * 0.5 -> 0x4000
 * 0.1 -> 0x0CCC
 * 0.05 -> 0x0666 best
 * 0.01 -> 0x0147
 * 0.005 -> 0x00A3
 * 0.001 -> 0x0020
 */

#define OUT //DAC: OUT_ERROR, OUT_REF, OUT_LMS
#define HALF_RANGE_UINT_16 32767U
#define NUMTAPS 30U
#define BLOCKSIZE 1U
#define MU 0x0666
#define SAMPLES_NUM 256U
#define SAMPLES_PERIOD 100U
#define INPUT_POWER 3U //hasta 15 -> minimo valor no nulo 3 9 12 15

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

arm_lms_instance_q15 lms_instance;
arm_fir_instance_q15 fir_instance;

/* DEBUG VARIABLES */
q15_t lmsCoeffArray[NUMTAPS][SAMPLES_NUM];
q63_t mseArray[SAMPLES_NUM];
q15_t acc[SAMPLES_PERIOD];
int samplesCnt = 0;
int jj = 0;

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

    while (1) {
    	q15_t rndVal = ((q15_t) rand()) >> INPUT_POWER;

    	arm_fir_q15(&fir_instance, &rndVal, firOutBuff, BLOCKSIZE);
    	arm_lms_q15(&lms_instance, &rndVal, firOutBuff, outBuff, errBuff, BLOCKSIZE);

    	acc[samplesCnt] = *pErr;

    	if(samplesCnt == SAMPLES_PERIOD-1) {
    		for(int ii = 0; ii < NUMTAPS; ii++) {
    			lmsCoeffArray[ii][jj] = lmsCoeff[ii];
    		}
    		arm_power_q15(acc, SAMPLES_PERIOD, &mseArray[jj]);
    		mseArray[jj] = mseArray[jj]/SAMPLES_PERIOD;

    		jj++;
    		if(jj == SAMPLES_NUM)
    			jj = 0;
    		samplesCnt = 0;
    	}
    	else
    		samplesCnt++;
    }

    return 0 ;
}

//CH0 de PIT usado para controlar la tasa de actualización del DAC (opcional)
void PIT_CHANNEL_0_IRQHANDLER(void) {
	uint32_t intStatus;
	/* Reading all interrupt flags of status register */
	intStatus = PIT_GetStatusFlags(PIT_PERIPHERAL, PIT_CHANNEL_0);
	PIT_ClearStatusFlags(PIT_PERIPHERAL, PIT_CHANNEL_0, intStatus);

	/* La función DAC_setBufferValue toma los 12 bits menos significativos,
	 * por eso se shiftea 4 veces para que incluya la parte más significativa */

#ifdef OUT_REF
	DAC_SetBufferValue(DAC0_PERIPHERAL, 0u, ((uint16_t) ((*pFirOut + HALF_RANGE_UINT_16) >> 4)));
#endif
#ifdef OUT_ERROR
	DAC_SetBufferValue(DAC0_PERIPHERAL, 0u, ((uint16_t) ((*pErr + HALF_RANGE_UINT_16) >> 4)));
#endif
#ifdef OUT_LMS
	DAC_SetBufferValue(DAC0_PERIPHERAL, 0u, ((uint16_t) ((*pOut + HALF_RANGE_UINT_16) >> 4)));
#endif

	/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
	 Store immediate overlapping exception return operation might vector to incorrect interrupt. */
	#if defined __CORTEX_M && (__CORTEX_M == 4U)
	__DSB();
	#endif
}
