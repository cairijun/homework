ENTRY(_start)

SECTIONS
{
    . = 1M;

    .text BLOCK(4K) : ALIGN(4K)
    {
        _START_OF_TEXT_ = .;
        *(.multiboot)
        *(.text)
        _END_OF_TEXT_ = .;
    }

    .rodata BLOCK(4K) : ALIGN(4K)
    {
        _START_OF_RODATA_ = .;
        *(.rodata)
        _END_OF_RODATA_ = .;
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
        _START_OF_DATA_ = .;
        *(.paging_info)
        *(.descriptor_tables)
        *(.data)
        _END_OF_DATA_ = .;
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        _START_OF_BSS_STACK_ = .;
        *(COMMON)
        *(.bss)
        *(.bootstrap_stack)
        _END_OF_BSS_STACK_ = .;
    }

    _END_OF_KERNEL_ = .;
}
