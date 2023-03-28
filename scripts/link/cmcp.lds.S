/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "plat_addr_map.h"

#define HEAP_SECTION_SIZE		0x1000
#define STACK_SECTION_SIZE		0x1000
#define CP_STACK_SECTION_SIZE		0x1000

#if defined(PSRAMCP_BASE) && (PSRAMCP_SIZE > 0)
#define MEMCP	PSRAMCP
#define MEMCPX	PSRAMCPX
#define MEMCPX_TO_MEMCP PSRAMX_TO_PSRAM
#else
#define MEMCP	RAMCP
#define MEMCPX	RAMCPX
#define MEMCPX_TO_MEMCP RAMX_TO_RAM
#endif

/* Linker script to configure memory regions. */
MEMORY
{
#if defined(CHIP_HAS_CP) && (CP_FLASH_SIZE > 0)
	FLASH		(r)		: ORIGIN = CP_FLASH_BASE,		LENGTH = CP_FLASH_SIZE
	FLASHX		(rx)	: ORIGIN = CP_FLASHX_BASE,		LENGTH = CP_FLASH_SIZE
#endif
#if defined(CHIP_HAS_CP) && (RAMCP_SIZE > 0)
	RAMCP 		(rwx)	: ORIGIN = RAMCP_BASE,				LENGTH = RAMCP_SIZE
	RAMCPX 		(rwx)	: ORIGIN = RAMCPX_BASE,				LENGTH = RAMCP_SIZE
#endif

#if defined(PSRAMCP_BASE) && (PSRAMCP_SIZE > 0)
	PSRAMCP 	(rwx)	: ORIGIN = PSRAMCP_BASE,			LENGTH = PSRAMCP_SIZE
	PSRAMCPX	(rx)	: ORIGIN = PSRAMCPX_BASE,			LENGTH = PSRAMCP_SIZE
#endif
#if defined(RAMV_BASE) && (RAMV_SIZE > 0)
	RAMV 		(rwx)	: ORIGIN = RAMV_BASE,				LENGTH = RAMV_SIZE
#endif
}

/* Library configurations */
#ifndef NUTTX_BUILD
GROUP(libgcc.a)
#endif

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions ROM and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __cp_etext
 *   __cp_data_start__
 *   __cp_data_end__
 *   __cp_bss_start__
 *   __cp_bss_end__
 *   __cp_HeapLimit
 *   __cp_StackLimit
 *   __cp_StackTop
 *   __cp_stack
 *   __Vectors_End
 *   __Vectors_Size
 *   __free_cp_ram
 */
ENTRY(Reset_Handler)

SECTIONS
{
#if (CP_FLASH_SIZE > 0)
	.cp_text (ORIGIN(FLASHX)) : AT (FLASHX_TO_FLASH(ADDR(.cp_text)))
#else
	.cp_text (ORIGIN(MEMCPX)) :
#endif
	{
		_stext = .;
		KEEP(*(.vectors))

		*(.text*)
		*(.cp_text_sram*)
#ifndef NOSTD
		KEEP(*(.init))
		KEEP(*(.fini))

		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)

		/* .dtors */
		*crtbegin.o(.dtors)
		*crtbegin?.o(.dtors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
		*(SORT(.dtors.*))
		*(.dtors)
#endif

		. = ALIGN(4);

		*(.rodata*)

#ifndef NOSTD
		KEEP(*(.eh_frame*))
#endif
		KEEP(*(.build_info))
		*(.note.gnu.build-id)

		_etext = .;
		. = ALIGN(4);
#if (CP_FLASH_SIZE > 0)
	} > FLASHX
#else
	} > MEMCPX
#endif

#if (CP_FLASH_SIZE > 0)
	.cp_got_info (.) : AT (FLASHX_TO_FLASH(ADDR(.cp_got_info)))
#else
	.cp_got_info (.) :
#endif
	{
		__cp_got_info_start = .;
		__cp_got_start = .;
		*(.got)
		. = ALIGN(4);
		__cp_got_end = .;
		__cp_got_plt_start = .;
		*(.got.plt)
		. = ALIGN(4);
		__cp_igot_plt_start = .;
		*(.igot.plt)
		. = ALIGN(4);
		__cp_dynamic_start = .;
		*(.dynamic)
		. = ALIGN(4);
		__cp_got_info_end = .;
#if (CP_FLASH_SIZE > 0)
	} > FLASHX
#else
	} > MEMCPX
#endif

#if (CP_FLASH_SIZE > 0)
	.ARM.extab (.) : AT (FLASHX_TO_FLASH(ADDR(.ARM.extab)))
#else
	.ARM.extab (.) :
#endif
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
#if (CP_FLASH_SIZE > 0)
	} > FLASHX
#else
	} > MEMCPX
#endif

	__exidx_start = .;

#if (CP_FLASH_SIZE > 0)
	.ARM.exidx (.) : AT (FLASHX_TO_FLASH(__exidx_start))
#else
	.ARM.exidx (.) :
#endif
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
#if (CP_FLASH_SIZE > 0)
	} > FLASHX
#else
	} > MEMCPX
#endif

	. = ALIGN(4);
	__exidx_end = .;
	_eronly = .;

#if (CP_FLASH_SIZE > 0)
	__cp_text_sram_start_flash__ = FLASHX_TO_FLASH(__exidx_end);
    .cp_text_sram (ORIGIN(MEMCPX)) : AT (__cp_text_sram_start_flash__)
	{
		__cp_text_sram_start = .;
		*(.cp_text_sram*)
		. = ALIGN(4);
		__cp_text_sram_end = .;
	} > MEMCPX
	__cp_etext = __cp_text_sram_start_flash__ + SIZEOF(.cp_text_sram);
	__cp_data_start__flash__ = __cp_etext;
#else
	__cp_etext = .;
#endif

	.cp_data (MEMCPX_TO_MEMCP(.)) : AT (__cp_etext)
	{
		__cp_data_start__ = .;
		_sdata = .;
		*(.data*)
		. = ALIGN(4);

#ifndef NOSTD
		*(vtable)

		. = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(4);
		/* init data */
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);

		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);

		KEEP(*(.jcr*))
		. = ALIGN(4);
#endif
		_edata = .;
		/* All data end */
		__cp_data_end__ = .;
	} > MEMCP

#ifndef NUTTX_BUILD
	.vector_table_cp (.) (NOLOAD) :
	{
		. = ALIGN(0x200);
		__cp_vector_start__ = .;
		KEEP(*(.vector_table_cp))
		. = __cp_vector_start__ + VECTOR_SECTION_SIZE;
		. = ALIGN(4);
	} > MEMCP
#endif

	.cp_bss (.) (NOLOAD) : AT (ADDR(.cp_bss))
	{
		. = ALIGN(4);
		_sbss = .;
		__cp_bss_start__ = .;
		*(.bss*)
		*(.cp_bss*)
		*(COMMON)
#ifdef OS_HEAP_LINK_NAME
                . = ALIGN(64);
                __os_heap_start__ = .;
                OS_HEAP_LINK_NAME = .;
                . += OS_HEAP_SIZE;
                __os_heap_end__ = .;
#endif
		. = ALIGN(4);
		__cp_bss_end__ = .;
		_ebss = .;
#ifndef NOSTD
		__bss_start__ = __cp_bss_start__;
		__bss_end__ = __cp_bss_end__;
#endif
#ifdef OPENAMP_SUPPORT
		. = ALIGN(64);
		__share_mem_pa_start__ = .;
		. += SHARED_MEM_SIZE;
		__share_mem_pa_end__ = .;
#endif
		. = ALIGN(4);
	} > MEMCP

	.cp_heap (.) (NOLOAD) :
	{
		. = ALIGN(8);
		__cp_HeapBase = .;
#ifndef NOSTD
		__end__ = .;
		end = __end__;
		. += HEAP_SECTION_SIZE;
#endif
		. = ALIGN(8);
		__cp_HeapLimit = .;
	} > MEMCP

	/* .stack_dummy section doesn't contains any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
	.cp_stack_dummy (.) (COPY) :
	{
		. = STACK_SECTION_SIZE;
		. = ALIGN(8);
	} > MEMCP

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__cp_StackTop = ORIGIN(MEMCP) + LENGTH(MEMCP);
	__cp_StackLimit = __cp_StackTop - SIZEOF(.cp_stack_dummy);
	PROVIDE(__cp_stack = __cp_StackTop);
#ifndef NOSTD
	__StackLimit = __cp_StackLimit;
	__StackTop = __cp_StackTop;
	PROVIDE(__stack = __cp_StackTop);
#endif
	_sstack = __cp_StackLimit;
	_estack = __cp_StackTop;

	/* Check if data + heap + stack exceeds RAM limit */
	ASSERT(__cp_StackLimit >= __cp_HeapLimit, "region RAM overflowed with stack")
	__free_cp_ram = __cp_StackLimit - __cp_HeapLimit;
	__cp_text_data_end = __cp_etext + SIZEOF(.cp_data);

#if defined(PSRAMCP_BASE) && (PSRAMCP_SIZE > 0) && (RAMCP_SIZE > 0)
	.cp_sram_bss (ORIGIN(RAMCP)) (NOLOAD) : AT (ADDR(.cp_sram_bss))
	{
		. = ALIGN(4);
		*(.sram_bss*)
	} > RAMCP
#endif
#ifdef NUTTX_BUILD
	_nuttx_heap_start = __cp_HeapLimit;
	_nuttx_heap_end = __cp_StackLimit - 4;
#endif


#if defined(RAMV_BASE) && (RAMV_SIZE > 0)
	.cp_vram_bss (ORIGIN(RAMV)) (NOLOAD) : AT (ADDR(.cp_vram_bss))
	{
		. = ALIGN(4);
		*(.vram_bss*)
	} > RAMV
#endif
}
