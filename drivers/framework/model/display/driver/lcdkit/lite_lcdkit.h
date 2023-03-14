/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef LITE_LCDKIT_H
#define LITE_LCDKIT_H
#include "hdf_base.h"
#include "device_resource_if.h"
#include "osal_mem.h"
#ifdef HDF_LOG_TAG
#undef HDF_LOG_TAG
#endif
#define HDF_LOG_TAG HDF_DISP

#define DSI_CMD_HEAD          3
#define DATA_LEN              2
#define CMD_DELAY             1
#define DATA_TYPE             0
#define POWER_SETTING_SIZE    3
#define POWER_SEQUEUE_SIZE    4
#define MAX_PANEL_CFG         50

enum PowerType {
    GPIO_POWER,
    REGULATOR_POWER,
};

struct PanelCmd {
    struct DsiCmdDesc  *dsiCmd;
    uint32_t           count;
};

struct PowerDesc {
    uint32_t type;
    uint32_t num;
    uint32_t vol;
};

struct PowerCtrl {
    uint32_t type;
    uint32_t num;
    uint32_t opt;
    uint32_t delay;
};

struct PowerSequeue {
    struct PowerCtrl  *pwCtrl;
    uint32_t          count;
};

struct PowerSetting {
    struct PowerDesc  *power;
    uint32_t          count;
};

struct PanelConfig {
    const char   *actPanel;
    uint32_t     dsiDev;
    DevHandle    pwmHandle;
    DevHandle    dsiHandle;
    struct PanelInfo      info;
    struct PanelCmd       onCmd;
    struct PanelCmd       offCmd;
    struct PowerSetting   setting;
    struct PowerSequeue   onSeq;
    struct PowerSequeue   offSeq;
};

int32_t ParseLcdConfig(const struct DeviceResourceNode *node, struct PanelConfig *cfg);
#endif /* LITE_LCDKIT_H */
