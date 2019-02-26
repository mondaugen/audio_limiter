/* ENTRY(Reset_Handler); */

MEMORY
{
    /* Progsrams reside in bank 1, leaving bank 2 free to record into. */
    flash (rx) : ORIGIN = 0x08000000, LENGTH = 1M
    sram1 (rwx)  : ORIGIN = 0x20000000, LENGTH = 112K
    sram2 (rwx)  : ORIGIN = 0x2001c000, LENGTH = 16K
    sram3 (rwx)  : ORIGIN = 0x20020000, LENGTH = 64K
    ccm (rw)   : ORIGIN = 0x10000000, LENGTH = 64K
}

_sram1_start = ORIGIN(sram1);
/* _estack    = ORIGIN(sram1) + LENGTH(sram1) - 4; */
/* top of stack is top of SRAM */
_estack = 0x2002ffff;
/* The biggest the heap can grow is to the end of SRAM1 */
_eheap  = 0x2001bfff;
/* top of stack is top of core-coupled memory (ccm) */
/* _estack = 0x1000ffff; */
_Min_Heap_Size = 32K;
_Min_Stack_Size = 32K;

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        *(.isr_vector)
        . = ALIGN(4);
    } >flash
    .text :
    {
        . = ALIGN(4);
        *(.flash_comment_text)
        *(.text)
        *(.text*)
        . = ALIGN(4);
        _etext = .;
    } >flash
    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >flash
    _sidata = LOADADDR(.data);
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >sram1 AT> flash
    _sismall_data = LOADADDR(.small_data);
    .small_data :
    {
        . = ALIGN(4);
        _ssmall_data = .;
        *(.small_data)
        *(.small_data*)
        . = ALIGN(4);
        _esmall_data = .;
    } >sram2 AT> flash
    _sibig_data = LOADADDR(.big_data);
    .big_data :
    {
        . = ALIGN(4);
        _sbig_data = .;
        *(.big_data)
        *(.big_data*)
        . = ALIGN(4);
        _ebig_data = .;
    } >sram3 AT> flash
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        /* Needed by standard library, it seems */
        __bss_start__ = .;
        *(.bss)
        *(.bss*)
        . = ALIGN(4);
        _ebss = .;
        /* Needed by standard library, it seems */
        __bss_end__ = .;
    } >sram1
    ._user_heap_stack :
    {
        . = ALIGN(4);
        _end = .;
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(4);
    } >sram1
    /DISCARD/ :
    {
        libc.a (*)
        libm.a (*)
        libgcc.a (*)
    }
}
