/*
 * TrustZone Linker Script for Cortex-M33 on MPS2-AN505
 * Combines Secure and Non-secure images in a single ELF
 * Secure: S_CODE_BOOT region (0x10000000)
 * Non-secure: NS_CODE and NS_RAM regions (0x00000000, 0x20000000)
 */

MEMORY
{
    /* Demonstrate separated memory regions */
    NS_CODE (rx)  : ORIGIN = 0x00000000, LENGTH = 512K  
    NS_RAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 512K
}

/* Entry point is the Secure Reset Handler */
ENTRY(Reset_Handler)

SECTIONS
{
    /* Vector table in NS CODE */
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > NS_CODE
    
    /* Code and rodata in NS CODE */
    .text :
    {
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > NS_CODE
    
    /* Data in NS RAM (copy from NS CODE) */
    .data : AT (_etext)
    {
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > NS_RAM
    
    /* BSS in NS RAM */
    .bss (NOLOAD) :
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > NS_RAM
    
    /* Heap starts after BSS in NS RAM */
    . = ALIGN(8);
    _heap_start = .;
    _heap_end = ORIGIN(NS_RAM) + LENGTH(NS_RAM) - 0x2000; /* Reserve 8KB for stack */
    
    /* Stack at the end of NS RAM */
    __StackTop = ORIGIN(NS_RAM) + LENGTH(NS_RAM);
    __StackTop_NS = ORIGIN(NS_RAM) + LENGTH(NS_RAM);
}

/*
 * Assertions to verify memory layout
 */
ASSERT(_ebss <= _heap_end, "Data overflow")
