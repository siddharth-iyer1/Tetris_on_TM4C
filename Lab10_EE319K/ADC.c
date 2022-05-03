// ADC.c
// Runs on TM4C123
// Provide functions that initialize ADC0
// Last Modified: 1/12/2021  
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
// Labs 8 and 9 specify PD2
#include <stdint.h>
#include "tm4c123gh6pm.h"

// ADC initialization function using PD2 
// Input: none
// Output: none
void ADC_Init(void){ 
	SYSCTL_RCGCADC_R |= 0x01;
	SYSCTL_RCGCGPIO_R |= 0x08;
	while((SYSCTL_PRGPIO_R&0x08) != 0x08){};
	GPIO_PORTD_DIR_R &= ~0x04;
	GPIO_PORTD_AFSEL_R |= 0x04;
	GPIO_PORTD_DEN_R &= ~0x04;
	GPIO_PORTD_AMSEL_R |= 0x04;
	ADC0_PC_R &= ~0x0F;
	ADC0_PC_R |= 0x01;
	ADC0_SSPRI_R = 0x123;
	ADC0_ACTSS_R &= ~0x0008;
	ADC0_EMUX_R &= ~0xF000;
	ADC0_SSMUX3_R &= ~0x000F;
	ADC0_SSMUX3_R += 5;
	ADC0_SSCTL3_R = 0x0006;
	ADC0_IM_R &= ~0x0008;
	ADC0_ACTSS_R |= 0x0008;
	ADC0_SAC_R = 5;
}
//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){
	uint32_t result=0;
	ADC0_PSSI_R = 0x0008;
	while((ADC0_RIS_R&0x08)==0){};
	result = ADC0_SSFIFO3_R&0x0FFF;
	ADC0_ISC_R = 0x0008;
	return result;
}


void leftButtonInit(void){
	SYSCTL_RCGCADC_R |= 0x0010;   // 1) activate ADC0
  SYSCTL_RCGCGPIO_R |= 0x10;    // 2) activate clock for Port E
  while((SYSCTL_PRGPIO_R&0x10) != 0x10){};  // 3 for stabilization
  GPIO_PORTE_DIR_R &= ~0x01;    // 4) make PE0 input
  GPIO_PORTE_AFSEL_R |= 0x01;   // 5) enable alternate function on PE0
  GPIO_PORTE_DEN_R &= ~0x01;    // 6) disable digital I/O on PE0
  GPIO_PORTE_AMSEL_R |= 0x01;   // 7) enable analog functionality on PE0
}

void rightButtonInit(void){
  GPIO_PORTE_DIR_R &= ~0x02;    // 4) make PE1 input
  GPIO_PORTE_AFSEL_R |= 0x02;   // 5) enable alternate function on PE1
  GPIO_PORTE_DEN_R &= ~0x02;    // 6) disable digital I/O on PE1
  GPIO_PORTE_AMSEL_R |= 0x02;   // 7) enable analog functionality on PE1
}

uint32_t buttonIn(void){
	uint32_t buttonVal=0;
	ADC1_PSSI_R = 0x0008;
	while((ADC1_RIS_R&0x08)==0){};
	buttonVal = ADC0_SSFIFO3_R&0x0FFF;
	ADC0_ISC_R = 0x0008;
	return buttonVal;
}
