; Lettuce (http://www.sconemad.com/lettuce)

	title "Lettuce PIC16F688 LINBus node"
	subtitle "Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>"

;
; This is the firmware for the Lettuce PIC16F688 simple LINBus node.
; This features: 8 general purpose I/O lines (PORTA & PORTC)
;

	list		p=16f688		; list directive to define processor
	#include	<P16F688.inc>	; processor specific variable definitions

	RADIX   	DEC				; Decimal radix
	
	__CONFIG    _CP_OFF & _CPD_OFF & _BOD_OFF & _PWRTE_ON & _WDT_OFF & _INTRC_OSC_NOCLKOUT & _MCLRE_OFF & _FCMEN_OFF & _IESO_OFF

; Pin assignments for this board
;
; R0 - RA0 (PGD)
; R1 - RA1 (PGC)
; R2 - RA2
; R3 - RA3 (MCLR)
; R4 - RC0
; R5 - RC1
; R6 - RC2
; R7 - RC3
;
; TX - RC4
; RX - RC5
;
; NSLP - RA5
; INH - RA4

CMD_READ_PORT	EQU	0x01
CMD_WRITE_PORT	EQU	0x02

;----------------------------------------------------------------------------
; VARIABLES
;
	cblock	0x20				; BANK 0 (max 80 bytes)
		timer1					; used in delay
		timer2					; used in delay
		cmd
		port
		value
		seq

		testmode
		testdelay
		testcount
	endc

	cblock	0x70				; ACCESS BANK (max 16 bytes)
		w_temp					; context saving
		status_temp				; context saving
		pclath_temp				; context saving
		eeprom_temp				; eeprom
		delay_temp				; delay
		index
	endc


;--------------------------------------------------------
; EEPROM
;
	ORG	0x2100					; data EEPROM location
	DE	1,2,3,4					; define first four EEPROM locations as 1, 2, 3, and 4


;----------------------------------------------------------------------------
; CODE ENTRY POINTS
;
	ORG			0x000			; processor reset vector
  	goto		reset			; go to beginning of program

	ORG			0x004			; interrupt vector location
	goto		int				; goto interrupt vector


;----------------------------------------------------------------------------
; STRING TABLE
;
lookup_string
	addwf		PCL,f
string_table

STR_Reset 		EQU $-string_table
	DT "16F871 is Reset\r\n",0

STR_Hello 		EQU $-string_table
	DT "Hello there you fool\r\n",0


;----------------------------------------------------------------------------
; INTERRUPT
;
int
	movwf		w_temp			; save off current W register contents
	movf		STATUS,w		; move status register into W register
	movwf		status_temp		; save off contents of STATUS register
	movf		PCLATH,w		; move pclath register into W register
	movwf		pclath_temp		; save off contents of PCLATH register

int_start
	clrf		PCLATH

	clrf		testmode
	
recv_cmd
	BANKSEL		seq
	movlw		0
	subwf		seq,w
	btfss		STATUS,Z
	goto		recv_value
	BANKSEL		RCREG
	movf		RCREG,w
	movwf		cmd

; CMD_READ_PORT - return value

	movlw		CMD_READ_PORT
	subwf		cmd,w
	btfss		STATUS,Z
	goto		recv_value

; send all port data

	BANKSEL		TRISA			; Set port tristates to input
	bsf			TRISA,0
	bsf			TRISA,1
	bsf			TRISA,2
	bsf			TRISA,3
	BANKSEL		TRISC
	bsf			TRISC,0
	bsf			TRISC,1
	bsf			TRISC,2
	bsf			TRISC,3

	BANKSEL		PORTA			; Compile value from PORTA
	clrf		value
	btfsc		PORTA,0
	bsf			value,0
	btfsc		PORTA,1
	bsf			value,1
	btfsc		PORTA,2
	bsf			value,2
	btfsc		PORTA,3
	bsf			value,3

	BANKSEL		PORTC			; Compile value from PORTC
	btfsc		PORTC,0
	bsf			value,4
	btfsc		PORTC,1
	bsf			value,5
	btfsc		PORTC,2
	bsf			value,6
	btfsc		PORTC,3
	bsf			value,7

	movf		value,w
	call		send

	goto		recv_endcmd	

recv_value
	movlw		1
	subwf		seq,w
	btfss		STATUS,Z
	goto		recv_next
	BANKSEL		RCREG
	movf		RCREG,w
	movwf		value

; CMD_WRITE_PORT <value> - return NULL

	movlw		CMD_WRITE_PORT
	subwf		cmd,w
	btfss		STATUS,Z
	goto		recv_next

	BANKSEL		TRISA			; Set port tristates to output
	bcf			TRISA,0
	bcf			TRISA,1
	bcf			TRISA,2
	bcf			TRISA,3
	BANKSEL		TRISC
	bcf			TRISC,0
	bcf			TRISC,1
	bcf			TRISC,2
	bcf			TRISC,3

	BANKSEL		PORTA
	btfsc		value,0
	bsf			PORTA,0
	btfss		value,0
	bcf			PORTA,0
	btfsc		value,1
	bsf			PORTA,1
	btfss		value,1
	bcf			PORTA,1
	btfsc		value,2
	bsf			PORTA,2
	btfss		value,2
	bcf			PORTA,2
	btfsc		value,3
	bsf			PORTA,3
	btfss		value,3
	bcf			PORTA,3

	BANKSEL		PORTC
	btfsc		value,4
	bsf			PORTC,0
	btfss		value,4
	bcf			PORTC,0
	btfsc		value,5
	bsf			PORTC,1
	btfss		value,5
	bcf			PORTC,1
	btfsc		value,6
	bsf			PORTC,2
	btfss		value,6
	bcf			PORTC,2
	btfsc		value,7
	bsf			PORTC,3
	btfss		value,7
	bcf			PORTC,3

	movlw		0
	call		send
	
	goto		recv_endcmd

recv_next
	incf		seq,f			; increment sequence

	movlw		2				; check for end of sequence
	subwf		seq,w
	btfss		STATUS,Z
	goto		int_end

recv_endcmd
	clrf		seq 			; restart sequence

int_end
	movf		pclath_temp,w	; retrieve copy of PCLATH register
	movwf		PCLATH			; restore pre-isr PCLATH register contents	
	movf		status_temp,w	; retrieve copy of STATUS register
	movwf		STATUS			; restore pre-isr STATUS register contents
	swapf		w_temp,f
	swapf		w_temp,w		; restore pre-isr W register contents
	retfie						; return from interrupt


;----------------------------------------------------------------------------
; delay_ms - delay for w milliseconds
;
delay_ms
    movwf		delay_temp      ; 1 ins (A)
    movlw   	249             ; 1 ins (A)
delay_ms_loop
	goto    	$+1             ; 2 ins (A,B,C)
	goto    	$+1             ; 2 ins (A,B,C)
	addlw   	-1              ; 1 ins (A,B,C)
	btfss   	STATUS,Z        ; 1 ins (C) / 2 ins (A,B)
	goto    	delay_ms_loop   ; 2 ins (C)
	decf    	delay_temp,F    ; 1 ins (A,B)
	btfsc   	STATUS,Z        ; 2 ins (B) / 1 ins (A)
	return                  	; 2 ins (A)
	goto    	$+1             ; 2 ins (B)
	nop                     	; 1 ins (B)
	movlw   	249             ; 1 ins (B)
	goto    	delay_ms_loop   ; 2 ins (B)


;----------------------------------------------------------------------------
; delay_us - delay for w microseconds
;

delay_us                        ;               [0mod2  1mod2]
	movwf		delay_temp      ; 1 ins          1      1
	btfsc		delay_temp,0    ; 1 ins / 2 ins  3      2
	goto		delay_2us       ; 2 ins                 4
delay_2us                       ; static delay 5 ins = 2.5us
	nop							; 1 ins
	rrf			delay_temp,F    ; 1 ins
	movlw		3               ; 1 ins
	subwf		delay_temp,W    ; 1 ins
	andlw		b'01111111'     ; 1 ins
delay_2us_loop                  ; static delay 5+3 ins = 4us
	addlw		-1              ; 1 ins (static,loop)
	btfss		STATUS,Z        ; 1 ins (loop) / 2 ins (static)
	goto		delay_2us_loop  ; 2 ins (loop)
	return						; 2 ins (static)


;----------------------------------------------------------------------------
; eeprom_read - read data byte from EEPROM
;
; w (in)          - Address to read from
; w (out)         - Data read from address
;
eeprom_read
	BANKSEL		EEADR           ; Select RAM bank
	movwf		EEADR
	BANKSEL		EECON1          ; Select RAM bank
	bcf     	EECON1,EEPGD    ; Select bank of EECON1
	bsf     	EECON1,RD       ; Start the read
	BANKSEL 	EEDATA          ; Select RAM bank
	movf   		EEDATA,W
	return


;----------------------------------------------------------------------------
; eeprom_write - write data byte to EEPROM
;
; w (in)          - Address to write to
; EEDATA          - Data to write to address
;
eeprom_write
	BANKSEL 	EEADR           ; Select RAM bank
	movwf   	EEADR
	movf    	INTCON,W
	bcf     	INTCON,GIE      ; Disable INTs
	BANKSEL 	EECON1          ; Select RAM bank
	movwf   	eeprom_temp
	bcf     	EECON1,EEPGD    ; Select bank of EECON1
	bsf     	EECON1,WREN     ; Enable EEPROM writes
	movlw   	0x55            ; \
	movwf   	EECON2          ;  \ Special sequence to
	movlw   	0xAA            ;  / write to EEPROM
	movwf   	EECON2          ; /
	bsf     	EECON1,WR       ; Start the write
	btfsc   	EECON1,WR       ; Check if write is complete
	goto    	$-1             ; Wait for write to complete
	bcf     	EECON1,WREN     ; Disable EEPROM writes
	btfsc   	eeprom_temp,GIE
	bsf     	INTCON,GIE      ; Enable INTs if enabled to start with
	BANKSEL 	EEDATA          ; Select RAM bank
	return


;----------------------------------------------------------------------------
; send - transmit a byte using USART
;
send
	BANKSEL 	TXREG
	movwf		TXREG
	bsf			STATUS,RP0
send_wait
	BANKSEL 	TXSTA
	btfss		TXSTA,TRMT
	goto		send_wait
	bcf			STATUS,RP0
	return


;----------------------------------------------------------------------------
; send_string - transmit a string using USART
;
send_string
	movwf		index
send_string_loop
	movf		index,W
	call		lookup_string
	addlw		0
	btfsc		STATUS,Z
	return
	call		send
	incf		index,F
	goto		send_string_loop


;----------------------------------------------------------------------------
; MAIN PROGRAM
;
reset

; General setup

	BANKSEL		OSCCON	
	movlw		b'01110111' ; Select internal 8MHz oscillator
	movwf		OSCCON

	BANKSEL		OPTION_REG
	movlw		b'11111000'
	movwf		OPTION_REG

; Setup interrupts

	BANKSEL		INTCON
	movlw		b'01000000'	; Enable peripheral ints
	movwf		INTCON

	BANKSEL		PIE1
	movlw		b'00100000'	; Interrupts
	movwf		PIE1

; Setup ports

	BANKSEL		PORTA
	movlw		b'00000000' ; PORT A
	movwf		PORTA

	BANKSEL		TRISA
;	movlw		b'00010000'	; TRIS A
	movlw		b'00011000'	; TRIS A
	movwf		TRISA

	BANKSEL		PORTC
	movlw		b'00010000' ; PORT C - default TX line high
	movwf		PORTC

	BANKSEL		TRISC
	movlw		b'00100000'	; TRIS C
	movwf		TRISC

; Setup USART

	BANKSEL		RCSTA
	movlw		b'10010000' ; Setup RCSTA
	movwf		RCSTA

	BANKSEL		TXSTA
	movlw		b'00100100'	; TXSTA
	movwf		TXSTA

	BANKSEL		BAUDCTL
	movlw		b'00001011'	; ABDEN=1 WUE=1
	movwf		BAUDCTL

; Put the MAX13021 into normal slope mode by holding NSLP high

	BANKSEL		PORTC
	bsf			PORTC,4
	movlw		10
	call		delay_us	

	BANKSEL		PORTA
	bsf			PORTA,5
	movlw		10
	call		delay_us	

; Initialise variables

	BANKSEL		seq
	clrf		seq
	clrf		testmode
	incf		testmode,f

; Globally enable interrupts and we are ready to go...

	BANKSEL		INTCON
	bsf			INTCON,GIE	

;	movlw		STR_Reset
;	call		send_string

;--------------------------------------------------------

main_loop

	BANKSEL		testmode
;	btfss		testmode,0
;	goto 		main_loop

delay1
	movlw		0x01
	call		delay_ms

	BANKSEL		value
	incf		value,f

	movf		value,w
	movwf		PORTA

	goto		main_loop

	END

