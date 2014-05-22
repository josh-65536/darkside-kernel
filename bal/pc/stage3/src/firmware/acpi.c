#include <types.h>
#include <mm/vmm.h>
#include <firmware/acpi.h>

/* RSDT and XSDT */
static struct rsdt *rsdt = NULL;
static struct xsdt *xsdt = NULL;

/* Using the XSDT */
static bool using_xsdt;

/* Do a checksum on an ACPI structure */
static bool do_checksum(void *ptr, uint32_t length)
{
	/* Current checksum value */
	uint8_t computed = 0;

	/* Calculate the checksum */
	uint8_t *buffer = ptr;
	for (uint32_t offset = 0; offset < length; offset++)
	{
		computed += buffer[offset];
	}

	/* It succeeded if we got 0 */
	return computed == 0;
}

/* Map an ACPI table */
static struct acpi_table_header *map_acpi_table(vaddr_t virtual_address, paddr_t physical_address)
{
	/* Map the first page of the table */
	map_page(virtual_address, physical_address, PAGE_READ | PAGE_WRITE);
	struct acpi_table_header *table = (struct acpi_table_header*) (virtual_address + (physical_address & 0xFFF));

	/* Determine how many more we need to map */
	uint32_t old_size = 0x1000 - (physical_address & 0xFFF);
	uint32_t remaining_bytes = 0;
	if (table->length > old_size)
	{
		remaining_bytes = table->length - old_size;
	}

	/* Map the rest */
	virtual_address += 0x1000;
	physical_address &= ~0xFFF;
	physical_address += 0x1000;
	for (uint32_t i = 0; i < remaining_bytes; i += 0x1000)
	{
		map_page(virtual_address, physical_address, PAGE_READ | PAGE_READ);
		virtual_address += 0x1000;
		physical_address += 0x1000;
	}

	return table;
}

/* Find an ACPI table by its signature */
struct acpi_table_header *acpi_find_table(uint32_t signature)
{
	/* ACPI table */
	struct acpi_table_header *table;

	/* Using the XSDT */
	if (using_xsdt)
	{
		/* Calculate the number of table entries */
		uint32_t num_table_entries = (xsdt->header.length - sizeof(struct acpi_table_header)) / 8;

		/* Look for the table */
		for (uint32_t i = 0; i < num_table_entries; i++)
		{
			/* Try mapping the ACPI table */
			table = map_acpi_table(0x10001000, xsdt->tables[i]);

			/* Verify the signature and checksum */
			if ((table->signature == signature) && do_checksum(table, table->length))
			{
				return table;
			}
		}
	}
	/* Using the RSDT */
	else
	{
		/* Calculate the number of table entries */
		uint32_t num_table_entries = (rsdt->header.length - sizeof(struct acpi_table_header)) / 4;

		/* Look for the table */
		for (uint32_t i = 0; i < num_table_entries; i++)
		{
			/* Try mapping the ACPI table */
			table = map_acpi_table(0x10001000, rsdt->tables[i]);

			/* Verify the signature and checksum */
			if ((table->signature == signature) && do_checksum(table, table->length))
			{
				return table;
			}
		}
	}

	return NULL;
}

/* Initialize the ACPI firmware interface */
int acpi_init()
{
	/* RSDP pointer and extended RSDP pointer */
	struct rsdp *rsdp = NULL;
	struct rsdp_ext *rsdp_ext = NULL;

	/* First, search the EBDA for the RSDP */
	for (vaddr_t i = 0x9FC00; i < 0xA0000; i += 0x10)
	{
		/* Candidate pointers */
		struct rsdp *candidate = (struct rsdp*) i;
		struct rsdp_ext *candidate_ext = (struct rsdp_ext*) i;

		/* If the signature matches */
		if (candidate->signature == RSDP_SIGNATURE)
		{
			/* Make sure the checksum is valid */
			if (!do_checksum(candidate, sizeof(struct rsdp)))
			{
				continue;
			}

			/* By now, we at least have a standard RSDP */
			rsdp = candidate;

			/* If the extended RSDP is supported, use it */
			if (candidate->revision >= 2 && do_checksum(candidate_ext, sizeof(struct rsdp_ext)))
			{
				rsdp_ext = candidate_ext;
				using_xsdt = true;
			}

			/* By now, we have either a valid standard RSDP or a valid extended RSDP */
			goto rsdp_found;
		}
	}

	/* Second, search the BIOS area for the RSDP */
	for (vaddr_t i = 0xE0000; i < 0x100000; i += 0x10)
	{
		/* Candidate pointers */
		struct rsdp *candidate = (struct rsdp*) i;
		struct rsdp_ext *candidate_ext = (struct rsdp_ext*) i;

		/* If the signature matches */
		if (candidate->signature == RSDP_SIGNATURE)
		{
			/* Make sure the checksum is valid */
			if (!do_checksum(candidate, sizeof(struct rsdp)))
			{
				continue;
			}

			/* By now, we at least have a standard RSDP */
			rsdp = candidate;

			/* If the extended RSDP is supported, use it */
			if (candidate->revision >= 2 && do_checksum(candidate_ext, sizeof(struct rsdp_ext)))
			{
				rsdp_ext = candidate_ext;
				using_xsdt = true;
			}

			/* By now, we have either a valid standard RSDP or a valid extended RSDP */
			goto rsdp_found;
		}
	}

	/* By now, we couldn't find a valid RSDP, so fail */
	return -1;
/* A valid RSDP was found */
rsdp_found:
	/* Map the RSDT or XSDT, depending on which one we want to use */
	if (using_xsdt)
	{
		/* Map the XSDT */
		xsdt = (struct xsdt*) map_acpi_table(0x10000000, rsdp_ext->xsdt_address);

		/* Verify its signature and checksum */
		if ((xsdt->header.signature != XSDT_SIGNATURE) || !do_checksum(xsdt, xsdt->header.length))
		{
			return -1;
		}
	}
	else
	{
		/* Map the RSDT */
		rsdt = (struct rsdt*) map_acpi_table(0x10000000, rsdp->rsdt_address);

		/* Verify its signature and checksum */
		if ((rsdt->header.signature != RSDT_SIGNATURE) || !do_checksum(rsdt, rsdt->header.length))
		{
			return -1;
		}
	}

	return 0;
}