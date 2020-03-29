; Filename:     main.s 
; Author:       ece353 staff 
; Description:  

    export __main

;******************************************
; Symbolic Constants
;****************************************** 
WORD    EQU     4
  
  
;******************************************
; SRAM
;******************************************
    AREA    SRAM, READWRITE
RESULT		SPACE 1* WORD
;******************************************
; Load Store Review
;******************************************


;******************************************
; Load Store Multiple
;******************************************
DEST_ARRAY     SPACE   32*WORD
DEST2_ARRAY    SPACE   32*WORD
DEST3_ARRAY    SPACE   32*WORD
    align
        
        
        
        
        
;**********************************************
; FLASH
;**********************************************
    AREA    FLASH, CODE, READONLY
ASCII_ARRAY		DCB		"ECE353"
				DCB		0
;******************************************
; Load Store Review
;******************************************


;******************************************
; Load Store Multiple
;******************************************
SRC_ARRAY   DCD         0x00000000
            DCD         0x00001111
            DCD         0x00002222
            DCD         0x00003333
            DCD         0x00004444
            DCD         0x00005555
            DCD         0x00006666
            DCD         0x00007777
            DCD         0x00008888 
            DCD         0x00009999
            DCD         0x0000AAAA
            DCD         0x0000BBBB
            DCD         0x0000CCCC
            DCD         0x0000DDDD
            DCD         0x0000EEEE
            DCD         0x0000FFFF
            DCD         0x11110000
            DCD         0x11111111
            DCD         0x11112222
            DCD         0x11113333
            DCD         0x11114444
            DCD         0x11115555
            DCD         0x11116666
            DCD         0x11117777
            DCD         0x11118888
            DCD         0x11119999
            DCD         0x1111AAAA
            DCD         0x1111BBBB
            DCD         0x1111CCCC
            DCD         0x1111DDDD
            DCD         0x1111EEEE
            DCD         0x1111FFFF
    align





;**********************************************
; Code (FLASH) Segment
; main assembly program
;**********************************************
__main   PROC
  
    ;******************************************
    ; Load Store Review
    ;******************************************
    LDR R0, =(ASCII_ARRAY)
	LDR R1, =(RESULT)
	MOV R3, #0x0	; result counter
	MOV R7, #0x0   ;R0 indexing
WHILE_START
	LDR R2, [R0, R7]
	CMP R2, #0x0
	BEQ WHILE_DONE
	
	CMP R2, #0x30    ; check greater than 30
	BLT EXIT_CONDITION
	CMP R2, #0x39	; check less than 39
	BGT EXIT_CONDITION
	SUB R4, R2, #0x30
	ADD R3, R4, R3	
EXIT_CONDITION
	ADD R7, R7, #0x1
	B WHILE_START
WHILE_DONE

	STR R3, [R1]

    ;******************************************
    ; Load Store Multiple
    ;******************************************
    ; Load the address of SRC_ARRAY into R0
	LDR R0, =(SRC_ARRAY)
    
    ; Load the address of DEST_ARRAY into R1
	LDR R1, =(DEST_ARRAY)
    
    ; Load the address of DEST2_ARRAY into R2
	LDR R2, =(DEST2_ARRAY)
       
    ; Using LDM, load the first 8 WORDs in SRC_ARRAY
    ; Use Registers R3-R10 as the destination 
    ; of the LDM instruction
	
	LDM R0, {R3-R10}
    
    ; Use STM to store R3-R10 to DEST_ARRAY

	STM R1,	{R3-R10} 
     ; Use STM to store R3-R10 to DEST2_ARRAY
     ; Specify the order of the registers in
     ; revers order (STM R2, {R10, R9, R8, R7, R6, R5, R4, R3})
     ; Observe if the order of the registers changes
     ; how the data arranged in SRAM
	 STMDB R2, {R3-R10}


    
    ; Using LDM, load the second 8 WORDs in SRC_ARRAY
    ; Use Registers R3-R10 as the destination 
    ; of the LDM instruction.
    ; Hint, you will need to modify R0 so that
    ; it contains the address of SRC_ARRAY[8]
	ADD R0, R0, #32		; 8 * 4
	LDM R0, {R3-R10}


    ; Use STM to store R3-R10 to DEST_ARRAY[8]
    ; Hint, you will need to modify R1 so that
    ; it contains the address of DEST_ARRAY[8]
	ADD R1, R1, #32
	STM R1, {R3-R10}
    
    
    ; Copy the contents of SRC_ARRAY to DEST2_ARRAY
    ; 8 WORDs at a time using LDM/STM. This time use
    ; the version of LDM/STM that updates the base address
    ; Load the address of SRC_ARRAY into R0
	LDR R0, =(SRC_ARRAY)
	LDM R0!, {R3-R10}
	STM R2!, {R3-R10}
	LDM R0,	{R3-R10}
	STM R2, {R3-R10}
    ;out of time ":("
    
    ; Write code that uses a loop to copies SRC_ARRAY
    ; to DEST3_ARRAY.  Each iteraction of the loop should
    ; copy 4 WORDs of the array
    ; 
    ; for(i = 0; i < 8; i++)
    ; {
    ;       Copy 4 WORDs from SRC_ARRAY to DEST3_ARRAY
    ; }

    
 
    

    ; DO NOT MODIFY ANTHING BELOW THIS LINE!!!
        
INFINITE_LOOP
    B INFINITE_LOOP
    
    ENDP
    align
        

    END            
