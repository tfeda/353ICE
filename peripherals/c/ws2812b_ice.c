#include <ws2812b.h>

void ws2812b_rotate(WS2812B_t *base, uint8_t num_leds){
	
	WS2812B_t temp = base[num_leds - 1];
	int i = 0;
	for(i = num_leds - 1; i > 0; i--){

		base[i] = base[i-1];
	}
	
	base[0] = temp;
}

void ws2812b_pulse( WS2812B_t *base, uint8_t num_leds){
	static int direction = 0;
	int z = 0;

	//Change the direction if we reach the top or bottom of the LEDs
	if((direction && base->red == 0xFF) || (!direction && base->red == 0x00)){
		direction = !direction;
	}
	for(z = 0; z < num_leds; z++){
		if(direction){	
			(base + z)->red++;
		}
		else{
			(base+z)->red--;
		}
	}
}
