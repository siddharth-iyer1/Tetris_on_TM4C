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
int ADC_Position(uint32_t output);
void Timer1A_Handler(void){ // can be used to perform tasks in background
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
   // execute user task
}

void GPIOPortF_Handler(void){
	if((GPIO_PORTF_RIS_R & 0x01) == 0x01){
		playsound(clearrow);									// Plays row clear sound. We can set the PortF Handler
	}																				// when we detect a row clear
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
struct tetrisBlock{
	unsigned short blockData[5][5];
	signed short bottomX;
	signed short bottomY;
	signed short offsetX[3]; //These offset values are the positions of the blocks relative to the set "center" position.
	signed short offsetY[3]; //3 in each array because total there are 4 squares that make up a Tetris block. (bottomX/Y is 1 square)
};
typedef struct tetrisBlock block_t;
static block_t allBlocks[16] = {
	//<0> L block regular
	{{{0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 2, 1, 0},
		{0, 0, 0, 0, 0},}, 3,2,
	{0,0,1},
	{-1,-2,0}},

	//<1> T block regular
	{{{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 1, 1, 1, 0},
		{0, 0, 2, 0, 0},
		{0, 0, 0, 0, 0},}, 3,2,
	{-1,0,1},
	{-1,-1,-1}
	}, 
	
	//<2> I block regular
	{{{0, 0, 1, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 2, 0, 0},
		{0, 0, 0, 0, 0},}, 3,2,
	{0,0,0},
	{-1,-2,-3}
	}, 
	
	//<3> Z block inverted
	{{{0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 1, 1, 0, 0},
		{0, 2, 0, 0, 0},
		{0, 0, 0, 0, 0},}, 3,1,
	{0,1,1},
	{-1,-1,-2}
	}, 
	
	//<4> L Block Rotated Once
	{{{0, 0, 0, 0, 0},
		{0, 0, 0, 1, 0},
		{0, 2, 1, 1, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},}, 2,1,
	{1,2,2},
	{0,0,-1}
	}, 

	//<5> T Block Rotated Once
	{{{0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 1, 1, 0},
		{0, 0, 2, 0, 0},
		{0, 0, 0, 0, 0},}, 3,2,
	{0,0,1},
	{-1,-2,-1}
	}, 
	
	//<6> I block rotated
	{{{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 2, 1, 1, 1},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},}, 2,1,
	{1,2,3},
	{0,0,0}
	}, 
	
	//<7> Z block rotated
	{{{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 2, 1, 0, 0},
		{0, 0, 1, 1, 0},
		{0, 0, 0, 0, 0},}, 2,1,
	{1,1,2},
	{0,1,1}
	}, 

	//<8> L Block Rotated Twice
	{{{0, 0, 0, 0, 0},
		{0, 1, 1, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 2, 0, 0},
		{0, 0, 0, 0, 0},}, 3,2,
	{-1,0,0},
	{-2,-2,-1}
	}, 
	
	//<9> T Block Rotated Twice
	{{{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 1, 2, 1, 0},
		{0, 0, 0, 0, 0},}, 3,2,
	{-1,0,1},
	{0,-1,0}
	}, 

	//<10> I block regular (Rotated Twice)
	{{{0, 0, 1, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 2, 0, 0},
		{0, 0, 0, 0, 0},},3,2,
	{0,0,0},
	{-1,-2,-3}
	}, 

	//<11> Z block inverted (Rotated Twice)
	{{{0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 1, 1, 0, 0},
		{0, 2, 0, 0, 0},
		{0, 0, 0, 0, 0},}, 3,1,
	{0,1,1},
	{-1,-1,-2}
	}, 

	//<12> L Block Rotated Thrice
	{{{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 1, 1, 1, 0},
		{0, 2, 0, 0, 0},
		{0, 0, 0, 0, 0},}, 3,1,
	{0,1,2},
	{-1,-1,-1}
	}, 

	//<13> T Block Rotated Thrice
	{{{0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 1, 1, 0, 0},
		{0, 0, 2, 0, 0},
		{0, 0, 0, 0, 0},}, 3,2,
	{-1,0,0},
	{-1,-1,-2}}, 

	//<14> I block rotated thrice
	{{{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 2, 1, 1, 1},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},}, 2,1,
	{1,2,3},
	{0,0,0},
	}, 
	
	//<15> Z block rotated thrice
	{{{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 2, 1, 0, 0},
		{0, 0, 1, 1, 0},
		{0, 0, 0, 0, 0},}, 2,1,
	{1,1,2},
	{0,1,1}
	}, 

	};
	






uint8_t BLOCK;
	void PortD_Init(void){
  volatile int delay;
  SYSCTL_RCGCGPIO_R |= 0x08;
  delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTD_DIR_R &= 0x00;
  GPIO_PORTD_DEN_R |= 0x0E;
}


volatile int8_t xpos=3;
volatile int8_t ypos=4;
volatile int8_t wantedXpos=3;
volatile int8_t wantedYpos=4;


void SysTick_Init(uint32_t period){
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = period-1;
	NVIC_ST_CURRENT_R =0;
	NVIC_SYS_PRI3_R = 0x20000000 | ((0x00FFFFFF) & (NVIC_SYS_PRI3_R)); // set priority to 1
	NVIC_ST_CTRL_R = 0x0007;
}


int collisionCheckY(){
	int check = 0;
	
	for(int i = 0; i<3; i++){
		if(tetrisBoard[wantedYpos][wantedXpos] == 1){
			check = 1;
		}
		else if(tetrisBoard[wantedYpos+(allBlocks[BLOCK].offsetY[i])][xpos]==1){
			check = 1;
		}
	}
	
	return check;
}

int collisionCheckX(){
	int check = 0;
	for(int i =0; i<3; i++){
		if(wantedXpos+allBlocks[BLOCK].offsetX[i]>=10 || wantedXpos+allBlocks[BLOCK].offsetX[i]<0){
			check = 1;
		}
		else if(tetrisBoard[ypos][wantedXpos+allBlocks[BLOCK].offsetX[i]] == 1){
			check = 1;
		}
	}
	return check;
}
void drawSquare(int x, int y, int color, int erase){
	for(int x1=0; x1<10; x1++){
		for(int y1=0; y1<10; y1++){
			if((x1 == 0 || y1 == 0 || x1 == 9 || y1 == 9) && (erase==0)){ //Each square will be 10x10. 0 and 9 coords = border
				ST7735_DrawPixel(x*10+x1+10,y*10+y1,ST7735_MAGENTA);
			}
			else{
				ST7735_DrawPixel(x*10+x1+10,y*10+y1,color);
			}
		}
	}
}
uint32_t n;
uint8_t status=0;

void SysTick_Handler(void){
	if(ypos<13){ //13 will be touching ground
		
		wantedYpos++;
		n = ADC_In(); 
		n =1538 * n/4095+176;
		uint32_t b = ADC_Position(n);
		wantedXpos = b;
		
		for(signed int x=0; x<3; x++){
			tetrisBoard[ypos][xpos] = 0;
			tetrisBoard[ypos+(allBlocks[BLOCK].offsetY[x])][xpos+(allBlocks[BLOCK].offsetX[x])] = 0; //Erases current block position
		}
		

		if(collisionCheckY() != 1 && collisionCheckX() != 1){
			ypos++;
			xpos = wantedXpos;
			tetrisBoard[ypos][xpos] = 1;
			for(signed int i=0; i<3; i++){
				tetrisBoard[ypos+(allBlocks[BLOCK].offsetY[i])][xpos+(allBlocks[BLOCK].offsetX[i])] = 1; 
			}
		}
		
		else if(collisionCheckY() != 1 && collisionCheckX() == 1){
			ypos = wantedYpos;
			xpos = xpos;
			tetrisBoard[ypos][xpos] = 1;
			for(signed int i=0; i<3; i++){
				tetrisBoard[ypos+(allBlocks[BLOCK].offsetY[i])][xpos+(allBlocks[BLOCK].offsetX[i])] = 1; 
			}
			
		}
		else if(collisionCheckY() == 1){
			tetrisBoard[ypos][xpos] = 1;
			for(signed int x=0; x<3; x++){
				tetrisBoard[ypos+(allBlocks[BLOCK].offsetY[x])][xpos+(allBlocks[BLOCK].offsetX[x])] = 1; 
			}
			ypos = 4; //Reset Y position
			wantedYpos = 4;
			BLOCK++; // Change block, will use random function.
			
		}
	}
	else if(ypos>=13){ //If it touches ground
		ypos = 4; //Reset Y position
		wantedYpos = 4;
		BLOCK++; // Change block, will use random function.
	}
}

int ADC_Position(uint32_t output){
		if(output > 100 && output < 350){
			return 0;
		}
	
		else if(output > 351 && output < 500){
			return 1;
		}
		
		else if(output >= 500 && output < 650){
			return 2;
		}
	
		else if(output >= 650 && output < 800){
			return 3;	
		}
	
		else if(output >= 800 && output < 950){
			return 4;
		}
	
		else if(output >= 950 && output < 1100){
			return 5;
		}
	
		else if(output >= 1100 && output < 1250){
			return 6;
		}
	
		else if(output >= 1250 && output < 1400){
			return 7;		  
		}               
										
		else if(output >= 1400 && output < 1550){
			return 8;		  
		}               
										
		else if(output >= 1550 && output < 2000){
			return 9;	
		}
}


int main(void){
//Code goes here
	DisableInterrupts();

	ST7735_InitR(INITR_REDTAB); 
  ADC_Init();         // turn on ADC, set channel to 1
	PortD_Init();
	SysTick_Init(80000000/30);
	Random_Init(55);
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

	while(GPIO_PORTD_DATA_R == 0){} //Welcome Screen ended, time to play
	
		//Sets up playing screen
		
	ST7735_FillScreen(0);
	ST7735_FillRect(0,146,180,1,ST7735_BLUE);
	ST7735_SetCursor(0,15);
	ST7735_DrawString(0,15,"Score: ", ST7735_CYAN);
	
	BLOCK = 0;//(Random32()%60)/4; // returns a random number from 0 to 59

	while(1){ //Tetris Game Starts
		EnableInterrupts();
		playsound(1);
		
		DisableInterrupts(); //Do not want interrupts during refreshing/updating of the LCD screen & Tetris Array
//		for(signed int x=0; x<3; x++){ //This function puts the current block data into the Board array
//			tetrisBoard[ypos][xpos] = allBlocks[BLOCK].blockData[allBlocks[BLOCK].bottomX][allBlocks[BLOCK].bottomY];
//			tetrisBoard[ypos+(allBlocks[BLOCK].offsetY[x])][xpos+(allBlocks[BLOCK].offsetX[x])] = 1;
//		}


		
		for(int i=0; i<10; i++){ //Displays board array onto screen.
			for(int j=0; j<14; j++){
				if(tetrisBoard[j][i] != 0){
					drawSquare(i,j,ST7735_BLUE, 0);
				}
				else{
					drawSquare(i,j,ST7735_BLACK, 1);
				}
					//ST7735_SetCursor(i,j);
					//LCD_OutDec(tetrisBoard[j][i]);
			}
		}
		
		EnableInterrupts();
		
//		for(int i=0; i<10; i++){
//			for(int j=5; j<13; j++){
//				ST7735_SetCursor(i,j-4);
//				LCD_OutDec(tetrisBoard[j][i]);
//			}
//		}
		

		//Game Over, restart
		//main();
		
	}
}




