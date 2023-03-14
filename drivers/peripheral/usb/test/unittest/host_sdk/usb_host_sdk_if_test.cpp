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
#include "usb_host_sdk_if_test.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"
#include "usb_ddk_interface.h"
}

#define USB_PIPE_DIR_OFFSET          7

using namespace std;
using namespace testing::ext;

namespace {
class UsbHostSdkIfTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

static struct UsbSession *g_session = NULL;
static struct AcmDevice g_deviceService;
static struct AcmDevice *g_acm = &g_deviceService;
static struct UsbInterface *g_ecmDataIface = NULL;
static struct UsbInterface *g_ecmIntIface = NULL;
static UsbInterfaceHandle *g_ecmDataDevHandle = NULL;
static UsbInterfaceHandle *g_ecmIntDevHandle = NULL;

void UsbHostSdkIfTest::SetUpTestCase()
{
}

void UsbHostSdkIfTest::TearDownTestCase()
{
}

void UsbHostSdkIfTest::SetUp()
{
}

void UsbHostSdkIfTest::TearDown()
{
}

static void AcmReadBulk(struct UsbRequest *req)
{
    uint32_t size;
    int32_t status = req->compInfo.status;
    size = req->compInfo.actualLength;
    printf("Bulk status:%d,actualLength:%u\n", status, size);
    return;
}

static void AcmWriteBulk(struct UsbRequest *req)
{
    int32_t status;

    if (req == nullptr) {
        printf("%s:%d req is nullptr!", __func__, __LINE__);
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

static int32_t AcmWriteBufAlloc(struct AcmDevice *acm)
{
    int32_t i;
    struct AcmWb *wb;
    for (wb = &acm->wb[0], i = 0; i < ACM_NW; i++, wb++) {
        wb->buf = (uint8_t *)OsalMemCalloc(acm->writeSize);
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
    return 0;
}

static void AcmProcessNotification(struct AcmDevice *acm, unsigned char *buf)
{
    struct UsbCdcNotification *dr = (struct UsbCdcNotification *)buf;
    switch (dr->bNotificationType) {
        case USB_DDK_CDC_NOTIFY_NETWORK_CONNECTION:
            printf("%s - network connection: %d\n", __func__, dr->wValue);
            break;
        case USB_DDK_CDC_NOTIFY_SERIAL_STATE:
            printf("the serial State change\n");
            break;
        default:
            printf("%s-%d received: index %d len %d\n",
                __func__,
                dr->bNotificationType, dr->wIndex, dr->wLength);
    }
    return;
}

static void AcmCtrlIrq(struct UsbRequest *req)
{
    if (req == nullptr) {
        printf("%s:%d req is nullptr!", __func__, __LINE__);
        return;
    }
    int32_t retval, ret;
    struct AcmDevice *acm = (struct AcmDevice *)req->compInfo.userData;
    unsigned int expectedSize, copySize, allocSize;
    int32_t status = req->compInfo.status;
    struct UsbCdcNotification *dr = (struct UsbCdcNotification *)req->compInfo.buffer;
    unsigned int currentSize = req->compInfo.actualLength;
    printf("Irqstatus:%d,actualLength:%u\n", status, currentSize);
    switch (status) {
        case 0:
            break;
        default:
            return;
    }
    if (acm->nbIndex) {
        dr = (struct UsbCdcNotification *)acm->notificationBuffer;
    }
    if (dr == nullptr) {
        printf("%s:%d dr is nullptr!", __func__, __LINE__);
        return;
    }
    expectedSize = sizeof(struct UsbCdcNotification) + Le16ToCpu(dr->wLength);
    if (currentSize < expectedSize) {
        if (acm->nbSize < expectedSize) {
            if (acm->nbSize) {
                OsalMemFree(acm->notificationBuffer);
                acm->nbSize = 0;
            }
            allocSize = expectedSize;
            acm->notificationBuffer = (uint8_t *)OsalMemCalloc(allocSize);
            if (!acm->notificationBuffer) {
                return;
            }
            acm->nbSize = allocSize;
        }
        copySize = MIN(currentSize, expectedSize - acm->nbIndex);
        ret = memcpy_s(&acm->notificationBuffer[acm->nbIndex], acm->nbSize - acm->nbIndex,
               req->compInfo.buffer, copySize);
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

    retval = UsbSubmitRequestAsync(req);

    printf("%s:%d exit", __func__, __LINE__);
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

/**
 * @tc.number    : CheckHostSdkIfInit001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfInit001, TestSize.Level1)
{
    int32_t ret;

    ret = UsbInitHostSdk(&g_session);
    EXPECT_EQ(HDF_SUCCESS, ret);
    g_acm->session = g_session;
}

/**
 * @tc.number    : CheckHostSdkIfExit001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfExit001, TestSize.Level1)
{
    int32_t ret;

    ret = UsbExitHostSdk(g_acm->session);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfInit002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfInit002, TestSize.Level1)
{
    int32_t ret;

    ret = UsbInitHostSdk(NULL);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfExit002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfExit002, TestSize.Level1)
{
    int32_t ret;

    ret = UsbExitHostSdk(NULL);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfInit003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfInit003, TestSize.Level1)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < 100; i++)
    {
        ret = UsbInitHostSdk(&g_session);
        EXPECT_EQ(HDF_SUCCESS, ret);
        g_acm->session = g_session;
        ret = UsbExitHostSdk(g_acm->session);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckHostSdkIfInit004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfInit004, TestSize.Level1)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < 100; i++)
    {
        ret = UsbInitHostSdk(NULL);
        EXPECT_EQ(HDF_SUCCESS, ret);
        ret = UsbExitHostSdk(NULL);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckHostSdkIfInit005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfInit005, TestSize.Level1)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < 100; i++)
    {
        ret = UsbInitHostSdk(NULL);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckHostSdkIfExit002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfExit003, TestSize.Level1)
{
    int32_t ret;
    int32_t i;
    for (i = 0; i < 100; i++)
    {
        ret = UsbExitHostSdk(NULL);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}


/**
 * @tc.number    : CheckHostSdkIfInit006
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfInit006, TestSize.Level1)
{
    int32_t ret;

    ret = UsbInitHostSdk(&g_session);
    EXPECT_EQ(HDF_SUCCESS, ret);
    g_acm->session = g_session;
}

/**
 * @tc.number    : CheckHostSdkIfClaimInterface001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfClaimInterface001, TestSize.Level1)
{
    g_acm->busNum = 1U;
    g_acm->devAddr = 2U;
    g_acm->interfaceIndex = 1U;

    g_acm->dataIface =  UsbClaimInterface(g_acm->session, g_acm->busNum, g_acm->devAddr, g_acm->interfaceIndex);
    EXPECT_NE(nullptr,  g_acm->dataIface);
}

/**
 * @tc.number    : CheckHostSdkIfReleaseInterface001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfReleaseInterface001, TestSize.Level1)
{
    int32_t ret;

    ret = UsbReleaseInterface(g_acm->dataIface);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfClaimInterface002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfClaimInterface002, TestSize.Level1)
{
    g_acm->busNum = 1U;
    g_acm->devAddr = 2U;
    g_acm->interfaceIndex = 0U;

    g_acm->intIface =  UsbClaimInterface(g_acm->session, g_acm->busNum, g_acm->devAddr, g_acm->interfaceIndex);
    EXPECT_NE(nullptr,  g_acm->intIface);
}

/**
 * @tc.number    : CheckHostSdkIfReleaseInterface002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfReleaseInterface002, TestSize.Level1)
{
    int32_t ret;

    ret = UsbReleaseInterface(g_acm->intIface);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfClaimInterface003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfClaimInterface003, TestSize.Level1)
{
    g_acm->busNum = 1U;
    g_acm->devAddr = 2U;
    g_acm->interfaceIndex = 255U;

    g_acm->ctrIface =  UsbClaimInterface(g_acm->session, g_acm->busNum, g_acm->devAddr, g_acm->interfaceIndex);
    EXPECT_NE(nullptr,  g_acm->ctrIface);
}

/**
 * @tc.number    : CheckHostSdkIfReleaseInterface003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfReleaseInterface003, TestSize.Level1)
{
    int32_t ret;

    ret = UsbReleaseInterface(g_acm->ctrIface);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfReleaseInterface004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfReleaseInterface004, TestSize.Level1)
{
    int32_t ret;

    ret = UsbReleaseInterface(NULL);
    EXPECT_NE(HDF_SUCCESS, ret);
}


/**
 * @tc.number    : CheckHostSdkIfClaimInterface004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfClaimInterface004, TestSize.Level1)
{
    g_acm->busNum = 1U;
    g_acm->devAddr = 2U;
    g_acm->interfaceIndex = 2U;

    g_acm->dataIface =  UsbClaimInterface(g_acm->session, g_acm->busNum, g_acm->devAddr, g_acm->interfaceIndex);
    EXPECT_NE(nullptr,  g_acm->dataIface);
}

/**
 * @tc.number    : CheckHostSdkIfClaimInterface005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfClaimInterface005, TestSize.Level1)
{
    g_acm->busNum = 1U;
    g_acm->devAddr = 2U;
    g_acm->interfaceIndex = 3U;

    g_acm->dataIface =  UsbClaimInterface(g_acm->session, g_acm->busNum, g_acm->devAddr, g_acm->interfaceIndex);
    EXPECT_NE(nullptr,  g_acm->dataIface);
}

/**
 * @tc.number    : CheckHostSdkIfClaimInterface006
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfClaimInterface006, TestSize.Level1)
{
    g_acm = &g_deviceService;
    g_acm->busNum = 1U;
    g_acm->devAddr = 2U;

    g_acm->interfaceIndex = 3U;
    g_acm->dataIface =  UsbClaimInterface(g_acm->session, g_acm->busNum, g_acm->devAddr, g_acm->interfaceIndex);
    EXPECT_NE(nullptr,  g_acm->dataIface);

    g_acm->interfaceIndex = 2U;
    g_acm->intIface =  UsbClaimInterface(g_acm->session, g_acm->busNum, g_acm->devAddr, g_acm->interfaceIndex);
    EXPECT_NE(nullptr,  g_acm->intIface);

    g_acm->interfaceIndex = 0U;
    g_ecmIntIface =  UsbClaimInterface(g_acm->session, g_acm->busNum, g_acm->devAddr, g_acm->interfaceIndex);
    EXPECT_NE(nullptr,  g_ecmIntIface);

    g_acm->interfaceIndex = 1U;
    g_ecmDataIface =  UsbClaimInterface(g_acm->session, g_acm->busNum, g_acm->devAddr, g_acm->interfaceIndex);
    EXPECT_NE(nullptr,  g_ecmDataIface);

    g_acm->interfaceIndex = 255U;
    g_acm->ctrIface =  UsbClaimInterface(g_acm->session, g_acm->busNum, g_acm->devAddr, g_acm->interfaceIndex);
    EXPECT_NE(nullptr,  g_acm->ctrIface);
}

/**
 * @tc.number    : CheckHostSdkIfOpenInterface001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfOpenInterface001, TestSize.Level1)
{
    g_acm->data_devHandle = UsbOpenInterface(g_acm->dataIface);
    EXPECT_NE(nullptr,  g_acm->data_devHandle);
}

/**
 * @tc.number    : CheckHostSdkIfCloseInterface001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfCloseInterface001, TestSize.Level1)
{
    int32_t ret;

    ret = UsbCloseInterface(g_acm->data_devHandle);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfOpenInterface002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfOpenInterface002, TestSize.Level1)
{
    g_acm->int_devHandle = UsbOpenInterface(g_acm->intIface);
    EXPECT_NE(nullptr,  g_acm->int_devHandle);
}

/**
 * @tc.number    : CheckHostSdkIfCloseInterface002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfCloseInterface002, TestSize.Level1)
{
    int32_t ret;

    ret = UsbCloseInterface(g_acm->int_devHandle);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfOpenInterface003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfOpenInterface003, TestSize.Level1)
{
    g_acm->ctrl_devHandle = UsbOpenInterface(g_acm->ctrIface);
    EXPECT_NE(nullptr,  g_acm->ctrl_devHandle);
}

/**
 * @tc.number    : CheckHostSdkIfCloseInterface003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfCloseInterface003, TestSize.Level1)
{
    int32_t ret;

    ret = UsbCloseInterface(NULL);
    EXPECT_NE(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfCloseInterface003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfCloseInterface004, TestSize.Level1)
{
    int32_t ret;

    ret = UsbCloseInterface(g_acm->ctrl_devHandle);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfOpenInterface004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfOpenInterface004, TestSize.Level1)
{
    g_acm->data_devHandle = UsbOpenInterface(NULL);
    EXPECT_EQ(nullptr,  g_acm->data_devHandle);
}

/**
 * @tc.number    : CheckHostSdkIfOpenInterface005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfOpenInterface005, TestSize.Level1)
{
    int32_t i;

    for (i = 0; i < 100; i++)
    {
        g_acm->data_devHandle = UsbOpenInterface(g_acm->dataIface);
        EXPECT_NE(nullptr,  g_acm->data_devHandle);
    }
}

/**
 * @tc.number    : CheckHostSdkIfOpenInterface006
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfOpenInterface006, TestSize.Level1)
{
    g_acm->data_devHandle = UsbOpenInterface(g_acm->dataIface);
    EXPECT_NE(nullptr,  g_acm->data_devHandle);
    g_acm->int_devHandle = UsbOpenInterface(g_acm->intIface);
    EXPECT_NE(nullptr,  g_acm->int_devHandle);
    g_acm->ctrl_devHandle = UsbOpenInterface(g_acm->ctrIface);
    EXPECT_NE(nullptr,  g_acm->ctrl_devHandle);
    g_ecmDataDevHandle = UsbOpenInterface(g_ecmDataIface);
    EXPECT_NE(nullptr,  g_ecmDataDevHandle);
    g_ecmIntDevHandle = UsbOpenInterface(g_ecmIntIface);
    EXPECT_NE(nullptr,  g_ecmIntDevHandle);
}

/**
 * @tc.number    : CheckHostSdkIfSelectInterfaceSetting001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfSelectInterfaceSetting001, TestSize.Level1)
{
    int32_t ret;
    int32_t settingIndex = 0;

    ret = UsbSelectInterfaceSetting(g_ecmDataDevHandle, settingIndex, &g_ecmDataIface);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfSelectInterfaceSetting002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfSelectInterfaceSetting002, TestSize.Level1)
{
    int32_t ret;
    int32_t settingIndex = 10;

    ret = UsbSelectInterfaceSetting(g_ecmDataDevHandle, settingIndex, &g_ecmDataIface);
    EXPECT_EQ(HDF_FAILURE, ret);
}

/**
 * @tc.number    : CheckHostSdkIfSelectInterfaceSetting003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfSelectInterfaceSetting003, TestSize.Level1)
{
    int32_t ret;
    int32_t settingIndex = 100;

    ret = UsbSelectInterfaceSetting(g_ecmDataDevHandle, settingIndex, &g_ecmDataIface);
    EXPECT_EQ(HDF_FAILURE, ret);
}

/**
 * @tc.number    : CheckHostSdkIfSelectInterfaceSetting004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfSelectInterfaceSetting004, TestSize.Level1)
{
    int32_t ret;
    int32_t settingIndex = 200;

    ret = UsbSelectInterfaceSetting(g_ecmDataDevHandle, settingIndex, &g_ecmDataIface);
    EXPECT_EQ(HDF_FAILURE, ret);
}

/**
 * @tc.number    : CheckHostSdkIfSelectInterfaceSetting005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfSelectInterfaceSetting005, TestSize.Level1)
{
    int32_t ret;
    int32_t settingIndex = 255;

    ret = UsbSelectInterfaceSetting(g_ecmDataDevHandle, settingIndex, &g_ecmDataIface);
    EXPECT_EQ(HDF_FAILURE, ret);
}

/**
 * @tc.number    : CheckHostSdkIfSelectInterfaceSetting006
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfSelectInterfaceSetting006, TestSize.Level1)
{
    int32_t ret;
    int32_t settingIndex = 1;

    ret = UsbSelectInterfaceSetting(g_ecmDataDevHandle, settingIndex, &g_ecmDataIface);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfClaimInterface007
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfClaimInterface007, TestSize.Level1)
{
    g_acm->busNum = 1U;
    g_acm->devAddr = 2U;
    g_acm->interfaceIndex = 0U;

    g_ecmIntIface =  UsbClaimInterface(g_acm->session, g_acm->busNum, g_acm->devAddr, g_acm->interfaceIndex);
    EXPECT_NE(nullptr,  g_ecmIntIface);
}

/**
 * @tc.number    : CheckHostSdkIfClaimInterface008
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfClaimInterface008, TestSize.Level1)
{
    g_acm->busNum = 100U;
    g_acm->devAddr = 200U;
    g_acm->interfaceIndex = 100U;

    g_ecmDataIface =  UsbClaimInterface(g_acm->session, g_acm->busNum, g_acm->devAddr, g_acm->interfaceIndex);
    EXPECT_EQ(nullptr,  g_ecmDataIface);
}

/**
 * @tc.number    : CheckHostSdkIfClaimInterface009
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfClaimInterface009, TestSize.Level1)
{
    g_acm->busNum = 1U;
    g_acm->devAddr = 2U;
    g_acm->interfaceIndex = 1U;

    g_ecmDataIface =  UsbClaimInterface(g_acm->session, g_acm->busNum, g_acm->devAddr, g_acm->interfaceIndex);
    EXPECT_NE(nullptr,  g_ecmDataIface);
}

/**
 * @tc.number    : CheckHostSdkIfGetPipe001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfGetPipe001, TestSize.Level1)
{
    int32_t ret;
    struct UsbPipeInfo p;
    int32_t i;

    for (i = 0;  i <= g_acm->dataIface->info.pipeNum; i++) {
        ret = UsbGetPipeInfo(NULL, g_acm->dataIface->info.curAltSetting, i, &p);
        if (ret < 0) {
            continue;
        }
        if ((p.pipeDirection == USB_PIPE_DIRECTION_IN) && (p.pipeType == USB_PIPE_TYPE_BULK)) {
            struct UsbPipeInfo *pi = (UsbPipeInfo *)OsalMemCalloc(sizeof(*pi));
            EXPECT_NE(nullptr,  pi);
            p.interfaceId = g_acm->dataIface->info.interfaceIndex;
            *pi = p;
            g_acm->dataInPipe = pi;
            break;
        }
    }
    EXPECT_EQ(nullptr,  g_acm->dataInPipe);
}

/**
 * @tc.number    : CheckHostSdkIfGetPipe002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfGetPipe002, TestSize.Level1)
{
    int32_t ret;
    struct UsbPipeInfo p;
    int32_t i;

    for (i = 0;  i <= g_acm->dataIface->info.pipeNum; i++) {
        ret = UsbGetPipeInfo(g_acm->data_devHandle, g_acm->dataIface->info.curAltSetting, i, &p);
        if (ret < 0) {
            continue;
        }
        if ((p.pipeDirection == USB_PIPE_DIRECTION_IN) && (p.pipeType == USB_PIPE_TYPE_BULK)) {
            struct UsbPipeInfo *pi = (UsbPipeInfo *)OsalMemCalloc(sizeof(*pi));
            EXPECT_NE(nullptr,  pi);
            p.interfaceId = g_acm->dataIface->info.interfaceIndex;
            *pi = p;
            g_acm->dataInPipe = pi;
            break;
        }
    }
    EXPECT_NE(nullptr,  g_acm->dataInPipe);
}

/**
 * @tc.number    : CheckHostSdkIfGetPipe003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfGetPipe003, TestSize.Level1)
{
    int32_t ret;
    struct UsbPipeInfo p;
    int32_t i;

    for (i = 0;  i <= g_acm->dataIface->info.pipeNum; i++) {
        ret = UsbGetPipeInfo(NULL, g_acm->dataIface->info.curAltSetting, i, &p);
        if (ret < 0) {
            continue;
        }
        if ((p.pipeDirection == USB_PIPE_DIRECTION_OUT) && (p.pipeType == USB_PIPE_TYPE_BULK)) {
            struct UsbPipeInfo *pi = (UsbPipeInfo *)OsalMemCalloc(sizeof(*pi));
            EXPECT_NE(nullptr,  pi);
            p.interfaceId = g_acm->dataIface->info.interfaceIndex;
            *pi = p;
            g_acm->dataOutPipe = pi;
            break;
        }
    }
    EXPECT_EQ(nullptr,  g_acm->dataOutPipe);
}

/**
 * @tc.number    : CheckHostSdkIfGetPipe004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfGetPipe004, TestSize.Level1)
{
    int32_t ret;
    struct UsbPipeInfo p;
    int32_t i;

    for (i = 0;  i <= g_acm->dataIface->info.pipeNum; i++) {
        ret = UsbGetPipeInfo(g_acm->data_devHandle, g_acm->dataIface->info.curAltSetting, i, &p);
        if (ret < 0) {
            continue;
        }
        if ((p.pipeDirection == USB_PIPE_DIRECTION_OUT) && (p.pipeType == USB_PIPE_TYPE_BULK)) {
            struct UsbPipeInfo *pi = (UsbPipeInfo *)OsalMemCalloc(sizeof(*pi));
            EXPECT_NE(nullptr,  pi);
            p.interfaceId = g_acm->dataIface->info.interfaceIndex;
            *pi = p;
            g_acm->dataOutPipe = pi;
            break;
        }
    }
    EXPECT_NE(nullptr,  g_acm->dataOutPipe);
}

/**
 * @tc.number    : CheckHostSdkIfGetPipe005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfGetPipe005, TestSize.Level1)
{
    int32_t ret;
    struct UsbPipeInfo p;
    int32_t i;

    for (i = 0;  i <= g_acm->intIface->info.pipeNum; i++) {
        ret = UsbGetPipeInfo(NULL, g_acm->intIface->info.curAltSetting, i, &p);
        if (ret < 0) {
            continue;
        }
        if ((p.pipeDirection == USB_PIPE_DIRECTION_IN) && (p.pipeType == USB_PIPE_TYPE_INTERRUPT)) {
            struct UsbPipeInfo *pi = (UsbPipeInfo *)OsalMemCalloc(sizeof(*pi));
            p.interfaceId = g_acm->intIface->info.interfaceIndex;
            *pi = p;
            g_acm->intPipe = pi;
            break;
        }
    }
    EXPECT_EQ(nullptr,  g_acm->intPipe);
}

/**
 * @tc.number    : CheckHostSdkIfGetPipe006
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfGetPipe006, TestSize.Level1)
{
    int32_t ret;
    struct UsbPipeInfo p;
    int32_t i;

    for (i = 0;  i <= g_acm->intIface->info.pipeNum; i++) {
        ret = UsbGetPipeInfo(g_acm->int_devHandle, g_acm->intIface->info.curAltSetting, i, &p);
        if (ret < 0) {
            continue;
        }
        if ((p.pipeDirection == USB_PIPE_DIRECTION_IN) && (p.pipeType == USB_PIPE_TYPE_INTERRUPT)) {
            struct UsbPipeInfo *pi = (UsbPipeInfo *)OsalMemCalloc(sizeof(*pi));
            p.interfaceId = g_acm->intIface->info.interfaceIndex;
            *pi = p;
            g_acm->intPipe = pi;
            break;
        }
    }
    EXPECT_NE(nullptr,  g_acm->intPipe);
}

/**
 * @tc.number    : CheckHostSdkIfGetPipe007
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfGetPipe007, TestSize.Level1)
{
    int32_t ret;
    struct UsbPipeInfo p;
    int32_t i;

    g_acm->interfaceIndex = 255;

    for (i = 0;  i <= g_acm->ctrIface->info.pipeNum; i++) {
        ret = UsbGetPipeInfo(NULL, g_acm->ctrIface->info.curAltSetting, i, &p);
        if (ret < 0) {
            continue;
        }
        if ((p.pipeDirection == USB_PIPE_DIRECTION_OUT) && (p.pipeType == USB_PIPE_TYPE_CONTROL)) {
            struct UsbPipeInfo *pi = (UsbPipeInfo *)OsalMemCalloc(sizeof(*pi));
            p.interfaceId = g_acm->interfaceIndex;
            *pi = p;
            g_acm->ctrPipe = pi;
            break;
        }
    }
    EXPECT_EQ(nullptr,  g_acm->ctrPipe);
}

/**
 * @tc.number    : CheckHostSdkIfGetPipe008
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfGetPipe008, TestSize.Level1)
{
    int32_t ret;
    struct UsbPipeInfo p;
    int32_t i;

    g_acm->interfaceIndex = 255;

    for (i = 0;  i <= g_acm->ctrIface->info.pipeNum; i++) {
        ret = UsbGetPipeInfo(g_acm->ctrl_devHandle, g_acm->ctrIface->info.curAltSetting, i, &p);
        if (ret < 0) {
            continue;
        }
        if ((p.pipeDirection == USB_PIPE_DIRECTION_OUT) && (p.pipeType == USB_PIPE_TYPE_CONTROL)) {
            struct UsbPipeInfo *pi = (UsbPipeInfo *)OsalMemCalloc(sizeof(*pi));
            p.interfaceId = g_acm->interfaceIndex;
            *pi = p;
            g_acm->ctrPipe = pi;
            break;
        }
    }
    EXPECT_NE(nullptr,  g_acm->ctrPipe);
}

/**
 * @tc.number    : CheckHostSdkIfGetPipe009
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfGetPipe009, TestSize.Level1)
{
    int32_t ret;
    struct UsbPipeInfo p;

    ret = UsbGetPipeInfo(NULL, 0, 0, &p);
    EXPECT_NE(HDF_SUCCESS,  ret);
}

/**
 * @tc.number    : CheckHostSdkIfGetPipe010
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfGetPipe010, TestSize.Level1)
{
    int32_t ret;

    ret = UsbGetPipeInfo(g_acm->ctrl_devHandle, 0, 0, NULL);
    EXPECT_NE(HDF_SUCCESS,  ret);

}

/**
 * @tc.number    : CheckHostSdkIfGetPipe0011
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfGetPipe011, TestSize.Level1)
{
    int32_t ret;
    struct UsbPipeInfo p;

    ret = UsbGetPipeInfo(g_acm->ctrl_devHandle, 0, 0, &p);
    EXPECT_EQ(HDF_SUCCESS,  ret);
}

/**
 * @tc.number    : CheckHostSdkIfAllocRequest001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfAllocRequest001, TestSize.Level1)
{
    int32_t i;

    g_acm->readSize = g_acm->dataInPipe->maxPacketSize;
    printf("------readSize = [%d]------\n", g_acm->readSize);
    for (i = 0; i < ACM_NR; i++) {
        g_acm->readReq[i] = UsbAllocRequest(NULL, 0, g_acm->readSize);
        EXPECT_EQ(nullptr,  g_acm->readReq[i]);
    }
}

/**
 * @tc.number    : CheckHostSdkIfAllocRequest002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfAllocRequest002, TestSize.Level1)
{
    int32_t i;

    g_acm->readSize = g_acm->dataInPipe->maxPacketSize;
    printf("------readSize = [%d]------\n", g_acm->readSize);
    for (i = 0; i < ACM_NR; i++) {
        g_acm->readReq[i] = UsbAllocRequest(g_acm->data_devHandle, 0, g_acm->readSize);
        EXPECT_NE(nullptr,  g_acm->readReq[i]);
    }
}

/**
 * @tc.number    : CheckHostSdkIfFreeRequest001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFreeRequest001, TestSize.Level1)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < ACM_NR; i++) {
        ret = UsbFreeRequest(g_acm->readReq[i]);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckHostSdkIfAllocRequest003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfAllocRequest003, TestSize.Level1)
{
    int32_t i;
    int32_t ret;

    g_acm->writeSize = g_acm->dataOutPipe->maxPacketSize;
    ret = AcmWriteBufAlloc(g_acm);
    EXPECT_EQ(HDF_SUCCESS, ret);

    for (i = 0; i < ACM_NW; i++) {
        g_acm->wb[i].request = UsbAllocRequest(NULL, 0, g_acm->writeSize);
        g_acm->wb[i].instance = g_acm;
        EXPECT_EQ(nullptr,  g_acm->wb[i].request);
    }
}

/**
 * @tc.number    : CheckHostSdkIfAllocRequest004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfAllocRequest004, TestSize.Level1)
{
    int32_t i;
    int32_t ret;

    g_acm->writeSize = g_acm->dataOutPipe->maxPacketSize;
    ret = AcmWriteBufAlloc(g_acm);
    EXPECT_EQ(HDF_SUCCESS, ret);

    for (i = 0; i < ACM_NW; i++) {
        g_acm->wb[i].request = UsbAllocRequest(g_acm->data_devHandle, 0, g_acm->writeSize);
        g_acm->wb[i].instance = g_acm;
        EXPECT_NE(nullptr,  g_acm->wb[i].request);
    }
}

/**
 * @tc.number    : CheckHostSdkIfFreeRequest002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFreeRequest002, TestSize.Level1)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < ACM_NR; i++) {
        ret = UsbFreeRequest(g_acm->wb[i].request);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckHostSdkIfAllocRequest005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfAllocRequest005, TestSize.Level1)
{
    g_acm->intSize = g_acm->intPipe->maxPacketSize;
    g_acm->notifyReq = UsbAllocRequest(NULL, 0, g_acm->intSize);
    EXPECT_EQ(nullptr,  g_acm->notifyReq);
}

/**
 * @tc.number    : CheckHostSdkIfAllocRequest006
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfAllocRequest006, TestSize.Level1)
{
    g_acm->intSize = g_acm->intPipe->maxPacketSize;
    g_acm->notifyReq = UsbAllocRequest(g_acm->int_devHandle, 0, g_acm->intSize);
    EXPECT_NE(nullptr,  g_acm->notifyReq);
}

/**
 * @tc.number    : CheckHostSdkIfFreeRequest003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFreeRequest003, TestSize.Level1)
{
    int32_t ret;

    ret = UsbFreeRequest(g_acm->notifyReq);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfAllocRequest007
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfAllocRequest007, TestSize.Level1)
{
    g_acm->ctrlSize = sizeof (struct UsbCdcLineCoding);
    g_acm->ctrlReq = UsbAllocRequest(NULL, 0, g_acm->ctrlSize);
    EXPECT_EQ(nullptr,  g_acm->ctrlReq);
}

/**
 * @tc.number    : CheckHostSdkIfAllocRequest008
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfAllocRequest008, TestSize.Level1)
{
    g_acm->ctrlSize = sizeof (struct UsbCdcLineCoding);
    g_acm->ctrlReq = UsbAllocRequest(g_acm->ctrl_devHandle, 0, g_acm->ctrlSize);
    EXPECT_NE(nullptr,  g_acm->ctrlReq);
}

/**
 * @tc.number    : CheckHostSdkIfFreeRequest004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFreeRequest004, TestSize.Level1)
{
    int32_t ret;

    ret = UsbFreeRequest(g_acm->ctrlReq);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfFreeRequest005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFreeRequest005, TestSize.Level1)
{
    int32_t ret;

    ret = UsbFreeRequest(NULL);
    EXPECT_NE(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfAllocRequest009
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfAllocRequest009, TestSize.Level1)
{
    int32_t i;

    g_acm->readSize = g_acm->dataInPipe->maxPacketSize;
    for (i = 0; i < ACM_NR; i++) {
        g_acm->readReq[i] = UsbAllocRequest(g_acm->data_devHandle, 0, g_acm->readSize);
        EXPECT_NE(nullptr,  g_acm->readReq[i]);
    }

    g_acm->writeSize = g_acm->dataOutPipe->maxPacketSize;
    for (int32_t i = 0; i < ACM_NW; i++) {
        g_acm->wb[i].request = UsbAllocRequest(g_acm->data_devHandle, 0, g_acm->writeSize);
        g_acm->wb[i].instance = g_acm;
        EXPECT_NE(nullptr,  g_acm->wb[i].request);
    }

    g_acm->intSize = g_acm->intPipe->maxPacketSize;
    g_acm->notifyReq = UsbAllocRequest(g_acm->int_devHandle, 0, g_acm->intSize);
    EXPECT_NE(nullptr,  g_acm->notifyReq);
    g_acm->ctrlSize = sizeof (struct UsbCdcLineCoding);
    g_acm->ctrlReq = UsbAllocRequest(g_acm->ctrl_devHandle, 0, g_acm->ctrlSize);
    EXPECT_NE(nullptr,  g_acm->ctrlReq);
}

/**
 * @tc.number    : CheckHostSdkIfAllocRequest010
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfAllocRequest010, TestSize.Level1)
{
    struct UsbRequest *req = UsbAllocRequest(NULL, 0, 0);
    EXPECT_EQ(nullptr, req);
}

/**
 * @tc.number    : CheckHostSdkIfFillRequest001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFillRequest001, TestSize.Level1)
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
        readParmas.timeout = USB_CTRL_SET_TIMEOUT;
        readParmas.dataReq.numIsoPackets = 0;
        readParmas.dataReq.directon = (UsbRequestDirection)((g_acm->dataInPipe->pipeDirection >> USB_DIR_OFFSET) \
            & 0x1);
        readParmas.dataReq.length = g_acm->readSize;
        ret = UsbFillRequest(g_acm->readReq[i], g_acm->data_devHandle, &readParmas);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckHostSdkIfFillRequest002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFillRequest002, TestSize.Level1)
{
    int32_t ret;
    struct UsbRequestParams parmas;
    int32_t i;
    char sendData[] = {"abcde\0"};
    uint32_t size = strlen(sendData) + 1;

    g_acm->writeSize = g_acm->dataOutPipe->maxPacketSize;
    size = (size > g_acm->writeSize) ? g_acm->writeSize : size;

    for (i = 0; i < 1; i++) {
        g_acm->wb[i].len = size;
        ret = memcpy_s(g_acm->wb[i].buf, g_acm->writeSize, sendData, size);
        if (ret) {
            printf("memcpy_s fial");
        }

        parmas.interfaceId = g_acm->dataOutPipe->interfaceId;
        parmas.pipeAddress = g_acm->dataOutPipe->pipeAddress;
        parmas.pipeId = g_acm->dataOutPipe->pipeId;
        parmas.callback = AcmWriteBulk;
        parmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
        parmas.timeout = USB_CTRL_SET_TIMEOUT;
        parmas.dataReq.numIsoPackets = 0;
        parmas.userData = (void *)&g_acm->wb[i];
        parmas.dataReq.length = g_acm->wb[i].len;
        parmas.dataReq.buffer = g_acm->wb[i].buf;
        ret = UsbFillRequest(g_acm->wb[i].request, g_acm->data_devHandle, &parmas);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckHostSdkIfFillRequest003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFillRequest003, TestSize.Level1)
{
    int32_t ret;
    struct UsbRequestParams intParmas;

    intParmas.userData = (void *)g_acm;
    intParmas.pipeAddress = g_acm->intPipe->pipeAddress;
    intParmas.pipeId = g_acm->intPipe->pipeId;
    intParmas.interfaceId = g_acm->intPipe->interfaceId;
    intParmas.callback = AcmCtrlIrq;
    intParmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
    intParmas.timeout = USB_CTRL_SET_TIMEOUT;
    intParmas.dataReq.numIsoPackets = 0;
    intParmas.dataReq.directon = (UsbRequestDirection)((g_acm->intPipe->pipeDirection >> USB_PIPE_DIR_OFFSET) \
        & DIRECTION_MASK);
    intParmas.dataReq.length = g_acm->intSize;
    ret = UsbFillRequest(g_acm->notifyReq, g_acm->int_devHandle, &intParmas);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfFillRequest004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFillRequest004, TestSize.Level1)
{
    int32_t ret;
    struct UsbRequestParams parmas;
    uint16_t index = 0;
    uint16_t value = 0;
    struct TestControlMsgData msgData = {0};

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
    msgData.size = sizeof (struct UsbCdcLineCoding);
    parmas.ctrlReq = UsbControlMsg(msgData);
    ret = UsbFillRequest(g_acm->ctrlReq, g_acm->ctrl_devHandle, &parmas);
    EXPECT_EQ(HDF_SUCCESS, ret);
}


/**
 * @tc.number    : CheckHostSdkIfFillRequest005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFillRequest005, TestSize.Level1)
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
        readParmas.timeout = USB_CTRL_SET_TIMEOUT;
        readParmas.dataReq.numIsoPackets = 0;
        readParmas.dataReq.directon = (UsbRequestDirection)((g_acm->dataInPipe->pipeDirection >> USB_DIR_OFFSET) \
            & 0x1);
        readParmas.dataReq.length = g_acm->readSize;
        ret = UsbFillRequest(g_acm->readReq[i], g_acm->data_devHandle, &readParmas);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckHostSdkIfFillRequest006
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFillRequest006, TestSize.Level1)
{
    int32_t ret;
    struct UsbRequestParams parmas;
    int32_t i;
    char sendData[] = {"abcde\0"};
    uint32_t size = strlen(sendData) + 1;

    g_acm->writeSize = g_acm->dataOutPipe->maxPacketSize;
    size = (size > g_acm->writeSize) ? g_acm->writeSize : size;

    for (i = 0; i < ACM_NR; i++) {
        g_acm->wb[i].len = size;
        ret = memcpy_s(g_acm->wb[i].buf, g_acm->writeSize, sendData, size);
        if (ret) {
            printf("memcpy_s fial");
        }

        parmas.interfaceId = g_acm->dataOutPipe->interfaceId;
        parmas.pipeAddress = g_acm->dataOutPipe->pipeAddress;
        parmas.pipeId = g_acm->dataOutPipe->pipeId;
        parmas.callback = AcmWriteBulk;
        parmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
        parmas.timeout = USB_CTRL_SET_TIMEOUT;
        parmas.dataReq.numIsoPackets = 0;
        parmas.userData = (void *)&g_acm->wb[i];
        parmas.dataReq.length = g_acm->wb[i].len;
        parmas.dataReq.buffer = g_acm->wb[i].buf;
        ret = UsbFillRequest(g_acm->wb[i].request, g_acm->data_devHandle, &parmas);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckHostSdkIfFillRequest007
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFillRequest007, TestSize.Level1)
{
    int32_t ret;
    struct UsbRequestParams intParmas;

    intParmas.userData = (void *)g_acm;
    intParmas.pipeAddress = g_acm->intPipe->pipeAddress;
    intParmas.pipeId = g_acm->intPipe->pipeId;
    intParmas.interfaceId = g_acm->intPipe->interfaceId;
    intParmas.callback = AcmCtrlIrq;
    intParmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
    intParmas.timeout = USB_CTRL_SET_TIMEOUT;
    intParmas.dataReq.numIsoPackets = 0;
    intParmas.dataReq.directon = (UsbRequestDirection)((g_acm->intPipe->pipeDirection >> USB_PIPE_DIR_OFFSET) \
        & DIRECTION_MASK);
    intParmas.dataReq.length = g_acm->intSize;
    ret = UsbFillRequest(g_acm->notifyReq, g_acm->int_devHandle, &intParmas);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfFillRequest008
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFillRequest008, TestSize.Level1)
{
    int32_t ret;
    struct UsbRequestParams parmas;
    uint16_t index = 0;
    uint16_t value = 0;
    struct TestControlMsgData msgData = {0};

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
    msgData.size = sizeof (struct UsbCdcLineCoding);
    parmas.ctrlReq = UsbControlMsg(msgData);
    ret = UsbFillRequest(g_acm->ctrlReq, g_acm->ctrl_devHandle, &parmas);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfFillRequest009
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFillRequest009, TestSize.Level1)
{
    int32_t ret;

    ret = UsbFillRequest(NULL, NULL, NULL);
    EXPECT_NE(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfFillRequest010
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFillRequest010, TestSize.Level1)
{
    int32_t ret;
    struct UsbRequestParams params;

    ret = UsbFillRequest(NULL, NULL, &params);
    EXPECT_NE(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfFillRequest011
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFillRequest011, TestSize.Level1)
{
    int32_t ret;
    UsbInterfaceHandle interfaceHandle;

    ret = UsbFillRequest(NULL, &interfaceHandle, NULL);
    EXPECT_NE(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfFillRequest012
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFillRequest012, TestSize.Level1)
{
    int32_t ret;
    struct UsbRequest request;

    ret = UsbFillRequest(&request, NULL, NULL);
    EXPECT_NE(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfFillRequest013
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFillRequest013, TestSize.Level1)
{
    int32_t ret;
    struct UsbRequestParams params;
    UsbInterfaceHandle interfaceHandle;

    ret = UsbFillRequest(NULL, &interfaceHandle, &params);
    EXPECT_NE(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfFillRequest014
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFillRequest014, TestSize.Level1)
{
    int32_t ret;
    UsbInterfaceHandle interfaceHandle;
    struct UsbRequest request;

    ret = UsbFillRequest(&request, &interfaceHandle, NULL);
    EXPECT_NE(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfFillRequest015
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfFillRequest015, TestSize.Level1)
{
    int32_t ret;
    struct UsbRequestParams params;
    struct UsbRequest request;

    ret = UsbFillRequest(&request, NULL, &params);
    EXPECT_NE(HDF_SUCCESS, ret);
}



/**
 * @tc.number    : CheckHostSdkIfClearInterfaceHalt001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfClearInterfaceHalt001, TestSize.Level1)
{
    int32_t ret;

    ret = UsbClearInterfaceHalt(g_acm->data_devHandle, g_acm->dataInPipe->pipeAddress);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfClearInterfaceHalt002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfClearInterfaceHalt002, TestSize.Level1)
{
    int32_t ret;

    ret = UsbClearInterfaceHalt(g_acm->data_devHandle, g_acm->dataOutPipe->pipeAddress);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfClearInterfaceHalt003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfClearInterfaceHalt003, TestSize.Level1)
{
    int32_t ret;

    ret = UsbClearInterfaceHalt(g_acm->int_devHandle, g_acm->intPipe->pipeAddress);
    EXPECT_NE(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfClearInterfaceHalt004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfClearInterfaceHalt004, TestSize.Level1)
{
    int32_t ret;

    ret = UsbClearInterfaceHalt(g_acm->ctrl_devHandle, g_acm->ctrPipe->pipeAddress);
    EXPECT_NE(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfClearInterfaceHalt005
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfClearInterfaceHalt005, TestSize.Level1)
{
    int32_t ret;

    ret = UsbClearInterfaceHalt(NULL, 0);
    EXPECT_NE(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfRemoveInterface001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfRemoveInterface001, TestSize.Level1)
{
    int32_t ret;
    UsbInterfaceStatus status = USB_INTERFACE_STATUS_REMOVE;

    ret = UsbAddOrRemoveInterface(g_acm->session, g_acm->busNum, g_acm->devAddr,
        g_acm->dataIface->info.interfaceIndex, status);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfAddInterface001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfAddInterface001, TestSize.Level1)
{
    int32_t ret;
    UsbInterfaceStatus status = USB_INTERFACE_STATUS_ADD;

    sleep(1);

    ret = UsbAddOrRemoveInterface(g_acm->session, g_acm->busNum, g_acm->devAddr,
        g_acm->dataIface->info.interfaceIndex, status);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfRemoveInterface002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfRemoveInterface002, TestSize.Level1)
{
    int32_t ret;
    UsbInterfaceStatus status = USB_INTERFACE_STATUS_REMOVE;

    sleep(1);

    ret = UsbAddOrRemoveInterface(g_acm->session, g_acm->busNum, g_acm->devAddr,
        g_acm->intIface->info.interfaceIndex, status);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfAddInterface002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfAddInterface002, TestSize.Level1)
{
    int32_t ret;
    UsbInterfaceStatus status = USB_INTERFACE_STATUS_ADD;

    sleep(1);

    ret = UsbAddOrRemoveInterface(g_acm->session, g_acm->busNum, g_acm->devAddr,
        g_acm->intIface->info.interfaceIndex, status);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfRemoveInterface003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfRemoveInterface003, TestSize.Level1)
{
    int32_t ret;
    UsbInterfaceStatus status = USB_INTERFACE_STATUS_REMOVE;

    ret = UsbAddOrRemoveInterface(g_acm->session, g_acm->busNum, g_acm->devAddr,
        g_acm->ctrIface->info.interfaceIndex, status);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfAddInterface003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTest, CheckHostSdkIfAddInterface003, TestSize.Level1)
{
    int32_t ret;
    UsbInterfaceStatus status = USB_INTERFACE_STATUS_ADD;

    ret = UsbAddOrRemoveInterface(g_acm->session, g_acm->busNum, g_acm->devAddr,
        g_acm->ctrIface->info.interfaceIndex, status);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

}
