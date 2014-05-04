#include <types.h>
#include <stdlib.h>
#include <string.h>
#include <bootvid.h>
#include <mm/e820.h>
#include <init/data.h>
#include <init/loader.h>
#include <mm/watermark.h>
#include <mm/vmm.h>
#include <fs/fs.h>
#include <fs/ext2.h>

#include <stdio.h>
#include <string.h>

/* Boot Application main function */
void ba_main(loader_block_t *loader_block);

/* Initialize the physical memory manager */
list_t *pmm_init(e820_entry_t *e820_entries, uint32_t num_e820_entries);

/* Initialize the storage tree */
void storage_init(uint32_t drive_number, uint32_t partition_start);

/* Boot Abstraction Layer main function */
void bal_main(data_t *_data)
{
	/* Initialize the boot video driver */
	bootvid_init(COLOR_WHITE, COLOR_BLACK);

	/* Set up the watermark allocator */
	watermark_init();

	/* Copy the local data structure onto the heap */
	data_t *data = (data_t*) malloc(sizeof(data_t));
	memcpy(data, _data, sizeof(data_t));

	/* Initialize the physical and virtual memory managers */
	list_t *phys_mem_map = pmm_init(data->e820_entries, data->num_e820_entries);
	vmm_init();

	bootvid_putch('A');
	bootvid_puts("BLAH\n");

	printf("Initialized the MM\n");
	printf("By the way, %X and also %X\n", 0xDEADBEEF, 0xDEADC0DE);
	while(1);

	/* Initialize the storage tree */
	storage_init(data->drive_number, data->partition_start);

	printf("Storage initialized\n");
	printf("By the way, %X\n", 0xDEADC0DE);

	/* Initialize the filesystem and each filesystem driver */
	fs_init();
	ext2_init();

	/* Generate a loader block to pass to the Boot Application */
	loader_block_t *loader_block = (loader_block_t*) malloc(sizeof(loader_block_t));

	loader_block->phys_mem_map = phys_mem_map;
	loader_block->fb = NULL;

	/* Pass control to the Boot Application */
	ba_main(loader_block);

	while(1);
}
