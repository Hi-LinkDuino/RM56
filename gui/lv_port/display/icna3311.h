/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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
#ifndef __ICNA3311_H__
#define __ICNA3311_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

#define TM_SCREEN_ID 0x42
#define HH_SCREEN_ID 0x41
#define HFP 8
#define HBP 8
#define VFP 8
#define VBP 8

#define ICNA3311_LSHIFT(val,bits) (val<<bits)

#define ICNA3311_REG_ID2        0xdb
#define ICNA3311_REG_SLEEP_OUT  0x11
#define ICNA3311_REG_DISPLAY_ON 0x29

#define ICNA3311_REG_PAGE_SWITCH        0xfe
#define ICNA3311_REG_TE_ON              0x35
#define ICNA3311_REG_MEM_CTRL           0x36
#define ICNA3311_REG_BRIGHTNESS_CTRL    0x53
#define ICNA3311_REG_BRIGHTNESS_SET     0x51
#define ICNA3311_REG_HBM_BRIGHTNESS_SET 0x63
#define ICNA3311_REG_ROW_ADDR_SET       0x2a
#define ICNA3311_REG_COLUMN_ADDR_SET    0x2b

#define ICNA3311_BRIGHTNESS_MAX 0xff
#define ICNA3311_BRIGHTNESS_MIN 0x00
#define ICNA3311_CMD_UCS        0x00  //User Command Sets
#define ICNA3311_TE_MODE_VBLANK 0x00
#define ICNA3311_TE_MODE_VBLANK_HBLANK 0x01

/* ---+---+---+---+---+---+---+---+
 *  Y | X | - | - | R | - | - | - |
 * ---+---+---+---+---+---+---+---+
 *  Y - 0 row address increase(in vertical)
 *  Y - 1 row address decrease
 *  X - 0 column address increase (in horizontal)
 *  X - 1 column address decrease
 *  R - 0 RGB order
 *  R - 1 BGR order
 */
#define ICNA3311_MEM_IMAGE (ICNA3311_LSHIFT(0,7) | ICNA3311_LSHIFT(0,6) | ICNA3311_LSHIFT(1, 3))

/* ---+---+---+---+---+---+---+---+
 *  - | - | B | - | D | - | - | - |
 * ---+---+---+---+---+---+---+---+
 * B - 0 disable (brightness)
 * B - 1 enable
 * D - 0 off (dimming)
 * D - 1 on
 */
#define ICNA3311_BRIGHTNESS_ON (ICNA3311_LSHIFT(1,5) | ICNA3311_LSHIFT(0,3))

typedef struct icna3311_cmd_
{
    uint8_t reg;
    uint8_t parameters;
}icna3311_cmd_t;

typedef struct icna3311_row_column_
{
    uint8_t reg;
    uint8_t parameters[4];
}icna3311_row_column_t;

#ifdef __cplusplus
}
#endif

#endif