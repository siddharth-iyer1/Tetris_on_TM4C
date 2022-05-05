#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "EdgeInterrupt.h"

//Includes debounce

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

// global variable visible in Watch window of debugger
// increments at least once per button press
volatile uint32_t FallingEdges = 0;
void EdgeCounter_Init(void){                          
  SYSCTL_RCGCGPIO_R |= 0x00000010; // (a) activate clock for port E
  FallingEdges = 0;             // (b) initialize counter
  GPIO_PORTE_DIR_R &= ~0x04;    // (c) make PE2 in (built-in button)
  GPIO_PORTE_AFSEL_R &= ~0x04;  //     disable alt funct on PE2
  GPIO_PORTE_DEN_R |= 0x04;     //     enable digital I/O on PE2   
  GPIO_PORTE_PCTL_R &= ~0x00000F00; // configure PE2 as GPIO
  GPIO_PORTE_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTE_PDR_R |= 0x04;     //     enable weak pull-up on PF4
  GPIO_PORTE_IS_R &= ~0x04;     // (d) PF4 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x04;    //     PF4 is not both edges
  GPIO_PORTE_IEV_R |= 0x04;    //     PF4 rising edge event
  GPIO_PORTE_ICR_R = 0x04;      // (e) clear flag4
  GPIO_PORTE_IM_R |= 0x04;      // (f) arm interrupt on PE2 
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x00000010;      // (h) enable interrupt 4 in NVIC
  EnableInterrupts();           // (i) Clears the I bit
}
void GPIOPortE_Handler(void){
  GPIO_PORTE_ICR_R = 0x04;      // acknowledge flag4
	FallingEdges = FallingEdges + 1;
	int i = 5000000;
	while(i>0){
		i--;
	}

}
