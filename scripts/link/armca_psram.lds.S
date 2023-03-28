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

#if DSP_RAM_SIZE
#define __TTB_SIZE                  0x5000
#else
#define __TTB_SIZE                  0x8000
#endif

#if defined(NUTTX_BUILD)
#undef __TTB_SIZE
#define __TTB_SIZE                  0x0
#endif

#ifdef NOSTD
#define __HEAP_SIZE                 0x1000
#else
#ifndef RTOS
#ifdef EXEC_IN_RAM
#define __HEAP_SIZE                 0x1000
#else
#define __HEAP_SIZE                 0x280000
#endif
#else
#define __HEAP_SIZE                 0x1000
#endif
#endif

#ifdef KERNEL_RTT
#define __STACK_SIZE                0x4000
#define __MAIN_STACK_SIZE           0x4000
#elif KERNEL_LITEOS_A
#define __STACK_SIZE                0x8000
#define __MAIN_STACK_SIZE           0
#else
#define __STACK_SIZE                0x1000
#define __MAIN_STACK_SIZE           0x1000
#endif
#ifdef KERNEL_RTT
#define __FIQ_STACK_SIZE            0
#define __IRQ_STACK_SIZE            0x2000
#define __SVC_STACK_SIZE            0x2000
#define __ABT_STACK_SIZE            0
#define __UND_STACK_SIZE            0x2000
#elif KERNEL_LITEOS_A
#define __FIQ_STACK_SIZE            0
#define __IRQ_STACK_SIZE            0
#define __SVC_STACK_SIZE            0
#define __ABT_STACK_SIZE            0
#define __UND_STACK_SIZE            0
#else
#define __FIQ_STACK_SIZE            0
#define __IRQ_STACK_SIZE            0x1000
#define __SVC_STACK_SIZE            0x1000
#define __ABT_STACK_SIZE            0
#define __UND_STACK_SIZE            0
#endif

/* TODO: Add FLASH_REGION_OFFSET if there are multiple levels of boot loaders */
#if !defined(OTA_BOOT_SIZE) && defined(OTA_CODE_OFFSET)
#define FLASH_REGION_BASE           (FLASH_BASE + OTA_CODE_OFFSET)
#else
#ifdef OTA_BOOT_OFFSET
#define FLASH_REGION_BASE			(FLASH_BASE + OTA_BOOT_OFFSET)
#else
#define FLASH_REGION_BASE			(FLASH_BASE)
#endif
#endif

#ifndef FLASH_REGION_SIZE
#define FLASH_REGION_SIZE           (FLASH_SIZE - (FLASH_REGION_BASE - FLASH_BASE))
#endif

#if (FLASH_REGION_BASE + FLASH_REGION_SIZE) > (FLASH_BASE + FLASH_SIZE)
#error "(FLASH_REGION_BASE + FLASH_REGION_SIZE) should <= (FLASH_BASE + FLASH_SIZE)"
#endif

#ifdef EXEC_IN_RAM
#define EXEC_REGION                 RAM
#elif defined(EXEC_IN_PSRAM)
#define EXEC_REGION                 PSRAM
#else
#define EXEC_REGION                 PSRAMUHS
#endif

#ifdef LOAD_IN_RAM
#define LOAD_REGION                 RAM
#elif defined(LOAD_IN_PSRAM)
#define LOAD_REGION                 PSRAM
#else
#define LOAD_REGION                 PSRAMUHS
#endif

#define BES_RPTUN_HEAP_SIZE   (0x20000 -0x1000)

MEMORY
{
    BOOT        (rwx)   : ORIGIN = DSP_BOOT_BASE,           LENGTH = DSP_BOOT_SIZE
    FLASH       (rx)    : ORIGIN = FLASH_REGION_BASE,       LENGTH = FLASH_REGION_SIZE
#if DSP_RAM_SIZE
    RAM         (rwx)   : ORIGIN = DSP_RAM_BASE,            LENGTH = DSP_RAM_SIZE - __TTB_SIZE
    L_TTB       (rw)    : ORIGIN = DSP_RAM_BASE + DSP_RAM_SIZE - __TTB_SIZE,    LENGTH = __TTB_SIZE
#else
    RAM         (rwx)   : ORIGIN = DSP_PSRAMUHS_BASE,            LENGTH = DSP_PSRAMUHS_SIZE - __TTB_SIZE
    L_TTB       (rw)    : ORIGIN = DSP_PSRAMUHS_BASE + DSP_PSRAMUHS_SIZE - __TTB_SIZE,    LENGTH = __TTB_SIZE
#endif
#if defined(PSRAM_BASE) && (PSRAM_SIZE > 0)
    PSRAM       (rwx)   : ORIGIN = PSRAM_BASE,              LENGTH = PSRAM_SIZE
#endif
#ifdef DSP_PSRAMUHS_BASE
    PSRAMUHS    (rwx)   : ORIGIN = DSP_PSRAMUHS_BASE,          LENGTH = DSP_PSRAMUHS_SIZE
    PSRAMUHSX   (rx)    : ORIGIN = DSP_PSRAMUHSX_BASE,         LENGTH = DSP_PSRAMUHS_SIZE
#endif
}

/* Library configurations */
GROUP(libgcc.a)

#ifdef GEN_BOOT_SECTION
ENTRY(Vectors)
#else
#ifndef RTOS
ENTRY(Reset_Handler)
#else
#ifdef KERNEL_RTT
ENTRY(system_vectors)
#elif defined(KERNEL_LITEOS_A)
ENTRY(reset_vector)
#else
ENTRY(vector_reset)
#endif
#endif
#endif

SECTIONS
{
#ifdef GEN_BOOT_SECTION

    .boot :
    {
        KEEP(*(.vectors))
    } > BOOT

    /DISCARD/ :
    {
        *(*)
    }
#else // !GEN_BOOT_SECTION

    .boot_loader :
    {
#if defined(NUTTX_BUILD) || defined(KERNEL_LITEOS_A)
        KEEP(*(.vectors))
#else
        *(.boot_loader)
        *(.boot_text_flash*)
#endif
        . = ALIGN(4);
    } > EXEC_REGION

    .sram_text :
    {
        __sram_text_data_start__ = .;
        *:_aeabi_*.o(.text*)
        *:arm_*.o(.text* .rodata*)

#if (LOAD_REGION != EXEC_REGION)
        /* memcpy.o or libc_nano.a(lib_a-memcpy.o/lib_a-memcpy-stub.o) */
        *:memcpy.o(.text*)
        *:lib_a-memcpy*.o(.text*)
        *:libc_rom.o(.text*)

        /* memset.o or libc_nano.a(lib_a-memset.o) */
        *:memset.o(.text*)
        *:lib_a-memset*.o(.text*)
        /* hal transq*/
        *:hal_transq.o(.text*)

        *(.boot_text_sram*)
        *(.sram_text*)
        *(.fast_text_sram*)

        . = ALIGN(4);
#endif
    } > EXEC_REGION AT > EXEC_REGION

    __sram_text_data_start_load__ = LOADADDR(.sram_text);

    .sram_data :
    {
#if (LOAD_REGION != EXEC_REGION)
        *(.boot_rodata_sram*)
        *(.boot_data*)
        . = ALIGN(4);
#if defined(NUTTX_BUILD)
        *:arm_vectors.o(.data*)
#endif
        *:hal_transq.o(.data*)
        *(.sram_data*)
        *(.psram_data*)
        . = ALIGN(4);
#endif
    } > EXEC_REGION AT > EXEC_REGION

    __sram_text_data_end_load__ = LOADADDR(.sram_data) + SIZEOF(.sram_data);

#if defined(PSRAM_BASE) && (PSRAM_SIZE > 0) && !defined(EXEC_IN_PSRAM)
    . = ORIGIN(PSRAM);
    /*
    __psram_text_data_start_load__ = .;
    .psram_text (ORIGIN(PSRAM)) : AT (__psram_text_data_start_load__)
    */
    .psram_text (.) : AT (ADDR(.psram_text))
    {
        __psram_text_start = .;
        *(.psram_text*)
#ifdef DSP_USE_PSRAM
        *(.text*)
        *(.flash_text*)
        *(.rodata*)
#endif
        . = ALIGN(4);
        __psram_text_end = .;
    } > PSRAM

	.psram_data (.) : AT (ADDR(.psram_data))
	{
		*(.psram_data*)
		. = ALIGN(4);
	} > PSRAM

	.psram_bss (.) (NOLOAD) : AT (ADDR(.psram_bss))
	{
		__psram_bss_start__ = .;
		*(.psram_bss*)
		. = ALIGN(4);
		__psram_bss_end__ = .;
	} > PSRAM
#endif
	. = __sram_text_data_end_load__;
    __text_start = .;
    .text :
    {
        *(SVC_TABLE)
        *(.text*)
        *(.flash_text*)

        KEEP(*(.init))
        KEEP(*(.fini))

        . = ALIGN(4);
        _sinit = .;
        /* preinit data */
        PROVIDE (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE (__preinit_array_end = .);
        /* init data */
        PROVIDE(__ctors_start__ = .);
        PROVIDE (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE (__init_array_end = .);
        PROVIDE(__ctors_end__ = .);
        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)
        _einit = .;
        . = ALIGN(4);
        /* finit data */
        PROVIDE (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE (__fini_array_end = .);
        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        . = ALIGN(4);

        __rodata_start = .;
        __kernel_init_level_0 = ABSOLUTE(.);
        KEEP(*( SORT (.rodata.init.kernel.0.*)));
        __kernel_init_level_1 = ABSOLUTE(.);
        KEEP(*( SORT (.rodata.init.kernel.1.*)));
        __kernel_init_level_2 = ABSOLUTE(.);
        KEEP(*( SORT (.rodata.init.kernel.2.*)));
        __kernel_init_level_3 = ABSOLUTE(.);
        KEEP(*( SORT (.rodata.init.kernel.3.*)));
        __kernel_init_level_4 = ABSOLUTE(.);
        KEEP(*( SORT (.rodata.init.kernel.4.*)));
        __kernel_init_level_5 = ABSOLUTE(.);
        KEEP(*( SORT (.rodata.init.kernel.5.*)));
        __kernel_init_level_6 = ABSOLUTE(.);
        KEEP(*( SORT (.rodata.init.kernel.6.*)));
        __kernel_init_level_7 = ABSOLUTE(.);
        KEEP(*( SORT (.rodata.init.kernel.7.*)));
        __kernel_init_level_8 = ABSOLUTE(.);
        KEEP(*( SORT (.rodata.init.kernel.8.*)));
        __kernel_init_level_9 = ABSOLUTE(.);
        KEEP(*( SORT (.rodata.init.kernel.9.*)));
        __kernel_init_level_10 = ABSOLUTE(.);

        _hdf_drivers_start = .;
        KEEP(*(.hdf.driver));
        _hdf_drivers_end = .;

        KEEP(*( SORT (.liteos.table.*)));

        __exc_table_start = .;
        KEEP(*(__exc_table))
        __exc_table_end = .;

        *(.rodata*)
        *(.boot_rodata*)
        __rodata_end = .;

        . = ALIGN(4);

        KEEP(*(.eh_frame*))
        KEEP(*(.build_info))

        *(.note.gnu.build-id)

        /*. = ALIGN(4);*/
        . = ALIGN(0x1000);
        _mmu_text_end = .;
        _etext = .;
    } > EXEC_REGION
    __text_end = .;

    _mmu_text_start = ORIGIN(EXEC_REGION);
    _stext = _mmu_text_start;
    /*
    _mmu_text_end = __rodata_end;
    */
    _mmu_text_len = _mmu_text_end - _mmu_text_start;
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > EXEC_REGION

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > EXEC_REGION
    __exidx_end = .;

    .copy.table :
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        LONG (__etext)
        LONG (__data_start__)
        LONG (__data_end__ - __data_start__)
        __copy_table_end__ = .;
    } > EXEC_REGION

    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (__bss_start__)
        LONG (__bss_end__ - __bss_start__)
        __zero_table_end__ = .;
    } > EXEC_REGION

    __etext = .;

    .tdata : {
        __tdata_start = .;
        *(.tdata)
        *(.tdata.*)
        *(.gnu.linkonce.td.*)
        __tdata_end = .;
    } > EXEC_REGION
    .data :
    {
        Image$$RW_DATA$$Base = .;
        __data_start__ = .;
        _sdata = .;
        *(.data*)
        Image$$RW_DATA$$Limit = .;
        . = ALIGN(4);
        /* All data end */
        *(*.got*)
        __data_end__ = .;
        _edata = .;

    } > EXEC_REGION

    .sram_bss (NOLOAD) : AT (ADDR(.sram_bss))
    {
        __sram_bss_start__ = .;
        *(.boot_bss*)
        . = ALIGN(4);
        *(.sram_bss*)
        . = ALIGN(4);
        __sram_bss_end__ = .;
    } > EXEC_REGION

#ifndef EXEC_IN_RAM
    __ram_end = .;
#endif
    .sbss (NOLOAD) : {
        __sbss_start = .;
        *(.sbss)
        *(.sbss.*)
        *(.gnu.linkonce.sb.*)
        __sbss_end = .;
    } > EXEC_REGION

    .tbss (NOLOAD) : {
        __tbss_start = .;
        *(.tbss)
        *(.tbss.*)
        *(.gnu.linkonce.tb.*)
        __tbss_end = .;
    } > EXEC_REGION

#if defined(__HEAP_SIZE) && (__HEAP_SIZE > 0)
    .heap (NOLOAD) :
    {
        . = ALIGN(8);
        Image$$HEAP$$ZI$$Base = .;
        . += __HEAP_SIZE;
        Image$$HEAP$$ZI$$Limit = .;
        __HeapLimit = .;
    } > EXEC_REGION
#endif

#ifdef EXEC_IN_RAM
    __ram_end = .;
#else
    __free_exec_region = ORIGIN(EXEC_REGION) + LENGTH(EXEC_REGION) - .;
#endif

#ifdef EXEC_IN_RAM
    .sync_flags ALIGN(0x1000) (NOLOAD) :
    {
        __sync_flags_start = .;
        *(.sync_flags*)
        . = ALIGN(0x1000);
        __sync_flags_end = .;
    } > EXEC_REGION

    __psramuhs_text_data_start_load__ = LOADADDR(.data) + SIZEOF(.data);
    .psramuhs_text (ORIGIN(PSRAMUHS)) : AT (__psramuhs_text_data_start_load__)
    {
        __psramuhs_text_start = .;
        *(.psramuhs_text*)
        . = ALIGN(4);
        __psramuhs_text_end = .;
    } > PSRAMUHS
    __psramuhs_text_data_end_load__ = __psramuhs_text_data_start_load__ + SIZEOF(.psramuhs_text);

    .psramuhs_data (ORIGIN(PSRAMUHS) + SIZEOF(.psramuhs_text)) :
		AT (__psramuhs_text_data_end_load__)
    {
        __psramuhs_data_start = .;
        *(.psramuhs_data*)
        . = ALIGN(4);
        __psramuhs_data_end = .;
    } > PSRAMUHS
    __psramuhs_text_data_end_load__ += SIZEOF(.psramuhs_data);
    .psramuhs_bss (.) (NOLOAD) : AT (ADDR(.psramuhs_bss))
    {
        __psramuhs_bss_start__ = .;
        *(.psramuhs_bss*)
        . = ALIGN(4);
        __psramuhs_bss_end__ = .;
    } > PSRAMUHS
    __exec_load_end__ = __psramuhs_text_data_end_load__;
#else
    __exec_load_end__ = LOADADDR(.data) + SIZEOF(.data);
    .psramuhs_bss (.) (NOLOAD) : AT (ADDR(.psramuhs_bss))
    {
        __psramuhs_bss_start__ = .;
        _sbss = .;
        *(.psramuhs_bss*)
        . = ALIGN(4);
        _ebss = .;
        . = ALIGN(8);
        __psramuhs_bss_end__ = .;
    } > PSRAMUHS

#if !defined(NUTTX_BUILD)
    .sync_flags (ORIGIN(RAM)) (NOLOAD) :
    {
        __sync_flags_start = .;
        *(.sync_flags*)
        __sync_flags_end = .;
    } > RAM
#endif
#endif
    /* The following section MUST be the last loaded section */
    .code_start_addr : AT (__exec_load_end__)
    {
        LONG(ADDR(.boot_loader));
    } > EXEC_REGION

    .bss ALIGN(0x400) (NOLOAD) :
    {
        __bss_start = .;
        Image$$ZI_DATA$$Base = .;
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
        Image$$ZI_DATA$$Limit = .;
        __end__ = .;
        end = __end__;
        __bss_end = .;
#if defined(OS_HEAP_SIZE) && (OS_HEAP_SIZE > 0)
	. += OS_HEAP_SIZE;
#endif
    } > EXEC_REGION

#if defined(NUTTX_BUILD)
    .noinit :
    {
        _snoinit = ABSOLUTE(.);
        *(.noinit*)
        _enoinit = ABSOLUTE(.);
        . = ALIGN(8);
        . += 0x1000; /* Idle task stack*/
        . = ALIGN(8);
        _nuttx_heap_start = .;
    } > EXEC_REGION
#endif

#if !defined(NUTTX_BUILD)
#if DSP_RAM_SIZE
    .stack (ORIGIN(RAM) + LENGTH(RAM) - __TTB_SIZE - __MAIN_STACK_SIZE - __STACK_SIZE - __FIQ_STACK_SIZE - __IRQ_STACK_SIZE - __SVC_STACK_SIZE - __ABT_STACK_SIZE - __UND_STACK_SIZE) (NOLOAD) : AT (ADDR(.stack))
#else
    .stack (ORIGIN(EXEC_REGION) + LENGTH(EXEC_REGION) - __TTB_SIZE - __MAIN_STACK_SIZE - __STACK_SIZE - __FIQ_STACK_SIZE - __IRQ_STACK_SIZE - __SVC_STACK_SIZE - __ABT_STACK_SIZE - __UND_STACK_SIZE) (NOLOAD) : AT (ADDR(.stack))
#endif
    {
        . = ALIGN(8);
#ifndef KERNEL_LITEOS_A
         __StackLimit = .;
        . += __MAIN_STACK_SIZE;
        Image$$SYS_STACK$$ZI$$Base = .;
        . += __STACK_SIZE;
        Image$$SYS_STACK$$ZI$$Limit = .;
        __stack = .;

        Image$$FIQ_STACK$$ZI$$Base = .;
        . += __FIQ_STACK_SIZE;
        Image$$FIQ_STACK$$ZI$$Limit = .;

        Image$$IRQ_STACK$$ZI$$Base = .;
        . += __IRQ_STACK_SIZE;
        Image$$IRQ_STACK$$ZI$$Limit = .;

        Image$$SVC_STACK$$ZI$$Base = .;
        . += __SVC_STACK_SIZE;
        Image$$SVC_STACK$$ZI$$Limit = .;

        Image$$ABT_STACK$$ZI$$Base = .;
        . += __ABT_STACK_SIZE;
        Image$$ABT_STACK$$ZI$$Limit = .;

        Image$$UND_STACK$$ZI$$Base = .;
        . += __UND_STACK_SIZE;
        Image$$UND_STACK$$ZI$$Limit = .;
        . = ALIGN(8);
        __StackTop = .;
#else
        *(.int_stack*)
#endif
#if DSP_RAM_SIZE
    } > RAM
#else
    } > EXEC_REGION
#endif

    __free_ram = ADDR(.stack) - __ram_end;
#ifdef EXEC_IN_RAM
    __free_exec_region = __free_ram;
#endif
#endif

    .ttb_l2 (NOLOAD) :
    {
        Image$$TTB$$ZI$$Base = .;
        *(.ttb_l2)
    } > L_TTB

    .ttb_l1 (NOLOAD) :
    {
        *(.ttb_l1)
        Image$$TTB$$ZI$$Limit = .;
    } > L_TTB

    __free_ttb = ORIGIN(L_TTB) + LENGTH(L_TTB) - .;

#if defined(NUTTX_BUILD)
    .framebuffer (ORIGIN(RAM)) (NOLOAD) :
    {
        *(.framebuffer*)
    } > RAM
#endif

#endif // !GEN_BOOT_SECTION
}

