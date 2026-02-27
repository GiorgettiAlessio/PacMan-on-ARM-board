/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "LPC17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include <stdio.h>
#include <stdlib.h>
#include "music/music.h"



/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
#define R 31
#define C 28


uint16_t SinTable[45] =                                       /* ÕýÏÒ±í                       */
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};


int score=0;
int vitaExtra=0;
int frightened=0;
int stopFrightened=0;

int stopBlinky=0;

int scoreToWin=2690;

int respawnTime=0;

int countdown = 60;

extern int go;

extern int lives;
extern int livesPoint;

extern int mat[31][28];
extern int pacmanPOS[2];	//posizione pacman
extern int blinkyPOS[2];	//posizione blinky
extern int powerUP[6][2];	//posizione powerup

extern int powerN[6];


void moveBlinky(int direction, uint16_t color){
		int x0,y0,x1,y1, size=7, mini=1, i, y=295;
		static int val=2, c=0;
		
		int val1 = blinkyPOS[0];
		int val2= blinkyPOS[1];
		x0 = blinkyPOS[1]*size + 22;
		y0 = blinkyPOS[0]*size + 70;
	
		if(direction == 1){
			blinkyPOS[0] = blinkyPOS[0]-1;
			blinkyPOS[1] = blinkyPOS[1];
		}
		else if(direction == 2){
			blinkyPOS[0] = blinkyPOS[0];
			blinkyPOS[1] = blinkyPOS[1]+1;
		}
		else if(direction == 3){
			blinkyPOS[0] = blinkyPOS[0];
			blinkyPOS[1] = blinkyPOS[1]-1;
		}
		else{
			blinkyPOS[0] = blinkyPOS[0]+1;
			blinkyPOS[1] = blinkyPOS[1];
		}
		
		//CONTROLLO HIT PACMAN
		if(val1 == pacmanPOS[0] && val2 == pacmanPOS[1] && frightened==0){
			//PACMAN MANGIATO
			lives--;
			livesPoint-=30;
			vitaExtra=1;
			
			for (i = 0; i < 15; i++) {
				 LCD_DrawLine(livesPoint, y + i, livesPoint + 15, y + i, Black); // Disegna le linee orizzontali per riempire l'interno
			}
			
			//RESTART
			x1 = pacmanPOS[1]*size + 22;
			y1 = pacmanPOS[0]*size + 70;
			for (i = 0; i < size - mini; i++) {
       LCD_DrawLine(x1 + mini, y1 + mini + i, x1 + size - mini, y1 + mini + i, Black);
			}
			
			
			mat[blinkyPOS[0]][blinkyPOS[1]] = 2;
			mat[pacmanPOS[0]][pacmanPOS[1]] = 2;
			mat[23][13] = 3;
			pacmanPOS[0]=23;
			pacmanPOS[1]=13;
			mat[14][13] = -1;
			blinkyPOS[0]=14;
			blinkyPOS[1]=13;

			x1 = blinkyPOS[1]*size + 22;
			y1 = blinkyPOS[0]*size + 70;
			for (i = 0; i < size - mini; i++) {
				 LCD_DrawLine(x0 + mini, y0 + mini + i, x0 + size - mini, y0 + mini + i, Black);
			}
			LCD_DrawLine(x1+2, y1+1, x1 + 5, y1 +1, color);
			LCD_DrawLine(x1+1, y1+2, x1 + 6, y1 +2, color);
			LCD_DrawLine(x1+1, y1+3, x1 + 6, y1 +3, color);
			LCD_DrawLine(x1+1, y1+4, x1 + 6, y1 +4, color);
		
			LCD_DrawLine(x1+1, y1+6, x1 + 2, y1 +6, color);	//-
			LCD_DrawLine(x1+2, y1+6, x1 + 2, y1 +4, color);	//|
			LCD_DrawLine(x1+2, y1+4, x1 + 5, y1 +4, color);	//-
			LCD_DrawLine(x1+5, y1+4, x1 + 5, y1 +6, color);	//|
			LCD_DrawLine(x1+5, y1+6, x1 + 6, y1 +6, color);	//-
			
			
			x1 = pacmanPOS[1]*size + 22;
			y1 = pacmanPOS[0]*size + 70;
			
			
			LCD_DrawLine(x1+3, y1+1, x1 + 4, y1 +1, Yellow);
			LCD_DrawLine(x1+2, y1+2, x1 + 5, y1 +2, Yellow);
			LCD_DrawLine(x1+1, y1+3, x1 + 6, y1 +3, Yellow);
			LCD_DrawLine(x1+1, y1+4, x1 + 6, y1 +4, Yellow);
			LCD_DrawLine(x1+2, y1+5, x1 + 5, y1 +5, Yellow);
			LCD_DrawLine(x1+3, y1+6, x1 + 4, y1 +6, Yellow);
			
		}
		else if(val1 == pacmanPOS[0] && val2 == pacmanPOS[1] && frightened==1){
			//BLINKY MANGIATO
			//disable_timer(2);
			stopBlinky=1;
			//enable_timer(3);
			respawnTime=1;
			mat[blinkyPOS[0]][blinkyPOS[1]] = 2;
			blinkyPOS[0]=14;
			blinkyPOS[1]=13;

			x1 = blinkyPOS[1]*size + 22;
			y1 = blinkyPOS[0]*size + 70;
			for (i = 0; i < size - mini; i++) {
				 LCD_DrawLine(x0 + mini, y0 + mini + i, x0 + size - mini, y0 + mini + i, Black);
			}
		}
		else{
			val = mat[blinkyPOS[0]][blinkyPOS[1]];
			
			
			x1 = blinkyPOS[1]*size + 22;
			y1 = blinkyPOS[0]*size + 70;
			for (i = 0; i < size - mini; i++) {
				 LCD_DrawLine(x0 + mini, y0 + mini + i, x0 + size - mini, y0 + mini + i, Black);
			}
			
			if(val == 0 && c>0){
				c--;
				mat[val1][val2]=0;
				mini=3;
				// Disegna il bordo superiore
				LCD_DrawLine(x0+mini, y0+mini, x0 + size - mini, y0 +mini, Yellow);
				// Disegna il bordo inferiore
				LCD_DrawLine(x0+mini, y0 + size - mini, x0 + size - mini, y0 + size - mini, 0xFC00 );
				// Disegna i bordi laterali
				LCD_DrawLine(x0+mini, y0+mini, x0+mini, y0 + size - mini, Yellow);
				LCD_DrawLine(x0 + size - mini, y0+mini, x0 + size - mini, y0 + size - mini, 0xFC00 );
				
				//mat[blinkyPOS[0]][blinkyPOS[1]] = -1;
			}
			else if(val==2 && mat[val1][val2]!=4 && mat[val1][val2]!=6){
				mat[val1][val2]=2;
				//mat[blinkyPOS[0]][blinkyPOS[1]] = -1;
				if(c>0){
					c--;
					mat[val1][val2]=0;
					mini=3;
					// Disegna il bordo superiore
					LCD_DrawLine(x0+mini, y0+mini, x0 + size - mini, y0 +mini, Yellow);
					// Disegna il bordo inferiore
					LCD_DrawLine(x0+mini, y0 + size - mini, x0 + size - mini, y0 + size - mini, 0xFC00 );
					// Disegna i bordi laterali
					LCD_DrawLine(x0+mini, y0+mini, x0+mini, y0 + size - mini, Yellow);
					LCD_DrawLine(x0 + size - mini, y0+mini, x0 + size - mini, y0 + size - mini, 0xFC00 );
				}
				
			}
			else if(val == 4 && direction==3){
				blinkyPOS[0] = 14;
				blinkyPOS[1] = 27;
				mat[val1][val2]=2;
				x1 = blinkyPOS[1]*size + 22;
				y1 = blinkyPOS[0]*size + 70;
			}
			else if(val == 4 && direction==2){
				blinkyPOS[0] = 14;
				blinkyPOS[1] = 0;
				mat[val1][val2]=2;
				x1 = blinkyPOS[1]*size + 22;
				y1 = blinkyPOS[0]*size + 70;
			}
			
			if(val==0)
				c++;
			
			LCD_DrawLine(x1+2, y1+1, x1 + 5, y1 +1, color);
			LCD_DrawLine(x1+1, y1+2, x1 + 6, y1 +2, color);
			LCD_DrawLine(x1+1, y1+3, x1 + 6, y1 +3, color);
			LCD_DrawLine(x1+1, y1+4, x1 + 6, y1 +4, color);
		
			LCD_DrawLine(x1+1, y1+6, x1 + 2, y1 +6, color);	//-
			LCD_DrawLine(x1+2, y1+6, x1 + 2, y1 +4, color);	//|
			LCD_DrawLine(x1+2, y1+4, x1 + 5, y1 +4, color);	//-
			LCD_DrawLine(x1+5, y1+4, x1 + 5, y1 +6, color);	//|
			LCD_DrawLine(x1+5, y1+6, x1 + 6, y1 +6, color);	//-
			
		}
		
		
}

void move(int x0, int y0, int x1, int y1, int eat){
		int i=0, mini=1, size=7, points=0; 
		volatile int x3=0, x4=0, y3=0, y4=0;
		
		//aggiorna posizione pacmano
		pacmanPOS[0] = x1;
		pacmanPOS[1] = y1;
		
		//se cella non vuota mangia
		if(eat == 1){
			points = 10;
			NOTE nota = {d4, time_semibiscroma};
			playNote(nota);
		}
		else if(eat == 2){
			points = 50;
			//enable_timer(3);
			frightened=1;
			stopFrightened = countdown-10;
		}
		
		
		score += points;
		
		PutNumber(160, 25, score, White, Black);
		
		//aggiorna matrice
		if(mat[x0][y0] != 4){
			mat[x0][y0] = 2;
		}
		//-
		if(mat[x1][y1] != 4){
			mat[x1][y1] = 3;
		}
		
		x3 = y0*size + 22;
		y3 = x0*size + 70;
		x4 = y1*size + 22;
		y4 = x1*size + 70;
		
		for (i = 0; i < size - mini; i++) {
       LCD_DrawLine(x3 + mini, y3 + mini + i, x3 + size - mini, y3 + mini + i, Black);//elimina il cibo mangiato
		}
		
		LCD_DrawLine(x4+3, y4+1, x4 + 4, y4 +1, Yellow);
		LCD_DrawLine(x4+2, y4+2, x4 + 5, y4 +2, Yellow);
		LCD_DrawLine(x4+1, y4+3, x4 + 6, y4 +3, Yellow);
		LCD_DrawLine(x4+1, y4+4, x4 + 6, y4 +4, Yellow);
		LCD_DrawLine(x4+2, y4+5, x4 + 5, y4 +5, Yellow);
		LCD_DrawLine(x4+3, y4+6, x4 + 4, y4 +6, Yellow);
		
}



void TIMER0_IRQHandler (void)
{
	//GAMEOVER
	static int respawnBlinky=0;
	
	PutNumber(50, 25, countdown, White, Black);
	countdown--;
	
	if(countdown < 0 || lives==0){
		LCD_Clear(Black);
		GUI_Text(90, 150, (uint8_t*) "Game Over", Red, Black);
		disable_timer(0);
		disable_timer(1);
		disable_timer(2);
	}
	
	//velocizza il fantasma
	if(countdown<30){
		uint32_t timer = LPC_TIM2->MR0;
		LPC_TIM1->MR1 = timer/2;
	}
	
	volatile int i,j,n,conta=0, si=0, x, y, sost=0, k=0;
	for(i=0;i<6;i++){
		if(countdown == powerN[i]){
			n = rand() % (240 + 1 - 0) + 0;		//pil random da sostituire
			while(si!=1){
				for(i=0;i<R;i++){
					for(j=0;j<C;j++){
						if(mat[i][j]==0 && conta==n){
							x = j * 7 + 22;
							y = i * 7 + 70;
							for (k = 0; k < 4; k++) {
								LCD_DrawLine(x + 2, y + 2 + k, x + 5, y + 2 + k, 0xFC00);
							}
							
							mat[i][j] = 5;
							si=1;
							
							
						}
						else if(mat[i][j] != 0 && conta==n){
							sost=1;
						}
						else if(mat[i][j]==0 && sost==1){
							x = j * 7 + 22;
							y = i * 7 + 70;
							for (k = 0; k < 4; k++) {
								LCD_DrawLine(x + 2, y + 2 + k, x + 5, y + 2 + k, 0xFC00);
							}
							
							mat[i][j] = 5;
							si=1;
							
							sost=0;
						}
						conta++;
					}
				}
			}
			
		}
	}
	
	if(respawnTime==1){
		respawnTime=0;
		respawnBlinky = countdown - 3;
	}
	
	//respawna blinky
	if(respawnBlinky == countdown){
		int x1,y1,size=7;
		//enable_timer(2);
		stopBlinky=0;
		frightened=0;
		score+=100;
		scoreToWin+=100;
		PutNumber(160, 25, score, White, Black);
		
		mat[14][13] = -1;
		blinkyPOS[0]=14;
		blinkyPOS[1]=13;

		x1 = blinkyPOS[1]*size + 22;
		y1 = blinkyPOS[0]*size + 70;
		
		LCD_DrawLine(x1+2, y1+1, x1 + 5, y1 +1, Red);
		LCD_DrawLine(x1+1, y1+2, x1 + 6, y1 +2, Red);
		LCD_DrawLine(x1+1, y1+3, x1 + 6, y1 +3, Red);
		LCD_DrawLine(x1+1, y1+4, x1 + 6, y1 +4, Red);

		LCD_DrawLine(x1+1, y1+6, x1 + 2, y1 +6, Red);	//-
		LCD_DrawLine(x1+2, y1+6, x1 + 2, y1 +4, Red);	//|
		LCD_DrawLine(x1+2, y1+4, x1 + 5, y1 +4, Red);	//-
		LCD_DrawLine(x1+5, y1+4, x1 + 5, y1 +6, Red);	//|
		LCD_DrawLine(x1+5, y1+6, x1 + 6, y1 +6, Red);	//-
	}
	
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	static int tm0,tm1,c=0;
	if(c==0){
		tm0 = LPC_TIM1->MR0;
		tm1 = LPC_TIM1->MR1;
		c++;
	}
	
	
	if(LPC_TIM1->IR & (1 << 0)){
		LPC_TIM1->MR0 += tm0;
		//pacman
		switch(go){
			//up
			case 1: 
				if(mat[pacmanPOS[0]-1][pacmanPOS[1]] != 1){
					if(mat[pacmanPOS[0]-1][pacmanPOS[1]] == 0){
							move(pacmanPOS[0], pacmanPOS[1], pacmanPOS[0]-1, pacmanPOS[1], 1);
						}
						else if(mat[pacmanPOS[0]-1][pacmanPOS[1]] == 2){
							move(pacmanPOS[0], pacmanPOS[1], pacmanPOS[0]-1, pacmanPOS[1], 0);
						}
						else if(mat[pacmanPOS[0]-1][pacmanPOS[1]] == 5){
							move(pacmanPOS[0], pacmanPOS[1], pacmanPOS[0]-1, pacmanPOS[1], 2);
						}
				}
				else{
					go=0;
					vitaExtra=0;
				}
				break;
			
			//right
			case 2:
				if(mat[pacmanPOS[0]][pacmanPOS[1]+1] != 1){				
						if(mat[pacmanPOS[0]][pacmanPOS[1]+1] == 0){
							move(pacmanPOS[0], pacmanPOS[1], pacmanPOS[0], pacmanPOS[1]+1, 1);
						}
						else if(mat[pacmanPOS[0]][pacmanPOS[1]+1] == 2){
							move(pacmanPOS[0], pacmanPOS[1], pacmanPOS[0], pacmanPOS[1]+1, 0);
						}
						else if(mat[pacmanPOS[0]][pacmanPOS[1]+1] == 5){
							move(pacmanPOS[0], pacmanPOS[1], pacmanPOS[0], pacmanPOS[1]+1, 2);
						}
						else if(mat[pacmanPOS[0]][pacmanPOS[1]+1] == 4){
							move(pacmanPOS[0], pacmanPOS[1], 14, 0, 0);	
						}
				}
				else{
					go=0;
					vitaExtra=0;
				}
				break;
			
			//left
			case 3:
				if(mat[pacmanPOS[0]][pacmanPOS[1]-1] != 1){
						if(mat[pacmanPOS[0]][pacmanPOS[1]-1] == 0){
							move(pacmanPOS[0], pacmanPOS[1], pacmanPOS[0], pacmanPOS[1]-1, 1);
						}
						else if(mat[pacmanPOS[0]][pacmanPOS[1]-1] == 2){
							move(pacmanPOS[0], pacmanPOS[1], pacmanPOS[0], pacmanPOS[1]-1, 0);
						}	
						else if(mat[pacmanPOS[0]][pacmanPOS[1]-1] == 5){
							move(pacmanPOS[0], pacmanPOS[1], pacmanPOS[0], pacmanPOS[1]-1, 2);
						}
						else if(mat[pacmanPOS[0]][pacmanPOS[1]-1] == 4){
							move(pacmanPOS[0], pacmanPOS[1], 14, 27, 0);	
						}
				}
				else{
					go=0;
					vitaExtra=0;
				}
				break;
				
			//down
			case 4:
				if(mat[pacmanPOS[0]+1][pacmanPOS[1]] != 1){
						if(mat[pacmanPOS[0]+1][pacmanPOS[1]] == 0){
							move(pacmanPOS[0], pacmanPOS[1], pacmanPOS[0]+1, pacmanPOS[1], 1);
						}
						else if(mat[pacmanPOS[0]+1][pacmanPOS[1]] == 2){
							move(pacmanPOS[0], pacmanPOS[1], pacmanPOS[0]+1, pacmanPOS[1], 0);
						}
						else if(mat[pacmanPOS[0]+1][pacmanPOS[1]] == 5){
							move(pacmanPOS[0], pacmanPOS[1], pacmanPOS[0]+1, pacmanPOS[1], 2);
						}						
				}
				else{
					go=0;
					vitaExtra=0;
				}
				break;
				
			default:
				break;
		}

		if(score != 0 && score % 1000 == 0 && vitaExtra==0){
			lives++;
			vitaExtra=1;
			int i, y=295;
			for (i = 0; i < 15; i++) {
				 LCD_DrawLine(livesPoint, y + i, livesPoint + 15, y + i, Yellow); // Disegna le linee orizzontali per riempire l'interno
			}
			livesPoint+=30;
		}

		if(score >= scoreToWin){
			disable_timer(0);
			disable_timer(1);
			disable_timer(2);
			LCD_Clear(Black);

			GUI_Text(90, 150, (uint8_t*) "VICTORY", Cyan, Black);
		}
		LPC_TIM1->IR = 1;
	}
	else if(LPC_TIM1->IR & (1 << 1)){
		LPC_TIM1->MR1 += tm1;
		//blinky
		static int direction=0;
		volatile int y_dist, x_dist, up_dist, right_dist, left_dist, down_dist;
		
		if(stopBlinky==0){
			if(mat[blinkyPOS[0]][blinkyPOS[1]] == 6 || mat[blinkyPOS[0]-1][blinkyPOS[1]] == 6){
				moveBlinky(1, Red);
			}
			else{
				if(countdown == stopFrightened){
					frightened=0;
				}
				
				if(frightened==0){
					up_dist=1000;
					right_dist=1000;
					left_dist=1000;
					down_dist=1000;
				}
				else{
					up_dist=-1000;
					right_dist=-1000;
					left_dist=-1000;
					down_dist=-1000;
				}
					
				
				
				//tile up
				if(mat[blinkyPOS[0]-1][blinkyPOS[1]] != 1 && mat[blinkyPOS[0]-1][blinkyPOS[1]] != 6){
					y_dist = abs((blinkyPOS[0]-1) - pacmanPOS[0]);
					x_dist = abs(blinkyPOS[1] - pacmanPOS[1]);
					up_dist = (y_dist*y_dist) + (x_dist*x_dist);
				}
				
				//tile right
				if(mat[blinkyPOS[0]][blinkyPOS[1]+1] != 1 && mat[blinkyPOS[0]][blinkyPOS[1]+1] != 6){
					y_dist = abs(blinkyPOS[0] - pacmanPOS[0]);
					x_dist = abs((blinkyPOS[1]+1) - pacmanPOS[1]);
					right_dist = (y_dist*y_dist) + (x_dist*x_dist);
				}
				
				//tile left
				if(mat[blinkyPOS[0]][blinkyPOS[1]-1] != 1 && mat[blinkyPOS[0]][blinkyPOS[1]-1] != 6){
					y_dist = abs(blinkyPOS[0] - pacmanPOS[0]);
					x_dist = abs((blinkyPOS[1]-1) - pacmanPOS[1]);
					left_dist = (y_dist*y_dist) + (x_dist*x_dist);
				}
				
				
				//tile down
				if(mat[blinkyPOS[0]+1][blinkyPOS[1]] != 1 && mat[blinkyPOS[0]+1][blinkyPOS[1]] != 6){
					y_dist = abs((blinkyPOS[0]+1) - pacmanPOS[0]);
					x_dist = abs(blinkyPOS[1] - pacmanPOS[1]);
					down_dist = (y_dist*y_dist) + (x_dist*x_dist);
				}
				
				volatile int min=0;
				volatile int max=0;
				
				switch(direction){
					case 0:
						min = up_dist;
						direction=1;
						if(left_dist<min){
							min=left_dist;
							direction=3;
						}
						if(down_dist<min){
							min=down_dist;
							direction=4;
						}	
						if(right_dist<min){
							min=right_dist;
							direction=2;
						}
							
						moveBlinky(direction, Red);
						break;
					
					case 1:
						if(frightened==0){
							min = up_dist;
							direction=1;
							if(left_dist<min){
								min=left_dist;
								direction=3;
							}
							if(right_dist<min){
								min=right_dist;
								direction=2;
							}
								
							moveBlinky(direction, Red);
							
						}
						else{
							max=up_dist;
							direction=1;
							if(left_dist>max){
								max=left_dist;
								direction=3;
							}
							if(right_dist>max){
								max=right_dist;
								direction=2;
							}
							moveBlinky(direction, Blue);
						}
						break;
					case 2:
						if(frightened==0){
							min = up_dist;
							direction=1;
							if(down_dist<min){
								min=down_dist;
								direction=4;
							}	
							if(right_dist<min){
								min=right_dist;
								direction=2;
							}
								
							moveBlinky(direction, Red);
						}
						else{
							max = up_dist;
							direction=1;
							if(down_dist>max){
								max=down_dist;
								direction=4;
							}	
							if(right_dist>max){
								max=right_dist;
								direction=2;
							}
								
							moveBlinky(direction, Blue);
						}
						break;
					
					case 3:
						if(frightened==0){
							min = up_dist;
							direction=1;
							if(left_dist<min){
								min=left_dist;
								direction=3;
							}
							if(down_dist<min){
								min=down_dist;
								direction=4;
							}
								
							moveBlinky(direction, Red);
						}
						else{
							max = up_dist;
							direction=1;
							if(left_dist>max){
								max=left_dist;
								direction=3;
							}
							if(down_dist>max){
								max=down_dist;
								direction=4;
							}
								
							moveBlinky(direction, Blue);
						}
						
						break;
					
					case 4:
						if(frightened==0){
							min = left_dist;
							direction=3;
							if(down_dist<min){
								min=down_dist;
								direction=4;
							}	
							if(right_dist<min){
								min=right_dist;
								direction=2;
							}
								
							moveBlinky(direction, Red);
						}
						else{
							max = left_dist;
							direction=3;
							if(down_dist>max){
								max=down_dist;
								direction=4;
							}	
							if(right_dist>max){
								max=right_dist;
								direction=2;
							}
								
							moveBlinky(direction, Blue);
						}
						
						break;
					
					default:
						break;
				}
			}
		}
		
		
		
		LPC_TIM1->IR = 2;
	}
	else{
		LPC_TIM1->MR0 = tm0;
		LPC_TIM1->MR1 = tm1;
		LPC_TIM1->IR = 4;
	}
	
	
	
  		
  return;
}


void TIMER2_IRQHandler (void)
{
	static int sineticks=0;
	/* DAC management */	
	static int currentValue; 
	currentValue = SinTable[sineticks];
	currentValue -= 410;
	currentValue /= 1;
	currentValue += 410;
	LPC_DAC->DACR = currentValue <<6;
	sineticks++;
	if(sineticks==45) sineticks=0;
  LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER3_IRQHandler (void)
{
	disable_timer(2);
	LPC_TIM3->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
