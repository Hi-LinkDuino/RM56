/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "usb_test.h"
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "usb_ddk_interface.h"
#ifdef LOSCFG_DRIVERS_HDF_USB_PNP_NOTIFY
#include "usb_pnp_notify.h"
#endif

#define HDF_LOG_TAG USB_TEST_C

static struct UsbSession *g_session = NULL;
static struct AcmDevice g_deviceService = {0};
static struct AcmDevice *g_acm = &g_deviceService;
static struct UsbInterface *g_ecmDataIface = NULL;
static struct UsbInterface *g_ecmIntIface = NULL;
static UsbInterfaceHandle *g_ecmDataDevHandle = NULL;
static UsbInterfaceHandle *g_ecmIntDevHandle = NULL;
static bool g_initFlag;
static bool g_writeBufFlag = 0;
static UsbInterfaceHandle *g_acmDataDevHandle[USB_LOOP_NUM];

static void AcmReadBulk(struct UsbRequest *req)
{
    uint32_t size;
    int32_t status = req->compInfo.status;
    size = req->compInfo.actualLength;
    printf("Bulk status:%d,actualLength:%u\n", status, size);
    switch (status) {
        case 0:
            break;
        case -ECONNRESET:
        case -ENOENT:
        case -ESHUTDOWN:
            return;
        default:
            goto EXIT;
    }
EXIT:
    return;
}

static void AcmWriteBulk(struct UsbRequest *req)
{
    int32_t status;

    if (req == NULL) {
        printf("%s:%d req is NULL!", __func__, __LINE__);
        return;
    }

    status = req->compInfo.status;
    printf("Bulk Write status:%d\n", status);
    struct AcmWb *wb  = (struct AcmWb *)req->compInfo.userData;
    switch (status) {
        case 0:
            wb->use = 0;
            break;
        case -ECONNRESET:
        case -ENOENT:
        case -ESHUTDOWN:
            return;
        default:
            return;
    }

    return;
}

static void AcmWriteIsoCallback(struct UsbRequest *requestArg)
{
    struct UsbRequest *req = requestArg;
    printf("%s:%d status:%d\n", __func__, __LINE__, req->compInfo.status);
}

static int32_t AcmWriteBufAllocHandle(const struct AcmDevice *acm)
{
    int32_t i;
    struct AcmWb *wb;
    for (wb = (struct AcmWb *)&acm->wb[0], i = 0; i < ACM_NW; i++, wb++) {
        wb->buf = (uint8_t *)OsalMemCalloc(acm->writeSize);
        if (!wb->buf) {
            while (i > 0) {
                --i;
                --wb;
                OsalMemFree(wb->buf);
                wb->buf = NULL;
            }
            return -HDF_ERR_MALLOC_FAIL;
        }
        g_writeBufFlag = true;
    }

    return HDF_SUCCESS;
}

static int32_t AcmWriteBufAlloc(struct AcmDevice *acm)
{
    int32_t ret = HDF_SUCCESS;

    if (!g_writeBufFlag) {
        ret = AcmWriteBufAllocHandle(acm);
    }

    return ret;
}

static void AcmWriteBufFree(struct AcmDevice *acm)
{
    int32_t i;
    struct AcmWb *wb;
    for (wb = &acm->wb[0], i = 0; i < ACM_NW; i++, wb++) {
        if (wb->buf) {
            OsalMemFree(wb->buf);
            wb->buf = NULL;
        }
    }
    g_writeBufFlag = false;
    return;
}

static void AcmCtrlIrq(struct UsbRequest *req)
{
    if (req == NULL) {
        printf("%s:%d req is NULL!", __func__, __LINE__);
        return;
    }
    int32_t status = req->compInfo.status;
    unsigned int currentSize = req->compInfo.actualLength;
    printf("Irqstatus:%d,actualLength:%u\n", status, currentSize);
}

static struct UsbControlRequest UsbControlMsg(struct TestControlMsgData msgData)
{
    struct UsbControlRequest dr;
    dr.target = (UsbRequestTargetType)(msgData.requestType & TARGET_MASK);
    dr.reqType = (UsbControlRequestType)((msgData.requestType >> USB_TYPE_OFFSET) & REQUEST_TYPE_MASK);
    dr.directon = (UsbRequestDirection)((msgData.requestType >> USB_DIR_OFFSET) & DIRECTION_MASK);
    dr.request = msgData.request;
    dr.value = CpuToLe16(msgData.value);
    dr.index = CpuToLe16(msgData.index);
    dr.buffer = msgData.data;
    dr.length = CpuToLe16(msgData.size);
    return dr;
}

static int32_t CheckHostSdkIfInit001(void)
{
    int32_t ret;
    ret = UsbInitHostSdk(&g_session);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->session = g_session;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfExit001(void)
{
    int32_t ret;

    ret = UsbExitHostSdk(g_acm->session);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->session = NULL;
    g_session = g_acm->session;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfInit002(void)
{
    int32_t ret;

    ret = UsbInitHostSdk(NULL);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfExit002(void)
{
    int32_t ret;

    ret = UsbExitHostSdk(NULL);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfInit003(void)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < USB_LOOP_NUM; i++) {
        ret = UsbInitHostSdk(&g_session);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
        g_acm->session = g_session;
        ret = UsbExitHostSdk(g_acm->session);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
        g_acm->session = NULL;
        g_session = g_acm->session;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfInit004(void)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < USB_LOOP_NUM; i++) {
        ret = UsbInitHostSdk(NULL);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
        ret = UsbExitHostSdk(NULL);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfInit005(void)
{
    int32_t ret;

    ret = UsbInitHostSdk(&g_session);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->session = g_session;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfClaimInterface001(void)
{
    g_acm->interfaceIndex = 1U;

    g_acm->dataIface =  (struct UsbInterface *)UsbClaimInterface(g_acm->session, g_acm->busNum,
        g_acm->devAddr, g_acm->interfaceIndex);
    if (g_acm->dataIface == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfReleaseInterface001(void)
{
    int32_t ret;

    ret = UsbReleaseInterface(g_acm->dataIface);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfClaimInterface002(void)
{
    g_acm->interfaceIndex = 0U;

    g_acm->intIface =  (struct UsbInterface *)UsbClaimInterface(g_acm->session, g_acm->busNum,
        g_acm->devAddr, g_acm->interfaceIndex);
    if (g_acm->intIface == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfReleaseInterface002(void)
{
    int32_t ret;

    ret = UsbReleaseInterface(g_acm->intIface);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfClaimInterface003(void)
{
    g_acm->interfaceIndex = USB_CTRL_INTERFACE_ID;

    g_acm->ctrIface =  (struct UsbInterface *)UsbClaimInterface(g_acm->session, g_acm->busNum,
        g_acm->devAddr, g_acm->interfaceIndex);
    if (g_acm->ctrIface == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfReleaseInterface003(void)
{
    int32_t ret;

    ret = UsbReleaseInterface(g_acm->ctrIface);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfClaimInterface004(void)
{
    g_acm->interfaceIndex = 2U;

    g_acm->dataIface =  (struct UsbInterface *)UsbClaimInterface(g_acm->session, g_acm->busNum,
        g_acm->devAddr, g_acm->interfaceIndex);
    if (g_acm->dataIface == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfClaimInterface005(void)
{
    g_acm->interfaceIndex = 3U;

    g_acm->dataIface =  (struct UsbInterface *)UsbClaimInterface(g_acm->session, g_acm->busNum,
        g_acm->devAddr, g_acm->interfaceIndex);
    if (g_acm->dataIface == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfClaimInterface006(void)
{
    g_acm = &g_deviceService;
    g_acm->interfaceIndex = 3U;
    g_acm->dataIface =  (struct UsbInterface *)UsbClaimInterface(g_acm->session, g_acm->busNum,
        g_acm->devAddr, g_acm->interfaceIndex);
    if (g_acm->dataIface == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }

    g_acm->interfaceIndex = 2U;
    g_acm->intIface =  (struct UsbInterface *)UsbClaimInterface(g_acm->session, g_acm->busNum,
        g_acm->devAddr, g_acm->interfaceIndex);
    if (g_acm->intIface == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }

    g_acm->interfaceIndex = 0U;
    g_ecmIntIface =  (struct UsbInterface *)UsbClaimInterface(g_acm->session, g_acm->busNum,
        g_acm->devAddr, g_acm->interfaceIndex);
    if (g_ecmIntIface == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }

    g_acm->interfaceIndex = 1U;
    g_ecmDataIface =  (struct UsbInterface *)UsbClaimInterface(g_acm->session, g_acm->busNum,
        g_acm->devAddr, g_acm->interfaceIndex);
    if (g_ecmDataIface == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }

    g_acm->interfaceIndex = USB_CTRL_INTERFACE_ID;
    g_acm->ctrIface =  (struct UsbInterface *)UsbClaimInterface(g_acm->session, g_acm->busNum,
        g_acm->devAddr, g_acm->interfaceIndex);
    if (g_acm->ctrIface == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfOpenInterface001(void)
{
    g_acm->data_devHandle = UsbOpenInterface(g_acm->dataIface);
    if (g_acm->data_devHandle == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfCloseInterface001(void)
{
    int32_t ret;

    ret = UsbCloseInterface(g_acm->data_devHandle);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfOpenInterface002(void)
{
    g_acm->int_devHandle = UsbOpenInterface(g_acm->intIface);
    if (g_acm->int_devHandle == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfCloseInterface002(void)
{
    int32_t ret;

    ret = UsbCloseInterface(g_acm->int_devHandle);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfOpenInterface003(void)
{
    g_acm->ctrl_devHandle = UsbOpenInterface(g_acm->ctrIface);
    if (g_acm->ctrl_devHandle == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfCloseInterface003(void)
{
    int32_t ret;
    ret = UsbCloseInterface(g_acm->ctrl_devHandle);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfOpenInterface004(void)
{
    g_acm->data_devHandle = UsbOpenInterface(NULL);
    if (g_acm->data_devHandle == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_FAILURE;
}

static int32_t CheckHostSdkIfOpenInterface005(void)
{
    int32_t i;
    for (i = 0; i < USB_LOOP_NUM; i++) {
        g_acmDataDevHandle[i] = UsbOpenInterface(g_acm->dataIface);
        if (g_acmDataDevHandle[i] == NULL) {
            HDF_LOGE("%s:%d i=%d error", __func__, __LINE__, i);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfCloseInterface005(void)
{
    int32_t i;
    int32_t ret;
    for (i = 0; i < USB_LOOP_NUM; i++) {
        ret = UsbCloseInterface(g_acmDataDevHandle[i]);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfOpenInterface006(void)
{
    g_acm->data_devHandle = UsbOpenInterface(g_acm->dataIface);
    if (g_acm->data_devHandle == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->int_devHandle = UsbOpenInterface(g_acm->intIface);
    if (g_acm->int_devHandle == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->ctrl_devHandle = UsbOpenInterface(g_acm->ctrIface);
    if (g_acm->ctrl_devHandle == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_ecmDataDevHandle = UsbOpenInterface(g_ecmDataIface);
    if (g_ecmDataDevHandle == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_ecmIntDevHandle = UsbOpenInterface(g_ecmIntIface);
    if (g_ecmIntDevHandle == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfSelectInterfaceSetting001(void)
{
    int32_t ret;
    int32_t settingIndex = 0;
    ret = UsbSelectInterfaceSetting(g_ecmDataDevHandle, settingIndex, &g_ecmDataIface);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfSelectInterfaceSetting002(void)
{
    int32_t ret;
    int32_t settingIndex = 10;
    ret = UsbSelectInterfaceSetting(g_ecmDataDevHandle, settingIndex, &g_ecmDataIface);
    if (ret != HDF_FAILURE) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfSelectInterfaceSetting003(void)
{
    int32_t ret;
    int32_t settingIndex = 100;
    ret = UsbSelectInterfaceSetting(g_ecmDataDevHandle, settingIndex, &g_ecmDataIface);
    if (ret != HDF_FAILURE) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfSelectInterfaceSetting004(void)
{
    int32_t ret;
    int32_t settingIndex = 200;
    ret = UsbSelectInterfaceSetting(g_ecmDataDevHandle, settingIndex, &g_ecmDataIface);
    if (ret != HDF_FAILURE) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfSelectInterfaceSetting005(void)
{
    int32_t ret;
    int32_t settingIndex = USB_MAX_BYTE;
    ret = UsbSelectInterfaceSetting(g_ecmDataDevHandle, settingIndex, &g_ecmDataIface);
    if (ret != HDF_FAILURE) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfSelectInterfaceSetting006(void)
{
    int32_t ret;
    int32_t settingIndex = 1;
    ret = UsbSelectInterfaceSetting(g_ecmDataDevHandle, settingIndex, &g_ecmDataIface);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfClaimInterface007(void)
{
    g_acm->interfaceIndex = 0U;

    g_ecmIntIface =  (struct UsbInterface *)UsbClaimInterface(g_acm->session, g_acm->busNum,
        g_acm->devAddr, g_acm->interfaceIndex);
    if (g_ecmIntIface == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfClaimInterface008(void)
{
    g_acm->interfaceIndex = 1U;
    g_ecmDataIface =  (struct UsbInterface *)UsbClaimInterface(g_acm->session, g_acm->busNum,
        g_acm->devAddr, g_acm->interfaceIndex);
    if (g_ecmDataIface == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfGetPipe001(void)
{
    int32_t ret;
    struct UsbPipeInfo p;
    uint8_t i;
    for (i = 0;  i <= g_acm->dataIface->info.pipeNum; i++) {
        ret = UsbGetPipeInfo(NULL, g_acm->dataIface->info.curAltSetting, i, &p);
        if (ret < 0) {
            continue;
        }
        if ((p.pipeDirection == USB_PIPE_DIRECTION_IN) && (p.pipeType == USB_PIPE_TYPE_BULK)) {
            struct UsbPipeInfo *pi = (struct UsbPipeInfo *)OsalMemCalloc(sizeof(struct UsbPipeInfo));
            if (pi == NULL) {
                HDF_LOGE("%s: error", __func__);
                return HDF_FAILURE;
            }
            p.interfaceId = g_acm->dataIface->info.interfaceIndex;
            *pi = p;
            g_acm->dataInPipe = pi;
            break;
        }
    }
    if (i > g_acm->dataIface->info.pipeNum) {
        return HDF_FAILURE;
    }
    if (g_acm->dataInPipe == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfGetPipe002(void)
{
    int32_t ret;
    struct UsbPipeInfo p;
    uint8_t i;
    for (i = 0;  i <= g_acm->dataIface->info.pipeNum; i++) {
        ret = UsbGetPipeInfo(g_acm->data_devHandle, g_acm->dataIface->info.curAltSetting, i, &p);
        if (ret < 0) {
            continue;
        }
        if ((p.pipeDirection == USB_PIPE_DIRECTION_IN) && (p.pipeType == USB_PIPE_TYPE_BULK)) {
            struct UsbPipeInfo *pi = (struct UsbPipeInfo *)OsalMemCalloc(sizeof(struct UsbPipeInfo));
            if (pi == NULL) {
                return HDF_FAILURE;
            }
            p.interfaceId = g_acm->dataIface->info.interfaceIndex;
            *pi = p;
            g_acm->dataInPipe = pi;
            break;
        }
    }
    if (g_acm->dataInPipe == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfGetPipe003(void)
{
    int32_t ret;
    struct UsbPipeInfo p;
    uint8_t i;
    for (i = 0;  i <= g_acm->dataIface->info.pipeNum; i++) {
        ret = UsbGetPipeInfo(NULL, g_acm->dataIface->info.curAltSetting, i, &p);
        if (ret < 0) {
            continue;
        }
        if ((p.pipeDirection == USB_PIPE_DIRECTION_OUT) && (p.pipeType == USB_PIPE_TYPE_BULK)) {
            struct UsbPipeInfo *pi = (struct UsbPipeInfo *)OsalMemCalloc(sizeof(struct UsbPipeInfo));
            if (pi == NULL) {
                HDF_LOGE("%s: error", __func__);
                return HDF_FAILURE;
            }
            p.interfaceId = g_acm->dataIface->info.interfaceIndex;
            *pi = p;
            g_acm->dataOutPipe = pi;
            break;
        }
    }
    if (i > g_acm->dataIface->info.pipeNum) {
        return HDF_FAILURE;
    }
    if (g_acm->dataOutPipe == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfGetPipe004(void)
{
    int32_t ret;
    struct UsbPipeInfo p;
    uint8_t i;
    for (i = 0;  i <= g_acm->dataIface->info.pipeNum; i++) {
        ret = UsbGetPipeInfo(g_acm->data_devHandle, g_acm->dataIface->info.curAltSetting, i, &p);
        if (ret < 0) {
            continue;
        }
        if ((p.pipeDirection == USB_PIPE_DIRECTION_OUT) && (p.pipeType == USB_PIPE_TYPE_BULK)) {
            struct UsbPipeInfo *pi = (struct UsbPipeInfo *)OsalMemCalloc(sizeof(struct UsbPipeInfo));
            if (pi == NULL) {
                return HDF_FAILURE;
            }
            p.interfaceId = g_acm->dataIface->info.interfaceIndex;
            *pi = p;
            g_acm->dataOutPipe = pi;
            break;
        }
    }
    if (g_acm->dataOutPipe == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfGetPipe005(void)
{
    int32_t ret;
    struct UsbPipeInfo p;
    uint8_t i;
    for (i = 0;  i <= g_acm->intIface->info.pipeNum; i++) {
        ret = UsbGetPipeInfo(NULL, g_acm->intIface->info.curAltSetting, i, &p);
        if (ret < 0) {
            continue;
        }
        if ((p.pipeDirection == USB_PIPE_DIRECTION_IN) && (p.pipeType == USB_PIPE_TYPE_INTERRUPT)) {
            struct UsbPipeInfo *pi = (struct UsbPipeInfo *)OsalMemCalloc(sizeof(struct UsbPipeInfo));
            if (pi == NULL) {
                return HDF_FAILURE;
            }
            p.interfaceId = g_acm->intIface->info.interfaceIndex;
            *pi = p;
            g_acm->intPipe = pi;
            break;
        }
    }
    if (i > g_acm->intIface->info.pipeNum) {
        return HDF_FAILURE;
    }
    if (g_acm->intPipe == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfGetPipe006(void)
{
    int32_t ret;
    struct UsbPipeInfo p;
    uint8_t i;
    for (i = 0;  i <= g_acm->intIface->info.pipeNum; i++) {
        ret = UsbGetPipeInfo(g_acm->int_devHandle, g_acm->intIface->info.curAltSetting, i, &p);
        if (ret < 0) {
            continue;
        }
        if ((p.pipeDirection == USB_PIPE_DIRECTION_IN) && (p.pipeType == USB_PIPE_TYPE_INTERRUPT)) {
            struct UsbPipeInfo *pi = (struct UsbPipeInfo *)OsalMemCalloc(sizeof(struct UsbPipeInfo));
            if (pi == NULL) {
                return HDF_FAILURE;
            }
            p.interfaceId = g_acm->intIface->info.interfaceIndex;
            *pi = p;
            g_acm->intPipe = pi;
            break;
        }
    }
    if (g_acm->intPipe == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfGetPipe007(void)
{
    int32_t ret;
    struct UsbPipeInfo p;
    uint8_t i;
    g_acm->interfaceIndex = USB_CTRL_INTERFACE_ID;
    for (i = 0;  i <= g_acm->ctrIface->info.pipeNum; i++) {
        ret = UsbGetPipeInfo(NULL, g_acm->ctrIface->info.curAltSetting, i, &p);
        if (ret < 0) {
            continue;
        }
        if ((p.pipeDirection == USB_PIPE_DIRECTION_OUT) && (p.pipeType == USB_PIPE_TYPE_CONTROL)) {
            struct UsbPipeInfo *pi = (struct UsbPipeInfo *)OsalMemCalloc(sizeof(struct UsbPipeInfo));
            if (pi == NULL) {
                return HDF_FAILURE;
            }
            p.interfaceId = g_acm->interfaceIndex;
            *pi = p;
            g_acm->ctrPipe = pi;
            break;
        }
    }
    if (i > g_acm->ctrIface->info.pipeNum) {
        return HDF_FAILURE;
    }
    if (g_acm->ctrPipe == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfGetPipe008(void)
{
    int32_t ret;
    struct UsbPipeInfo p;
    uint8_t i;
    g_acm->interfaceIndex = USB_CTRL_INTERFACE_ID;
    for (i = 0;  i <= g_acm->ctrIface->info.pipeNum; i++) {
        ret = UsbGetPipeInfo(g_acm->ctrl_devHandle, g_acm->ctrIface->info.curAltSetting, i, &p);
        if (ret < 0) {
            continue;
        }
        if ((p.pipeDirection == USB_PIPE_DIRECTION_OUT) && (p.pipeType == USB_PIPE_TYPE_CONTROL)) {
            struct UsbPipeInfo *pi = (struct UsbPipeInfo *)OsalMemCalloc(sizeof(struct UsbPipeInfo));
            if (pi == NULL) {
                return HDF_FAILURE;
            }
            p.interfaceId = g_acm->interfaceIndex;
            *pi = p;
            g_acm->ctrPipe = pi;
            break;
        }
    }
    if (g_acm->ctrPipe == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfAllocRequest001(void)
{
    int32_t i;
    g_acm->readSize = g_acm->dataInPipe->maxPacketSize;
    printf("------readSize = [%d]------\n", g_acm->readSize);
    for (i = 0; i < ACM_NR; i++) {
        g_acm->readReq[i] = UsbAllocRequest(NULL, 0, g_acm->readSize);
        if (g_acm->readReq[i] == NULL) {
            HDF_LOGE("%s: error", __func__);
            return HDF_SUCCESS;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_FAILURE;
}

static int32_t CheckHostSdkIfAllocRequest002(void)
{
    int32_t i;
    g_acm->readSize = g_acm->dataInPipe->maxPacketSize;
    printf("------readSize = [%d]------\n", g_acm->readSize);
    for (i = 0; i < ACM_NR; i++) {
        g_acm->readReq[i] = UsbAllocRequest(g_acm->data_devHandle, 0, g_acm->readSize);
        if (g_acm->readReq[i] == NULL) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFreeRequest001(void)
{
    int32_t ret;
    int32_t i;
    for (i = 0; i < ACM_NR; i++) {
        ret = UsbFreeRequest(g_acm->readReq[i]);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
        g_acm->readReq[i] = NULL;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfAllocRequest003(void)
{
    int32_t i;
    int32_t ret;
    g_acm->writeSize = g_acm->dataOutPipe->maxPacketSize;
    ret = AcmWriteBufAlloc(g_acm);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    for (i = 0; i < ACM_NW; i++) {
        g_acm->wb[i].request = UsbAllocRequest(NULL, 0, g_acm->writeSize);
        g_acm->wb[i].instance = g_acm;
        if (g_acm->wb[i].request == NULL) {
            HDF_LOGE("%s: error", __func__);
            return HDF_SUCCESS;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_FAILURE;
}

static int32_t CheckHostSdkIfAllocRequest004(void)
{
    int32_t i;
    int32_t ret;
    g_acm->writeSize = g_acm->dataOutPipe->maxPacketSize;
    ret = AcmWriteBufAlloc(g_acm);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }

    for (i = 0; i < ACM_NW; i++) {
        g_acm->wb[i].request = UsbAllocRequest(g_acm->data_devHandle, 0, g_acm->writeSize);
        g_acm->wb[i].instance = g_acm;
        if (g_acm->wb[i].request == NULL) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFreeRequest002(void)
{
    int32_t ret;
    int32_t i;
    for (i = 0; i < ACM_NR; i++) {
        ret = UsbFreeRequest(g_acm->wb[i].request);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
        g_acm->wb[i].request = NULL;
    }
    AcmWriteBufFree(g_acm);
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfAllocRequest005(void)
{
    g_acm->intSize = g_acm->intPipe->maxPacketSize;
    g_acm->notifyReq = UsbAllocRequest(NULL, 0, g_acm->intSize);
    if (g_acm->notifyReq == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_FAILURE;
}

static int32_t CheckHostSdkIfAllocRequest006(void)
{
    g_acm->intSize = g_acm->intPipe->maxPacketSize;
    g_acm->notifyReq = UsbAllocRequest(g_acm->int_devHandle, 0, g_acm->intSize);
    if (g_acm->notifyReq == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFreeRequest003(void)
{
    int32_t ret;

    ret = UsbFreeRequest(g_acm->notifyReq);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->notifyReq = NULL;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfAllocRequest007(void)
{
    g_acm->ctrlSize = sizeof (struct UsbCdcLineCoding);
    g_acm->ctrlReq = UsbAllocRequest(NULL, 0, g_acm->ctrlSize);
    if (g_acm->ctrlReq == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_FAILURE;
}

static int32_t CheckHostSdkIfAllocRequest008(void)
{
    g_acm->ctrlSize = sizeof (struct UsbCdcLineCoding);
    g_acm->ctrlReq = UsbAllocRequest(g_acm->ctrl_devHandle, 0, g_acm->ctrlSize);
    if (g_acm->ctrlReq == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfAllocRequest010(void)
{
    size_t size = g_acm->dataOutPipe->maxPacketSize;
    g_acm->isoReq = UsbAllocRequest(NULL, USB_ISO_PACKAT_CNT, size);
    if (g_acm->isoReq == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}
static int32_t CheckHostSdkIfAllocRequest011(void)
{
    size_t size = g_acm->dataOutPipe->maxPacketSize;
    g_acm->isoReq = UsbAllocRequest(g_acm->iso_devHandle, USB_ISO_PACKAT_CNT, size);
    if (g_acm->isoReq == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFreeRequest006(void)
{
    int32_t ret;

    if (g_acm->isoReq == NULL) {
        HDF_LOGE("%s: isoReq is NULL", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFreeRequest(g_acm->isoReq);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->isoReq = NULL;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFillIsoRequest001(void)
{
    int32_t ret;
    struct UsbRequestParams parmas = {0};
    int32_t i;
    char sendData[] = {"abcde\0"};
    uint32_t size = strlen(sendData) + 1;
    if (g_acm->isoPipe == NULL) {
        return HDF_FAILURE;
    }
    for (i = 0; i < 1; i++) {
        parmas.interfaceId = g_acm->isoPipe->interfaceId;
        parmas.pipeAddress = g_acm->isoPipe->pipeAddress;
        parmas.pipeId = g_acm->isoPipe->pipeId;
        parmas.callback = AcmWriteIsoCallback;
        parmas.userData = NULL;
        parmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
        parmas.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;
        parmas.dataReq.numIsoPackets = USB_ISO_PACKAT_CNT;
        parmas.dataReq.directon = USB_REQUEST_DIR_TO_DEVICE;
        parmas.dataReq.length = (int)size;
        parmas.dataReq.buffer = (unsigned char *)sendData;
        ret = UsbFillRequest(g_acm->isoReq, g_acm->iso_devHandle, &parmas);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFillIsoRequest002(void)
{
    int32_t ret;
    struct UsbRequestParams parmas;
    int32_t i;
    char sendData[] = {"abcde\0"};
    uint32_t size = strlen(sendData) + 1;
    if (g_acm->isoPipe == NULL) {
        return HDF_FAILURE;
    }
    for (i = 0; i < 1; i++) {
        parmas.interfaceId = g_acm->isoPipe->interfaceId;
        parmas.pipeAddress = g_acm->isoPipe->pipeAddress;
        parmas.pipeId = g_acm->isoPipe->pipeId;
        parmas.callback = AcmWriteIsoCallback;
        parmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
        parmas.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;
        parmas.dataReq.numIsoPackets = USB_ISO_PACKAT_CNT;
        parmas.dataReq.directon = USB_REQUEST_DIR_TO_DEVICE;
        parmas.dataReq.length = (int)size;
        parmas.dataReq.buffer = (unsigned char *)sendData;
        ret = UsbFillRequest(NULL, g_acm->iso_devHandle, &parmas);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFillIsoRequest003(void)
{
    int32_t ret;
    struct UsbRequestParams parmas;
    int32_t i;
    char sendData[] = {"abcde\0"};
    uint32_t size = strlen(sendData) + 1;
    if (g_acm->isoPipe == NULL) {
        return HDF_FAILURE;
    }
    for (i = 0; i < 1; i++) {
        parmas.interfaceId = g_acm->isoPipe->interfaceId;
        parmas.pipeAddress = g_acm->isoPipe->pipeAddress;
        parmas.pipeId = g_acm->isoPipe->pipeId;
        parmas.callback = AcmWriteIsoCallback;
        parmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
        parmas.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;
        parmas.dataReq.numIsoPackets = USB_ISO_PACKAT_CNT;
        parmas.dataReq.directon = USB_REQUEST_DIR_TO_DEVICE;
        parmas.dataReq.length = (int)size;
        parmas.dataReq.buffer = (unsigned char *)sendData;
        ret = UsbFillRequest(g_acm->isoReq, NULL, &parmas);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFillIsoRequest004(void)
{
    int32_t ret;
    struct UsbRequestParams parmas;
    int32_t i;
    char sendData[] = {"abcde\0"};
    uint32_t size = strlen(sendData) + 1;
    if (g_acm->isoPipe == NULL) {
        return HDF_FAILURE;
    }
    for (i = 0; i < 1; i++) {
        parmas.interfaceId = g_acm->isoPipe->interfaceId;
        parmas.pipeAddress = g_acm->isoPipe->pipeAddress;
        parmas.pipeId = g_acm->isoPipe->pipeId;
        parmas.callback = AcmWriteIsoCallback;
        parmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
        parmas.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;
        parmas.dataReq.numIsoPackets = USB_ISO_PACKAT_CNT;
        parmas.dataReq.directon = USB_REQUEST_DIR_TO_DEVICE;
        parmas.dataReq.length = (int)size;
        parmas.dataReq.buffer = (unsigned char *)sendData;
        ret = UsbFillRequest(g_acm->isoReq, g_acm->iso_devHandle, NULL);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFillIsoRequest005(void)
{
    int32_t ret;
    struct UsbRequestParams parmas;
    int32_t i;
    char sendData[] = {"abcde\0"};
    uint32_t size = strlen(sendData) + 1;
    if (g_acm->isoPipe == NULL) {
        return HDF_FAILURE;
    }
    for (i = 0; i < 1; i++) {
        parmas.interfaceId = g_acm->isoPipe->interfaceId;
        parmas.pipeAddress = g_acm->isoPipe->pipeAddress;
        parmas.pipeId = g_acm->isoPipe->pipeId;
        parmas.callback = AcmWriteIsoCallback;
        parmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
        parmas.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;
        parmas.dataReq.numIsoPackets = USB_ISO_PACKAT_CNT;
        parmas.dataReq.directon = USB_REQUEST_DIR_TO_DEVICE;
        parmas.dataReq.length = (int)size;
        parmas.dataReq.buffer = (unsigned char *)sendData;
        ret = UsbFillRequest(NULL, NULL, &parmas);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFillIsoRequest006(void)
{
    int32_t ret;
    struct UsbRequestParams parmas;
    int32_t i;
    char sendData[] = {"abcde\0"};
    uint32_t size = strlen(sendData) + 1;
    if (g_acm->isoPipe == NULL) {
        return HDF_FAILURE;
    }
    for (i = 0; i < 1; i++) {
        parmas.interfaceId = g_acm->isoPipe->interfaceId;
        parmas.pipeAddress = g_acm->isoPipe->pipeAddress;
        parmas.pipeId = g_acm->isoPipe->pipeId;
        parmas.callback = AcmWriteIsoCallback;
        parmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
        parmas.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;
        parmas.dataReq.numIsoPackets = USB_ISO_PACKAT_CNT;
        parmas.dataReq.directon = USB_REQUEST_DIR_TO_DEVICE;
        parmas.dataReq.length = (int)size;
        parmas.dataReq.buffer = (unsigned char *)sendData;
        ret = UsbFillRequest(g_acm->isoReq, NULL, NULL);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFreeRequest004(void)
{
    int32_t ret;
    ret = UsbFreeRequest(g_acm->ctrlReq);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->ctrlReq = NULL;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfAllocRequest009(void)
{
    int32_t i;
    int32_t ret;
    g_acm->readSize = g_acm->dataInPipe->maxPacketSize;
    for (i = 0; i < ACM_NR; i++) {
        g_acm->readReq[i] = UsbAllocRequest(g_acm->data_devHandle, 0, g_acm->readSize);
        if (g_acm->readReq[i] == NULL) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }

    g_acm->writeSize = g_acm->dataOutPipe->maxPacketSize;
    ret = AcmWriteBufAlloc(g_acm);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    for (int32_t i = 0; i < ACM_NW; i++) {
        g_acm->wb[i].request = UsbAllocRequest(g_acm->data_devHandle, 0, g_acm->writeSize);
        g_acm->wb[i].instance = g_acm;
        if (g_acm->wb[i].request == NULL) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    g_acm->intSize = g_acm->intPipe->maxPacketSize;
    g_acm->notifyReq = UsbAllocRequest(g_acm->int_devHandle, 0, g_acm->intSize);
    if (g_acm->notifyReq == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    g_acm->ctrlSize = sizeof (struct UsbCdcLineCoding);
    g_acm->ctrlReq = UsbAllocRequest(g_acm->ctrl_devHandle, 0, g_acm->ctrlSize);
    if (g_acm->ctrlReq == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFillRequest001(void)
{
    int32_t ret;
    struct UsbRequestParams readParmas;
    int32_t i;
    for (i = 0; i < 1; i++) {
        readParmas.userData = (void *)g_acm;
        readParmas.pipeAddress = g_acm->dataInPipe->pipeAddress;
        readParmas.pipeId = g_acm->dataInPipe->pipeId;
        readParmas.interfaceId = g_acm->dataInPipe->interfaceId;
        readParmas.callback = AcmReadBulk;
        readParmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
        readParmas.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;
        readParmas.dataReq.numIsoPackets = 0;
        readParmas.dataReq.directon =
            (UsbRequestDirection)(((uint32_t)g_acm->dataInPipe->pipeDirection >> USB_DIR_OFFSET) & DIRECTION_MASK);
        readParmas.dataReq.length = (int)g_acm->readSize;
        ret = UsbFillRequest(g_acm->readReq[i], g_acm->data_devHandle, &readParmas);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFillRequest002(void)
{
    int32_t ret;
    struct UsbRequestParams parmas;
    int32_t i;
    char sendData[] = {"abcde\0"};
    uint32_t size = strlen(sendData) + 1;
    g_acm->writeSize = g_acm->dataOutPipe->maxPacketSize;
    size = (size > g_acm->writeSize) ? g_acm->writeSize : size;

    for (i = 0; i < 1; i++) {
        g_acm->wb[i].len = (int)size;
        ret = memcpy_s(g_acm->wb[i].buf, g_acm->writeSize, sendData, size);
        if (ret) {
            HDF_LOGE("%s: memcpy_s failed", __func__);
            return HDF_FAILURE;
        }
        parmas.interfaceId = g_acm->dataOutPipe->interfaceId;
        parmas.pipeAddress = g_acm->dataOutPipe->pipeAddress;
        parmas.pipeId = g_acm->dataOutPipe->pipeId;
        parmas.callback = AcmWriteBulk;
        parmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
        parmas.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;
        parmas.dataReq.numIsoPackets = 0;
        parmas.userData = (void *)&g_acm->wb[i];
        parmas.dataReq.directon = USB_REQUEST_DIR_TO_DEVICE;
        parmas.dataReq.length = g_acm->wb[i].len;
        parmas.dataReq.buffer = g_acm->wb[i].buf;
        ret = UsbFillRequest(g_acm->wb[i].request, g_acm->data_devHandle, &parmas);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFillRequest003(void)
{
    int32_t ret;
    struct UsbRequestParams intParmas;
    intParmas.userData = (void *)g_acm;
    intParmas.pipeAddress = g_acm->intPipe->pipeAddress;
    intParmas.pipeId = g_acm->intPipe->pipeId;
    intParmas.interfaceId = g_acm->intPipe->interfaceId;
    intParmas.callback = AcmCtrlIrq;
    intParmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
    intParmas.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;
    intParmas.dataReq.numIsoPackets = 0;
    intParmas.dataReq.directon = (UsbRequestDirection)(((uint32_t)g_acm->intPipe->pipeDirection >> USB_PIPE_DIR_OFFSET)
        & DIRECTION_MASK);
    intParmas.dataReq.length = (int)g_acm->intSize;
    intParmas.dataReq.buffer = NULL;
    ret = UsbFillRequest(g_acm->notifyReq, g_acm->int_devHandle, &intParmas);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFillRequest004(void)
{
    int32_t ret;
    struct UsbRequestParams parmas;
    uint16_t index = 2;
    uint16_t value = 0;
    struct TestControlMsgData msgData;
    parmas.interfaceId = USB_CTRL_INTERFACE_ID;
    parmas.pipeAddress = 0;
    parmas.pipeId = 0;
    parmas.callback = AcmCtrlIrq;
    parmas.requestType = USB_REQUEST_PARAMS_CTRL_TYPE;
    parmas.timeout = USB_CTRL_SET_TIMEOUT;
    g_acm->lineCoding.dwDTERate = CpuToLe32(DATARATE);
    g_acm->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    g_acm->lineCoding.bParityType = USB_CDC_NO_PARITY;
    g_acm->lineCoding.bDataBits = DATA_BITS_LENGTH;
    msgData.request = USB_DDK_CDC_REQ_SET_LINE_CODING;
    msgData.requestType = USB_DDK_TYPE_CLASS | USB_DDK_RECIP_INTERFACE;
    msgData.value = value;
    msgData.index = index;
    msgData.data = &g_acm->lineCoding;
    msgData.size = sizeof(struct UsbCdcLineCoding);
    parmas.ctrlReq = UsbControlMsg(msgData);
    parmas.userData = NULL;
    ret = UsbFillRequest(g_acm->ctrlReq, g_acm->ctrl_devHandle, &parmas);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfSubmitRequestSync001(void)
{
    int32_t ret;
    int32_t i;
    for (i = 0; i < 1; i++) {
        printf("------UsbSubmitRequestSync i = [%d]------\n", i);
        ret = UsbSubmitRequestSync(g_acm->readReq[i]);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfSubmitRequestSync002(void)
{
    int32_t ret;
    int32_t i;
    for (i = 0; i < 1; i++) {
        printf("------UsbSubmitRequestSync i = [%d]------\n", i);
        ret = UsbSubmitRequestSync(g_acm->wb[i].request);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfSubmitRequestSync003(void)
{
    int32_t ret;

    ret = UsbSubmitRequestSync(g_acm->ctrlReq);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfSubmitRequestSync004(void)
{
    int32_t ret;
    ret = UsbSubmitRequestSync(g_acm->notifyReq);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFillRequest005(void)
{
    int32_t ret;
    struct UsbRequestParams readParmas;
    int32_t i;
    for (i = 0; i < ACM_NR; i++) {
        readParmas.userData = (void *)g_acm;
        readParmas.pipeAddress = g_acm->dataInPipe->pipeAddress;
        readParmas.pipeId = g_acm->dataInPipe->pipeId;
        readParmas.interfaceId = g_acm->dataInPipe->interfaceId;
        readParmas.callback = AcmReadBulk;
        readParmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
        readParmas.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;
        readParmas.dataReq.numIsoPackets = 0;
        readParmas.dataReq.directon =
            (UsbRequestDirection)(((uint32_t)g_acm->dataInPipe->pipeDirection >> USB_DIR_OFFSET) & DIRECTION_MASK);
        readParmas.dataReq.length = (int)g_acm->readSize;
        ret = UsbFillRequest(g_acm->readReq[i], g_acm->data_devHandle, &readParmas);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFillRequest006(void)
{
    int32_t ret;
    struct UsbRequestParams parmas;
    int32_t i;
    char sendData[] = {"abcde\0"};
    uint32_t size = strlen(sendData) + 1;
    g_acm->writeSize = g_acm->dataOutPipe->maxPacketSize;
    size = (size > g_acm->writeSize) ? g_acm->writeSize : size;
    for (i = 0; i < ACM_NR; i++) {
        g_acm->wb[i].len = (int)size;
        ret = memcpy_s(g_acm->wb[i].buf, g_acm->writeSize, sendData, size);
        if (ret) {
            HDF_LOGE("%s: memcpy_s failed", __func__);
            return HDF_FAILURE;
        }
        parmas.interfaceId = g_acm->dataOutPipe->interfaceId;
        parmas.pipeAddress = g_acm->dataOutPipe->pipeAddress;
        parmas.pipeId = g_acm->dataOutPipe->pipeId;
        parmas.callback = AcmWriteBulk;
        parmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
        parmas.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;
        parmas.dataReq.numIsoPackets = 0;
        parmas.userData = (void *)&g_acm->wb[i];
        parmas.dataReq.length = g_acm->wb[i].len;
        parmas.dataReq.buffer = g_acm->wb[i].buf;
        ret = UsbFillRequest(g_acm->wb[i].request, g_acm->data_devHandle, &parmas);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFillRequest007(void)
{
    int32_t ret;
    struct UsbRequestParams intParmas;
    intParmas.userData = (void *)g_acm;
    intParmas.pipeAddress = g_acm->intPipe->pipeAddress;
    intParmas.pipeId = g_acm->intPipe->pipeId;
    intParmas.interfaceId = g_acm->intPipe->interfaceId;
    intParmas.callback = AcmCtrlIrq;
    intParmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
    intParmas.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;
    intParmas.dataReq.buffer = NULL;
    intParmas.dataReq.numIsoPackets = 0;
    intParmas.dataReq.directon = (UsbRequestDirection)(((uint32_t)g_acm->intPipe->pipeDirection >> USB_PIPE_DIR_OFFSET)
        & DIRECTION_MASK);
    intParmas.dataReq.length = (int)g_acm->intSize;
    ret = UsbFillRequest(g_acm->notifyReq, g_acm->int_devHandle, &intParmas);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfFillRequest008(void)
{
    int32_t ret;
    struct UsbRequestParams parmas;
    uint16_t index = 2;
    uint16_t value = 0;
    struct TestControlMsgData msgData;

    parmas.interfaceId = USB_CTRL_INTERFACE_ID;
    parmas.pipeAddress = 0;
    parmas.pipeId = 0;
    parmas.callback = AcmCtrlIrq;
    parmas.requestType = USB_REQUEST_PARAMS_CTRL_TYPE;
    parmas.timeout = USB_CTRL_SET_TIMEOUT;
    g_acm->lineCoding.dwDTERate = CpuToLe32(DATARATE);
    g_acm->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    g_acm->lineCoding.bParityType = USB_CDC_NO_PARITY;
    g_acm->lineCoding.bDataBits = DATA_BITS_LENGTH;
    msgData.request = USB_DDK_CDC_REQ_SET_LINE_CODING;
    msgData.requestType = USB_DDK_TYPE_CLASS | USB_DDK_RECIP_INTERFACE;
    msgData.value = value;
    msgData.index = index;
    msgData.data = &g_acm->lineCoding;
    msgData.size = sizeof(struct UsbCdcLineCoding);
    parmas.ctrlReq = UsbControlMsg(msgData);
    parmas.userData = NULL;
    ret = UsbFillRequest(g_acm->ctrlReq, g_acm->ctrl_devHandle, &parmas);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfSubmitRequestAsync001(void)
{
    int32_t ret;
    int32_t i;
    for (i = 0; i < 1; i++) {
        ret = UsbSubmitRequestAsync(g_acm->readReq[i]);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfCancelRequest001(void)
{
    int32_t ret;
    int32_t i = 0;
    ret = UsbCancelRequest(g_acm->readReq[i]);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfSubmitRequestAsync002(void)
{
    int32_t ret;
    int32_t i;
    for (i = 0; i < 1; i++) {
        ret = UsbSubmitRequestAsync(g_acm->wb[i].request);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfCancelRequest002(void)
{
    int32_t ret;
    int32_t i = 0;
    ret = UsbCancelRequest(g_acm->wb[i].request);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfSubmitRequestAsync003(void)
{
    int32_t ret;
    ret = UsbSubmitRequestAsync(g_acm->notifyReq);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfCancelRequest003(void)
{
    int32_t ret;
    ret = UsbCancelRequest(g_acm->notifyReq);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfSubmitRequestAsync004(void)
{
    int32_t ret;
    ret = UsbSubmitRequestAsync(g_acm->ctrlReq);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfCancelRequest004(void)
{
    int32_t ret;
    ret = UsbCancelRequest(g_acm->ctrlReq);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfClearInterfaceHalt002(void)
{
    int32_t ret;
    ret = UsbClearInterfaceHalt(g_acm->data_devHandle, g_acm->dataOutPipe->pipeAddress);
    if (ret < 0) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfClearInterfaceHalt003(void)
{
    int32_t ret;
    ret = UsbClearInterfaceHalt(g_acm->int_devHandle, g_acm->intPipe->pipeAddress);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_FAILURE;
}

static int32_t CheckHostSdkIfClearInterfaceHalt004(void)
{
    int32_t ret;
    ret = UsbClearInterfaceHalt(g_acm->ctrl_devHandle, g_acm->ctrPipe->pipeAddress);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_FAILURE;
}

static int32_t CheckHostSdkIfRemoveInterface001(void)
{
    int32_t ret;
    UsbInterfaceStatus status = USB_INTERFACE_STATUS_REMOVE;
    ret = UsbAddOrRemoveInterface(g_acm->session, g_acm->busNum, g_acm->devAddr,
        g_acm->dataIface->info.interfaceIndex, status);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfAddInterface001(void)
{
    int32_t ret;
    UsbInterfaceStatus status = USB_INTERFACE_STATUS_ADD;
    OsalSleep(1);
    ret = UsbAddOrRemoveInterface(g_acm->session, g_acm->busNum, g_acm->devAddr,
        g_acm->dataIface->info.interfaceIndex, status);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfRemoveInterface002(void)
{
    int32_t ret;
    UsbInterfaceStatus status = USB_INTERFACE_STATUS_REMOVE;
    OsalSleep(1);
    ret = UsbAddOrRemoveInterface(g_acm->session, g_acm->busNum, g_acm->devAddr,
        g_acm->intIface->info.interfaceIndex, status);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfAddInterface002(void)
{
    int32_t ret;
    UsbInterfaceStatus status = USB_INTERFACE_STATUS_ADD;

    OsalSleep(1);

    ret = UsbAddOrRemoveInterface(g_acm->session, g_acm->busNum, g_acm->devAddr,
        g_acm->intIface->info.interfaceIndex, status);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfRemoveInterface003(void)
{
    int32_t ret;
    UsbInterfaceStatus status = USB_INTERFACE_STATUS_REMOVE;

    ret = UsbAddOrRemoveInterface(g_acm->session, g_acm->busNum, g_acm->devAddr,
        g_acm->ctrIface->info.interfaceIndex, status);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfAddInterface003(void)
{
    int32_t ret;
    UsbInterfaceStatus status = USB_INTERFACE_STATUS_ADD;

    ret = UsbAddOrRemoveInterface(g_acm->session, g_acm->busNum, g_acm->devAddr,
        g_acm->ctrIface->info.interfaceIndex, status);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CheckHostSdkIfCloseInterface006(void)
{
    int32_t ret;
    ret = UsbCloseInterface(g_ecmIntDevHandle);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    ret = UsbCloseInterface(g_ecmDataDevHandle);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    ret = UsbCloseInterface(g_acm->ctrl_devHandle);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    ret = UsbCloseInterface(g_acm->int_devHandle);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    ret = UsbCloseInterface(g_acm->data_devHandle);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

struct UsbTestFunc g_usbTestFunc[] = {
    { USB_HOSTSDK_INIT_001_TEST, CheckHostSdkIfInit001},
    { Usb_HOSTSDK_EXIT_001_TEST, CheckHostSdkIfExit001},
    { USB_HOSTSDK_INIT_002_TEST, CheckHostSdkIfInit002},
    { USB_HOSTSDK_EXIT_002_TEST, CheckHostSdkIfExit002},
    { USB_HOSTSDK_INIT_003_TEST, CheckHostSdkIfInit003},
    { USB_HOSTSDK_INIT_004_TEST, CheckHostSdkIfInit004},
    { USB_HOSTSDK_INIT_005_TEST, CheckHostSdkIfInit005},
    { USB_HOSTSDK_CLAIM_INTERFACE_001_TEST, CheckHostSdkIfClaimInterface001},
    { USB_HOSTSDK_RELEASE_INTERFACE_001_TEST, CheckHostSdkIfReleaseInterface001},
    { USB_HOSTSDK_CLAIM_INTERFACE_002_TEST, CheckHostSdkIfClaimInterface002},
    { USB_HOSTSDK_RELEASE_INTERFACE_002_TEST, CheckHostSdkIfReleaseInterface002},
    { USB_HOSTSDK_CLAIM_INTERFACE_003_TEST, CheckHostSdkIfClaimInterface003},
    { USB_HOSTSDK_RELEASE_INTERFACE_003_TEST, CheckHostSdkIfReleaseInterface003},
    { USB_HOSTSDK_CLAIM_INTERFACE_004_TEST, CheckHostSdkIfClaimInterface004},
    { USB_HOSTSDK_CLAIM_INTERFACE_005_TEST, CheckHostSdkIfClaimInterface005},
    { USB_HOSTSDK_CLAIM_INTERFACE_006_TEST, CheckHostSdkIfClaimInterface006},
    { USB_HOSTSDK_OPEN_INTERFACE_001_TEST, CheckHostSdkIfOpenInterface001},
    { USB_HOSTSDK_CLOSE_INTERFACE_001_TEST, CheckHostSdkIfCloseInterface001},
    { USB_HOSTSDK_OPEN_INTERFACE_002_TEST, CheckHostSdkIfOpenInterface002},
    { USB_HOSTSDK_CLOSE_INTERFACE_002_TEST, CheckHostSdkIfCloseInterface002},
    { USB_HOSTSDK_OPEN_INTERFACE_003_TEST, CheckHostSdkIfOpenInterface003},
    { USB_HOSTSDK_CLOSE_INTERFACE_003_TEST, CheckHostSdkIfCloseInterface003},
    { USB_HOSTSDK_OPEN_INTERFACE_004_TEST, CheckHostSdkIfOpenInterface004},
    { USB_HOSTSDK_OPEN_INTERFACE_005_TEST, CheckHostSdkIfOpenInterface005},
    { USB_HOSTSDK_CLOSE_INTERFACE_005_TEST, CheckHostSdkIfCloseInterface005},
    { USB_HOSTSDK_OPEN_INTERFACE_006_TEST, CheckHostSdkIfOpenInterface006},
    { USB_HOSTSDK_SELECT_INTERFACE_001_TEST, CheckHostSdkIfSelectInterfaceSetting001},
    { USB_HOSTSDK_SELECT_INTERFACE_002_TEST, CheckHostSdkIfSelectInterfaceSetting002},
    { USB_HOSTSDK_SELECT_INTERFACE_003_TEST, CheckHostSdkIfSelectInterfaceSetting003},
    { USB_HOSTSDK_SELECT_INTERFACE_004_TEST, CheckHostSdkIfSelectInterfaceSetting004},
    { USB_HOSTSDK_SELECT_INTERFACE_005_TEST, CheckHostSdkIfSelectInterfaceSetting005},
    { USB_HOSTSDK_SELECT_INTERFACE_006_TEST, CheckHostSdkIfSelectInterfaceSetting006},
    { USB_HOSTSDK_CLAIM_INTERFACE_007_TEST, CheckHostSdkIfClaimInterface007},
    { USB_HOSTSDK_CLAIM_INTERFACE_008_TEST, CheckHostSdkIfClaimInterface008},
    { USB_HOSTSDK_GET_PIPE_001_TEST, CheckHostSdkIfGetPipe001},
    { USB_HOSTSDK_GET_PIPE_002_TEST, CheckHostSdkIfGetPipe002},
    { USB_HOSTSDK_GET_PIPE_003_TEST, CheckHostSdkIfGetPipe003},
    { USB_HOSTSDK_GET_PIPE_004_TEST, CheckHostSdkIfGetPipe004},
    { USB_HOSTSDK_GET_PIPE_005_TEST, CheckHostSdkIfGetPipe005},
    { USB_HOSTSDK_GET_PIPE_006_TEST, CheckHostSdkIfGetPipe006},
    { USB_HOSTSDK_GET_PIPE_007_TEST, CheckHostSdkIfGetPipe007},
    { USB_HOSTSDK_GET_PIPE_008_TEST, CheckHostSdkIfGetPipe008},
    { USB_HOSTSDK_ALLOC_REQUEST_001_TEST, CheckHostSdkIfAllocRequest001},
    { USB_HOSTSDK_ALLOC_REQUEST_002_TEST, CheckHostSdkIfAllocRequest002},
    { USB_HOSTSDK_FREE_REQUEST_001_TEST, CheckHostSdkIfFreeRequest001},
    { USB_HOSTSDK_ALLOC_REQUEST_003_TEST, CheckHostSdkIfAllocRequest003},
    { USB_HOSTSDK_ALLOC_REQUEST_004_TEST, CheckHostSdkIfAllocRequest004},
    { USB_HOSTSDK_FREE_REQUEST_002_TEST, CheckHostSdkIfFreeRequest002},
    { USB_HOSTSDK_ALLOC_REQUEST_005_TEST, CheckHostSdkIfAllocRequest005},
    { USB_HOSTSDK_ALLOC_REQUEST_006_TEST, CheckHostSdkIfAllocRequest006},
    { USB_HOSTSDK_FREE_REQUEST_003_TEST, CheckHostSdkIfFreeRequest003},
    { USB_HOSTSDK_ALLOC_REQUEST_007_TEST, CheckHostSdkIfAllocRequest007},
    { USB_HOSTSDK_ALLOC_REQUEST_008_TEST, CheckHostSdkIfAllocRequest008},
    { USB_HOSTSDK_ALLOC_REQUEST_010_TEST, CheckHostSdkIfAllocRequest010},
    { USB_HOSTSDK_ALLOC_REQUEST_011_TEST, CheckHostSdkIfAllocRequest011},
    { USB_HOSTSDK_FREE_REQUEST_006_TEST, CheckHostSdkIfFreeRequest006},
    { USB_HOSTSDK_FILL_ISO_REQUEST_001_TEST, CheckHostSdkIfFillIsoRequest001},
    { USB_HOSTSDK_FILL_ISO_REQUEST_002_TEST, CheckHostSdkIfFillIsoRequest002},
    { USB_HOSTSDK_FILL_ISO_REQUEST_003_TEST, CheckHostSdkIfFillIsoRequest003},
    { USB_HOSTSDK_FILL_ISO_REQUEST_004_TEST, CheckHostSdkIfFillIsoRequest004},
    { USB_HOSTSDK_FILL_ISO_REQUEST_005_TEST, CheckHostSdkIfFillIsoRequest005},
    { USB_HOSTSDK_FILL_ISO_REQUEST_006_TEST, CheckHostSdkIfFillIsoRequest006},
    { USB_HOSTSDK_FREE_REQUEST_004_TEST, CheckHostSdkIfFreeRequest004},
    { USB_HOSTSDK_ALLOC_REQUEST_009_TEST, CheckHostSdkIfAllocRequest009},
    { USB_HOSTSDK_FILL_REQUEST_001_TEST, CheckHostSdkIfFillRequest001},
    { USB_HOSTSDK_FILL_REQUEST_002_TEST, CheckHostSdkIfFillRequest002},
    { USB_HOSTSDK_FILL_REQUEST_003_TEST, CheckHostSdkIfFillRequest003},
    { USB_HOSTSDK_FILL_REQUEST_004_TEST, CheckHostSdkIfFillRequest004},
    { USB_HOSTSDK_SUBMIT_REQUEST_SYNC_001_TEST, CheckHostSdkIfSubmitRequestSync001},
    { USB_HOSTSDK_SUBMIT_REQUEST_SYNC_002_TEST, CheckHostSdkIfSubmitRequestSync002},
    { USB_HOSTSDK_SUBMIT_REQUEST_SYNC_003_TEST, CheckHostSdkIfSubmitRequestSync003},
    { USB_HOSTSDK_SUBMIT_REQUEST_SYNC_004_TEST, CheckHostSdkIfSubmitRequestSync004},
    { USB_HOSTSDK_FILL_REQUEST_005_TEST, CheckHostSdkIfFillRequest005},
    { USB_HOSTSDK_FILL_REQUEST_006_TEST, CheckHostSdkIfFillRequest006},
    { USB_HOSTSDK_FILL_REQUEST_007_TEST, CheckHostSdkIfFillRequest007},
    { USB_HOSTSDK_FILL_REQUEST_008_TEST, CheckHostSdkIfFillRequest008},
    { USB_HOSTSDK_SUBMIT_REQUEST_ASYNC_001_TEST, CheckHostSdkIfSubmitRequestAsync001},
    { USB_HOSTSDK_SUBMIT_CANCEL_REQUEST_001_TEST, CheckHostSdkIfCancelRequest001},
    { USB_HOSTSDK_SUBMIT_REQUEST_ASYNC_002_TEST, CheckHostSdkIfSubmitRequestAsync002},
    { USB_HOSTSDK_SUBMIT_CANCEL_REQUEST_002_TEST, CheckHostSdkIfCancelRequest002},
    { USB_HOSTSDK_SUBMIT_REQUEST_ASYNC_003_TEST, CheckHostSdkIfSubmitRequestAsync003},
    { USB_HOSTSDK_SUBMIT_CANCEL_REQUEST_003_TEST, CheckHostSdkIfCancelRequest003},
    { USB_HOSTSDK_SUBMIT_REQUEST_ASYNC_004_TEST, CheckHostSdkIfSubmitRequestAsync004},
    { USB_HOSTSDK_SUBMIT_CANCEL_REQUEST_004_TEST, CheckHostSdkIfCancelRequest004},
    { USB_HOSTSDK_CLEAR_INTERFACE_HALT_002_TEST, CheckHostSdkIfClearInterfaceHalt002},
    { USB_HOSTSDK_CLEAR_INTERFACE_HALT_003_TEST, CheckHostSdkIfClearInterfaceHalt003},
    { USB_HOSTSDK_CLEAR_INTERFACE_HALT_004_TEST, CheckHostSdkIfClearInterfaceHalt004},
    { USB_HOSTSDK_REMOVE_INTERFACE_001_TEST, CheckHostSdkIfRemoveInterface001},
    { USB_HOSTSDK_ADD_INTERFACE_001_TEST, CheckHostSdkIfAddInterface001},
    { USB_HOSTSDK_REMOVE_INTERFACE_002_TEST, CheckHostSdkIfRemoveInterface002},
    { USB_HOSTSDK_ADD_INTERFACE_002_TEST, CheckHostSdkIfAddInterface002},
    { USB_HOSTSDK_REMOVE_INTERFACE_003_TEST, CheckHostSdkIfRemoveInterface003},
    { USB_HOSTSDK_ADD_INTERFACE_003_TEST, CheckHostSdkIfAddInterface003},
    { USB_HOSTSDK_CLOSE_INTERFACE_006_TEST, CheckHostSdkIfCloseInterface006},
};

int32_t UsbTestEntry(int32_t cmd)
{
    int32_t i;
    int32_t ret = HDF_ERR_NOT_SUPPORT;

    if (g_initFlag == false) {
#ifdef LOSCFG_DRIVERS_HDF_USB_PNP_NOTIFY
        struct UsbGetDevicePara paraData;
        struct usb_device *usbPnpDevice = NULL;
        paraData.type = USB_PNP_DEVICE_VENDOR_PRODUCT_TYPE;
        paraData.vendorId = USB_DEVICE_VENDOR_ID;
        paraData.productId = USB_DEVICE_PRODUCT_ID;
        usbPnpDevice = UsbPnpNotifyGetUsbDevice(paraData);
        if (usbPnpDevice == NULL) {
            HDF_LOGE("%s:%d UsbPnpNotifyGetUsbDevice is NULL!", __func__, __LINE__);
            return HDF_ERR_INVALID_PARAM;
        }
        g_acm->busNum = usbPnpDevice->address;
        g_acm->devAddr = usbPnpDevice->port_no;
        HDF_LOGE("%s: busNum %d++devAddr %d", __func__, g_acm->busNum, g_acm->devAddr);
        g_initFlag = true;
#else
        HDF_LOGE("%s:%d pnp notify module is not support!", __func__, __LINE__);
        return HDF_ERR_NOT_SUPPORT;
#endif
    }
    for (i = 0; i < sizeof(g_usbTestFunc) / sizeof(g_usbTestFunc[0]); i++) {
        if (cmd == g_usbTestFunc[i].type && g_usbTestFunc[i].Func != NULL) {
            ret = g_usbTestFunc[i].Func();
            HDF_LOGE("%s: cmd %d ret %d", __func__, cmd, ret);
            break;
        }
    }
    return ret;
}
