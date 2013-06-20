#include <hal/i386/vmm.h>
#include <kernel/init/main.h>
#include <kernel/modules/multiboot.h>

void load_higherhalf(struct multiboot *mboot_ptr, unsigned int *pd, unsigned int *pt_lower, unsigned int *pt_higher, unsigned int *pt_bitmap)
{
	/* Enable global pages */
	unsigned int cr4;
	asm volatile ("mov %%cr4, %0" : "=r" (cr4));
	cr4 |= 0x80;
	asm volatile ("mov %0, %%cr4" :: "r" (cr4));

	/* Map the multiboot, text, data, and BSS sections to their addresses */
	unsigned int address;

	/* Lower half */
	for (address = 0; address < 0x400000; address += 0x1000)
	{
		pt_lower[address / 0x1000] = address | PAGE_KERNEL;
	}

	/* Higher half */
	for (address = 0x100000; address < 0x400000; address += 0x1000)
	{
		pt_higher[(address - 0x100000) / 0x1000] = address | PAGE_KERNEL | PAGE_FLAG_GLOBAL;
	}

	/* Add the page tables into the page directory */
	pd[0] = (unsigned int) pt_lower | PAGE_KERNEL;
	pd[512] = (unsigned int) pt_higher | PAGE_KERNEL | PAGE_FLAG_GLOBAL;
	pd[575] = (unsigned int) pt_bitmap | PAGE_KERNEL | PAGE_FLAG_GLOBAL;
	pd[1022] = 0;
	pd[1023] = (unsigned int) pd | PAGE_KERNEL;

	/* Switch to the page directory */
	asm volatile("mov %0, %%cr3" :: "r" (pd));

	/* Now enable paging! */
	unsigned int cr0;
    asm volatile("mov %%cr0, %0" : "=r" (cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
    
    unsigned int esp;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    esp = PHYSICAL_TO_HIGHER(esp);
    asm volatile("mov %0, %%esp" :: "r"(esp));

	/* Call kernel main */
	asm volatile("push %0" :: "r"(mboot_ptr));
	asm volatile("jmp *%0" :: "r"(&kernel_main));
}
