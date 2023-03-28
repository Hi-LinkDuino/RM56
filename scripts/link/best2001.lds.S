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

#if defined(KERNEL_LITEOS_M)
#define CUSTOM_REBOOT_DUMP_SIZE		0x4000
#endif

#ifdef RB_CODEC
#define HEAP_SECTION_SIZE			0x1000
#else
#define HEAP_SECTION_SIZE			0x2000
#endif

#if defined(NUTTX_BUILD)
#include "nuttx/config.h"
#define NUTTX_HEAP_SECTION_SIZE		0x20000
#define STACK_SECTION_SIZE		CONFIG_IDLETHREAD_STACKSIZE
#undef HEAP_SECTION_SIZE
#define HEAP_SECTION_SIZE		0x0000
#else
#define STACK_SECTION_SIZE		0x2000
#endif
#if defined(CP_ENABLE) && !defined(CP_BOOT)
#define CP_STACK_SECTION_SIZE		0x6000
#else
#define CP_STACK_SECTION_SIZE		0x1000
#endif

#if defined(ROM_UTILS_ON)
#if defined(CHIP_BEST1400) || defined(CHIP_BEST1402)
#define ROM_UTILS_RESV_RAM_SIZE     0x64
#else
#define ROM_UTILS_RESV_RAM_SIZE     0x0
#endif
#endif

#if defined(CHIP_HAS_CP) && (RAMCP_SIZE > 0)
#define OVERLAY_DATA_SECTION_SIZE	0x10000
#elif defined(RB_CODEC)
#define OVERLAY_DATA_SECTION_SIZE	0xA000
#else
#define OVERLAY_DATA_SECTION_SIZE	0x8000
#endif

#if !defined(FAST_XRAM_SECTION_SIZE)
#if (RAM_SIZE >= 0x48000)
#define RTOS_IN_RAM
#if defined(BTUSB_AUDIO_MODE)
#define FAST_XRAM_SECTION_SIZE		0x20000
#elif (defined(CHIP_BEST1402) && defined(SPEECH_TX_2MIC_NS2))
#define FAST_XRAM_SECTION_SIZE		0xF800
#else
#define FAST_XRAM_SECTION_SIZE		0x16000
#endif
#elif (RAM_SIZE >= 0x40000)
#if defined(CHIP_BEST1402) || defined(CHIP_BEST1400)
#define FAST_XRAM_SECTION_SIZE		0x5200
#else
#define FAST_XRAM_SECTION_SIZE		0x3800
#endif
#else
#define FAST_XRAM_SECTION_SIZE		0x6000
#endif
#endif
#if defined(NUTTX_BUILD) && defined(ARM_CMSE)
#undef FAST_XRAM_SECTION_SIZE
#define FAST_XRAM_SECTION_SIZE		0x0
#endif
#if !defined(OTA_BOOT_SIZE) && defined(OTA_CODE_OFFSET)
#define FLASH_REGION_BASE			(FLASH_BASE + OTA_CODE_OFFSET)
#else
#ifdef OTA_BOOT_OFFSET
#define FLASH_REGION_BASE			(FLASH_BASE + OTA_BOOT_OFFSET)
#else
#define FLASH_REGION_BASE			(FLASH_BASE)
#endif
#endif

#ifndef FLASH_REGION_SIZE
#define FLASH_REGION_SIZE			(FLASH_SIZE - (FLASH_REGION_BASE - FLASH_BASE) - CP_FLASH_SIZE)
#endif

#if (FLASH_REGION_BASE + FLASH_REGION_SIZE) > (FLASH_BASE + FLASH_SIZE)
#error "(FLASH_REGION_BASE + FLASH_REGION_SIZE) should <= (FLASH_BASE + FLASH_SIZE)"
#endif

#define FLASH_NC_REGION_BASE		FLASH_C_TO_NC(FLASH_REGION_BASE)
#define FLASHX_REGION_BASE			FLASH_TO_FLASHX(FLASH_REGION_BASE)

#if defined(CHIP_HAS_CP) && (RAMCP_SIZE > 0)
#define OVERLAY_FOR_CP
#endif

#ifdef OVERLAY_FOR_CP
#define REGION_OVERLAY_TEXT			RAMCPX
#define REGION_OVERLAY_DATA			RAMCP
#else
#define REGION_OVERLAY_TEXT			FRAMX
#define REGION_OVERLAY_DATA			RAM
#endif

/* Linker script to configure memory regions. */
/* See plat_addr_map.h and common.mk for the macro definitions */
MEMORY
{
	ROM			(rx)	: ORIGIN = ROM_BASE,				LENGTH = ROM_SIZE
	FLASH 		(r)		: ORIGIN = FLASH_REGION_BASE,		LENGTH = FLASH_REGION_SIZE
	FLASH_NC	(r)		: ORIGIN = FLASH_NC_REGION_BASE,	LENGTH = FLASH_REGION_SIZE
	FLASHX		(rx)	: ORIGIN = FLASHX_REGION_BASE,		LENGTH = FLASH_REGION_SIZE
#ifdef CHIP_HAS_EXT_NORFLASH
	FLASH1 		(r)		: ORIGIN = FLASH1_BASE,		LENGTH = 0x1000000
	FLASH1_NC	(r)		: ORIGIN = FLASH1_NC_BASE,	LENGTH = 0x1000000
	FLASH1X		(rx)	: ORIGIN = FLASH1X_BASE,	LENGTH = 0x1000000
#endif
	RAM			(rwx)	: ORIGIN = RAM_BASE,				LENGTH = RAM_SIZE - FAST_XRAM_SECTION_SIZE
	RAMX		(rx)	: ORIGIN = RAMX_BASE,				LENGTH = RAM_SIZE - FAST_XRAM_SECTION_SIZE
#if defined(ARM_CMSE) && (RAM_NSC_SIZE != 0)
	RAMX_NSC	(rx)	: ORIGIN = RAMX_BASE + RAM_S_SIZE,	LENGTH = RAM_NSC_SIZE
#endif
	FRAMX		(rwx)	: ORIGIN = RAMX_BASE + RAM_SIZE - FAST_XRAM_SECTION_SIZE,	LENGTH = FAST_XRAM_SECTION_SIZE
#ifdef PSRAM_BASE
	PSRAM 		(rwx)	: ORIGIN = PSRAM_BASE,				LENGTH = PSRAM_SIZE
	PSRAM_NC	(rwx)	: ORIGIN = PSRAM_NC_BASE,			LENGTH = PSRAM_SIZE
	PSRAMX		(rx)	: ORIGIN = PSRAMX_BASE,				LENGTH = PSRAM_SIZE
#endif
#ifdef PSRAMUHS_BASE
	PSRAMUHS	(rwx)	: ORIGIN = MCU_PSRAMUHS_BASE,			LENGTH = MCU_PSRAMUHS_SIZE
	PSRAMUHS_NC	(rwx)	: ORIGIN = MCU_PSRAMUHS_NC_BASE,		LENGTH = MCU_PSRAMUHS_SIZE
	PSRAMUHSX	(rx)	: ORIGIN = MCU_PSRAMUHSX_BASE,			LENGTH = MCU_PSRAMUHS_SIZE
#endif
#if defined(CHIP_HAS_CP) && (RAMCP_SIZE > 0)
	RAMCP 		(rwx)	: ORIGIN = RAMCP_BASE,				LENGTH = RAMCP_SIZE
	RAMCPX 		(rwx)	: ORIGIN = RAMCPX_BASE,				LENGTH = RAMCPX_SIZE
#endif
#ifdef MEM_POOL_BASE
	MEM_POOL	(rwx)	: ORIGIN = MEM_POOL_BASE,			LENGTH = MEM_POOL_SIZE
#endif
#if defined(RAMV_BASE) && (RAMV_SIZE > 0)
	RAMV 		(rwx)	: ORIGIN = RAMV_BASE,				LENGTH = RAMV_SIZE
#endif
}

/* Library configurations */
#ifndef NUTTX_BUILD
#ifndef NO_LIBC
GROUP(libgcc.a libc.a libm.a libnosys.a)
#endif
#endif

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __export_fn_rom
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   __free_ram
 *   __factory_start
 *   __factory_end
 *   __calib_start
 *   __calib_end
 *   __flash_start
 *   __flash_end
 *   __free_flash
 *   __boot_sram_start_flash__
 *   __boot_sram_end_flash__
 *   __boot_sram_start__
 *   __boot_bss_sram_start__
 *   __boot_bss_sram_end__
 *   __sram_text_data_start_flash__
 *   __sram_text_data_end_flash__
 *   __fast_sram_text_data_start__
 *   __fast_sram_text_data_end__
 *   __fast_sram_text_data_start_flash__
 *   __fast_sram_text_data_end_flash__
 *   __sram_text_data_start__
 *   __sram_bss_start__
 *   __sram_bss_end__
 *   __overlay_text_start__
 *   __overlay_text_exec_start__
 *   __overlay_data_start__
 */
ENTRY(Boot_Loader)

SECTIONS
{
	__export_fn_rom = (ORIGIN(ROM) + LENGTH(ROM) - ROM_BUILD_INFO_SECTION_SIZE - ROM_EXPORT_FN_SECTION_SIZE);

	.boot_struct (ORIGIN(FLASH)) :
	{
		__flash_start = .;
		KEEP(*(.boot_struct))
		. = ALIGN(4);
	} > FLASH

	. = FLASH_TO_FLASHX(.);

	.boot_text_flash (.) : AT (FLASHX_TO_FLASH(ADDR(.boot_text_flash)))
	{
		__flashx_start_addr__ = .;
		*(.boot_loader)
		*(.boot_text_flash*)
		*(.boot_rodata_flash*)
		. = ALIGN(4);
	} > FLASHX

#if defined(__BES_OTA_MODE__)
	.ota_boot_info (ALIGN(4096)) :
	{
		*(.ota_boot_info)
		. = (0x1000);
	} > FLASHX

	.ota_boot_rev (ALIGN(4096)) :
	{
		*(.ota_boot_rev)
		. = (0x1000);
	} > FLASHX
#endif

	. = FLASHX_TO_FLASH(.);

#if defined(CHIP_HAS_EXT_NORFLASH) && defined(TEST_FOR_EXT_NORFLASH)
	__flash2_code_start_flash = .;
	.flash2_code (ORIGIN(FLASH1X)): AT(__flash2_code_start_flash)
	{
		__flash2_code_start = .;
		*:flash2_demo*.o(.text*)
		/**(.flash2_code)*/
		. = ALIGN(4);
		__flash2_code_end = .;		
	} > FLASH1X
	__flash2_code_end_flash = __flash2_code_start_flash + SIZEOF(.flash2_code);
	. = __flash2_code_end_flash;
#endif

	.got_info (.) :
	{
		__got_info_start = .;
		__got_start = .;
		*(.got)
		. = ALIGN(4);
		__got_end = .;
		__got_plt_start = .;
		*(.got.plt)
		. = ALIGN(4);
		__igot_plt_start = .;
		*(.igot.plt)
		. = ALIGN(4);
		__dynamic_start = .;
		*(.dynamic)
		. = ALIGN(4);
		__got_info_end = .;
	} > FLASH

	.vector_table (ORIGIN(RAM)) (NOLOAD) :
	{
		KEEP(*(.vector_table))
		. = VECTOR_SECTION_SIZE;
		. = ALIGN(4);
	} > RAM

	.reboot_param (.) (NOLOAD) :
	{
		KEEP(*(.reboot_param))
		. = REBOOT_PARAM_SECTION_SIZE;
		. = ALIGN(4);
	} > RAM

	.userdata_pool (.) (NOLOAD) :
	{
		*(.userdata_pool)
		. = ALIGN(4);
		__userdata_pool_end__ = .;
	} > RAM

	__boot_sram_start_flash__ = LOADADDR(.got_info) + SIZEOF(.got_info);
	__boot_sram_start__ = __userdata_pool_end__;

	.boot_text_sram (RAM_TO_RAMX(__boot_sram_start__)) : AT (__boot_sram_start_flash__)
	{
		_stext = .;
		/* memcpy.o or libc_nano.a(lib_a-memcpy.o/lib_a-memcpy-stub.o) */
		*:memcpy.o(.text*)
		*:lib_a-memcpy*.o(.text*)
		*:libc_rom.o(.text*)
		*:libc_rom_mem.o(.text*)

		/* memset.o or libc_nano.a(lib_a-memset.o) */
		*:memset.o(.text*)
		*:lib_a-memset*.o(.text*)

		*:hal_norflash*.o(.text*)
		*:norflash_*.o(EXCLUDE_FILE(*:norflash_api.o) .text*)

#ifdef BOOT_CODE_IN_RAM
		*:hal_cmu_*.o(.text*)
		*:hal_iomux_*.o(.text*)
		*:hal_analogif_*.o(.text*)
		*:hal_spi.o(.text*)
		*:hal_timer.o(.text*)
		*:hal_sysfreq.o(.text*)
#endif

		*:hal_mcu2cp.o(.text*)
		*:wifi_agent.o(.text*)
#ifndef WIFI_ON_CP
		*:wifi_srv.o(.text*)
#endif
		*:ethernetif.o(.text*)

#ifdef ROM_IN_FLASH
		*:hal_cmu_*.o(.text.hal_cmu_flash*_set_freq .text.hal_cmu_reset_set .text.hal_cmu_reset_clear)
		*:hal_cmu_common.o(.text.hal_cmu_get_crystal_freq)
		*:hal_timer.o(.text.hal_sys_timer_delay .text.hal_sys_timer_delay_us
			.text.hal_sys_timer_delay_ns .text.hal_sys_timer_get
			.text.hal_fast_sys_timer_get)
#endif
#if defined(ARM_CMSE) && defined(INTSRAM_RUN)
		/* BEST2003 SAU set FLASH_BASE region as nonsecure region becase of the SAU region number limitation,
		CPU can't access FLASH_BASE region in secure state, so the text should be located in SRAM */
		*(EXCLUDE_FILE(*crt*) .text*)
#endif

		*psram*.o(.text*)
		*(.boot_text_sram*)
		. = ALIGN(4);
		__boot_text_sram_end__ = .;
	} > RAMX

	.boot_data_sram (RAMX_TO_RAM(ADDR(.boot_text_sram) + SIZEOF(.boot_text_sram))) :
					AT (__boot_sram_start_flash__ + SIZEOF(.boot_text_sram))
	{
		__boot_data_sram_start__ = .;
		*:hal_norflash*.o(.data* .rodata*)
		*:norflash_*.o(EXCLUDE_FILE(*:norflash_api.o) .data* EXCLUDE_FILE(*:norflash_api.o) .rodata*)
		*psram*.o(.data* .rodata*)
		*:libc_rom.o(.rodata*)
		*:libc_rom_mem.o(.rodata*)

#if defined(ARM_CMSE) && defined(INTSRAM_RUN)
		/* BEST2003 SAU set FLASH_BASE region as nonsecure region becase of the SAU region number limitation,
		CPU can't access FLASH_BASE region in secure state, so the data and rodata should be located in SRAM */
		*(.data* .rodata*)
#endif
		/* Stack protector instrumentation rodata for functions in special sections */
		*(.rodata.cst4)

		*(.boot_rodata_sram*)
		*(.boot_data*)
		. = ALIGN(4);
#ifdef OPENAMP_SUPPORT
		*(.resource_table)
		. = ALIGN(4);
#endif
		__boot_data_sram_end__ = .;
	} > RAM

	__boot_sram_end_flash__ = __boot_sram_start_flash__ +
								SIZEOF(.boot_text_sram) + SIZEOF(.boot_data_sram);

	.boot_bss_sram (.) (NOLOAD) : AT (ADDR(.boot_bss_sram))
	{
		__boot_bss_sram_start__ = .;
		*:hal_norflash*.o(.bss*)
		*:norflash_*.o(EXCLUDE_FILE(*:norflash_api.o) .bss*)
		*psram*.o(.bss*)
		*(.boot_bss*)
		. = ALIGN(32);
		__boot_bss_sram_end__ = .;
	} > RAM

	__sram_text_data_start_flash__ = __boot_sram_end_flash__;
	__sram_text_data_start__ = __boot_bss_sram_end__;

	.sram_text (RAM_TO_RAMX(__sram_text_data_start__)) : AT (__sram_text_data_start_flash__)
	{
		__sram_text_start__ = .;
#ifdef RTOS_IN_RAM
		*:rtos_lib.o(.text*)
#endif

#if defined(KERNEL_LITEOS_M)
		/*
		   Enhance wifi throughput in the liteos system.
		   include file *los_*, *driverif*, *sys_arch*, *sockets_porting*, *ethernetif*, *tcp*
		 */
		*los_*.o(.text*)
		*driverif*.o(.text*)
		*sys_arch*.o(.text*)
		*sockets_porting*.o(.text*)
		*ethernetif*.o(.text*)
		*tcp*.o(.text*)
#endif

#if defined(CHIP_HAS_CP) && (RAMCP_SIZE > 0)
		*:hal_memsc.o(.text*)
		*:hal_trace.o(.text*)
		*:hal_trace_mod.o(.text*)
		*:mpu_armv*.o(.text*)
		*:codec_sbc.o(.text*)
		*:multi_heap.o(.text*)
		*:pool_api.o(.text*)
		*libgcc.a:(.text*)

		/* for LIBC_ROM=0 */
		*libc_nano.a:(.text*)
		/* for NOSTD=1  */
		*:atoi.o(.text*)
		*:ctype.o(.text*)
		*:memchr.o(.text*)
		*:memcmp.o(.text*)
		*:memmove.o(.text*)
		*:strcat.o(.text*)
		*:strchr.o(.text*)
		*:strcmp.o(.text*)
		*:strcpy.o(.text*)
		*:strlen.o(.text*)
		*:strncat.o(.text*)
		*:strncmp.o(.text*)
		*:strncpy.o(.text*)
		*:strnicmp.o(.text*)
		*:strstr.o(.text*)
		*:strtol.o(.text*)
		*:strtoul.o(.text*)
		*:vsprintf.o(.text*)
		*:vsscanf.o(.text*)

		/* for SCO_CP_ACCEL=1 */
		*:cmsis_dsp_lib.o(.text*)
		*libm.a:(.text*)

		*:a2dp_encoder_cp.o(.text*)
		*:a2dp_encoder_sbc_cp.o(.text*)
#elif defined(ARM_CMSE) && (RAMCP_SIZE == 0)
		*(.cp_text_sram*)
#endif

#ifndef LC3_IN_A7_DSP_CODEC
#if(A2DP_LC3_ON)
		*:lc3codec.o(.text*)
#endif
#endif
		*:norflash_api.o(.text*)
		*:tcp.o(.text*)
		*:tcp_in.o(.text*)
		. = ALIGN(0x2000); /*align for dubug hook*/
		*:tcp_out.o(.text*)
		/* for SCO_CP_ACCEL=1 */
		*:cmsis_dsp_lib.o(.text*)
		*libm.a:(.text*)

#ifdef KWS_IN_RAM
		*:kws_lib.o(.text*)
#endif
		*:tcp*.o(.text*)
		*:net*.o(.text*)
		*:agg*.o(.text*)
		*:michael.o(.text*)
		*:rx.o(.text*)
		*:event.o(.text*)
		*:util.o(.text*)
		*:rate.o(.text*)
		*:rc80211*.o(.text*)
		*:tx.o(.text*)
		*:status.o(.text*)
		*:hw_crc.o(.text*)
		*:queue.o(.text*)
		*:hwio.o(.text*)
		*:fwio.o(.text*)
		*:bh.o(.text*)
		*:txrx.o(.text*)
		*:skbuff.o(.text*)
		*:eth*.o(.text*)
		*:ip*.o(.text*)
		*:def.o(.text*)
		*:pbuf.o(.text*)
		*:raw.o(.text*)
		*:sockets.o(.text*)
		*:cmsis*.o(.text*)
		*:list.o(.text*)
		*:memp*.o(.text*)
		*:mcu_slave_init.o(.text*)
		*:dsp_code_decom.o(.text*)
		*:lzma_decode.o(.text*)
		*:hal_transq.o(.text*)
		*:ftp_client.o(.text*)
		*:port_s.o(.text*)
		*:k_*.o(.text*)
		*:wifi_drv*.o(.text*)
		*:dpd*.o(.text*)
		*:flash*.o(.text*)
		/* freeRTOS kernel */
		*:heap_5.o(.text*)
		*:port.o(.text*)
		*:mpu_wrappers.o(.text*)
		*:croutine.o(.text*)
		*:event_groups.o(.text*)
		*:list.o(.text*)
		*:queue.o(.text*)
		*:stream_buffer.o(.text*)
		*:tasks.o(.text*)
		*:timers.o(.text*)
		*(.sram_text*)
		/*
		   If you want protect one section by armv8's mpu, the section should
		   be aligned to 32bytes
		 */
		. = ALIGN(32);
		__sram_text_end__ = .;
	} > RAMX

	.sram_data (RAMX_TO_RAM(ADDR(.sram_text) + SIZEOF(.sram_text))) :
				AT (__sram_text_data_start_flash__ + SIZEOF(.sram_text))
	{
#ifdef RTOS_IN_RAM
		*:rtos_lib.o(.data* .rodata*)
#endif

#if defined(KERNEL_LITEOS_M)
		/*
		   Enhance wifi throughput in the liteos system.
		   include file *los_*, *driverif*, *sys_arch*, *sockets_porting*, *ethernetif*, *tcp*
		 */
		*los_*.o(.data* .rodata*)
		*driverif*.o(.data* .rodata*)
		*sys_arch*.o(.data* .rodata*)
		*sockets_porting*.o(.data* .rodata*)
		*ethernetif*.o(.data* .rodata*)
		*tcp*.o(.data* .rodata*)
#endif

#if defined(GLOBAL_SRAM_KISS_FFT)
		*:fftwrap.o(.data* .rodata*)
		*:kiss_fft.o(.data* .rodata*)
		*:kiss_fftr.o(.data* .rodata*)
#endif

#if defined(GLOBAL_SRAM_CMSIS_FFT)
		*:CommonTables.o(.rodata.armBitRevIndexTable256)
		*:CommonTables.o(.rodata.twiddleCoef_256)
		*:CommonTables.o(.rodata.twiddleCoef_rfft_512)
		*:CommonTables.o(.rodata.armBitRevIndexTable128)
		*:CommonTables.o(.rodata.twiddleCoef_128)
		*:CommonTables.o(.rodata.twiddleCoef_rfft_256)
#endif

#if defined(VOICE_ALGO_CP_ACCEL)
		*:CommonTables.o(.rodata.armBitRevIndexTable256)
		*:CommonTables.o(.rodata.twiddleCoef_256)
		*:CommonTables.o(.rodata.twiddleCoef_rfft_512)

#if defined(VOICE_TX_AEC)
		*:ssp_aec.o(.data* .rodata*)
#endif
#if defined(APP_NOISE_ESTIMATION)
		*:noise_estimator.o(.data* .rodata*)
#endif
#endif
#if defined(NEW_NV_RECORD_ENALBED)
		. = ALIGN(NV_EXTENSION_MIRROR_RAM_SIZE);
        *(.nv_extension_pool)
        . = ALIGN(NV_EXTENSION_MIRROR_RAM_SIZE);
#endif
#if defined(BTUSB_AUDIO_MODE)
		*:hal_usb.o(.rodata*)
#endif

#if defined(SCO_CP_ACCEL)
		*:plc_8000.o(.data* .rodata*)
#if defined(_CVSD_BYPASS_) || defined(CVSD_BYPASS)
		*:Pcm8k_Cvsd.o(.data* .rodata*)
		*:cvsd_codec.o(.data* .rodata*)
#endif
		*:table.o(.data* .rodata*)
		*:plc_16000.o(.data* .rodata*)
		/* Move algos from overlay to here */
#endif

		*:utils.o(.data* .rodata*)
		*:cqueue.o(.data* .rodata*)
		*:iir_process.o(.data* .rodata*)
		*:sbcplc.o(.data* .rodata*)
		*:sbc.o(.data* .rodata*)
		*:sbc_math.o(.data* .rodata*)
		*:crossover_filter.o(.data* .rodata*)
		*:exp_func_tables.o(.data* .rodata*)
#if defined(AQE_KWS)
		*:kws_nn.o(.data* .rodata.second_conv_wt)
		*:command_recognition.o(.data* .rodata*)
		*:arm_bitreversal_16.o(.data* .rodata*)
		*:arm_cfft_radix8_f32.o(.data* .rodata*)
		*:arm_bitreversal2.o(.data* .rodata*)
		*:arm_mult_f32.o(.data* .rodata*)
		*:arm_cmplx_mag_squared_f32.o(.data* .rodata*)
		*:arm_cmplx_mult_cmplx_f32.o(.data* .rodata*)
		*:arm_abs_f32.o(.data* .rodata*)
		*:arm_cos_f32.o(.data* .rodata*)
		*:arm_sin_f32.o(.data* .rodata*)
		*:nsx_core.o(.data* .rodata*)
#endif

#if defined(SPEECH_TX_NN_NS)
		*:nn_ns_large_network.o(.data* .rodata*)
		*:denoise_20ms.o(.data* .rodata*)
#endif

#if defined(SPEECH_TX_NN_NS2)
		*:nn_ns_data.o(.data* .rodata*)
#endif

#if defined(CHIP_HAS_CP) && (RAMCP_SIZE > 0)
		*:hal_memsc.o(.rodata*)
		*:hal_trace.o(.rodata*)
		*:hal_trace_mod.o(.rodata*)
		*:mpu.o(.rodata*)
		*:multi_heap.o(.rodata*)
		*:pool_api.o(.rodata*)
		*:a2dp_decoder_cp.o(.rodata*)
		*:a2dp_encoder_cp.o(.rodata*)
		*:a2dp_encoder_sbc_cp.o(.rodata*)
		*libgcc.a:(.rodata*)

		/* for LIBC_ROM=0 */
		*libc_nano.a:(.rodata*)
		/* for NOSTD=1  */
		*:atoi.o(.rodata*)
		*:ctype.o(.rodata*)
		*:memchr.o(.rodata*)
		*:memcmp.o(.rodata*)
		*:memmove.o(.rodata*)
		*:strcat.o(.rodata*)
		*:strchr.o(.rodata*)
		*:strcmp.o(.rodata*)
		*:strcpy.o(.rodata*)
		*:strlen.o(.rodata*)
		*:strncat.o(.rodata*)
		*:strncmp.o(.rodata*)
		*:strncpy.o(.rodata*)
		*:strnicmp.o(.rodata*)
		*:strstr.o(.rodata*)
		*:strtol.o(.rodata*)
		*:strtoul.o(.rodata*)
		*:vsprintf.o(.rodata*)
		*:vsscanf.o(.rodata*)

		/* for SCO_CP_ACCEL=1 */
		*:cmsis_dsp_lib.o(.rodata*)
		*libm.a:(.rodata*)
#endif

#if defined(GSOUND_HOTWORD_EXTERNAL)
		*:voice_sbc.o(.data* .rodata*)
		*:gsound_custom_audio.o(.data* .rodata*)
		*:gsound_custom_hotword_external.o(.data* .rodata*)
		*:libspeech*.o(.data* .rodata*)
#endif

#ifndef LC3_IN_A7_DSP_CODEC
#if(A2DP_LC3_ON)
		*:lc3codec.o(.data* .rodata*)
#endif
#endif

		*:norflash_api.o(.rodata*)
#if defined(A2DP_SCALABLE_ON)
		*:util.o(.data* .rodata*)
#endif
		*:tcp*.o(.data*)
		*:net*.o(.data*)
		*:agg*.o(.data*)
		*:rx.o(.data*)
		*:event.o(.data*)
		*:util.o(.data*)
		*:rate.o(.data*)
		*:rc80211*.o(.data*)
		*:tx.o(.data*)
		*:status.o(.data*)
		*:hw_crc.o(.data*)
		*:queue.o(.data*)
		*:hwio.o(.data*)
		*:fwio.o(.data*)
		*:bh.o(.data*)
		*:txrx.o(.data*)
		*:skbuff.o(.data*)
		*:eth*.o(.data*)
		*:ip*.o(.data*)
		*:def.o(.data*)
		*:pbuf.o(.data*)
		*:raw.o(.data*)
		*:sockets.o(.data*)
		*:apps.o(.data*)
		*:cmsis*.o(.data*)
		*:list.o(.data*)
		*:k_*.o(.data*)
		*:mcu_slave_init.o(.data*)
		*:dsp_code_decom.o(.data*)
		*:lzma_decode.o(.data*)
		*:hal_transq.o(.data*)
		*:ftp_client.o(.data*)
		*(.sram_data*)
#if (RAMCP_SIZE == 0)
		*(.cp_data*)
#endif
#if RTT_APP_SUPPORT > 0
    /* section information for finsh shell */
        . = ALIGN(4);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;

        . = ALIGN(4);
        __vsymtab_start = .;
        KEEP(*(VSymTab))
        __vsymtab_end = .;

        /* section information for initial. */
        . = ALIGN(4);
        __rt_init_start = .;
        KEEP(*(SORT(.rti_fn*)))
        __rt_init_end = .;
#endif
#ifdef ARM_CMSE
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

		. = ALIGN(4);
		__cust_cmd_hldr_tbl_start = .;
		KEEP(*(.cust_cmd_hldr_tbl))
		__cust_cmd_hldr_tbl_end = .;
		. = ALIGN(4);
		__ai_gatt_server_table_start = .;
		KEEP(*(.ai_gatt_server_table))
		__ai_gatt_server_table_end = .;
		. = ALIGN(4);
		__ai_ble_handler_table_start = .;
		KEEP(*(.ai_ble_handler_table))
		__ai_ble_handler_table_end = .;
		. = ALIGN(4);
		__ai_spp_register_table_start = .;
		KEEP(*(.ai_spp_register_table))
		__ai_spp_register_table_end = .;
		. = ALIGN(4);
		__tota_handler_table_start = .;
		KEEP(*(.tota_handler_table))
		__tota_handler_table_end = .;
		. = ALIGN(4);
		__ai_handler_function_table_start = .;
		KEEP(*(.ai_handler_function_table))
		__ai_handler_function_table_end = .;
		. = ALIGN(4);
		__custom_handler_table_start = .;
		KEEP(*(.custom_handler_table))
		__custom_handler_table_end = .;
		. = ALIGN(4);
		__mixprompt_property_table_start = .;
		KEEP(*(.mixprompt_property_table))
		__mixprompt_property_table_end = .;
		. = ALIGN(4);
		__core_bridge_task_cmd_table_start = .;
		KEEP(*(.core_bridge_task_cmd_table))
		__core_bridge_task_cmd_table_end = .;
		. = ALIGN(4);
		__core_bridge_instant_cmd_table_start = .;
		KEEP(*(.core_bridge_instant_cmd_table))
		__core_bridge_instant_cmd_table_end = .;
		. = ALIGN(4);
		__app_bt_sync_command_handler_table_start = .;
		KEEP(*(.app_bt_sync_command_handler_table))
		__app_bt_sync_command_handler_table_end = .;
		. = ALIGN(4);
		KEEP(*(.eh_frame*))
		*(.note.gnu.build-id)
#endif
		. = ALIGN(4);
	} > RAM

	__sram_text_data_end_flash__ = __sram_text_data_start_flash__ +
								SIZEOF(.sram_text) + SIZEOF(.sram_data);

	.sram_bss (.) (NOLOAD) : AT (ADDR(.sram_bss))
	{
		__sram_bss_start__ = .;
		*:utils.o(.bss*)
#if defined(_CVSD_BYPASS_) || defined(CVSD_BYPASS)
		*:Pcm8k_Cvsd.o(.bss*)
		*:cvsd_codec.o(.bss*)
#endif

#if defined(VOICE_TX_AEC)
		*:ssp_aec.o(.bss*)
#endif

#if defined(APP_NOISE_ESTIMATION)
		*:noise_estimator.o(.bss*)
#endif
		*:tcp*.o(.bss*)
		*:net*.o(.bss*)
		*:agg*.o(.bss*)
		*:rx.o(.bss*)
		*:event.o(.bss*)
		*:util.o(.bss*)
		*:rate.o(.bss*)
		*:rc80211*.o(.bss*)
		*:tx.o(.bss*)
		*:status.o(.bss*)
		*:hw_crc.o(.bss*)
		*:queue.o(.bss*)
		*:hwio.o(.bss*)
		*:fwio.o(.bss*)
		*:bh.o(.bss*)
		*:txrx.o(.bss*)
		*:skbuff.o(.bss*)
		*:eth*.o(.bss*)
		*:ip*.o(.bss*)
		*:def.o(.bss*)
		*:pbuf.o(.bss*)
		*:raw.o(.bss*)
		*:sockets.o(.bss*)
		*:apps.o(.bss*)
		*:cmsis*.o(.bss*)
		*:list.o(.bss*)
		*:memp*.o(.bss*)
		*:k_*.o(.bss*)
		*:mcu_slave_init.o(.bss*)
		*:dsp_code_decom.o(.bss*)
		*:lzma_decode.o(.bss*)
		*:ftp_client.o(.bss*)
		*:hal_transq.o(.bss*)
		*:hal_mcu2cp.o(.bss*)
		*:wifi_agent.o(.bss*)
#ifndef WIFI_ON_CP
		*:wifi_srv.o(.bss*)
#endif
		*:ethernetif.o(.bss*)

		*(.sram_bss*)
#if (RAMCP_SIZE == 0)
		*(.cp_bss*)
#endif
		. = ALIGN(32);
		__sram_bss_end__ = .;
#ifdef OPENAMP_SUPPORT
		. = ALIGN(64);
		__share_mem_pa_start__ = .;
		. += SHARED_MEM_SIZE;
		__share_mem_pa_end__ = .;
#endif
	} > RAM


#if defined(ARM_CMSE) && (RAM_NSC_SIZE != 0)
	__sram_nsc_text_start_flash__ = __sram_text_data_end_flash__;
	.gnu.sgstubs (ORIGIN(SRAM_NSC)) : AT (__sram_nsc_text_start_flash__)
	{
		__sram_nsc_text_start__ = .;
		. = ALIGN(4);
	} > RAMX_NSC
	__sram_nsc_text_end_flash__ = __sram_nsc_text_start_flash__ + SIZEOF(.gnu.sgstubs);
	__fast_sram_text_data_start_flash__ = __sram_nsc_text_end_flash__;
#else
	__fast_sram_text_data_start_flash__ = __sram_text_data_end_flash__;
#endif

	.fast_text_sram (ORIGIN(FRAMX)) : AT (__fast_sram_text_data_start_flash__)
	{
		__fast_sram_text_exec_start__ = .;
#if defined(GLOBAL_SRAM_KISS_FFT)
		*:fftwrap.o(.text*)
		*:kiss_fft.o(.text*)
		*:kiss_fftr.o(.text*)
#endif

#if defined(GLOBAL_SRAM_CMSIS_FFT)
		*:cmsis_fftwrap.o(.text*)
		*:arm_rfft_fast_f32.o(.text*)
		*:arm_cfft_f32.o(.text*)
#endif

#if defined(VOICE_ALGO_CP_ACCEL)
		*:cmsis_fftwrap.o(.text*)
		*:arm_rfft_fast_f32.o(.text*)
		*:arm_cfft_f32.o(.text*.arm_cfft_f32)
		*:arm_bitreversal2.o(.text*.arm_bitreversal_32)
		*:arm_cfft_radix8_f32.o(.text*.arm_radix8_butterfly_f32)

		*:voice_algo.o(.text*.voice_algo_process_impl)
		*:voice_algo.o(.text*.algo_process_impl)
#if defined(VOICE_TX_AEC)
		*:voice_tx_aec.o(.text*.voice_tx_aec_process)
		*:ssp_aec.o(.text*)
#endif
#if defined(APP_NOISE_ESTIMATION)
		*:voice_noise_est.o(.text*.voice_noise_est_process)
		*:noise_estimator.o(.text*)
#endif
#endif

#if defined(SCO_CP_ACCEL)
		*:frame_resize.o(.text*)
		*:buffer_manager.o(.text*)
		*:plc_8000.o(.text*)
#if defined(_CVSD_BYPASS_) || defined(CVSD_BYPASS)
		*:Pcm8k_Cvsd.o(.text*)
		*:cvsd_codec.o(.text*)
#endif
		*:dspfns.o(.text*)
		*:lpc_plc.o(.text*)
		*:plc_16000.o(.text*)
		*:sbc_filter.o(.text*)
/* Move algos from overlay to here */

#endif

#if defined(ROM_UTILS_ON)
		*:rom_utils.o(.text*)
#endif

#if defined(BTUSB_AUDIO_MODE)
		*:hal_dma.o(.text*)
		*:hal_uart.o(.text*)
		*:hal_trace.o(.text*)
#endif

#if defined(BTUSB_AUDIO_MODE)
		*:hal_usb.o(.text*)

		*:usb_audio.o(.text*)
		*:usb_audio_sync.o(.text*)
		*:usb_audio_cdc.o(.text*)

		*:usbaudio_entry.o(.text*)
		*:usb_audio_app.o(.text*)
#endif

		/* memmove.o or libc_nano.a(lib_a-memmove.o) */
		*:memmove.o(.text*)
		*:lib_a-memmove.o(.text*)
		*:utils.o(.text*)
		*:cqueue.o(.text*)
		*:audio_resample_ex.o(.text*)
#if defined(MIX_MIC_DURING_MUSIC)
		*:app_bt_stream.o(.text*)
#endif
#if defined(__SBC_FUNC_IN_ROM__) || (defined(CHIP_HAS_CP) && (RAMCP_SIZE > 0))
		*:sbc.o(.text*)
#else
		*:sbc.o(.text*.sbc_is_stream_info_valid)
		*:sbc.o(.text*.sbc_frame_length_get)
		*:sbc.o(.text*.sbc_4bit_crc_calc)
		*:sbc.o(.text*.sbc_bits_for_mono_alloc)
		*:sbc.o(.text*.sbc_bits_for_stereo_alloc)
		*:sbc.o(.text*.sbc_header_process)
		*:sbc.o(.text*.sbc_factors_process)
		*:sbc.o(.text*.sbc_sample_extract)
		*:sbc.o(.text*.sbc_sbsamples_make)
		*:sbc.o(.text*.sbc_joint_process)
		*:sbc.o(.text*.sbc_4band_synth_filter)
		*:sbc.o(.text*.sbc_8band_synth_filter)
		*:sbc.o(.text*.sbc_decoder_state_reset)
		*:sbc.o(.text*.sbc_frame_mute)
		*:sbc.o(.text*.sbc_frames_decode)
		*:sbc.o(.text*.sbc_frames_decode_do)
		*:sbc.o(.text*.sbc_frames_decode_select_channel)
		*:sbc.o(.text*.sbc_frames_decode)
		*:sbc.o(.text*.sbc_samples_quant)
		*:sbc.o(.text*.sbc_decoder_init)
		*:sbc.o(.text*.sbc_encoder_init)
		*:sbc.o(.text*.sbc_frames_encode)
#endif
		*:sbc_math.o(.text*)
#if defined(A2DP_AAC_ON)
		*:fixpoint_math.o(.text*)
		*:FDK_bitbuffer.o(.text*)
		*:FDK_tools_rom.o(.text*FDKgetWindowSlope*)
		*:FDK_tools_rom.o(.text*getBitstreamElementList*)
		*:dct.o(.text*dct_IV*)
#if !defined(__SBC_FUNC_IN_ROM_VBEST2000__)
		*:fft.o(.text*._Z3fftiPlPi)
		*:fft_rad2.o(.text*dit_fft*)
		*:dct.o(.text*getTables*)
#endif
#if defined(OPUS_IN_OVERLAY)
		*:voice_opus.o(.text*)
#elif defined(VOICE_DATAPATH) || defined(__ALEXA_WWE) || defined(AQE_KWS_ALEXA)
		*:opus_encoder.o(.text*)
#elif defined(__AI_VOICE__) && (RAM_SIZE >= 0x48000)
		*:voice_opus.o(.text*)
		/**:opus_encoder.o(.text*)*/
		*:opus_encoder.o(.text*.frame_size_select)
		*:opus_encoder.o(.text*.opus_encode_native)
		*:opus_encoder.o(.text*.user_bitrate_to_bitrate)
		*:opus_encoder.o(.text*.compute_equiv_rate)
		*:opus_encoder.o(.text*.decide_fec)
		*:opus_encoder.o(.text*.dc_reject)
		*:opus_encoder.o(.text*.gen_toc)
		*:opus_encoder.o(.text*.opus_encode)
		*:opus_encoder.o(.text*.opus_encoder_create)
		*:opus_encoder.o(.text*.opus_encoder_get_size)
		*:opus_encoder.o(.text*.opus_encoder_init)
		*:opus_encoder.o(.text*.opus_encoder_ctl)
		*:analysis.o(.text*)
		/**:celt_encoder.o(.text*)*/
		*:celt_encoder.o(.text*.opus_custom_encoder_ctl)
		*:celt_encoder.o(.text*.celt_encoder_get_size)
		*:celt_encoder.o(.text*.opus_custom_encoder_get_size)
		*:celt_encoder.o(.text*.celt_encoder_init)
		*:celt_encoder.o(.text*.opus_custom_encoder_init_arch)
		/**:repacketizer.o(.text*)*/
		*:repacketizer.o(.text*.opus_packet_pad)
		*:repacketizer.o(.text*.opus_repacketizer_init)
		*:repacketizer.o(.text*.opus_repacketizer_cat)
		*:repacketizer.o(.text*.opus_repacketizer_cat_impl)
		*:repacketizer.o(.text*.opus_repacketizer_out_range_impl)
		*:opus.o(.text*)
		*:enc_API.o(.text*)
		/**:entenc.o(.text*)*/
		*:entenc.o(.text*.ec_enc_init)
		*:entenc.o(.text*.ec_enc_icdf)
		*:entenc.o(.text*.ec_enc_normalize)
		*:entenc.o(.text*.ec_enc_carry_out)
		*:entenc.o(.text*.ec_write_byte)
		*:entenc.o(.text*.ec_enc_patch_initial_bits)
		*:entenc.o(.text*.ec_enc_done)
		*:control_audio_bandwidth.o(.text*)
		*:encode_pulses.o(.text*)
		*:LPC_analysis_filter.o(.text*)
		*:burg_modified_FIX.o(.text*.silk_burg_modified_c)
		*:vector_ops_FIX.o(.text*.silk_scale_copy_vector16)
		*:vector_ops_FIX.o(.text*.silk_inner_prod_aligned)
		*:vector_ops_FIX.o(.text*.silk_inner_prod16_aligned_64_c)
		*:NLSF_del_dec_quant.o(.text*.silk_NLSF_del_dec_quant)
		*:encode_frame_FIX.o(.text*.silk_encode_frame_FIX)
		*:encode_frame_FIX.o(.text*.silk_encode_do_VAD_FIX)
		*:encode_frame_FIX.o(.text*.silk_LBRR_encode_FIX)
		/**:pitch.o(.text*)*/
		*:pitch.o(.text*.celt_pitch_xcorr_c)
		*:resampler.o(.text*)
		*:VAD.o(.text*)
		*:encode_indices.o(.text*)
		/**:control_codec.o(.text*)*/
		*:control_codec.o(.text*.silk_control_encoder)
		*:control_codec.o(.text*.silk_setup_resamplers)
		*:control_codec.o(.text*.silk_setup_fs)
		*:control_codec.o(.text*.silk_setup_complexity)
		*:control_codec.o(.text*.silk_setup_LBRR)
		/**:celt_lpc.o(.text*)*/
		*:celt_lpc.o(.text*._celt_autocorr)
		/**:shell_coder.o(.text*)*/
		*:shell_coder.o(.text*.combine_pulses)
		*:shell_coder.o(.text*.silk_shell_encoder)
		*:shell_coder.o(.text*.encode_split)
		*:init_encoder.o(.text*)
		*:log2lin.o(.text*)
		*:biquad_alt.o(.text*)
		*:stereo_encode_pred.o(.text*)
		*:NLSF_unpack.o(.text*)
		*:code_signs.o(.text*)
		*:lin2log.o(.text*)
		*:HP_variable_cutoff.o(.text*)
		*:sum_sqr_shift.o(.text*)
		*:inner_prod_aligned.o(.text*)
		*:stereo_quant_pred.o(.text*)
		*:entcode.o(.text*)
		*:celt.o(.text*)
		*:laplace.o(.text*)
		/**:NSQ.o(.text*)*/ /*complexity 1*/
		/**:NSQ_del_dec.o(.text*)*/ /*complexity 2*/
#endif
#endif

#if defined(AI_CAPTURE_DATA_AEC)
		*:coherence.o(.text*)
		*:echo_canceller.o(.text*)
		*:echo_suppression.o(.text*)
		*:lcmmse.o(.text*)
		*:logmmse.o(.text*)
		*:mdf2.o(.text*)
		*:noise_generator.o(.text*)
		*:pink_noise_gen.o(.text*)
		*:white_noise_gen.o(.text*)
#endif

#if defined(__AUDIO_SPECTRUM__)
		*:spectrum_fix.o(.text*)
		*:audio_spectrum.o(.text*)
#endif
#if defined(A2DP_SCALABLE_ON) && defined(A2DP_AAC_ON)
		*:mdct.o(.text*)
#endif
		*:crossover_filter.o(.text*)
		*:data_compressor.o(.text*)
		*:drc.o(.text*)
		*:limiter.o(.text*)
#if defined(AQE_KWS)
		*:arm_*.o(.text*)
		*:aqe_mfcc.o(.text*)
		*:kws_nn.o(.text*)
		*:command_recognition.o(.text*)
		*:arm_radix8_butterfly_f32.o(.text*)
		*:arm_bitreversal_32.o(.text*)
		*:arm_bitreversal_16.o(.text*)
		*:dct.o(.text*dct_IV*)
		*:fft.o(.text*._Z3fftiPlPi)
		*:fft_rad2.o(.text*dit_fft*)
		*:dct.o(.text*getTables*)
		*:arm_cfft_radix8_f32.o(.text*)
		*:arm_bitreversal2.o(.text*)
		*:arm_mult_f32.o(.text*)
		*:arm_cmplx_mag_squared_f32.o(.text*)
		*:arm_cmplx_mult_cmplx_f32.o(.text*)
		*:lib_a-wf_sqrt.o(.text*)
		*:arm_abs_f32.o(.text*)
		*:arm_cos_f32.o(.text*)
		*:arm_sin_f32.o(.text*)

		*:coherence.o(.text*)
		*:echo_canceller.o(.text*)
		*:echo_suppression.o(.text*)
		*:lcmmse.o(.text*)
		*:logmmse.o(.text*)
		*:mdf2.o(.text*)
		*:noise_generator.o(.text*)
		*:pink_noise_gen.o(.text*)
		*:white_noise_gen.o(.text*)

		*:ae_common.o(.text*)
		*:ae_rand.o(.text*)
		*:fastmath.o(.text*)
		*:integer_delay.o(.text*)
		*:iirfilt.o(.text*)
		*:ringbuf.o(.text*)
		*:scale_int16.o(.text*)
		*:speech_conv.o(.text*)
		*:speech_trans_buf.o(.text*)

		*:ns3.o(.text*)
		*:noise_suppression_x.o(.text*)
		*:nsx_core.o(.text*)
		*:nsx_core_c.o(.text*)
		*:copy_set_operations.o(.text*)
		*:cross_correlation.o(.text*)
		*:division_operations.o(.text*)
		*:dot_product_with_scale.o(.text*)
		*:downsample_fast.o(.text*)
		*:energy.o(.text*)
		*:get_scaling_square.o(.text*)
		*:min_max_operations.o(.text*)
		*:real_fft.o(.text*)
		*:resample_by_2.o(.text*)
		*:resample_by_2_internal.o(.text*)
		*:spl_init.o(.text*)
		*:spl_sqrt.o(.text*)
		*:spl_sqrt_floor.o(.text*)
		*:vector_scaling_operations.o(.text*)
#endif
#if defined(SPEECH_TX_NN_NS2)
	*:nn_ns_pitch.o(.text*)
	*:nn_ns_celt_lpc.o(.text*)
	*:nn_ns_model.o(.text*)
	*:nn_ns2.o(.text*)
#endif



#if !defined(SCO_OPTIMIZE_FOR_RAM)
		:audio_prompt_sbc.o(.text*)
#endif

#if defined(A2DP_SCALABLE_ON)
		*:util.o(.text*)
#endif
		*(.fast_text_sram*)
#if (RAMCP_SIZE == 0)
		*(.cp_text_sram*)
#endif
		/*
		   If you want protect one section by armv8's mpu, the section should
		   be aligned to 32bytes
		 */
		. = ALIGN(32);
		__fast_sram_text_exec_end__ = .;
	} > FRAMX

	__fast_sram_text_data_end_flash__ = __fast_sram_text_data_start_flash__ + SIZEOF(.fast_text_sram);
	__fast_sram_text_data_start__ = RAMX_TO_RAM(__fast_sram_text_exec_start__);
	__fast_sram_text_data_end__ = RAMX_TO_RAM(__fast_sram_text_exec_end__);

#if defined(CHIP_HAS_CP) && (RAMCP_SIZE > 0)
	__cp_text_sram_start_flash__ = __fast_sram_text_data_end_flash__;

	.cp_text_sram (ORIGIN(RAMCPX)) : AT (__cp_text_sram_start_flash__)
	{
		__cp_text_sram_exec_start__ = .;
		*(.cp_text_sram*)
		*:cp_queue.o(.text*)
#if defined(GSOUND_HOTWORD_EXTERNAL)
		//*:libspeech*.o(.text*)
#endif

#ifdef MP3_DECODE_IN_CP
		/* mp3 */
		*:mini_mp3.o(.text*)
#endif

#ifdef AAC_DECODE_IN_CP
		/* aac */
		*:aacdecoder_lib.o(.text*)
		*:tpdec_lib.o(.text*)
		*:tpdec_adts.o(.text*)
		*:tpdec_asc.o(.text*)
		*:aacdecoder.o(.text*)
		*:channel.o(.text*)
		*:channelinfo.o(.text*)
		*:aac_ram.o(.text*)
		*:aacdec_pns.o(.text*)
		*:aacdec_tns.o(.text*)
		*:stereo.o(.text*)
		*:fixpoint_math.o(.text*)
		*:FDK_bitbuffer.o(.text*)
		*:block.o(.text*)
		*:pulsedata.o(.text*)
		*:mdct.o(.text*)
		*:genericStds.o(.text*)
#endif

#ifdef OPUS_DECODE_IN_CP
		/* opus */
		*:opus_decoder.o(.text*)
		*:bands.o(.text*)
		*:celt_decoder.o(.text*)
		*:entenc.o(.text*)
		*:entdec.o(.text*)
		*:vq.o(.text*)
		*:decode_core.o(.text*)
		*:opus.o(.text*)
		*:dec_API.o(.text*)
		*:init_decoder.o(.text*)
		*:CNG.o(.text*)
		*:PLC.o(.text*)
		*:decoder_set_fs.o(.text*)
		*:decode_frame.o(.text*)
		*:decode_indices.o(.text*)
		*:decode_pulses.o(.text*)
		*:decode_parameters.o(.text*)
		*:decode_pitch.o(.text*)
		*:control_audio_bandwidth.o(.text*)
		*:gain_quant.o(.text*)
		*:NLSF_decode.o(.text*)
		*:NLSF_stabilize.o(.text*)
		*:NLSF2A.o(.text*)
		*:stereo_MS_to_LR.o(.text*)
		*:LPC_fit.o(.text*)
		*:bwexpander_32.o(.text*)
		*:LPC_inv_pred_gain.o(.text*)
		*:resampler_private_IIR_FIR.o(.text*)
		*:resampler_private_AR2.o(.text*)
		*:stereo_decode_pred.o(.text*)
		*:resampler_private_down_FIR.o(.text*)
		*:resampler_private_up2_HQ.o(.text*)
		*:bwexpander.o(.text*)
		*:quant_bands.o(.text*)
		*:opus_kiss_fft.o(.text*)
		*:rate.o(.text*.compute_allocation)
		*:cwrs.o(.text*)
		*:mathops.o(.text*)
		*:LPC_analysis_filter.o(.text*)
		*:burg_modified_FIX.o(.text*)
		*:vector_ops_FIX.o(.text*)
		*:NLSF_del_dec_quant.o(.text*)
		*:encode_frame_FIX.o(.text*)
		*:pitch.o(.text*)
		*:resampler.o(.text*)
		*:VAD.o(.text*)
		*:encode_indices.o(.text*)
		*:control_codec.o(.text*)
		*:celt_lpc.o(.text*)
		*:shell_coder.o(.text*)
		*:init_encoder.o(.text*)
		*:log2lin.o(.text*)
		*:biquad_alt.o(.text*)
		*:stereo_encode_pred.o(.text*)
		*:NLSF_unpack.o(.text*)
		*:code_signs.o(.text*)
		*:lin2log.o(.text*)
		*:HP_variable_cutoff.o(.text*)
		*:sum_sqr_shift.o(.text*)
		*:inner_prod_aligned.o(.text*)
		*:stereo_quant_pred.o(.text*)
		*:entcode.o(.text*)
		*:celt.o(.text*)
		*:laplace.o(.text*)
		*:NSQ.o(.text*) /*complexity 1*/
		*:NSQ_del_dec.o(.text*) /*complexity 2*/
#endif

#ifdef OPUS_ENCODE_IN_CP
		/* /opus/src/src */
		*:analysis.o(.text*)
		*:mlp.o(.text*)
		*:mlp_data.o(.text*)
		*:opus.o(.text*)
		/*:opus_decoder.o(.text*) */
		*:opus_encoder.o(.text*)
		*:opus_memory.o(.text*)
		*:opus_multistream.o(.text*)
		/*:opus_multistream_decoder.o(.text*) */
		*:opus_multistream_encoder.o(.text*)
		*:repacketizer.o(.text*)
		/* /opus/src/silk */
		*:A2NLSF.o(.text*)
		*:ana_filt_bank_1.o(.text*)
		*:biquad_alt.o(.text*)
		*:bwexpander_32.o(.text*)
		*:bwexpander.o(.text*)
		*:check_control_input.o(.text*)
		*:CNG.o(.text*)
		*:code_signs.o(.text*)
		*:control_audio_bandwidth.o(.text*)
		*:control_codec.o(.text*)
		*:control_SNR.o(.text*)
		*:debug.o(.text*)
		/*:dec_API.o(.text*) */
		/*:decode_core.o(.text*) */
		/*:decode_frame.o(.text*) */
		/*:decode_indices.o(.text*) */
		/*:decode_parameters.o(.text*) */
		/*:decode_pitch.o(.text*) */
		*:decode_pulses.o(.text*)
		/*:decoder_set_fs.o(.text*) */
		*:enc_API.o(.text*)
		*:encode_indices.o(.text*)
		*:encode_pulses.o(.text*)
		*:gain_quant.o(.text*)
		*:HP_variable_cutoff.o(.text*)
		/*:init_decoder.o(.text*) */
		*:init_encoder.o(.text*)
		*:inner_prod_aligned.o(.text*)
		*:interpolate.o(.text*)
		*:lin2log.o(.text*)
		*:log2lin.o(.text*)
		*:LPC_analysis_filter.o(.text*)
		*:LPC_fit.o(.text*)
		*:LPC_inv_pred_gain.o(.text*)
		*:LP_variable_cutoff.o(.text*)
		*:NLSF2A.o(.text*)
		*:NLSF_decode.o(.text*)
		*:NLSF_del_dec_quant.o(.text*)
		*:NLSF_encode.o(.text*)
		*:NLSF_stabilize.o(.text*)
		*:NLSF_unpack.o(.text*)
		*:NLSF_VQ.o(.text*)
		*:NLSF_VQ_weights_laroia.o(.text*)
		*:NSQ.o(.text*)
		*:NSQ_del_dec.o(.text*)
		*:pitch_est_tables.o(.text*)
		*:PLC.o(.text*)
		*:process_NLSFs.o(.text*)
		*:quant_LTP_gains.o(.text*)
		*:resampler.o(.text*)
		*:resampler_down2_3.o(.text*)
		*:resampler_down2.o(.text*)
		*:resampler_private_AR2.o(.text*)
		*:resampler_private_down_FIR.o(.text*)
		*:resampler_private_IIR_FIR.o(.text*)
		*:resampler_private_up2_HQ.o(.text*)
		*:resampler_rom.o(.text*)
		*:shell_coder.o(.text*)
		*:sigm_Q15.o(.text*)
		*:sort.o(.text*)
		/*:stereo_decode_pred.o(.text*) */
		*:stereo_encode_pred.o(.text*)
		*:stereo_find_predictor.o(.text*)
		*:stereo_LR_to_MS.o(.text*)
		*:stereo_MS_to_LR.o(.text*)
		*:stereo_quant_pred.o(.text*)
		*:sum_sqr_shift.o(.text*)
		*:table_LSF_cos.o(.text*)
		*:tables_gain.o(.text*)
		*:tables_LTP.o(.text*)
		*:tables_NLSF_CB_NB_MB.o(.text*)
		*:tables_NLSF_CB_WB.o(.text*)
		*:tables_other.o(.text*)
		*:tables_pitch_lag.o(.text*)
		*:tables_pulses_per_block.o(.text*)
		*:VAD.o(.text*)
		*:VQ_WMat_EC.o(.text*)
		/* /opus/src/silk/fixed */
		*:apply_sine_window_FIX.o(.text*)
		*:autocorr_FIX.o(.text*)
		*:burg_modified_FIX.o(.text*)
		*:corrMatrix_FIX.o(.text*)
		*:encode_frame_FIX.o(.text*)
		*:find_LPC_FIX.o(.text*)
		*:find_LTP_FIX.o(.text*)
		*:find_pitch_lags_FIX.o(.text*)
		*:find_pred_coefs_FIX.o(.text*)
		*:k2a_FIX.o(.text*)
		*:k2a_Q16_FIX.o(.text*)
		*:LTP_analysis_filter_FIX.o(.text*)
		*:LTP_scale_ctrl_FIX.o(.text*)
		*:noise_shape_analysis_FIX.o(.text*)
		*:pitch_analysis_core_FIX.o(.text*)
		*:process_gains_FIX.o(.text*)
		*:regularize_correlations_FIX.o(.text*)
		*:residual_energy16_FIX.o(.text*)
		*:residual_energy_FIX.o(.text*)
		*:schur64_FIX.o(.text*)
		*:schur_FIX.o(.text*)
		*:vector_ops_FIX.o(.text*)
		*:warped_autocorrelation_FIX.o(.text*)
		/* /opus/src/celt */
		*:bands.o(.text*)
		*:celt.o(.text*)
		/*:celt_decoder.o(.text*) */
		*:celt_encoder.o(.text*)
		*:celt_lpc.o(.text*)
		*:cwrs.o(.text*)
		*:entcode.o(.text*)
		*:entdec.o(.text*)
		*:entenc.o(.text*)
		*:laplace.o(.text*)
		*:mathops.o(.text*)
		*:mdct.o(.text*)
		*:modes.o(.text*)
		*:opus_kiss_fft.o(.text*)
		*:pitch.o(.text*)
		*:quant_bands.o(.text*)
		*:rate.o(.text*.compute_allocation)
		*:vq.o(.text*)
#endif

#ifdef VOIP_ALG_ALI_TEST_IN_CP
		/* alg voip */
		*:audio_alg.o(.text*)
#endif

#ifdef AUDIO_PROCESS_IN_CP
		/* eq */
		*:iir_process.o(.text*)
		/* drc */
		*:drc.o(*text)
		*:data_compressor.o(*text)
		/* limiter */
		*:limiter.o(.text*)
		*:floatlimiter.o(.text*)
#endif

#ifdef ALSA_RESAMPLE_IN_CP
		*:audio_resample_ex.o(.text*)
		*:resample_coef.o(.text*)
#endif
		. = ALIGN(4);
		__cp_text_sram_exec_end__ = .;
	} > RAMCPX

	__cp_text_sram_start = RAMX_TO_RAM(__cp_text_sram_exec_start__);
	__cp_text_sram_end = RAMX_TO_RAM(__cp_text_sram_exec_end__);

	.vector_table_cp (ORIGIN(RAMCP)) (NOLOAD) :
	{
		KEEP(*(.vector_table_cp))
		. = VECTOR_SECTION_SIZE;
		. = ALIGN(4);
	} > RAMCP

	__cp_data_sram_start_flash__ = __cp_text_sram_start_flash__ + SIZEOF(.cp_text_sram);

	.cp_data_sram (.) : AT (__cp_data_sram_start_flash__)
	{
		__cp_data_sram_start = .;
		*(.cp_data*)
		*:cp_queue.o(.data* .rodata*)
#if defined(GSOUND_HOTWORD_EXTERNAL)
		//*:libspeech*.o(.data* .rodata*)
#endif
		. = ALIGN(4);

#ifdef MP3_DECODE_IN_CP
		/* mp3 */
		*:mini_mp3.o(.data* .rodata*)
#endif

#ifdef AAC_DECODE_IN_CP
		/* aac */
		*:block.o(.data* .rodata*)
		*:tpdec_lib.o(.data* .rodata*)
		*:tpdec_latm.o(.data* .rodata*)
		*:aacdec_pns.o(.data* .rodata*)
		*:aacdec_tns.o(.data* .rodata*)
		*:pulsedata.o(.data* .rodata*)
		*:channel.o(.data* .rodata*)
		*:aacdecoder.o(.data* .rodata*)
		*:channelinfo.o(.data* .rodata*)
		*:aacdecoder_lib.o(.data* .rodata*)
		*:stereo.o(.data* .rodata*)
		*:fixpoint_math.o(.data* .rodata*)
		*:FDK_bitbuffer.o(.data* .rodata*)
		*:tpdec_adts.o(.data* .rodata*)
		*:aac_rom.o(.data* .rodata*)
		*:FDK_tools_rom.o(.data* .rodata*)
#endif

#ifdef OPUS_DECODE_IN_CP
		/* opus */
		*:resampler.o(.data* .rodata*)
		*:tables_other.o(.data* .rodata*)
		*:tables_gain.o(.data* .rodata*)
		*:tables_LTP.o(.data* .rodata*)
		*:tables_NLSF_CB_NB_MB.o(.data* .rodata*)
		*:NLSF2A.o(.data* .rodata*)
		*:table_LSF_cos.o(.data* .rodata*)
		*:tables_pulses_per_block.o(.data* .rodata*)
		*:PLC.o(.data* .rodata*)
		*:resampler_rom.o(.data* .rodata*)
		*:pitch_est_tables.o(.data* .rodata*)
		*:tables_pitch_lag.o(.data* .rodata*)
		*:celt_decoder.o(.data* .rodata*)
		*:bands.o(.data* .rodata*)
		*:vq.o(.data* .rodata*)
		*:cwrs.o(.data* .rodata*)
		*:entcode.o(.data* .rodata*)
		*:rate.o(.rodata.LOG2_FRAC_TABLE)
		*:modes.o(.data* .rodata*)
		*:quant_bands.o(.data* .rodata*)
		*:opus_decoder.o(.data* .rodata*)
		*:dec_API.o(.data* .rodata*)
		*:tables_NLSF_CB_WB.o(.data* .rodata*)
		*:celt.o(.data* .rodata*)
#endif

#ifdef OPUS_ENCODE_IN_CP
		/* /opus/src/src */
		*:analysis.o(.data* .rodata*)
		*:mlp.o(.data* .rodata*)
		*:mlp_data.o(.data* .rodata*)
		*:opus.o(.data* .rodata*)
		/*:opus_decoder.o(.data* .rodata*) */
		*:opus_encoder.o(.data* .rodata*)
		*:opus_memory.o(.data* .rodata*)
		*:opus_multistream.o(.data* .rodata*)
		/*:opus_multistream_decoder.o(.data* .rodata*) */
		*:opus_multistream_encoder.o(.data* .rodata*)
		*:repacketizer.o(.data* .rodata*)
		/* /opus/src/silk */
		*:A2NLSF.o(.data* .rodata*)
		*:ana_filt_bank_1.o(.data* .rodata*)
		*:biquad_alt.o(.data* .rodata*)
		*:bwexpander_32.o(.data* .rodata*)
		*:bwexpander.o(.data* .rodata*)
		*:check_control_input.o(.data* .rodata*)
		*:CNG.o(.data* .rodata*)
		*:code_signs.o(.data* .rodata*)
		*:control_audio_bandwidth.o(.data* .rodata*)
		*:control_codec.o(.data* .rodata*)
		*:control_SNR.o(.data* .rodata*)
		*:debug.o(.data* .rodata*)
		/*:dec_API.o(.data* .rodata*) */
		/*:decode_core.o(.data* .rodata*) */
		/*:decode_frame.o(.data* .rodata*) */
		/*:decode_indices.o(.data* .rodata*) */
		/*:decode_parameters.o(.data* .rodata*) */
		/*:decode_pitch.o(.data* .rodata*) */
		*:decode_pulses.o(.data* .rodata*)
		/*:decoder_set_fs.o(.data* .rodata*) */
		*:enc_API.o(.data* .rodata*)
		*:encode_indices.o(.data* .rodata*)
		*:encode_pulses.o(.data* .rodata*)
		*:gain_quant.o(.data* .rodata*)
		*:HP_variable_cutoff.o(.data* .rodata*)
		/*:init_decoder.o(.data* .rodata*) */
		*:init_encoder.o(.data* .rodata*)
		*:inner_prod_aligned.o(.data* .rodata*)
		*:interpolate.o(.data* .rodata*)
		*:lin2log.o(.data* .rodata*)
		*:log2lin.o(.data* .rodata*)
		*:LPC_analysis_filter.o(.data* .rodata*)
		*:LPC_fit.o(.data* .rodata*)
		*:LPC_inv_pred_gain.o(.data* .rodata*)
		*:LP_variable_cutoff.o(.data* .rodata*)
		*:NLSF2A.o(.data* .rodata*)
		*:NLSF_decode.o(.data* .rodata*)
		*:NLSF_del_dec_quant.o(.data* .rodata*)
		*:NLSF_encode.o(.data* .rodata*)
		*:NLSF_stabilize.o(.data* .rodata*)
		*:NLSF_unpack.o(.data* .rodata*)
		*:NLSF_VQ.o(.data* .rodata*)
		*:NLSF_VQ_weights_laroia.o(.data* .rodata*)
		*:NSQ.o(.data* .rodata*)
		*:NSQ_del_dec.o(.data* .rodata*)
		*:pitch_est_tables.o(.data* .rodata*)
		*:PLC.o(.data* .rodata*)
		*:process_NLSFs.o(.data* .rodata*)
		*:quant_LTP_gains.o(.data* .rodata*)
		*:resampler.o(.data* .rodata*)
		*:resampler_down2_3.o(.data* .rodata*)
		*:resampler_down2.o(.data* .rodata*)
		*:resampler_private_AR2.o(.data* .rodata*)
		*:resampler_private_down_FIR.o(.data* .rodata*)
		*:resampler_private_IIR_FIR.o(.data* .rodata*)
		*:resampler_private_up2_HQ.o(.data* .rodata*)
		*:resampler_rom.o(.data* .rodata*)
		*:shell_coder.o(.data* .rodata*)
		*:sigm_Q15.o(.data* .rodata*)
		*:sort.o(.data* .rodata*)
		/*:stereo_decode_pred.o(.data* .rodata*) */
		*:stereo_encode_pred.o(.data* .rodata*)
		*:stereo_find_predictor.o(.data* .rodata*)
		*:stereo_LR_to_MS.o(.data* .rodata*)
		*:stereo_MS_to_LR.o(.data* .rodata*)
		*:stereo_quant_pred.o(.data* .rodata*)
		*:sum_sqr_shift.o(.data* .rodata*)
		*:table_LSF_cos.o(.data* .rodata*)
		*:tables_gain.o(.data* .rodata*)
		*:tables_LTP.o(.data* .rodata*)
		*:tables_NLSF_CB_NB_MB.o(.data* .rodata*)
		*:tables_NLSF_CB_WB.o(.data* .rodata*)
		*:tables_other.o(.data* .rodata*)
		*:tables_pitch_lag.o(.data* .rodata*)
		*:tables_pulses_per_block.o(.data* .rodata*)
		*:VAD.o(.data* .rodata*)
		*:VQ_WMat_EC.o(.data* .rodata*)
		/* /opus/src/silk/fixed */
		*:apply_sine_window_FIX.o(.data* .rodata*)
		*:autocorr_FIX.o(.data* .rodata*)
		*:burg_modified_FIX.o(.data* .rodata*)
		*:corrMatrix_FIX.o(.data* .rodata*)
		*:encode_frame_FIX.o(.data* .rodata*)
		*:find_LPC_FIX.o(.data* .rodata*)
		*:find_LTP_FIX.o(.data* .rodata*)
		*:find_pitch_lags_FIX.o(.data* .rodata*)
		*:find_pred_coefs_FIX.o(.data* .rodata*)
		*:k2a_FIX.o(.data* .rodata*)
		*:k2a_Q16_FIX.o(.data* .rodata*)
		*:LTP_analysis_filter_FIX.o(.data* .rodata*)
		*:LTP_scale_ctrl_FIX.o(.data* .rodata*)
		*:noise_shape_analysis_FIX.o(.data* .rodata*)
		*:pitch_analysis_core_FIX.o(.data* .rodata*)
		*:process_gains_FIX.o(.data* .rodata*)
		*:regularize_correlations_FIX.o(.data* .rodata*)
		*:residual_energy16_FIX.o(.data* .rodata*)
		*:residual_energy_FIX.o(.data* .rodata*)
		*:schur64_FIX.o(.data* .rodata*)
		*:schur_FIX.o(.data* .rodata*)
		*:vector_ops_FIX.o(.data* .rodata*)
		*:warped_autocorrelation_FIX.o(.data* .rodata*)
		/* /opus/src/celt */
		*:bands.o(.data* .rodata*)
		*:celt.o(.data* .rodata*)
		/*:celt_decoder.o(.data* .rodata*) */
		*:celt_encoder.o(.data* .rodata*)
		*:celt_lpc.o(.data* .rodata*)
		*:cwrs.o(.data* .rodata*)
		*:entcode.o(.data* .rodata*)
		*:entdec.o(.data* .rodata*)
		*:entenc.o(.data* .rodata*)
		*:laplace.o(.data* .rodata*)
		*:mathops.o(.data* .rodata*)
		*:mdct.o(.data* .rodata*)
		*:modes.o(.data* .rodata*)
		*:opus_kiss_fft.o(.data* .rodata*)
		*:pitch.o(.data* .rodata*)
		*:quant_bands.o(.data* .rodata*)
		*:rate.o(.rodata.LOG2_FRAC_TABLE)
		*:vq.o(.data* .rodata*)
#endif

#ifdef VOIP_ALG_ALI_TEST_IN_CP
		/* alg voip */
		*:audio_alg.o(.data* .rodata*)
#endif

#ifdef AUDIO_PROCESS_IN_CP
		/* drc */
		*:drc.o(.data* .rodata*)
		*:data_compressor.o(.data* .rodata*)
		*:floatlimiter.o(.data* .rodata*)
#endif

#ifdef ALSA_RESAMPLE_IN_CP
		*:audio_resample_ex.o(.data* .rodata*)
		*:resample_coef.o(.data* .rodata*)
#endif
		. = ALIGN(4);
		__cp_data_sram_end = .;
	} > RAMCP

	__cp_sram_end_flash__ = __cp_data_sram_start_flash__ + SIZEOF(.cp_data_sram);

	.cp_bss_sram (.) (NOLOAD) : AT (ADDR(.cp_bss_sram))
	{
		__cp_bss_sram_start = .;
		*(.cp_bss*)
		. = ALIGN(4);

#ifdef MP3_DECODE_IN_CP
		/* mp3 */
		*:mini_mp3.o(.bss*)
#endif

#ifdef AAC_DECODE_IN_CP
		/* aac */
		*:aacdecoder.o(.bss*)
#endif

#ifdef OPUS_DECODE_IN_CP
		/* opus */
		*:celt.o(.bss*)
#endif

#ifdef OPUS_ENCODE_IN_CP
		/* /opus/src/src */
		*:analysis.o(.bss*)
		*:mlp.o(.bss*)
		*:mlp_data.o(.bss*)
		*:opus.o(.bss*)
		/*:opus_decoder.o(.bss*) */
		*:opus_encoder.o(.bss*)
		*:opus_memory.o(.bss*)
		*:opus_multistream.o(.bss*)
		/*:opus_multistream_decoder.o(.bss*) */
		*:opus_multistream_encoder.o(.bss*)
		*:repacketizer.o(.bss*)
		/* /opus/src/silk */
		*:A2NLSF.o(.bss*)
		*:ana_filt_bank_1.o(.bss*)
		*:biquad_alt.o(.bss*)
		*:bwexpander_32.o(.bss*)
		*:bwexpander.o(.bss*)
		*:check_control_input.o(.bss*)
		*:CNG.o(.bss*)
		*:code_signs.o(.bss*)
		*:control_audio_bandwidth.o(.bss*)
		*:control_codec.o(.bss*)
		*:control_SNR.o(.bss*)
		*:debug.o(.bss*)
		/*:dec_API.o(.bss*) */
		/*:decode_core.o(.bss*) */
		/*:decode_frame.o(.bss*) */
		/*:decode_indices.o(.bss*) */
		/*:decode_parameters.o(.bss*) */
		/*:decode_pitch.o(.bss*) */
		*:decode_pulses.o(.bss*)
		/*:decoder_set_fs.o(.bss*) */
		*:enc_API.o(.bss*)
		*:encode_indices.o(.bss*)
		*:encode_pulses.o(.bss*)
		*:gain_quant.o(.bss*)
		*:HP_variable_cutoff.o(.bss*)
		/*:init_decoder.o(.bss*) */
		*:init_encoder.o(.bss*)
		*:inner_prod_aligned.o(.bss*)
		*:interpolate.o(.bss*)
		*:lin2log.o(.bss*)
		*:log2lin.o(.bss*)
		*:LPC_analysis_filter.o(.bss*)
		*:LPC_fit.o(.bss*)
		*:LPC_inv_pred_gain.o(.bss*)
		*:LP_variable_cutoff.o(.bss*)
		*:NLSF2A.o(.bss*)
		*:NLSF_decode.o(.bss*)
		*:NLSF_del_dec_quant.o(.bss*)
		*:NLSF_encode.o(.bss*)
		*:NLSF_stabilize.o(.bss*)
		*:NLSF_unpack.o(.bss*)
		*:NLSF_VQ.o(.bss*)
		*:NLSF_VQ_weights_laroia.o(.bss*)
		*:NSQ.o(.bss*)
		*:NSQ_del_dec.o(.bss*)
		*:pitch_est_tables.o(.bss*)
		*:PLC.o(.bss*)
		*:process_NLSFs.o(.bss*)
		*:quant_LTP_gains.o(.bss*)
		*:resampler.o(.bss*)
		*:resampler_down2_3.o(.bss*)
		*:resampler_down2.o(.bss*)
		*:resampler_private_AR2.o(.bss*)
		*:resampler_private_down_FIR.o(.bss*)
		*:resampler_private_IIR_FIR.o(.bss*)
		*:resampler_private_up2_HQ.o(.bss*)
		*:resampler_rom.o(.bss*)
		*:shell_coder.o(.bss*)
		*:sigm_Q15.o(.bss*)
		*:sort.o(.bss*)
		/*:stereo_decode_pred.o(.bss*) */
		*:stereo_encode_pred.o(.bss*)
		*:stereo_find_predictor.o(.bss*)
		*:stereo_LR_to_MS.o(.bss*)
		*:stereo_MS_to_LR.o(.bss*)
		*:stereo_quant_pred.o(.bss*)
		*:sum_sqr_shift.o(.bss*)
		*:table_LSF_cos.o(.bss*)
		*:tables_gain.o(.bss*)
		*:tables_LTP.o(.bss*)
		*:tables_NLSF_CB_NB_MB.o(.bss*)
		*:tables_NLSF_CB_WB.o(.bss*)
		*:tables_other.o(.bss*)
		*:tables_pitch_lag.o(.bss*)
		*:tables_pulses_per_block.o(.bss*)
		*:VAD.o(.bss*)
		*:VQ_WMat_EC.o(.bss*)
		/* /opus/src/silk/fixed */
		*:apply_sine_window_FIX.o(.bss*)
		*:autocorr_FIX.o(.bss*)
		*:burg_modified_FIX.o(.bss*)
		*:corrMatrix_FIX.o(.bss*)
		*:encode_frame_FIX.o(.bss*)
		*:find_LPC_FIX.o(.bss*)
		*:find_LTP_FIX.o(.bss*)
		*:find_pitch_lags_FIX.o(.bss*)
		*:find_pred_coefs_FIX.o(.bss*)
		*:k2a_FIX.o(.bss*)
		*:k2a_Q16_FIX.o(.bss*)
		*:LTP_analysis_filter_FIX.o(.bss*)
		*:LTP_scale_ctrl_FIX.o(.bss*)
		*:noise_shape_analysis_FIX.o(.bss*)
		*:pitch_analysis_core_FIX.o(.bss*)
		*:process_gains_FIX.o(.bss*)
		*:regularize_correlations_FIX.o(.bss*)
		*:residual_energy16_FIX.o(.bss*)
		*:residual_energy_FIX.o(.bss*)
		*:schur64_FIX.o(.bss*)
		*:schur_FIX.o(.bss*)
		*:vector_ops_FIX.o(.bss*)
		*:warped_autocorrelation_FIX.o(.bss*)
		/* /opus/src/celt */
		*:bands.o(.bss*)
		*:celt.o(.bss*)
		/*:celt_decoder.o(.bss*) */
		*:celt_encoder.o(.bss*)
		*:celt_lpc.o(.bss*)
		*:cwrs.o(.bss*)
		*:entcode.o(.bss*)
		*:entdec.o(.bss*)
		*:entenc.o(.bss*)
		*:laplace.o(.bss*)
		*:mathops.o(.bss*)
		*:mdct.o(.bss*)
		*:modes.o(.bss*)
		*:opus_kiss_fft.o(.bss*)
		*:pitch.o(.bss*)
		*:quant_bands.o(.bss*)
		/*:rate.o(.bss*)*/
		*:vq.o(.bss*)
#endif

#ifdef VOIP_ALG_ALI_TEST_IN_CP
		/* alg voip */
		*:audio_alg.o(.bss*)
#endif

#ifdef AUDIO_PROCESS_IN_CP
		*:data_compressor.o(.bss*)
		*:floatlimiter.o(.bss*)
#endif

#ifdef ALSA_RESAMPLE_IN_CP
		*:audio_resample_ex.o(.bss*)
		*:resample_coef.o(.bss*)
#endif
		. = ALIGN(4);
		__cp_bss_sram_end = .;
	} > RAMCP

	__overlay_text_start_flash__ = __cp_sram_end_flash__;
#else
	__overlay_text_start_flash__ = __fast_sram_text_data_end_flash__;
#endif

#ifdef OVERLAY_FOR_CP
	__overlay_text_start__ = __cp_text_sram_end;
#else
	__overlay_text_start__ = __fast_sram_text_data_end__;
#endif
	__overlay_text_exec_start__ = RAM_TO_RAMX(__overlay_text_start__);

	OVERLAY (__overlay_text_exec_start__) : NOCROSSREFS AT (__overlay_text_start_flash__)
	{
#ifndef NO_OVERLAY
		.overlay_text0
		{
			LONG(0);
#if !defined(GLOBAL_SRAM_KISS_FFT)
			*:kiss_fft.o(.text*)
			*:kiss_fftr.o(.text*)
			*:fftwrap.o(.text*)
#endif

#if !defined(GLOBAL_SRAM_CMSIS_FFT)
			*:cmsis_fftwrap.o(.text*)
			*:arm_rfft_fast_f32.o(.text*)
			*:arm_cfft_f32.o(.text*)
#endif

#if defined(SCO_OPTIMIZE_FOR_RAM)
			*:arm_biquad_cascade_df1_f32.o(.text*)
			*:integer_delay.o(.text*)
			*:lc_mmse_ns_float.o(.text*.speech_ns2float_process)
			*:coherence.o(.text*)
			*:echo_suppression.o(.text*.float_cmp)
			*:echo_suppression.o(.text*.es_process)
			*:lcmmse.o(.text*.lcmmse_noise_estimate)
			*:lcmmse.o(.text*.lcmmse_get_speech_prob)
			*:logmmse.o(.text*.filterbank_compute_bank_pflt)
			*:logmmse.o(.text*.filterbank_compute_psd_pflt)
			*:logmmse.o(.text*.logmmse_analysis)
			*:logmmse.o(.text*.logmmse_process)
#if defined(CHIP_BEST1402) || defined(CHIP_BEST1400)
			*:lpc_plc.o(.text*.WB_PLC_common)
			*:lpc_plc.o(.text*.WB_PLC_erasure)
			*:lpc_plc.o(.text*.apfilterQ1_Q0)
			*:lpc_plc.o(.text*.apfilterQ0_Q0)
			*:lpc_plc.o(.text*.azfilterQ0_Q1)
			*:lpc_plc.o(.text*.decim)
			*:lpc_plc.o(.text*.merit)
#endif
#else
			/* common */
#if !defined(AUDIO_ANC_FB_ADJ_MC)
			*:ae_common.o(.text*)
			*:ae_rand.o(.text*)
			*:fastmath.o(.text*)
			*:integer_delay.o(.text*)
			*:iirfilt.o(.text*)
			*:ringbuf.o(.text*)
#endif
			*:scale_int16.o(.text*)
			*:speech_conv.o(.text*)
			*:speech_trans_buf.o(.text*)

#if !defined(SCO_CP_ACCEL)
			*:frame_resize.o(.text*)
			*:buffer_manager.o(.text*)
			*:plc_8000.o(.text*)
#if defined(_CVSD_BYPASS_) || defined(CVSD_BYPASS)
			*:Pcm8k_Cvsd.o(.text*)
			*:cvsd_codec.o(.text*)
#endif
			*:dspfns.o(.text*)
			*:lpc_plc.o(.text*)
			*:plc_16000.o(.text*)
			*:sbc_filter.o(.text*)
#endif
			*:preprocess.o(.text*)
			*:filterbank.o(.text*)
			*:mdf.o(.text*)
#if defined(SPEECH_TX_2MIC_NS)
			*:webrtc_vad.o(.text*.WebRtcVad_Process)
			*:webrtc_vad.o(.text*.WebRtcVad_ValidRateAndFrameLength)
			*:webrtc_vad.o(.text*.WebRtcVad_CalcVad16khz)
			*:webrtc_vad.o(.text*.WebRtcVad_Downsampling)
			*:webrtc_vad.o(.text*.WebRtcVad_CalcVad8khz)
			*:vad_filterbank.o(.text*)
			*:energy.o(.text*)
			*:get_scaling_square.o(.text*)
			*:dual_mic_denoise.o(.text*)
			*:cross_correlation.o(.text*)
			*:min_max_operations.o(.text*)
			*:resample_by_2_internal.o(.text*)
			*:division_operations.o(.text*)
			*:downsample_fast.o(.text*)
			*:resample_fractional.o(.text*)
			*:resample_48khz.o(.text*)
			*:vad_sp.o(.text*)
			*:vad_core.o(.text*)
			*:webrtc_vad.o(.text*)
			*:vad_gmm.o(.text*)
#endif
#if defined(SPEECH_TX_2MIC_NS2)
			*:speech_2mic_ns2_denoise.o(.text*)
			*:speech_2mic_ns2_denoise_float.o(.text*)
#endif
#if defined(SPEECH_TX_2MIC_NS4)
			*:sensormic_denoise.o(.text*)
#endif
#if defined(SPEECH_TX_2MIC_NS5)
            *:leftright_denoise.o(.text*)
#endif
#if defined(SPEECH_TX_NS2) || defined(SPEECH_RX_NS2)
			*:lc_mmse_ns.o(.text*)
#endif
#if defined(SPEECH_TX_NS2FLOAT) || defined(SPEECH_RX_NS2FLOAT)
			*:lc_mmse_ns_float.o(.text*)
#endif
#if defined(SPEECH_TX_NS3) || defined(SPEECH_RX_NS3)
			*:ns3.o(.text*)
			*:noise_suppression_x.o(.text*)
			*:nsx_core.o(.text*)
			*:nsx_core_c.o(.text*)
			*:copy_set_operations.o(.text*)
			*:cross_correlation.o(.text*)
			*:division_operations.o(.text*)
			*:dot_product_with_scale.o(.text*)
			*:downsample_fast.o(.text*)
			*:energy.o(.text*)
			*:get_scaling_square.o(.text*)
			*:min_max_operations.o(.text*)
			*:real_fft.o(.text*)
			*:resample_by_2.o(.text*)
			*:resample_by_2_internal.o(.text*)
			*:spl_init.o(.text*)
			*:spl_sqrt.o(.text*)
			*:spl_sqrt_floor.o(.text*)
			*:vector_scaling_operations.o(.text*)
#endif
#if defined(SPEECH_TX_AEC2FLOAT) && !defined(AUDIO_ANC_FB_ADJ_MC)
			*:coherence.o(.text*)
			*:echo_canceller.o(.text*)
			*:echo_suppression.o(.text*)
			*:lcmmse.o(.text*)
			*:logmmse.o(.text*)
			*:mdf2.o(.text*)
			*:noise_generator.o(.text*)
			*:pink_noise_gen.o(.text*)
			*:white_noise_gen.o(.text*)
#endif
#if defined(SPEECH_TX_AEC3)
			*:SubBandBasedAEC.o(.text*)
#endif
#if defined(SPEECH_TX_3MIC_NS)
			*:speech_3mic_ns.o(.text*)
#endif
#if defined(SPEECH_CODEC_FIXED_SAMPLE_RATE)
			*:iir_resample.o(.text*)
#endif
#if defined(SPEECH_TX_COMPEXP)
			*:compexp.o(.text*)
#endif
#if defined(SPEECH_TX_EQ) || defined(SPEECH_RX_EQ) || defined(SPEECH_CODEC_FIXED_SAMPLE_RATE)
			*:speech_eq.o(.text*)
			*:speech_arm_eq.o(.text*)
			*:arm_biquad_cascade_df1_f32.o(.text*)
			*:arm_biquad_cascade_df1_ex_f32.o(.text*)
#endif
#if defined(SPEECH_TX_POST_GAIN) || defined(SPEECH_RX_POST_GAIN)
			*:speech_gain.o(.text*)
#endif
#if defined(__SMARTVOICE__)
#if defined(SPEECH_TX_2MIC_NS2) || defined(SPEECH_TX_NS2)
			*:app_smartvoice.o(.text*._Z23mic_data_process_deinitv)
			*:app_smartvoice.o(.text*._Z21mic_data_process_initv)
			*:app_smartvoice.o(.text*._Z20mic_data_process_runPsm)
#endif
#endif

#endif // #if !defined(SCO_OPTIMIZE_FOR_RAM)
			*(.overlay_text0)
			. = ALIGN(4);
		}
		.overlay_text1
		{
			LONG(0);
#if !defined(RB_CODEC)
			*:fixedpoint.o(.text*)
			*:replaygain.o(.text*)
			*:dsp_core.o(.text*)
			*:dsp_misc.o(.text*)
			*:dsp_filter.o(.text*)
			*:dsp_sample_input.o(.text*)
			*:dsp_sample_output.o(.text*)
			*:eq.o(.text*)
			*:pga.o(.text*)
#if !defined(A2DP_AAC_ON) && !defined(A2DP_LHDC_ON)
			*:fir_process.o(.text*)
			*:iir_process.o(.text*.iir_run .text*.*iir_run_per_*)
#endif
#endif
#if defined(CHIP_HAS_CP) && (RAMCP_SIZE > 0)
#endif
			*(.overlay_a2dp_sbc*)
			*(.overlay_text1)
			. = ALIGN(4);
		}
		.overlay_text2
		{
			LONG(0);
			*:fmdec.o(.text* .data* .rodata* .bss*)
			*(.overlay_text2)
			. = ALIGN(4);
		}
		.overlay_text3
		{
			LONG(0);
			*:fft128dot.o(.text*)
			*(.overlay_text3)
			. = ALIGN(4);
		}
		.overlay_text4
		{
			LONG(0);
#ifdef A2DP_SOURCE_AAC_ON
            *:adj_thr.o(.text.*FDKaacEnc_AdjThrClose*)
            *:bit_cnt.o(.text*)
            *:quantize.o(.text*FDKaacEnc_quantizeLines*)
            *:adj_thr.o(.text*CalcRedValPower*)
            *:tpenc_lib.o(.text.*transportEnc_GetStaticBits*)
            *:tpenc_latm.o(.text.*transportEnc_LatmCountTotalBitDemandHeader*)
            *:tpenc_lib.o(.text.*transportEnc_GetBitstream*)
            *:line_pe.o(.text.*FDKaacEnc_calcSfbPe*)
            *:adj_thr.o(.text.*FDKaacEnc_calcPe*)
            *:spreading.o(.text.*FDKaacEnc_SpreadingMax*)
            *:bitenc.o(.text.*FDKaacEnc_writeExtensionData*)
            *:bitenc.o(.text.*FDKaacEnc_ChannelElementWrite*)
            *:qc_main.o(.text.*FDKaacEnc_getTotalConsumedBits*)
            *:adj_thr.o(.text.*FDKaacEnc_reduceThresholdsCBR*)
            *:band_nrg.o(.text.*FDKaacEnc_CalcSfbMaxScaleSpec*)
            *:aacenc_tns.o(.text.*FDKaacEnc_AutoToParcor*)
            *:aacenc_tns.o(.text.*FDKaacEnc_Parcor2Index*)
            *:aacenc_tns.o(.text.*FDKaacEnc_TnsDetect*)
            *:aacenc_tns.o(.text.*FDKaacEnc_TnsEncode*)
            *:pre_echo_control.o(.text.*FDKaacEnc_PreEchoControl*)
            *:transform.o(.text.*FDKaacEnc_Transform_Real*)
            *:band_nrg.o(.text.*FDKaacEnc_CheckBandEnergyOptim*)
            *:tpenc_latm.o(.text.*AdvanceAudioMuxElement*)
            *:aacenc.o(.text.*FDKaacEnc_EncodeFrame*)
            *:aacenc_pns.o(.text.*FDKaacEnc_PnsDetect*)
#else
#if defined(A2DP_AAC_ON)
			*:block.o(.text*.*CBlock_GetEscape*)
			*:tpdec_lib.o(.text*.*transportDec_GetBitstream*)
			*:tpdec_lib.o(.text*.*transportDec_GetAuBitsRemaining*)
			*:tpdec_latm.o(.text*.*CLatmDemux_GetValue*)
			*:tpdec_latm.o(.text*.*CLatmDemux_Read*)
			*:tpdec_lib.o(.text*.*transportDec_FillData*)
			*:tpdec_lib.o(.text*.*transportDec_ReadAccessUnit*)
			*:tpdec_lib.o(.text*.*transportDec_readHeader*)
			*:block.o(.text*.*ApplyTools*)
			*:aacdec_pns.o(.text*.*CPns_Apply*)
			*:aacdec_tns.o(.text*.*CTns_Apply*)
			*:aacdec_tns.o(.text*.*CTns_Reset*)
			*:aacdec_pns.o(.text*.*CPns_ResetData*)
			*:pulsedata.o(.text*.*CPulseData_Read*)
#if !defined(AAC_TEXT_PARTIAL_IN_FLASH)
			*:channel.o(.text*.*CChannelElement_Read*)
			*:tpdec_asc.o(.text*.*CProgramConfig_Reset*)
			*:tpdec_lib.o(.text*.*transportDec_CrcCheck*)
			*:block.o(.text*.*CBlock_FrequencyToTime*)
			*:block.o(.text*.*CBlock_ReadSectionData*)
			*:channel.o(.text*.*CChannelElement_Decode*)
			*:tpdec_lib.o(.text*.*transportDec_CrcEndReg*)
			*:aacdecoder.o(.text*.*CAacDecoder_DecodeFrame*)
			*:aac_mdct.o(.text*)
#endif

#if !defined(__SBC_FUNC_IN_ROM_VBEST2000__) && !defined(AAC_TEXT_PARTIAL_IN_FLASH)
			*:block.o(.text*.*CBlock_ReadSpectralData*)
			*:block.o(.text*.*CBlock_ScaleSpectralData*)
			*:aacdec_tns.o(.text*.*CTns_ReadDataPresentFlag*)
			*:tpdec_lib.o(.text*.*transportDec_CrcStartReg*)
			*:block.o(.text*.*CBlock_ReadScaleFactorData*)
			*:tpdec_lib.o(.text*.*transportDec_EndAccessUnit*)
			*:tpdec_latm.o(.text*.*CLatmDemux_GetNrOfSubFrames*)
			*:tpdec_lib.o(.text*.*transportDec_GetAuBitsTotal*)
			*:tpdec_asc.o(.text*.*CProgramConfig_LookupElement*)
			*:tpdec_latm.o(.text*.*CLatmDemux_GetFrameLengthInBits*)
			*:tpdec_latm.o(.text*.*CLatmDemux_ReadPayloadLengthInfo*)
			*:block.o(.text*.*CBlock_InverseQuantizeSpectralData*)
			*:channelinfo.o(.text*.*IcsRead*)
			*:aacdec_tns.o(.text*.*CTns_Read*)
			*:tpdec_lib.o(.text*.*transportDec_AdjustEndOfAccessUnit*)
			*:pulsedata.o(.text*.*CPulseData_Apply*)
			*:tpdec_asc.o(.text*.*AudioSpecificConfig_Init*)
			*:tpdec_asc.o(.text*.*AudioSpecificConfig_Parse*)
			*:tpdec_latm.o(.text*.*CLatmDemux_ReadStreamMuxConfig*)
			*:tpdec_asc.o(.text*.*getSampleRate*)
			*:tpdec_asc.o(.text*.*getAOT*)
			*:aacdecoder.o(.text*.*CAacDecoder_Init*)
			*:tpdec_asc.o(.text*.*CProgramConfig_Init*)
			*:aacdecoder_lib.o(.text*.*aacDecoder_ConfigCallback*)
			*:aacdecoder.o(.text*.*CAacDecoder_ExtPayloadParse*)
			*:stereo.o(.text*.*CJointStereo_Read*)
#endif
#endif
#if defined(CHIP_HAS_CP) && (RAMCP_SIZE > 0)
			*:a2dp_decoder_aac_lc.o(.text*)
			*:fdkaac_codec.o(.text*)
#endif
			*(.overlay_a2dp_aac*)
#endif
			*(.overlay_text4)
			. = ALIGN(4);
		}
		.overlay_text5
		{
			LONG(0);
			*:mpa.o(.text*)
			*:bit.o(.text*)
			*:frame.o(.text*)
			*:huffman.o(.text*)
			*:layer12.o(.text*)
			*:layer3.o(.text*)
			*:stream.o(.text*)
			*:synth.o(.text*)
			*(.overlay_text5)
			. = ALIGN(4);
		}
		.overlay_text6
		{
			LONG(0);
#if defined(A2DP_SCALABLE_ON)
			*:rc_decode.o(.text*)
			*:audio_int32.o(.text*)
			*:ssc_ifft.o(.text*)
			*:vector_quant.o(.text*)
			*:ssc_quant.o(.text*)
			*:ssc_pulsealloc.o(.text*)
			*:audio.o(.text*)
			*:set_sbm.o(.text*)
			*:rc_code.o(.text*)
			*:SBM_pitchBased.o(.text*)
			*:ssc_imdct.o(.text*)
			*:audio_decoder.o(.text*)
			*:SoundBooster_DRC_classic.o(.text*)
			*:ssc_cwrs.o(.text*)
			*:ssc_mode.o(.text*)
			*:ssc_math.o(.text*)
			*:ssc_lap.o(.text*)
			*:SeamlessBufferManagement.o(.text*)
#if defined(CHIP_HAS_CP) && (RAMCP_SIZE > 0)
			*:a2dp_decoder_scalable.o(.text*)
#endif
			*(.overlay_a2dp_ssc*)
#endif

#if defined(A2DP_LHDC_ON)
			*(.overlay_a2dp_lhdc*)
			*:a2dp_decoder_lhdc.o(.text*)
			*:testkey.o(.text*)
			*:lhdcUtil.o(.text*)
			*:lzh_dec.o(.text*)
			*:licdecrypt.o(.text*)
			*:lic_check.o(.text*)
			*:fast_upsampling_2x.o(.text*)
			*:encdec.o(.text*)
			*:dec.o(.text*)
			*:cirbuf.o(.text*)
			*:DecodeUps2xLPF.o(.text*)
			*:Arc.o(.text*)
#endif
#if defined(A2DP_LDAC_ON)
			*:a2dp_decoder_ldac.o(.text*)
			*:bitalloc_ldac.o(.text*)
			*:bitalloc_sub_ldac.o(.text*)
			*:encode_ldac.o(.text*)
			*:ldacBT.o(.text*)
			*:ldaclib.o(.text*)
			*:pack_ldac.o(.text*)
			*:quant_ldac.o(.text*)
			*:mdct_ldac.o(.text*)
			*:ldacBT_api.o(.text*)
			*:decode_ldac.o(.text*)
			*:dequant_ldac.o(.text*)
			*:imdct_ldac.o(.text*)
			*:ldacBT_internal.o(.text*)
			*:ldaclib_api.o(.text*)
			*:memory_ldac.o(.text*)
			*:setpcm_ldac.o(.text*)
			*:sigana_ldac.o(.text*)
			*:tables_ldac.o(.text*)
			*:tables_sigproc_ldac.o(.text*)
			*:unpack_ldac.o(.text*)
			*(.overlay_a2dp_ldac*)
#endif

			*(.overlay_text6)
			. = ALIGN(4);
		}
		.overlay_text7
		{
			LONG(0);
#if defined(OPUS_IN_OVERLAY)
			/**:voice_opus.o(.text*)*/
			*:opus_encoder.o(.text*)
			*:analysis.o(.text*)
			/**:celt_encoder.o(.text*)*/
			*:celt_encoder.o(.text*.opus_custom_encoder_ctl)
			*:celt_encoder.o(.text*.celt_encoder_get_size)
			*:celt_encoder.o(.text*.opus_custom_encoder_get_size)
			*:celt_encoder.o(.text*.celt_encoder_init)
			*:celt_encoder.o(.text*.opus_custom_encoder_init_arch)
			*:celt_encoder.o(.text*.transient_analysis)
			*:celt_encoder.o(.text*.patch_transient_decision)
			*:celt_encoder.o(.text*.celt_preemphasis)
			*:celt_encoder.o(.text*.l1_metric)
			*:celt_encoder.o(.text*.tf_analysis)
			*:celt_encoder.o(.text*.tf_encode)
			*:celt_encoder.o(.text*.alloc_trim_analysis)
			*:celt_encoder.o(.text*.stereo_analysis)
			*:celt_encoder.o(.text*.median_of_5)
			*:celt_encoder.o(.text*.median_of_3)
			*:celt_encoder.o(.text*.dynalloc_analysis)
			*:celt_encoder.o(.text*.run_prefilter)
			*:celt_encoder.o(.text*.compute_vbr)
			*:celt_encoder.o(.text*.celt_encode_with_ec)
			*:celt_encoder.o(.text*.opus_custom_encoder_ctl)
			*:repacketizer.o(.text*)
			*:opus.o(.text*)
			*:enc_API.o(.text*)
			*:entenc.o(.text*)
			*:control_audio_bandwidth.o(.text*)
			*:encode_pulses.o(.text*)
			*:LPC_analysis_filter.o(.text*)
			*:burg_modified_FIX.o(.text*)
			*:vector_ops_FIX.o(.text*)
			*:NLSF_del_dec_quant.o(.text*)
			*:encode_frame_FIX.o(.text*)
			*:pitch.o(.text*)
			*:resampler.o(.text*)
			*:VAD.o(.text*)
			*:encode_indices.o(.text*)
			*:control_codec.o(.text*)
			*:celt_lpc.o(.text*)
			*:shell_coder.o(.text*)
			*:init_encoder.o(.text*)
			*:log2lin.o(.text*)
			*:biquad_alt.o(.text*)
			*:stereo_encode_pred.o(.text*)
			*:NLSF_unpack.o(.text*)
			*:code_signs.o(.text*)
			*:lin2log.o(.text*)
			*:HP_variable_cutoff.o(.text*)
			*:sum_sqr_shift.o(.text*)
			*:inner_prod_aligned.o(.text*)
			*:stereo_quant_pred.o(.text*)
			*:entcode.o(.text*)
			*:celt.o(.text*)
			*:laplace.o(.text*)
			*:NSQ.o(.text*) /*complexity 1*/
			*:NSQ_del_dec.o(.text*) /*complexity 2*/
#endif
			*(.overlay_text7)
			. = ALIGN(4);
		}
#endif
		.overlay_text_last
		{
#ifndef NO_OVERLAY
			LONG(0);
#endif
			. = ALIGN(4);
		}
	} > REGION_OVERLAY_TEXT

	.dummy_overlay_text_end (.) (NOLOAD) : AT (ADDR(.dummy_overlay_text_end))
	{
		__overlay_text_exec_end__ = .;
	} > REGION_OVERLAY_TEXT

#ifdef OVERLAY_FOR_CP
	__fast_sram_end__ = __fast_sram_text_data_end__;
#else
	__fast_sram_end__ = RAMX_TO_RAM(__overlay_text_exec_end__);
#endif

	ASSERT(__fast_sram_end__ - __fast_sram_text_data_start__ <= FAST_XRAM_SECTION_SIZE, "fast xram sections too large")
	__free_fram = FAST_XRAM_SECTION_SIZE - (__fast_sram_end__ - __fast_sram_text_data_start__);

#ifdef OVERLAY_FOR_CP
	__overlay_data_start__ = __cp_bss_sram_end;
#else
	__overlay_data_start__ = __sram_bss_end__;
#endif

	OVERLAY (__overlay_data_start__) : NOCROSSREFS AT (__load_stop_overlay_text_last)
	{
#ifndef NO_OVERLAY
		.overlay_data0
		{
			/* Explicitly place 4 bytes at section start to avoid "section type changed to PROGBITS" warnings */
			LONG(0);
#if defined(CHIP_HAS_CP) && (RAMCP_SIZE > 0)
			*:bt_sco_chain.o(.data* .rodata*)
			*:bt_sco_chain_cp.o(.data* .rodata*)
#endif
#if !defined(SCO_OPTIMIZE_FOR_RAM)
#if !defined(GLOBAL_SRAM_KISS_FFT)
			*:fftwrap.o(.data* .rodata*)
			*:kiss_fft.o(.data* .rodata*)
			*:kiss_fftr.o(.data* .rodata*)
#endif

#if !defined(GLOBAL_SRAM_CMSIS_FFT)
			*:CommonTables.o(.rodata.armBitRevIndexTable256)
			*:CommonTables.o(.rodata.twiddleCoef_256)
			*:CommonTables.o(.rodata.twiddleCoef_rfft_512)
			*:CommonTables.o(.rodata.armBitRevIndexTable128)
			*:CommonTables.o(.rodata.twiddleCoef_128)
			*:CommonTables.o(.rodata.twiddleCoef_rfft_256)
#endif

			/* common */
			*:fastmath_const.o(.data* .rodata*)

			*:preprocess.o(.data* .rodata*)
			*:filterbank.o(.data* .rodata*)

#if !defined(SCO_CP_ACCEL)
			*:plc_8000.o(.data* .rodata*)
#if defined(_CVSD_BYPASS_) || defined(CVSD_BYPASS)
			*:Pcm8k_Cvsd.o(.data* .rodata* .bss*)
			*:cvsd_codec.o(.data* .rodata* .bss*)
#endif
			*:table.o(.data* .rodata*)
			*:plc_16000.o(.data* .rodata*)
#endif
#if !defined(AUDIO_ANC_FB_ADJ_MC)
			*:mdf.o(.data* .rodata*)
#endif
#if defined(SPEECH_TX_2MIC_NS)
			*:dual_mic_denoise.o(.data* .rodata*)
			*:vad_filterbank.o(.data* .rodata*)
			*:energy.o(.data* .rodata*)
			*:get_scaling_square.o(.data* .rodata*)

			*:cross_correlation.o(.data* .rodata*)
			*:min_max_operations.o(.data* .rodata*)
			*:resample_by_2_internal.o(.data* .rodata*)
			*:division_operations.o(.data* .rodata*)
			*:downsample_fast.o(.data* .rodata*)
			*:resample_fractional.o(.data* .rodata*)
			*:resample_48khz.o(.data* .rodata*)
			*:vad_sp.o(.data* .rodata*)
			*:vad_core.o(.data* .rodata*)
			*:webrtc_vad.o(.data* .rodata*)
			*:vad_gmm.o(.data* .rodata*)
#endif
#if defined(SPEECH_TX_2MIC_NS2)
			*:speech_2mic_ns2_denoise.o(.data* .rodata*)
			*:speech_2mic_ns2_denoise_float.o(.data* .rodata*)
#endif
#if defined(SPEECH_TX_2MIC_NS4)
			*:sensormic_denoise.o(.data* .rodata*)
#endif
#if defined(SPEECH_TX_2MIC_NS5)
            *:leftright_denoise.o(.data* .rodata*)
#endif
#if defined(SPEECH_TX_NS2) || defined(SPEECH_RX_NS2)
			*:lc_mmse_ns.o(.data* .rodata*)
#endif
#if defined(SPEECH_TX_NS3) || defined(SPEECH_RX_NS3)
			*:nsx_core.o(.data* .rodata*)
#endif
#if defined(SPEECH_TX_NS2FLOAT) || defined(SPEECH_RX_NS2FLOAT)
			*:lc_mmse_ns_float.o(.data* .rodata*)
#endif
#if defined(SPEECH_TX_3MIC_NS)
			*:speech_3mic_ns.o(.data* .rodata*)
#endif
#if defined(SPEECH_TX_AEC3)
			*:SubBandBasedAEC.o(.data* .rodata*)
#endif
#if defined(SPEECH_CODEC_FIXED_SAMPLE_RATE)
			*:iir_resample.o(.data* .rodata*)
#endif

#endif // #if !defined(SCO_OPTIMIZE_FOR_RAM)

#if defined(SCO_CP_ACCEL)
			*:speech_win.o(.rodata.conj_fwin_60_128_cp)
			*:speech_win.o(.rodata.conj_fwin_120_256_cp)
			*:speech_win.o(.rodata.conj_fwin_128_256_cp)
			*:speech_win.o(.rodata.conj_fwin_240_512_cp)
			*:speech_win.o(.rodata.conj_fwin_256_512_cp)
#endif
			*(.overlay_data0 .overlay_rodata0)
			. = ALIGN(4);
			*(.overlay_sco_cache*)
			. = ALIGN(4);
		}
		.overlay_data1
		{
			LONG(0);
#if !defined(RB_CODEC)
			*:fixedpoint.o(.data* .rodata*)
			*:replaygain.o(.data* .rodata*)
			*:dsp_core.o(.data* .rodata*)
			*:dsp_misc.o(.data* .rodata*)
			*:dsp_filter.o(.data* .rodata*)
			*:dsp_sample_input.o(.data* .rodata*)
			*:dsp_sample_output.o(.data* .rodata*)
			*:eq.o(.data* .rodata*)
			*:pga.o(.data* .rodata*)
#if !defined(A2DP_AAC_ON)
			*:fir_process.o(.data* .rodata*)
#endif
#endif
#if defined(CHIP_HAS_CP) && (RAMCP_SIZE > 0)
		*:a2dp_decoder_sbc.o(.rodata*)
#endif
			*(.overlay_data1 .overlay_rodata1)
			. = ALIGN(4);
		}
		.overlay_data2
		{
			LONG(0);
			*(.overlay_data2 .overlay_rodata2)
			. = ALIGN(4);
		}
		.overlay_data3
		{
			LONG(0);
			*:fft128dot.o(.data* .rodata*)
			*(.overlay_data3 .overlay_rodata3)
			. = ALIGN(4);
		}
		.overlay_data4
		{
			LONG(0);
#ifdef A2DP_SOURCE_AAC_ON
			*:aacenc_lib.o(.data* .rodata*)
            *:aacenc.o(.data* .rodata*)
            *:adj_thr.o(.data* .rodata*)
            *:bit_cnt.o(.data* .rodata*)
            *:quantize.o(.data* .rodata*)
            *:tpenc_lib.o(.data* .rodata*)
            *:tpenc_latm.o(.data* .rodata*)
            *:line_pe.o(.data* .rodata*)
            *:spreading.o(.data* .rodata*)
            *:bitenc.o(.data* .rodata*)
            *:qc_main.o(.data* .rodata*)
            *:band_nrg.o(.data* .rodata*)
            *:aacenc_tns.o(.data* .rodata*)
            *:pre_echo_control.o(.data* .rodata*)

            *:transform.o(.data* .rodata*)
            *:dyn_bits.o(.data* .rodata*)
            *:aacenc_pns.o(.data* .rodata*)
            *:aacEnc_ram.o(.data* .rodata*)
            *:aacEnc_rom.o(.data* .rodata*)
            *:bandwidth.o(.data* .rodata*)
            *:block_switch.o(.data* .rodata*)
            *:channel_map.o(.data* .rodata*)
            *:chaosmeasure.o(.data* .rodata*)
            *:grp_data.o(.data* .rodata*)
            *:intensity.o(.data* .rodata*)
            *:metadata_compressor.o(.data* .rodata*)
            *:metadata_main.o(.data* .rodata*)
            *:ms_stereo.o(.data* .rodata*)
            *:noisedet.o(.data* .rodata*)
            *:pnsparam.o(.data* .rodata*)
            *:psy_configuration.o(.data* .rodata*)
            *:psy_main.o(.data* .rodata*)
            *:sf_estim.o(.data* .rodata*)
            *:tonality.o(.data* .rodata*)
#else
#if defined(A2DP_AAC_ON)
			*:block.o(.data* .rodata*)
			*:tpdec_lib.o(.data* .rodata*)
			*:tpdec_latm.o(.data* .rodata*)
			*:aacdec_pns.o(.data* .rodata*)
			*:aacdec_tns.o(.data* .rodata*)
			*:pulsedata.o(.data* .rodata*)
			*:channel.o(.data* .rodata*)
			*:tpdec_asc.o(.data* .rodata*)
			*:aacdecoder.o(.data* .rodata*)
			*:channelinfo.o(.data* .rodata*)
			*:aacdecoder_lib.o(.data* .rodata*)
			*:stereo.o(.data* .rodata*)
			*:fixpoint_math.o(.data* .rodata*)
			*:FDK_bitbuffer.o(.data* .rodata*)
#endif
#if defined(CHIP_HAS_CP) && (RAMCP_SIZE > 0)
			*:a2dp_decoder_aac_lc.o(.rodata*)
			*:fdkaac_codec.o(.rodata*)
#endif
#endif
			*(.overlay_data4 .overlay_rodata4)
			. = ALIGN(4);
		}
		.overlay_data5
		{
			LONG(0);
			*:mpa.o(.data* .bss*)
			*:bit.o(.data* .bss*)
			*:frame.o(.data* .bss*)
			*:huffman.o(.data* .bss*)
			*:layer12.o(.data* .bss*)
			*:layer3.o(.data* .bss*)
			*:stream.o(.data* .bss*)
			*:synth.o(.data* .bss*)
			*(.overlay_data5 .overlay_rodata5)
			. = ALIGN(4);
		}
		.overlay_data6
		{
			LONG(0);
#if defined(A2DP_SCALABLE_ON)
			*:rc_decode.o(.data* .rodata*)
			*:audio_int32.o(.data* .rodata*)
			*:ssc_ifft.o(.data* .rodata*)
			*:vector_quant.o(.data* .rodata*)
			*:ssc_quant.o(.data* .rodata*)
			*:ssc_pulsealloc.o(.data* .rodata*)
			*:audio.o(.data* .rodata*)
			*:set_sbm.o(.data* .rodata*)
			*:rc_code.o(.data* .rodata*)
			*:SBM_pitchBased.o(.data* .rodata*)
			*:ssc_imdct.o(.data* .rodata*)
			*:audio_decoder.o(.data* .rodata*)
			*:SoundBooster_DRC_classic.o(.data* .rodata*)
			*:ssc_cwrs.o(.data* .rodata*)
			*:ssc_mode.o(.data* .rodata*)
			*:ssc_math.o(.data* .rodata*)
			*:ssc_lap.o(.data* .rodata*)
			*:SeamlessBufferManagement.o(.data* .rodata*)
#if defined(CHIP_HAS_CP) && (RAMCP_SIZE > 0)
			*:a2dp_decoder_scalable.o(.rodata*)
#endif
#endif
#if defined(A2DP_LHDC_ON)
			*:a2dp_decoder_lhdc.o(.data* .rodata*)
			*:testkey.o(.data* .rodata*)
			*:lhdcUtil.o(.data* .rodata*)
			*:lzh_dec.o(.data* .rodata*)
			*:licdecrypt.o(.data* .rodata*)
			*:lic_check.o(.data* .rodata*)
			*:fast_upsampling_2x.o(.data* .rodata*)
			*:encdec.o(.data* .rodata*)
			*:dec.o(.data* .rodata*)
			*:cirbuf.o(.data* .rodata*)
			*:DecodeUps2xLPF.o(.data* .rodata*)
			*:Arc.o(.data* .rodata*)
#endif
#if defined(A2DP_LDAC_ON)
			*:a2dp_decoder_ldac.o(.data* .rodata*)
			*:bitalloc_ldac.o(.data* .rodata*)
			*:bitalloc_sub_ldac.o(.data* .rodata*)
			*:encode_ldac.o(.data* .rodata*)
			*:ldacBT.o(.data* .rodata*)
			*:ldaclib.o(.data* .rodata*)
			*:pack_ldac.o(.data* .rodata*)
			*:quant_ldac.o(.data* .rodata*)
			*:mdct_ldac.o(.data* .rodata*)
			*:ldacBT_api.o(.data* .rodata*)
			*:decode_ldac.o(.data* .rodata*)
			*:dequant_ldac.o(.data* .rodata*)
			*:imdct_ldac.o(.data* .rodata*)
			*:ldacBT_internal.o(.data* .rodata*)
			*:ldaclib_api.o(.data* .rodata*)
			*:memory_ldac.o(.data* .rodata*)
			*:setpcm_ldac.o(.data* .rodata*)
			*:sigana_ldac.o(.data* .rodata*)
			*:tables_ldac.o(.data* .rodata*)
			*:tables_sigproc_ldac.o(.data* .rodata*)
			*:unpack_ldac.o(.data* .rodata*)
#endif
			*(.overlay_data6 .overlay_rodata6)
			. = ALIGN(4);
		}
		.overlay_data7
		{
			LONG(0);
#if defined(OPUS_IN_OVERLAY)
			*:voice_opus.o(.data* .rodata*)
			*:opus_encoder.o(.data* .rodata*)
			*:analysis.o(.data* .rodata*)
			*:celt_encoder.o(.data* .rodata*)
			*:repacketizer.o(.data* .rodata*)
			*:opus.o(.data* .rodata*)
			*:enc_API.o(data* .rodata)
			*:entenc.o(.data* .rodata*)
			*:control_audio_bandwidth.o(.data* .rodata*)
			*:encode_pulses.o(.data* .rodata*)
			*:LPC_analysis_filter.o(.data* .rodata*)
			*:burg_modified_FIX.o(.data* .rodata*)
			*:vector_ops_FIX.o(.data* .rodata*)
			*:NLSF_del_dec_quant.o(.data* .rodata*)
			*:encode_frame_FIX.o(.data* .rodata*)
			*:pitch.o(.data* .rodata*)
			*:resampler.o(.data* .rodata*)
			*:VAD.o(.data* .rodata*)
			*:encode_indices.o(.data* .rodata*)
			*:control_codec.o(.data* .rodata*)
			*:celt_lpc.o(.data* .rodata*)
			*:shell_coder.o(.data* .rodata*)
			*:init_encoder.o(.data* .rodata*)
			*:log2lin.o(.data* .rodata*)
			*:biquad_alt.o(.data* .rodata*)
			*:stereo_encode_pred.o(.data* .rodata*)
			*:NLSF_unpack.o(.data* .rodata*)
			*:code_signs.o(.data* .rodata*)
			*:lin2log.o(.data* .rodata*)
			*:HP_variable_cutoff.o(.data* .rodata*)
			*:sum_sqr_shift.o(.data* .rodata*)
			*:inner_prod_aligned.o(.data* .rodata*)
			*:stereo_quant_pred.o(.data* .rodata*)
			*:entcode.o(.data* .rodata*)
			*:celt.o(.data* .rodata*)
			*:laplace.o(.data* .rodata*)
			*:NSQ.o(.data* .rodata*) /*complexity 1*/
			*:NSQ_del_dec.o(.data* .rodata*) /*complexity 2*/
#endif
			*(.overlay_data7 .overlay_rodata7)
			. = ALIGN(4);
		}
#endif
		.overlay_data_last
		{
#ifndef NO_OVERLAY
			LONG(0);
#endif
			. = ALIGN(4);
		}
	} > REGION_OVERLAY_DATA

	.dummy_overlay_data_end (.) (NOLOAD) : AT (ADDR(.dummy_overlay_data_end))
	{
		__overlay_data_end__ = .;
	} > REGION_OVERLAY_DATA

	ASSERT(__overlay_data_end__ - __overlay_data_start__ <= OVERLAY_DATA_SECTION_SIZE, "overlay data sections too large")

#if defined(CHIP_HAS_CP) && (RAMCP_SIZE > 0)
	.cp_ramx_last_dummy (NOLOAD) : AT (ADDR(.cp_ramx_last_dummy))
	{
		__cp_ramx_last_dummy_start = .;
	} > RAMCPX

	.cp_ram_last_dummy (NOLOAD) : AT (ADDR(.cp_ram_last_dummy))
	{
		. = ALIGN(8);
		__cp_ram_last_dummy_start = .;
	} > RAMCP

	__cp_stack_top = ORIGIN(RAMCP) + LENGTH(RAMCP);
	__cp_stack_limit = __cp_stack_top - CP_STACK_SECTION_SIZE;

	/* LD v2.24 (GCC 4.9) cannot handle address conversion correctly if involving immediate value.
	   E.g., <absolute_addr> + <immediate_val> + <relative_addr> */
	__free_ramcpx = ORIGIN(RAMCPX) + LENGTH(RAMCPX) - ABSOLUTE(__cp_ramx_last_dummy_start);
	ASSERT(__cp_stack_limit >= __cp_ram_last_dummy_start, "region RAMCP overflowed with stack")
	__free_ramcp = __cp_stack_limit - __cp_ram_last_dummy_start;

#ifdef MEM_POOL_IN_CP_RAM
	__cp_mem_pool_start__ = __cp_ram_last_dummy_start;
	__cp_mem_pool_end__ = __cp_ram_last_dummy_start;
	__mem_pool_start__ = __cp_ram_last_dummy_start;
	__mem_pool_end__ = __cp_stack_limit;
#else
	__cp_mem_pool_start__ = __cp_ram_last_dummy_start;
	__cp_mem_pool_end__ = __cp_stack_limit;
#endif
#endif

	__flash_text_data_start__ = __load_stop_overlay_data_last;

	.flashx_text (FLASH_TO_FLASHX(__flash_text_data_start__)) : AT (__flash_text_data_start__)
	{
		__flashx_text_start__ = .;
		*:notify.o(.text*)
		*:scan.o(.text*)
		*:wifi_conf_nvrecord.o(.text*)
		*:blacklist.o(.text*)
		*:ap.o(.text*)
		*:wpa_sme.o(.text*)
		*:wpas_glue.o(.text*)
		*:offchannel.o(.text*)
		*:wpa_supplicant.o(.text*)
		*:eap_register.o(.text*)
		*:wpa_interface.o(.text*)
		*:p2p_supplicant.o(.text*)
		*:config.o(.text*)
		*:sme.o(.text*)
		*:wps_supplicant.o(.text*)
		*:bss.o(.text*)
		*:trace.o(.text*)
		*:wpa_debug.o(.text*)
		*:wpabuf.o(.text*)
		*:common.o(.text*)
		*:bitfield.o(.text*)
		*:eloop.o(.text*)
		*:wpa_os.o(.text*)
		*:uuid.o(.text*)
		*:ieee802_11_common.o(.text*)
		*:wpa_common.o(.text*)
		*:ieee802_11_auth.o(.text*)
		*:wpa_auth_glue.o(.text*)
		*:eap_user_db.o(.text*)
		*:wnm_ap.o(.text*)
		*:sta_info.o(.text*)
		*:ieee802_1x.o(.text*)
		*:ctrl_iface_ap.o(.text*)
		*:wpa_auth_ft.o(.text*)
		*:wpa_auth_ie.o(.text*)
		*:preauth_auth.o(.text*)
		*:ieee802_11_ht.o(.text*)
		*:wpa_auth.o(.text*)
		*:ap_mlme.o(.text*)
		*:wmm.o(.text*)
		*:ieee802_11.o(.text*)
		*:dfs.o(.text*)
		*:hostapd.o(.text*)
		*:ieee802_11_vht.o(.text*)
		*:wps_hostapd.o(.text*)
		*:p2p_hostapd.o(.text*)
		*:drv_callbacks.o(.text*)
		*:ap_config.o(.text*)
		*:ieee802_11_shared.o(.text*)
		*:ap_drv_ops.o(.text*)
		*:tkip_countermeasures.o(.text*)
		*:peerkey_auth.o(.text*)
		*:hw_features.o(.text*)
		*:beacon.o(.text*)
		*:pmksa_cache_auth.o(.text*)
		*:utils.o(.text*)
		*:p2p_parse.o(.text*)
		*:p2p_utils.o(.text*)
		*:p2p_invitation.o(.text*)
		*:p2p.o(.text*)
		*:p2p_build.o(.text*)
		*:p2p_group.o(.text*)
		*:p2p_pd.o(.text*)
		*:p2p_dev_disc.o(.text*)
		*:p2p_go_neg.o(.text*)
		*:pmksa_cache.o(.text*)
		*:wpa_ie.o(.text*)
		*:wpa.o(.text*)
		*:preauth.o(.text*)
		*:driver_nl80211.o(.text*)
		*:nl80211.o(.text*)
		*:wpa_events.o(.text*)
		*:eap_common.o(.text*)
		*:eap_wsc_common.o(.text*)
		*:eap.o(.text*)
		*:eap_wsc.o(.text*)
		*:eap_methods.o(.text*)
		*:eapol_auth_sm.o(.text*)
		*:eapol_auth_dump.o(.text*)
		*:eapol_supp_sm.o(.text*)
		*:wps_attr_process.o(.text*)
		*:ndef.o(.text*)
		*:wps_validate.o(.text*)
		*:wps_attr_parse.o(.text*)
		*:wps_dev_attr.o(.text*)
		*:wps.o(.text*)
		*:wps_common.o(.text*)
		*:wps_registrar.o(.text*)
		*:wps_attr_build.o(.text*)
		*:wps_enrollee.o(.text*)
		*:tlsv1_server.o(.text*)
		*:pkcs8.o(.text*)
		*:x509v3.o(.text*)
		*:tlsv1_cred.o(.text*)
		*:tlsv1_client_read.o(.text*)
		*:tlsv1_client.o(.text*)
		*:rsa.o(.text*)
		*:tlsv1_client_write.o(.text*)
		*:asn1.o(.text*)
		*:tlsv1_server_read.o(.text*)
		*:pkcs1.o(.text*)
		*:pkcs5.o(.text*)
		*:tlsv1_record.o(.text*)
		*:bignum.o(.text*)
		*:tlsv1_server_write.o(.text*)
		*:tlsv1_common.o(.text*)
		*:eap_server.o(.text*)
		*:eap_server_identity.o(.text*)
		*:eap_server_methods.o(.text*)
		*:eap_server_wsc.o(.text*)
		*:hostapd_interface.o(.text*)
		*(EXCLUDE_FILE(*hal_*.o *cmsis_nvic.o *system_ARMCM4.o *system_utils.o *system_cp.o *cp_accel.o *bpabi.o *div*.o *_dvmd_tls.o *best*.o wifi_drv*.o) .text*)
		. = ALIGN(4);
		__flashx_text_end__ = .;
	}
	__flash_text_data_start__ += SIZEOF(.flashx_text);

#if defined(PSRAM_BASE) && (PSRAM_SIZE > 0)
	__psram_text_data_start__ = ORIGIN(PSRAM);
	__psram_text_data_start_flash__ = __flash_text_data_start__;
	.psram_text (PSRAM_TO_PSRAMX(__psram_text_data_start__)) : AT (__psram_text_data_start_flash__)
	{
/*
		__psramx_text_start__ .;

		*(EXCLUDE_FILE(*hal_*.o *cmsis_nvic.o *system_ARMCM4.o *system_utils.o *system_cp.o *cp_accel.o  *bpabi.o *div*.o *_dvmd_tls.o *best*.o) .text*)
		*(.psram_text*)
		. = ALIGN(4);
		__psramx_text_end__ = .;
*/
	} > PSRAMX

	.psram_data (__psram_text_data_start__ + SIZEOF(.psram_text)) :
				AT (__psram_text_data_start_flash__ + SIZEOF(.psram_text))
	{
		*(EXCLUDE_FILE(*hal_*.o *cmsis_nvic.o *system_ARMCM4.o *system_utils.o *system_cp.o *cp_accel.o *bpabi.o *div*.o *_dvmd_tls.o *best*.o ) .data*)
		*(.psram_data*)
		. = ALIGN(4);
	} > PSRAM

	__psram_text_data_end_flash__ = __psram_text_data_start_flash__ + SIZEOF(.psram_text)+ SIZEOF(.psram_data);

	.psram_bss (.) (NOLOAD) : AT (ADDR(.psram_bss))
	{
		__psram_bss_start__ = .;
		*(EXCLUDE_FILE(*hal_*.o *cmsis_nvic.o *system_ARMCM4.o *system_utils.o *system_cp.o *cp_accel.o *bpabi.o *div*.o *_dvmd_tls.o *best*.o ) .bss*)
		*:*ffmpeg*.o(COMMON)
		*(.psram_bss*)
		*(.bss*)
#ifdef __BT_RAMRUN__
		. = ALIGN(4);
		__mem_pool_start__ = .;
		. += 128*1000;
		__mem_pool_end__ = .;
#endif
#ifdef BLE_AUDIO_ENABLED
		. = ALIGN(4);
		__le_audio_mem_pool_start__ = .;
		. += 210*1024;
		__le_audio_mem_pool_end__ = .;
#endif		
#ifdef OS_HEAP_LINK_NAME
		. = ALIGN(64);
		__os_heap_start__ = .;
		OS_HEAP_LINK_NAME = .;
		. += OS_HEAP_SIZE;
		__os_heap_end__ = .;
#endif
#ifdef OS_HEAP_USE_ALL
		. = ALIGN(64);
		__os_heap_start__ = .;
		__os_heap_end__ = ABSOLUTE(PSRAM_BASE+PSRAM_SIZE);
#endif
		. = ALIGN(4);
		__psram_bss_end__ = .;
#if defined(NUTTX_BUILD)
		. = ALIGN(8);
		_nuttx_heap_start = .;
		_nuttx_heap_end = ABSOLUTE(PSRAM_BASE+PSRAM_SIZE);
#endif
	} > PSRAM
	__flash_text_data_start__ = __psram_text_data_end_flash__;
#endif

#if defined(PSRAMUHS_BASE) && (MCU_PSRAMUHS_SIZE > 0)
	__psramuhs_text_data_start__ = ORIGIN(PSRAMUHS);
	__psramuhs_text_data_start_flash__ = __flash_text_data_start__;
	.psramuhs_text (PSRAMUHS_TO_PSRAMUHSX(__psramuhs_text_data_start__)) : AT (__psramuhs_text_data_start_flash__)
	{
		__psramux_text_start__ = .;
/*
		*(EXCLUDE_FILE(*hal_*.o *cmsis_nvic.o *system_ARMCM4.o *system_utils.o *system_cp.o *cp_accel.o *bpabi.o *div*.o *_dvmd_tls.o *best*.o wifi_drv*.o) .text*)
*/
		*(.psramuhs_text*)
		. = ALIGN(4);
		__psramux_text_end__ = .;
	} > PSRAMUHSX

	.psramuhs_data (__psramuhs_text_data_start__ + SIZEOF(.psramuhs_text)) :
				AT (__psramuhs_text_data_start_flash__ + SIZEOF(.psramuhs_text))
	{
/*
		*(EXCLUDE_FILE(*hal_*.o *cmsis_nvic.o *system_ARMCM4.o *system_utils.o *system_cp.o *cp_accel.o *bpabi.o *div*.o *_dvmd_tls.o *best*.o wifi_drv*.o) .data*)
*/
		*(.psramuhs_data*)
		. = ALIGN(4);
	} > PSRAMUHS

	__psramuhs_text_data_end_flash__ = __psramuhs_text_data_start_flash__ + SIZEOF(.psramuhs_text)+ SIZEOF(.psramuhs_data);

	.psramuhs_bss (.) (NOLOAD) : AT (ADDR(.psramuhs_bss))
	{
		__psramuhs_bss_start__ = .;
/*
		*(EXCLUDE_FILE(*hal_*.o *cmsis_nvic.o *system_ARMCM4.o *system_utils.o *system_cp.o *cp_accel.o *bpabi.o *div*.o *_dvmd_tls.o *best*.o wifi_drv*.o) .bss*)
*/
		*(.psramuhs_bss*)
		. = ALIGN(4);
		__psramuhs_bss_end__ = .;
	} > PSRAMUHS
	__flash_text_data_start__ = __psramuhs_text_data_end_flash__;
#endif

	.text (FLASH_TO_FLASHX(__flash_text_data_start__)) : AT (FLASHX_TO_FLASH(ADDR(.text)))
	{
		__text_start__ = .;
		*(.text*)
		*(.flash_text*)

		KEEP(*(.init))
		KEEP(*(.fini))
		_sinit = ABSOLUTE(.);

		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)
		_einit = ABSOLUTE(.);

		/* .dtors */
		*crtbegin.o(.dtors)
		*crtbegin?.o(.dtors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
		*(SORT(.dtors.*))
		*(.dtors)
		__text_end__ = .;
	} > FLASHX
#if defined(ARM_CMNS) || defined(ARM_CMSE)
	. = ALIGN(32);
	__nsc_text_start__ = .;
	.gnu.sgstubs (.) : AT (FLASHX_TO_FLASH(ADDR(.gnu.sgstubs)))
	{
		. = ALIGN(4);
		*(.gnu.sgstubs.*)
	} > FLASHX
	. = ALIGN(32);
	__nsc_text_end__ = .;
#endif
	.ARM.extab (.) : AT (FLASHX_TO_FLASH(ADDR(.ARM.extab)))
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > FLASHX

	__exidx_start = .;
	/* .ARM.exidx contains R_ARM_PREL31 (+-0x40000000) offset to functions, which means
	 * the session location cannot be too far away from the function addresses */
	.ARM.exidx (.) : AT (FLASHX_TO_FLASH(ADDR(.ARM.exidx)))
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
		__flashx_text_end__ = .;
	} > FLASHX
	__exidx_end = .;

#ifdef ARM_CMSE
	/* BEST2003 SAU set FLASH_BASE region as nonsecure region becase of the SAU region number limitation,
	CPU can't access FLASH_BASE region in secure state, so the rodata should be located in SRAM or FLASHX*/
	.sec_rodata (.) : AT (FLASHX_TO_FLASH(ADDR(.sec_rodata)))
	{
		*(.rodata*)
	} > FLASHX
#endif
	__flash_text_end__ = .;

	. = FLASHX_TO_FLASH(.);
#ifdef TRACE_STR_SECTION
	.rodata_str (.):
	{
	    __rodata_str_start__ = .;
		*(.rodata.__func__.*)
		*(.rodata.*__func__)
		*(.rodata.str1*)
		*(.rodata.*.str1*)
		*(.rodata.__FUNCTION__.*)
		*(.rodata.*__FUNCTION__)
		__rodata_str_end__ = .;
	} > FLASH

	RODATA_ADDRESS = .;
	. = 0xFFFC0000;
	.trc_str (.):
	{
		*(.rodata.__func__.*)
		*(.rodata.*__func__)
		*(.rodata.__FUNCTION__.*)
		*(.rodata.*__FUNCTION__)
		*(.trc_str*)
	}
	. = RODATA_ADDRESS;
#endif
	.rodata (.) :
	{
		*(.rodata*)
		*(.flash_rodata*)
		*(.constdata)
#ifdef CONFIG_HAVE_CXX
		. = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(4);
		/* init data */
		PROVIDE(__ctors_start__ = .);
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);
		PROVIDE(__ctors_end__ = .);
#endif
		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);

		. = ALIGN(4);
		__cust_cmd_hldr_tbl_start = .;
		KEEP(*(.cust_cmd_hldr_tbl))
		__cust_cmd_hldr_tbl_end = .;
		. = ALIGN(4);
		__ai_gatt_server_table_start = .;
		KEEP(*(.ai_gatt_server_table))
		__ai_gatt_server_table_end = .;
		. = ALIGN(4);
		__ai_ble_handler_table_start = .;
		KEEP(*(.ai_ble_handler_table))
		__ai_ble_handler_table_end = .;
		. = ALIGN(4);
		__ai_spp_register_table_start = .;
		KEEP(*(.ai_spp_register_table))
		__ai_spp_register_table_end = .;
		. = ALIGN(4);
		__tota_handler_table_start = .;
		KEEP(*(.tota_handler_table))
		__tota_handler_table_end = .;
		. = ALIGN(4);
		__ai_handler_function_table_start = .;
		KEEP(*(.ai_handler_function_table))
		__ai_handler_function_table_end = .;
		. = ALIGN(4);
		__custom_handler_table_start = .;
		KEEP(*(.custom_handler_table))
		__custom_handler_table_end = .;
		. = ALIGN(4);
		__mixprompt_property_table_start = .;
		KEEP(*(.mixprompt_property_table))
		__mixprompt_property_table_end = .;
		. = ALIGN(4);
		__core_bridge_task_cmd_table_start = .;
		KEEP(*(.core_bridge_task_cmd_table))
		__core_bridge_task_cmd_table_end = .;
		. = ALIGN(4);
		__core_bridge_instant_cmd_table_start = .;
		KEEP(*(.core_bridge_instant_cmd_table))
		__core_bridge_instant_cmd_table_end = .;
		__eshell_command_table_start = .;
		KEEP(*(.eshell_command_table))
		__eshell_command_table_end = .;
		. = ALIGN(4);
		__app_bt_sync_command_handler_table_start = .;
		KEEP(*(.app_bt_sync_command_handler_table))
		__app_bt_sync_command_handler_table_end = .;
		. = ALIGN(4);
		. = ALIGN(4);
	 	KEEP(*(.eh_frame*))
		*(.note.gnu.build-id)

		. = ALIGN(4);
                __test_cases_table_start = .;
                KEEP(*(.test_cases_table))
                __test_cases_table_end = .;
                . = ALIGN(4);
		_etext = .;
	} > FLASH

#if ALIOS_APP_SUPPORT > 0
	.FSymTab (.) :
	{
		_syscall_table_begin = .;
		KEEP(*(FSymTab))
		. = ALIGN(4);
		_syscall_table_end = .;
	} > FLASH
#endif

#ifdef TRACE_STR_SECTION
	.trc_str (.) :
	{
		*(.trc_str*)
	} > FLASH
#endif

#if defined(FIRMWARE_REV)
	.firmware_rev (.) :
	{
		. = ALIGN(64);
		*(.fw_rev)
	} > FLASH
#endif
/* zInit code and data - will be freed after init */
	.zInit (.) :
	{
		__zinitcall_bsp_start = .;
		KEEP (*(.zinitcall.bsp0.init))
		KEEP (*(.zinitcall.bsp1.init))
		KEEP (*(.zinitcall.bsp2.init))
		KEEP (*(.zinitcall.bsp3.init))
		KEEP (*(.zinitcall.bsp4.init))
		__zinitcall_bsp_end = .;
		. = ALIGN(4);
		__zinitcall_device_start = .;
		KEEP (*(.zinitcall.device0.init))
		KEEP (*(.zinitcall.device1.init))
		KEEP (*(.zinitcall.device2.init))
		KEEP (*(.zinitcall.device3.init))
		KEEP (*(.zinitcall.device4.init))
		__zinitcall_device_end = .;
		. = ALIGN(4);
		__zinitcall_core_start = .;
		KEEP (*(.zinitcall.core0.init))
		KEEP (*(.zinitcall.core1.init))
		KEEP (*(.zinitcall.core2.init))
		KEEP (*(.zinitcall.core3.init))
		KEEP (*(.zinitcall.core4.init))
		__zinitcall_core_end = .;
		. = ALIGN(4);
		__zinitcall_sys_service_start = .;
		KEEP (*(.zinitcall.sys.service0.init))
		KEEP (*(.zinitcall.sys.service1.init))
		KEEP (*(.zinitcall.sys.service2.init))
		KEEP (*(.zinitcall.sys.service3.init))
		KEEP (*(.zinitcall.sys.service4.init))
		__zinitcall_sys_service_end = .;
		. = ALIGN(4);
		__zinitcall_sys_feature_start = .;
		KEEP (*(.zinitcall.sys.feature0.init))
		KEEP (*(.zinitcall.sys.feature1.init))
		KEEP (*(.zinitcall.sys.feature2.init))
		KEEP (*(.zinitcall.sys.feature3.init))
		KEEP (*(.zinitcall.sys.feature4.init))
		__zinitcall_sys_feature_end = .;
		. = ALIGN(4);
		__zinitcall_run_start = .;
		KEEP (*(.zinitcall.run0.init))
		KEEP (*(.zinitcall.run1.init))
		KEEP (*(.zinitcall.run2.init))
		KEEP (*(.zinitcall.run3.init))
		KEEP (*(.zinitcall.run4.init))
		__zinitcall_run_end = .;
		. = ALIGN(4);
		__zinitcall_app_service_start = .;
		KEEP (*(.zinitcall.app.service0.init))
		KEEP (*(.zinitcall.app.service1.init))
		KEEP (*(.zinitcall.app.service2.init))
		KEEP (*(.zinitcall.app.service3.init))
		KEEP (*(.zinitcall.app.service4.init))
		__zinitcall_app_service_end = .;
		. = ALIGN(4);
		__zinitcall_app_feature_start = .;
		KEEP (*(.zinitcall.app.feature0.init))
		KEEP (*(.zinitcall.app.feature1.init))
		KEEP (*(.zinitcall.app.feature2.init))
		KEEP (*(.zinitcall.app.feature3.init))
     		  KEEP (*(.zinitcall.app.feature4.init))
		__zinitcall_app_feature_end = .;
		. = ALIGN(4);
		__zinitcall_test_start = .;
		KEEP (*(.zinitcall.test0.init))
		KEEP (*(.zinitcall.test1.init))
		KEEP (*(.zinitcall.test2.init))
		KEEP (*(.zinitcall.test3.init))
		KEEP (*(.zinitcall.test4.init))
		__zinitcall_test_end = .;
		. = ALIGN(4);
		__zinitcall_exit_start = .;
		KEEP (*(.zinitcall.exit0.init))
		KEEP (*(.zinitcall.exit1.init))
		KEEP (*(.zinitcall.exit2.init))
		KEEP (*(.zinitcall.exit3.init))
		KEEP (*(.zinitcall.exit4.init))
		__zinitcall_exit_end = .;
		. = ALIGN(4);
	} > FLASH



	.interp (.) :
	{
		KEEP(*(interp))
		. = ALIGN(4);
	} > FLASH

	.dynstr (.) :
	{
		KEEP(*(dynstr))
		. = ALIGN(4);
	} > FLASH

	/* To copy multiple FLASH to RAM sections,
	 * uncomment .copy.table section and,
	 * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S */
	/*
	.copy.table (.) :
	{
		. = ALIGN(4);
		__copy_table_start__ = .;
		LONG (__etext)
		LONG (__data_start__)
		LONG (__data_end__ - __data_start__)
		LONG (__etext2)
		LONG (__data2_start__)
		LONG (__data2_end__ - __data2_start__)
		__copy_table_end__ = .;
	} > FLASH
	*/

	/* To clear multiple BSS sections,
	 * uncomment .zero.table section and,
	 * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */
	/*
	.zero.table (.) :
	{
		. = ALIGN(4);
		__zero_table_start__ = .;
		LONG (__bss_start__)
		LONG (__bss_end__ - __bss_start__)
		LONG (__bss2_start__)
		LONG (__bss2_end__ - __bss2_start__)
		__zero_table_end__ = .;
	} > FLASH
	*/

	/* Location counter can end up 2byte aligned with narrow Thumb code but
	   __etext is assumed by startup code to be the LMA of a section in RAM
	   which must be 4byte aligned */
	__etext = ALIGN(4);

	/* The VMA is either the end of overlay_data or the end of sram bss */

	.data : AT (__etext)
	{
		__data_start__ = .;
		_sdata = .;
		*(vtable)
		. = ALIGN(4);
		*(.data*)

		. = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);

		KEEP(*(.jcr*))

		/* All data end */
		. = ALIGN(4);
		*(*.got*)

		/* section information for finsh shell */
		. = ALIGN(4);
		__fsymtab_start = .;
		KEEP(*(FSymTab))
		__fsymtab_end = .;

		. = ALIGN(4);
		__vsymtab_start = .;
		KEEP(*(VSymTab))
		__vsymtab_end = .;

		/* section information for initial. */
		. = ALIGN(4);
		__rt_init_start = .;
		KEEP(*(SORT(.rti_fn*)))
		__rt_init_end = .;

		_hdf_drivers_start = .;
		KEEP(*(.hdf.driver))
		_hdf_drivers_end = .;

		__data_end__ = .;
		_edata = .;
        __shell_root_cmds_start = .;
        KEEP(*(SORT(.shell_root_cmd_*)));
        __shell_root_cmds_end = .;
	} > RAM

	.bss (.) (NOLOAD) : AT (ADDR(.bss))
	{
		. = ALIGN(4);
		__bss_start__ = .;
		_sbss = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
#if defined(OS_HEAP_SIZE) && (!defined(PSRAM_BASE) || (PSRAM_SIZE == 0))
		__os_heap_start__ = .;
		OS_HEAP_LINK_NAME = .;
		. += OS_HEAP_SIZE;
		__os_heap_end__ = .;
#endif
		_ebss = .;
	} > RAM
#if defined(NUTTX_BUILD) && (!defined(PSRAM_BASE) || (PSRAM_SIZE == 0)) && defined(CONFIG_USE_BES_BT_STACK)
	.nuttx_heap (.) (NOLOAD) :
	{
		_nuttx_heap_start = .;
		. = ALIGN(8);
		. += NUTTX_HEAP_SECTION_SIZE;
		. = ALIGN(8);
		_nuttx_heap_end = .;
	} > RAM
#endif
	.reboot_custom_param (.) (NOLOAD) :
	{
		KEEP(*(.reboot_custom_param*))
		. = ALIGN(4);
#if defined(CUSTOM_REBOOT_DUMP_SIZE)
		__custom_reboot_dump_start__ = .;
		. += CUSTOM_REBOOT_DUMP_SIZE;
		__custom_reboot_dump_end__ = .;
#endif
		__reboot_custom_param_end__ = .;
	} > RAM

	.heap (.) (NOLOAD) :
	{
		. = ALIGN(8);
		__HeapBase = .;
		__end__ = .;
		end = __end__;
		. += HEAP_SECTION_SIZE;
		. = ALIGN(8);
		__HeapLimit = .;
	} > RAM

#ifdef ROM_IN_FLASH
	.scratch_buf (.) (NOLOAD) :
	{
		. = ALIGN(4);
		*(.scratch_buf)
		. = ALIGN(4);
	} > RAM
#endif

    /* ramv will be occupied by nts heap of cp if WIFI_ON_CP defined */
#if defined(RAMV_BASE) && (RAMV_SIZE > 0) && !defined(WIFI_ON_CP)
	.cp_vram_bss (ORIGIN(RAMV)) (NOLOAD) : AT (ADDR(.cp_vram_bss))
	{
		. = ALIGN(4);
		*(.vram_bss*)
	} > RAMV
#endif

	/* .stack_dummy section doesn't contains any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
	.stack_dummy (.) (COPY) :
	{
		. = STACK_SECTION_SIZE;
		. = ALIGN(8);
	} > RAM

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
#if defined(ROM_UTILS_ON)
	__StackTop = ORIGIN(RAM) + LENGTH(RAM) - ROM_UTILS_RESV_RAM_SIZE;
#else
	__StackTop = ORIGIN(RAM) + LENGTH(RAM);
#endif
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);
	_sstack = __StackLimit;
	_estack = __stack;
#if defined(NUTTX_BUILD) && (!defined(PSRAM_BASE) || (PSRAM_SIZE == 0)) && !defined(CONFIG_USE_BES_BT_STACK)
	_nuttx_heap_start = __HeapLimit;
	_nuttx_heap_end = __StackLimit - 4;
#endif
	/* Check if data + heap + stack exceeds RAM limit */
	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
	__free_ram = __StackLimit - __HeapLimit;

#ifndef __BT_RAMRUN__
#ifdef MEM_POOL_BASE
	.mem_pool (ORIGIN(MEM_POOL)) (NOLOAD) :
	{
		__mem_pool_start__ = .;
		. = LENGTH(MEM_POOL);
		__mem_pool_end__ = .;
	} > MEM_POOL
#elif !(defined(CHIP_HAS_CP) && (RAMCP_SIZE > 0) && defined(MEM_POOL_IN_CP_RAM))
	__mem_pool_start__ = __HeapLimit;
	__mem_pool_end__ = __StackLimit;
#endif
#endif

	. = __etext + SIZEOF(.data);

	.system_info (.) :
	{
		KEEP(*(.system_info_list_size))
		KEEP(*(.system_info_list))
		. = ALIGN(4);
	}  > FLASH

	.build_info (.) :
	{
		KEEP(*(.build_info))
		. = ALIGN(4);
	} > FLASH = 0x00000000

#ifdef SLAVE_BIN_FLASH_OFFSET
	.slave_code (ORIGIN(FLASH) + SLAVE_BIN_FLASH_OFFSET):
	{
		KEEP(*(.slave_code_flash_rodata))
	} > FLASH
#endif

	/* The following section MUST be the last loaded section */
	.code_start_addr (.) :
	{
#ifdef TRACE_STR_SECTION
		LONG(ABSOLUTE(__rodata_str_start__));
#else
		LONG(0);
#endif
		LONG(BUILD_INFO_MAGIC);
		LONG(ABSOLUTE(__flash_start));
	} > FLASH

	__flash_end = .;

#if 0
	/* file system fixed to 0x4B8000 space, start addr:0xB38000 */
	.filesystem (ORIGIN(FLASH_NC) + LENGTH(FLASH_NC)- 0x4C8000) :
	{
		__file_system_flash_start = .;
		. = FILE_SYSTEM_SECTION_SIZE;
		__file_system_flash_stop = .;
	} > FLASH_NC
#endif
	.combo (ORIGIN(FLASH_NC) + COMBO_SECTION_OFFSET + FLASH_BASE - FLASH_REGION_BASE ) (NOLOAD):
	{
		__combo_start = .;
		. = COMBO_SECTION_SIZE;
		__combo_end = .;
	} > FLASH_NC

	.prompt (ORIGIN(FLASH_NC) + PROMPT_SECTION_OFFSET + FLASH_BASE - FLASH_REGION_BASE) (NOLOAD):
	{
		__prompt_start = .;
		. = PROMPT_SECTION_SIZE;
		__prompt_end = .;
	} > FLASH_NC

	.hotword_model (ORIGIN(FLASH_NC) + HOTWORD_SECTION_OFFSET + FLASH_BASE - FLASH_REGION_BASE) (NOLOAD):
	{
		__hotword_model_start = .;
		. = HOTWORD_SECTION_SIZE;
		__hotword_model_end = .;
	} > FLASH_NC

	.coredump_section (ORIGIN(FLASH_NC) + CORE_DUMP_SECTION_OFFSET + FLASH_BASE - FLASH_REGION_BASE) (NOLOAD) :
	{
		__coredump_section_start = .;
		. = CORE_DUMP_SECTION_SIZE;
		__coredump_section_end = .;
	} > FLASH_NC

	.ota_upgrade_log (ORIGIN(FLASH_NC) + OTA_UPGRADE_LOG_OFFSET + FLASH_BASE - FLASH_REGION_BASE) (NOLOAD) :
	{
		__ota_upgrade_log_start = .;
		. = OTA_UPGRADE_LOG_SIZE;
		__ota_upgrade_log_end = .;
	} > FLASH_NC

	.log_dump (ORIGIN(FLASH_NC) + LOG_DUMP_SECTION_OFFSET + FLASH_BASE - FLASH_REGION_BASE) (NOLOAD) :
	{
		__log_dump_start = .;
		. = LOG_DUMP_SECTION_SIZE;
		__log_dump_end = .;
	} > FLASH_NC

	.crash_dump (ORIGIN(FLASH_NC) + CRASH_DUMP_SECTION_OFFSET + FLASH_BASE - FLASH_REGION_BASE) (NOLOAD) :
	{
		__crash_dump_start = .;
		. = CRASH_DUMP_SECTION_SIZE;
		__crash_dump_end = .;
	} > FLASH_NC

	.custom_parameter (ORIGIN(FLASH_NC) + CUSTOM_PARAMETER_SECTION_OFFSET + FLASH_BASE - FLASH_REGION_BASE) (NOLOAD) :
	{
		__custom_parameter_start = .;
		. = CUSTOM_PARAMETER_SECTION_SIZE;
		__custom_parameter_end = .;
	} > FLASH_NC

	.lhdc_license (ORIGIN(FLASH_NC) + LHDC_LICENSE_SECTION_OFFSET + FLASH_BASE - FLASH_REGION_BASE) (NOLOAD) :
	{
		__lhdc_license_start = .;
		. = LHDC_LICENSE_SECTION_SIZE;
		__lhdc_license_end = .;
	} > FLASH_NC

	.userdata (ORIGIN(FLASH_NC) + USERDATA_SECTION_OFFSET + FLASH_BASE - FLASH_REGION_BASE) (NOLOAD) :
	{
		__userdata_start = .;
		. = USERDATA_SECTION_SIZE*2;
		__userdata_end = .;
	} > FLASH_NC

	.audio (ORIGIN(FLASH_NC) + AUD_SECTION_OFFSET + FLASH_BASE - FLASH_REGION_BASE) (NOLOAD) :
	{
		__aud_start = .;
		. = AUD_SECTION_SIZE;
		__aud_end = .;
	} > FLASH_NC

	.reserved (ORIGIN(FLASH_NC) + RESERVED_SECTION_OFFSET + FLASH_BASE - FLASH_REGION_BASE) (NOLOAD) :
	{
		__reserved_start = .;
		. = RESERVED_SECTION_SIZE;
		__reserved_end = .;
	} > FLASH_NC

	.factory (ORIGIN(FLASH_NC) + FACTORY_SECTION_OFFSET + FLASH_BASE - FLASH_REGION_BASE) (NOLOAD) :
	{
		__factory_start = .;
		. = FACTORY_SECTION_SIZE;
		__factory_end = .;
	} > FLASH_NC

#ifdef OTA_BOOT_SIZE
#ifndef OTA_BOOT_OFFSET
#error "OTA_BOOT_OFFSET should be set along with OTA_BOOT_SIZE"
#endif
#if (OTA_BOOT_SIZE > FLASH_REGION_SIZE)
#error "OTA_BOOT_SIZE should <= FLASH_REGION_SIZE"
#endif
#if defined(OTA_CODE_OFFSET) && ((OTA_BOOT_OFFSET + OTA_BOOT_SIZE) > OTA_CODE_OFFSET)
#error "(OTA_BOOT_OFFSET + OTA_BOOT_SIZE) should <= OTA_CODE_OFFSET"
#endif
	__tail_section_start = FLASH_NC_BASE + OTA_BOOT_OFFSET + OTA_BOOT_SIZE;
#else
	__tail_section_start = __combo_start;
#endif

	ASSERT(FLASH_NC_TO_C(__tail_section_start) >= __flash_end, "region FLASH overflowed")
	__free_flash = FLASH_NC_TO_C(__tail_section_start) - __flash_end;

#ifdef OTA_REMAP_OFFSET
#ifndef OTA_CODE_OFFSET
#error "OTA_CODE_OFFSET should be set along with OTA_REMAP_OFFSET"
#endif
#ifndef OTA_BOOT_SIZE
	/* This is not an OTA booting code, but a normal working code */
	ASSERT((FLASH_BASE & 0x03FFFFFF) == 0, "bad FLASH_BASE")
#ifdef CHIP_BEST2300P
	ASSERT((FLASH_SIZE & (FLASH_SIZE - 1)) == 0, "bad FLASH_SIZE")
	ASSERT(__flash_end <= FLASH_BASE + OTA_REMAP_OFFSET, "flash code size too large to remap")
	ASSERT(__flash_start - FLASH_BASE >= FLASH_NC_BASE + FLASH_SIZE - __tail_section_start, "flash code conflicted with tail sections in remap")
	ASSERT(__flash_start - FLASH_BASE >= FLASH_SIZE / (1 << (4 + 1)), "flash code cannot remap within 4 regions (max code size is half of flash size)")
	ASSERT((__flash_start - FLASH_BASE) % (FLASH_SIZE / (1 << (4 + 1))) == 0, "flash code start address not aligned with remap region boundary")
#else
	ASSERT(((OTA_REMAP_OFFSET) & (0x8000 - 1)) == 0, "OTA_REMAP_OFFSET not aligned with 32K sector boundary")
	ASSERT(__flash_end <= FLASH_BASE + (OTA_CODE_OFFSET + OTA_REMAP_OFFSET), "flash code size too large to remap")
	ASSERT((__tail_section_start & (0x1000 - 1)) == 0, "__tail_section_start not aligned with 4K sector boundary")
	ASSERT(__flash_end + (OTA_REMAP_OFFSET) <= FLASH_NC_TO_C(__tail_section_start), "flash code conflicted with tail sections in remap")
#endif
#endif
#endif
}

