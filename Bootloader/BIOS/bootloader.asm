; -------- ENVIRONMENT -------- ;
[bits 16]
[org 0x7C00]

; -------- MAIN -------- ;
main:
    ; Set up segments
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Save boot drive number
    mov [boot_drive], dl

    ; Clear the screen
    mov ah, 0x00
    mov al, 0x03
    int 0x10

    mov ah, 0x06   ; scroll up function
	mov al, 0      ; clear the whole screen
	mov bh, 0x0F   ; attribute (white on black)
	mov cx, 0      ; starting row and column (upper left corner)
	mov dx, 0x184F ; ending row and column (bottom right corner)
	int 0x10       ; call BIOS video interrupt


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

    ; Print first message
    mov dx, 1975 ; sets text coordinates
    mov bh, 0
    mov ah, 0x2
    int 0x10
    mov si, message
    call print_string

    ; Print second message
    mov dx, 2406 ; sets text coordinates
    mov bh, 0
    mov ah, 0x2
    int 0x10
    mov si, message2
    call print_string

    ; Print third message
    mov dx, 3005 ; sets text coordinates
    mov bh, 0
    mov ah, 0x2
    int 0x10
    mov si, message3
    call print_string

    ; Wait for 'P' key
    call wait_for_p_key

    ; Load additional sectors
    call load_snake

    ; If we return here, something went wrong
    jmp hang

; -------- FUNCTIONS -------- ;

white_text:
	mov cx, 2000
    mov bh, 0
    mov bl, 0x0F
    mov al, 0x20
    mov ah, 0x9
    int 0x10

	ret

green_text:
mov cx, 2000
mov bh, 0
mov bl, 0x02
mov al, 0x20
mov ah, 0x9
int 0x10

ret

print_string:
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

wait_for_p_key:
    mov ah, 0x00
    int 0x16
    or al, 0x20  ; Convert to lowercase
    cmp al, 'p'
    jne wait_for_p_key
    ret

load_snake:
    mov ah, 0x02        ; BIOS read sector function
    mov al, 2           ; Number of sectors to read (2nd and 3rd)
    mov ch, 0           ; Cylinder 0
    mov cl, 2           ; Start from sector 2
    mov dh, 0           ; Head 0
    mov dl, [boot_drive]; Drive number
    mov bx, 0x8000      ; Load to ES:BX = 0x0000:0x1000

    int 0x13
    jc read_error

    ; Jump to the loaded code
    jmp 0x0000:0x8000

read_error:
    mov si, error_msg
    call print_string
    jmp hang

hang:
    jmp hang

; -------- VARIABLES -------- ;
message db "Your computer has been infected", 0
message2 db "To fix it, win this game of snake.", 0
message3 db "Press P to continue.", 0
error_msg db "Error loading sectors!", 0x0D, 0x0A, 0
boot_drive db 0

; -------- BOOT CONFIGURATION -------- ;
times 510 - ($ - $$) db 0
dw 0xAA55

