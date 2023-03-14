/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_DISP_H
#define HDF_DISP_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "hdf_workqueue.h"
#include "mipi_dsi_if.h"
#include "osal_mem.h"
#include "osal_mutex.h"
#include "osal_timer.h"

#ifdef HDF_LOG_TAG
#undef HDF_LOG_TAG
#endif
#define HDF_LOG_TAG HDF_DISP
#define ESD_DEFAULT_INTERVAL   5000
#define ESD_MAX_RECOVERY       10
/* support max panel number */
#define PANEL_MAX 2

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
    enum DsiMode mode;             /* output mode: DSI_VIDEO/DSI_CMD */
    enum DsiBurstMode burstMode;
    enum DsiOutFormat format;
};

enum PowerStatus {
    POWER_STATUS_ON,              /* The power status is on */
    POWER_STATUS_STANDBY,         /* The power status is standby */
    POWER_STATUS_SUSPEND,         /* The power status is suspend */
    POWER_STATUS_OFF,             /* The power status is off */
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
    uint32_t clockFreq;
    uint32_t pWidth;
    uint32_t pHeight;
    enum LcdIntfType intfType;
    uint32_t intfSync;
    struct MipiDsiDesc mipi;
    struct BlkDesc blk;
    struct PwmCfg pwm;
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
    struct PanelInfo *info;
    enum PowerStatus powerStatus;
    struct PanelEsd *esd;
    struct BacklightDev *blDev;
    void *priv;
};

struct PanelManager {
    struct PanelData *panel[PANEL_MAX];
    uint32_t panelNum;
};

typedef int32_t (*DispCmdHandle)(struct HdfDeviceObject *device, struct HdfSBuf *reqData, struct HdfSBuf *rspData);

struct DispInfo {
    uint32_t width;
    uint32_t hbp;
    uint32_t hfp;
    uint32_t hsw;
    uint32_t height;
    uint32_t vbp;
    uint32_t vfp;
    uint32_t vsw;
    uint32_t frameRate;
    uint32_t intfType;
    uint32_t intfSync;
    uint32_t minLevel;
    uint32_t maxLevel;
    uint32_t defLevel;
};

struct DispOperations {
    int32_t (*init)(uint32_t devId);
    int32_t (*on)(uint32_t devId);
    int32_t (*off)(uint32_t devId);
    int32_t (*setBacklight)(uint32_t devId, uint32_t level);
    int32_t (*getDispInfo)(uint32_t devId, struct DispInfo *info);
};

enum EsdState {
    ESD_READY = 1,
    ESD_RUNNING,
};

struct DispEsd {
    struct PanelEsd **panelEsd;
    HdfWork **work;
    bool *workInit;
    OsalTimer **timer;
    uint32_t panelNum;
};

struct DispManager {
    struct PanelManager *panelManager;
    struct OsalMutex dispMutex;
    HdfWorkQueue dispWorkQueue;
    bool initialzed;
    struct DispEsd *esd;
};

int32_t RegisterPanel(struct PanelData *data);
struct PanelManager *GetPanelManager(void);
struct DispManager *GetDispManager(void);

#endif /* HDF_DISP_H */
