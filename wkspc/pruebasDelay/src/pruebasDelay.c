/*
===============================================================================
 Name        : pruebasDelay.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <lpc17xx_clkpwr.h>
#include <lpc17xx_pinsel.h>
#include <lpc17xx_gpio.h>
#include <lpc17xx_dac.h>
#include <lpc17xx_rit.h>
#include <lpc17xx_exti.h>
#include <lpc17xx_adc.h>
#include <stdbool.h>

/*
 * //////////////////////////////////////////////////////////////////////////////////////////////////////////////
 * xxx Variables/parametros.
 * //////////////////////////////////////////////////////////////////////////////////////////////////////////////
 */
#define fs 16000
#define suavizado 4
#define depth 512
#define offset 2000

//salida, buffer de muestras
unsigned int ynn;
int fifo[depth];

//flag de efecto
bool flgFx=FALSE;

volatile unsigned int average;

/*
 * //////////////////////////////////////////////////////////////////////////////////////////////////////////////
 * xxx Funciones auxiliares.
 * //////////////////////////////////////////////////////////////////////////////////////////////////////////////
 */
//config DAC
void cfgDac(){
	//P0.26 a DAC (habilita bloque)
	PINSEL_CFG_Type cfgP026={
		.Funcnum=2,
		.OpenDrain=PINSEL_PINMODE_NORMAL,
		.Pinmode=PINSEL_PINMODE_TRISTATE,
		.Pinnum=26,
		.Portnum=0
	};
	PINSEL_ConfigPin(&cfgP026);

	//inicializar DAC, clk del bloque a 25MHz (default)
	DAC_Init(LPC_DAC);

	//DAC max-rate 1MHz (default)
	DAC_SetBias(LPC_DAC, 0);

	return;
}

//config RIT
void cfgRit(){
	//inicializar bloque RIT, apagado
	RIT_Init(LPC_RIT);
	RIT_Cmd(LPC_RIT, DISABLE);

	/* MUETREO a 8KHz -> 125us
	 * calculate compare value for RIT to generate interrupt at
	 * specified time interval
	 * time_interval = COMPVAL/RIT_PCLK -> COMPVAL = time_interval*RIT_PCLK = 1562.5
	 */
	LPC_RIT->RICOMPVAL = 1563<<1;

//	/* MUESTREO a 16KHz, promedio de 4 muestras -> 64KHz = 15.6us
//	 * calculate compare value for RIT to generate interrupt at
//	 * specified time interval
//	 * time_interval = COMPVAL/RIT_PCLK -> COMPVAL = time_interval*RIT_PCLK = 1562.5
//	 */
//	LPC_RIT->RICOMPVAL = 390;

	//limpiar en cada match
	LPC_RIT->RICTRL|=(1<<1);

	//seguir corriendo en debug
	RIT_TimerDebugCmd(LPC_RIT, ENABLE);

	//bajar banderas y cargar en NVIC
	NVIC_ClearPendingIRQ(RIT_IRQn);
	NVIC_EnableIRQ(RIT_IRQn);

	return;
}

//config EINT
void cfgEint0(){
	//P2.10 a EINT0
	PINSEL_CFG_Type cfgP210={
		.Funcnum=1,
		.OpenDrain=PINSEL_PINMODE_NORMAL,
		.Pinmode=PINSEL_PINMODE_PULLUP,
		.Pinnum=10,
		.Portnum=2
	};
	PINSEL_ConfigPin(&cfgP210);

	//inicializar bloque EINT
	EXTI_Init();

	//EINT0, por flanco, bajada
	EXTI_InitTypeDef cfgEINT0={
		.EXTI_Line=EXTI_EINT0,
		.EXTI_Mode=EXTI_MODE_EDGE_SENSITIVE,
		.EXTI_polarity=EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE
	};
	EXTI_Config(&cfgEINT0);

	//bajar banderas y cargar en NVIC
	EXTI_ClearEXTIFlag(EXTI_EINT0);
	NVIC_ClearPendingIRQ(EINT0_IRQn);
	NVIC_EnableIRQ(EINT0_IRQn);

	return;
}

//config ADC
void cfgAdc(){
	//P0.23 a AD0
	PINSEL_CFG_Type cfgP023={
		.Funcnum=1,
		.OpenDrain=PINSEL_PINMODE_NORMAL,
		.Pinmode=PINSEL_PINMODE_TRISTATE,
		.Pinnum=23,
		.Portnum=0
	};
	PINSEL_ConfigPin(&cfgP023);

	//inicializar bloque ADC, forzar ADCLK a 12.5MHz (5 us/sample aprox.)
	ADC_Init(LPC_ADC, (int)(12.5E6/65));

	//habilitar ch0, disparo por soft
	ADC_ChannelCmd(LPC_ADC, 0, ENABLE);

	//interrupcion por ch0
	/*
	 * POR DEFAULT ESTA HABILITADA LA INTERRUPCION POR GLOBALDATA
	 */
	ADC_IntConfig(LPC_ADC, ADC_ADGINTEN, DISABLE);
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN0, ENABLE);

	//bajar banderas y cargar en NVIC
	NVIC_ClearPendingIRQ(ADC_IRQn);
	NVIC_EnableIRQ(ADC_IRQn);

	return;
}

/*
 * //////////////////////////////////////////////////////////////////////////////////////////////////////////////
 * xxx Don main.
 * //////////////////////////////////////////////////////////////////////////////////////////////////////////////
 */
int main(void) {
	/*
	 * Inicializar sistema
	 */
	SystemInit();

    /*
     * Iniciar perisfericos
     */
    cfgDac();
    cfgRit();
    cfgEint0();
    cfgAdc();

    //activar RIT, muestreo a 16KHz
    RIT_Cmd(LPC_RIT, ENABLE);

    while(1) {
    	//proximamente... nada...
    }

    return 0 ;
}

/*
 * //////////////////////////////////////////////////////////////////////////////////////////////////////////////
 * xxx Handlers.
 * //////////////////////////////////////////////////////////////////////////////////////////////////////////////
 */
void RIT_IRQHandler(){
	//bajar bandera (lee pero tambien baja la bandera)
	RIT_GetIntStatus(LPC_RIT);

	//tomar otra muestra
	ADC_StartCmd(LPC_ADC, ADC_START_NOW);

	return;
}

void EINT0_IRQHandler(){
	//bajar bandera
	EXTI_ClearEXTIFlag(EXTI_EINT0);

	//cambiar flag de efecto
	flgFx=!flgFx;

	return;
}

void ADC_IRQHandler(){

	//recuperar muestra
	average=ADC_GetData(0);

	//push a fifo
	for(int i=depth-1-1; i>=0; i--){
		fifo[i+1]=fifo[i];
	}
	//atenuar muestras con delay
	fifo[0]=average>>1;

	//si efecto, calcular delay
	if(flgFx){
		//muestra mas ultima posicion de la fifo
		unsigned int yn=((average+fifo[depth-1])>>2)&0x3FF;

		//saturar a 10bits
		if(yn>1204-1) yn=1024-1;

		DAC_UpdateValue(LPC_DAC, yn);
	}
	//sino, bypass
	else{
		DAC_UpdateValue(LPC_DAC, ((average)>>2)&0x3FF);
	}

	return;
}
