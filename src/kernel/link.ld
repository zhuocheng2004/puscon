
ENTRY(_start)

SECTIONS {
	_kernel_start = .;

	.text : {
		*(.text);
	}

	.rodata : {
		*(.rodata);
	}

	_kernel_end = .;
}
