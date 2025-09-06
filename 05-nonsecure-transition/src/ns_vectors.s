/*
 * Non-secure Vector Table and Reset Handler
 * This code runs in Non-secure state after TrustZone transition
 */

    .syntax unified
    .thumb

    .extern ns_main
    .global Reset_Handler_NS

    /* Non-secure vector table */
    .section .isr_vector_ns
    .long   __StackTop_NS        /* Non-secure MSP initial value */
    .long   Reset_Handler_NS     /* Non-secure Reset Handler */
    .long   0                    /* NMI Handler */
    .long   0                    /* HardFault Handler */
    .long   0                    /* MemManage Handler */
    .long   0                    /* BusFault Handler */
    .long   0                    /* UsageFault Handler */
    .long   0                    /* SecureFault Handler */
    .long   0                    /* Reserved */
    .long   0                    /* Reserved */
    .long   0                    /* Reserved */
    .long   0                    /* SVCall Handler */
    .long   0                    /* Debug Monitor Handler */
    .long   0                    /* Reserved */
    .long   0                    /* PendSV Handler */
    .long   0                    /* SysTick Handler */

    .text
    .thumb_func
Reset_Handler_NS:
    /*
     * Non-secure Reset Handler
     * Initialize Non-secure world and jump to main
     */
    
    /* Initialize Non-secure stack pointer (already set by Secure boot) */
    /* MSP_NS was set by the Secure bootloader */
    
    /* Jump to Non-secure main application */
    ldr r0, =ns_main
    bx  r0

    /* Should never return */
hang_ns:
    b hang_ns
