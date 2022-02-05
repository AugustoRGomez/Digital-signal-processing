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
#include "samples_q15.h"

///* ----------------------------------------------------------------------
//** TODO: Defines
//** ------------------------------------------------------------------- */
#define frec 60000000
#define sizeRate 5
#define dcOffset 2020
#define fftPoints 512

/* ----------------------------------------------------------------------
** TODO: Variables y Buffers
** ------------------------------------------------------------------- */
//8KHz (125us), 16KHz (63us), 22KHz (45us), 44KHz (23us), 48KHz (21u)
long rate[sizeRate]={	USEC_TO_COUNT(125u,frec),
						USEC_TO_COUNT(63u,frec),
						USEC_TO_COUNT(45u,frec),
						USEC_TO_COUNT(23u,frec),
						USEC_TO_COUNT(21u,frec)};
char contRate=0;

q15_t buffIn[fftPoints];
q15_t buffAux[fftPoints];
q15_t buffOut[fftPoints];
q15_t buffOutFFT[fftPoints];
q15_t buffFFT[fftPoints*2];

volatile bool byPass=true;
volatile bool uartEnable= false;

int cy0=0;
int cy=0;
uint32_t primask;

const uint8_t sendWave[]= ".WAV";
const uint8_t sendFFT[]= ".FFT";

/* ----------------------------------------------------------------------
** TODO: Punteros
** ------------------------------------------------------------------- */
// FFT
q15_t *pBuffIn, *pBuffOut, *fBuffIn, *fBuffOut, *cont, *finCont;
q15_t *pBuffFFT, *fBuffFFT, *pBuffOutFFT, *fBuffOutFFT;
long *pRate, *fRate;


/* ----------------------------------------------------------------------
** TODO: Instancias
** ------------------------------------------------------------------- */
arm_rfft_instance_q15 fft512;

/* ----------------------------------------------------------------------
** TODO: Rutinas de Interrupcion
** ------------------------------------------------------------------- */

/* TODO: Enviar datos */
void enviarDatos(q15_t *p, q15_t *pFin) {
	uint8_t data[2];

	while (uartEnable) {
		data[0]= (uint8_t)(*p & 0x00FF);
		data[1]= (uint8_t)((*p >> 8) & 0x00FF);
		UART_WriteBlocking(UART0, &data[0], 2);

		if(p== pFin) {
			p= pFin;
			uartEnable= false;
		}
		else p++;
	}

}

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

/* TODO: PORTA_IRQn interrupt handler */
void GPIOA_IRQHANDLER(void) {
  /* Get pin flags */
  uint32_t pin_flags = GPIO_PortGetInterruptFlags(GPIOA);

  /* Place your interrupt code here */
  mostrarRate();
  uartEnable= true;

  /* Clear pin flags */
  GPIO_PortClearInterruptFlags(GPIOA, pin_flags);

  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}

/* TODO: PORTC_IRQn interrupt handler */
void GPIOC_IRQHANDLER(void) {
  /* Get pin flags */
  uint32_t pin_flags = GPIO_PortGetInterruptFlags(GPIOC);

  /* Place your interrupt code here */
  byPass= !(byPass);

  /* Clear pin flags */
  GPIO_PortClearInterruptFlags(GPIOC, pin_flags);

  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}

/* UART0_RX_TX_IRQn interrupt handler */
void UART0_SERIAL_RX_TX_IRQHANDLER(void) {
  uint8_t config[4];
  uint8_t readS1, readD;

  /* Reading all interrupt flags of status registers */
  readS1= UART0->S1;

  /* Place your code here */
  if(readS1 == 0b11110000){
	  UART_ReadBlocking(UART0, &config[0], 4);
	  if(config[0]== 46) {
		  if(config[1]== sendWave[1]) byPass= true;
		  else if(config[1]== sendFFT[1]) byPass= false;
		  uartEnable= true;
	  }

	  /* De esta forma se limpia el flag RxFullReg */
	  readS1= UART0->S1;
	  readD= UART0->D;
  }

  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}

/* ----------------------------------------------------------------------
** TODO: Main
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

    //inicializa punteros y finales
	//nombre del array, equivale a la direccion del primer elemento del array
	pRate=rate; fRate=&rate[sizeRate-1];

	pBuffIn=&buffIn[0]; fBuffIn=&buffIn[fftPoints-1];
	pBuffOut=&buffOut[0]; fBuffOut=&buffOut[fftPoints-1];
	pBuffOutFFT=&buffOutFFT[0]; fBuffOutFFT=&buffOutFFT[fftPoints-1];

	//inicializar FFT
	arm_rfft_init_q15(&fft512, fftPoints, 0, 1);

	//trasladar todas las muestras al buffer de entrada
	for(int i=0;i<fftPoints ;i++)
		buffIn[i]= sineTable[i];

	//ventana hanning, by elDarius
	q15_t wHanning[fftPoints];
	for(int i=0;i<fftPoints ;i++)
		wHanning[i]=(q15_t)(0.5f*32700.0f*(1.0f-cosf(2.0f*PI*i/512)));

	//multiplicar por hanning
	arm_mult_q15(&wHanning[0], &buffIn[0], &buffOut[0], fftPoints);

	//paso intermedio
	arm_mult_q15(&wHanning[0], &buffIn[0], &buffAux[0], fftPoints);

	//calcular FFT
	arm_rfft_q15(&fft512, &buffAux[0], &buffFFT[0]);

	//FFT512 UPSCALE 8
	for(int i=0;i<2*fftPoints;i++)
		buffFFT[i]=buffFFT[i]<<8;

	//modulo de la fft al buffOut
	arm_cmplx_mag_q15(&buffFFT[0], &buffOutFFT[0], fftPoints);

    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {

    		if(uartEnable) {
    			if(byPass)
    				enviarDatos(pBuffOut,fBuffOut);
    			else
    				enviarDatos(pBuffOutFFT, fBuffOutFFT);
    		}
    	}

    return 0 ;
}
