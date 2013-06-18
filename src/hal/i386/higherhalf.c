#include <hal/i386/vmm.h>
#include <kernel/init/main.h>
#include <kernel/modules/multiboot.h>

void load_higherhalf(struct multiboot *mboot_ptr, unsigned int *pd, unsigned int *pt_lower, unsigned int *pt_higher, unsigned int *pt_bitmap, unsigned int *pt_paging)
{
	/* Map the multiboot, text, data, and BSS sections to their addresses */
	unsigned int address;

	/* Lower half */
	for (address = 0; address < 0x400000; address += 0x1000)
	{
		pt_lower[address / 0x1000] = address | 0x03;
	}

	/* Higher half */
	for (address = 0x100000; address < 0x400000; address += 0x1000)
	{
		pt_higher[(address - 0x100000) / 0x1000] = address | 0x03;
	}

	/* Paging structures */
	pt_paging[0] = (unsigned int) pd | 0x03;
	pt_paging[1] = (unsigned int) pt_lower | 0x03;
	pt_paging[512] = (unsigned int) pt_higher | 0x03;
	pt_paging[576] = (unsigned int) pt_bitmap | 0x03;
	pt_paging[961] = (unsigned int) pt_paging | 0x03;

	/* Add the page tables into the page directory */
	pd[0] = (unsigned int) pt_lower | 0x03;
	pd[512] = (unsigned int) pt_higher | 0x03;
	pd[575] = (unsigned int) pt_bitmap | 0x03;
	pd[960] = (unsigned int) pt_paging | 0x03;

	/* Switch to the page directory */
	asm volatile("mov %0, %%cr3" :: "r"(pd));

	/* Now enable paging! */
	unsigned int cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
    
    unsigned int esp;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    esp = PHYSICAL_TO_HIGHER(esp);
    asm volatile("mov %0, %%esp" :: "r"(esp));

	/* Call kernel main */
	asm volatile("push %0" :: "r"(mboot_ptr));
	asm volatile("jmp %0" :: "r"(&kernel_main));
}
