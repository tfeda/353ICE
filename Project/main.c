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

#define ADDR_START    256

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
volatile bool ALERT_MISSLE = false;
volatile bool ALERT_HEART = false;
volatile bool paused = false;
volatile PS2_DIR_t PS2_DIR = PS2_DIR_CENTER;


uint16_t BOMB_X = BOARD_WIDTH / 2; // put bomb in middle of the board
uint16_t BOMB_Y = BOARD_HEIGHT / 2;

uint16_t HEART_X = BOARD_WIDTH / 2;
uint16_t HEART_Y = BOARD_HEIGHT - (18) - 5; // put heart at bottom of board

volatile bool ALERT_BOMB_HOLDER;

uint32_t SCORE = 0; // Starting score for eeprom
uint32_t HIGH_SCORE;
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
	SCORE++;
	lcd_draw_image(node->x_coord, missleWidthPixels, node->y_coord - 1, missleHeightPixels, missleBitmaps, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
	if(prev != (M_node *)0){
		prev->next = node->next;
		free(node);
		return prev->next;
	}
	prev = node->next;
	free(node);
	return prev;
	
}

//Pauses the game. Busy loop until unpaused.
void pause_game(void){
	while(paused) {}
	}

bool game_over(void){
	uint8_t data;
	printf("SCORE %i\n\r", SCORE);
	printf("HIGH_SCORE %i\n\r", HIGH_SCORE);
	printf("Press restart buttong to play again.");


	if(SCORE > HIGH_SCORE){
		//write the data to eeprom in little Endian
		data = SCORE & 0xFF;
		if (eeprom_byte_write(I2C1_BASE,ADDR_START, data) != I2C_OK) return false;
		data = SCORE >> 4;
		if (eeprom_byte_write(I2C1_BASE, ADDR_START + 1, data) != I2C_OK) return false;
	}
	
	lcd_clear_screen(LCD_COLOR_BLACK);
	while(1);
}
const uint8_t* getNumberMap(uint8_t num){
	switch(num){
		case 0: return number_0Bitmaps;
		case 1: return number_1Bitmaps;
		case 2: return number_2Bitmaps;
		case 3: return number_3Bitmaps;
		case 4: return number_4Bitmaps;
		case 5: return number_5Bitmaps;
		case 6: return number_6Bitmaps;
		case 7: return number_7Bitmaps;
		case 8: return number_8Bitmaps;
		case 9: return number_9Bitmaps;
	}
}
void display_High_Score(void){
	const uint8_t *first;
	const uint8_t *second;
	const uint8_t *third;
	int i;
	
	first = getNumberMap(HIGH_SCORE / 100);
	second = getNumberMap((HIGH_SCORE % 100) / 10);
	third = getNumberMap((HIGH_SCORE % 100) % 10);
	
	lcd_clear_screen(LCD_COLOR_BLACK);
	lcd_draw_image(BOARD_WIDTH / 2 - 25, number_WidthPixels, BOARD_HEIGHT/4, number_HeightPixels, first, LCD_COLOR_GREEN, LCD_COLOR_BLACK);
	lcd_draw_image(BOARD_WIDTH / 2, number_WidthPixels, BOARD_HEIGHT/4, number_HeightPixels, second, LCD_COLOR_GREEN, LCD_COLOR_BLACK);
	lcd_draw_image(BOARD_WIDTH / 2 + 25, number_WidthPixels, BOARD_HEIGHT/4, number_HeightPixels, third, LCD_COLOR_GREEN, LCD_COLOR_BLACK);
	for(i = 0; i < 10000000; i++){}

}


bool start_game(void){
	uint8_t data;
	char lcd_char;
	
	if (eeprom_byte_read(I2C1_BASE,ADDR_START, &data) != I2C_OK) return false;
	HIGH_SCORE = data;
	if (eeprom_byte_read(I2C1_BASE,ADDR_START + 1, &data) != I2C_OK) return false;
	HIGH_SCORE = HIGH_SCORE | (data << 4);
	

	display_High_Score();
	return true;
	
}
//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
	uint16_t missle_time;
	uint8_t button_data;
	M_node *newNode;
	uint8_t life_data;
	
	init_hardware();
	
	start_game();
	printf("Running...");
	lcd_clear_screen(LCD_COLOR_BLACK);
	s_missle = malloc(sizeof(M_node));
	s_missle->x_coord = BOARD_WIDTH / 4;
	s_missle->y_coord = BOARD_HEIGHT / 4;
	lcd_draw_image(s_missle->x_coord, missleWidthPixels, s_missle->y_coord, missleHeightPixels, missleBitmaps, LCD_COLOR_BLUE, LCD_COLOR_BLACK);
	
	lcd_draw_image(BOARD_WIDTH / 2,	bombHolderWidthPixels, BOARD_HEIGHT / 2, bombHolderHeightPixels, bombHolderBitmaps, LCD_COLOR_GREEN, LCD_COLOR_BLACK);
	lcd_draw_image(HEART_X, heartWidthPixels, HEART_Y, heartHeightPixels, heartBitmaps, LCD_COLOR_RED, LCD_COLOR_BLACK);

	//initialize our starting health
	life_data = 0xFF;
	io_expander_write_reg(MCP23017_GPIOA_R, life_data);
	missle_time = 0;
	
	//gp_timer_wait(TIMER2_BASE, 1000000);
		
    while(1){
			if(paused){
				pause_game();
			} // Pause game when spacebar is pressed
					
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
					
					//Case where missle reaches bottom of board
					if(node->y_coord > BOARD_HEIGHT - (missleHeightPixels / 2) - 10){
						node = clear_node(node, prev);
					}
					//Case where missle hits heart
					else if(node->y_coord + (missleHeightPixels / 2) >= HEART_Y - (heartHeightPixels / 2) && // missle is below top of heart
									((node->x_coord + (missleWidthPixels / 2) >= HEART_X - (heartWidthPixels / 2) 
									&& node->x_coord - (missleWidthPixels /2) <= HEART_X - (heartWidthPixels / 2)) ||  // left side of heart
									(node->x_coord - (missleWidthPixels / 2) <= HEART_X + (heartWidthPixels / 2) 
									&& node->x_coord + (missleWidthPixels /2) >= HEART_X + (heartWidthPixels / 2)))// right side of heart			
									) 
									{
										life_data = life_data >> 3;
										io_expander_write_reg(MCP23017_GPIOA_R, life_data);
										
										if(life_data == 0) game_over();
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
			if(ALERT_HEART){
				switch(PS2_DIR){
					case PS2_DIR_LEFT:
						if(HEART_X - 1 >= (heartWidthPixels / 2)+ 5){
							HEART_X--;
							lcd_draw_image(HEART_X, heartWidthPixels, HEART_Y, heartHeightPixels, heartBitmaps, LCD_COLOR_RED, LCD_COLOR_BLACK);
						}
						break;
					case PS2_DIR_RIGHT:
						if(HEART_X + 1 < BOARD_WIDTH - (heartWidthPixels / 2) - 5){
							HEART_X++;
							lcd_draw_image(HEART_X, heartWidthPixels, HEART_Y, heartHeightPixels, heartBitmaps, LCD_COLOR_RED, LCD_COLOR_BLACK);
						}
					default:
						break;
				}
			}
		};
}

