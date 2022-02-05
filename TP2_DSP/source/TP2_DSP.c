
/******* TP2_DSP *******/

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
#include "LP_8k.h"
#include "samples_q15.h"

///* ----------------------------------------------------------------------
//** Variables
//** ------------------------------------------------------------------- */
q15_t* cont;
q15_t* finCont; //ojo q15_t* cont,finCont toma a finCont de 16bits!
char contRate=0;
bool adcon=false;
bool flag= 1;

long* pRate;
long* finRate;

//pit frec 60MHz

#define frec 60000000
//8KHz (125us), 16KHz (63us), 22KHz (45us), 44KHz (23us), 48KHz (21u)

long rate[5]={		USEC_TO_COUNT(100000u,frec),
					USEC_TO_COUNT(63u,frec),
					USEC_TO_COUNT(45u,frec),
					USEC_TO_COUNT(23u,frec),
					USEC_TO_COUNT(21u,frec)};
//USEC_TO_COUNT(125u,frec)
/* ----------------------------------------------------------------------
** Parametros para los filtros FIR
** ------------------------------------------------------------------- */
#define TEST_LENGTH_SAMPLES  512
#define BLOCK_SIZE           512
#define NUM_TAPS              64

static q15_t testOutput[TEST_LENGTH_SAMPLES]; //Output buffer
static q15_t firStateF32[BLOCK_SIZE + NUM_TAPS - 1]; //State Buffer

uint16_t blockSize = BLOCK_SIZE;
uint16_t numBlocks = TEST_LENGTH_SAMPLES/BLOCK_SIZE;

arm_fir_instance_q15 S;

//punteros y otras var
q15_t  *inputF32, *outputF32;
int8_t lowByte, highByte;


/* ----------------------------------------------------------------------
** Rutinas de Interrupcion
** ------------------------------------------------------------------- */
/**
   * @brief Rutina encargada de segmentar los datos en bytes y enviarlos
*/
void enviarDatos(){
	lowByte= (int8_t)(*cont & 0x00FF); //0x1D95
	highByte= (int8_t)(*cont >> 8);
	UART_WriteByte(UART0, highByte);
	UART_WriteByte(UART0, lowByte);
	if(cont==finCont) {
		cont= &sineTable[0];
//		PIT_StopTimer(PIT, kPIT_Chnl_0);;
	}
	else cont++;
}
/**
   * @brief Actualizar color del LED
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

/**
   * @brief El SW2 envia los datos via UART
*/
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

/**
   * @brief El SW3 cambia el color del LED
*/
void GPIOA_IRQHANDLER(){
	//bajar banderas y seleccionar y mostrar sample rate
	GPIO_PortClearInterruptFlags(GPIOA, 1u<<4u);
	if(pRate==finRate) pRate=&rate;
	else pRate++;
	PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, (int)*pRate);
	mostrarRate();
	__DSB();
}

void PIT_CHANNEL_0_IRQHANDLER(){
	//bajar banderas e iniciar conversion
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
//	uint32_t interrupciones = DisableGlobalIRQ();
	enviarDatos();
//	EnableGlobalIRQ(interrupciones);
	__DSB();
}

/* ----------------------------------------------------------------------
** Main
** ------------------------------------------------------------------- */
int main(void) {
  	/* Init board hardware. */
	BOARD_InitPins();
	BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
	BOARD_InitDEBUG_UARTPins(); //NO OLVIDARSE DE ESTE!
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif
    mostrarRate();

    /* Inicializar punteros */
    cont= &sineTable[510];
    finCont= &sineTable[(sizeof(sineTable)/sizeof(sineTable[0])-1)];
    pRate= &rate[0];
    finRate= &rate[(sizeof(rate)/sizeof(rate[0])-1)];


    /* Initialize input and output buffer pointers */
    inputF32 = &sineTable[0];
    outputF32 = &testOutput[0];

    /* Call FIR init function to initialize the instance structure. */
    arm_fir_init_q15(&S, NUM_TAPS, &N[0] ,&firStateF32[0], blockSize);
    arm_fir_q15(&S, inputF32, outputF32, blockSize);

    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {}
    return 0 ;
}

