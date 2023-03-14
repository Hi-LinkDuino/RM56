/*
 * Copyright (c) 2022 Jiangsu Hoperun Software Co., Ltd.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <stdlib.h>
#include <securec.h>
#include "i2c_core.h"
#include "i2c_if.h"
#include "wm_i2c.h"
#include "wm_gpio_afsel.h"
#include "device_resource_if.h"
#include "osal_mutex.h"

#define DEC_NUM 10
#define GROUP_PIN_NUM 8
#define I2C_INVALID_ADDR 0xFFFF
#define HAL_I2C_ID_NUM 1

struct I2cResource {
    uint32_t port;
    uint32_t sclPin;
    uint32_t sdaPin;
    uint32_t speed;
};

struct I2cDevice {
    uint16_t devAddr;      /**< slave device addr */
    uint32_t addressWidth; /**< Addressing mode: 7 bit or 10 bit */
    struct OsalMutex mutex;
    uint32_t port;
    struct I2cResource resource;
};

/* HdfDriverEntry method definitions */
static int32_t i2cDriverBind(struct HdfDeviceObject *device);
static int32_t i2cDriverInit(struct HdfDeviceObject *device);
static void i2cDriverRelease(struct HdfDeviceObject *device);

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_i2cDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "W800_I2C_MODULE_HDF",
    .Bind = i2cDriverBind,
    .Init = i2cDriverInit,
    .Release = i2cDriverRelease,
};

// Initialize HdfDriverEntry
HDF_INIT(g_i2cDriverEntry);

/* I2cHostMethod method definitions */
static int32_t i2cHostTransfer(struct I2cCntlr *cntlr, struct I2cMsg *msgs, int16_t count);

struct I2cMethod g_i2cHostMethod = {
    .transfer = i2cHostTransfer,
};

int32_t InitI2cDevice(struct I2cDevice *device)
{
    int32_t ret = -1;
    uint32_t i2cPort;
    struct I2cResource *resource = NULL;
    
    if (device == NULL) {
        HDF_LOGE("device is NULL\r\n");
        return HDF_ERR_INVALID_PARAM;
    }
    
    resource = &device->resource;
    device->port = resource->port;
    i2cPort = device->port;
    if (i2cPort >= HAL_I2C_ID_NUM) {
        HDF_LOGE("i2c port %u not support\r\n", i2cPort);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (OsalMutexInit(&device->mutex) != HDF_SUCCESS) {
        HDF_LOGE("%s %d OsalMutexInit fail\r\n", __func__, __LINE__);
        return HDF_FAILURE;
    }

    if (HDF_SUCCESS != OsalMutexLock(&device->mutex)) {
        HDF_LOGE("%s %d osMutexWait fail\r\n", __func__, __LINE__);
        return HDF_ERR_TIMEOUT;
    }

    if ((resource->sclPin == WM_IO_PA_01) && (resource->sdaPin == WM_IO_PA_04)) {
        wm_i2c_scl_config(WM_IO_PA_01);
        wm_i2c_sda_config(WM_IO_PA_04);
    } else {
        HDF_LOGE("%s %d scl sda pin fail\r\n", __func__, __LINE__);
        OsalMutexUnlock(&device->mutex);
        return HDF_ERR_INVALID_PARAM;
    }

    tls_i2c_init(resource->speed);
    OsalMutexUnlock(&device->mutex);
    return HDF_SUCCESS;
}

static int32_t HostRestI2cDevice(struct I2cDevice *device)
{
    int32_t ret = -1;
    struct I2cResource *resource = NULL;
    uint32_t i2cPort;
    
    if (device == NULL) {
        HDF_LOGE("%s %d device is null\r\n", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    resource = &device->resource;
    if (resource == NULL) {
        HDF_LOGE("%s %d: invalid parameter\r\n", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }
    device->port = resource->port;
    i2cPort = device->port;
    if (i2cPort > HAL_I2C_ID_NUM) {
        HDF_LOGE("i2c port %u not support\r\n", i2cPort);
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static uint32_t GetI2cDeviceResource(struct I2cDevice *device,
                                     const struct DeviceResourceNode *resourceNode)
{
    uint32_t tempPin = 0;
    struct I2cResource *resource = NULL;
    struct DeviceResourceIface *dri = NULL;
    if (device == NULL || resourceNode == NULL) {
        HDF_LOGE("device or resourceNode is NULL\r\n");
        return HDF_ERR_INVALID_PARAM;
    }
    resource = &device->resource;
    if (resource == NULL) {
        HDF_LOGE("%s %d: invalid parameter\r\n", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }
    dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (dri == NULL || dri->GetUint32 == NULL) {
        HDF_LOGE("DeviceResourceIface is invalid\r\n");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (dri->GetUint32(resourceNode, "port", &resource->port, 0) != HDF_SUCCESS) {
        HDF_LOGE("i2c config port fail\r\n");
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "sclPin", &tempPin, 0) != HDF_SUCCESS) {
        HDF_LOGE("i2c config sclPin fail\r\n");
        return HDF_FAILURE;
    }
    resource->sclPin = ((tempPin / DEC_NUM) * GROUP_PIN_NUM) + (tempPin % DEC_NUM);

    if (dri->GetUint32(resourceNode, "sdaPin", &tempPin, 0) != HDF_SUCCESS) {
        HDF_LOGE("i2c config sdaPin fail\r\n");
        return HDF_FAILURE;
    }
    resource->sdaPin = ((tempPin / DEC_NUM) * GROUP_PIN_NUM) + (tempPin % DEC_NUM);

    if (dri->GetUint32(resourceNode, "speed", &resource->speed, 0) != HDF_SUCCESS) {
        HDF_LOGE("i2c config speed fail\r\n");
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t AttachI2cDevice(struct I2cCntlr *host, struct HdfDeviceObject *device)
{
    int32_t ret;
    struct I2cDevice *i2cDevice = NULL;
    struct I2cResource *resource = NULL;

    if (device == NULL || host == NULL) {
        HDF_LOGE("%s: device or host is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    i2cDevice = (struct I2cDevice *)OsalMemAlloc(sizeof(struct I2cDevice));
    if (i2cDevice == NULL) {
        HDF_LOGE("%s: OsalMemAlloc i2cDevice error\r\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    (void)memset_s(i2cDevice, sizeof(struct I2cDevice), 0, sizeof(struct I2cDevice));
    ret = GetI2cDeviceResource(i2cDevice, device->property);
    if (ret != HDF_SUCCESS) {
        OsalMemFree(i2cDevice);
        return HDF_FAILURE;
    }
    resource = &i2cDevice->resource;
    if (resource == NULL) {
        HDF_LOGE("%s %d: invalid parameter\r\n", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }

    host->priv = i2cDevice;
    host->busId = i2cDevice->port;

    return InitI2cDevice(i2cDevice);
}

static int32_t i2cDriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct I2cCntlr *host = NULL;
    if (device == NULL) {
        HDF_LOGE("%s: device is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    host = (struct I2cCntlr *)OsalMemAlloc(sizeof(struct I2cCntlr));
    if (host == NULL) {
        HDF_LOGE("%s: host is NULL\r\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    (void)memset_s(host, sizeof(struct I2cCntlr), 0, sizeof(struct I2cCntlr));
    host->ops = &g_i2cHostMethod;
    device->priv = (void *)host;
    ret = AttachI2cDevice(host, device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: attach error\r\n", __func__);
        i2cDriverRelease(device);
        return HDF_DEV_ERR_ATTACHDEV_FAIL;
    }
    ret = I2cCntlrAdd(host);
    if (ret != HDF_SUCCESS) {
        i2cDriverRelease(device);
        return HDF_FAILURE;
    }
    return ret;
}

static int32_t i2cDriverBind(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: I2c device object is NULL\r\n", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static void i2cDriverRelease(struct HdfDeviceObject *device)
{
    struct I2cCntlr *i2cCntrl = NULL;
    struct I2cDevice *i2cDevice = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL\r\n", __func__);
        return;
    }
    i2cCntrl = device->priv;
    if (i2cCntrl == NULL || i2cCntrl->priv == NULL) {
        HDF_LOGE("%s: i2cCntrl is NULL\r\n", __func__);
        return;
    }
    i2cCntrl->ops = NULL;
    i2cDevice = (struct I2cDevice *)i2cCntrl->priv;
    OsalMemFree(i2cCntrl);

    if (i2cDevice != NULL) {
        OsalMutexDestroy(&i2cDevice->mutex);
        OsalMemFree(i2cDevice);
    }
}

static int32_t i2c_send(struct I2cMsg *msg)
{
    uint16_t len;
    uint8_t ifack, ifstop, ifstart;

    len = msg->len;
    ifstop = 0;
    if (msg->flags & I2C_FLAG_READ) {
        ifack = msg->flags & I2C_FLAG_READ_NO_ACK ? 0 : 1;
        for (int32_t j = 0; j < len; j++) {
            if (((msg->flags & I2C_FLAG_STOP) && j) == (len - 1)) {
                ifstop = 1;
            }
            msg->buf[j] = tls_i2c_read_byte(ifack, ifstop);
        }
    } else {
        ifack = msg->flags & I2C_FLAG_IGNORE_NO_ACK ? 0 : 1;
        for (int32_t j = 0; j < len; j++) {
            if (((msg->flags & I2C_FLAG_NO_START) == 0) && (j == 0)) {
                ifstart = 1;
            }
            tls_i2c_write_byte(msg->buf[j], ifstart);
            if (ifack) {
                tls_i2c_wait_ack();
            }
        }
    }
}

static int32_t i2c_transfer(struct I2cDevice *device, struct I2cMsg *msgs, int16_t count)
{
    int ret;
    struct I2cMsg *msg = NULL;
    
    uint32_t i2cPort;
    if (device == NULL || msgs == NULL) {
        HDF_LOGE("%s: device or  msgs is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    i2cPort = (uint32_t)device->port;
    if (i2cPort > HAL_I2C_ID_NUM) {
        HDF_LOGE("i2c port %u not support\r\n", i2cPort);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (HDF_SUCCESS != OsalMutexLock(&device->mutex)) {
        HDF_LOGE("%s %d OsalMutexTimedLock fail\r\n", __func__, __LINE__);
        return HDF_ERR_TIMEOUT;
    }
    for (int32_t i = 0; i < count; i++) {
        msg = &msgs[i];
        i2c_send(msg);
    }
    OsalMutexUnlock(&device->mutex);
    return count;
}

static int32_t i2cHostTransfer(struct I2cCntlr *cntlr, struct I2cMsg *msgs, int16_t count)
{
    struct I2cDevice *device = NULL;
    if (cntlr == NULL || msgs == NULL || cntlr->priv == NULL) {
        HDF_LOGE("%s: I2cCntlr or msgs is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    device = (struct I2cDevice *)cntlr->priv;
    if (device == NULL) {
        HDF_LOGE("%s: I2cDevice is NULL\r\n", __func__);
        return HDF_DEV_ERR_NO_DEVICE;
    }

    return i2c_transfer(device, msgs, count);
}
