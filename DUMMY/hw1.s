; hw1.s ECE353


    export hw1_search_memory
	export hw1_init
	export hw1_ledx
	import WS2812B_write

;**********************************************
; SRAM
;**********************************************
    AREA    SRAM, READWRITE
LED_ARRAY SPACE 24

    align
		
;**********************************************
; Constant Variables (FLASH) Segment
;**********************************************
    AREA    FLASH, CODE, READONLY

hw1_search_memory PROC
	BX LR
	ENDP
		
		;Updates the LEDs according to LED array
hw1_update_leds PROC
	LDR R1, =(LED_ARRAY)
	MOV R2, #8
	PUSH {LR}
	BL WS2812B_write
	POP {LR}
	BX LR
	ENDP
	align
		
    align 

hw1_init PROC
	PUSH {R0-R2}
	LDR R0, =(LED_ARRAY)
	MOV R1, #0			; counter for Loop
	MOV R2, #0			; Value to store in LED_ARRAY indexes
	
INIT_LOOP_BEGIN			;Loop: set all values in LED_ARRAY to 0
	CMP R1, #24
	BGE INIT_LOOP_END
	STRB R2, [R0, R1]
	ADD R1, R1, #1
	B	INIT_LOOP_BEGIN

INIT_LOOP_END
	PUSH	{LR}
	BL hw1_update_leds ;Update the LEDs
	POP	{LR}
	POP {R0-R2}
	BX LR
	
	ENDP
	align
		
hw1_ledx PROC
	PUSH 	{LR}
	PUSH {R0, R2-R3}
	LDR R2, =(LED_ARRAY)
	
	MOV R3, #7
	SUB R0, R3, R0
	MOV R3, #3
	MUL R3, R0, R3
	ADD R3, R2, R3	; R3 now has LED address
	
	;GREEN CHANGE
	AND R2, R1, #0xFF0000
	MOV R2, R2, LSR #16 ;Shift the value to 0x00 - 0xFF
	STRB R2, [R3]
	
	;RED CHANGE
	AND R2, R1, #0xFF00
	MOV R2, R2, LSR #8
	STRB R2, [R3, #1]
	
	;BLUE CHANGE
	AND R2, R1, #0xFF
	STRB R2, [R3, #2]
	
	;UPDATE LED
	BL 		hw1_update_leds
	
	POP 	{R0, R2-R3}
	POP		{LR}
	BX 		LR
	ENDP
	align
    
    END



