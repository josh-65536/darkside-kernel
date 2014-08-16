; Copyright (C) 2014 DarkSide Project
; Authored by George Klees <gksharkboy@gmail.com>
; vbe.s - VBE BIOS Extensions (VBE) initialization and mode-setting
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License version 3 as
; published by the Free Software Foundation.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public Licens
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
%include "src/arch/i686/rmode.inc"

[BITS 32]
section .text

; Initialize VBE
global vbe_init
vbe_init:
	; Switch from protected mode to real mode
	pm2rm
	
	; Get the controller info
	mov ax, 0x4F00
	mov di, [ebp + 8]
	int 0x10
	
	; Save return value
	and eax, 0xFFFF
	push eax
	
	; Switch from real mode to protected mode
	rm2pm
	
; Get a VBE mode
global vbe_get_mode
vbe_get_mode:
	; Switch from protected mode to real mode
	pm2rm
	
	; Get the mode info
	mov ax, 0x4F01
	mov cx, [ebp + 12]
	mov di, [ebp + 8]
	int 0x10
	
	; Save return value
	and eax, 0xFFFF
	push eax
	
	; Switch from real mode to protected mode
	rm2pm
	
; Set a VBE mode
global vbe_set_mode
vbe_set_mode:
	; Switch from protected mode to real mode
	pm2rm
	
	; Set the mode
	mov ax, 0x4F02
	mov bx, [ebp + 8]
	int 0x10
	
	; Save return value
	and eax, 0xFFFF
	push eax
	
	; Switch from real mode to protected mode
	rm2pm
	
section .data

; Real mode IDT
idt
