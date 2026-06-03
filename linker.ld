ENTRY(kernel_main)

SECTIONS
{
    /* The kernel will be loaded at 2MB */
    . = 0x1000000;

    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
}
