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

namespace {
class UsbRawSdkIfTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

#define USB_RAW_IO_SLEEP_MS_TIME    500
#define USB_IO_THREAD_STACK_SIZE   8192

static struct UsbSession *g_session = NULL;
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
            printf("%s:%d UsbRawHandleRequests faile, ret = %d \n", __func__, __LINE__, ret);
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
    threadCfg.name      = (char *)("usb io thread");
    threadCfg.priority  = OSAL_THREAD_PRI_LOW;
    threadCfg.stackSize = USB_IO_THREAD_STACK_SIZE;

    ret = OsalThreadCreate(&acm->ioThread, \
                           (OsalThreadEntry)UsbIoThread, (void *)acm);
    if (ret != HDF_SUCCESS) {
        printf("%s:%d OsalThreadCreate faile, ret=%d \n", __func__, __LINE__, ret);
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

void UsbRawSdkIfTest::SetUpTestCase()
{
    g_acm = &g_deviceService;
    UsbStartIo(g_acm);
}

void UsbRawSdkIfTest::TearDownTestCase()
{
    g_acm = &g_deviceService;
    if (UsbStopIo(g_acm) != HDF_SUCCESS) {
        HDF_LOGW("%s:%d UsbStopIo error!", __func__, __LINE__);
    }
}

void UsbRawSdkIfTest::SetUp()
{
}

void UsbRawSdkIfTest::TearDown()
{
}

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
        printf("%s: write req failed, status = %d", __func__, req->status);
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

static void AcmProcessNotification(struct AcmDevice *acm, unsigned char *buf)
{
    struct UsbCdcNotification *dr = (struct UsbCdcNotification *)buf;

    printf("%s:%d entry!", __func__, __LINE__);

    switch (dr->bNotificationType) {
        case USB_DDK_CDC_NOTIFY_NETWORK_CONNECTION:
            printf("%s - network connection: %d\n", __func__, dr->wValue);
            break;
        case USB_DDK_CDC_NOTIFY_SERIAL_STATE:
            printf("the serial State change\n");
            break;
        default:
            printf("%s-%d received: index %d len %d\n", __func__, dr->bNotificationType, dr->wIndex, dr->wLength);
    }
}

static void AcmNotifyReqCallback(const void *requestArg)
{
    struct UsbRawRequest *req = (struct UsbRawRequest *)requestArg;
    if (req == nullptr) {
        return;
    }
    struct AcmDevice *acm = (struct AcmDevice *)req->userData;
    if (acm == nullptr) {
        return;
    }
    struct UsbCdcNotification *dr = (struct UsbCdcNotification *)req->buffer;
    if (dr == nullptr) {
        return;
    }
    unsigned int currentSize = req->actualLength;
    unsigned int expectedSize, copySize, allocSize;
    int32_t ret;
    if (req->status != USB_REQUEST_COMPLETED)
        goto EXIT;
    if (acm->nbIndex)
        dr = (struct UsbCdcNotification *)acm->notificationBuffer;
    expectedSize = sizeof(struct UsbCdcNotification) + Le16ToCpu(dr->wLength);
    if (currentSize < expectedSize) {
        if (acm->nbSize < expectedSize) {
            if (acm->nbSize) {
                OsalMemFree(acm->notificationBuffer);
                acm->nbSize = 0;
            }
            allocSize = expectedSize;
            acm->notificationBuffer = (uint8_t *)OsalMemCalloc(allocSize);
            if (!acm->notificationBuffer)
                goto EXIT;
            acm->nbSize = allocSize;
        }
        copySize = MIN(currentSize, expectedSize - acm->nbIndex);
        ret = memcpy_s(&acm->notificationBuffer[acm->nbIndex], acm->nbSize - acm->nbIndex,
            req->buffer, copySize);
        if (ret) {
            printf("memcpy_s fail\n");
        }
        acm->nbIndex += copySize;
        currentSize = acm->nbIndex;
    }
    if (currentSize >= expectedSize) {
        AcmProcessNotification(acm, (unsigned char *)dr);
        acm->nbIndex = 0;
    }
    if (UsbRawSubmitRequest(req))
        printf("%s - UsbRawSubmitRequest failed", __func__);
EXIT:
    printf("%s:%d exit", __func__, __LINE__);
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

static void AcmCtrlReqCallback(const void *requestArg)
{
    printf("%s:%d entry!", __func__, __LINE__);
}

static int32_t UsbParseConfigDescriptor(struct AcmDevice *acm, struct UsbRawConfigDescriptor *config)
{
    uint8_t numInterfaces;
    uint8_t i;
    uint8_t j;
    int32_t ret;
    uint8_t ifaceClass;
    uint8_t numEndpoints;
    const struct UsbRawInterface *interface = NULL;

    numInterfaces = config->configDescriptor.bNumInterfaces;
    printf("------numInterfaces = [%d]------\n", numInterfaces);
    for (i = 0; i < numInterfaces; i++) {
        interface = config->interface[i];
        ifaceClass = interface->altsetting->interfaceDescriptor.bInterfaceClass;
        numEndpoints = interface->altsetting->interfaceDescriptor.bNumEndpoints;

        ret = UsbRawClaimInterface(acm->devHandle, i);
        if (ret) {
            printf("%s:%d claim interface %u failed\n", __func__, __LINE__, i);
            continue;
        }

        switch (ifaceClass) {
            case USB_DDK_CLASS_COMM:
                acm->ctrlIface = i;
                acm->notifyEp = (UsbEndpoint *)OsalMemAlloc(sizeof(struct UsbEndpoint));
                if (acm->notifyEp == nullptr) {
                    printf("%s:%d allocate endpoint failed\n", __func__, __LINE__);
                    return;
                }
                /* get the first endpoint by default */
                acm->notifyEp->addr = interface->altsetting->endPoint[0].endpointDescriptor.bEndpointAddress;
                acm->notifyEp->interval = interface->altsetting->endPoint[0].endpointDescriptor.bInterval;
                acm->notifyEp->maxPacketSize = interface->altsetting->endPoint[0].endpointDescriptor.wMaxPacketSize;
                break;
            case USB_DDK_CLASS_CDC_DATA:
                acm->dataIface = i;
                for (j = 0; j < numEndpoints; j++) {
                    const struct UsbRawEndpointDescriptor *endPoint = &interface->altsetting->endPoint[j];

                    /* get bulk in endpoint */
                    if ((endPoint->endpointDescriptor.bEndpointAddress \
                        & USB_DDK_ENDPOINT_DIR_MASK) == USB_DDK_DIR_IN) {
                        acm->dataInEp = (UsbEndpoint *)OsalMemAlloc(sizeof(struct UsbEndpoint));
                        if (acm->dataInEp == nullptr) {
                            printf("%s:%d allocate dataInEp failed\n", __func__, __LINE__);
                            break;
                        }
                        acm->dataInEp->addr = endPoint->endpointDescriptor.bEndpointAddress;
                        acm->dataInEp->interval = endPoint->endpointDescriptor.bInterval;
                        acm->dataInEp->maxPacketSize = endPoint->endpointDescriptor.wMaxPacketSize;
                    } else { /* get bulk out endpoint */
                        acm->dataOutEp = (UsbEndpoint *)OsalMemAlloc(sizeof(struct UsbEndpoint));
                        if (acm->dataOutEp == nullptr) {
                            printf("%s:%d allocate dataOutEp failed\n", __func__, __LINE__);
                            break;
                        }
                        acm->dataOutEp->addr = endPoint->endpointDescriptor.bEndpointAddress;
                        acm->dataOutEp->interval = endPoint->endpointDescriptor.bInterval;
                        acm->dataOutEp->maxPacketSize = endPoint->endpointDescriptor.wMaxPacketSize;
                    }
                }
                break;
            default:
                printf("%s:%d wrong descriptor type\n", __func__, __LINE__);
                break;
        }
    }

    return HDF_SUCCESS;
}

/**
 * @tc.number    : CheckRawSdkIfInit001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfInit001, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawInit(&g_session);
    EXPECT_EQ(HDF_SUCCESS, ret);
    g_acm->session = g_session;
}

/**
 * @tc.number    : CheckRawSdkIfExit001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfExit001, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawExit(g_acm->session);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfInit002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfInit002, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawInit(NULL);
    EXPECT_EQ(HDF_SUCCESS, ret);
    g_acm->session = g_session;
}

/**
 * @tc.number    : CheckRawSdkIfExit002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfExit002, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawExit(NULL);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfInit003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfInit003, TestSize.Level1)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < 100; i++)
    {
        ret = UsbRawInit(&g_session);
        EXPECT_EQ(HDF_SUCCESS, ret);
        g_acm->session = g_session;
        ret = UsbRawExit(g_acm->session);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckRawSdkIfInit004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfInit004, TestSize.Level1)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < 100; i++)
    {
        ret = UsbRawInit(NULL);
        EXPECT_EQ(HDF_SUCCESS, ret);
        ret = UsbRawExit(NULL);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckRawSdkIfInit005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfInit005, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawInit(&g_session);
    EXPECT_EQ(HDF_SUCCESS, ret);
    g_acm->session = g_session;
}

/**
 * @tc.number    : CheckRawSdkIfOpenDevice001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfOpenDevice001, TestSize.Level1)
{
    g_acm->busNum = 1U;
    g_acm->devAddr = 2U;

    g_devHandle = UsbRawOpenDevice(NULL, g_acm->busNum, g_acm->devAddr);
    EXPECT_EQ(nullptr,  g_devHandle);
    g_acm->devHandle = g_devHandle;
}

/**
 * @tc.number    : CheckRawSdkIfOpenDevice002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfOpenDevice002, TestSize.Level1)
{
    g_acm->busNum = 1U;
    g_acm->devAddr = 100U;

    g_devHandle = UsbRawOpenDevice(g_acm->session, g_acm->busNum, g_acm->devAddr);
    EXPECT_EQ(nullptr,  g_devHandle);
    g_acm->devHandle = g_devHandle;
}

/**
 * @tc.number    : CheckRawSdkIfOpenDevice003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfOpenDevice003, TestSize.Level1)
{
    g_acm->busNum = 1U;
    g_acm->devAddr = 255U;

    g_devHandle = UsbRawOpenDevice(g_acm->session, g_acm->busNum, g_acm->devAddr);
    EXPECT_EQ(nullptr,  g_devHandle);
    g_acm->devHandle = g_devHandle;
}

/**
 * @tc.number    : CheckRawSdkIfOpenDevice004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfOpenDevice004, TestSize.Level1)
{
    g_acm->busNum = 100U;
    g_acm->devAddr = 2U;

    g_devHandle = UsbRawOpenDevice(g_acm->session, g_acm->busNum, g_acm->devAddr);
    EXPECT_EQ(nullptr,  g_devHandle);
    g_acm->devHandle = g_devHandle;
}

/**
 * @tc.number    : CheckRawSdkIfOpenDevice005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfOpenDevice005, TestSize.Level1)
{
    g_acm->busNum = 255U;
    g_acm->devAddr = 2U;

    g_devHandle = UsbRawOpenDevice(g_acm->session, g_acm->busNum, g_acm->devAddr);
    EXPECT_EQ(nullptr,  g_devHandle);
    g_acm->devHandle = g_devHandle;
}

/**
 * @tc.number    : CheckRawSdkIfOpenDevice006
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfOpenDevice006, TestSize.Level1)
{
    g_acm->busNum = 1U;
    g_acm->devAddr = 2U;

    g_devHandle = UsbRawOpenDevice(g_acm->session, g_acm->busNum, g_acm->devAddr);
    EXPECT_NE(nullptr,  g_devHandle);
    g_acm->devHandle = g_devHandle;
}

/**
 * @tc.number    : CheckRawSdkIfResetDevice001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfResetDevice001, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawResetDevice(NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfResetDevice002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfResetDevice002, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawResetDevice(g_acm->devHandle);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfCloseDevice001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfCloseDevice001, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawCloseDevice(NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfCloseDevice002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfCloseDevice002, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawCloseDevice(g_acm->devHandle);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfOpenDevice007
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfOpenDevice007, TestSize.Level1)
{
    g_acm->busNum = 1U;
    g_acm->devAddr = 2U;

    g_devHandle = UsbRawOpenDevice(g_session, g_acm->busNum, g_acm->devAddr);
    EXPECT_NE(nullptr,  g_devHandle);
    g_acm->devHandle = g_devHandle;
}

/**
 * @tc.number    : CheckRawSdkIfGetConfiguration001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetConfiguration001, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawGetConfiguration(NULL, &g_activeConfig);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetConfiguration002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetConfiguration002, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawGetConfiguration(g_acm->devHandle, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetConfiguration003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetConfiguration003, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawGetConfiguration(NULL, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetConfiguration004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetConfiguration004, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawGetConfiguration(g_acm->devHandle, &g_activeConfig);
    printf("------activeConfig = [%d]------\n", g_activeConfig);
    EXPECT_EQ(HDF_SUCCESS, ret);
}


/**
 * @tc.number    : CheckRawSdkIfGetDevice001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDevice001, TestSize.Level1)
{
    g_dev = UsbRawGetDevice(NULL);
    EXPECT_EQ(nullptr,  g_dev);
}

/**
 * @tc.number    : CheckRawSdkIfGetDevice002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDevice002, TestSize.Level1)
{
    g_dev = UsbRawGetDevice(g_acm->devHandle);
    EXPECT_NE(nullptr,  g_dev);
}

/**
 * @tc.number    : CheckRawSdkIfGetConfigDescriptor001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetConfigDescriptor001, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawGetConfigDescriptor(NULL, g_activeConfig, &g_acm->config);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetConfigDescriptor002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetConfigDescriptor002, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawGetConfigDescriptor(g_dev, g_activeConfig, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetConfigDescriptor003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetConfigDescriptor003, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawGetConfigDescriptor(NULL, g_activeConfig, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetConfigDescriptor004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetConfigDescriptor004, TestSize.Level1)
{
    int32_t ret;

    printf("------activeConfig = [%d]------\n", g_activeConfig);
    ret = UsbRawGetConfigDescriptor(g_dev, g_activeConfig, &g_acm->config);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSetConfiguration001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfSetConfiguration001, TestSize.Level1)
{
    int32_t ret;
    int32_t config = 0;

    ret = UsbRawSetConfiguration(NULL, config);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSetConfiguration002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfSetConfiguration002, TestSize.Level1)
{
    int32_t ret;
    int32_t config = 0;

    ret = UsbRawSetConfiguration(g_acm->devHandle, config);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSetConfiguration003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfSetConfiguration003, TestSize.Level1)
{
    int32_t ret;
    int32_t config = 1;

    ret = UsbRawSetConfiguration(g_acm->devHandle, config);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSetConfiguration004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfSetConfiguration004, TestSize.Level1)
{
    int32_t ret;
    int32_t config = 10;

    ret = UsbRawSetConfiguration(g_acm->devHandle, config);
    EXPECT_EQ(HDF_FAILURE, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSetConfiguration005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfSetConfiguration005, TestSize.Level1)
{
    int32_t ret;
    int32_t config = 100;

    ret = UsbRawSetConfiguration(g_acm->devHandle, config);
    EXPECT_EQ(HDF_FAILURE, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSetConfiguration006
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfSetConfiguration006, TestSize.Level1)
{
    int32_t ret;
    int32_t config = 200;

    ret = UsbRawSetConfiguration(g_acm->devHandle, config);
    EXPECT_EQ(HDF_FAILURE, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSetConfiguration007
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfSetConfiguration007, TestSize.Level1)
{
    int32_t ret;
    int32_t config = 255;

    ret = UsbRawSetConfiguration(g_acm->devHandle, config);
    EXPECT_EQ(HDF_FAILURE, ret);
}

/**
 * @tc.number    : CheckRawSdkIfSetConfiguration008
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfSetConfiguration008, TestSize.Level1)
{
    int32_t ret;
    int32_t config = 1;

    ret = UsbRawSetConfiguration(g_acm->devHandle, config);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDeviceDescriptor001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDeviceDescriptor001, TestSize.Level1)
{
    struct UsbDeviceDescriptor desc;
    int32_t ret;

    ret = UsbRawGetDeviceDescriptor(NULL, &desc);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDeviceDescriptor002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDeviceDescriptor002, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawGetDeviceDescriptor(g_dev, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDeviceDescriptor003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDeviceDescriptor003, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawGetDeviceDescriptor(NULL, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDeviceDescriptor004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDeviceDescriptor004, TestSize.Level1)
{
    struct UsbDeviceDescriptor desc;
    int32_t ret;

    ret = UsbRawGetDeviceDescriptor(g_dev, &desc);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetConfigDescriptor005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetConfigDescriptor005, TestSize.Level1)
{
    int32_t ret;

    printf("------activeConfig = [%d]------\n", g_activeConfig);
    ret = UsbRawGetConfigDescriptor(g_dev, g_activeConfig, &g_acm->config);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDeviceDescriptor005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDeviceDescriptor005, TestSize.Level1)
{
    struct UsbDeviceDescriptor desc;
    int32_t ret;

    ret = UsbRawGetDeviceDescriptor(g_dev, &desc);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfClaimInterface001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfClaimInterface001, TestSize.Level1)
{
    int32_t ret;
    int32_t interfaceNumber = 1;

    ret = UsbRawClaimInterface(NULL, interfaceNumber);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfClaimInterface002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfClaimInterface002, TestSize.Level1)
{
    int32_t ret;
    int32_t interfaceNumber = 1;

    ret = UsbRawClaimInterface(g_devHandle, interfaceNumber);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfClaimInterface003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfClaimInterface003, TestSize.Level1)
{
    int32_t ret;
    int32_t interfaceNumber = 0;

    ret = UsbRawClaimInterface(g_devHandle, interfaceNumber);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfClaimInterface004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfClaimInterface004, TestSize.Level1)
{
    int32_t ret;
    int32_t interfaceNumber = 255;

    ret = UsbRawClaimInterface(g_devHandle, interfaceNumber);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfClaimInterface005
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfClaimInterface005, TestSize.Level1)
{
    int32_t ret;

    ret = UsbParseConfigDescriptor(g_acm, g_acm->config);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfReleaseInterface001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfReleaseInterface001, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawReleaseInterface(NULL, g_acm->ctrlIface);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfReleaseInterface002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfReleaseInterface002, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawReleaseInterface(g_acm->devHandle, g_acm->ctrlIface);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfReleaseInterface003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfReleaseInterface003, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawReleaseInterface(NULL, g_acm->dataIface);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfReleaseInterface004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfReleaseInterface004, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawReleaseInterface(g_acm->devHandle, g_acm->dataIface);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfClaimInterface006
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfClaimInterface006, TestSize.Level1)
{
    int32_t ret;

    ret = UsbParseConfigDescriptor(g_acm, g_acm->config);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfAllocRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfAllocRequest001, TestSize.Level1)
{
    int32_t i;
    int32_t ret;

    ret = AcmWriteBufAlloc(g_acm);
    EXPECT_EQ(HDF_SUCCESS, ret);

    for (i = 0; i < ACM_NW; i++) {
        g_acm->wb[i].request = UsbRawAllocRequest(NULL, 0, g_acm->dataOutEp->maxPacketSize);
        g_acm->wb[i].instance = g_acm;
        EXPECT_EQ(nullptr,  g_acm->wb[i].request);
    }
}

/**
 * @tc.number    : CheckRawSdkIfAllocRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfAllocRequest002, TestSize.Level1)
{
    int32_t i;
    int32_t ret;

    ret = AcmWriteBufAlloc(g_acm);
    EXPECT_EQ(HDF_SUCCESS, ret);

    for (i = 0; i < ACM_NW; i++) {
        g_acm->wb[i].request = UsbRawAllocRequest(g_acm->devHandle, 0, g_acm->dataOutEp->maxPacketSize);
        g_acm->wb[i].instance = g_acm;
        ((struct UsbHostRequest *)(g_acm->wb[i].request))->devHandle = (struct UsbDeviceHandle *)g_acm->devHandle;
        EXPECT_NE(nullptr,  g_acm->wb[i].request);
    }
}

/**
 * @tc.number    : CheckRawSdkIfAllocRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfAllocRequest003, TestSize.Level1)
{
    int32_t i;

    for (i = 0; i < ACM_NR; i++) {
        g_acm->readReq[i] = UsbRawAllocRequest(NULL, 0, g_acm->dataInEp->maxPacketSize);
        EXPECT_EQ(nullptr,  g_acm->readReq[i]);
    }
}

/**
 * @tc.number    : CheckRawSdkIfAllocRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfAllocRequest004, TestSize.Level1)
{
    int32_t i;

    for (i = 0; i < ACM_NR; i++) {
        g_acm->readReq[i] = UsbRawAllocRequest(g_acm->devHandle, 0, g_acm->dataInEp->maxPacketSize);
        ((struct UsbHostRequest *)(g_acm->readReq[i]))->devHandle = (struct UsbDeviceHandle *)g_acm->devHandle;
        EXPECT_NE(nullptr,  g_acm->readReq[i]);
    }
}

/**
 * @tc.number    : CheckRawSdkIfAllocRequest005
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfAllocRequest005, TestSize.Level1)
{
    g_acm->ctrlReq = UsbRawAllocRequest(NULL, 0, USB_CTRL_REQ_SIZE);
    EXPECT_EQ(nullptr,  g_acm->ctrlReq);
}

/**
 * @tc.number    : CheckRawSdkIfAllocRequest006
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfAllocRequest006, TestSize.Level1)
{
    g_acm->ctrlReq = UsbRawAllocRequest(g_acm->devHandle, 0, USB_CTRL_REQ_SIZE);
    ((struct UsbHostRequest *)(g_acm->ctrlReq))->devHandle = (struct UsbDeviceHandle *)g_acm->devHandle;
    EXPECT_NE(nullptr,  g_acm->ctrlReq);
}

/**
 * @tc.number    : CheckRawSdkIfAllocRequest007
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfAllocRequest007, TestSize.Level1)
{
    g_acm->notifyReq = UsbRawAllocRequest(NULL, 0, g_acm->notifyEp->maxPacketSize);
    EXPECT_EQ(nullptr,  g_acm->notifyReq);
}

/**
 * @tc.number    : CheckRawSdkIfAllocRequest008
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfAllocRequest008, TestSize.Level1)
{
    g_acm->notifyReq = UsbRawAllocRequest(g_acm->devHandle, 0, g_acm->notifyEp->maxPacketSize);
    ((struct UsbHostRequest *)(g_acm->notifyReq))->devHandle = (struct UsbDeviceHandle *)g_acm->devHandle;
    EXPECT_NE(nullptr,  g_acm->notifyReq);
}

/**
 * @tc.number    : CheckRawSdkIfFreeRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFreeRequest001, TestSize.Level1)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < ACM_NW; i++) {
        ret = UsbRawFreeRequest(g_acm->wb[i].request);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckRawSdkIfFreeRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFreeRequest002, TestSize.Level1)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < ACM_NW; i++) {
        ret = UsbRawFreeRequest(g_acm->readReq[i]);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckRawSdkIfFreeRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFreeRequest003, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawFreeRequest(g_acm->ctrlReq);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfFreeRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFreeRequest004, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawFreeRequest(g_acm->notifyReq);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfFreeRequest005
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFreeRequest005, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawFreeRequest(NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfAllocRequest009
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfAllocRequest009, TestSize.Level1)
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

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDescriptor001, TestSize.Level1)
{
    UsbRawDescriptorParam param;
    unsigned char data[100];
    int32_t ret;

    ret = UsbRawGetDescriptor(NULL, g_acm->devHandle, &param, data);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDescriptor002, TestSize.Level1)
{
    UsbRawDescriptorParam param;
    unsigned char data[100];
    int32_t ret;

    ret = UsbRawGetDescriptor(g_acm->ctrlReq, NULL, &param, data);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDescriptor003, TestSize.Level1)
{
    UsbRawDescriptorParam param;
    unsigned char data[100];
    int32_t ret;

    ret = UsbRawGetDescriptor(NULL, NULL, &param, data);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDescriptor004, TestSize.Level1)
{
    unsigned char data[100];
    int32_t ret;

    ret = UsbRawGetDescriptor(g_acm->ctrlReq, g_acm->devHandle, NULL, data);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDescriptor005, TestSize.Level1)
{
    UsbRawDescriptorParam param;
    int32_t ret;

    ret = UsbRawGetDescriptor(g_acm->ctrlReq, g_acm->devHandle, &param, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor006
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDescriptor006, TestSize.Level1)
{
    UsbRawDescriptorParam param;
    unsigned char data[100];
    int32_t ret;

    param.descType = 0;
    param.descIndex = 0;
    param.length = sizeof(data);

    ret = UsbRawGetDescriptor(g_acm->ctrlReq, g_acm->devHandle, &param, data);
    EXPECT_EQ(HDF_ERR_IO, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor007
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDescriptor007, TestSize.Level1)
{
    unsigned char data[100];
    int32_t ret;

    ret = UsbRawGetDescriptor(g_acm->ctrlReq, NULL, NULL, data);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor008
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDescriptor008, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawGetDescriptor(g_acm->ctrlReq, g_acm->devHandle, NULL, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor009
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDescriptor009, TestSize.Level1)
{
    unsigned char data[100];
    int32_t ret;

    ret = UsbRawGetDescriptor(NULL, g_acm->devHandle, NULL, data);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor010
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDescriptor010, TestSize.Level1)
{
    UsbRawDescriptorParam param;
    unsigned char data[100];
    int32_t ret;

    param.descType = 0;
    param.descIndex = 0;
    param.length = sizeof(data);

    ret = UsbRawGetDescriptor(NULL, g_acm->devHandle, &param, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor011
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDescriptor011, TestSize.Level1)
{
    UsbRawDescriptorParam param;
    unsigned char data[100];
    int32_t ret;

    param.descType = 0;
    param.descIndex = 0;
    param.length = sizeof(data);

    ret = UsbRawGetDescriptor(g_acm->ctrlReq, NULL, &param, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor012
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDescriptor012, TestSize.Level1)
{
    unsigned char data[100];
    int32_t ret;

    ret = UsbRawGetDescriptor(NULL, NULL, NULL, data);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor013
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDescriptor013, TestSize.Level1)
{
    UsbRawDescriptorParam param;
    unsigned char data[100];
    int32_t ret;

    param.descType = 0;
    param.descIndex = 0;
    param.length = sizeof(data);

    ret = UsbRawGetDescriptor(NULL, NULL, &param, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor014
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDescriptor014, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawGetDescriptor(NULL, g_acm->devHandle, NULL, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor015
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDescriptor015, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawGetDescriptor(g_acm->ctrlReq, NULL, NULL, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfGetDescriptor016
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfGetDescriptor016, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawGetDescriptor(NULL, NULL, NULL, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}





/**
 * @tc.number    : CheckRawSdkIfFillBulkRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillBulkRequest001, TestSize.Level1)
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
 * @tc.number    : CheckRawSdkIfFillBulkRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillBulkRequest002, TestSize.Level1)
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

/**
 * @tc.number    : CheckRawSdkIfFillInterruptRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillInterruptRequest001, TestSize.Level1)
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
 * @tc.number    : CheckRawSdkIfFillInterruptRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillInterruptRequest002, TestSize.Level1)
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

    ret = UsbRawFillInterruptRequest(NULL, g_acm->devHandle, &fillRequestData);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfFillInterruptRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillInterruptRequest003, TestSize.Level1)
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

    ret = UsbRawFillInterruptRequest(g_acm->notifyReq, NULL, &fillRequestData);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfFillInterruptRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillInterruptRequest004, TestSize.Level1)
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

    ret = UsbRawFillInterruptRequest(g_acm->notifyReq, g_acm->devHandle, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}


/**
 * @tc.number    : CheckRawSdkIfFillControlRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillControlRequest001, TestSize.Level1)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    int32_t completed = 0;

    fillRequestData.callback  = AcmCtrlReqCallback;
    fillRequestData.userData  = &completed;
    fillRequestData.timeout   = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlRequest(g_acm->ctrlReq, g_acm->devHandle, &fillRequestData);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfFillControlRequest002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillControlRequest002, TestSize.Level1)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    int32_t completed = 0;

    fillRequestData.callback  = AcmCtrlReqCallback;
    fillRequestData.userData  = &completed;
    fillRequestData.timeout   = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlRequest(NULL, g_acm->devHandle, &fillRequestData);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfFillControlRequest003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillControlRequest003, TestSize.Level1)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    int32_t completed = 0;

    fillRequestData.callback  = AcmCtrlReqCallback;
    fillRequestData.userData  = &completed;
    fillRequestData.timeout   = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlRequest(g_acm->ctrlReq, NULL, &fillRequestData);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfFillControlRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillControlRequest004, TestSize.Level1)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    int32_t completed = 0;

    fillRequestData.callback  = AcmCtrlReqCallback;
    fillRequestData.userData  = &completed;
    fillRequestData.timeout   = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlRequest(g_acm->ctrlReq, g_acm->devHandle, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfFillControlRequest005
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillControlRequest005, TestSize.Level1)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    int32_t completed = 0;

    fillRequestData.callback  = AcmCtrlReqCallback;
    fillRequestData.userData  = &completed;
    fillRequestData.timeout   = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlRequest(NULL, g_acm->devHandle, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfFillControlRequest006
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillControlRequest006, TestSize.Level1)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    int32_t completed = 0;

    fillRequestData.callback  = AcmCtrlReqCallback;
    fillRequestData.userData  = &completed;
    fillRequestData.timeout   = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlRequest(g_acm->ctrlReq, NULL, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfFillControlRequest007
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillControlRequest007, TestSize.Level1)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    int32_t completed = 0;

    fillRequestData.callback  = AcmCtrlReqCallback;
    fillRequestData.userData  = &completed;
    fillRequestData.timeout   = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlRequest(NULL, NULL, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfFillControlRequest008
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillControlRequest008, TestSize.Level1)
{
    struct UsbRawFillRequestData fillRequestData;
    int32_t ret;
    int32_t completed = 0;

    fillRequestData.callback  = AcmCtrlReqCallback;
    fillRequestData.userData  = &completed;
    fillRequestData.timeout   = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlRequest(NULL, NULL, &fillRequestData);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfFillIsoRequest001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillIsoRequest001, TestSize.Level1)
{

}

/**
 * @tc.number    : CheckRawSdkIfFillControlSetup001
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillControlSetup001, TestSize.Level1)
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

    ret = UsbRawFillControlSetup(NULL, &ctrlReq);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfFillControlSetup002
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillControlSetup002, TestSize.Level1)
{
    unsigned char setup[100] = {0};
    int32_t ret;

    ret = UsbRawFillControlSetup(setup, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfFillControlSetup003
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillControlSetup003, TestSize.Level1)
{
    int32_t ret;

    ret = UsbRawFillControlSetup(NULL, NULL);
    EXPECT_EQ(HDF_ERR_INVALID_PARAM, ret);
}

/**
 * @tc.number    : CheckRawSdkIfFillControlSetup004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillControlSetup004, TestSize.Level1)
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
    ctrlReq.index       = 0;
    ctrlReq.data        = (unsigned char *)&g_acm->lineCoding;
    ctrlReq.length      = sizeof(struct UsbCdcLineCoding);
    ctrlReq.timeout     = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillControlSetup(setup, &ctrlReq);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckRawSdkIfFillBulkRequest004
 * @tc.name      :
 * @tc.type      : PERFs
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillBulkRequest004, TestSize.Level1)
{
    struct UsbRawFillRequestData reqData;
    int32_t ret;
    int32_t i;
    uint32_t size = g_acm->dataInEp->maxPacketSize;

    for (i = 0; i < ACM_NW; i++) {
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
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFillInterruptRequest005, TestSize.Level1)
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
 * @tc.number    : CheckRawSdkIfFreeConfigDescriptor001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFreeConfigDescriptor001, TestSize.Level1)
{
    UsbRawFreeConfigDescriptor(NULL);
}

/**
 * @tc.number    : CheckRawSdkIfFreeConfigDescriptor002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbRawSdkIfTest, CheckRawSdkIfFreeConfigDescriptor002, TestSize.Level1)
{
    UsbRawFreeConfigDescriptor(g_acm->config);
    g_acm->config = nullptr;
}
}
