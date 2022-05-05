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
#include "Timer0.h"
#include "Tetris.h"
#include "EdgeInterrupt.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
int ADC_Position(uint32_t output);
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
struct tetrisBlock{
	unsigned short blockData[5][5];
	signed short bottomX;
	signed short bottomY;
	signed short offsetX[3]; //These offset values are the positions of the blocks relative to the set "center" position.
	signed short offsetY[3]; //3 in each array because total there are 4 squares that make up a Tetris block. (bottomX/Y is 1 square)
};
typedef struct tetrisBlock block_t;
volatile static block_t allBlocks[16] = {
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
		{0, 1, 1, 0, 0},
		{0, 0, 2, 1, 0},
		{0, 0, 0, 0, 0},}, 3,2,
	{-1,0,1},
	{-1,-1,0}
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
		{0, 1, 1, 0, 0},
		{0, 0, 2, 1, 0},
		{0, 0, 0, 0, 0},}, 3,2,
	{-1,0,1},
	{-1,-1,0}
	}, 

	};
	





int gamestart=0;
uint8_t BLOCK;
	void PortE_Init(void){
  volatile int delay;
  SYSCTL_RCGCGPIO_R |= 0x10;
  delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTE_DIR_R &= 0x00;
  GPIO_PORTE_DEN_R |= 0x0E;
}


volatile int8_t xpos=3;
volatile int8_t ypos=4;
volatile int8_t wantedXpos=3;
volatile int8_t wantedYpos=4;
	int score=0;
volatile uint32_t FallingEdges = 0;
volatile uint32_t FallingEdges1 = 0;
void EdgeCounter_Init(void){                          
  SYSCTL_RCGCGPIO_R |= 0x00000010; // (a) activate clock for port E
	SYSCTL_RCGCGPIO_R |= 0x00000020;
  FallingEdges = 0;             // (b) initialize counter
  GPIO_PORTE_DIR_R &= ~0x04;    // (c) make PE2 in (built-in button)
  GPIO_PORTE_AFSEL_R &= ~0x04;  //     disable alt funct on PE2
  GPIO_PORTE_DEN_R |= 0x04;     //     enable digital I/O on PE2   
  GPIO_PORTE_PCTL_R &= ~0x00000F00; // configure PE2 &PE1 as GPIO
  GPIO_PORTE_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTE_PDR_R |= 0x04;     //     enable weak pull-up on PF4
  GPIO_PORTE_IS_R &= ~0x04;     // (d) PF4 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x04;    //     PF4 is not both edges
  GPIO_PORTE_IEV_R |= 0x04;    //     PF4 rising edge event
  GPIO_PORTE_ICR_R = 0x04;      // (e) clear flag4
  GPIO_PORTE_IM_R |= 0x04;      // (f) arm interrupt on PE2 
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
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
}
void GPIOPortE_Handler(void){
  GPIO_PORTE_ICR_R = 0x04;     // acknowledge flag2
	if(gamestart==1){
		rotateBlock();
		int i = 4000000;
		while(i>0){
			i--;
		}
	}
}

void GPIOPortF_Handler(void){
  GPIO_PORTF_ICR_R = 0x10;      // acknowledge flag4
	if(gamestart==1){
		blockDrop();
		int i = 7000000;
		while(i>0){
			i--;
		}
	}
}

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

int checkClear(void){
	int val = 0;
	int rowCleared = 0;
	int row = 0;
	for(int j=0;j<13;j++){
		int check = 0;
		for(int i=0; i<10; i++){
			if(tetrisBoard[j][i]==0){
				check = 1;
			}
		}
		if(check==0){
			val = 1;
			row = j;
			rowCleared++;
		}
	}
	
	if(rowCleared!=0){
		for(int j = row; j>row-rowCleared; j--){
			for(int i=0; i<10; i++){
				tetrisBoard[j][i] = 0;
			}
		}
		
		for(int j=0; j<(row-rowCleared)+1; j++){
			for(int i =0; i<10; i++){
				tetrisBoard[j+rowCleared][i]=tetrisBoard[j][i];
			}
		}
	}
	return val;
}
void rotateBlock(){
	for(signed int x=0; x<3; x++){
		tetrisBoard[ypos][xpos] = 0;
		tetrisBoard[ypos+(allBlocks[BLOCK].offsetY[x])][xpos+(allBlocks[BLOCK].offsetX[x])] = 0; //Erases current block position
	}
	int temp = BLOCK;
	BLOCK = (BLOCK+4)%16;
	wantedXpos = xpos;
	wantedYpos = ypos;
	if(collisionCheckX() != 1 && collisionCheckY() !=1){
		for(signed int x=0; x<3; x++){
			tetrisBoard[ypos][xpos] = 1;
			tetrisBoard[ypos+(allBlocks[BLOCK].offsetY[x])][xpos+(allBlocks[BLOCK].offsetX[x])] = 1; //Erases current block position
		}
	}
	else{
		BLOCK = temp;
		for(signed int x=0; x<3; x++){
			tetrisBoard[ypos][xpos] = 1;
			tetrisBoard[ypos+(allBlocks[BLOCK].offsetY[x])][xpos+(allBlocks[BLOCK].offsetX[x])] = 1; //Redraw
		}
	}
	
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
		n =(1538 * n)/4095+176;
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
		else if(collisionCheckY() == 1 && collisionCheckX() == 1){
			tetrisBoard[ypos][xpos] = 1;
			for(signed int x=0; x<3; x++){
				tetrisBoard[ypos+(allBlocks[BLOCK].offsetY[x])][xpos+(allBlocks[BLOCK].offsetX[x])] = 1; 
			}
			ypos = 3; //Reset Y position
			wantedYpos = 3;
			wantedXpos = 3;
			xpos=3;
			score=score+100;
			BLOCK = (Random32()%15); // Change block, will use random function.
		}
		else if(collisionCheckY() == 1 && collisionCheckX() != 1){
			xpos = wantedXpos;
			tetrisBoard[ypos][xpos] = 1;
			for(signed int x=0;x<3; x++){
				tetrisBoard[ypos+(allBlocks[BLOCK].offsetY[x])][xpos+(allBlocks[BLOCK].offsetX[x])] = 1; 
			}
			ypos = 3; //Reset Y position
			wantedYpos = 3;
			wantedXpos = 3;
			xpos=3;
			BLOCK = (Random32()%15); // Change block, will use random function.
		}
	}
	else if(ypos>=13){ //If it touches ground
		ypos = 3; //Reset Y position
		wantedYpos = 3;
		wantedXpos = 3;
		xpos=3;
		score=score+100;
		BLOCK = (Random32()%15); // Change block, will use random function.
	}
}



void drawBoard(void){
	for(int i=0; i<10; i++){ //Displays board array onto screen.
		for(int j=13; j>-1; j--){
			if(tetrisBoard[j][i] != 0){
				drawSquare(i,j,ST7735_BLUE, 0);
			}
			else{
				drawSquare(i,j,ST7735_BLACK, 1);
			}
		}
	}
}

void blockDrop(void){
	for(int j=13; j>ypos; j--){
		wantedYpos = j;
		wantedXpos = xpos;
		if(collisionCheckX() != 1 && collisionCheckY() != 1){
			for(signed int x=0; x<3; x++){
				tetrisBoard[ypos][xpos] = 0;
				tetrisBoard[ypos+(allBlocks[BLOCK].offsetY[x])][xpos+(allBlocks[BLOCK].offsetX[x])] = 0; //Erases current block position
			}
			ypos = wantedYpos;
			for(signed int x=0; x<3; x++){
				tetrisBoard[ypos][xpos] = 1;
				tetrisBoard[ypos+(allBlocks[BLOCK].offsetY[x])][xpos+(allBlocks[BLOCK].offsetX[x])] = 1;
			}
		}
	}

}

int checkGameOver(void){
	int gameover = 0;
	for(int i=0; i<10;i++){
		if(tetrisBoard[0][i] == 1){
			gameover = 1;
		}
	}
	return gameover;
}

int languageSelection(){
	int language; //0 = spanish, 1 = english
	ST7735_SetCursor(2,10);
	ST7735_OutString("Choose a Language!");
	ST7735_SetCursor(4,13);
	ST7735_OutString("Espa\xA4ol [L]");
	ST7735_SetCursor(4,14);
	ST7735_OutString("English [R]");
	
	while(GPIO_PORTE_DATA_R == 0 && GPIO_PORTF_DATA_R == 0){}
	
	if(GPIO_PORTE_DATA_R != 0){
		language = 0;
	}
	else if(GPIO_PORTF_DATA_R !=0){
		language = 1;
	}
	ST7735_SetCursor(2,10);
	ST7735_OutString("                 ");
	ST7735_SetCursor(3,13);
	ST7735_OutString("                 ");
	ST7735_SetCursor(3,14);
	ST7735_OutString("                 ");
	return language;
}

int main(void){
//Code goes here
	DisableInterrupts();

	ST7735_InitR(INITR_REDTAB); 
  ADC_Init();         // turn on ADC, set channel to 1

	Sound_Init();
	EdgeCounter_Init();
	Random_Init(55);
	//Initializations
	ST7735_FillScreen(0);
	Timer0_Init(*playgameover,4000);
	Timer1_Init(*playrowclear,4000);
	Timer2_Init(*playtetris,4000);

	//Welcome Screen
	ST7735_DrawBitmap(10,90,tetrisTitle,105,80);

	ST7735_SetTextColor(ST7735_CYAN);
	
	if(languageSelection()==0){
		ST7735_SetCursor(4,9);
		ST7735_OutString("Bienvenido a");
		ST7735_SetCursor(7,10);
		ST7735_OutString("Tetris!");
		ST7735_SetCursor(2,12);
		ST7735_OutString("Presiona el boton");
		ST7735_SetCursor(3,13);
		ST7735_OutString("izquierdo para");
		ST7735_SetCursor(8,14);
		ST7735_OutString("jugar");
	}
	
	else{
		ST7735_SetCursor(3,9);
		ST7735_OutString("Welcome to Tetris!");
		ST7735_SetCursor(3,13);
		ST7735_OutString("Press the Left");
		ST7735_SetCursor(3,14);
		ST7735_OutString("Button to play");
	}


	
	while(GPIO_PORTE_DATA_R == 0){} //Welcome Screen ended, time to play

	
		//Sets up playing screen
	SysTick_Init(80000000/80);
	ST7735_FillScreen(0);
	ST7735_FillRect(0,146,180,1,ST7735_BLUE);
	ST7735_SetCursor(0,15);
	ST7735_DrawString(0,15,"Score: ", ST7735_CYAN);
	for(int i=0; i<10; i++){ //Displays board array onto screen.
		for(int j=13; j>-1; j--){
			tetrisBoard[j][i] = 0;
		}
	}
	BLOCK = (Random32()%15); // returns a random number from 0 to 59

	while(1){ //Tetris Game Starts
		gamestart=1;
		EnableInterrupts();
		//Timer2_Init(*playtetris,4000);	
		//playtetris();
		
		DisableInterrupts(); //Do not want interrupts during refreshing/updating of the LCD screen & Tetris Array
		
		drawBoard();
		Timer2A_Start();
		playtetris();
		if(checkClear()==1){
			drawBoard();
		}
		ST7735_SetCursor(6,15);
		LCD_OutDec(score);
		if(checkGameOver() == 1){
			ST7735_FillScreen(0);
			ST7735_SetCursor(3,9);
			ST7735_OutString("GAME OVER!");
			ST7735_SetCursor(3,13);
			ST7735_OutString(":(");
			ST7735_SetCursor(3,14);
			int i = 50000000;
			while(i>0){
				Timer1B_Start();
				i--;
			}
			goto GAMEOVER;
		}
		
		if(score>2000){
			SysTick_Init(80000000/200);
		}
		else if(score>1000){
			SysTick_Init(80000000/100);
		}
		
		else{
			}
		EnableInterrupts();
	}
		GAMEOVER:
//Game Over, restart
		for(int i=0; i<10; i++){ //Displays board array onto screen.
		for(int j=13; j>-1; j--){
			tetrisBoard[j][i] = 0;
		}
		Timer0A_Start();
		Timer1B_Start();
		score =0;
		wantedYpos=4;
		wantedXpos=3;
	}
		main();
	

}






