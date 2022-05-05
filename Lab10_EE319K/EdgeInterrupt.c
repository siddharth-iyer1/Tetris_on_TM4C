#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "EdgeInterrupt.h"
#include "Tetris.h"
//Includes debounce

// EdgeInterrupt.c


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

// global variable visible in Watch window of debugger
// increments at least once per button press
volatile uint32_t FallingEdges = 0;
volatile uint32_t FallingEdges1 = 0;
void EdgeCounter_Init(void){                          
  SYSCTL_RCGCGPIO_R |= 0x00000010; // (a) activate clock for port E
	SYSCTL_RCGCGPIO_R |= 0x00000020;
  FallingEdges = 0;             // (b) initialize counter
  GPIO_PORTE_DIR_R &= ~0x06;    // (c) make PE2 in (built-in button)
  GPIO_PORTE_AFSEL_R &= ~0x06;  //     disable alt funct on PE2
  GPIO_PORTE_DEN_R |= 0x06;     //     enable digital I/O on PE2   
  GPIO_PORTE_PCTL_R &= ~0x00000F00; // configure PE2 &PE1 as GPIO
  GPIO_PORTE_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTE_PDR_R |= 0x06;     //     enable weak pull-up on PF4
  GPIO_PORTE_IS_R &= ~0x06;     // (d) PF4 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x06;    //     PF4 is not both edges
  GPIO_PORTE_IEV_R |= 0x06;    //     PF4 rising edge event
  GPIO_PORTE_ICR_R = 0x06;      // (e) clear flag4
  GPIO_PORTE_IM_R |= 0x06;      // (f) arm interrupt on PE2 
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x00000010;      // (h) enable interrupt 4 in NVIC
	
	
	GPIO_PORTF_DIR_R &= ~0x10;    // (c) make PF4 in (built-in button)
	GPIO_PORTF_AFSEL_R &= ~0x10;  //     disable alt funct on PE2
  GPIO_PORTF_DEN_R |= 0x10;     //     enable digital I/O on PF4
  GPIO_PORTF_PCTL_R &= ~0x0F000000; // configure PE2 &PE1 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PDR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x10;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R |= 0x10;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x10;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4
  NVIC_PRI7_R |= (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R |= 0x40000000;      // (h) enable interrupt 30 in NVIC
  EnableInterrupts();           // (i) Clears the I bit
}
void GPIOPortE_Handler(void){
  GPIO_PORTE_ICR_R = 0x04;      // acknowledge flag4
	void rotateBlock(void);
	int i = 2000000;
	while(i>0){
		i--;
	}
}

void GPIOPortF_Handler(void){
  GPIO_PORTF_ICR_R = 0x10;      // acknowledge flag4
	void blockDrop(void);
	int i = 2000000;
	while(i>0){
		i--;
	}
}

//debug code
int main(void){
  EdgeCounter_Init();           // initialize GPIO Port F interrupt
  while(1){
    WaitForInterrupt();
  }
}
