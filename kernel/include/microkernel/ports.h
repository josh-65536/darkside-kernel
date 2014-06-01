#ifndef __HAL_PORTS_H
#define __HAL_PORTS_H

#include <types.h>

/* Read a byte from an I/O port */
uint8_t io_read_8(uint32_t port);

/* Write a byte to an I/O port */
void io_write_8(uint32_t port, uint8_t data);

/* Read a word from an I/O port */
uint16_t io_read_16(uint32_t port);

/* Write a word to an I/O port */
void io_write_16(uint32_t port, uint16_t data);

/* Read a dword from an I/O port */
uint32_t io_read_32(uint32_t port);

/* Write a dword to an I/O port */
void io_write_32(uint32_t port, uint32_t data);

/* Wait for an I/O port access to complete */
void io_wait();

#endif
