/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_disp.h"
#include <securec.h>
#include "hdf_base.h"
#include "hdf_bl.h"
#include "hdf_log.h"
#include "osal.h"

#define OFFSET_TWO_BYTE    16
static struct DispManager *g_dispManager = NULL;
static struct PanelManager g_panelManager;
int32_t RegisterPanel(struct PanelData *panel)
{
    uint32_t panelNum;

    if (panel == NULL) {
        HDF_LOGE("%s: panel data is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (panel->info == NULL) {
        HDF_LOGE("%s panel info is null", __func__);
        return HDF_FAILURE;
    }
    if ((panel->on == NULL) || (panel->off == NULL)) {
        HDF_LOGE("%s on or off is null", __func__);
        return HDF_FAILURE;
    }
    panel->powerStatus = POWER_STATUS_OFF;
    panelNum = g_panelManager.panelNum;
    if (panelNum >= PANEL_MAX) {
        HDF_LOGE("%s registered panel up PANEL_MAX", __func__);
        return HDF_FAILURE;
    }
    g_panelManager.panel[panelNum] = panel;
    g_panelManager.panelNum++;
    HDF_LOGI("%s: register success", __func__);
    return HDF_SUCCESS;
}

struct PanelManager *GetPanelManager(void)
{
    if (g_panelManager.panelNum == 0) {
        return NULL;
    } else {
        return &g_panelManager;
    }
}

struct DispManager *GetDispManager(void)
{
    if (g_dispManager != NULL && g_dispManager->initialzed) {
        return g_dispManager;
    }
    return NULL;
}

static int32_t InitDisp(uint32_t devId);
int32_t DispOn(uint32_t devId);
int32_t DispOff(uint32_t devId);
int32_t SetDispBacklight(uint32_t devId, uint32_t level);
static int32_t GetDispInfo(uint32_t devId, struct DispInfo *info);
static int32_t SetDispPower(uint32_t devId, uint32_t powerStatus);
static void EsdCheckStartUp(struct DispEsd *esd, uint32_t devId);
static void EsdCheckEnd(struct DispEsd *esd, uint32_t devId);

struct DispOperations *GetDispOps(void)
{
    static struct DispOperations dispOps = {
        .init = InitDisp,
        .on = DispOn,
        .off = DispOff,
        .setBacklight = SetDispBacklight,
        .getDispInfo = GetDispInfo,
    };
    return &dispOps;
}

#ifdef __KERNEL__
EXPORT_SYMBOL(GetDispOps);
EXPORT_SYMBOL(DispOn);
EXPORT_SYMBOL(DispOff);
EXPORT_SYMBOL(SetDispBacklight);
#endif

static int32_t InitDisp(uint32_t devId)
{
    return 0;
}

int32_t DispOn(uint32_t devId)
{
    return SetDispPower(devId, POWER_STATUS_ON);
}

int32_t DispOff(uint32_t devId)
{
    return SetDispPower(devId, POWER_STATUS_OFF);
}

int32_t SetDispBacklight(uint32_t devId, uint32_t level)
{
    struct DispManager *disp = NULL;
    struct PanelData *panel = NULL;

    disp = GetDispManager();
    if (disp && disp->panelManager && devId < disp->panelManager->panelNum) {
        panel = disp->panelManager->panel[devId];
    }
    if ((panel == NULL) || (UpdateBrightness(panel->blDev, level) != HDF_SUCCESS)) {
        HDF_LOGE("%s:panel is null or UpdateBrightness failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s:level = %u", __func__, level);
    return HDF_SUCCESS;
}

static int32_t GetDispInfo(uint32_t devId, struct DispInfo *info)
{
    struct DispManager *disp = NULL;
    struct PanelData *panel = NULL;

    if (info == NULL) {
        HDF_LOGE("%s:info is null", __func__);
        return HDF_FAILURE;
    }
    disp = GetDispManager();
    if (disp == NULL) {
        HDF_LOGE("%s: disp is null", __func__);
        return HDF_FAILURE;
    }
    if (devId >= disp->panelManager->panelNum) {
        HDF_LOGE("%s: devId exceed registered panelNum", __func__);
        return HDF_FAILURE;
    }
    panel = disp->panelManager->panel[devId];
    info->width = panel->info->width;
    info->height = panel->info->height;
    info->hbp = panel->info->hbp;
    info->hfp = panel->info->hfp;
    info->hsw = panel->info->hsw;
    info->vbp = panel->info->vbp;
    info->vfp = panel->info->vfp;
    info->vsw = panel->info->vsw;
    info->intfType = panel->info->intfType;
    info->intfSync = panel->info->intfSync;
    info->frameRate = panel->info->frameRate;
    info->minLevel = panel->info->blk.minLevel;
    info->maxLevel = panel->info->blk.maxLevel;
    info->defLevel = panel->info->blk.defLevel;
    return HDF_SUCCESS;
}

static int32_t SetDispPower(uint32_t devId, uint32_t powerStatus)
{
    int32_t ret = HDF_FAILURE;
    struct DispManager *disp = NULL;
    struct PanelData *panel = NULL;

    disp = GetDispManager();
    if (disp == NULL) {
        HDF_LOGE("%s:disp is null", __func__);
        return HDF_FAILURE;
    }
    if (devId >= disp->panelManager->panelNum) {
        HDF_LOGE("%s:devId exceed registered panelNum", __func__);
        return HDF_FAILURE;
    }
    panel = disp->panelManager->panel[devId];
    OsalMutexLock(&disp->dispMutex);
    if (panel->powerStatus == powerStatus) {
        OsalMutexUnlock(&disp->dispMutex);
        HDF_LOGE("%s: panel already in mode = %d", __func__, powerStatus);
        return HDF_SUCCESS;
    }
    switch (powerStatus) {
        case POWER_STATUS_ON:
            ret = panel->on(panel);
            if (ret == HDF_SUCCESS) {
                panel->powerStatus = POWER_STATUS_ON;
                ret = UpdateBacklightState(panel->blDev, FB_POWER_ON);
                EsdCheckStartUp(disp->esd, devId);
            }
            break;
        case POWER_STATUS_OFF:
            ret = panel->off(panel);
            if (ret == HDF_SUCCESS) {
                panel->powerStatus = POWER_STATUS_OFF;
                ret = UpdateBacklightState(panel->blDev, FB_POWER_OFF);
                EsdCheckEnd(disp->esd, devId);
            }
            break;
        default:
            HDF_LOGE("%s: not support powerStatus: %d", __func__, powerStatus);
            break;
    }
    OsalMutexUnlock(&disp->dispMutex);
    return ret;
}

static int32_t SetPowerStatus(struct HdfDeviceObject *device, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    uint32_t para = 0;

    (void)device;
    (void)rspData;
    if (reqData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(reqData, &para)) {
        HDF_LOGE("%s: HdfSbufReadBuffer failed", __func__);
        return HDF_FAILURE;
    }
    uint32_t devId = (para >> OFFSET_TWO_BYTE) & 0xffff;
    uint32_t powerStatus = para & 0xffff;
    return SetDispPower(devId, powerStatus);
}

static int32_t GetPowerStatus(struct HdfDeviceObject *device, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    uint32_t devId = 0;
    enum PowerStatus powerStatus;
    struct DispManager *disp = NULL;
    struct PanelData *panel = NULL;

    (void)device;
    if (reqData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(reqData, &devId)) {
        HDF_LOGE("%s: HdfSbufReadBuffer failed", __func__);
        return HDF_FAILURE;
    }
    disp = GetDispManager();
    if (disp == NULL || disp->panelManager == NULL || (devId >= disp->panelManager->panelNum)) {
        HDF_LOGE("%s: get panel failed", __func__);
        return HDF_FAILURE;
    }
    OsalMutexLock(&disp->dispMutex);
    panel = disp->panelManager->panel[devId];
    powerStatus = panel->powerStatus;
    OsalMutexUnlock(&disp->dispMutex);
    if (!HdfSbufWriteUint32(rspData, powerStatus)) {
        HDF_LOGE("%s: HdfSbufWriteUint32 failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t SetBacklight(struct HdfDeviceObject *device, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t ret;

    (void)device;
    (void)rspData;
    if (reqData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t para = 0;
    if (!HdfSbufReadUint32(reqData, &para)) {
        HDF_LOGE("%s: HdfSbufReadBuffer failed", __func__);
        return HDF_FAILURE;
    }
    uint32_t devId = (para >> OFFSET_TWO_BYTE) & 0xffff;
    uint32_t level = para & 0xffff;
    ret = SetDispBacklight(devId, level);
    return ret;
}

static int32_t GetBacklight(struct HdfDeviceObject *device, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    uint32_t devId = 0;
    uint32_t currLevel;
    struct DispManager *disp = NULL;
    struct PanelData *panel = NULL;

    (void)device;
    if (reqData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(reqData, &devId)) {
        HDF_LOGE("%s: HdfSbufReadBuffer failed", __func__);
        return HDF_FAILURE;
    }
    disp = GetDispManager();
    if (disp == NULL || disp->panelManager == NULL || (devId >= disp->panelManager->panelNum)) {
        HDF_LOGE("%s: get panel failed", __func__);
        return HDF_FAILURE;
    }
    panel = disp->panelManager->panel[devId];
    if (GetCurrBrightness(panel->blDev, &currLevel) != HDF_SUCCESS) {
        HDF_LOGE("%s: GetCurrBrightness failed", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(rspData, currLevel)) {
        HDF_LOGE("%s: HdfSbufWriteUint32 failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t GetInfo(struct HdfDeviceObject *device, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    (void)device;
    (void)rspData;
    uint32_t devId = 0;
    struct DispInfo info;

    if (reqData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(reqData, &devId)) {
        HDF_LOGE("%s: HdfSbufReadBuffer failed", __func__);
        return HDF_FAILURE;
    }
    (void)memset_s(&info, sizeof(struct DispInfo), 0, sizeof(struct DispInfo));
    if (GetDispInfo(devId, &info) != HDF_SUCCESS) {
        HDF_LOGE("%s: GetDispInfo failed", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteBuffer(rspData, &info, sizeof(struct DispInfo)) != 0) {
        HDF_LOGE("%s: copy info failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

DispCmdHandle g_dispCmdHandle[] = {
    GetPowerStatus,
    GetInfo,
    SetPowerStatus,
    SetBacklight,
    GetBacklight,
};

static int32_t DispCmdProcess(struct HdfDeviceObject *device, int32_t cmd, struct HdfSBuf *reqData,
    struct HdfSBuf *rspData)
{
    int32_t cmdNum = sizeof(g_dispCmdHandle) / sizeof(g_dispCmdHandle[0]);
    if (device == NULL || reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (cmd >= cmdNum || cmd < 0) {
        HDF_LOGE("%s: invalid cmd = %d", __func__, cmd);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: cmd = %d", __func__, cmd);
    if (g_dispCmdHandle[cmd] == NULL) {
        return HDF_FAILURE;
    }

    return g_dispCmdHandle[cmd](device, reqData, rspData);
}

static int32_t HdfDispDispatch(struct HdfDeviceIoClient *client, int id, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    if (client == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    return DispCmdProcess(client->device, id, data, reply);
}

static int HdfDispBind(struct HdfDeviceObject *dev)
{
    if (dev == NULL) {
        return HDF_FAILURE;
    }
    static struct IDeviceIoService dispService = {
        .object.objectId = 1,
        .Dispatch = HdfDispDispatch,
    };
    dev->service = &dispService;
    return HDF_SUCCESS;
}

static void PanelRecovery(struct PanelData *panel)
{
    HDF_LOGI("%s enter", __func__);
    if (panel->off) {
        panel->off(panel);
    }
    OsalMSleep(150); // delay 150ms
    if (panel->on) {
        panel->on(panel);
    }
}

static void EsdTimerHandler(uintptr_t arg)
{
    uint32_t devId = (uint32_t)arg;
    struct DispManager *disp = NULL;

    disp = GetDispManager();
    if ((disp == NULL) || (disp->esd == NULL)) {
        HDF_LOGE("%s: disp or esd is null", __func__);
        return;
    }
    if (devId >= disp->esd->panelNum) {
        HDF_LOGE("%s: esd is null", __func__);
        return;
    }
    HdfAddWork(&disp->dispWorkQueue, disp->esd->work[devId]);
    HDF_LOGD("%s devId[%d] add work to wq", __func__, devId);
}

static void EsdWorkHandler(void *arg)
{
    int32_t ret = HDF_SUCCESS;
    uint32_t devId = (uint32_t)arg;
    struct PanelData *panel = NULL;
    struct DispManager *disp = NULL;

    disp = GetDispManager();
    if ((disp == NULL) || (disp->panelManager == NULL)) {
        HDF_LOGE("%s: disp or panelManager is null", __func__);
        return;
    }
    if (devId >= disp->panelManager->panelNum) {
        HDF_LOGE("%s: dispCtrl is null or panel is null", __func__);
        return;
    }
    panel = disp->panelManager->panel[devId];
    if ((panel->esd == NULL) || (panel->esd->checkFunc == NULL)) {
        HDF_LOGE("%s: esd or checkFunc is null", __func__);
        return;
    }
    ret = panel->esd->checkFunc(panel);
    if (ret != HDF_SUCCESS) {
        OsalMutexLock(&disp->dispMutex);
        if (panel->esd->state == ESD_RUNNING) {
            PanelRecovery(panel);
        } else {
            HDF_LOGI("%s: esd check has disabled", __func__);
            OsalMutexUnlock(&disp->dispMutex);
            return;
        }
        OsalMutexUnlock(&disp->dispMutex);
        panel->esd->recoveryNum++;
    }
    HDF_LOGD("%s recoveryNum = %d", __func__, panel->esd->recoveryNum);
    if (panel->esd->recoveryNum >= ESD_MAX_RECOVERY) {
        panel->esd->recoveryNum = 0;
        OsalMutexLock(&disp->dispMutex);
        if (panel->esd->state == ESD_RUNNING) {
            OsalTimerDelete(disp->esd->timer[devId]);
            panel->esd->state = ESD_READY;
            HDF_LOGI("%s disable esd check", __func__);
        } else {
            HDF_LOGI("%s: esd check has disabled", __func__);
            OsalMutexUnlock(&disp->dispMutex);
            return;
        }
        OsalMutexUnlock(&disp->dispMutex);
    }
}

static void EsdCheckStartUp(struct DispEsd *esd, uint32_t devId)
{
    if (esd == NULL) {
        HDF_LOGE("%s esd is null", __func__);
        return;
    }
    HDF_LOGD("%s enter", __func__);
    if ((esd->panelEsd[devId] != NULL) && esd->panelEsd[devId]->support) {
        if (esd->panelEsd[devId]->state == ESD_READY) {
            OsalTimerCreate(esd->timer[devId], esd->panelEsd[devId]->interval,
                EsdTimerHandler, (uintptr_t)devId);
            OsalTimerStartLoop(esd->timer[devId]);
            esd->panelEsd[devId]->state = ESD_RUNNING;
            HDF_LOGI("%s panel enable esd check", __func__);
        }
    }
}

static void EsdCheckEnd(struct DispEsd *esd, uint32_t devId)
{
    if (esd == NULL) {
        HDF_LOGE("%s esd is null", __func__);
        return;
    }
    HDF_LOGD("%s enter", __func__);
    if ((esd->panelEsd[devId] != NULL) && (esd->panelEsd[devId]->support)) {
        esd->panelEsd[devId]->recoveryNum = 0;
        if (esd->panelEsd[devId]->state == ESD_RUNNING) {
            OsalTimerDelete(esd->timer[devId]);
            esd->panelEsd[devId]->state = ESD_READY;
            HDF_LOGI("%s panel disable esd check", __func__);
        }
    }
}

static struct DispEsd *EsdResMalloc(int32_t panelNum)
{
    struct DispEsd *esd = NULL;

    esd = (struct DispEsd *)OsalMemCalloc(sizeof(struct DispEsd));
    if (esd == NULL) {
        HDF_LOGE("%s esd malloc fail", __func__);
        return NULL;
    }
    esd->panelEsd = (struct PanelEsd **)OsalMemCalloc(sizeof(struct PanelEsd *) * panelNum);
    if (esd->panelEsd == NULL) {
        HDF_LOGE("%s panelEsd malloc fail", __func__);
        goto PANEL_ESD_EXIT;
    }
    esd->work = (HdfWork **)OsalMemCalloc(sizeof(HdfWork *) * panelNum);
    if (esd->work == NULL) {
        HDF_LOGE("%s work malloc fail", __func__);
        goto WORK_EXIT;
    }
    esd->timer = (OsalTimer **)OsalMemCalloc(sizeof(OsalTimer *) * panelNum);
    if (esd->timer == NULL) {
        HDF_LOGE("%s timer malloc fail", __func__);
        goto TIMER_EXIT;
    }
    esd->workInit = (bool *)OsalMemCalloc(sizeof(bool) * panelNum);
    if (esd->workInit == NULL) {
        HDF_LOGE("%s workInit malloc fail", __func__);
        goto WORK_INIT_EXIT;
    }
    return esd;

WORK_INIT_EXIT:
    OsalMemFree(esd->timer);
TIMER_EXIT:
    OsalMemFree(esd->work);
WORK_EXIT:
    OsalMemFree(esd->panelEsd);
PANEL_ESD_EXIT:
    OsalMemFree(esd);

    return NULL;
}

static void EsdMemFree(struct DispEsd *esd)
{
    uint32_t i;

    for (i = 0; i < esd->panelNum; i++) {
        if (esd->workInit[i] == true) {
            HdfWorkDestroy(esd->work[i]);
        }
        OsalMemFree(esd->work[i]);
        OsalMemFree(esd->timer[i]);
    }
    OsalMemFree(esd->panelEsd);
    OsalMemFree(esd->work);
    OsalMemFree(esd->timer);
    OsalMemFree(esd);
}

static int32_t EsdResInit(struct DispEsd *esd, struct PanelData **panel)
{
    uint32_t i;
    int32_t ret;

    for (i = 0; i < esd->panelNum; i++) {
        if ((panel[i]->esd != NULL) && panel[i]->esd->support) {
            esd->panelEsd[i] = panel[i]->esd;
            esd->work[i] = (HdfWork *)OsalMemCalloc(sizeof(HdfWork));
            if (esd->work[i] == NULL) {
                HDF_LOGE("%s work malloc fail", __func__);
                EsdMemFree(esd);
                return HDF_FAILURE;
            }
            esd->timer[i] = (OsalTimer *)OsalMemCalloc(sizeof(OsalTimer));
            if (esd->timer[i] == NULL) {
                EsdMemFree(esd);
                HDF_LOGE("%s timer malloc fail", __func__);
                return HDF_FAILURE;
            }
            if (esd->panelEsd[i]->interval < ESD_DEFAULT_INTERVAL) {
                esd->panelEsd[i]->interval = ESD_DEFAULT_INTERVAL;
            }
            ret = HdfWorkInit(esd->work[i], EsdWorkHandler, (void *)i);
            if (ret != HDF_SUCCESS) {
                EsdMemFree(esd);
                HDF_LOGE("%s HdfWorkInit fail", __func__);
                return HDF_FAILURE;
            }
            esd->workInit[i] = true;
            esd->panelEsd[i]->state = ESD_READY;
        }
    }
    return HDF_SUCCESS;
}

static int32_t EsdCheckInit(struct DispManager *disp)
{
    uint32_t i;
    int32_t ret;
    int32_t count = 0;
    uint32_t panelNum;
    struct DispEsd *esd = NULL;
    struct PanelData **panel = NULL;

    if (disp->panelManager == NULL) {
        HDF_LOGE("%s panelManager is null", __func__);
        return HDF_FAILURE;
    }
    panel = disp->panelManager->panel;
    panelNum = disp->panelManager->panelNum;
    for (i = 0; i < panelNum; i++) {
        if ((panel[i]->esd != NULL) && panel[i]->esd->support) {
            count++;
            break;
        }
    }
    if (count == 0) {
        HDF_LOGI("%s none of panels support esd", __func__);
        return HDF_SUCCESS;
    }
    esd = EsdResMalloc(panelNum);
    if (esd == NULL) {
        HDF_LOGE("%s EsdResInit fail", __func__);
        return HDF_FAILURE;
    }
    esd->panelNum = panelNum;
    ret = EsdResInit(esd, panel);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s EsdResInit fail", __func__);
        EsdMemFree(esd);
        return HDF_FAILURE;
    }
    disp->esd = esd;
    return HDF_SUCCESS;
}

static int32_t DispManagerInit(struct PanelManager *panelManager)
{
    int32_t ret;

    g_dispManager = (struct DispManager *)OsalMemCalloc(sizeof(struct DispManager));
    if (g_dispManager == NULL) {
        HDF_LOGE("%s g_dispManager malloc fail", __func__);
        return HDF_FAILURE;
    }
    g_dispManager->panelManager = panelManager;
    ret = HdfWorkQueueInit(&g_dispManager->dispWorkQueue, "dispWQ");
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HdfWorkQueueInit fail", __func__);
        return HDF_FAILURE;
    }
    ret = EsdCheckInit(g_dispManager);
    if (ret != HDF_SUCCESS) {
        HdfWorkQueueDestroy(&g_dispManager->dispWorkQueue);
        HDF_LOGE("%s: EsdCheckInit fail", __func__);
        return HDF_FAILURE;
    }
    OsalMutexInit(&g_dispManager->dispMutex);
    g_dispManager->initialzed = true;
    return HDF_SUCCESS;
}

static int32_t HdfDispEntryInit(struct HdfDeviceObject *object)
{
    int32_t ret;
    struct PanelManager *panelManager = NULL;

    if (object == NULL) {
        HDF_LOGE("%s: object is null!", __func__);
        return HDF_FAILURE;
    }
    panelManager = GetPanelManager();
    if (panelManager == NULL) {
        HDF_LOGE("%s: panelManager is null!", __func__);
        return HDF_FAILURE;
    }
    ret = DispManagerInit(panelManager);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: DispManagerInit fail", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s success", __func__);
    return HDF_SUCCESS;
}

struct HdfDriverEntry g_dispDevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_DISP",
    .Init = HdfDispEntryInit,
    .Bind = HdfDispBind,
};

HDF_INIT(g_dispDevEntry);
