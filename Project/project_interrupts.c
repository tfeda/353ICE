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

#include "project_interrupts.h"

#define PS2_ADC_LOW_THRESHOLD		    0x0400
#define PS2_ADC_HIGH_THRESHOLD	    0xC00;

static volatile uint16_t PS2_X_DATA = 0;
static volatile uint16_t PS2_Y_DATA = 0;

//*****************************************************************************
// Returns the most current direction that was pressed.
//*****************************************************************************
PS2_DIR_t ps2_get_direction(void)
{
	PS2_DIR_t return_value=PS2_DIR; //Default, stay in the center
	
	if (PS2_X_DATA > 0xC00) return_value = PS2_DIR_LEFT; //Return Left
	
	else if (PS2_X_DATA < PS2_ADC_LOW_THRESHOLD) return_value = PS2_DIR_RIGHT;///Return Right
	
	else if (PS2_Y_DATA > 0xC00) return_value = PS2_DIR_UP; // Return Up
	
	else if (PS2_Y_DATA < PS2_ADC_LOW_THRESHOLD) return_value = PS2_DIR_DOWN; // Return Down
	
	else if (PS2_X_DATA > PS2_ADC_LOW_THRESHOLD && PS2_X_DATA < 0xC00) return_value = PS2_DIR_CENTER;
	
	return return_value;
	
}

void TIMER3A_Handler(void){
	ALERT_MISSLE = true;
	ALERT_HEART = true;
	TIMER3->ICR |= TIMER_ICR_TATOCINT;
}

void TIMER2A_Handler(void){
	uint8_t touch_event;
	uint16_t touch_x;
	uint16_t touch_y;
	touch_event = ft6x06_read_td_status();
	
	if (touch_event > 0)
		{
		  touch_x = ft6x06_read_x();
		  touch_y = ft6x06_read_y();
			
			CAP_TOUCH_X = touch_x < bombWidthPixels / 2 + 20? bombWidthPixels / 2 + 20: touch_x > BOARD_WIDTH - (bombWidthPixels / 2) - 20 ?
			BOARD_WIDTH - (bombWidthPixels / 2) - 20: touch_x;
			
			CAP_TOUCH_Y = touch_y < bombHeightPixels / 2 + 20? bombHeightPixels / 2 + 20: touch_y > BOARD_HEIGHT - (bombHeightPixels / 2) - 20 ?
			BOARD_HEIGHT - (bombHeightPixels / 2)  - 20: touch_y;
			
			ALERT_BOMB_HOLDER = true;
		}
		
	TIMER2->ICR |= TIMER_ICR_TATOCINT;
}
//*****************************************************************************
// TIMER4 ISR is used to trigger the ADC
//*****************************************************************************
void TIMER4A_Handler(void)
{	
	//Set off the Service Sequencer handler for the adc register controlling the ps2 analog stick
	ADC0->PSSI |= ADC_PSSI_SS2;
		// Clear the interrupt
	TIMER4->ICR |= TIMER_ICR_TATOCINT; 
}

//*****************************************************************************
// ADC0 SS2 ISR
//*****************************************************************************
void ADC0SS2_Handler(void)
{
	
	PS2_X_DATA = ADC0->SSFIFO2;
	PS2_Y_DATA = ADC0->SSFIFO2;
	
	PS2_DIR = ps2_get_direction();
	
	  // Clear the interrupt
  ADC0->ISC |= ADC_ISC_IN2;
}

void GPIOF_Handler() 
{
	//printf("entered GPIOF_handler\n");
		BUTTON_PRESSED = true;
		GPIOF->ICR |= GPIO_ICR_GPIO_M;
}