// Timer0.c
// Runs on LM4F120/TM4C123
// Use TIMER0 in 32-bit periodic mode to request interrupts at a periodic rate
// Daniel Valvano
// Last Modified: 11/15/2021  
// You can use this timer only if you learn how it works

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
  Program 7.5, example 7.6

 Copyright 2021 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include <stdint.h>

#include "tm4c123gh6pm.h"
#include "Sound.h"

// ***************** Timer0_Init ****************
// Activate TIMER0 interrupts to run user task periodically
// Inputs:  period in units (1/clockfreq)
//          priority is 0 (high) to 7 (low)
// Outputs: none

void(*soundplay0)();						// Sound which will be called according to Timer

void Timer0_Init(void(*task)(void), uint32_t period){	
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  soundplay0 = task;  
	TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = period-1;    // 4) reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI3_R = (NVIC_PRI3_R&0x00FFFFFF)|0x6000000; // priority 3
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
	TIMER0_CTL_R = 0x00000001;
}

void Timer0_Start(void){
	TIMER0_CTL_R |= 0x01;
}

void Timer0_Stop(void){
	TIMER0_CTL_R &= ~0x01;
}

void Timer0_Handler(void){
	TIMER0_ICR_R = TIMER_ICR_TATOCINT; //acknowledge timer0a timeout
	(*soundplay0)();
}





// ***************** Timer1_Init ****************

void(*soundplay1)();						// Sound which will be called according to Timer

void Timer1_Init(void(*task)(void), uint32_t period){	
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER0
  soundplay1 = task;  
	TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = period-1;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x8000000; // priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 19 in NVIC
	TIMER1_CTL_R = 0x00000001;
}

void Timer1_Start(void){
	TIMER1_CTL_R |= 0x01;
}

void Timer1_Stop(void){
	TIMER1_CTL_R &= ~0x01;
}

void Timer1_Handler(void){
	TIMER1_ICR_R = TIMER_ICR_TATOCINT; //acknowledge timer0a timeout
	(*soundplay1)();
}


// ***************** Timer2_Init ****************

void(*soundplay2)();						// Sound which will be called according to Timer

void Timer2_Init(void(*task)(void), uint32_t period){	
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate TIMER0
  soundplay2 = task;  
	TIMER2_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x0000A000; // priority 5
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 19 in NVIC
	TIMER2_CTL_R = 0x00000001;
}

uint32_t timercount=0;
void Timer2A_Start(void){
	TIMER2_CTL_R |= 0x01;
}

void Timer2A_Stop(void){
	TIMER2_CTL_R &= ~0x01;
}

void Timer2A_Handler(void){
	TIMER2_ICR_R = TIMER_ICR_TATOCINT; //acknowledge timer0a timeout
	playtetris();
	timercount++;
}
