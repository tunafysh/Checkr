; -------- CREATED BY: CYCLAWPS52 -------- ;

; -------- ENVIRONMENT -------- ;
    ; defines 16 bit env
    [bits 16]
    ; MBR is always loaded at offset 0x07C00
    ; make life easier by using relative references
    [org 0x7C00]

    mov ah, 07h
    int 10h

; -------- MAIN -------- ;
main:
    ; disable the blinky cursor because it looks better
    pushf
    push eax ; gets memory location of base program
    push edx ; sets bounds
    mov dx, 0x3D4
    mov al, 0xA   ; controls cursor shape
    out dx, al
    inc dx
    mov al, 0x20 ; bit 5 disables the cursor
    out dx, al
    pop edx
    pop eax ; clear the stack because I want to
    popf

    ; begin color display
    mov ax, cs
    mov ds, ax
    mov dx, 0 ; sets coordinate for background to start
    mov bh, 0
    mov ah, 0x2
    int 0x10
    mov cx, 2000 ; print count
    mov bh, 0
    mov bl, 0x0F ; 4=red bg, 0=black fg
    mov al, 0x20 ; blank char
    mov ah, 0x9
    int 0x10

    mov dx, 1985 ; sets text coordinates
    mov bh, 0
    mov ah, 0x2
    int 0x10
    mov si, lolString          
    call printString

; -------- FUNCTIONS -------- ;

printString:
    ;Does what the name says. Prints the text
    pusha
    cld

nextChar:
    mov al, [si]
    cmp al, 0            
    je endPrintString
    mov ah, 0x0e
    int 0x10
    inc si
    cmp si, 10
    je nextChar
    jmp printString

endPrintString:
    popa
    ret

; -------- VARIABLES -------- ;
lolString db "Checkmate.",10
; 32 (spaces) are just for time delay before string resets

; -------- BOOT CONFIGURATION -------- ;
    ; fill the remaining bytes of the MBR with 0s
    times 510 - ($ - $$) db 0
    ; add the 'magic bytes' AA and 55 to the end of MBR
    ; this tells the bios this is a valid bootloader
    dw 0xAA55