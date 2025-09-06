    .syntax unified
    .thumb

    .section .isr_vector
    .long   __StackTop           /* MSP initial value */
    .long   Reset_Handler        /* Reset Handler */

    .text
    .thumb_func
    .global Reset_Handler
Reset_Handler:
    /* 스택 포인터 설정 */
    ldr r0, =__StackTop
    mov sp, r0
    
    /* main 함수 호출 */
    bl main
    
hang:
    b hang
