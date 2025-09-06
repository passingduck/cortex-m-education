MEMORY
{
   NS_CODE (rx)     : ORIGIN = 0x00000000, LENGTH = 512K
   S_CODE_BOOT (rx) : ORIGIN = 0x10000000, LENGTH = 512K  
   RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 512K
}

ENTRY(Reset_Handler)

SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector))
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
    } > S_CODE_BOOT
    
    .data :
    {
        _sdata = .;
        *(.data)
        *(.data*)
        _edata = .;
    } > S_CODE_BOOT
    
    .bss :
    {
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > S_CODE_BOOT
    
    __StackTop = ORIGIN(S_CODE_BOOT) + LENGTH(S_CODE_BOOT);
}
