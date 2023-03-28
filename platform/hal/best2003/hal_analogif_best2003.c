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
#ifndef NO_ISPI

#include "plat_types.h"
#include "cmsis.h"
#include "hal_analogif.h"
#include "hal_location.h"
#include "hal_spi.h"

#define ANA_REG_CHIP_ID                 0x00

#define ANA_CHIP_ID_SHIFT               (4)
#define ANA_CHIP_ID_MASK                (0xFFF << ANA_CHIP_ID_SHIFT)
#define ANA_CHIP_ID(n)                  BITFIELD_VAL(ANA_CHIP_ID, n)
#define ANA_VAL_CHIP_ID                 0x203

// ISPI_ARBITRATOR_ENABLE should be defined when:
// 1) BT and MCU will access RF register at the same time; or
// 2) BT can access PMU/ANA, and BT will access RF register at the same time
//    when MCU is accessing PMU/ANA register

#ifdef ISPI_ARBITRATOR_ENABLE
// Min padding OSC cycles needed: BT=0 MCU=6
// When OSC=26M and SPI=6.5M, min padding SPI cycles is BT=0 MCU=2
#define PADDING_CYCLES                  2
#else
#define PADDING_CYCLES                  0
#endif

#define ANA_READ_CMD(r)                 (((1 << 26) | (((r) & 0x3FF) << 16)) << PADDING_CYCLES)
#define ANA_WRITE_CMD(r, v)             (((((r) & 0x3FF) << 16) | ((v) & 0xFFFF)) << PADDING_CYCLES)
#define ANA_READ_VAL(v)                 (((v) >> PADDING_CYCLES) & 0xFFFF)

#define ANA_PAGE_0                      0xA000
#define ANA_PAGE_1                      0xA010
#define ANA_PAGE_QTY                    2

#define ISPI_REG_CS(r)                  ((r) >> 12)
#define ISPI_REG_OFFSET(r)              ((r) & 0x3FF)

enum ANAIF_CS_T {
    ANAIF_CS_PMU = 0,
    ANAIF_CS_ANA,
    ANAIF_CS_BTRF,
    ANAIF_CS_WIFIRF,

    ANAIF_CS_QTY,
};

static const struct HAL_SPI_CFG_T spi_cfg = {
    .clk_delay_half = false,
    .clk_polarity = false,
    .slave = false,
    .dma_rx = false,
    .dma_tx = false,
    .rx_sep_line = true,
    .cs = 0,
    .rate = 6500000,
    .tx_bits = 27 + PADDING_CYCLES,
    .rx_bits = 27 + PADDING_CYCLES,
    .rx_frame_bits = 0,
};

static bool BOOT_BSS_LOC analogif_inited = false;
static uint8_t BOOT_BSS_LOC ana_cs;

static int BOOT_TEXT_SRAM_LOC hal_analogif_rawread(unsigned short reg, unsigned short *val)
{
    int ret;
    unsigned int data;
    unsigned int cmd;

    data = 0;
    cmd = ANA_READ_CMD(reg);
    ret = hal_ispi_recv(&cmd, &data, 4);
    if (ret) {
        return ret;
    }
    *val = ANA_READ_VAL(data);
    return 0;
}

static int BOOT_TEXT_SRAM_LOC hal_analogif_rawwrite(unsigned short reg, unsigned short val)
{
    int ret;
    unsigned int cmd;

    cmd = ANA_WRITE_CMD(reg, val);
    ret = hal_ispi_send(&cmd, 4);
    if (ret) {
        return ret;
    }
    return 0;
}

int BOOT_TEXT_SRAM_LOC hal_analogif_reg_read(unsigned short reg, unsigned short *val)
{
    uint32_t lock;
    int ret;
    uint8_t cs;

    cs = ISPI_REG_CS(reg);
    reg = ISPI_REG_OFFSET(reg);

    lock = int_lock();
    if (cs != ana_cs) {
        hal_ispi_activate_cs(cs);
        ana_cs = cs;
    }
    ret = hal_analogif_rawread(reg, val);
    int_unlock(lock);

    return ret;
}

int BOOT_TEXT_SRAM_LOC hal_analogif_reg_write(unsigned short reg, unsigned short val)
{
    uint32_t lock;
    int ret;
    uint8_t cs;

    cs = ISPI_REG_CS(reg);
    reg = ISPI_REG_OFFSET(reg);

    lock = int_lock();
    if (cs != ana_cs) {
        hal_ispi_activate_cs(cs);
        ana_cs = cs;
    }
    ret = hal_analogif_rawwrite(reg, val);
    int_unlock(lock);

    return ret;
}

int BOOT_TEXT_FLASH_LOC hal_analogif_open(void)
{
    int ret;
    unsigned short chip_id;
    const struct HAL_SPI_CFG_T *cfg_ptr;
    struct HAL_SPI_CFG_T cfg;

    if (analogif_inited) {
        // Restore the nominal rate
        cfg_ptr = &spi_cfg;
    } else {
        analogif_inited = true;
        // Crystal freq is unknown yet. Let SPI run on half of the nominal rate
        cfg = spi_cfg;
        cfg.rate /= 2;
        cfg_ptr = &cfg;
    }

    ret = hal_ispi_open(cfg_ptr);
    if (ret) {
        return ret;
    }

    ana_cs = 0;
    hal_ispi_activate_cs(ana_cs);

    ret = hal_analogif_rawread(ANA_REG_CHIP_ID, &chip_id);
    if (ret) {
        return ret;
    }

#if defined(FPGA) || defined(SIMU)
    chip_id = ANA_CHIP_ID(ANA_VAL_CHIP_ID);
#endif

    if (GET_BITFIELD(chip_id, ANA_CHIP_ID) != ANA_VAL_CHIP_ID) {
        return -1;
    }

    return 0;
}

#endif

