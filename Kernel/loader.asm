global loader
extern main
extern initializeKernelBinary
extern pit_handler
extern keyboard_handler
extern sys_read
extern sys_write
extern sys_malloc
extern sys_free
extern sys_cls
extern sys_exit
extern sys_sleep
extern sys_beep
extern sys_play_note
extern sys_list_ps
extern sys_create_process
extern sys_opipe
extern sys_cpipe
extern sys_wpipe
extern sys_rpipe
extern sys_kill
extern sys_draw_frect
extern sys_draw_char
extern sys_draw_circle
extern sys_draw_rect
extern sys_flush
extern sys_wait_pid
extern sys_get_pid
extern sys_list_ipcs
extern sys_mute

extern switch_user_to_kernel
extern switch_kernel_to_user

global force_schedule

;definicion de syscalls ids
READ            equ 1
WRITE           equ 2
MALLOC          equ 3
FREE            equ 4
CLS             equ 5
EXIT            equ 6
SLEEP           equ 7
BEEP            equ 8
NOTE            equ 9
LISTPROC        equ 10
CREATEPROC      equ 11
DRAWFRECT       equ 12
O_PIPE          equ 13 
C_PIPE          equ 14
W_PIPE          equ 15
R_PIPE          equ 16
KILL            equ 17
DRAWCHAR        equ 18
DRAWCIRCLE      equ 19
DRAWRECT        equ 20
FLUSHBUFFER     equ 21
WAITPID         equ 22
GETPID          equ 23
LISTIPCS        equ 24
MUTE            equ 25


%macro pushaq 0
    push rax      ;save current rax
    push rbx      ;save current rbx
    push rcx      ;save current rcx
    push rdx      ;save current rdx
    push rbp      ;save current rbp
    push rdi      ;save current rdi
    push rsi      ;save current rsi
    push r8       ;save current r8
    push r9       ;save current r9
    push r10      ;save current r10
    push r11      ;save current r11
    push r12      ;save current r12
    push r13      ;save current r13
    push r14      ;save current r14
    push r15      ;save current r15
    push fs
    push gs
%endmacro

%macro popaq 0
    pop gs
    pop fs
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

section .text

loader:
    call initializeKernelBinary	; Set up the kernel binary, and get thet stack address
    mov rsp, rax				; Set up the stack with the returned address
    call set_idt_handlers
    call start_pic
    call main
hang:
    hlt							; halt machine should kernel return
    jmp hang

;Inicializo el PIC
start_pic:    
    mov al, 11111000b							; Enable Cascade, Keyboard
    out 0x21, al
    sti                 ;Habilito las interrupciones
    ret

;Configuro la IDT
set_idt_handlers:
    mov rax, pit_hand
    mov rdi, 0x20
    call set_gate        ;seteo la interrupcion del PIT en el puerto 20h de la IDT

    mov rax, keyboard_hand
    mov rdi, 0x21
    call set_gate        ;seteo la interrupcion de teclado en 21h
    
    mov rax, soft_int
    mov rdi, 0x80
    call set_gate        ;seteo las interrupciones de software en 80h
    
    lidt [IDTRx64]       ;configuro el IDTR
    ret

;Seteo el puerto (gate) de cierta interrupcion
set_gate:
    push rax
    push rdi
    
    shl rdi, 4        ;RDI apunta a dir de memoria de la gate del IDT
    stosw               ;guardo AX en [RDI] (15....0)
    
    add rdi, 4          ;me salto el segment selector y flags del Gate
    shr rax, 16
    stosw               ;guardo AX (31...16)
    shr rax, 16
    stosd               ;guardo EAX (63-32)
    
    
    pop rdi
    pop rax
    ret

;Handler del PIT (el PIT manda por default una interrupcion con ~18Hz de frecuencia)
;entonces puedo usarlo como contador de segs.
ALIGN 8
pit_hand:
    	
    call pit_handler
    
    pushaq

    ; paso como parametro el reg rsp
    mov rdi, rsp

    ; paso al contexto del kernel seteando el rsp del kernel en el proceso actual
    call switch_user_to_kernel

    xor rax, rsp
    jz .end_pit
    
    mov rsp, rax

    ; paso al prox proceso, obteniendo su rsp y lo seteo
    call switch_kernel_to_user

    mov rsp, rax
    
.end_pit:
    mov al, 0x20
    out 0x20, al        ;mando EOI al PIC (master)
    
    popaq
    
    iretq


;Force re schedule
force_schedule:
    pop         QWORD[ret_addr]             ;Direccion de retorno

    mov         QWORD[ss_addr],     ss      ;Stack Segment
    push        QWORD[ss_addr]

    push        rsp
    pushf                                   ;Se pushean los flags
    mov         QWORD[cs_addr],     cs      ;Code Segment
    push        QWORD[cs_addr]
    push        QWORD[ret_addr]             ;Direccion de retorno

    ;En este momento el stack contiene:
    ;
    ; > red_addr
    ;   cs
    ;   rflags
    ;   rsp
    ;   ss

    pushaq

    mov         rdi,     rsp
    call        switch_user_to_kernel
    mov         rsp,    rax

    call        switch_kernel_to_user
    mov         rsp,     rax

    popaq
    iretq




;Handler del teclado.
;Levanto el scan code y se la paso a mi handler en C.
ALIGN 8
keyboard_hand:
    push rax
    push rdi
    
    xor rax, rax
    in al, 0x60        ;levanto el scan code del teclado
    
    mov rdi, rax
    call keyboard_handler
    
    mov al, 0x20
    out 0x20, al       ;mando EOI al PIC
    
    pop rdi
    pop rax
    iretq

;Handler de la interrupciones de soft.
;Recibe por RAX el codigo de la int y despues por RDI, RSI, etc los params.
ALIGN 8
soft_int:
    cmp rax, READ
    jz .sys_read_l
    
    cmp rax, WRITE
    jz .sys_write_l

    cmp rax, MALLOC
    jz .sys_malloc_l

    cmp rax, FREE
    jz .sys_free_l

    cmp rax, CLS
    jz .sys_cls_l

    cmp rax, EXIT
    jz .sys_exit_l
 
    cmp rax, SLEEP 
    jz .sys_sleep_l
    
    cmp rax, BEEP
    jz .sys_beep_l
    
    cmp rax, NOTE
    jz .sys_play_note_l

    cmp rax, LISTPROC
    jz .sys_list_process_l

    cmp rax, CREATEPROC
    jz .sys_create_process_l

    cmp rax, DRAWFRECT
    jz .sys_draw_frect_l

    cmp rax, O_PIPE
    jz .sys_open_pipe_l

    cmp rax, C_PIPE
    jz .sys_close_pipe_l

    cmp rax, W_PIPE
    jz .sys_write_pipe_l

    cmp rax, R_PIPE
    jz .sys_read_pipe_l

    cmp rax, KILL
    jz .sys_kill_l

    cmp rax, DRAWCHAR
    jz .sys_draw_char_l

    cmp rax, DRAWCIRCLE
    jz .sys_draw_circle_l

    cmp rax, DRAWRECT
    jz .sys_draw_rect_l

    cmp rax, FLUSHBUFFER
    jz .sys_flush_l

    cmp rax, WAITPID
    jz .sys_wait_pid_l

    cmp rax, GETPID
    jz .sys_get_pid_l

    cmp rax, LISTIPCS
    jz .sys_list_ipcs_l

    cmp rax, MUTE
    jz .sys_mute_l
    
    jmp .end
    
.sys_read_l:
    call sys_read
    jmp .end
    
.sys_write_l:
    call sys_write
    jmp .end

.sys_malloc_l:
    call sys_malloc
    jmp .end

.sys_free_l:
    ;call sys_free
    jmp .end

.sys_cls_l:
    call sys_cls
    jmp .end

.sys_exit_l:
    call sys_exit
    jmp .end

.sys_sleep_l:
    call sys_sleep
    jmp .end

.sys_beep_l:
    call sys_beep
    jmp .end

.sys_play_note_l:
    call sys_play_note
    jmp .end

.sys_list_process_l:
    call sys_list_ps
    jmp .end

.sys_create_process_l:
    call sys_create_process
    jmp .end

.sys_draw_frect_l:
    call sys_draw_frect
    jmp .end

.sys_open_pipe_l:
    call sys_opipe
    jmp .end

.sys_close_pipe_l:
    call sys_cpipe
    jmp .end

.sys_write_pipe_l:
    call sys_wpipe
    jmp .end
.sys_read_pipe_l:
    call sys_rpipe
    jmp .end

.sys_kill_l:
    call sys_kill
    jmp .end

.sys_draw_char_l:
    call sys_draw_char
    jmp .end

.sys_draw_circle_l:
    call sys_draw_circle
    jmp .end

.sys_draw_rect_l:
    call sys_draw_rect
    jmp .end

.sys_flush_l:
    call sys_flush
    jmp .end

.sys_wait_pid_l:
    call sys_wait_pid
    jmp .end

.sys_get_pid_l
    call sys_get_pid
    jmp .end

.sys_list_ipcs_l:
    call sys_list_ipcs
    jmp .end

.sys_mute_l:
    call sys_mute
    jmp .end

.end:
    push rax
    
    ;mov al, 0x20
    ;out 0x20, al        ;EOI al PIC que no hace falta porque no se usa el pic aca
    
    pop rax
    iretq
    

section .rodata
IDTRx64: dw 256*16-1       ;Limite de la IDT (4KB - 1)
         dq 0x0            ;Direccion de la IDT (Pure64 la carga en 0x0000000000000000)

section .bss

ret_addr:
        resq 1
cs_addr:
        resq 1
ss_addr:
        resq 1
