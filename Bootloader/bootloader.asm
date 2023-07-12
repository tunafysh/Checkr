; -------- CREATED BY: HANAN -------- ;

; -------- ENVIRONMENT -------- ;
    ; defines 16-bit environment
    [bits 16]
    ; MBR is always loaded at offset 0x07C00
    ; make life easier by using relative references
    [org 0x7C00]

    mov ah, 07h
    int 10h

; -------- MAIN -------- ;
main:
    ; disable the blinky cursor
    mov ah, 0x01
    mov cx, 0x2000
    int 0x10

    ; control cursor shape
    mov dx, 0x3D4
    mov al, 0x0A
    out dx, al
    inc dx
    mov al, 0x20
    out dx, al

    ; disable the cursor
    mov dx, 0x3D4
    mov al, 0x0A
    out dx, al
    inc dx
    mov al, 0x1F
    out dx, al

    ; initialize color display
    mov ax, cs
    mov ds, ax
    mov dx, 0
    mov bh, 0
    mov ah, 0x2
    int 0x10

    ; set background color to black and foreground color to white
    mov cx, 2000
    mov bh, 0
    mov bl, 0x0F
    mov al, 0x20
    mov ah, 0x9
    int 0x10

; -------- ADD BLINKING CURSOR -------- ;
addBlinkingCursor:
	; print "Checkmate."
	mov dx, 1985 ; sets text coordinates for "Checkmate"
	mov bh, 0
	mov ah, 0x2
	int 0x10
	
	; print blinking cursor at the end of "Checkmate."
	mov si,cursorString ; sets text coordinates for blinking cursor at the end of "Checkmate."
	call printString
	
	jmp $

; -------- FUNCTIONS -------- ;

printString:
	; Does what the name says. Prints the text.
	pusha 
	cld

nextChar:
	mov al,[si]
	cmp al,0 
	je endPrintString 
	mov ah,0xE 
	int 10h 
	inc si 
	jmp nextChar 

endPrintString:
	popa 
	ret 

; -------- VARIABLES -------- ;
checkmateString db "Checkmate",10 
cursorString db " ",7,".",10 

; -------- BOOT CONFIGURATION -------- ;
	; fill the remaining bytes of the MBR with 0s 
	times 510 - ($ - $$) db 0 
	; add the 'magic bytes' AA and 55 to the end of MBR 
	; this tells the BIOS this is a valid bootloader 
	dw 0xAA55 

