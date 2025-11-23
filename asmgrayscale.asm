; Calculate and convert pixel values from int to float
section .data
    divisor dd 255.0        ; Single-precision float (32-bit)

section .text
    bits 64
    default rel
    global imgCvtGrayInttoFloat
    extern malloc

imgCvtGrayInttoFloat:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    
    mov r12, rcx            ; store 1st argument n in r12
    mov rbx, rdx            ; rbx = pointer to int array
    
    ; Allocate memory for float array (n * 4 bytes)
    mov rcx, r12
    imul rcx, 4             ; rcx = n * 4
    sub rsp, 32             ; Shadow space for malloc
    call malloc
    add rsp, 32
    
    ; Check if malloc failed
    test rax, rax
    jz .malloc_failed      ; If NULL, return NULL
    
    ; rax now holds pointer to float array
    mov r10, rax            ; r10 = pointer to output float array
    xor r11, r11            ; r11 = counter (i = 0)
    
    cmp r12, 0
    jle .done
    
.convert_loop:
    cmp r11, r12 ; loop through number of elements
    jge .done
    
    ; Load int value from input array
    mov ecx, dword[rbx + r11*4]  
    
    ; Convert int to float: divide by 255.0
    cvtsi2ss xmm0, ecx      ; xmm0 = (float)array[i] (use 32-bit register)
    movss xmm1, [rel divisor]
    divss xmm0, xmm1        ; xmm0 = array[i] / 255.0
    
    movss dword[r10 + r11*4], xmm0
    
    inc r11
    jmp .convert_loop
    
.malloc_failed:
    xor rax, rax            ; Return NULL
    jmp .cleanup
    
.done:
    mov rax, r10            ; Return pointer to float array
    
.cleanup:
    pop r12
    pop rbx
    mov rsp, rbp            ; Restore stack pointer
    pop rbp                 ; Restore base pointer
    ret