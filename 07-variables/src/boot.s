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
    
    /* 전역변수 초기화 관찰을 위한 브레이크포인트 */
    nop                          /* Global variable initialization point */
    
    /* main 함수 호출 */
    bl main
    
hang:
    b hang
