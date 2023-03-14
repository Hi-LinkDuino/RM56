/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "usb_raw_test.h"
#include "data_fifo.h"
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_dlist.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "usb_ddk_interface.h"
#ifdef LOSCFG_DRIVERS_HDF_USB_PNP_NOTIFY
#include "usb_pnp_notify.h"
#endif

#define HDF_LOG_TAG usb_raw_test_c

int32_t CheckRawSdkIfGetDeviceDescriptor001(void)
{
    struct UsbDeviceDescriptor desc;
    int32_t ret;
    ret = UsbRawGetDeviceDescriptor(NULL, &desc);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfClaimInterface001(void)
{
    int32_t ret;
    int32_t interfaceNumber = 1;

    ret = UsbRawClaimInterface(NULL, interfaceNumber);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfClaimInterface005(void)
{
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = UsbParseConfigDescriptor(rawAcm, rawAcm->config);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfReleaseInterface001(void)
{
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = UsbRawReleaseInterface(NULL, rawAcm->ctrlIface);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfReleaseInterface002(void)
{
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = UsbRawReleaseInterface(rawAcm->devHandle, rawAcm->ctrlIface);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfReleaseInterface003(void)
{
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = UsbRawReleaseInterface(NULL, rawAcm->dataIface);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfReleaseInterface004(void)
{
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = UsbRawReleaseInterface(rawAcm->devHandle, rawAcm->dataIface);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfClaimInterface006(void)
{
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = UsbParseConfigDescriptor(rawAcm, rawAcm->config);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfAllocRequest001(void)
{
    int32_t i;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = AcmWriteBufAlloc(rawAcm);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }

    for (i = 0; i < ACM_NW; i++) {
        rawAcm->wb[i].request = UsbRawAllocRequest(NULL, 0, rawAcm->dataOutEp.maxPacketSize);
        rawAcm->wb[i].instance = rawAcm;
        if (rawAcm->wb[i].request) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfAllocRequest002(void)
{
    int32_t i;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = AcmWriteBufAlloc(rawAcm);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }

    for (i = 0; i < ACM_NW; i++) {
        rawAcm->wb[i].request = UsbRawAllocRequest(rawAcm->devHandle, 0, rawAcm->dataOutEp.maxPacketSize);
        rawAcm->wb[i].instance = rawAcm;
        if (rawAcm->wb[i].request == NULL) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
        ((struct UsbHostRequest *)(rawAcm->wb[i].request))->devHandle = (struct UsbDeviceHandle *)rawAcm->devHandle;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfAllocRequest003(void)
{
    int32_t i;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    for (i = 0; i < ACM_NR; i++) {
        rawAcm->readReq[i] = UsbRawAllocRequest(NULL, 0, rawAcm->dataInEp.maxPacketSize);
        if (rawAcm->readReq[i]) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfAllocRequest004(void)
{
    int32_t i;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    for (i = 0; i < ACM_NR; i++) {
        rawAcm->readReq[i] = UsbRawAllocRequest(rawAcm->devHandle, 0, rawAcm->dataInEp.maxPacketSize);
        if (rawAcm->readReq[i] == NULL) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
        ((struct UsbHostRequest *)(rawAcm->readReq[i]))->devHandle = (struct UsbDeviceHandle *)rawAcm->devHandle;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfAllocRequest005(void)
{
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    rawAcm->ctrlReq = UsbRawAllocRequest(NULL, 0, USB_CTRL_REQ_SIZE);
    if (rawAcm->ctrlReq) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfAllocRequest006(void)
{
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    rawAcm->ctrlReq = UsbRawAllocRequest(rawAcm->devHandle, 0, USB_CTRL_REQ_SIZE);
    if (rawAcm->ctrlReq == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    ((struct UsbHostRequest *)(rawAcm->ctrlReq))->devHandle = (struct UsbDeviceHandle *)rawAcm->devHandle;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfAllocRequest007(void)
{
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    rawAcm->notifyReq = UsbRawAllocRequest(NULL, 0, rawAcm->notifyEp.maxPacketSize);
    if (rawAcm->notifyReq) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfAllocRequest008(void)
{
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    rawAcm->notifyReq = UsbRawAllocRequest(rawAcm->devHandle, 0, rawAcm->notifyEp.maxPacketSize);
    if (rawAcm->notifyReq == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    ((struct UsbHostRequest *)(rawAcm->notifyReq))->devHandle = (struct UsbDeviceHandle *)rawAcm->devHandle;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}
int32_t CheckRawSdkIfAllocRequest010(void)
{
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    rawAcm->isoReq = UsbRawAllocRequest(NULL, USB_ISO_PACKAT_CNT, rawAcm->isoEp.maxPacketSize);
    if (rawAcm->isoReq == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    ((struct UsbHostRequest *)(rawAcm->isoReq))->devHandle = (struct UsbDeviceHandle *)rawAcm->devHandle;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}
int32_t CheckRawSdkIfAllocRequest011(void)
{
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    rawAcm->isoReq = UsbRawAllocRequest(rawAcm->devHandle, USB_ISO_PACKAT_CNT, rawAcm->isoEp.maxPacketSize);
    if (rawAcm->isoReq == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    ((struct UsbHostRequest *)(rawAcm->isoReq))->devHandle = (struct UsbDeviceHandle *)rawAcm->devHandle;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFreeRequest006(void)
{
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = UsbRawFreeRequest(rawAcm->isoReq);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    rawAcm->isoReq = NULL;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillIsoRequest001(void)
{
    struct UsbRawFillRequestData reqData;
    int32_t ret;
    int32_t i;
    uint32_t size;
    char sendData[] = {"abcde\0"};
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    size = strlen(sendData) + 1;
    printf("---size:%u\n", size);
    size = (size > rawAcm->isoEp.maxPacketSize) ? rawAcm->isoEp.maxPacketSize : size;
    for (i = 0; i < 1; i++) {
        reqData.endPoint      = rawAcm->isoEp.addr;
        reqData.numIsoPackets = USB_ISO_PACKAT_CNT;
        reqData.callback      = AcmWriteIsoCallback;
        reqData.timeout       = USB_CTRL_SET_TIMEOUT;
        reqData.buffer        = (unsigned char*)sendData;
        reqData.length        = size;
        ret = UsbRawFillIsoRequest(rawAcm->isoReq, rawAcm->devHandle, &reqData);
        if (ret) {
            printf("%s: error++ret=%d\n", __func__, ret);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillIsoRequest002(void)
{
    struct UsbRawFillRequestData reqData;
    int32_t ret;
    int32_t i;
    uint32_t size;
    char sendData[] = {"abcde\0"};
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    size = strlen(sendData) + 1;
    printf("---size:%u\n", size);
    size = (size > rawAcm->isoEp.maxPacketSize) ? rawAcm->isoEp.maxPacketSize : size;
    for (i = 0; i < 1; i++) {
        reqData.endPoint      = rawAcm->isoEp.addr;
        reqData.numIsoPackets = USB_ISO_PACKAT_CNT;
        reqData.callback      = AcmWriteIsoCallback;
        reqData.timeout       = USB_CTRL_SET_TIMEOUT;
        reqData.buffer        = (unsigned char*)sendData;
        reqData.length        = size;
        ret = UsbRawFillIsoRequest(NULL, rawAcm->devHandle, &reqData);
        if (ret) {
            printf("%s: error++ret=%d\n", __func__, ret);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillIsoRequest003(void)
{
    struct UsbRawFillRequestData reqData;
    int32_t ret;
    int32_t i;
    uint32_t size;
    char sendData[] = {"abcde\0"};
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    size = strlen(sendData) + 1;
    printf("---size:%u\n", size);
    size = (size > rawAcm->isoEp.maxPacketSize) ? rawAcm->isoEp.maxPacketSize : size;
    for (i = 0; i < 1; i++) {
        reqData.endPoint      = rawAcm->isoEp.addr;
        reqData.numIsoPackets = USB_ISO_PACKAT_CNT;
        reqData.callback      = AcmWriteIsoCallback;
        reqData.timeout       = USB_CTRL_SET_TIMEOUT;
        reqData.buffer        = (unsigned char*)sendData;
        reqData.length        = size;
        ret = UsbRawFillIsoRequest(rawAcm->isoReq, rawAcm->devHandle, NULL);
        if (ret) {
            printf("%s: error++ret=%d\n", __func__, ret);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillIsoRequest004(void)
{
    struct UsbRawFillRequestData reqData;
    int32_t ret;
    int32_t i;
    uint32_t size;
    char sendData[] = {"abcde\0"};
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    size = strlen(sendData) + 1;
    printf("---size:%u\n", size);
    size = (size > rawAcm->isoEp.maxPacketSize) ? rawAcm->isoEp.maxPacketSize : size;
    for (i = 0; i < 1; i++) {
        reqData.endPoint      = rawAcm->isoEp.addr;
        reqData.numIsoPackets = USB_ISO_PACKAT_CNT;
        reqData.callback      = AcmWriteIsoCallback;
        reqData.timeout       = USB_CTRL_SET_TIMEOUT;
        reqData.buffer        = (unsigned char*)sendData;
        reqData.length        = size;
        ret = UsbRawFillIsoRequest(rawAcm->isoReq, NULL, &reqData);
        if (ret) {
            printf("%s: error++ret=%d\n", __func__, ret);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillIsoRequest005(void)
{
    struct UsbRawFillRequestData reqData;
    int32_t ret;
    int32_t i;
    uint32_t size;
    char sendData[] = {"abcde\0"};
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    size = strlen(sendData) + 1;
    printf("---size:%u\n", size);
    size = (size > rawAcm->isoEp.maxPacketSize) ? rawAcm->isoEp.maxPacketSize : size;
    for (i = 0; i < 1; i++) {
        reqData.endPoint      = rawAcm->isoEp.addr;
        reqData.numIsoPackets = USB_ISO_PACKAT_CNT;
        reqData.callback      = AcmWriteIsoCallback;
        reqData.timeout       = USB_CTRL_SET_TIMEOUT;
        reqData.buffer        = (unsigned char*)sendData;
        reqData.length        = size;
        ret = UsbRawFillIsoRequest(NULL, NULL, &reqData);
        if (ret) {
            printf("%s: error++ret=%d\n", __func__, ret);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillIsoRequest006(void)
{
    struct UsbRawFillRequestData reqData;
    int32_t ret;
    int32_t i;
    uint32_t size;
    char sendData[] = {"abcde\0"};
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    size = strlen(sendData) + 1;
    printf("---size:%u\n", size);
    size = (size > rawAcm->isoEp.maxPacketSize) ? rawAcm->isoEp.maxPacketSize : size;
    for (i = 0; i < 1; i++) {
        reqData.endPoint      = rawAcm->isoEp.addr;
        reqData.numIsoPackets = USB_ISO_PACKAT_CNT;
        reqData.callback      = AcmWriteIsoCallback;
        reqData.timeout       = USB_CTRL_SET_TIMEOUT;
        reqData.buffer        = (unsigned char*)sendData;
        reqData.length        = size;
        ret = UsbRawFillIsoRequest(NULL, NULL, NULL);
        if (ret) {
            printf("%s: error++ret=%d\n", __func__, ret);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}
int32_t CheckRawSdkIfFreeRequest001(void)
{
    int32_t ret;
    int32_t i;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    for (i = 0; i < ACM_NW; i++) {
        ret = UsbRawFreeRequest(rawAcm->wb[i].request);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
        rawAcm->wb[i].request = NULL;
    }
    AcmWriteBufFree(rawAcm);
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFreeRequest002(void)
{
    int32_t ret;
    int32_t i;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    for (i = 0; i < ACM_NW; i++) {
        ret = UsbRawFreeRequest(rawAcm->readReq[i]);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
        rawAcm->readReq[i] = NULL;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFreeRequest003(void)
{
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = UsbRawFreeRequest(rawAcm->ctrlReq);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    rawAcm->ctrlReq = NULL;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFreeRequest004(void)
{
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = UsbRawFreeRequest(rawAcm->notifyReq);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    rawAcm->notifyReq = NULL;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFreeRequest005(void)
{
    int32_t ret;

    ret = UsbRawFreeRequest(NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfAllocRequest009(void)
{
    int32_t i;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = AcmWriteBufAlloc(rawAcm);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }

    for (i = 0; i < ACM_NW; i++) {
        rawAcm->wb[i].request = UsbRawAllocRequest(rawAcm->devHandle, 0, rawAcm->dataOutEp.maxPacketSize);
        rawAcm->wb[i].instance = rawAcm;
        if (rawAcm->wb[i].request == NULL) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }

    for (i = 0; i < ACM_NR; i++) {
        rawAcm->readReq[i] = UsbRawAllocRequest(rawAcm->devHandle, 0, rawAcm->dataInEp.maxPacketSize);
        if (rawAcm->readReq[i] == NULL) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }

    rawAcm->ctrlReq = UsbRawAllocRequest(rawAcm->devHandle, 0, USB_CTRL_REQ_SIZE);
    if (rawAcm->ctrlReq == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }

    rawAcm->notifyReq = UsbRawAllocRequest(rawAcm->devHandle, 0, rawAcm->notifyEp.maxPacketSize);
    if (rawAcm->notifyReq == NULL) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetDescriptor001(void)
{
    struct UsbRawDescriptorParam param;
    unsigned char *data = NULL;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    data = OsalMemCalloc(USB_BUFFER_MAX_SIZE);
    if (data == NULL) {
        HDF_LOGE("%s:%d OsalMemCalloc error", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = UsbRawGetDescriptor(NULL, rawAcm->devHandle, &param, data);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        ret = HDF_FAILURE;
        goto ERROR;
    }
    HDF_LOGE("%s: success", __func__);
    ret = HDF_SUCCESS;
ERROR:
    OsalMemFree(data);
    data = NULL;

    return ret;
}

int32_t CheckRawSdkIfGetDescriptor002(void)
{
    struct UsbRawDescriptorParam param;
    unsigned char *data = NULL;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    data = OsalMemCalloc(USB_BUFFER_MAX_SIZE);
    if (data == NULL) {
        HDF_LOGE("%s:%d OsalMemCalloc error", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = UsbRawGetDescriptor(rawAcm->ctrlReq, NULL, &param, data);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        ret = HDF_FAILURE;
        goto ERROR;
    }
    HDF_LOGE("%s: success", __func__);
    ret = HDF_SUCCESS;
ERROR:
    OsalMemFree(data);
    data = NULL;

    return ret;
}

int32_t CheckRawSdkIfGetDescriptor003(void)
{
    struct UsbRawDescriptorParam param;
    unsigned char *data = NULL;
    int32_t ret;

    data = OsalMemCalloc(USB_BUFFER_MAX_SIZE);
    if (data == NULL) {
        HDF_LOGE("%s:%d OsalMemCalloc error", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = UsbRawGetDescriptor(NULL, NULL, &param, data);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        ret = HDF_FAILURE;
        goto ERROR;
    }
    HDF_LOGE("%s: success", __func__);
    ret = HDF_SUCCESS;
ERROR:
    OsalMemFree(data);
    data = NULL;

    return ret;
}

int32_t CheckRawSdkIfGetDescriptor004(void)
{
    unsigned char *data = NULL;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    data = OsalMemCalloc(USB_BUFFER_MAX_SIZE);
    if (data == NULL) {
        HDF_LOGE("%s:%d OsalMemCalloc error", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = UsbRawGetDescriptor(rawAcm->ctrlReq, rawAcm->devHandle, NULL, data);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        ret = HDF_FAILURE;
        goto ERROR;
    }
    HDF_LOGE("%s: success", __func__);
    ret = HDF_SUCCESS;
ERROR:
    OsalMemFree(data);
    data = NULL;

    return ret;
}

int32_t CheckRawSdkIfGetDescriptor005(void)
{
    struct UsbRawDescriptorParam param;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = UsbRawGetDescriptor(rawAcm->ctrlReq, rawAcm->devHandle, &param, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetDescriptor006(void)
{
    struct UsbRawDescriptorParam param;
    unsigned char *data = NULL;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    data = OsalMemCalloc(USB_BUFFER_MAX_SIZE);
    if (data == NULL) {
        HDF_LOGE("%s:%d OsalMemCalloc error", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }

    param.descType = USB_DESC_TYPE;
    param.descIndex = 0;
    param.length = USB_BUFFER_MAX_SIZE;

    ret = UsbRawGetDescriptor(rawAcm->ctrlReq, rawAcm->devHandle, &param, data);
    if (ret < 0) {
        HDF_LOGE("%s: error", __func__);
        ret = HDF_FAILURE;
        goto ERROR;
    }
    HDF_LOGE("%s: success", __func__);
    ret = HDF_SUCCESS;
ERROR:
    OsalMemFree(data);
    data = NULL;

    return ret;
}

int32_t CheckRawSdkIfGetDescriptor007(void)
{
    unsigned char *data = NULL;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    data = OsalMemCalloc(USB_BUFFER_MAX_SIZE);
    if (data == NULL) {
        HDF_LOGE("%s:%d OsalMemCalloc error", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = UsbRawGetDescriptor(rawAcm->ctrlReq, NULL, NULL, data);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        ret = HDF_FAILURE;
        goto ERROR;
    }
    HDF_LOGE("%s: success", __func__);
    ret = HDF_SUCCESS;
ERROR:
    OsalMemFree(data);
    data = NULL;

    return ret;
}

int32_t CheckRawSdkIfGetDescriptor008(void)
{
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = UsbRawGetDescriptor(rawAcm->ctrlReq, rawAcm->devHandle, NULL, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetDescriptor009(void)
{
    unsigned char *data = NULL;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    data = OsalMemCalloc(USB_BUFFER_MAX_SIZE);
    if (data == NULL) {
        HDF_LOGE("%s:%d OsalMemCalloc error", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = UsbRawGetDescriptor(NULL, rawAcm->devHandle, NULL, data);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        ret = HDF_FAILURE;
        goto ERROR;
    }
    HDF_LOGE("%s: success", __func__);
    ret = HDF_SUCCESS;
ERROR:
    OsalMemFree(data);
    data = NULL;

    return ret;
}

int32_t CheckRawSdkIfGetDescriptor010(void)
{
    struct UsbRawDescriptorParam param;
    unsigned char data[100];
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    param.descType = 0;
    param.descIndex = 0;
    param.length = sizeof(data);

    ret = UsbRawGetDescriptor(NULL, rawAcm->devHandle, &param, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetDescriptor011(void)
{
    struct UsbRawDescriptorParam param;
    unsigned char data[100];
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    param.descType = 0;
    param.descIndex = 0;
    param.length = sizeof(data);

    ret = UsbRawGetDescriptor(rawAcm->ctrlReq, NULL, &param, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetDescriptor012(void)
{
    unsigned char *data = NULL;
    int32_t ret;

    data = OsalMemCalloc(USB_BUFFER_MAX_SIZE);
    if (data == NULL) {
        HDF_LOGE("%s:%d OsalMemCalloc error", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = UsbRawGetDescriptor(NULL, NULL, NULL, data);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        ret = HDF_FAILURE;
        goto ERROR;
    }
    HDF_LOGE("%s: success", __func__);
    ret = HDF_SUCCESS;
ERROR:
    OsalMemFree(data);
    data = NULL;

    return ret;
}

int32_t CheckRawSdkIfGetDescriptor013(void)
{
    struct UsbRawDescriptorParam param;
    unsigned char data[100];
    int32_t ret;

    param.descType = 0;
    param.descIndex = 0;
    param.length = sizeof(data);

    ret = UsbRawGetDescriptor(NULL, NULL, &param, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetDescriptor014(void)
{
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = UsbRawGetDescriptor(NULL, rawAcm->devHandle, NULL, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetDescriptor015(void)
{
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = UsbRawGetDescriptor(rawAcm->ctrlReq, NULL, NULL, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfGetDescriptor016(void)
{
    int32_t ret;

    ret = UsbRawGetDescriptor(NULL, NULL, NULL, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillBulkRequest001(void)
{
    struct UsbRawFillRequestData reqData;
    int32_t ret;
    int32_t i;
    uint32_t size;
    char sendData[] = {"abcde\0"};
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    size = strlen(sendData) + 1;
    printf("---size:%u\n", size);

    size = (size > rawAcm->dataOutEp.maxPacketSize) ? rawAcm->dataOutEp.maxPacketSize : size;
    for (i = 0; i < 1; i++) {
        struct RawWb *snd = &rawAcm->wb[i];
        snd->len = (int)size;
        ret = memcpy_s(snd->buf, rawAcm->dataOutEp.maxPacketSize, sendData, size);
        if (ret) {
            HDF_LOGE("%s: memcpy_s fail", __func__);
            return HDF_FAILURE;
        }
        rawAcm->transmitting++;

        reqData.endPoint      = rawAcm->dataOutEp.addr;
        reqData.numIsoPackets = 0;
        reqData.callback      = AcmWriteBulkCallback;
        reqData.userData      = (void *)snd;
        reqData.timeout       = USB_CTRL_SET_TIMEOUT;
        reqData.buffer        = snd->buf;
        reqData.length        = snd->len;
        printf("maxPacketSize:%d\n", rawAcm->dataOutEp.maxPacketSize);
        ret = UsbRawFillBulkRequest(snd->request, rawAcm->devHandle, &reqData);
        if (ret) {
            printf("%s: error++ret=%d\n", __func__, ret);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillBulkRequest002(void)
{
    struct UsbRawFillRequestData reqData;
    int32_t ret;
    int32_t i;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    uint32_t size = rawAcm->dataInEp.maxPacketSize;

    for (i = 0; i < 1; i++) {
        reqData.endPoint      = rawAcm->dataInEp.addr;
        reqData.numIsoPackets = 0;
        reqData.callback      = AcmReadBulkCallback;
        reqData.userData      = (void *)rawAcm;
        reqData.timeout       = USB_CTRL_SET_TIMEOUT;
        reqData.length        = size;

        ret = UsbRawFillBulkRequest(rawAcm->readReq[i], rawAcm->devHandle, &reqData);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillInterruptRequest001(void)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    uint32_t size = rawAcm->notifyEp.maxPacketSize;

    fillRequestData.endPoint = rawAcm->notifyEp.addr;
    fillRequestData.length = size;
    fillRequestData.numIsoPackets = 0;
    fillRequestData.callback = AcmNotifyReqCallback;
    fillRequestData.userData = (void *)rawAcm;
    fillRequestData.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;

    ret = UsbRawFillInterruptRequest(rawAcm->notifyReq, rawAcm->devHandle, &fillRequestData);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillInterruptRequest002(void)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    uint32_t size = rawAcm->notifyEp.maxPacketSize;

    fillRequestData.endPoint = rawAcm->notifyEp.addr;
    fillRequestData.length = size;
    fillRequestData.numIsoPackets = 0;
    fillRequestData.callback = AcmNotifyReqCallback;
    fillRequestData.userData = (void *)rawAcm;
    fillRequestData.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;

    ret = UsbRawFillInterruptRequest(NULL, rawAcm->devHandle, &fillRequestData);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillInterruptRequest003(void)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    uint32_t size = rawAcm->notifyEp.maxPacketSize;

    fillRequestData.endPoint = rawAcm->notifyEp.addr;
    fillRequestData.length = size;
    fillRequestData.numIsoPackets = 0;
    fillRequestData.callback = AcmNotifyReqCallback;
    fillRequestData.userData = (void *)rawAcm;
    fillRequestData.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;

    ret = UsbRawFillInterruptRequest(rawAcm->notifyReq, NULL, &fillRequestData);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillInterruptRequest004(void)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    uint32_t size = rawAcm->notifyEp.maxPacketSize;

    fillRequestData.endPoint = rawAcm->notifyEp.addr;
    fillRequestData.length = size;
    fillRequestData.numIsoPackets = 0;
    fillRequestData.callback = AcmNotifyReqCallback;
    fillRequestData.userData = (void *)rawAcm;
    fillRequestData.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;

    ret = UsbRawFillInterruptRequest(rawAcm->notifyReq, rawAcm->devHandle, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillControlRequest001(void)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    int32_t completed = 0;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    fillRequestData.callback  = AcmCtrlReqCallback;
    fillRequestData.userData  = &completed;
    fillRequestData.timeout   = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlRequest(rawAcm->ctrlReq, rawAcm->devHandle, &fillRequestData);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillControlRequest002(void)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    int32_t completed = 0;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    fillRequestData.callback  = AcmCtrlReqCallback;
    fillRequestData.userData  = &completed;
    fillRequestData.timeout   = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlRequest(NULL, rawAcm->devHandle, &fillRequestData);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillControlRequest003(void)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    int32_t completed = 0;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    fillRequestData.callback  = AcmCtrlReqCallback;
    fillRequestData.userData  = &completed;
    fillRequestData.timeout   = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlRequest(rawAcm->ctrlReq, NULL, &fillRequestData);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillControlRequest004(void)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    int32_t completed = 0;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    fillRequestData.callback  = AcmCtrlReqCallback;
    fillRequestData.userData  = &completed;
    fillRequestData.timeout   = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlRequest(rawAcm->ctrlReq, rawAcm->devHandle, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillControlRequest005(void)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    int32_t completed = 0;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    fillRequestData.callback  = AcmCtrlReqCallback;
    fillRequestData.userData  = &completed;
    fillRequestData.timeout   = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlRequest(NULL, rawAcm->devHandle, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillControlRequest006(void)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    int32_t completed = 0;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    fillRequestData.callback  = AcmCtrlReqCallback;
    fillRequestData.userData  = &completed;
    fillRequestData.timeout   = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlRequest(rawAcm->ctrlReq, NULL, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillControlRequest007(void)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    int32_t completed = 0;

    fillRequestData.callback  = AcmCtrlReqCallback;
    fillRequestData.userData  = &completed;
    fillRequestData.timeout   = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlRequest(NULL, NULL, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillControlRequest008(void)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    int32_t completed = 0;

    fillRequestData.callback  = AcmCtrlReqCallback;
    fillRequestData.userData  = &completed;
    fillRequestData.timeout   = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlRequest(NULL, NULL, &fillRequestData);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillControlSetup001(void)
{
    struct UsbControlRequestData ctrlReq;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    rawAcm->lineCoding.dwDTERate = CpuToLe32(DATARATE);
    rawAcm->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    rawAcm->lineCoding.bParityType = USB_CDC_NO_PARITY;
    rawAcm->lineCoding.bDataBits = DATA_BITS_LENGTH;

    ctrlReq.requestType = USB_DDK_DIR_OUT | USB_DDK_TYPE_CLASS | USB_DDK_RECIP_INTERFACE;
    ctrlReq.requestCmd  = USB_DDK_CDC_REQ_SET_LINE_CODING;
    ctrlReq.value       = CpuToLe16(0);
    ctrlReq.index       = 0;
    ctrlReq.data        = (unsigned char *)&rawAcm->lineCoding;
    ctrlReq.length      = sizeof(struct UsbCdcLineCoding);
    ctrlReq.timeout     = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlSetup(NULL, &ctrlReq);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillControlSetup002(void)
{
    unsigned char *setup = NULL;
    int32_t ret;

    setup = OsalMemCalloc(USB_BUFFER_MAX_SIZE);
    if (setup == NULL) {
        HDF_LOGE("%s:%d OsalMemCalloc error", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = UsbRawFillControlSetup(setup, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        ret = HDF_FAILURE;
        goto ERROR;
    }
    HDF_LOGE("%s: success", __func__);
    ret = HDF_SUCCESS;
ERROR:
    OsalMemFree(setup);
    setup = NULL;

    return ret;
}

int32_t CheckRawSdkIfFillControlSetup003(void)
{
    int32_t ret;

    ret = UsbRawFillControlSetup(NULL, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillControlSetup004(void)
{
    struct UsbControlRequestData ctrlReq;
    unsigned char *setup = NULL;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    rawAcm->lineCoding.dwDTERate = CpuToLe32(DATARATE);
    rawAcm->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    rawAcm->lineCoding.bParityType = USB_CDC_NO_PARITY;
    rawAcm->lineCoding.bDataBits = DATA_BITS_LENGTH;

    ctrlReq.requestType = USB_DDK_DIR_OUT | USB_DDK_TYPE_CLASS | USB_DDK_RECIP_INTERFACE;
    ctrlReq.requestCmd  = USB_DDK_CDC_REQ_SET_LINE_CODING;
    ctrlReq.value       = CpuToLe16(0);
    ctrlReq.index       = 0;
    ctrlReq.data        = (unsigned char *)&rawAcm->lineCoding;
    ctrlReq.length      = sizeof(struct UsbCdcLineCoding);
    ctrlReq.timeout     = USB_CTRL_SET_TIMEOUT;

    setup = OsalMemCalloc(USB_BUFFER_MAX_SIZE);
    if (setup == NULL) {
        HDF_LOGE("%s:%d OsalMemCalloc error", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = UsbRawFillControlSetup(setup, &ctrlReq);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        ret = HDF_FAILURE;
        goto ERROR;
    }
    HDF_LOGE("%s: success", __func__);

ERROR:
    OsalMemFree(setup);
    setup = NULL;

    return ret;
}

int32_t CheckRawSdkIfSendControlRequest001(void)
{
    struct UsbControlRequestData ctrlReq;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    rawAcm->lineCoding.dwDTERate = CpuToLe32(DATARATE);
    rawAcm->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    rawAcm->lineCoding.bParityType = USB_CDC_NO_PARITY;
    rawAcm->lineCoding.bDataBits = DATA_BITS_LENGTH;
    ctrlReq.requestType = USB_DDK_DIR_OUT | USB_DDK_TYPE_CLASS | USB_DDK_RECIP_INTERFACE;
    ctrlReq.requestCmd  = USB_DDK_CDC_REQ_SET_LINE_CODING;
    ctrlReq.value       = CpuToLe16(0);
    ctrlReq.index       = 0;
    ctrlReq.data        = (unsigned char *)&rawAcm->lineCoding;
    ctrlReq.length      = sizeof(struct UsbCdcLineCoding);
    ctrlReq.timeout     = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawSendControlRequest(NULL, rawAcm->devHandle, &ctrlReq);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSendControlRequest002(void)
{
    struct UsbControlRequestData ctrlReq;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    rawAcm->lineCoding.dwDTERate = CpuToLe32(DATARATE);
    rawAcm->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    rawAcm->lineCoding.bParityType = USB_CDC_NO_PARITY;
    rawAcm->lineCoding.bDataBits = DATA_BITS_LENGTH;
    ctrlReq.requestType = USB_DDK_DIR_OUT | USB_DDK_TYPE_CLASS | USB_DDK_RECIP_INTERFACE;
    ctrlReq.requestCmd  = USB_DDK_CDC_REQ_SET_LINE_CODING;
    ctrlReq.value       = CpuToLe16(0);
    ctrlReq.index       = 0;
    ctrlReq.data        = (unsigned char *)&rawAcm->lineCoding;
    ctrlReq.length      = sizeof(struct UsbCdcLineCoding);
    ctrlReq.timeout     = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawSendControlRequest(rawAcm->ctrlReq, NULL, &ctrlReq);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSendControlRequest003(void)
{
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    ret = UsbRawSendControlRequest(rawAcm->ctrlReq, rawAcm->devHandle, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSendControlRequest004(void)
{
    struct UsbControlRequestData ctrlReq;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    rawAcm->lineCoding.dwDTERate = CpuToLe32(DATARATE);
    rawAcm->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    rawAcm->lineCoding.bParityType = USB_CDC_NO_PARITY;
    rawAcm->lineCoding.bDataBits = DATA_BITS_LENGTH;
    ctrlReq.requestType = USB_DDK_DIR_OUT | USB_DDK_TYPE_CLASS | USB_DDK_RECIP_INTERFACE;
    ctrlReq.requestCmd  = USB_DDK_CDC_REQ_SET_LINE_CODING;
    ctrlReq.value       = CpuToLe16(0);
    ctrlReq.index       = 2;
    ctrlReq.data        = (unsigned char *)&rawAcm->lineCoding;
    ctrlReq.length      = sizeof(struct UsbCdcLineCoding);
    ctrlReq.timeout     = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawSendControlRequest(rawAcm->ctrlReq, rawAcm->devHandle, &ctrlReq);
    if (ret < 0) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSendControlRequest005(void)
{
    struct UsbControlRequestData ctrlReq;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    rawAcm->lineCoding.dwDTERate = CpuToLe32(DATARATE);
    rawAcm->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    rawAcm->lineCoding.bParityType = USB_CDC_NO_PARITY;
    rawAcm->lineCoding.bDataBits = DATA_BITS_LENGTH;
    ctrlReq.requestType = USB_DDK_DIR_OUT | USB_DDK_TYPE_CLASS | USB_DDK_RECIP_INTERFACE;
    ctrlReq.requestCmd  = USB_DDK_CDC_REQ_SET_LINE_CODING;
    ctrlReq.value       = CpuToLe16(0);
    ctrlReq.index       = 0;
    ctrlReq.data        = (unsigned char *)&rawAcm->lineCoding;
    ctrlReq.length      = sizeof(struct UsbCdcLineCoding);
    ctrlReq.timeout     = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawSendControlRequest(NULL, NULL, &ctrlReq);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSendControlRequest006(void)
{
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = UsbRawSendControlRequest(NULL, rawAcm->devHandle, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSendControlRequest007(void)
{
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = UsbRawSendControlRequest(rawAcm->ctrlReq, NULL, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSendBulkRequest001(void)
{
    struct UsbRequestData reqData;
    int32_t ret;
    int32_t i;
    uint32_t size;
    char sendData[] = {"abcd\0"};
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    size = strlen(sendData) + 1;
    size = (size > rawAcm->dataOutEp.maxPacketSize) ? rawAcm->dataOutEp.maxPacketSize : size;

    for (i = 0; i < 1; i++) {
        struct RawWb *snd = &rawAcm->wb[i];
        snd->len = (int)size;
        ret = memcpy_s(snd->buf, rawAcm->dataOutEp.maxPacketSize, sendData, size);
        if (ret) {
            HDF_LOGE("%s: memcpy_s fail", __func__);
            return HDF_FAILURE;
        }
        rawAcm->transmitting++;

        reqData.endPoint      = rawAcm->dataOutEp.addr;
        reqData.timeout       = USB_RAW_REQUEST_TIME_ZERO_MS;
        reqData.data        = snd->buf;
        reqData.length        = snd->len;
        reqData.requested   = (int32_t *)&size;
    }

    for (i = 0; i < 1; i++) {
        struct RawWb *snd = &rawAcm->wb[i];
        printf("UsbRawSendBulkRequest i = [%d]\n", i);
        ret = UsbRawSendBulkRequest(snd->request, rawAcm->devHandle, &reqData);
        if (ret) {
            printf("%s: error+ret:%d", __func__, ret);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSendBulkRequest002(void)
{
    struct UsbRequestData reqData;
    int32_t ret;
    int32_t i;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    uint32_t size = rawAcm->dataInEp.maxPacketSize;

    for (i = 0; i < 1; i++) {
        reqData.endPoint      = rawAcm->dataInEp.addr;
        reqData.timeout       = USB_RAW_REQUEST_TIME_ZERO_MS;
        reqData.length        = size;
        reqData.data        = ((struct UsbRawRequest *)rawAcm->readReq[i])->buffer;
        reqData.requested      = (int32_t *)&size;
    }

    for (i = 0; i < 1; i++) {
        printf("UsbRawSendBulkRequest i = [%d]\n", i);
        ret = UsbRawSendBulkRequest(rawAcm->readReq[i], rawAcm->devHandle, &reqData);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSendBulkRequest003(void)
{
    struct UsbRequestData reqData;
    int32_t ret;
    int32_t i;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    uint32_t size = rawAcm->dataInEp.maxPacketSize;

    for (i = 0; i < 1; i++) {
        reqData.endPoint      = rawAcm->dataInEp.addr;
        reqData.timeout       = USB_RAW_REQUEST_TIME_ZERO_MS;
        reqData.length        = size;
        reqData.data        = ((struct UsbRawRequest *)rawAcm->readReq[i])->buffer;
        reqData.requested      = (int32_t *)&size;
    }

    for (i = 0; i < 1; i++) {
        printf("UsbRawSendBulkRequest i = [%d]\n", i);
        ret = UsbRawSendBulkRequest(NULL, rawAcm->devHandle, &reqData);
        if (ret != HDF_ERR_INVALID_PARAM) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSendBulkRequest004(void)
{
    struct UsbRequestData reqData;
    int32_t ret;
    int32_t i;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    uint32_t size = rawAcm->dataInEp.maxPacketSize;

    for (i = 0; i < 1; i++) {
        reqData.endPoint      = rawAcm->dataInEp.addr;
        reqData.timeout       = USB_RAW_REQUEST_TIME_ZERO_MS;
        reqData.length        = size;
        reqData.data        = ((struct UsbRawRequest *)rawAcm->readReq[i])->buffer;
        reqData.requested      = (int32_t *)&size;
    }
    for (i = 0; i < 1; i++) {
        printf("UsbRawSendBulkRequest i = [%d]\n", i);
        ret = UsbRawSendBulkRequest(rawAcm->readReq[i], NULL, &reqData);
        if (ret != HDF_ERR_INVALID_PARAM) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSendBulkRequest005(void)
{
    int32_t ret;
    int32_t i;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    for (i = 0; i < 1; i++) {
        printf("UsbRawSendBulkRequest i = [%d]\n", i);
        ret = UsbRawSendBulkRequest(rawAcm->readReq[i], rawAcm->devHandle, NULL);
        if (ret != HDF_ERR_INVALID_PARAM) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSendInterruptRequest001(void)
{
    struct UsbRequestData reqData;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    uint32_t size = rawAcm->notifyEp.maxPacketSize;

    reqData.endPoint = rawAcm->notifyEp.addr;
    reqData.length = size;
    reqData.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;
    reqData.data        = ((struct UsbRawRequest *)rawAcm->notifyReq)->buffer;
    reqData.requested      = (int32_t *)&size;
    ret = UsbRawSendInterruptRequest(rawAcm->notifyReq, rawAcm->devHandle, &reqData);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSendInterruptRequest002(void)
{
    struct UsbRequestData reqData;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    uint32_t size = rawAcm->notifyEp.maxPacketSize;

    reqData.endPoint = rawAcm->notifyEp.addr;
    reqData.length = size;
    reqData.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;
    reqData.data        = ((struct UsbRawRequest *)rawAcm->notifyReq)->buffer;
    reqData.requested      = (int32_t *)&size;

    ret = UsbRawSendInterruptRequest(NULL, rawAcm->devHandle, &reqData);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSendInterruptRequest003(void)
{
    struct UsbRequestData reqData;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    uint32_t size = rawAcm->notifyEp.maxPacketSize;

    reqData.endPoint = rawAcm->notifyEp.addr;
    reqData.length = size;
    reqData.timeout = USB_RAW_REQUEST_TIME_ZERO_MS;
    reqData.data        = ((struct UsbRawRequest *)rawAcm->notifyReq)->buffer;
    reqData.requested      = (int32_t *)&size;

    ret = UsbRawSendInterruptRequest(rawAcm->notifyReq, NULL, &reqData);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSendInterruptRequest004(void)
{
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = UsbRawSendInterruptRequest(rawAcm->notifyReq, rawAcm->devHandle, NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillBulkRequest003(void)
{
    struct UsbRawFillRequestData reqData;
    int32_t ret;
    int32_t i;
    uint32_t size;
    char sendData[] = {"abcde\0"};
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    size = strlen(sendData) + 1;
    size = (size > rawAcm->dataOutEp.maxPacketSize) ? rawAcm->dataOutEp.maxPacketSize : size;

    for (i = 0; i < ACM_NW; i++) {
        struct RawWb *snd = &rawAcm->wb[i];
        snd->len = (int)size;
        ret = memcpy_s(snd->buf, rawAcm->dataOutEp.maxPacketSize, sendData, size);
        if (ret) {
            HDF_LOGE("%s: memcpy_s fail", __func__);
            return HDF_FAILURE;
        }
        rawAcm->transmitting++;

        reqData.endPoint      = rawAcm->dataOutEp.addr;
        reqData.numIsoPackets = 0;
        reqData.callback      = AcmWriteBulkCallback;
        reqData.userData      = (void *)snd;
        reqData.timeout       = USB_CTRL_SET_TIMEOUT;
        reqData.buffer        = snd->buf;
        reqData.length        = snd->len;

        ret = UsbRawFillBulkRequest(snd->request, rawAcm->devHandle, &reqData);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillBulkRequest004(void)
{
    struct UsbRawFillRequestData reqData;
    int32_t ret;
    int32_t i;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    uint32_t size = rawAcm->dataInEp.maxPacketSize;

    for (i = 0; i < ACM_NW; i++) {
        reqData.endPoint      = rawAcm->dataInEp.addr;
        reqData.numIsoPackets = 0;
        reqData.callback      = AcmReadBulkCallback;
        reqData.userData      = (void *)rawAcm;
        reqData.timeout       = USB_RAW_REQUEST_TIME_ZERO_MS;
        reqData.length        = size;
        ret = UsbRawFillBulkRequest(rawAcm->readReq[i], rawAcm->devHandle, &reqData);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfFillInterruptRequest005(void)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();
    uint32_t size = rawAcm->notifyEp.maxPacketSize;
    fillRequestData.endPoint = rawAcm->notifyEp.addr;
    fillRequestData.length = size;
    fillRequestData.numIsoPackets = 0;
    fillRequestData.callback = AcmNotifyReqCallback;
    fillRequestData.userData = (void *)rawAcm;
    fillRequestData.timeout = USB_CTRL_SET_TIMEOUT;
    ret = UsbRawFillInterruptRequest(rawAcm->notifyReq, rawAcm->devHandle, &fillRequestData);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSubmitRequest001(void)
{
    int32_t ret;
    int32_t i;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    for (i = 0; i < ACM_NW; i++) {
        struct RawWb *snd = &rawAcm->wb[i];
        printf("UsbRawSubmitRequest i = [%d]\n", i);
        ret = UsbRawSubmitRequest(snd->request);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSubmitRequest002(void)
{
    int32_t ret;
    int32_t i;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    for (i = 0; i < ACM_NW; i++) {
        printf("UsbRawSubmitRequest i = [%d]\n", i);
        ret = UsbRawSubmitRequest(rawAcm->readReq[i]);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSubmitRequest003(void)
{
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = UsbRawSubmitRequest(rawAcm->notifyReq);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfSubmitRequest004(void)
{
    int32_t ret;

    ret = UsbRawSubmitRequest(NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfCancelRequest001(void)
{
    int32_t ret;
    int32_t i;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    for (i = 0; i < ACM_NW; i++) {
        struct RawWb *snd = &rawAcm->wb[i];
        ret = UsbRawCancelRequest(snd->request);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfCancelRequest002(void)
{
    int32_t ret;
    int32_t i;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    for (i = 0; i < ACM_NR; i++) {
        ret = UsbRawCancelRequest(rawAcm->readReq[i]);
        printf("%s+%d+ret:%d\n", __func__, __LINE__, ret);
        if (ret) {
            HDF_LOGE("%s: error", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfCancelRequest003(void)
{
    int32_t ret;
    struct AcmRawDevice *rawAcm = UsbGetIoAcm();

    ret = UsbRawCancelRequest(rawAcm->notifyReq);
    if (ret) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t CheckRawSdkIfCancelRequest004(void)
{
    int32_t ret;

    ret = UsbRawCancelRequest(NULL);
    if (ret != HDF_ERR_INVALID_PARAM) {
        HDF_LOGE("%s: error", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}
