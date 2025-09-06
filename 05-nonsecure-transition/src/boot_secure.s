/*
 * Secure Boot Assembly for Cortex-M33 TrustZone
 * Transitions from Secure to Non-secure state
 * Sets up SAU and hands off to Non-secure application
 */

    .syntax unified
    .thumb

    /* SAU Register addresses */
    .equ SAU_CTRL,  0xE000EDD0
    .equ SAU_TYPE,  0xE000EDD4
    .equ SAU_RNR,   0xE000EDD8
    .equ SAU_RBAR,  0xE000EDDC
    .equ SAU_RLAR,  0xE000EDE0

    /* SCB Non-secure registers */
    .equ SCB_NS_VTOR, 0xE002ED08

    /* Non-secure memory regions */
    .equ NS_CODE_BASE,  0x00000000
    .equ NS_CODE_LIMIT, 0x0007FFFF
    .equ NS_RAM_BASE,   0x20000000
    .equ NS_RAM_LIMIT,  0x2007FFFF

    .section .isr_vector
    .long   __StackTop_NS        /* NS MSP initial value */
    .long   Reset_Handler        /* Reset Handler */

    .text
    .thumb_func
    .global Reset_Handler
Reset_Handler:
    /*
     * Direct execution in NS RAM
     * Demonstrates code/stack in NS RAM region
     */
    
    /* Stack already set by vector table */
    /* Jump directly to NS main */
    ldr r0, =ns_main
    bx  r0

    /* Should never reach here */
hang:
    b hang
