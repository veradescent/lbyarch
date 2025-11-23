; Calculate and convert pixel values from int to float
section .data
    divisor dq 255.0

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
    cvtsi2ss xmm0, rcx      ; xmm0 = (double)array[i]
    movss xmm1, [rel divisor]
    divss xmm0, xmm1        ; xmm0 = array[i] / 255.0
    
    movss dword[r10 + r11*4], xmm0
    
    inc r11
    jmp .convert_loop
    
.done:
    pop r12
    pop rbx
    pop rsp
    pop rbp
    ret