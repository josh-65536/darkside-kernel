#ifndef __ADDRSPACE_H
#define __ADDRSPACE_H

#include <microkernel/lock.h>
#include <mm/vad.h>

/* Special address space pointers */
#define ADDRSPACE_CURRENT	1
#define ADDRSPACE_SYSTEM	2

/* Address space structure */
typedef struct addrspace
{
	/* Low-level address space */
	paddr_t address_space;

	/* Used and free regions */
	vad_t used, free;

	/* NUMA domain */
	int numa_domain;

	/* Address space lock */
	spinlock_t lock;
} addrspace_t;

/* Create and destroy an address space */
addrspace_t *addrspace_create();
void addrspace_destroy(addrspace_t *addrspace);

/* Allocate and free regions of a virtual address space */
void *addrspace_alloc(addrspace_t *addrspace, size_t size_reserved, size_t size_committed, int flags);
void addrspace_free(addrspace_t *addrspace, void *ptr, size_t size);

#endif
