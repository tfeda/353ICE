#include <ws2812b.h>

void ws2812b_rotate(WS2812B_t *base, uint8_t num_leds){
	
	WS2812B_t temp = base[num_leds - 1];
	WS2812B_t temp2 = NULL;
	int i = 0;
	for(i = 0; i < num_leds - 1; i++){
		temp2 = base[i];

		base[i] = temp;
		temp = temp2;
	}
}

void ws2812b_pulse( WS2812B_t *base, uint8_t num_leds){
	static int direction = 1
	int i;
	for(i = 0; i < num_leds; i++){
		
	}
}