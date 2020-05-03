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
struct m_node {
    uint16_t x_coord;
		uint16_t y_coord;
	
		struct m_node *next;
};
static const uint16_t START_STATE = 0xACE7u;
typedef struct m_node M_node;
M_node *s_missle;
volatile uint16_t CAP_TOUCH_X = BOARD_WIDTH / 2;
volatile uint16_t CAP_TOUCH_Y = BOARD_HEIGHT / 2;
volatile bool BUTTON_PRESSED = false;
volatile bool ALERT_MISSLE;
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
// Generates a random number
// https://en.wikipedia.org/wiki/Linear-feedback_shift_register  -- DO NOT MODIFY
// Borrowed from HW3
//*****************************************************************************
uint16_t generate_random_number(
)
{   
    static uint16_t lfsr = START_STATE;
    uint16_t bit;
    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    lfsr =  (lfsr >> 1) | (bit << 15);
    return lfsr;
}


//Used to remove a missle node from a linked list of nodes
M_node *
clear_node(M_node *node, M_node *prev){
	if(prev != (M_node *)0){
		prev->next = node->next;
		free(node);
		return prev->next;
	}
	prev = node->next;
	free(node);
	return prev;
	
}

//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
	uint16_t missle_time;
	uint8_t button_data;
	M_node *newNode;

	init_hardware();
	
	s_missle = malloc(sizeof(M_node));
	s_missle->x_coord = BOARD_WIDTH / 4;
	s_missle->y_coord = BOARD_HEIGHT / 4;
	lcd_draw_image(s_missle->x_coord, missleWidthPixels, s_missle->y_coord, missleHeightPixels, missleBitmaps, LCD_COLOR_BLUE, LCD_COLOR_BLACK);
	
	lcd_draw_image(BOARD_WIDTH / 2,	bombHolderWidthPixels, BOARD_HEIGHT / 2, bombHolderHeightPixels, bombHolderBitmaps, LCD_COLOR_GREEN, LCD_COLOR_BLACK);
	

	io_expander_write_reg(MCP23017_GPIOA_R, 0xFF);
	missle_time = 0;
		
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
			if(ALERT_MISSLE){
				//Update missles on the board to move downwards
				M_node *node = s_missle;
				M_node *prev = (M_node *)0;
				
				while(node != (M_node *)0){
					node->y_coord += 1;
					if(node->y_coord > BOARD_HEIGHT - (missleHeightPixels / 2) - 10){
						lcd_draw_image(node->x_coord, missleWidthPixels, node->y_coord - 1, missleHeightPixels, missleBitmaps, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
						node = clear_node(node, prev);
					}
					else{
						lcd_draw_image(node->x_coord, missleWidthPixels, node->y_coord, missleHeightPixels, missleBitmaps, LCD_COLOR_BLUE, LCD_COLOR_BLACK);
						prev = node;
						node = node->next;
					}
				}
				
				//Create a random new missle every few cycles 
				missle_time++;
				if(missle_time >= 30){
					missle_time = 0;
					newNode = malloc(sizeof(M_node));
					newNode->y_coord = missleHeightPixels + 10; // set to be the top of the screen
					newNode->x_coord = (generate_random_number() % (BOARD_WIDTH - missleWidthPixels - 5)) + (missleWidthPixels / 2); // set to be a random x location
					newNode->next = s_missle;
					s_missle = newNode; // set the missle to the front of the list
				}
				
				ALERT_MISSLE = false;
			}
			if(BUTTON_PRESSED)
			{
				button_data = io_expander_read_reg(0x13);
				
				if (button_data & DIR_BTN_UP_PIN){
					lcd_clear_screen(LCD_COLOR_BLACK);
					while(1);
				}
			}
		};
}

