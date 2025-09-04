/* 
 * Simple Linker Script for MPS2-AN505
 * Based on working armv8m-hello example
 */

MEMORY
{
   NS_CODE (rx)     : ORIGIN = 0x00000000, LENGTH = 512K
   S_CODE_BOOT (rx) : ORIGIN = 0x10000000, LENGTH = 512K
   RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 512K
}

/* Entry Point */
ENTRY(Reset_Handler)

SECTIONS
{
    .text : 
    {
        KEEP(*(.isr_vector))
        *(.text)
        *(.data)
        *(.bss) 
    } > S_CODE_BOOT
    
    /* Set stack top to end of S_CODE_BOOT. */
    __StackTop = ORIGIN(S_CODE_BOOT) + LENGTH(S_CODE_BOOT);
}
