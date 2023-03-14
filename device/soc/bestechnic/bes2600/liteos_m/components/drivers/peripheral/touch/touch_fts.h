/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __TOUCH_FTS_H__
#define __TOUCH_FTS_H__

#include "touch.h"

#define FTS_CHIP_TYPE 0x6336D003 //_FT6336U
#define FLAGBIT(x) (0x00000001 << (x))
#define FLAGBITS(x, y) ((0xFFFFFFFF >> (32 - (y)-1)) << (x))

#define FLAG_ICSERIALS_LEN 8
#define FLAG_HID_BIT 10
#define FLAG_IDC_BIT 11

#define FTS_CHIP_IDC ((FTS_CHIP_TYPE & FLAGBIT(FLAG_IDC_BIT)) == FLAGBIT(FLAG_IDC_BIT))
#define FTS_HID_SUPPORTTED ((FTS_CHIP_TYPE & FLAGBIT(FLAG_HID_BIT)) == FLAGBIT(FLAG_HID_BIT))
#define FTS_CHIP_TYPE_MAPPING {{0x03,0x64, 0x26, 0x64, 0x26, 0x00, 0x00, 0x79, 0x1C}}

#define I2C_BUFFER_LENGTH_MAXINUM 256
#define FILE_NAME_LENGTH 128
#define ENABLE 1
#define DISABLE 0
#define VALID 1

#define FTS_CMD_START1 0x55
#define FTS_CMD_START2 0xAA
#define FTS_CMD_START_DELAY 10
#define FTS_CMD_READ_ID 0x90
#define FTS_CMD_READ_ID_LEN 4
#define FTS_CMD_READ_ID_LEN_INCELL 1
/*register address*/
#define FTS_REG_INT_CNT 0x8F
#define FTS_REG_FLOW_WORK_CNT 0x91
#define FTS_REG_WORKMODE 0x00
#define FTS_REG_WORKMODE_FACTORY_VALUE 0x40
#define FTS_REG_WORKMODE_WORK_VALUE 0x00
#define FTS_REG_ESDCHECK_DISABLE 0x8D
#define FTS_REG_CHIP_ID 0xA3
#define FTS_REG_CHIP_ID2 0x9F
#define FTS_REG_POWER_MODE 0xA5
#define FTS_REG_POWER_MODE_SLEEP_VALUE 0x03
#define FTS_REG_FW_VER 0xA6
#define FTS_REG_VENDOR_ID 0xA8
#define FTS_REG_LCD_BUSY_NUM 0xAB
#define FTS_REG_FACE_DEC_MODE_EN 0xB0
#define FTS_REG_FACE_DEC_MODE_STATUS 0x01
#define FTS_REG_IDE_PARA_VER_ID 0xB5
#define FTS_REG_IDE_PARA_STATUS 0xB6
#define FTS_REG_GLOVE_MODE_EN 0xC0
#define FTS_REG_COVER_MODE_EN 0xC1
#define FTS_REG_CHARGER_MODE_EN 0x8B
#define FTS_REG_GESTURE_EN 0xD0
#define FTS_REG_GESTURE_OUTPUT_ADDRESS 0xD3
#define FTS_REG_MODULE_ID 0xE3
#define FTS_REG_LIC_VER 0xE4
#define FTS_REG_ESD_SATURATE 0xED

#define FTS_MAX_POINTS_SUPPORT 10 /* constant value, can't be changed */
#define FTS_MAX_ID 0x0A
#define FTS_ONE_TCH_LEN 6
#define FTS_FACE_DETECT_POS 1
#define FTS_TOUCH_X_H_POS 3
#define FTS_TOUCH_X_L_POS 4
#define FTS_TOUCH_Y_H_POS 5
#define FTS_TOUCH_Y_L_POS 6
#define FTS_TOUCH_PRE_POS 7
#define FTS_TOUCH_AREA_POS 8
#define FTS_TOUCH_EVENT_POS 3
#define FTS_TOUCH_ID_POS 5
#define FTS_TOUCH_POINT_NUM 2
#define FTS_TOUCH_XY_POS 7
#define FTS_TOUCH_MISC 8
#define FTS_COORDS_ARR_SIZE 4
#define FT_FW_NAME_MAX_LEN 50

#define TPD_RES_X 480
#define TPD_RES_Y 480

#define FTS_TOUCH_DOWN 0
#define FTS_TOUCH_UP 1
#define FTS_TOUCH_CONTACT 2
#define EVENT_DOWN(flag) ((FTS_TOUCH_DOWN == flag) || (FTS_TOUCH_CONTACT == flag))
#define EVENT_UP(flag) (FTS_TOUCH_UP == flag)
#define EVENT_NO_DOWN(data) (!data->point_num)
#define KEY_EN 0 //(tpd_dts_data.use_tpd_button)
#define TOUCH_IS_KEY(y, key_y) (y > key_y)

#ifndef RTPM_PRIO_TPD
#define RTPM_PRIO_TPD 0x04
#endif

#define FTX_MAX_COMPATIBLE_TYPE 4

/*****************************************************************************
* Private enumerations, structures and unions using typedef
*****************************************************************************/
struct i2c_client {
    DevHandle handle;
    uint16_t id;
    uint16_t addr;
};

struct ft_chip_t {
    uint64_t type;
    uint8_t chip_idh;
    uint8_t chip_idl;
    uint8_t rom_idh;
    uint8_t rom_idl;
    uint8_t pb_idh;
    uint8_t pb_idl;
    uint8_t bl_idh;
    uint8_t bl_idl;
};

struct ts_ic_info {
    bool is_incell;
    bool hid_supported;
    struct ft_chip_t ids;
};

/*touch event info*/
struct ts_event {
    int x;    /*x coordinate */
    int y;    /*y coordinate */
    int p;    /* pressure */
    int flag; /* touch event flag: 0 -- down; 1-- up; 2 -- contact */
    int id;   /*touch ID */
    int area;
};
struct fts_ts_data {
    struct i2c_client *client;
    struct ts_ic_info ic_info;
    osMutexId_t report_mutex;
    int irq;
    bool suspended;
    bool fw_loading;
    bool irq_disabled;
    bool power_disabled;
    /* multi-touch */
    struct ts_event *events;
    uint8_t *point_buf;
    int pnt_buf_size;
    int touchs;
    bool key_down;
    int touch_point;
    int point_num;
};

#endif
