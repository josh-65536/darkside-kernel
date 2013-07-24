#ifndef __DEV_H
#define __DEV_H

#include <lib/libc/types.h>

/* Device ID, made up of major and minor numbers */
typedef uint32_t dev_t;

/* Manipulate device IDs */
#define MAKEDEV(major, minor) (((major & 0xFFFF) << 16) | (minor & 0xFFFF))
#define MAJOR(id) ((id >> 16) & 0xFFFF)
#define MINOR(id) (id & 0xFFFF)

/* Character device structure */
typedef struct chardev
{
	/* Device ID */
	dev_t id;

	/* Device specific data */
	void *data;

	/* Read a specified amount of data at the given offset from a block device into a buffer */
	uint64_t (*read)(struct blockdev *blockdev, uint8_t *buffer, uint64_t length);

	/* Write a specified amount of data from a buffer into a block device at the given offset */
	uint64_t (*write)(struct blockdev *blockdev, uint8_t *buffer, uint64_t length);

	/* Issue a device specific request */
	int32_t (*ioctl)(struct blockdev *blockdev, int32_t request, uint32_t length);
} chardev_t;

/* Block device structure */
typedef struct blockdev
{
	/* Device ID */
	dev_t id;

	/* Block size */
	uint32_t blocksize;

	/* Device specific data */
	void *data;

	/* Read a specified amount of data at the given offset from a block device into a buffer */
	uint64_t (*read)(struct blockdev *blockdev, uint8_t *buffer, uint64_t offset, uint64_t length);

	/* Write a specified amount of data from a buffer into a block device at the given offset */
	uint64_t (*write)(struct blockdev *blockdev, uint8_t *buffer, uint64_t offset, uint64_t length);

	/* Issue a device specific request */
	int32_t (*ioctl)(struct blockdev *blockdev, int32_t request, uint8_t *buffer, uint32_t length);
} blockdev_t;

/* Create, destroy, initialize, read from, and write to a character device structure */
chardev_t *chardev_create();
void chardev_destroy(chardev_t *chardev);
void chardev_init(chardev_t *chardev);
uint64_t chardev_read(chardev_t *chardev, uint8_t *buffer, uint64_t length);
uint64_t chardev_write(chardev_t *chardev, uint8_t *buffer, uint64_t length);
int32_t chardev_ioctl(chardev_t *chardev, int32_t request, uint8_t *buffer, uint32_t length);

/* Create, destroy, initialize, read from, and write to a block device structure */
blockdev_t *blockdev_create();
void blockdev_destroy(blockdev_t *blockdev);
void blockdev_init(blockdev_t *blockdev);
uint64_t blockdev_read(blockdev_t *blockdev, uint8_t *buffer, uint64_t offset, uint64_t length);
uint64_t blockdev_write(blockdev_t *blockdev, uint8_t *buffer, uint64_t offset, uint64_t length);
int32_t blockdev_ioctl(blockdev_t *blockdev, int32_t request, uint8_t *buffer, uint32_t length);

/* Register and unregister character and block devices */
void register_chardev(chardev_t *chardev, uint8_t *name);
void unregister_chardev(chardev_t *chardev, uint8_t *name);
void register_blockdev(blockdev_t *blockdev, uint8_t *name);
void unregister_blockdev(blockdev_t *blockdev, uint8_t *name);

#endif