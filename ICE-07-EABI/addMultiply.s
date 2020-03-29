; Filename:     addMultiply.s 
; Author:       ece353 staff 
; Description:  

    export addMultiply

;**********************************************
; SRAM
;**********************************************
    AREA    SRAM, READWRITE
    align
        
;**********************************************
; Constant Variables (FLASH) Segment
;**********************************************
    AREA    FLASH, CODE, READONLY
    align


;**********************************************
; Four arrays of 8-bit signed numbers are 
; passed via the first four paramters.
; The 5th paramter indicates the length of the
; arrays. For each entry in the array, the
; following operation takes place.
;
; Array3[i] = (Array0[i] + Array1[i]) * Array2[i]
;
; Parameter 0       Array Address 0
; Parameter 1       Array Address 1
; Parameter 2       Array Address 2
; Parameter 3       Array Address 3
; Parameter 4       Array Size 
;
; Returns
;   if ALL parameters are valid, return 0
;   else return -1.
;
;  An address is valid if it is non zero
;  The size is valid if it is greater than zero
;**********************************************
addMultiply PROC
    PUSH {R4-R7}
    ; Validate Parameters
	CMP R0, #0
	BEQ INVALID_END
	CMP R1, #0
	BEQ INVALID_END
	CMP R2, #0
	BEQ INVALID_END
	CMP R3, #0
	BEQ INVALID_END
	
	
    ; Save required registers
	LDR R4, [SP, #16]
	CMP R4, #0
	BLT INVALID_END
	
	MOV R5, #0

LOOP_BEGIN
	CMP R4, R5
	BLE LOOP_END
	
	LDRB R6, [R0, R5]
	LDRB R7, [R1, R5]
	ADD R6, R6, R7
	LDRB R7, [R2, R5]
	MUL R6, R6, R7
	STRB R6, [R3, R5]
	
	ADD R5, R5, #1
	B LOOP_BEGIN
    ; For each index in the arrays, compute  
    ; Array3[i] = (Array0[i] + Array1[i]) * Array2[i]
  
    ; Restore registers saved to the stack
LOOP_END
    POP {R4-R7}
    ; Return from the loop
	MOV R0, #0
	BX LR
	
INVALID_END
	POP {R4-R7}
	MOV R0, #-1
	BX	LR

    ENDP


        
    align
        

    END            
