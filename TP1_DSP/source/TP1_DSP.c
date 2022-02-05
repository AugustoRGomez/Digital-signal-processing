
/**
 * @file    TP1_DSP.c
 * @brief
 *
*/

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "arm_math.h"
#include "fsl_common.h"
#include "stdbool.h"
#include "stdint.h"
#include "filtros8k.h"
//#include "freemaster.h"
//#include "freemaster_cfg.h"
//#include "freemaster_serial.h"

/* TODO: insert other include files here. */

//pit frec 60MHz
#define frec 60000000

//8KHz (125us), 16KHz (63us), 22KHz (45us), 44KHz (23us), 48KHz (21u)
const long rate[5]={USEC_TO_COUNT(125u,frec),
					USEC_TO_COUNT(63u,frec),
					USEC_TO_COUNT(45u,frec),
					USEC_TO_COUNT(23u,frec),
					USEC_TO_COUNT(21u,frec)};

//punteros y limites para el rate
long* pRate;
long* finRate;
char contRate=0;

q15_t buffer[512];
int* pBuffer;
int* finBuffer;

volatile q15_t adcValor=0;
bool adcon=false;

/* TODO: insert other definitions and declarations here. */
/*
 * @brief   TP1_DSP
 */

void mostrarRate(){

	switch(contRate){
		case 0: LED_BLUE_OFF();
				LED_GREEN_OFF();
				LED_RED_ON();
		break;
		case 1: LED_RED_OFF();
				LED_GREEN_ON();
		break;
		case 2: LED_GREEN_OFF();
				LED_BLUE_ON();
		break;
		case 3: LED_RED_ON();
		break;
		case 4: LED_BLUE_ON();
				LED_GREEN_ON();
				LED_RED_ON();
		break;
		default: LED_BLUE_OFF();
				 LED_RED_OFF();
				 LED_GREEN_OFF();
		break;
	}
	if(contRate==4) contRate=0u;
	else contRate++;
}

void GPIOC_IRQHANDLER(){
	//bajar banderas e iniciar pit
	GPIO_PortClearInterruptFlags(GPIOC, 1u<<6u);
	if(adcon==false){
		PIT_StartTimer(PIT, kPIT_Chnl_0);
		adcon=true;
	}
	else{
		PIT_StopTimer(PIT, kPIT_Chnl_0);
		adcon=false;
	}
	__DSB();
}

void GPIOA_IRQHANDLER(){
	//bajar banderas y seleccionar y mostrar sample rate
	GPIO_PortClearInterruptFlags(GPIOA, 1u<<4u);
	if(pRate==finRate) pRate=&rate;
	else pRate++;
	PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, (int)*pRate);
	//printf("%d us.\n",COUNT_TO_USEC(*pRate,frec));
	mostrarRate();
	__DSB();
}

void PIT_CHANNEL_0_IRQHANDLER(){
	//bajar banderas e iniciar conversion
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
	ADC16_SetChannelConfig(ADC0, 0u, &ADC0_channelsConfig[0]);
	__DSB();
}

void ADC0_IRQHANDLER(){
	//bajar banderas y guardar conversion en buffer
	ADC16_ClearStatusFlags(ADC0, kADC16_ChannelConversionDoneFlag);
	*pBuffer=(q15_t)ADC16_GetChannelConversionValue(ADC0, 0u);
	adcValor=(q15_t)ADC16_GetChannelConversionValue(ADC0, 0u);
	__DSB();
}

/******* TP2_DSP *******/

int main(void) {
  	/* Init board hardware. */
	BOARD_InitPins();
	BOARD_InitButtonsPins();
	BOARD_InitLEDsPins();
	BOARD_InitDEBUG_UARTPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    /* FREEMASTER */

    pRate=&rate;
    pBuffer=&buffer;
    //finRate=&rate[4];
    finRate=&rate[(sizeof(rate)/sizeof(rate[0])-1)];
    finBuffer=&buffer[(sizeof(rate)/sizeof(rate[0])-1)];

    PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, rate[0]);
    mostrarRate();
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
    }
    return 0 ;
}

