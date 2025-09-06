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
    
    /* PC 값 확인을 위한 라벨 */
    nop                          /* PC 관찰 포인트 1 */
    
    /* main 함수 호출 */
    bl main
    
hang:
    b hang
