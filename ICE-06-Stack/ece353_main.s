; Filename:     main.s 
; Author:       ece353 staff 
; Description:  

    export __main
    import matrixMultiply

WORD        EQU     4
HALF_WORD   EQU     2
BYTE        EQU     1
    
NULL        EQU     0
SUCCESS     EQU     0
FAILED      EQU     -1
    
;**********************************************
; SRAM
;**********************************************
    AREA    SRAM, READWRITE
MATRIX_A    DCD 0x00000000
MATRIX_B    DCD 0x00000000
MATRIX_C    DCD 0x00000000
    align
        
;**********************************************
; Constant Variables (FLASH) Segment
;**********************************************
    AREA    FLASH, CODE, READONLY

    align

;**********************************************
; Initializes a variable in SRAM
; (Caller Saved)
;
; Paramters
;   R0 - Address
;   R1 - Bytes
;   R2 - Value
;
; Returns
;   R0 - 0 for success
;      - -1 for error
;**********************************************
initVar PROC
    
    ; This is a caller saved function, so there
    ; is no need to save any of the registers 
    ; that are modifed.
    
    ; If the Address is equal to 0, return -1
    CMP     R0, #NULL
    MOVEQ   R0, #FAILED
    BXEQ    LR
    
    ; if ( Data Size == 4)
    ; Word Store
    CMP     R1, #WORD
    STREQ   R2, [R0]
    MOVEQ   R0, #SUCCESS
    BXEQ    LR

    ; if ( Data Size == 2)
    ; Half Word Store
    CMP     R1, #HALF_WORD
    STRHEQ  R2, [R0]
    MOVEQ   R0, #SUCCESS
    BXEQ    LR       

    ; if (Data Size == 1)
    ; Byte Store
    CMP     R1, #BYTE
    STRBEQ  R2, [R0]
    MOVEQ   R0, #SUCCESS
    BXEQ    LR

   ; Any other word size return -1
   MOV      R0, #FAILED
   BX       LR
    
    ENDP
        
        
;**********************************************
; Code (FLASH) Segment
; main assembly program
;**********************************************
__main   PROC
    
    MOV     R1, #4
    LDR     R4, =(MATRIX_A)
    LDR     R5, =(MATRIX_B)
    LDR     R6, =(MATRIX_C)

    ; Initialize MATRIX_A
    ; <ADD CODE  1 >
	PUSH {R1, R4-R6}
	MOV R0, R4
	MOV32 R2, #0x07060504
	BL initVar
	POP  {R1, R4-R6}
    ; Initialize MATRIX_B
    ; <ADD CODE  2 >
    PUSH {R1, R4-R6}
	MOV R0, R5
	MOV32 R2, #0x07060504
	BL initVar
	
    ; Initialize MATRIX_C
    ; <ADD CODE  3 >
    PUSH {R4-R6}
	MOV R0, R6
	MOV R2, #0x0
	POP {R4-R6}
    
    ; Call matrixMultiply.  Pass the three paramters to using
    ; the stack
    ; <ADD CODE  4 >
    PUSH {R4}
	PUSH {R5}
	PUSH {R6}
	BL matrixMultiply
	POP {R6}
	POP {R5}
	POP {R4}
	
    ; Verify that MATRIX_C has a value
    ; of 0x1F1A0706.  If it does, branch to 
    ; PASSED
    ; <ADD CODE  10 >
	STR R0, [R6]
	MOV32 R1, #0x01F1A0706
	CMP R0, R1
	BEQ PASSED
    

FAIL
    B       FAIL

PASSED
    B       PASSED

    ; DO NOT MODIFY ANTHING BELOW THIS LINE!!!
        
INFINITE_LOOP
    B INFINITE_LOOP
    
    ENDP


        
    align
        

    END            