; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

I			EQU	0x3B9ACA00
TEN 		EQU 0xA
SPECIAL 	EQU 0x270F 
THOUSAND 	EQU 0x3E8
	PRESERVE8
    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
; R0=0,    then output "0"
; R0=3,    then output "3"
; R0=89 = #0x59, 0101   then output "89"
; R0=123,  then output "123"
; R0=9999, then output "9999"
; R0=4294967295, then output "4294967295"

LCD_OutDec
		PUSH{R4,R5}
		LDR R1,= I ; 1 billion
		LDR R3,= TEN
firstLoop

		UDIV R2, R0, R1 ; R2= R0/I
		CMP R2, #0x00
		BEQ repeatAgain ; KEep going until we find first number
		BNE printDec

repeatAgain
		UDIV R1, R1, R3 ; I=I/10
		CMP R1, #0x00 ; Checks if I = 0, last digit
		BEQ printDec ; If it is, that means number overall is 0
		B firstLoop

printDec
		PUSH{R0}
		MOV R0, R2 ; R0 <- R2 (R0/I)
		ADD R0, #0x30 ; Convert to ASCII
		PUSH{R0,R1,R2,R3,R4,LR}
		BL ST7735_OutChar
		POP{R0,R1,R2,R3,R4,LR}
		
		POP{R0}
		;MOV R0,R4 ; Place original R0 back into R0
		MUL R4, R2, R1 ; R4 <- (R0/I)*I
		SUB R0, R0, R4 ; R0 = R0-(R0/I)*I
		UDIV R1, R1, R3 ; I=I/10
		CMP R1, #0x00 ; Checks if I = 0, last digit
		BEQ outDecDone
		UDIV R2, R0, R1 ; R2 <- R0/I
		B printDec
				
				
				
outDecDone
		POP{R4,R5}
				BX  LR
		
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000"
;       R0=3,    then output "0.003"
;       R0=89,   then output "0.089"
;       R0=123,  then output "0.123"
;       R0=9999, then output "9.999"
;       R0>9999, then output "*.***"
; Invariables: This function must not permanently modify registers R4 to R11

LCD_OutFix
				
				LDR R1,= THOUSAND
				MOV R2, #4
				LDR R3,= SPECIAL
				LDR R5,= TEN
				
				CMP R0, R3
				BHI specialCondition
				
fixCheck		CMP R2, #0x00 ; Counter 4 digits
				BLT doneLCD ; If its less than, finished printing
				CMP R2, #0x03 ; Check if it needs to print decimal
				BEQ fixDecimal
				
				PUSH{R0} ; push original R0 on stack
				UDIV R0, R0, R1  ; R0 = R0/I
				MOV R6, R0 ; R6<- R0
				MUL R6, R0, R1; R6<- R0*I
				ADD R0, #0x30 ; Convert to ASCII
				PUSH{R0,R1,R2,R3,R4,LR}
				BL ST7735_OutChar
				POP{R0,R1,R2,R3,R4,LR}
				
				POP{R0}
				
				UDIV R1, R1, R5 ; R1 = R1/10
				SUB R0, R0, R6 ; R0=R0-(R0/R1)*R1
				SUB R2, #0x01 ; Decrement
				B fixCheck
				
fixDecimal
				PUSH{R0}
				MOV R0, #0x2E
				PUSH{R0,R1,R2,R3,R4,LR}
				BL ST7735_OutChar
				POP{R0,R1,R2,R3,R4,LR}
				SUB R2, #0x01
				POP{R0}
				B fixCheck
				
specialCondition
specialCheck	CMP R2, #0x00
				BLT doneLCD
				CMP R2, #0x03
				BEQ specDec
				MOV R0, #0x2A
				PUSH{R0,R1,R2,R3,R4,LR}
				BL ST7735_OutChar
				POP{R0,R1,R2,R3,R4,LR}
				SUB R2, #0x01
				B specialCheck
specDec			MOV R0, #0x2E
				PUSH{R0,R1,R2,R3,R4,LR}
				BL ST7735_OutChar
				POP{R0,R1,R2,R3,R4,LR}
				SUB R2, #0x01
				B specialCheck

doneLCD			BX   LR
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file