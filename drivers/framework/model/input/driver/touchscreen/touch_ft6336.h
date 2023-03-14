/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef TOUCH_FT6236_H
#define TOUCH_FT6236_H

/* register address */
#define FTS_REG_POINT_RATE    0x88    // report rate
#define FTS_REG_FW_VER        0xA6    // FW version
#define FTS_REG_CHIP_ID       0xA3    // Chip ID
#define FTS_REG_VENDOR_ID     0xA8    // TP vendor ID

/* the config for parsing event buffer */
#define HALF_BYTE_MASK 0x0F
#define HALF_BYTE_OFFSET   4
#define SIX_BIT_OFFSET     6
#define ONE_BYTE_OFFSET    8

/* read data buffer len */
/* head info occupy 3byte (Mode Switch, reserved, TD_status--point num),
 * each point info occupy 6 bytes.
 */
#define POINT_BUFFER_LEN (FT_POINT_SIZE * MAX_SUPPORT_POINT + 3)
/* the FT6236 driver ic can support 2 points */
#define MAX_SUPPORT_POINT 2
/* buffer size of one event */
#define FT_POINT_SIZE     6
/* point data position */
#define FT_POINT_NUM_POS  2
#define FT_EVENT_POS      3
#define FT_X_H_POS        3
#define FT_X_L_POS        4
#define FT_Y_H_POS        5
#define FT_FINGER_POS     5
#define FT_Y_L_POS        6

#endif
