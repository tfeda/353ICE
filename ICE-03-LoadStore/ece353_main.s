; Filename:     main.s 
; Author:       ece353 staff 
; Description:  

    export __main

;**********************************************
; Add Symbolic Constants here
;**********************************************
BYTE 		EQU 		1
HALF_WORD 	EQU			2
WORD		EQU			4
;**********************************************
; SRAM
;**********************************************
    AREA    SRAM, READWRITE
ARRAY1	SPACE	8 * HALF_WORD
ARRAY2	SPACE	8 * HALF_WORD
    align
        
;**********************************************
; Constant Variables (FLASH) Segment
;**********************************************
    AREA    FLASH, CODE, READONLY
LTABLE	DCW		0
		DCW		1
		DCW		8
		DCW		27
		DCW		64
		DCW		125
		DCW		216
		DCW		343
    align

;**********************************************
; Code (FLASH) Segment
; main assembly program
;**********************************************
__main   PROC
	LDR R0, =(LTABLE)
	LDR R1, =(ARRAY1)
	LDR R2, =(ARRAY2)
	LDRH R10, [R0, #0x6]
	
	LDRH R3, [R0]
	LDRH R4, [R0, #0x2]
	LDRH R5, [R0, #0x4]
	LDRH R6, [R0, #0x6]
	LDRH R7, [R0, #0x8]
	LDRH R8, [R0, #0xA]
	LDRH R9, [R0, #0xC]
	
	;pre-indexed
	STRH R3, [R1]
	STRH R4, [R1, #0x2]
	STRH R5, [R1, #0x4]
	STRH R6, [R1, #0x6]
	STRH R7, [R1, #0x8]
	STRH R8, [R1, #0xA]
	STRH R9, [R1, #0xC]
	
	;post-indexed
	STRH R3, [R2], #0x2
	STRH R4, [R2], #0x2
	STRH R5, [R2], #0x2
	STRH R6, [R2], #0x2
	STRH R7, [R2], #0x2
	STRH R8, [R2], #0x2
	STRH R9, [R2], #0x2
	
	
	
	
    
    ; DO NOT MODIFY ANTHING BELOW THIS LINE!!!	
        
INFINITE_LOOP
    B INFINITE_LOOP
    
    ENDP
    align
        

    END            