/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "uart_dev_sample.h"
#include "fs/fs.h"
#include "securec.h"
#include "user_copy.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "uart_pl011_sample.h"

#define HDF_LOG_TAG uart_dev_sample
#define HDF_UART_FS_MODE 0660

static int32_t UartSampleDevOpen(struct file *filep)
{
    struct UartHost *host = NULL;

    if (filep == NULL || filep->f_vnode == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct drv_data *drv = (struct drv_data *)filep->f_vnode->data;
    host = (struct UartHost *)drv->priv;
    if (host == NULL) {
        HDF_LOGE("%s: host is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGI("%s: open uart%d success", __func__, host->num);
    return HDF_SUCCESS;
}
static int32_t UartSampleRelease(struct file *filep)
{
    struct UartHost *host = NULL;

    if (filep == NULL || filep->f_vnode == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct drv_data *drv = (struct drv_data *)filep->f_vnode->data;
    host = (struct UartHost *)drv->priv;
    if (host == NULL) {
        HDF_LOGE("%s: host is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGI("%s: close uart%d success", __func__, host->num);
    return HDF_SUCCESS;
}

static ssize_t UartSampleRead(struct file *filep, char *buf, size_t count)
{
    int32_t ret;
    uint8_t *tmpBuf = NULL;
    struct UartHost *host = NULL;

    if (filep == NULL || filep->f_vnode == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct drv_data *drv = (struct drv_data *)filep->f_vnode->data;
    host = (struct UartHost *)drv->priv;

    if (LOS_IsUserAddressRange((vaddr_t)(uintptr_t)buf, count)) {
        tmpBuf = (uint8_t *)OsalMemCalloc(count);
        if (tmpBuf == NULL) {
            HDF_LOGE("%s: OsalMemCalloc error", __func__);
            return HDF_ERR_MALLOC_FAIL;
        }
        ret = UartHostRead(host, tmpBuf, count);
        if (ret == HDF_SUCCESS) {
            ret = LOS_ArchCopyToUser(buf, tmpBuf, count);
        }
        OsalMemFree(tmpBuf);
        return ret;
    } else {
        return UartHostRead(host, (uint8_t *)buf, count);
    }
}

static ssize_t UartSampleWrite(struct file *filep, const char *buf, size_t count)
{
    int32_t ret;
    uint8_t *tmpBuf = NULL;
    struct UartHost *host = NULL;

    if (filep == NULL || filep->f_vnode == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct drv_data *drv = (struct drv_data *)filep->f_vnode->data;
    host = (struct UartHost *)drv->priv;

    if (LOS_IsUserAddressRange((vaddr_t)(uintptr_t)buf, count)) {
        tmpBuf = (uint8_t *)OsalMemCalloc(count);
        if (tmpBuf == NULL) {
            HDF_LOGE("%s: OsalMemCalloc error", __func__);
            return HDF_ERR_MALLOC_FAIL;
        }
        ret = LOS_ArchCopyFromUser(tmpBuf, buf, count);
        if (ret != LOS_OK) {
            OsalMemFree(tmpBuf);
            return ret;
        }
        ret = UartHostWrite(host, tmpBuf, count);
        OsalMemFree(tmpBuf);
        return ret;
    } else {
        return UartHostWrite(host, (uint8_t *)buf, count);
    }
}

static int32_t UartSampleDevIoctl(struct file *filep, int32_t cmd, unsigned long arg)
{
    int32_t ret;
    struct UartHost *host = NULL;
    if (filep == NULL || filep->f_vnode == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct drv_data *drv = (struct drv_data *)filep->f_vnode->data;
    host = (struct UartHost *)drv->priv;
    if (host->priv == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    
    ret = HDF_FAILURE;
    switch (cmd) {
        case UART_CFG_BAUDRATE:
            ret = UartHostSetBaud(host, arg);
            break;
        default:
            HDF_LOGE("%s cmd %d not support", __func__, cmd);
            ret = HDF_ERR_NOT_SUPPORT;
            break;
    }
    return ret;
}

const struct file_operations_vfs g_uartSampleDevFops = {
    .open   = UartSampleDevOpen,
    .close  = UartSampleRelease,
    .read   = UartSampleRead,
    .write  = UartSampleWrite,
    .ioctl  = UartSampleDevIoctl,
};

#define MAX_DEV_NAME_SIZE 32
static void AddRemoveUartDev(struct UartHost *host, bool add)
{
    int32_t ret;
    char *devName = NULL;

    if (host == NULL || host->priv == NULL) {
        HDF_LOGW("%s: invalid parameter", __func__);
        return;
    }
    devName = (char *)OsalMemCalloc(sizeof(char) * (MAX_DEV_NAME_SIZE + 1));
    if (devName == NULL) {
        HDF_LOGE("%s: OsalMemCalloc error", __func__);
        return;
    }
    ret = snprintf_s(devName, MAX_DEV_NAME_SIZE + 1, MAX_DEV_NAME_SIZE, "/dev/uartdev-%d", host->num);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        OsalMemFree(devName);
        return;
    }
    if (add) {
        if (register_driver(devName, &g_uartSampleDevFops, HDF_UART_FS_MODE, host)) {
            HDF_LOGE("%s: gen /dev/uartdev-%d fail!", __func__, host->num);
            OsalMemFree(devName);
            return;
        }
    } else {
        if (unregister_driver(devName)) {
            HDF_LOGE("%s: remove /dev/uartdev-%d fail!", __func__, host->num);
            OsalMemFree(devName);
            return;
        }
    }
    OsalMemFree(devName);
}

void AddUartDevice(struct UartHost *host)
{
    AddRemoveUartDev(host, true);
}

void RemoveUartDevice(struct UartHost *host)
{
    AddRemoveUartDev(host, false);
}
