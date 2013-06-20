#ifndef __GDT_H
#define __GDT_H

#include <lib/libc/stdint.h>

/* Segment registers */
#define KERNEL_CODE_SEG	0x08
#define KERNEL_DATA_SEG	0x10
#define USER_CODE_SEG	0x18
#define USER_DATA_SEG	0x20

/* Defines a GDT entry */
struct gdt_entry
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

/* Defines a GDT pointer */
struct gdt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

/* Defines a TSS entry */
struct tss_entry
{
	uint32_t prev_tss;
	uint32_t esp0;	// Kernel stack
	uint32_t ss0;	// Kernel stack segment
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint16_t trap;
	uint16_t iomap_base;
} __attribute__((packed));

/* Set the kernel stack in the TSS and the kernel stack MSR */
void set_kernel_stack(uint32_t stack);

/* Install the GDT */
void gdt_install();

#endif
