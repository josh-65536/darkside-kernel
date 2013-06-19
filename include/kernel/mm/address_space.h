#ifndef __ADDRESS_SPACE_H
#define __ADDRESS_SPACE_H

extern unsigned int end;

/* Kernel memory */
#define KERNEL_PHYSICAL_START		0x100000
#define KERNEL_VIRTUAL_START		0x80000000
#define KERNEL_PHYSICAL_SIZE		(((unsigned int) &end) - KERNEL_VIRTUAL_START)
#define KERNEL_VIRTUAL_SIZE			0x10000000

#define KHEAP_PHYSICAL_START		0x400000
#define KHEAP_VIRTUAL_START			0xE0000000
#define KHEAP_INITIAL_SIZE			0x100000
#define KHEAP_MIN_SIZE				0x70000
#define KHEAP_MAX_SIZE				0xFFFF000

#define KERNEL_STACK_START			0xB0000000
#define KERNEL_STACK_SIZE			0x10000

#define KERNEL_MODULES_START		0xB0000000
#define KERNEL_MODULES_SIZE			0x40000000

#define PAGE_STRUCTURES_START		0xFFC00000
#define PAGE_STRUCTURES_SIZE		0x400000

#endif
