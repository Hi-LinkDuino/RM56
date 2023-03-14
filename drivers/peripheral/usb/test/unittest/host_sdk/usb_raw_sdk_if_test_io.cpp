/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <gtest/gtest.h>
extern "C" {
#include "usb_raw_sdk_if_test.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"
#include "hdf_usb_pnp_manage.h"
}

using namespace std;
using namespace testing::ext;

namespace acmraw {
class UsbRawSdkIfTestIo : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

#define USB_RAW_IO_SLEEP_MS_TIME    500
#define USB_IO_THREAD_STACK_SIZE   8192

static struct AcmDevice *g_acm = NULL;
static struct AcmDevice g_deviceService;
static UsbRawHandle *g_devHandle = NULL;
static UsbRawDevice *g_dev = NULL;
static int32_t g_activeConfig;
static bool g_stopIoThreadFlag = false;

static int32_t UsbIoThread(void *data)
{
    int32_t ret;
    struct AcmDevice *acm = (struct AcmDevice *)data;

    for (;;) {
        if (acm == nullptr) {
            printf("%s:%d acm is nullptr\n", __func__, __LINE__);
            OsalMSleep(USB_RAW_IO_SLEEP_MS_TIME);
            continue;
        }

        if (acm->devHandle == nullptr) {
            printf("%s:%d acm->devHandle is nullptr!\n", __func__, __LINE__);
            OsalMSleep(USB_RAW_IO_SLEEP_MS_TIME);
            continue;
        }
        ret = UsbRawHandleRequests(acm->devHandle);
        if (ret < 0) {
            printf("%s:%d UsbRawHandleRequests failed, ret = %d \n", __func__, __LINE__, ret);
            if (ret == HDF_DEV_ERR_NO_DEVICE) {
                printf("%s:%d, ret = %d\n", __func__, __LINE__, ret);
                OsalMSleep(USB_RAW_IO_SLEEP_MS_TIME);
            }
        }

        if (g_stopIoThreadFlag) {
            printf("%s:%d\n", __func__, __LINE__);
            g_stopIoThreadFlag = false;
            break;
        }
    }

    return HDF_SUCCESS;
}


static int32_t UsbStartIo(struct AcmDevice *acm)
{
    struct OsalThreadParam threadCfg;
    int32_t ret;

    printf("%s start\n", __func__);

    /* creat Io thread */
    (void)memset_s(&threadCfg, sizeof(threadCfg), 0, sizeof(threadCfg));
    threadCfg.name = (char *)("usb io thread");
    threadCfg.priority = OSAL_THREAD_PRI_LOW;
    threadCfg.stackSize = USB_IO_THREAD_STACK_SIZE;

    ret = OsalThreadCreate(&acm->ioThread, \
                           (OsalThreadEntry)UsbIoThread, (void *)acm);
    if (ret != HDF_SUCCESS) {
        printf("%s:%d OsalThreadCreate failed, ret = %d \n", __func__, __LINE__, ret);
        return ret;
    }

    ret = OsalThreadStart(&acm->ioThread, &threadCfg);
    if (ret != HDF_SUCCESS) {
        printf("%s:%d OsalThreadStart faile, ret=%d \n", __func__, __LINE__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t UsbStopIo(struct AcmDevice *acm)
{
    int32_t ret;

    HDF_LOGD("%{public}s:%{public}d", __func__, __LINE__);
    if (!g_stopIoThreadFlag) {
        HDF_LOGD("%{public}s:%{public}d", __func__, __LINE__);
        g_stopIoThreadFlag = true;
    }
    ret = OsalThreadDestroy(&acm->ioThread);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d OsalThreadDestroy faile, ret=%{public}d ", __func__, __LINE__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

void UsbRawSdkIfTestIo::TearDownTestCase()
{
    g_acm = &g_deviceService;
    if (UsbStopIo(g_acm) != HDF_SUCCESS) {
        HDF_LOGW("%s:%d UsbStopIo error!", __func__, __LINE__);
    }
}

extern "C" {
static void AcmWriteBulkCallback(const void *requestArg)
{
    struct UsbRawRequest *req = (struct UsbRawRequest *)requestArg;

    printf("%s:%d entry!", __func__, __LINE__);
    if (req == nullptr) {
        printf("%s:%d req is nullptr!", __func__, __LINE__);
        return;
    }
    struct AcmWb *wb  = (struct AcmWb *)req->userData;
    if (wb == nullptr) {
        printf("%s:%d userData(wb) is nullptr!", __func__, __LINE__);
        return;
    }

    if (req->status != USB_REQUEST_COMPLETED) {
        printf("%s: write req failed, status=%d", __func__, req->status);
    }

    wb->use = 0;
}

static void AcmReadBulkCallback(const void *requestArg)
{
    struct UsbRawRequest *req = (struct UsbRawRequest *)requestArg;

    printf("%s:%d entry!", __func__, __LINE__);
    if (req == nullptr) {
        printf("%s:%d req is nullptr!", __func__, __LINE__);
        return;
    }
    struct AcmDevice *acm = (struct AcmDevice *)req->userData;
    if (acm == nullptr) {
        printf("%s:%d userData(acm) is nullptr!", __func__, __LINE__);
        return;
    }
    size_t size = req->actualLength;

    switch (req->status) {
        case USB_REQUEST_COMPLETED:
            HDF_LOGD("Bulk status: %{public}d+size:%{public}zu\n", req->status, size);
            if (size) {
                uint8_t *data = req->buffer;
                uint32_t count;

                OsalMutexLock(&acm->readLock);
                if (DataFifoIsFull(&acm->port->readFifo)) {
                    DataFifoSkip(&acm->port->readFifo, size);
                }
                count = DataFifoWrite(&acm->port->readFifo, data, size);
                if (count != size) {
                    printf("%s: write %u less than expected %zu", __func__, count, size);
                }
                OsalMutexUnlock(&acm->readLock);
            }
            break;
        case USB_REQUEST_CANCELLED:
            printf("%s: the request is cancelled", __func__);
            break;
        default:
            printf("%s: the request is failed", __func__);
            break;
    }
}

static void AcmNotifyReqCallback(const void *requestArg)
{
    struct UsbRawRequest *req = (struct UsbRawRequest *)requestArg;

    printf("%s:%d entry!", __func__, __LINE__);
    if (req == nullptr) {
        printf("%s:%d req is nullptr!", __func__, __LINE__);
        return;
    }
    struct AcmDevice *acm = (struct AcmDevice *)req->userData;
    if (acm == nullptr) {
        printf("%s:%d userData(acm) is nullptr!", __func__, __LINE__);
        return;
    }
    struct UsbCdcNotification *dr = (struct UsbCdcNotification *)req->buffer;
    if (dr == nullptr) {
        printf("%s:%d req->buffer(dr) is nullptr!", __func__, __LINE__);
        return;
    }
    unsigned int currentSize = req->actualLength;
    printf("Irqstatus:%d,actualLength:%u\n", req->status, currentSize);
}
}
static int32_t AcmWriteBufAlloc(struct AcmDevice *acm)
{
    struct AcmWb *wb = &acm->wb[0];
    int32_t i;

    for (i = 0; i < ACM_NW; i++, wb++) {
        wb->buf = (uint8_t *)OsalMemCalloc(acm->dataOutEp->maxPacketSize);
        if (!wb->buf) {
            while (i != 0) {
                --i;
                --wb;
                OsalMemFree(wb->buf);
                wb->buf = NULL;
            }
            return -HDF_ERR_MALLOC_FAIL;
        }
    }
    return HDF_SUCCESS;
}

static int32_t UsbParseConfigDescriptor(struct AcmDevice *acm, struct UsbRawConfigDescriptor *config)
{
    uint8_t i;
    uint8_t j;
    int32_t ret;

    if ((acm == nullptr) || (config == nullptr)) {
        HDF_LOGE("%{public}s:%{public}d acm or config is nullptr", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    acm->interfaceCnt = 2;
    acm->interfaceIndex[0] = 2;
    acm->interfaceIndex[1] = 3;

    for (i = 0; i < acm->interfaceCnt; i++) {
        uint8_t interfaceIndex = acm->interfaceIndex[i];
        const struct UsbRawInterface *interface = config->interface[interfaceIndex];
        uint8_t ifaceClass = interface->altsetting->interfaceDescriptor.bInterfaceClass;
        uint8_t numEndpoints = interface->altsetting->interfaceDescriptor.bNumEndpoints;

        ret = UsbRawClaimInterface(acm->devHandle, interfaceIndex);
        if (ret) {
            HDF_LOGE("%{public}s:%{public}d claim interface %{public}u failed", __func__, __LINE__, i);
            return ret;
        }

        switch (ifaceClass) {
            case USB_DDK_CLASS_COMM:
                acm->ctrlIface = interfaceIndex;
                acm->notifyEp = (struct UsbEndpoint *)OsalMemAlloc(sizeof(struct UsbEndpoint));
                if (acm->notifyEp == nullptr) {
                    HDF_LOGE("%{public}s:%{public}d allocate endpoint failed", __func__, __LINE__);
                    break;
                }
                /* get the first endpoint by default */
                acm->notifyEp->addr = interface->altsetting->endPoint[0].endpointDescriptor.bEndpointAddress;
                acm->notifyEp->interval = interface->altsetting->endPoint[0].endpointDescriptor.bInterval;
                acm->notifyEp->maxPacketSize = interface->altsetting->endPoint[0].endpointDescriptor.wMaxPacketSize;
                break;
            case USB_DDK_CLASS_CDC_DATA:
                acm->dataIface = interfaceIndex;
                for (j = 0; j < numEndpoints; j++) {
                    const struct UsbRawEndpointDescriptor *endPoint = &interface->altsetting->endPoint[j];

                    /* get bulk in endpoint */
                    if ((endPoint->endpointDescriptor.bEndpointAddress \
                        & USB_DDK_ENDPOINT_DIR_MASK) == USB_DDK_DIR_IN) {
                        acm->dataInEp = (struct UsbEndpoint *)OsalMemAlloc(sizeof(struct UsbEndpoint));
                        if (acm->dataInEp == nullptr) {
                            HDF_LOGE("%{public}s:%{public}d allocate dataInEp failed", __func__, __LINE__);
                            break;
                        }
                        acm->dataInEp->addr = endPoint->endpointDescriptor.bEndpointAddress;
                        acm->dataInEp->interval = endPoint->endpointDescriptor.bInterval;
                        acm->dataInEp->maxPacketSize = endPoint->endpointDescriptor.wMaxPacketSize;
                    } else { /* get bulk out endpoint */
                        acm->dataOutEp = (struct UsbEndpoint *)OsalMemAlloc(sizeof(struct UsbEndpoint));
                        if (acm->dataOutEp == nullptr) {
                            HDF_LOGE("%{public}s:%{public}d allocate dataOutEp failed", __func__, __LINE__);
                            break;
                        }
                        acm->dataOutEp->addr = endPoint->endpointDescriptor.bEndpointAddress;
                        acm->dataOutEp->interval = endPoint->endpointDescriptor.bInterval;
                        acm->dataOutEp->maxPacketSize = endPoint->endpointDescriptor.wMaxPacketSize;
                    }
                }
                break;
            default:
                HDF_LOGE("%{public}s:%{public}d wrong descriptor type", __func__, __LINE__);
                break;
        }
    }

    return HDF_SUCCESS;
}

static void AcmRawAllocRequest()
{
    int32_t i;

    for (i = 0; i < ACM_NW; i++) {
        g_acm->wb[i].request = UsbRawAllocRequest(g_acm->devHandle, 0, g_acm->dataOutEp->maxPacketSize);
        g_acm->wb[i].instance = g_acm;
        EXPECT_NE(nullptr,  g_acm->wb[i].request);
    }
    for (i = 0; i < ACM_NR; i++) {
        g_acm->readReq[i] = UsbRawAllocRequest(g_acm->devHandle, 0, g_acm->dataInEp->maxPacketSize);
        EXPECT_NE(nullptr,  g_acm->readReq[i]);
    }

    g_acm->ctrlReq = UsbRawAllocRequest(g_acm->devHandle, 0, USB_CTRL_REQ_SIZE);
    EXPECT_NE(nullptr,  g_acm->ctrlReq);

    g_acm->notifyReq = UsbRawAllocRequest(g_acm->devHandle, 0, g_acm->notifyEp->maxPacketSize);
    EXPECT_NE(nullptr,  g_acm->notifyReq);
}

static void AcmRawFillWriteReq()
{
    struct UsbRawFillRequestData reqData;
    int32_t ret;
    int32_t i;
    uint32_t size;
    char sendData[] = {"abcde\0"};

    size = strlen(sendData) + 1;

    size = (size > g_acm->dataOutEp->maxPacketSize) ? g_acm->dataOutEp->maxPacketSize : size;

    for (i = 0; i < 1; i++) {
        AcmWb *snd = &g_acm->wb[i];
        snd->len = size;
        if ((snd->buf == nullptr) || (g_acm->dataOutEp->maxPacketSize == 0)) {
            break;
        }
        ret = memcpy_s(snd->buf, g_acm->dataOutEp->maxPacketSize, sendData, size);
        if (ret != EOK) {
            printf("memcpy_s fial");
            break;
        }

        reqData.endPoint      = g_acm->dataOutEp->addr;
        reqData.numIsoPackets = 0;
        reqData.callback      = AcmWriteBulkCallback;
        reqData.userData      = (void *)snd;
        reqData.timeout       = USB_CTRL_SET_TIMEOUT;
        reqData.buffer        = snd->buf;
        reqData.length        = snd->len;

        ret = UsbRawFillBulkRequest(snd->request, g_acm->devHandle, &reqData);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

static void AcmRawFillReadReq()
{
    struct UsbRawFillRequestData reqData;
    int32_t ret;
    int32_t i;
    uint32_t size = g_acm->dataInEp->maxPacketSize;

    for (i = 0; i < 1; i++) {
        reqData.endPoint      = g_acm->dataInEp->addr;
        reqData.numIsoPackets = 0;
        reqData.callback      = AcmReadBulkCallback;
        reqData.userData      = (void *)g_acm;
        reqData.timeout       = USB_CTRL_SET_TIMEOUT;
        reqData.length        = size;

        ret = UsbRawFillBulkRequest(g_acm->readReq[i], g_acm->devHandle, &reqData);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}


static void AcmRawFillIntReq()
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    uint32_t size = g_acm->notifyEp->maxPacketSize;

    fillRequestData.endPoint = g_acm->notifyEp->addr;
    fillRequestData.length = size;
    fillRequestData.numIsoPackets = 0;
    fillRequestData.callback = AcmNotifyReqCallback;
    fillRequestData.userData = (void *)g_acm;
    fillRequestData.timeout = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillInterruptRequest(g_acm->notifyReq, g_acm->devHandle, &fillRequestData);
    EXPECT_EQ(HDF_SUCCESS, ret);
}


static void AcmRawFillCtrlReq()
{
    struct UsbControlRequestData ctrlReq;
    unsigned char setup[100] = {0};
    int32_t ret;

    g_acm->lineCoding.dwDTERate = CpuToLe32(DATARATE);
    g_acm->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    g_acm->lineCoding.bParityType = USB_CDC_NO_PARITY;
    g_acm->lineCoding.bDataBits = DATA_BITS_LENGTH;

    ctrlReq.requestType = USB_DDK_DIR_OUT | USB_DDK_TYPE_CLASS | USB_DDK_RECIP_INTERFACE;
    ctrlReq.requestCmd  = USB_DDK_CDC_REQ_SET_LINE_CODING;
    ctrlReq.value       = CpuToLe16(0);
    ctrlReq.index       = 2;
    ctrlReq.data        = (unsigned char *)&g_acm->lineCoding;
    ctrlReq.length      = sizeof(struct UsbCdcLineCoding);
    ctrlReq.timeout     = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlSetup(setup, &ctrlReq);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

static void AcmRawInit()
{
    int32_t ret;
    struct UsbDeviceDescriptor desc;
    g_acm->busNum = 1U;
    g_acm->devAddr = 2U;

    ret = UsbRawInit(NULL);
    EXPECT_EQ(HDF_SUCCESS, ret);

    g_devHandle = UsbRawOpenDevice(NULL, g_acm->busNum, g_acm->devAddr);
    EXPECT_NE(nullptr,  g_devHandle);
    g_acm->devHandle = g_devHandle;

    ret = UsbRawGetConfiguration(g_acm->devHandle, &g_activeConfig);
    EXPECT_EQ(HDF_SUCCESS, ret);
    g_dev = UsbRawGetDevice(g_acm->devHandle);
    EXPECT_NE(nullptr,  g_dev);
    ret = UsbRawGetConfigDescriptor(g_dev, g_activeConfig, &g_acm->config);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = UsbRawGetDeviceDescriptor(g_dev, &desc);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = UsbParseConfigDescriptor(g_acm, g_acm->config);
    EXPECT_EQ(HDF_SUCCESS, ret);

    AcmRawAllocRequest();
    ret = AcmWriteBufAlloc(g_acm);
    EXPECT_EQ(HDF_SUCCESS, ret);
    AcmRawFillWriteReq();
    AcmRawFillReadReq();
    AcmRawFillIntReq();
    AcmRawFillCtrlReq();
}

void UsbRawSdkIfTestIo::SetUpTestCase()
{
    g_acm = &g_deviceService;
    AcmRawInit();
    UsbStartIo(g_acm);
}

/**
 * @tc.number    : CheckRawSdkIfSendControlRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSendControlRequest001, TestSize.Level1)
{
    struct UsbControlRequestData ctrlReq;
    int32_t ret;

    g_acm->lineCoding.dwDTERate = CpuToLe32(DATARATE);
    g_acm->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    g_acm->lineCoding.bParityType = USB_CDC_NO_PARITY;
    g_acm->lineCoding.bDataBits = DATA_BITS_LENGTH;

    ctrlReq.requestType = USB_DDK_DIR_OUT | USB_DDK_TYPE_CLASS | USB_DDK_RECIP_INTERFACE;
    ctrlReq.requestCmd  = USB_DDK_CDC_REQ_SET_LINE_CODING;
    ctrlReq.value       = CpuToLe16(0);
    ctrlReq.index       = 0;
    ctrlReq.data        = (unsigned char *)&g_acm->lineCoding;
    ctrlReq.length      = sizeof(struct UsbCdcLineCoding);
    ctrlReq.timeout     = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawSendControlRequest(NULL, g_acm->devHandle, &ctrlReq);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSendControlRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSendControlRequest002, TestSize.Level1)
{
    struct UsbControlRequestData ctrlReq;
    int32_t ret;

    g_acm->lineCoding.dwDTERate = CpuToLe32(DATARATE);
    g_acm->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    g_acm->lineCoding.bParityType = USB_CDC_NO_PARITY;
    g_acm->lineCoding.bDataBits = DATA_BITS_LENGTH;

    ctrlReq.requestType = USB_DDK_DIR_OUT | USB_DDK_TYPE_CLASS | USB_DDK_RECIP_INTERFACE;
    ctrlReq.requestCmd  = USB_DDK_CDC_REQ_SET_LINE_CODING;
    ctrlReq.value       = CpuToLe16(0);
    ctrlReq.index       = 0;
    ctrlReq.data        = (unsigned char *)&g_acm->lineCoding;
    ctrlReq.length      = sizeof(struct UsbCdcLineCoding);
    ctrlReq.timeout     = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawSendControlRequest(g_acm->ctrlReq, NULL, &ctrlReq);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSendControlRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSendControlRequest003, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawSendControlRequest(g_acm->ctrlReq, g_acm->devHandle, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSendControlRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSendControlRequest004, TestSize.Level1)
{
    struct UsbControlRequestData ctrlReq;
    int32_t ret;

    g_acm->lineCoding.dwDTERate = CpuToLe32(DATARATE);
    g_acm->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    g_acm->lineCoding.bParityType = USB_CDC_NO_PARITY;
    g_acm->lineCoding.bDataBits = DATA_BITS_LENGTH;

    ctrlReq.requestType = USB_DDK_DIR_OUT | USB_DDK_TYPE_CLASS | USB_DDK_RECIP_INTERFACE;
    ctrlReq.requestCmd  = USB_DDK_CDC_REQ_SET_LINE_CODING;
    ctrlReq.value       = CpuToLe16(0);
    ctrlReq.index       = 2;
    ctrlReq.data        = (unsigned char *)&g_acm->lineCoding;
    ctrlReq.length      = sizeof(struct UsbCdcLineCoding);
    ctrlReq.timeout     = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawSendControlRequest(g_acm->ctrlReq, g_acm->devHandle, &ctrlReq);
    EXPECT_NE(HDF_ERR_IO, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSendControlRequest005
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSendControlRequest005, TestSize.Level1)
{
    struct UsbControlRequestData ctrlReq;
    int32_t ret;

    g_acm->lineCoding.dwDTERate = CpuToLe32(DATARATE);
    g_acm->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    g_acm->lineCoding.bParityType = USB_CDC_NO_PARITY;
    g_acm->lineCoding.bDataBits = DATA_BITS_LENGTH;

    ctrlReq.requestType = USB_DDK_DIR_OUT | USB_DDK_TYPE_CLASS | USB_DDK_RECIP_INTERFACE;
    ctrlReq.requestCmd  = USB_DDK_CDC_REQ_SET_LINE_CODING;
    ctrlReq.value       = CpuToLe16(0);
    ctrlReq.index       = 0;
    ctrlReq.data        = (unsigned char *)&g_acm->lineCoding;
    ctrlReq.length      = sizeof(struct UsbCdcLineCoding);
    ctrlReq.timeout     = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawSendControlRequest(NULL, NULL, &ctrlReq);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSendControlRequest006
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSendControlRequest006, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawSendControlRequest(NULL, g_acm->devHandle, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSendControlRequest007
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSendControlRequest007, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawSendControlRequest(g_acm->ctrlReq, NULL, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSendBulkRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSendBulkRequest001, TestSize.Level1)
{
    struct UsbRequestData reqData;
    int32_t ret;
    int32_t i;
    uint32_t size;
    char sendData[] = {"abcd\0"};

    size = strlen(sendData) + 1;
    size = (size > g_acm->dataOutEp->maxPacketSize) ? g_acm->dataOutEp->maxPacketSize : size;

    for (i = 0; i < 1; i++) {
        AcmWb *snd = &g_acm->wb[i];
        snd->len = size;
        ret = memcpy_s(snd->buf, g_acm->dataOutEp->maxPacketSize, sendData, size);
        if (ret) {
            printf("memcpy_s fial");
        }

        reqData.endPoint      = g_acm->dataOutEp->addr;
        reqData.timeout       = USB_CTRL_SET_TIMEOUT;
        reqData.data        = snd->buf;
        reqData.length        = snd->len;
        reqData.requested   = (int32_t *)&size;
    }

    for (i = 0; i < 1; i++) {
        AcmWb *snd = &g_acm->wb[i];
        printf("UsbRawSendBulkRequest i = [%d]\n", i);
        ret = UsbRawSendBulkRequest(snd->request, g_acm->devHandle, &reqData);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckRawSdkIfSendBulkRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSendBulkRequest002, TestSize.Level1)
{
    struct UsbRequestData reqData;
    int32_t ret;
    int32_t i;
    uint32_t size = g_acm->dataInEp->maxPacketSize;

    for (i = 0; i < 1; i++) {
        reqData.endPoint      = g_acm->dataInEp->addr;
        reqData.timeout       = USB_CTRL_SET_TIMEOUT;
        reqData.length        = size;
        reqData.data        = ((UsbRawRequest *)g_acm->readReq[i])->buffer;
        reqData.requested      = (int32_t *)&size;
    }

    for (i = 0; i < 1; i++) {
        printf("UsbRawSendBulkRequest i = [%d]\n", i);
        ret = UsbRawSendBulkRequest(g_acm->readReq[i], g_acm->devHandle, &reqData);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckRawSdkIfSendBulkRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSendBulkRequest003, TestSize.Level1)
{
    struct UsbRequestData reqData;
    int32_t ret;
    int32_t i;
    uint32_t size = g_acm->dataInEp->maxPacketSize;

    for (i = 0; i < 1; i++) {
        reqData.endPoint      = g_acm->dataInEp->addr;
        reqData.timeout       = USB_CTRL_SET_TIMEOUT;
        reqData.length        = size;
        reqData.data        = ((UsbRawRequest *)g_acm->readReq[i])->buffer;
        reqData.requested      = (int32_t *)&size;
    }

    for (i = 0; i < 1; i++) {
        printf("UsbRawSendBulkRequest i = [%d]\n", i);
        ret = UsbRawSendBulkRequest(NULL, g_acm->devHandle, &reqData);
        EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
    }
}

/**
 * @tc.number    : CheckRawSdkIfSendBulkRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSendBulkRequest004, TestSize.Level1)
{
    struct UsbRequestData reqData;
    int32_t ret;
    int32_t i;
    uint32_t size = g_acm->dataInEp->maxPacketSize;

    for (i = 0; i < 1; i++) {
        reqData.endPoint = g_acm->dataInEp->addr;
        reqData.timeout = USB_CTRL_SET_TIMEOUT;
        reqData.length = size;
        reqData.data = ((UsbRawRequest *)g_acm->readReq[i])->buffer;
        reqData.requested = (int32_t *)&size;
    }

    for (i = 0; i < 1; i++) {
        printf("UsbRawSendBulkRequest i = [%d]\n", i);
        ret = UsbRawSendBulkRequest(g_acm->readReq[i], NULL, &reqData);
        EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
    }
}

/**
 * @tc.number    : CheckRawSdkIfSendBulkRequest005
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSendBulkRequest005, TestSize.Level1)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < 1; i++) {
        printf("UsbRawSendBulkRequest i = [%d]\n", i);
        ret = UsbRawSendBulkRequest(g_acm->readReq[i], g_acm->devHandle, NULL);
        EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
    }
}

/**
 * @tc.number    : CheckRawSdkIfSendInterruptRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSendInterruptRequest001, TestSize.Level1)
{
    struct UsbRequestData reqData;
    int32_t ret;
    uint32_t size = g_acm->notifyEp->maxPacketSize;

    reqData.endPoint = g_acm->notifyEp->addr;
    reqData.length = size;
    reqData.timeout = USB_CTRL_SET_TIMEOUT;
    reqData.data = ((UsbRawRequest *)g_acm->notifyReq)->buffer;
    reqData.requested = (int32_t *)&size;

    ret = UsbRawSendInterruptRequest(g_acm->notifyReq, g_acm->devHandle, &reqData);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSendInterruptRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSendInterruptRequest002, TestSize.Level1)
{
    struct UsbRequestData reqData;
    int32_t ret;
    uint32_t size = g_acm->notifyEp->maxPacketSize;

    reqData.endPoint = g_acm->notifyEp->addr;
    reqData.length = size;
    reqData.timeout = USB_CTRL_SET_TIMEOUT;
    reqData.data        = ((UsbRawRequest *)g_acm->notifyReq)->buffer;
    reqData.requested      = (int32_t *)&size;

    ret = UsbRawSendInterruptRequest(NULL, g_acm->devHandle, &reqData);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSendInterruptRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSendInterruptRequest003, TestSize.Level1)
{
    struct UsbRequestData reqData;
    int32_t ret;
    uint32_t size = g_acm->notifyEp->maxPacketSize;

    reqData.endPoint = g_acm->notifyEp->addr;
    reqData.length = size;
    reqData.timeout = USB_CTRL_SET_TIMEOUT;
    reqData.data        = ((UsbRawRequest *)g_acm->notifyReq)->buffer;
    reqData.requested      = (int32_t *)&size;

    ret = UsbRawSendInterruptRequest(g_acm->notifyReq, NULL, &reqData);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSendInterruptRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSendInterruptRequest004, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawSendInterruptRequest(g_acm->notifyReq, g_acm->devHandle, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfFillBulkRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfFillBulkRequest003, TestSize.Level1)
{
    struct UsbRawFillRequestData reqData;
    int32_t ret;
    int32_t i;
    uint32_t size;
    char sendData[] = {"abcde\0"};

    size = strlen(sendData) + 1;

    size = (size > g_acm->dataOutEp->maxPacketSize) ? g_acm->dataOutEp->maxPacketSize : size;

    for (i = 0; i < ACM_NW; i++) {
        AcmWb *snd = &g_acm->wb[i];
        snd->len = size;
        ret = memcpy_s(snd->buf, g_acm->dataOutEp->maxPacketSize, sendData, size);
        if (ret) {
            printf("memcpy_s fial");
        }
        g_acm->transmitting++;

        reqData.endPoint      = g_acm->dataOutEp->addr;
        reqData.numIsoPackets = 0;
        reqData.callback      = AcmWriteBulkCallback;
        reqData.userData      = (void *)snd;
        reqData.timeout       = USB_CTRL_SET_TIMEOUT;
        reqData.buffer        = snd->buf;
        reqData.length        = snd->len;

        ret = UsbRawFillBulkRequest(snd->request, g_acm->devHandle, &reqData);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckRawSdkIfFillBulkRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfFillBulkRequest004, TestSize.Level1)
{
    struct UsbRawFillRequestData reqData;
    int32_t ret;
    int32_t i;
    uint32_t size = g_acm->dataInEp->maxPacketSize;

    for (i = 0; i < ACM_NR; i++) {
        reqData.endPoint      = g_acm->dataInEp->addr;
        reqData.numIsoPackets = 0;
        reqData.callback      = AcmReadBulkCallback;
        reqData.userData      = (void *)g_acm;
        reqData.timeout       = USB_CTRL_SET_TIMEOUT;
        reqData.length        = size;

        ret = UsbRawFillBulkRequest(g_acm->readReq[i], g_acm->devHandle, &reqData);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckRawSdkIfFillInterruptRequest005
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfFillInterruptRequest005, TestSize.Level1)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    uint32_t size = g_acm->notifyEp->maxPacketSize;

    fillRequestData.endPoint = g_acm->notifyEp->addr;
    fillRequestData.length = size;
    fillRequestData.numIsoPackets = 0;
    fillRequestData.callback = AcmNotifyReqCallback;
    fillRequestData.userData = (void *)g_acm;
    fillRequestData.timeout = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillInterruptRequest(g_acm->notifyReq, g_acm->devHandle, &fillRequestData);
    EXPECT_EQ(HDF_SUCCESS, ret);
}


/**
 * @tc.number    : CheckRawSdkIfSubmitRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSubmitRequest001, TestSize.Level1)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < 1; i++) {
        AcmWb *snd = &g_acm->wb[i];
        printf("UsbRawSubmitRequest i = [%d]\n", i);
        ret = UsbRawSubmitRequest(snd->request);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckRawSdkIfSubmitRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSubmitRequest002, TestSize.Level1)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < 1; i++) {
        printf("UsbRawSubmitRequest i = [%d]\n", i);
        ret = UsbRawSubmitRequest(g_acm->readReq[i]);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckRawSdkIfSubmitRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSubmitRequest003, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawSubmitRequest(g_acm->notifyReq);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSubmitRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfSubmitRequest004, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawSubmitRequest(NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfCancelRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfCancelRequest001, TestSize.Level1)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < ACM_NW; i++) {
        AcmWb *snd = &g_acm->wb[i];
        ret = UsbRawCancelRequest(snd->request);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckRawSdkIfCancelRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfCancelRequest002, TestSize.Level1)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < 1; i++) {
        ret = UsbRawCancelRequest(g_acm->readReq[i]);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckRawSdkIfCancelRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfCancelRequest003, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawCancelRequest(g_acm->notifyReq);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfCancelRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTestIo, CheckRawSdkIfCancelRequest004, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawCancelRequest(NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

}
