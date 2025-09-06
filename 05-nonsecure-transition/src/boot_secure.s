/*
 * Real TrustZone Secure Boot for Cortex-M33
 * Transitions from Secure to actual Non-secure state
 */

    .syntax unified
    .thumb

    /* SAU Register addresses */
    .equ SAU_CTRL,  0xE000EDDC
    .equ SAU_RNR,   0xE000EDD0
    .equ SAU_RBAR,  0xE000EDD4
    .equ SAU_RLAR,  0xE000EDD8

    /* SCB Non-secure registers */
    .equ SCB_NS_VTOR, 0xE002ED08

    .section .isr_vector
    .long   __StackTop           /* Secure MSP initial value */
    .long   Reset_Handler        /* Secure Reset Handler */

    .text
    .thumb_func
    .global Reset_Handler
Reset_Handler:
    /*
     * Simple approach: Set up NS stack and jump to NS code
     * This demonstrates code running from NS memory regions
     */
    
    /* Set up Non-secure stack pointer in NS_RAM */
    ldr r0, =__StackTop_NS
    mov sp, r0
    
    /* Jump directly to NS Reset handler in NS_CODE region */
    ldr r0, =Reset_Handler_NS
    bx r0

    /* Should never reach here */
hang:
    b hang
