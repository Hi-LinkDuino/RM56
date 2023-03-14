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

#ifndef LCD_ABS_IF_H
#define LCD_ABS_IF_H

#include "disp_common.h"
#include "mipi_dsi_if.h"

/* support max panel number */
#define PANEL_MAX 1

enum LcdIntfType {
    MIPI_DSI,
    LCD_6BIT,
    LCD_8BIT,
    LCD_16BIT,
    LCD_18BIT,
    LCD_24BIT,
};

enum BacklightType {
    BLK_PWM,
    BLK_MIPI,
};

struct MipiDsiDesc {
    enum DsiLane lane;
    enum DsiMode mode; /* output mode: DSI_VIDEO/DSI_CMD */
    enum DsiBurstMode burstMode;
    enum DsiOutFormat format;
};

enum PowerStatus {
    POWER_STATUS_ON,      /* The power status is on */
    POWER_STATUS_STANDBY, /* The power status is standby */
    POWER_STATUS_SUSPEND, /* The power status is suspend */
    POWER_STATUS_OFF,     /* The power status is off */
    POWER_STATUS_BUTT
};

struct BlkDesc {
    uint32_t type;
    uint32_t minLevel;
    uint32_t maxLevel;
    uint32_t defLevel;
};

struct PwmCfg {
    uint32_t dev;
    uint32_t period;
};

struct PanelInfo {
    uint32_t width;
    uint32_t height;
    uint32_t hbp;
    uint32_t hfp;
    uint32_t hsw;
    uint32_t vbp;
    uint32_t vfp;
    uint32_t vsw;
    uint32_t frameRate;
    enum LcdIntfType intfType;
    enum IntfSync intfSync;
    struct MipiDsiDesc mipi;
    struct BlkDesc blk;
    struct PwmCfg pwm;
};

struct PanelStatus {
    enum PowerStatus powerStatus;
    uint32_t currLevel;
};

struct PanelData;
struct PanelEsd {
    bool support;
    uint32_t interval;
    uint32_t state;
    uint32_t recoveryNum;
    uint32_t cmpMode;
    int32_t (*checkFunc)(struct PanelData *panel);
    void *expect_data;
};

struct PanelData {
    struct HdfDeviceObject *object;
    int32_t (*init)(struct PanelData *panel);
    int32_t (*on)(struct PanelData *panel);
    int32_t (*off)(struct PanelData *panel);
    int32_t (*setBacklight)(struct PanelData *panel, uint32_t level);
    struct PanelInfo *info;
    struct PanelStatus status;
    struct PanelEsd *esd;
};

struct PanelManager {
    struct PanelData *panel[PANEL_MAX];
    uint32_t panelNum;
};

int32_t RegisterPanel(struct PanelData *data);
struct PanelManager *GetPanelManager(void);
struct PanelData *GetPanel(int32_t index);

int32_t GetBitsPerPixel(enum DsiOutFormat format);
uint32_t CalcPixelClk(struct PanelInfo *info);
uint32_t CalcDataRate(struct PanelInfo *info);

#endif /* LCD_ABS_IF_H */
