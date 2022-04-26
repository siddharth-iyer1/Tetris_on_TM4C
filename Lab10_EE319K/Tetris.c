//Tetris
// Siddharth Iyer & Kyle Le

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Print.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer1.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds

void Timer1A_Handler(void){ // can be used to perform tasks in background
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
   // execute user task
}

typedef enum {English, Spanish, Portuguese, French} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Hello_Portuguese[] = "Ol\xA0";
const char Hello_French[] ="All\x83";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Goodbye_Portuguese[] = "Tchau";
const char Goodbye_French[] = "Au revoir";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais";
const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};

void SysTick_Init(uint32_t period){
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = period-1;
	NVIC_ST_CURRENT_R =0;
	NVIC_SYS_PRI3_R = 0x20000000 | ((0x00FFFFFF) & (NVIC_SYS_PRI3_R)); // set priority to 1
	NVIC_ST_CTRL_R = 0x0007;
}


void SysTick_Handler(void){
  //Handle things
}

void PortD_Init(void){
  volatile int delay;
  SYSCTL_RCGCGPIO_R |= 0x08;
  delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTD_DIR_R &= 0x00;
  GPIO_PORTD_DEN_R |= 0x0E;
}

int main(void){
//Code goes here
	DisableInterrupts();
	TExaS_Init(SCOPE);
	ST7735_InitR(INITR_REDTAB); 
  ADC_Init();         // turn on ADC, set channel to 1
	PortD_Init();
	//Initializations
	ST7735_FillScreen(0);
	EnableInterrupts();
	//Welcome Screen
	ST7735_DrawBitmap(10,90,tetrisTitle,105,80);
	ST7735_SetCursor(2,10);
	ST7735_SetTextColor(ST7735_CYAN);

	ST7735_OutString("Welcome to Tetris!");
	ST7735_SetCursor(3,13);
	ST7735_OutString("Press the Left");
	ST7735_SetCursor(3,14);
	ST7735_OutString("Button to play");

	while((GPIO_PORTD_DATA_R && 0x01) == 0){
	
	}
	uint32_t score = 5000;
	ST7735_FillScreen(0);
	ST7735_FillRect(0,146,180,1,ST7735_BLUE);
	ST7735_SetCursor(0,15);
	ST7735_DrawString(0,15,"Score: ", ST7735_CYAN);

	while(1){
		/*
		Game Engine goes here
		*/

		ST7735_SetCursor(6,15);
		LCD_OutDec(1538*ADC_In()/4095+176); //Not actually the score, just me testing ADC
		ST7735_OutString("    ");
		//Game Over, restart
		
	}
}

