; hw1.s ECE353


    export hw1_search_memory
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

;Search through memory byte by byte and responde to commands that
;are given for the board.
hw1_search_memory PROC
	PUSH {LR}
	PUSH {R0-R4}
	MOV R1, #0
	MOV R2, #0
	MOV R3, #0
	MOV R4, #0
	
	MOV R2, R0
	LDRB R1, [R2], #1
	
SEARCH_LOOP_BEGIN
	;Use the first byte to determine what command it would be
	CMP R1, #83    ;'S' GOTO STRT
	BLEQ SEARCH_STRT
	CMP R1, #66		; 'B' GOTO BREK
	BLEQ SEARCH_BREK
	CMP R1, #72		; 'H' GOTO HOLD
	BLEQ SEARCH_HOLD
	CMP R1, #76		; 'L' GOTO LEDx
	BEQ SEARCH_LED
	
	LDRB R1, [R2], #1
	B SEARCH_LOOP_BEGIN  ; default, the current command is invalid, so 
							; we load the next ascii character in
SEARCH_LED
	LDRB R1, [R2], #1
	CMP R1, #69		; 'E'
	BLNE SEARCH_LOOP_BEGIN
	
	LDRB R1, [R2], #1
	CMP R1, #68		; 'D'
	BLNE SEARCH_LOOP_BEGIN
	
	;Make sure the 4th character is 0-7
	LDRB R1, [R2], #1
	CMP R1, #48		; '0'
	BLLT SEARCH_LOOP_BEGIN
	CMP R1, #55		; '7'
	BLGT	SEARCH_LOOP_BEGIN
	SUB R3, R1, #48		; R3 now holds the next character for LEDX Call
	
	;Valid Green
	LDRB R1, [R2], #1
	MOV R0, R1
	BL hw1_ascii_to_hex
	CMP R0, #0xFFFFFFFF
	BLEQ SEARCH_LOOP_BEGIN
	MOV R4, R0, LSL #20
	
	LDRB R1, [R2], #1
	MOV R0, R1
	BL hw1_ascii_to_hex
	CMP R0, #0xFFFFFFFF
	BLEQ SEARCH_LOOP_BEGIN
	ADD R4, R0, LSL #16
	
	;Valid Red
	LDRB R1, [R2], #1
	MOV R0, R1
	BL hw1_ascii_to_hex
	CMP R0, #0xFFFFFFFF
	BLEQ SEARCH_LOOP_BEGIN
	ADD R4, R0, LSL #12
	
	LDRB R1, [R2], #1
	MOV R0, R1
	BL hw1_ascii_to_hex
	CMP R0, #0xFFFFFFFF
	BLEQ SEARCH_LOOP_BEGIN
	ADD R4, R0, LSL #8
	
	;Valid Blue
	LDRB R1, [R2], #1
	MOV R0, R1
	BL hw1_ascii_to_hex
	CMP R0, #0xFFFFFFFF
	BLEQ SEARCH_LOOP_BEGIN
	ADD R4, R0, LSL #4
	
	LDRB R1, [R2], #1
	MOV R0, R1
	BL hw1_ascii_to_hex
	CMP R0, #0xFFFFFFFF
	BLEQ SEARCH_LOOP_BEGIN		
	ADD R4, R0
	
	;Valid Command, Call LEDx
	MOV R0, R3
	MOV R1, R4
	BL hw1_ledx
	LDRB R1, [R2], #1
	BL SEARCH_LOOP_BEGIN

SEARCH_HOLD
	LDRB R1, [R2], #1
	CMP R1, #79		; 'O'
	BNE SEARCH_LOOP_BEGIN
	
	LDRB R1, [R2], #1
	CMP R1, #76		; 'L'
	BNE SEARCH_LOOP_BEGIN
	
	LDRB R1, [R2], #1
	CMP R1, #68		; 'D'
	BNE SEARCH_LOOP_BEGIN
	
	;1000 index
	LDRB R1, [R2], #1
	MOV R0, R1
	BL hw1_ascii_to_dec
	CMP R0, #0xFFFFFFFF
	BEQ SEARCH_LOOP_BEGIN
	MOV R4, #1000
	MUL R3, R0, R4
	
	;100 index
	LDRB R1, [R2], #1
	MOV R0, R1
	BL hw1_ascii_to_dec
	CMP R0, #0xFFFFFFFF
	BEQ SEARCH_LOOP_BEGIN
	MOV R4, #100
	MUL R0, R0, R4
	ADD R3, R3, R0
	
	;10 index
	LDRB R1, [R2], #1
	MOV R0, R1
	BL hw1_ascii_to_dec
	CMP R0, #0xFFFFFFFF
	BEQ SEARCH_LOOP_BEGIN
	MOV R4, #10
	MUL R0, R0, R4
	ADD R3, R3, R0
	
	;1 index
	LDRB R1, [R2], #1
	MOV R0, R1
	BL hw1_ascii_to_dec
	CMP R0, #0xFFFFFFFF
	BEQ SEARCH_LOOP_BEGIN
	ADD R3, R3, R0
	
	;Valid Hold Command, Call wait
	MOV R0, R3
	MOV R3, #10000
	MUL R0, R0, R3
	BL hw1_wait
	B SEARCH_LOOP_BEGIN
	
	
SEARCH_BREK
	LDRB R1, [R2], #1
	CMP R1, #82		; 'R'
	BNE SEARCH_LOOP_BEGIN
	
	LDRB R1, [R2], #1
	CMP R1, #69		; 'E'
	BNE SEARCH_LOOP_BEGIN
	
	LDRB R1, [R2], #1
	CMP R1, #75		; 'K'
	BNE SEARCH_LOOP_BEGIN
	
	;Valid Command, begin infinite Loop
INF_LOOP
	B INF_LOOP
	
SEARCH_STRT
	LDRB R1, [R2], #1
	CMP R1, #84		; 'T'
	BNE SEARCH_LOOP_BEGIN
	
	LDRB R1, [R2], #1
	CMP R1, #82		; 'R'
	BNE SEARCH_LOOP_BEGIN
	
	LDRB R1, [R2], #1
	CMP R1, #84		; 'T'
	BNE SEARCH_LOOP_BEGIN
	
	;Valid STRT Command, initialize the board
	BL hw1_init
	LDRB R1, [R2], #1
	B SEARCH_LOOP_BEGIN
	
	POP {R0-R4}
	BX LR
	ENDP
		
    align        
   
 
;Params:
;	R0; Character to convert
;Returns:
;	R0 Decimal of value if valid, otherwise0xFFFFFFFF
hw1_ascii_to_hex PROC
	
	CMP R0, #48			; If below 48, it will always be an error
	BLT HEX_ERROR_RET
	CMP R0, #58			; Must be numeral if 48 <= R0 < 58
	BLT HEX_NUMERAL_RET
	CMP R0, #65			; Must be error if 58 <= R0 < 65
	BLT HEX_ERROR_RET
	CMP R0, #71			; Must be Uppercase letter if 65 <= R0 < 71
	BLT HEX_UPPER_RET
	CMP R0, #97			; Must be error if 71 <= R0 < 97
	BLT HEX_ERROR_RET
	CMP R0, #102		; Must be error if R0 > 102
	BGT HEX_ERROR_RET
	
	SUB R0, R0, #87		; Must be Lowercase letter
	BX LR
	
HEX_UPPER_RET
	SUB R0, R0, #55
	BX LR
	
HEX_NUMERAL_RET
	SUB R0, R0, #48
	BX LR
	
HEX_ERROR_RET
	MOV R0, #0xFFFFFFFF
	BX LR
	
	ENDP
	align

;Takes in an ascii value and converts it to hex
;Params:
;	R0; Character to convert
;Returns:
;	R0 Decimal of value if valid, otherwise0xFFFFFFFF
hw1_ascii_to_dec PROC
	
	CMP R0, #48			; If below 48, it will always be an error
	BLT DEC_ERROR_RET
	CMP R0, #58			; Must be ERROR if R0 > 57
	BGT DEC_ERROR_RET
	
	SUB R0, R0, #48
	BX LR
	
DEC_ERROR_RET
	MOV R0, #0xFFFFFFFF
	BX LR
	
	ENDP
	align

;Initializes the LEDs by turning them all off -> sets all values
; in LED array to 0 and updates the board
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
	BL hw1_update_leds 	;Update the LEDs
	POP	{LR}
	POP {R0-R2}
	BX LR
	
	ENDP
	align

;Executes an infinite loop for a specified amount of time.
;Params:
;	R0, 32 bit unsigned number represending number of iterations for loop
hw1_wait PROC
	PUSH{R0}

;Beginning of the loop
WAIT_BEGIN
	CMP R0, #0
	BEQ WAIT_END
	SUB R0, R0, #1
	B	WAIT_BEGIN

WAIT_END
	POP{R0}
	BX LR
	ENDP
	align

;Updates the specified LED to the specified intensity
;Params
;	R0: LED index to update
;	R1:
;		Byte3: Nothing
;		Byte2: Green Intensity
;		Byte1: Red Intensity
;		Byte0: Blue Intensity
hw1_ledx PROC
	PUSH 	{LR}
	PUSH {R0, R2-R3}
	LDR R2, =(LED_ARRAY)
	
	;Do math wumbo jumbo to get the right LED index
	MOV R3, #7
	SUB R0, R3, R0
	MOV R3, #3
	MUL R3, R0, R3
	ADD R3, R2, R3	; R3 now has the LED index's address
	
	;GREEN CHANGE
	AND R2, R1, #0xFF0000
	MOV R2, R2, LSR #16 ;Shift the value to 0x00 - 0xFF
	STRB R2, [R3]
	
	;RED CHANGE
	AND R2, R1, #0xFF00
	MOV R2, R2, LSR #8  ;Shift the value to 0x00 - 0xFF
	STRB R2, [R3, #1]
	
	;BLUE CHANGE
	AND R2, R1, #0xFF	;Don't need to shift, already there
	STRB R2, [R3, #2]
	
	;UPDATE LED
	BL 		hw1_update_leds
	
	POP 	{R0, R2-R3}
	POP		{LR}
	BX 		LR
	ENDP
	align

	END
