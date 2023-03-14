/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "i3c_core.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_mutex.h"

#define I3C_SERVICE_NAME "HDF_PLATFORM_I3C_MANAGER"

struct I3cManager {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    struct I3cCntlr *cntlrs[I3C_CNTLR_MAX];
    struct OsalMutex lock;
};

static struct I3cManager *g_i3cManager = NULL;
static struct DListHead g_i3cDeviceList;
static OsalSpinlock g_listLock;

int I3cCheckReservedAddr(uint16_t addr)
{
    if ((addr == I3C_RESERVED_ADDR_7H00) || (addr == I3C_RESERVED_ADDR_7H01) ||
        (addr == I3C_RESERVED_ADDR_7H02) || (addr == I3C_RESERVED_ADDR_7H3E) ||
        (addr == I3C_RESERVED_ADDR_7H5E) || (addr == I3C_RESERVED_ADDR_7H6E) ||
        (addr == I3C_RESERVED_ADDR_7H76) || (addr == I3C_RESERVED_ADDR_7H78) ||
        (addr == I3C_RESERVED_ADDR_7H79) || (addr == I3C_RESERVED_ADDR_7H7A) ||
        (addr == I3C_RESERVED_ADDR_7H7B) || (addr == I3C_RESERVED_ADDR_7H7C) ||
        (addr == I3C_RESERVED_ADDR_7H7D) || (addr == I3C_RESERVED_ADDR_7H7E) ||
        (addr == I3C_RESERVED_ADDR_7H7F)) {
        return I3C_ADDR_RESERVED;
    }
    return I3C_ADDR_FREE;
}

static inline int32_t I3cCntlrLockDefault(struct I3cCntlr *cntlr)
{
    if (cntlr == NULL) {
        return HDF_ERR_DEVICE_BUSY;
    }
    return OsalSpinLock(&cntlr->lock);
}

static inline void I3cCntlrUnlockDefault(struct I3cCntlr *cntlr)
{
    if (cntlr == NULL) {
        return;
    }
    (void)OsalSpinUnlock(&cntlr->lock);
}

static const struct I3cLockMethod g_i3cLockOpsDefault = {
    .lock = I3cCntlrLockDefault,
    .unlock = I3cCntlrUnlockDefault,
};

static inline int32_t I3cCntlrLock(struct I3cCntlr *cntlr)
{
    if (cntlr->lockOps == NULL || cntlr->lockOps->lock == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    return cntlr->lockOps->lock(cntlr);
}

static inline void I3cCntlrUnlock(struct I3cCntlr *cntlr)
{
    if (cntlr->lockOps != NULL && cntlr->lockOps->unlock != NULL) {
        cntlr->lockOps->unlock(cntlr);
    }
}

static struct DListHead *I3cDeviceListGet(void)
{
    static struct DListHead *head = NULL;

    head = &g_i3cDeviceList;
    while (OsalSpinLock(&g_listLock));

    return head;
}

static void I3cDeviceListPut(void)
{
    (void)OsalSpinUnlock(&g_listLock);
}

static int32_t GetAddrStatus(struct I3cCntlr *cntlr, uint16_t addr)
{
    int32_t status;

    if (addr > I3C_ADDR_MAX) {
        HDF_LOGE("%s: The address 0x%x exceeds the maximum address", __func__, addr);
        return HDF_ERR_INVALID_PARAM;
    }

    status = ADDR_STATUS_MASK & ((cntlr->addrSlot[addr / ADDRS_PER_UINT16])   \
              >> ((addr % ADDRS_PER_UINT16) * ADDRS_STATUS_BITS));

    return status;
}

static int32_t SetAddrStatus(struct I3cCntlr *cntlr, uint16_t addr, enum I3cAddrStatus status)
{
    uint16_t temp;
    int32_t ret;
    uint16_t statusMask;

    if (addr > I3C_ADDR_MAX) {
        HDF_LOGE("%s: The address 0x%x exceeds the maximum address", __func__, addr);
        return HDF_ERR_INVALID_PARAM;
    }

    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = I3cCntlrLock(cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Lock cntlr failed!", __func__);
        return ret;
    }

    statusMask = ADDR_STATUS_MASK << ((addr % ADDRS_PER_UINT16) * ADDRS_STATUS_BITS);
    temp = (cntlr->addrSlot[addr / (uint16_t)ADDRS_PER_UINT16]) & (uint16_t)~statusMask;
    temp |= (uint16_t)(((uint16_t)status) << ((addr % ADDRS_PER_UINT16) * ADDRS_STATUS_BITS));
    cntlr->addrSlot[addr / ADDRS_PER_UINT16] = temp;

    I3cCntlrUnlock(cntlr);

    return HDF_SUCCESS;
}

static void inline I3cInitAddrStatus(struct I3cCntlr *cntlr)
{
    uint16_t addr;

    for (addr = 0; addr <= I3C_ADDR_MAX; addr++) {
        if (I3cCheckReservedAddr(addr) == I3C_ADDR_RESERVED) {
            (void)SetAddrStatus(cntlr, addr, I3C_ADDR_RESERVED);
        }
    }
}

static int32_t GetFreeAddr(struct I3cCntlr *cntlr)
{
    enum I3cAddrStatus status;
    int16_t count;
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = I3cCntlrLock(cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Lock cntlr failed!", __func__);
        return ret;
    }

    for (count = 0; count <= I3C_ADDR_MAX; count++) {
        status = (enum I3cAddrStatus)GetAddrStatus(cntlr, count);
        if (status == I3C_ADDR_FREE) {
            return (int32_t)count;
        }
    }
    I3cCntlrUnlock(cntlr);
    HDF_LOGE("%s: No free addresses left!", __func__);

    return HDF_FAILURE;
}

int32_t I3cCntlrSendCccCmd(struct I3cCntlr *cntlr, struct I3cCccCmd *ccc)
{
    int32_t ret;

    if (ccc == NULL) {
        HDF_LOGE("%s: ccc is NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (cntlr->ops == NULL || cntlr->ops->sendCccCmd == NULL) {
        HDF_LOGE("%s: ops or sendCccCmd is null", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (I3cCntlrLock(cntlr) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock controller fail!", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }

    ret = cntlr->ops->sendCccCmd(cntlr, ccc);
    I3cCntlrUnlock(cntlr);

    return ret;
}

struct I3cDevice *GetDeviceByAddr(struct I3cCntlr *cntlr, uint16_t addr)
{
    struct DListHead *head = NULL;
    struct I3cDevice *pos = NULL;
    struct I3cDevice *tmp = NULL;
    enum I3cAddrStatus addrStatus;

    if (addr > I3C_ADDR_MAX) {
        HDF_LOGE("%s: The address 0x%x exceeds the maximum address", __func__, addr);
        return NULL;
    }
    addrStatus = GetAddrStatus(cntlr, addr);
    if (addrStatus == I3C_ADDR_FREE) {
        HDF_LOGE("%s: The addr 0x%x is unavailable", __func__, addr);
        return NULL;
    }
    if (addrStatus == I3C_ADDR_RESERVED) {
        HDF_LOGE("%s: The addr 0x%x is reserved", __func__, addr);
        return NULL;
    }

    if (addrStatus == I3C_ADDR_I2C_DEVICE || addrStatus == I3C_ADDR_I3C_DEVICE) {
        head = I3cDeviceListGet();
        DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, head, struct I3cDevice, list) {
            if ((pos->dynaAddr == addr) && (pos->cntlr == cntlr)) {
                I3cDeviceListPut();
                HDF_LOGI("%s: found by dynaAddr,done!", __func__);
                return pos;
            } else if ((!pos->dynaAddr) && (pos->addr == addr) && (pos->cntlr == cntlr)) {
                HDF_LOGI("%s: found by Addr,done!", __func__);
                I3cDeviceListPut();
                return pos;
            }
        }
    }
    HDF_LOGE("%s: No such device found! addr: 0x%x", __func__, addr);

    return NULL;
}

static int32_t I3cDeviceDefineI3cDevices(struct I3cDevice *device)
{
    int32_t ret, addr;

    ret = SetAddrStatus(device->cntlr, device->addr, I3C_ADDR_I3C_DEVICE);
    if (ret != HDF_SUCCESS) {
        addr = GetFreeAddr(device->cntlr);
        if (addr <= 0) {
            HDF_LOGE("%s: No free addresses left!", __func__);
            return HDF_ERR_DEVICE_BUSY;
        }
        ret = SetAddrStatus(device->cntlr, (uint16_t)addr, I3C_ADDR_I3C_DEVICE);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: Add I3C device failed!", __func__);
            return ret;
        }
    }

    return HDF_SUCCESS;
}

int32_t I3cDeviceAdd(struct I3cDevice *device)
{
    struct DListHead *head = NULL;
    struct I3cDevice *pos = NULL;
    struct I3cDevice *tmp = NULL;
    int32_t ret;

    if ((device == NULL) || (GetAddrStatus(device->cntlr, device->addr) != I3C_ADDR_FREE)) {
        HDF_LOGE("%s: device or addr is unavailable", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (device->type == I3C_CNTLR_I2C_DEVICE || device->type == I3C_CNTLR_I2C_LEGACY_DEVICE) {
        ret = SetAddrStatus(device->cntlr, device->addr, I3C_ADDR_I2C_DEVICE);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: Add I2C device failed!", __func__);
            return ret;
        }
    } else {
        ret = I3cDeviceDefineI3cDevices(device);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: I3c DEFSLVS error!", __func__);
            return ret;
        }
    }
    head = I3cDeviceListGet();
    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, head, struct I3cDevice, list) {
        if (pos == NULL) { // empty list
            break;
        }
        if ((pos->pid == device->pid) && (pos->cntlr == device->cntlr)) {
            I3cDeviceListPut();
            HDF_LOGE("%s: device already existed!: 0x%llx", __func__, device->pid);
            (void)SetAddrStatus(device->cntlr, device->addr, I3C_ADDR_RESERVED);
            return HDF_ERR_IO;
        }
    }
    DListHeadInit(&device->list);
    DListInsertTail(&device->list, head);
    I3cDeviceListPut();
    HDF_LOGI("%s: done!", __func__);

    return HDF_SUCCESS;
}

void I3cDeviceRemove(struct I3cDevice *device)
{
    int32_t ret;
    if (device == NULL) {
        return;
    }

    ret = SetAddrStatus(device->cntlr, device->addr, I3C_ADDR_RESERVED);
    if (ret != HDF_SUCCESS) {
        return;
    }
    (void)I3cDeviceListGet();
    DListRemove(&device->list);
    I3cDeviceListPut();
}

static int32_t I3cManagerAddCntlr(struct I3cCntlr *cntlr)
{
    int32_t ret;
    struct I3cManager *manager = g_i3cManager;

    if (cntlr->busId >= I3C_CNTLR_MAX) {
        HDF_LOGE("%s: busId:%d exceed!", __func__, cntlr->busId);
        return HDF_ERR_INVALID_PARAM;
    }

    if (manager == NULL) {
        HDF_LOGE("%s: get i3c manager fail!", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock i3c manager fail!", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }

    if (manager->cntlrs[cntlr->busId] != NULL) {
        HDF_LOGE("%s: cntlr of bus:%d already exits!", __func__, cntlr->busId);
        ret = HDF_FAILURE;
    } else {
        manager->cntlrs[cntlr->busId] = cntlr;
        ret = HDF_SUCCESS;
    }
    (void)OsalMutexUnlock(&manager->lock);

    return ret;
}

static void I3cManagerRemoveCntlr(struct I3cCntlr *cntlr)
{
    struct I3cManager *manager = g_i3cManager;

    if (cntlr->busId < 0 || cntlr->busId >= I3C_CNTLR_MAX) {
        HDF_LOGE("%s: invalid busId:%d!", __func__, cntlr->busId);
        return;
    }

    if (manager == NULL) {
        HDF_LOGE("%s: get i3c manager fail!", __func__);
        return;
    }
    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock i3c manager fail!", __func__);
        return;
    }

    if (manager->cntlrs[cntlr->busId] != cntlr) {
        HDF_LOGE("%s: cntlr(%d) not in manager!", __func__, cntlr->busId);
    } else {
        manager->cntlrs[cntlr->busId] = NULL;
    }

    (void)OsalMutexUnlock(&manager->lock);
}

struct I3cCntlr *I3cCntlrGet(int16_t number)
{
    struct I3cCntlr *cntlr = NULL;
    struct I3cManager *manager = g_i3cManager;

    if (number < 0 || number >= I3C_CNTLR_MAX) {
        HDF_LOGE("%s: invalid busId:%d!", __func__, number);
        return NULL;
    }

    if (manager == NULL) {
        HDF_LOGE("%s: get i3c manager fail!", __func__);
        return NULL;
    }

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock i3c manager fail!", __func__);
        return NULL;
    }
    cntlr = manager->cntlrs[number];
    (void)OsalMutexUnlock(&manager->lock);

    return cntlr;
}

void I3cCntlrPut(struct I3cCntlr *cntlr)
{
    (void)cntlr;
}

int32_t I3cCntlrAdd(struct I3cCntlr *cntlr)
{
    int32_t ret;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->ops == NULL) {
        HDF_LOGE("%s: no ops supplied!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->lockOps == NULL) {
        HDF_LOGI("%s: use default lock methods!", __func__);
        cntlr->lockOps = &g_i3cLockOpsDefault;
    }

    if (OsalSpinInit(&cntlr->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: init lock fail!", __func__);
        return HDF_FAILURE;
    }

    I3cInitAddrStatus(cntlr);
    ret = I3cManagerAddCntlr(cntlr);
    if (ret != HDF_SUCCESS) {
        (void)OsalSpinDestroy(&cntlr->lock);
        return ret;
    }

    return HDF_SUCCESS;
}

void I3cCntlrRemove(struct I3cCntlr *cntlr)
{
    if (cntlr == NULL) {
        return;
    }
    I3cManagerRemoveCntlr(cntlr);
    (void)OsalSpinDestroy(&cntlr->lock);
}

int32_t I3cCntlrTransfer(struct I3cCntlr *cntlr, struct I3cMsg *msgs, int16_t count)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->ops == NULL || cntlr->ops->Transfer == NULL) {
        HDF_LOGE("%s: ops or i3transfer is null", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (I3cCntlrLock(cntlr) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock controller fail!", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }

    ret = cntlr->ops->Transfer(cntlr, msgs, count);
    I3cCntlrUnlock(cntlr);

    return ret;
}

int32_t I3cCntlrI2cTransfer(struct I3cCntlr *cntlr, struct I3cMsg *msgs, int16_t count)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->ops == NULL || cntlr->ops->i2cTransfer == NULL) {
        HDF_LOGE("%s: ops or i2ctransfer is null", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (I3cCntlrLock(cntlr) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock controller fail!", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }
    ret = cntlr->ops->i2cTransfer(cntlr, msgs, count);
    I3cCntlrUnlock(cntlr);

    return ret;
}

int32_t I3cCntlrSetConfig(struct I3cCntlr *cntlr, struct I3cConfig *config)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (config == NULL) {
        HDF_LOGE("%s: config is NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (cntlr->ops == NULL || cntlr->ops->setConfig == NULL) {
        HDF_LOGE("%s: ops or setConfig is NULL!", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (I3cCntlrLock(cntlr) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock controller fail!", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }

    ret = cntlr->ops->setConfig(cntlr, config);
    cntlr->config = *config;
    I3cCntlrUnlock(cntlr);

    return ret;
}

int32_t I3cCntlrGetConfig(struct I3cCntlr *cntlr, struct I3cConfig *config)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (config == NULL) {
        HDF_LOGE("%s: config is NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (cntlr->ops == NULL || cntlr->ops->getConfig == NULL) {
        HDF_LOGE("%s: ops or getConfig is NULL!", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (I3cCntlrLock(cntlr) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock controller fail!", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }

    ret = cntlr->ops->getConfig(cntlr, config);
    cntlr->config = *config;
    I3cCntlrUnlock(cntlr);

    return ret;
}

int32_t I3cCntlrRequestIbi(struct I3cCntlr *cntlr, uint16_t addr, I3cIbiFunc func, uint32_t payload)
{
    struct I3cDevice *device = NULL;
    struct I3cIbiInfo *ibi = NULL;
    uint16_t ptr;
    int32_t ret;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops == NULL || cntlr->ops->requestIbi == NULL) {
        HDF_LOGE("%s: Not support!", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if ((func == NULL) || (addr >= I3C_ADDR_MAX)) {
        HDF_LOGE("%s: invalid func or addr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    device = GetDeviceByAddr(cntlr, addr);
    if (device == NULL) {
        HDF_LOGE("%s: Get device failed!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (device->supportIbi != I3C_DEVICE_SUPPORT_IBI) {
        HDF_LOGE("%s: not support!", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (I3cCntlrLock(cntlr) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock controller fail!", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }

    for (ptr = 0; ptr < I3C_IBI_MAX; ptr++) {
        if (cntlr->ibiSlot[ptr] != NULL) {
            continue;
        }
        ibi = (struct I3cIbiInfo *)OsalMemCalloc(sizeof(*ibi));
        if (ibi == NULL) {
            HDF_LOGE("func:%s ibi is NULL!", __func__);
            return HDF_ERR_MALLOC_FAIL;
        }
        ibi->ibiFunc = func;
        ibi->payload = payload;
        ibi->data = (uint8_t *)OsalMemCalloc(sizeof(uint8_t) * payload);
        device->ibi = ibi;
        cntlr->ibiSlot[ptr] = device->ibi;
        ret = cntlr->ops->requestIbi(device);
        I3cCntlrUnlock(cntlr);
        return ret;
    }
    I3cCntlrUnlock(cntlr);

    return HDF_ERR_DEVICE_BUSY;
}

int32_t I3cCntlrFreeIbi(struct I3cCntlr *cntlr, uint16_t addr)
{
    struct I3cDevice *device = NULL;
    uint16_t ptr;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (addr >= I3C_ADDR_MAX) {
        HDF_LOGE("%s: Invalid addr: %x", __func__, addr);
        return HDF_ERR_INVALID_PARAM;
    }

    device = GetDeviceByAddr(cntlr, addr);
    if (device == NULL) {
        HDF_LOGE("%s: Get device failed!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    for (ptr = 0; ptr < I3C_IBI_MAX; ptr++) {
        if (cntlr->ibiSlot[ptr] == NULL || device->ibi == NULL) {
            break;
        }
        if (cntlr->ibiSlot[ptr] != device->ibi) {
            break;
        }
        cntlr->ibiSlot[ptr] = NULL;
        if (device->ibi->data != NULL) {
            OsalMemFree(device->ibi->data);
        }
        OsalMemFree(device->ibi);
        device->ibi = NULL;
        break;
    }

    return HDF_SUCCESS;
}

int32_t I3cCntlrIbiCallback(struct I3cDevice *device)
{
    struct I3cIbiData *ibiData = NULL;

    if (device == NULL) {
            HDF_LOGW("%s: invalid device!", __func__);
            return HDF_ERR_INVALID_PARAM;
    }

    ibiData = (struct I3cIbiData *)OsalMemCalloc(sizeof(*ibiData));
    if (ibiData == NULL) {
        HDF_LOGE("%s: Memcalloc failed", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ibiData->buf = device->ibi->data;
    ibiData->payload = device->ibi->payload;

    if (device->ibi->ibiFunc == NULL) {
        HDF_LOGW("%s: device->ibi or ibiFunc is NULL!", __func__);
        OsalMemFree(ibiData);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (device->dynaAddr != 0) {
        (void)device->ibi->ibiFunc(device->cntlr, device->dynaAddr, *ibiData);
        OsalMemFree(ibiData);
    } else {
        (void)device->ibi->ibiFunc(device->cntlr, device->addr, *ibiData);
        OsalMemFree(ibiData);
    }

    return HDF_SUCCESS;
}

static int32_t I3cManagerBind(struct HdfDeviceObject *device)
{
    (void)device;
    HDF_LOGI("%s:Enter!", __func__);
    return HDF_SUCCESS;
}

static int32_t I3cManagerInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct I3cManager *manager = NULL;

    HDF_LOGI("%s: Enter!", __func__);
    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    manager = (struct I3cManager *)OsalMemCalloc(sizeof(*manager));
    if (manager == NULL) {
        HDF_LOGE("%s: malloc manager fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = OsalMutexInit(&manager->lock);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: mutex init fail:%d", __func__, ret);
        OsalMemFree(manager);
        return HDF_FAILURE;
    }
    manager->device = device;
    g_i3cManager = manager;
    DListHeadInit(&g_i3cDeviceList);
    OsalSpinInit(&g_listLock);

    return HDF_SUCCESS;
}

static void I3cManagerRelease(struct HdfDeviceObject *device)
{
    struct I3cManager *manager = NULL;

    HDF_LOGI("%s: enter", __func__);
    if (device == NULL) {
        HDF_LOGI("%s: device is null", __func__);
        return;
    }
    manager = (struct I3cManager *)device->service;
    if (manager == NULL) {
        HDF_LOGI("%s: no service binded!", __func__);
        return;
    }
    g_i3cManager = NULL;
    OsalMemFree(manager);
}

struct HdfDriverEntry g_i3cManagerEntry = {
    .moduleVersion = 1,
    .Bind = I3cManagerBind,
    .Init = I3cManagerInit,
    .Release = I3cManagerRelease,
    .moduleName = "HDF_PLATFORM_I3C_MANAGER",
};
HDF_INIT(g_i3cManagerEntry);
