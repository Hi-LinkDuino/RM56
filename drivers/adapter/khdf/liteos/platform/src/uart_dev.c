/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fs/driver.h"
#include "securec.h"
#include "user_copy.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "uart_dev.h"

#define HDF_LOG_TAG hdf_uart_dev
#define HDF_UART_FS_MODE 0660

static int32_t UartDevOpen(struct file *filep)
{
    struct UartHost *host = NULL;

    if (filep == NULL || filep->f_vnode == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct drv_data *drv = (struct drv_data *)filep->f_vnode->data;
    host = (struct UartHost *)drv->priv;
    return UartHostRequest(host);
}
static int32_t UartDevRelease(struct file *filep)
{
    struct UartHost *host = NULL;

    if (filep == NULL || filep->f_vnode == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct drv_data *drv = (struct drv_data *)filep->f_vnode->data;
    host = (struct UartHost *)drv->priv;

    return UartHostRelease(host);
}

static ssize_t UartDevRead(struct file *filep, char *buf, size_t count)
{
    int32_t size;
    int32_t ret = LOS_OK;
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
        size = UartHostRead(host, tmpBuf, count);
        if (size > 0) {
            ret = (int32_t)LOS_ArchCopyToUser(buf, tmpBuf, size);
        }
        OsalMemFree(tmpBuf);
        return ret != LOS_OK ? ret : size;
    } else {
        return UartHostRead(host, (uint8_t *)buf, count);
    }
}

static ssize_t UartDevWrite(struct file *filep, const char *buf, size_t count)
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
        ret = (int32_t)LOS_ArchCopyFromUser(tmpBuf, buf, count);
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

static int32_t UartCfgAttr(struct UartHost *host, unsigned long arg)
{
    int32_t ret;
    struct UartAttribute attr;
    int32_t len = sizeof(struct UartAttribute);

    if (!LOS_IsUserAddressRange((vaddr_t)arg, len)) {
        ret = memcpy_s((void *)&attr, len, (void *)arg, len);
    } else {
        ret = (int32_t)LOS_ArchCopyFromUser(&attr, (void *)(uintptr_t)arg, len);
    }
    if (ret != LOS_OK) {
        return ret;
    }
    ret = UartHostSetAttribute(host, &attr);
    return ret;
}

static int32_t UartDevIoctl(struct file *filep, int32_t cmd, unsigned long arg)
{
    int32_t ret = HDF_FAILURE;
    struct UartHost *host = NULL;
    if (filep == NULL || filep->f_vnode == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct drv_data *drv = (struct drv_data *)filep->f_vnode->data;
    host = (struct UartHost *)drv->priv;

    switch (cmd) {
        case UART_CFG_BAUDRATE:
            ret = UartHostSetBaud(host, arg);
            break;
        case UART_CFG_RD_BLOCK:
            if (arg == UART_RD_BLOCK) {
                ret = UartHostSetTransMode(host, UART_MODE_RD_BLOCK);
            } else if (arg == UART_RD_NONBLOCK) {
                ret = UartHostSetTransMode(host, UART_MODE_RD_NONBLOCK);
            }
            break;
        case UART_CFG_ATTR:
            ret = UartCfgAttr(host, arg);
            break;
        case TIOCGWINSZ:
            /* Simply support ioctl(f->fd, TIOCGWINSZ, &wsz) system call, and the detailed design will be done later  */
            ret = LOS_OK;
            break;
        default:
            HDF_LOGE("%s cmd %d not support", __func__, cmd);
            ret = HDF_ERR_NOT_SUPPORT;
            break;
    }
    return ret;
}
extern void poll_wait(struct file *filp,
                      wait_queue_head_t *wait_address, poll_table *p);
static int uartdev_poll(struct file *filep, poll_table *table)
{
    struct UartHost *host = NULL;

    if (filep == NULL || filep->f_vnode == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct drv_data *drv = (struct drv_data *)filep->f_vnode->data;
    host = (struct UartHost *)drv->priv;

    return UartHostPollEvent(host, filep, table);
}

static const struct file_operations_vfs g_uartDevFops = {
    .open   = UartDevOpen,
    .close  = UartDevRelease,
    .read   = UartDevRead,
    .write  = UartDevWrite,
    .ioctl  = UartDevIoctl,
#ifndef CONFIG_DISABLE_POLL
    .poll   = uartdev_poll,
#endif

};

#define MAX_DEV_NAME_SIZE 32
static void UartAddRemoveDev(struct UartHost *host, bool add)
{
    int32_t ret;
    char *devName = NULL;

    if (host == NULL || host->priv == NULL) {
        HDF_LOGE("%s invalid parameter", __func__);
        return;
    }

    devName = (char *)OsalMemCalloc(sizeof(char) * (MAX_DEV_NAME_SIZE + 1));
    if (devName == NULL) {
        HDF_LOGE("%s: OsalMemCalloc error", __func__);
        return;
    }
    ret = snprintf_s(devName, MAX_DEV_NAME_SIZE + 1, MAX_DEV_NAME_SIZE, "/dev/uartdev-%d", host->num);
    if (ret < 0) {
        HDF_LOGE("%s snprintf_s failed", __func__);
        OsalMemFree(devName);
        return;
    }
    if (add) {
        HDF_LOGI("create /dev/uartdev-%d", host->num);
        if (register_driver(devName, &g_uartDevFops, HDF_UART_FS_MODE, host)) {
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

void UartAddDev(struct UartHost *host)
{
    UartAddRemoveDev(host, true);
}

void UartRemoveDev(struct UartHost *host)
{
    UartAddRemoveDev(host, false);
}
