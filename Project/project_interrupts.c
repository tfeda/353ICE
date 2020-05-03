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
#include "project_interrupts.h"


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



