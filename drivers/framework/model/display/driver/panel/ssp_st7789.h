/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SSP_ST7789_H
#define SSP_ST7789_H
#include "hdf_base.h"
#define RESET_GPIO       5
#define LCD_CMD          0
#define LCD_DATA         1
#define SPI_MSG_SIZE     2
#define BITS_PER_BYTE    8
#define BITS_PER_WORD    9
#define SPI_MAX_SPEED    115200

struct LcdCmd {
    uint8_t cmd;
    uint8_t data;
    uint8_t delay;
};
#endif /* SSP_ST7789_H */
