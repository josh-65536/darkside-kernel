#ifndef __FILE_H
#define __FILE_H

#include <lib/libc/types.h>
#include <kernel/vfs/vfs.h>
#include <kernel/vfs/stat.h>

/* File flags */
#define FILE_READ	0x01
#define FILE_WRITE	0x02
#define FILE_RW		(FILE_READ | FILE_WRITE)
#define FILE_APPEND	0x04
#define FILE_CREATE	0x08

/* File structure */
typedef struct file
{
	inode_t *node;	// Inode
	int32_t flags;	// File flags
	uint64_t pos;	// File position
} file_t;

/* Create, open, close, read from, write to, and seek a file */
file_t *file_create();
int32_t file_open(file_t *file, uint8_t *path, int32_t flags, mode_t mode);
int32_t file_close(file_t *file);
uint64_t file_read(file_t *file, uint8_t *buffer, uint64_t length);
uint64_t file_write(file_t *file, uint8_t *buffer, uint64_t length);
uint64_t file_seek(file_t *file, int64_t offset, int32_t origin);
int32_t file_ioctl(file_t *file, int32_t request, uint8_t *buffer, uint32_t length);

#endif