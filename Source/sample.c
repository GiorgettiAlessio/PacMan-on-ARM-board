/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "joystick/joystick.h"
#include "RIT/RIT.h"


#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

#define R 31
#define C 28
//h=31 w=28
int mat[31][28] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
    {1, 2, 2, 2, 2, 1, 0, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 0, 1, 2, 2, 2, 2, 1},
    {1, 2, 2, 2, 2, 1, 0, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 1, 2, 2, 2, 2, 1},
    {1, 2, 2, 2, 2, 1, 0, 1, 1, 2, 1, 1, 1, 6, 6, 1, 1, 1, 2, 1, 1, 0, 1, 2, 2, 2, 2, 1},
    {1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 6, 6, 6, 6, 6, 6, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1},
    {4, 2, 2, 2, 2, 2, 0, 2, 2, 2, 1, 6, 6, -1, 6, 6, 6, 1, 2, 2, 2, 0, 2, 2, 2, 2, 2, 4},
    {1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 6, 6, 6, 6, 6, 6, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1},
    {1, 2, 2, 2, 2, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 2, 2, 2, 2, 1},
    {1, 2, 2, 2, 2, 1, 0, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 1, 2, 2, 2, 2, 1},
    {1, 2, 2, 2, 2, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 2, 2, 2, 2, 1},
    {1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1},
    {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1},
    {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};



int powerUP[6][2];
int conta=0;
int pacmanPOS[2];
int blinkyPOS[2];
int lives=1;
int livesPoint=40;

int powerN[6];

int pause=1;


void pacman(int x, int y){
		
		LCD_DrawLine(x+3, y+1, x + 4, y +1, Yellow);
		LCD_DrawLine(x+2, y+2, x + 5, y +2, Yellow);
		LCD_DrawLine(x+1, y+3, x + 6, y +3, Yellow);
		LCD_DrawLine(x+1, y+4, x + 6, y +4, Yellow);
		LCD_DrawLine(x+2, y+5, x + 5, y +5, Yellow);
		LCD_DrawLine(x+3, y+6, x + 4, y +6, Yellow);
		
}

void blinky(int x, int y){
		LCD_DrawLine(x+2, y+1, x + 5, y +1, Red);
		LCD_DrawLine(x+1, y+2, x + 6, y +2, Red);
		LCD_DrawLine(x+1, y+3, x + 6, y +3, Red);
		LCD_DrawLine(x+1, y+4, x + 6, y +4, Red);
	
		LCD_DrawLine(x+1, y+6, x + 2, y +6, Red);	//-
		LCD_DrawLine(x+2, y+6, x + 2, y +4, Red);	//|
		LCD_DrawLine(x+2, y+4, x + 5, y +4, Red);	//-
		LCD_DrawLine(x+5, y+4, x + 5, y +6, Red);	//|
		LCD_DrawLine(x+5, y+6, x + 6, y +6, Red);	//-
}

void map(int x, int y) {
    int size = 7; // Dimensione del blocco (7x7)
		
    // Disegna il bordo superiore
    LCD_DrawLine(x, y, x + size - 1, y, Blue);
    // Disegna il bordo inferiore
    LCD_DrawLine(x, y + size - 1, x + size - 1, y + size - 1, Blue);
    // Disegna i bordi laterali
    LCD_DrawLine(x, y, x, y + size - 1, Blue);
    LCD_DrawLine(x + size - 1, y, x + size - 1, y + size - 1, Blue);
}

void food(int x, int y) {
    int size = 7, mini=3, i=0; // Dimensione del blocco (7x7)
		
	
		// Disegna il bordo superiore
		LCD_DrawLine(x+mini, y+mini, x + size - mini, y+mini, Yellow);
		// Disegna il bordo inferiore
		LCD_DrawLine(x+mini, y + size - mini, x + size - mini, y + size - mini, 0xFC00 );
		// Disegna i bordi laterali
		LCD_DrawLine(x+mini, y+mini, x+mini, y + size - mini, Yellow);
		LCD_DrawLine(x + size - mini, y+mini, x + size - mini, y + size - mini, 0xFC00 );
				
		
		conta++;
}


void draw(int mat[R][C]){
	int i = 0, j=0, c=0, countF=0, size=7, UP=0;
	
	
	for (i = 0; i < 6; i++) {
		powerN[i] = rand() % (59 + 1 - 10) + 10;		//momento random per stampare power pills
  }

	for(i=0;i<R;i++){
			for(j=0;j<C;j++){
					if(mat[i][j] == 1){
							int x = j * size + 22;
              int y = i * size + 70;
              map(x, y);
					}
					else if(mat[i][j] == 0){
							int x = j * size + 22;
              int y = i * size + 70;
							food(x, y);
					}
					else if(mat[i][j] == 3){
							int x = j * size + 22;
              int y = i * size + 70;
							pacmanPOS[0]=i;
							pacmanPOS[1]=j;
							pacman(x,y);
					}
					else if(mat[i][j] == -1){
						int x = j * size + 22;
						int y = i * size + 70;
						blinkyPOS[0] = i;
						blinkyPOS[1] = j;
						blinky(x,y);
						
					}
			}
	}
	return;
}

int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	
  LCD_Initialization();
	BUTTON_init();
  TP_Init();
	joystick_init();	
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
	enable_RIT();													/* RIT enabled												*/
	ADC_init();

	
	
	uint8_t gameover[] = "GameOver in";
	uint8_t score[] = "Score";

	//240x320
	LCD_Clear(Black);
	LCD_DrawLine(0, 50, 240, 50, White);
	GUI_Text(10, 5, gameover, White, Black);
	GUI_Text(150, 5, score, White, Black);
	
	
	
	init_timer(0, 0x017D7840);	//timeout
	init_timer(1, 0x00393870);	//pacman & blinky movement
	//init_timer(2, 0x00393870);	//blinky movement
	
	

	
	draw(mat);
	//disegno vite
	int i, y=295;
	for (i = 0; i < 15; i++) {
       LCD_DrawLine(livesPoint, y + i, livesPoint + 15, y + i, Yellow); // Disegna le linee orizzontali per riempire l'interno
	}			 
	livesPoint +=30;
	
	
	enable_timer(0);
	enable_timer(1);
	
	GUI_Text(100, 164, (uint8_t*) "PAUSE", White, Black);
	pause = 1;
	disable_timer(0);
	disable_timer(1);
	disable_timer(2);
	
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);				

	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);	
	
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
