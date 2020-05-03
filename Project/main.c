// Copyright (c) 2015-19, Joe Krachey
// All rights reserved.
//
// Redistribution and use in source or binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in source form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "main.h"

volatile uint16_t CAP_TOUCH_X = BOARD_WIDTH / 2;
volatile uint16_t CAP_TOUCH_Y = BOARD_HEIGHT / 2;


uint16_t BOMB_X = BOARD_WIDTH / 2;
uint16_t BOMB_Y = BOARD_HEIGHT / 2;

volatile bool ALERT_BOMB_HOLDER;

//*****************************************************************************
//*****************************************************************************
void DisableInterrupts(void)
{
  __asm {
         CPSID  I
  }
}

//*****************************************************************************
//*****************************************************************************
void EnableInterrupts(void)
{
  __asm {
    CPSIE  I
  }
}


//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
	
	init_hardware();
	
	lcd_draw_image(BOARD_WIDTH / 2,	bombHolderWidthPixels, BOARD_HEIGHT / 2, bombHolderHeightPixels, bombHolderBitmaps, LCD_COLOR_GREEN, LCD_COLOR_BLACK);

		
	//gp_timer_wait(TIMER2_BASE, 1000000);

    while(1){
			if(ALERT_BOMB_HOLDER)
			{
					//Clear the old bomb
					lcd_draw_image(BOMB_X, clear_bombHolderWidthPixels, BOMB_Y, clear_bombHolderHeightPixels, clear_bombHolderBitmaps, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
				
					BOMB_X = CAP_TOUCH_X;
					BOMB_Y = CAP_TOUCH_Y;
					lcd_draw_image(CAP_TOUCH_X,	bombHolderWidthPixels, CAP_TOUCH_Y, bombHolderHeightPixels, bombHolderBitmaps, LCD_COLOR_GREEN, LCD_COLOR_BLACK);
					ALERT_BOMB_HOLDER = false;
					
			}
		};
}

