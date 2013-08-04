#include <lib/libc/types.h>
#include <kernel/console/kprintf.h>
#include <kernel/device/dev.h>
#include <kernel/mm/heap.h>
#include <kernel/vfs/vfs.h>

/* Root of the dev filesystem */
static inode_t devfs_root;

/* Create a character device structure */
chardev_t *chardev_create()
{
	/* Create a character device and make sure it's 0 */
	chardev_t *chardev = (chardev_t*) kmalloc(sizeof(chardev_t));
	memset(chardev, 0, sizeof(chardev_t));
}

/* Destroy a character device structure */
void chardev_destroy(chardev_t *chardev)
{
	/* Set its fields to 0 */
	chardev->id = 0;
	chardev->data = 0;

	chardev->read = 0;
	chardev->write = 0;
	chardev->ioctl = 0;

	/* Destroy the character device structure */
	kfree(chardev);
}

/* Initialize a character device structure */
void chardev_init(chardev_t *chardev, dev_t id)
{
	/* Set the device ID */
	chardev->id = id;
}

/* Read from a character device */
uint64_t chardev_read(chardev_t *chardev, uint8_t *buffer, uint64_t length)
{
	return chardev->read(chardev, buffer, offset, length);
}

/* Write to a character device */
uint64_t chardev_write(chardev_t *chardev, uint8_t *buffer, uint64_t length)
{
	return chardev->write(chardev, buffer, offset, length);
}

/* Issue a device specific request */
int32_t chardev_ioctl(chardev_t *chardev, int32_t request, uint8_t *buffer, uint32_t length)
{
	return chardev->ioctl(chardev, request, buffer, length);
}

/* Create a block device structure */
blockdev_t *blockdev_create()
{
	/* Create a block device and make sure it's 0 */
	blockdev_t *blockdev = (blockdev_t*) kmalloc(sizeof(blockdev_t));
	memset(blockdev, 0, sizeof(blockdev_t));
}

/* Destroy a block device structure */
void blockdev_destroy(blockdev_t *blockdev)
{
	/* Set its fields to 0 */
	blockdev->id = 0;
	blockdev->data = 0;

	blockdev->read = 0;
	blockdev->write = 0;
	blockdev->ioctl = 0;

	/* Destroy the block device structure */
	kfree(blockdev);
}

/* Initialize a block device structure */
void blockdev_init(blockdev_t *blockdev, dev_t id)
{
	/* Set the device ID */
	blockdev->id = id;
}

/* Read from a block device */
uint64_t blockdev_read(blockdev_t *blockdev, uint8_t *buffer, uint64_t offset, uint64_t length)
{
	return blockdev->read(blockdev, buffer, offset, length);
}

/* Write to a block device */
uint64_t blockdev_write(blockdev_t *blockdev, uint8_t *buffer, uint64_t offset, uint64_t length)
{
	return blockdev->write(blockdev, buffer, offset, length);
}

/* Issue a device specific request */
int32_t blockdev_ioctl(blockdev_t *blockdev, int32_t request, uint8_t *buffer, uint32_t length)
{
	return blockdev->ioctl(blockdev, request, buffer, length);
}

/* Register a character device */
void register_chardev(chardev_t *chardev, uint8_t *name)
{
	/* Create a new dev node for the character device */
	inode_t *node = (inode_t*) kmalloc(sizeof(inode_t));
	int result = vfs_dev->mknod(vfs_dev, name, INODE_TYPE_CHARDEV, chardev->id, inode);

	if (result == -1)
	{
		kprintf(LOG_ERROR, "Failed to register character device (unable to create inode for device)");
	}

	/* Set the inode specific data to the character device */
	node->data = (void*) chardev;
}

/* Unregister a character device */
void unregister_chardev(chardev_t *chardev, uint8_t *name)
{
	/* Find the inode for the character device by name */
	inode_t *node = (inode_t*) kmalloc(sizeof(inode_t));
	int result = vfs_dev->finddir(vfs_dev, vfs_dev->root, name, node);

	if (result == -1)
	{
		kprintf(LOG_ERROR, "Failed to unregister character device (unable to find dev entry)");
		return;
	}

	/* Make sure it matches with the character device */
	if (node->data == chardev && node->id == chardev->id)
	{
		/* Unlink the node from the dev directory */
		result = vfs_dev->unlink(vfs_dev, name);
		if (result == -1)
		{
			kprintf(LOG_ERROR, "Failed to unregister character device (unable to unlink inode)");
			return;
		}
	}

	kprintf(LOG_ERROR, "Failed to unregister character device (inode and character device do not match)");
}

/* Register a block device */
void register_blockdev(blockdev_t *blockdev, uint8_t *name)
{
	/* Create a new dev node for the block device */
	inode_t *node = (inode_t*) kmalloc(sizeof(inode_t));
	int result = vfs_dev->mknod(vfs_dev, name, INODE_TYPE_BLOCKDEV, blockdev->id, inode);

	if (result == -1)
	{
		kprintf(LOG_ERROR, "Failed to register block device (unable to create inode for device)");
	}

	/* Set the inode specific data to the block device */
	node->data = (void*) blockdev;
}

/* Unregister a block device */
void unregister_blockdev(blockdev_t *blockdev, uint8_t *name)
{
	/* Find the inode for the block device by name */
	inode_t *node = (inode_t*) kmalloc(sizeof(inode_t));
	int result = vfs_dev->finddir(vfs_dev, vfs_dev->root, name, node);

	if (result == -1)
	{
		kprintf(LOG_ERROR, "Failed to unregister block device (unable to find dev entry)");
		return;
	}

	/* Make sure it matches with the block device */
	if (node->data == blockdev && node->id == blockdev->id)
	{
		/* Unlink the node from the dev directory */
		result = vfs_dev->unlink(vfs_dev, name);
		if (result == -1)
		{
			kprintf(LOG_ERROR, "Failed to unregister block device (unable to unlink inode)");
			return;
		}
	}

	kprintf(LOG_ERROR, "Failed to unregister block device (inode and block device do not match)");
}

/* Get the root inode of the dev filesystem */
static void devfs_get_root(filesystem_t *fs, dev_t dev, inode_t *node)
{
	memcpy(node, &devfs_root, sizeof(inode_t));
}

/* Destroy the dev filesystem */
static void devfs_destroy(filesystem_t *fs, dev_t dev)
{
}

/* Read from a device in dev */
static uint64_t devfs_read(filesystem_t *fs, dev_t dev, inode_t *node, uint8_t *buffer, uint64_t offset, uint64_t length)
{
	/* Character device */
	if (inode->type == INODE_TYPE_CHARDEV)
	{
		/* Cast the inode specific data to a character device and read from it */
		blockdev_t *chardev = (chardev_t*) node->data;
		return chardev->read(chardev, buffer, offset, length);
	}
	/* Block device */
	if (inode->type == INODE_TYPE_BLOCKDEV)
	{
		/* Cast the inode specific data to a block device and read from it */
		blockdev_t *blockdev = (blockdev_t*) node->data;
		return blockdev->read(blockdev, buffer, offset, length);
	}

	return 0;
}

/* Write to a device in dev */
static uint64_t devfs_write(filesystem_t *fs, dev_t dev, inode_t *node, uint8_t *buffer, uint64_t offset, uint64_t length)
{
	/* Character device */
	if (inode->type == INODE_TYPE_CHARDEV)
	{
		/* Cast the inode specific data to a character device and read from it */
		blockdev_t *chardev = (chardev_t*) node->data;
		return chardev->write(chardev, buffer, offset, length);
	}
	/* Block device */
	if (inode->type == INODE_TYPE_BLOCKDEV)
	{
		/* Cast the inode specific data to a block device and read from it */
		blockdev_t *blockdev = (blockdev_t*) node->data;
		return blockdev->write(blockdev, buffer, offset, length);
	}
	return 0;
}

/* Return a list of directory entries in a directory in dev */

/* Create a new directory entry to an inode in dev */

/* Remove directory entry in dev */

/* Create a new symbolic link to an inode in dev */

/* Create a new inode in dev */

/* Rename a directory entry in dev */
static int32_t devfs_rename(struct filesystem *fs, dev_t dev, uint8_t *oldpath, uint8_t *newpath)
{
}

/* Issue a device specfic request to a node in dev */
static int32_t devfs_ioctl(filesystem_t *fs, dev_t dev, inode_t *node, int32_t request, uint8_t *buffer, uint32_t length)
{
	/* Character device */
	if (inode->type == INODE_TYPE_CHARDEV)
	{
		/* Cast the inode specific data to a character device and read from it */
		blockdev_t *chardev = (chardev_t*) node->data;
		return chardev->ioctl(chardev, request, buffer, length);
	}
	/* Block device */
	if (inode->type == INODE_TYPE_BLOCKDEV)
	{
		/* Cast the inode specific data to a block device and read from it */
		blockdev_t *blockdev = (blockdev_t*) node->data;
		return blockdev->read(blockdev, request, buffer, length);
	}

	return -1;
}

/* Initialize the dev filesystem */
void devfs_init()
{
	/* Create the dev filesystem and fill out its information */
	filesystem_t *devfs = (filesystem_t*) kmalloc(sizeof(filesystem_t));

	devfs->get_root = &devfs_get_root;
	devfs->destroy = &devfs_destroy;
	devfs->read = &devfs_read;
	devfs->write = &devfs_write;
	devfs->readdir = 0;
	devfs->link = 0;
	devfs->unlink = 0;
	devfs->symlink = 0;
	devfs->mknod = 0;
	devfs->rename = &devfs_rename;
	devfs->ioctl = &devfs_ioctl;

	/* Fill out the root of the dev filesystem */
	devfs_root->mp = 0;
	devfs_root->type = INODE_TYPE_DIR;
	devfs_root->parent = 0;

	devfs_root->size = 0;
	devfs_root->mode = 0777;
	devfs_root->nlink = 0;
	devfs_root->uid = 0;
	devfs_root->gid = 0;
	devfs_root->atime = devfs_root->mtime = devfs_root->ctime = 0;

	/* Register the dev filesystem */
	register_filesystem(devfs, "devfs");
}
