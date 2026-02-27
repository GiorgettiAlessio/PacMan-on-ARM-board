/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "GLCD/GLCD.h" 
#include "music/music.h"


// beat 1/4 = 1.65/4 seconds
#define RIT_SEMIMINIMA 8
#define RIT_MINIMA 16
#define RIT_INTERA 32

#define UPTICKS 1


//SHORTENING UNDERTALE: TOO MANY REPETITIONS
NOTE song[] = 
{
	// 1
	{d3, time_semicroma},
	{d3, time_semicroma},
	{d4, time_croma},
	{a3, time_croma},
	{p, time_semicroma},
	{a3b, time_semicroma},
	{p, time_semicroma},
	{g3, time_croma},
	{f3, time_semicroma*2},
	{d3, time_semicroma},
	{f3, time_semicroma},
	{g3, time_semicroma},
	// 2
	{c3, time_semicroma},
	{c3, time_semicroma},
	{d4, time_croma},
	{a3, time_croma},
	{p, time_semicroma},
	{a3b, time_semicroma},
	{p, time_semicroma},
	{g3, time_croma},
	{f3, time_semicroma*2},
	{d3, time_semicroma},
	{f3, time_semicroma},
	{g3, time_semicroma},
	// 3
	{c3b, time_semicroma},
	{c3b, time_semicroma},
	{d4, time_croma},
	{a3, time_croma},
	{p, time_semicroma},
	{a3b, time_semicroma},
	{p, time_semicroma},
	{g3, time_croma},
	{f3, time_semicroma*2},
	{d3, time_semicroma},
	{f3, time_semicroma},
	{g3, time_semicroma},
	// 4
	{a2b, time_semicroma},
	{a2b, time_semicroma},
	{d4, time_croma},
	{a3, time_croma},
	{p, time_semicroma},
	{a3b, time_semicroma},
	{p, time_semicroma},
	{g3, time_croma},
	{f3, time_semicroma*2},
	{d3, time_semicroma},
	{f3, time_semicroma},
	{g3, time_semicroma},
	// 5
	
};



/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

volatile int press=0;
extern int pause;

int go=0;



void RIT_IRQHandler (void)
{					
	static int currentNote = 0;
	static int ticks = 0;
	if(!isNotePlaying())
	{
		++ticks;
		if(ticks == UPTICKS)
		{
			ticks = 0;
			playNote(song[currentNote++]);
		}
	}
	
	if(currentNote == (sizeof(song) / sizeof(song[0])))
	{
		disable_RIT();
	}
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
	
	

	static int position=0;	
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		/* Joytick UP pressed */
		go=1;
		switch(go){
			case 1:
					
				
			break;
			default:
				break;
		}
	}
	else{
			//up=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		/* Joytick RIGHT pressed */
		go=2;
		switch(go){
			case 1:
			
			
			break;
			default:
				break;
		}
	}
	else{
			//right=0;
	}
	
		if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		/* Joytick LEFT pressed */
		go=3;
		switch(go){
			case 1:
			

				break;
			default:
				break;
		}
	}
	else{
			//left=0;
	}
	
		if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
		/* Joytick DOWN pressed */
		go=4;
		switch(go){
			case 1:


				break;
			default:
				break;
		}
	}
	else{
			//down=0;
	}
	

	/* button management */
	if(press>=1){ 
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){	/* KEY0 pressed */
			switch(press){				
				case 2:				
					
					if(pause == 0){
						disable_timer(0);
						disable_timer(1);
						disable_timer(2);
						GUI_Text(100, 164, (uint8_t*) "PAUSE", White, Black);
						pause = 1;
					}
					else{
						enable_timer(0);
						enable_timer(1);
						enable_timer(2);
						GUI_Text(100, 164, (uint8_t*) "PAUSE", Black, Black);
						pause = 0;

					}
				
					break;
				default:
					break;
			}
			press++;
		}
		else {	/* button released */
			press=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	}
/*	else{
			if(down==1)
				down++;
	} */
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
