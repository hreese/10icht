MEMORY {
	FLASH (RX) : ORIGIN = 0x00000000, LENGTH = 16k
	RAM (RW) : ORIGIN = 0x10000000, LENGTH = 4k
}

SECTIONS {
	ENTRY(startup)

	_estack = ORIGIN(RAM) + LENGTH(RAM);

	.text : {
		KEEP(*(.nvic_table))

		*(.text*)

		*(.rodata*)

		_etext = .;
	} > FLASH

	.data : AT(_etext)
	{
		_data = .;
		*(.data)
		_edata = .;
	} > RAM

	.bss : {
		_bstart = .;
		*(.bss)
		*(COMMON)
		_bend = .;

	} > RAM
}
