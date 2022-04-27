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
		struct tetrisBlock{
		unsigned short blockData[5][5];
	};
	typedef struct tetrisBlock block_t;
	static block_t allBlocks[16] = {
	//<0> L block regular
	{{{0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 2, 0, 0},
		{0, 0, 1, 1, 0},
		{0, 0, 0, 0, 0},}}, 

	//<1> T block regular
	{{{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 1, 2, 1, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 0, 0, 0},}}, 
	
	//<2> I block regular
	{{{0, 0, 1, 0, 0},
		{0, 0, 2, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 0, 0, 0},}}, 
	
	//<3> Z block inverted
	{{{0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 2, 1, 0, 0},
		{0, 1, 0, 0, 0},
		{0, 0, 0, 0, 0},}}, 
	
	//<4> L Block Rotated Once
	{{{0, 0, 0, 0, 0},
		{0, 0, 0, 1, 0},
		{0, 1, 2, 1, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},}}, 

	//<5> T Block Rotated Once
	{{{0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 2, 1, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 0, 0, 0},}}, 
	
	//<6> I block rotated
	{{{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 1, 2, 1, 1},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},}}, 
	
	//<7> Z block rotated
	{{{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 1, 2, 0, 0},
		{0, 0, 1, 1, 0},
		{0, 0, 0, 0, 0},}}, 

	//<8> L Block Rotated Twice
	{{{0, 0, 0, 0, 0},
		{0, 1, 1, 0, 0},
		{0, 0, 2, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 0, 0, 0},}}, 
	
	//<9> T Block Rotated Twice
	{{{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 1, 2, 1, 0},
		{0, 0, 0, 0, 0},}}, 

	{{{0, 0, 1, 0, 0},
		{0, 0, 2, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 0, 0, 0},}}, //<10> I block regular (Rotated Twice)

	{{{0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 2, 1, 0, 0},
		{0, 1, 0, 0, 0},
		{0, 0, 0, 0, 0},}}, //<11> Z block inverted (Rotated Twice)

	{{{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 1, 2, 1, 0},
		{0, 1, 0, 0, 0},
		{0, 0, 0, 0, 0},}}, //<12> L Block Rotated Thrice

	{{{0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 1, 2, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 0, 0, 0},}}, //<13> T Block Rotated Thrice

	{{{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 1, 2, 1, 1},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},}}, //<14> I block rotated thrice
	
	{{{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 1, 2, 0, 0},
		{0, 0, 1, 1, 0},
		{0, 0, 0, 0, 0},}}, //<15> Z block rotated thrice

	};
	
	TExaS_Init(SCOPE);
	ST7735_InitR(INITR_REDTAB); 
  ADC_Init();         // turn on ADC, set channel to 1
	PortD_Init();
	//Initializations
	ST7735_FillScreen(0);

	//Welcome Screen
	ST7735_DrawBitmap(10,90,tetrisTitle,105,80);
	ST7735_SetCursor(2,10);
	ST7735_SetTextColor(ST7735_CYAN);

	ST7735_OutString("Welcome to Tetris!");
	ST7735_SetCursor(3,13);
	ST7735_OutString("Press the Left");
	ST7735_SetCursor(3,14);
	ST7735_OutString("Button to play");

	while(GPIO_PORTD_DATA_R == 0){
		ST7735_SetCursor(0,15);

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
		EnableInterrupts();
		ST7735_SetCursor(6,15);
		
		for(int x=0; x<5; x++){
			for(int y=0; y<5; y++){
				if(allBlocks[0].blockData[y][x] !=0){
					for(int x1=-3; x1<4; x1++){
						for(int y1=-3; y1<4; y1++){
							if(x1==-3 || y1 ==-3 || x1 == 3 || y1 == 3){
								ST7735_DrawPixel(x*6+x1,y*6+y1,ST7735_MAGENTA);
							}
							else{
								ST7735_DrawPixel(x*6+x1,y*6+y1,ST7735_BLUE);
							}
						}
					}
					

				}
			}
		}
		//Game Over, restart
		
	}
}

