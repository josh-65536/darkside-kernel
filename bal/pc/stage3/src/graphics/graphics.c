#include <types.h>
#include <stdlib.h>
#include <graphics/graphics.h>
#include <graphics/vbe.h>

#include <mm/vmm.h>

#include <stdio.h>

/* VBE controller and mode info */
static controller_info_t *controller_info = (controller_info_t*) 0x510;
static mode_info_t *mode_info = (controller_info_t*) 0x524;

/* Create a framebuffer from a VBE mode */
static framebuffer_t *framebuffer_create(uint16_t mode)
{
	while(1);

	/* Set the mode */
	uint32_t status = vbe_set_mode(mode);

	/* Read the mode into memory */
	status = vbe_get_mode(mode);

	/* Map the LFB into memory (ALERT GIANT HACK) */
	map_page(0x80000000, (uint64_t) mode_info->lfb & 0xFFFFF000, PAGE_READ | PAGE_WRITE);

	/* Initialize the framebuffer */
	framebuffer_t *fb = (framebuffer_t*) malloc(sizeof(framebuffer_t));
	
	fb->buffer = (void*) 0x80000000 + (mode_info->lfb - (mode_info->lfb & 0xFFFFF000));

	fb->width = (int) mode_info->width;
	fb->height = (int) mode_info->height;
	fb->bpp = mode_info->bpp;
	fb->planes = mode_info->planes;
	fb->banks = mode_info->banks;
	fb->pitch = mode_info->pitch;

	return fb;
}

/* Initialize graphics */
framebuffer_t *graphics_init(int width, int height, uint8_t bpp)
{
	/* Best mode we've found so far */
	uint16_t best_mode = 0x13;

	/* Best area and depth we've found so far */
	int best_area = width * height;
	int best_depth = bpp;

	/* Initialize VBE */
	uint32_t status = vbe_init();

	if (status != 0x4F)
	{
		return framebuffer_create(best_mode);
	}

	/* Search for the best possible mode */
	uint16_t *modes = (uint16_t*) (controller_info->modes[1] * 0x10) + controller_info->modes[0];
	for (int i = 0; modes[i] != 0xFFFF; i++)
	{
		printf("%d\n", modes[i]);
		while(1);

		/* Get the mode info */
		status = vbe_get_mode(modes[i]);

		if (status != 0x4F)
		{
			printf("Getting mode failed\n");
			continue;
		}

		/* Make sure the mode supports LFB */
		if ((mode_info->attributes & 0x90) != 0x90)
		{
			printf("Doesn't support LFB\n");
			continue;
		}

		/* Make sure the mode is direct color or packed pixel */
		if (mode_info->memory_model != 4 && mode_info->memory_model != 6)
		{
			printf("Not direct color or packed pixel\n");
			continue;
		}

		printf("%d %d %d\n", mode_info->width, mode_info->height, mode_info->bpp);

		/* Calculate the area and depth */
		int area = mode_info->width * mode_info->height;
		int depth = mode_info->bpp;

		/* Compare the mode to the closest match so far, keeping it if it's the best */
		if ((area > best_area && depth >= best_depth) || (area >= best_area && depth > best_depth))
		{
			/* Verify it matches the resolution and BPP desired by the user */
			if (width != 0 && height != 0 && bpp != 0)
			{
				if (area > (width * height) || depth > bpp)
				{
					continue;
				}
			}

			/* Make sure the mode is supported by the monitor using EDID */

			/* Keep it */
			best_mode = modes[i];
			best_area = area;
			best_depth = bpp;
		}
	}

	/* Create a framebuffer for that mode */
	return framebuffer_create(best_mode);
}
