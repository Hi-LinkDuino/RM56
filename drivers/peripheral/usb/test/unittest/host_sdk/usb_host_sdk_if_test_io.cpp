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
class UsbHostSdkIfTestIo : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

static struct AcmDevice g_deviceService;
static struct AcmDevice *g_acm = &g_deviceService;

void UsbHostSdkIfTestIo::SetUpTestCase()
{
}

void UsbHostSdkIfTestIo::TearDownTestCase()
{
}

void UsbHostSdkIfTestIo::SetUp()
{
}

void UsbHostSdkIfTestIo::TearDown()
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

static void AcmCtrlIrq(struct UsbRequest *req)
{
    if (req == nullptr) {
        printf("%s:%d req is nullptr!", __func__, __LINE__);
        return;
    }
    int32_t status = req->compInfo.status;
    unsigned int currentSize = req->compInfo.actualLength;
    printf("Irqstatus:%d,actualLength:%u\n", status, currentSize);
    switch (status) {
        case 0:
            break;
        default:
            return;
    }

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
static void AcmGetPipe()
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
        }

        if ((p.pipeDirection == USB_PIPE_DIRECTION_OUT) && (p.pipeType == USB_PIPE_TYPE_BULK)) {
            struct UsbPipeInfo *pi = (UsbPipeInfo *)OsalMemCalloc(sizeof(*pi));
            EXPECT_NE(nullptr,  pi);
            p.interfaceId = g_acm->dataIface->info.interfaceIndex;
            *pi = p;
            g_acm->dataOutPipe = pi;
        }
    }

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

    g_acm->interfaceIndex = USB_CTRL_INTERFACE_ID;
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
}

static void AcmGetRequest()
{
    int32_t ret;
    int32_t i;
    g_acm->readSize = g_acm->dataInPipe->maxPacketSize;
    for (i = 0; i < ACM_NR; i++) {
        g_acm->readReq[i] = UsbAllocRequest(g_acm->data_devHandle, 0, g_acm->readSize);
        EXPECT_NE(nullptr,  g_acm->readReq[i]);
    }

    g_acm->writeSize = g_acm->dataOutPipe->maxPacketSize;
    ret = AcmWriteBufAlloc(g_acm);
    EXPECT_EQ(HDF_SUCCESS, ret);

    for (i = 0; i < ACM_NW; i++) {
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

static void AcmFillReadRequest()
{
    int32_t i;
    struct UsbRequestParams readParmas;
    int32_t ret;
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

static void AcmFillWriteRequest()
{
    struct UsbRequestParams parmas;
    char sendData[] = {"abcde\0"};
    uint32_t size = strlen(sendData) + 1;
    int32_t i;
    int32_t ret;

    g_acm->writeSize = g_acm->dataOutPipe->maxPacketSize;
    size = (size > g_acm->writeSize) ? g_acm->writeSize : size;

    for (i = 0; i < ACM_NW; i++) {
        g_acm->wb[i].len = size;
        if ((g_acm->wb[i].buf == nullptr) || (g_acm->writeSize == 0)) {
            break;
        }
        ret = memcpy_s(g_acm->wb[i].buf, g_acm->writeSize, sendData, size);
        if (ret != EOK) {
            printf("memcpy_s fial");
            break;
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

static void AcmFillIntRequest()
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

static void AcmFillCtrlRequest()
{
    int32_t ret;
    struct UsbRequestParams parmas;
    uint16_t index = 2;
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
    msgData.size = sizeof(struct UsbCdcLineCoding);
    parmas.ctrlReq = UsbControlMsg(msgData);
    ret = UsbFillRequest(g_acm->ctrlReq, g_acm->ctrl_devHandle, &parmas);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

static void AcmInit()
{
    int32_t ret;
    struct UsbSession *session = NULL;

    ret = UsbInitHostSdk(&session);
    EXPECT_EQ(HDF_SUCCESS, ret);
    g_acm->session = session;

    g_acm->busNum = 1U;
    g_acm->devAddr = 2U;
    g_acm->interfaceIndex = 3U;
    g_acm->dataIface =  UsbClaimInterface(g_acm->session, g_acm->busNum, g_acm->devAddr, g_acm->interfaceIndex);
    EXPECT_NE(nullptr,  g_acm->dataIface);

    g_acm->interfaceIndex = 2U;
    g_acm->intIface =  UsbClaimInterface(g_acm->session, g_acm->busNum, g_acm->devAddr, g_acm->interfaceIndex);
    EXPECT_NE(nullptr,  g_acm->intIface);

    g_acm->interfaceIndex = 255U;
    g_acm->ctrIface =  UsbClaimInterface(g_acm->session, g_acm->busNum, g_acm->devAddr, g_acm->interfaceIndex);
    EXPECT_NE(nullptr,  g_acm->ctrIface);

    g_acm->data_devHandle = UsbOpenInterface(g_acm->dataIface);
    EXPECT_NE(nullptr,  g_acm->data_devHandle);
    g_acm->int_devHandle = UsbOpenInterface(g_acm->intIface);
    EXPECT_NE(nullptr,  g_acm->int_devHandle);
    g_acm->ctrl_devHandle = UsbOpenInterface(g_acm->ctrIface);
    EXPECT_NE(nullptr,  g_acm->ctrl_devHandle);

    AcmGetPipe();
    AcmGetRequest();
    AcmFillReadRequest();
    AcmFillWriteRequest();
    AcmFillIntRequest();
    AcmFillCtrlRequest();
}

/**
 * @tc.number    : CheckHostSdkIfSubmitRequestSync001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTestIo, CheckHostSdkIfSubmitRequestSync001, TestSize.Level1)
{
    int32_t ret;
    int32_t i;
    AcmInit();

    for (i = 0; i < 1; i++) {
        printf("------UsbSubmitRequestSync i = [%d]------\n", i);
        ret = UsbSubmitRequestSync(g_acm->readReq[i]);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckHostSdkIfSubmitRequestSync002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTestIo, CheckHostSdkIfSubmitRequestSync002, TestSize.Level1)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < 1; i++) {
        printf("------UsbSubmitRequestSync i = [%d]------\n", i);
        ret = UsbSubmitRequestSync(g_acm->wb[i].request);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckHostSdkIfSubmitRequestSync003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTestIo, CheckHostSdkIfSubmitRequestSync003, TestSize.Level1)
{
    int32_t ret;

    ret = UsbSubmitRequestSync(g_acm->notifyReq);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfSubmitRequestSync004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTestIo, CheckHostSdkIfSubmitRequestSync004, TestSize.Level1)
{
    int32_t ret;

    ret = UsbSubmitRequestSync(g_acm->notifyReq);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfSubmitRequestAsync001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTestIo, CheckHostSdkIfSubmitRequestAsync001, TestSize.Level1)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < ACM_NR; i++) {
        ret = UsbSubmitRequestAsync(g_acm->readReq[i]);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckHostSdkIfCancelRequest001
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTestIo, CheckHostSdkIfCancelRequest001, TestSize.Level1)
{
    int32_t ret;
    int32_t i = 0;

    ret = UsbCancelRequest(g_acm->readReq[i]);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfSubmitRequestAsync002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTestIo, CheckHostSdkIfSubmitRequestAsync002, TestSize.Level1)
{
    int32_t ret;
    int32_t i;

    for (i = 0; i < ACM_NR; i++) {
        ret = UsbSubmitRequestAsync(g_acm->wb[i].request);
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.number    : CheckHostSdkIfCancelRequest002
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTestIo, CheckHostSdkIfCancelRequest002, TestSize.Level1)
{
    int32_t ret;
    int32_t i = 0;

    i = ACM_NR-1;
    ret = UsbCancelRequest(g_acm->wb[i].request);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfSubmitRequestAsync003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTestIo, CheckHostSdkIfSubmitRequestAsync003, TestSize.Level1)
{
    int32_t ret;

    ret = UsbSubmitRequestAsync(g_acm->notifyReq);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfCancelRequest003
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTestIo, CheckHostSdkIfCancelRequest003, TestSize.Level1)
{
    int32_t ret;

    ret = UsbCancelRequest(g_acm->notifyReq);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfSubmitRequestAsync004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTestIo, CheckHostSdkIfSubmitRequestAsync004, TestSize.Level1)
{
    int32_t ret;

    ret = UsbSubmitRequestAsync(g_acm->readReq[0]);

    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.number    : CheckHostSdkIfCancelRequest004
 * @tc.name      :
 * @tc.type      : PERF
 * @tc.level     : Level 1
 */
HWTEST_F(UsbHostSdkIfTestIo, CheckHostSdkIfCancelRequest004, TestSize.Level1)
{
    int32_t ret;

    ret = UsbSubmitRequestAsync(g_acm->readReq[0]);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

}
