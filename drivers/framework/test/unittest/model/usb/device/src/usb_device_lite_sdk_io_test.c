/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "usb_device_lite_cdcacm_test.h"

#define HDF_LOG_TAG usb_device_sdk_test

static void ReadComplete(uint8_t pipe, struct UsbFnRequest *req)
{
    struct AcmDevice *acmDevice = UsbGetAcmDevice();
    if ((req == NULL) || (acmDevice == NULL)) {
        return;
    }
    if (req->actual) {
        uint8_t *data = (uint8_t *)req->buf;
        data[req->actual] = '\0';
        dprintf("receive [%d] bytes data: %s\n", req->actual, data);
        if (strcmp((const char *)data, "q") == 0 || \
            strcmp((const char *)data, "q\n") == 0) {
            acmDevice->submitExit = 1;
        }
    }
    acmDevice->submit = 1;
}

int32_t UsbFnDviceTestRequestAsync(void)
{
    struct UsbFnRequest *req = NULL;
    int32_t ret;

    ret = UsbFnSubmitRequestAsync(req);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: async Request success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRequestAsync002(void)
{
    struct UsbFnRequest *req = NULL;
    int32_t ret = HDF_SUCCESS;
    int32_t ret1;
    struct AcmDevice *acmDevice = UsbGetAcmDevice();
    if (acmDevice == NULL || acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    dprintf("wait receiving data form host, please connect\n");
    req = UsbFnAllocRequest(acmDevice->dataIface.handle, acmDevice->dataOutPipe.id,
        acmDevice->dataOutPipe.maxPacketSize);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    req->complete = ReadComplete;
    req->context = acmDevice;
    while (acmDevice->connect == false) {
        OsalMSleep(WAIT_100MS);
    }
    while (1) {
        acmDevice->submit = 0;
        ret = UsbFnSubmitRequestAsync(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: async Request error", __func__);
            ret = HDF_FAILURE;
            break;
        }
        while (acmDevice->submit == 0) {
            OsalMSleep(WAIT_100MS);
        }
        if (req->actual > 0) {
            break;
        }
    }
    ret1 = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret1) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return ret;
}

int32_t UsbFnDviceTestRequestAsync003(void)
{
    struct UsbFnRequest *req = NULL;
    int32_t ret = HDF_SUCCESS;
    int32_t ret1;
    struct AcmDevice *acmDevice = UsbGetAcmDevice();
    if (acmDevice == NULL || acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    dprintf("wait receiving data form host, please connect\n");
    req = UsbFnAllocRequest(acmDevice->dataIface.handle, acmDevice->dataOutPipe.id,
        acmDevice->dataOutPipe.maxPacketSize);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    req->complete = ReadComplete;
    req->context = acmDevice;
    while (acmDevice->connect == false) {
        OsalMSleep(WAIT_100MS);
    }
    acmDevice->submitExit = 0;
    while (acmDevice->submitExit == 0) {
        acmDevice->submit = 0;
        ret = UsbFnSubmitRequestAsync(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: async Request error", __func__);
            ret = HDF_FAILURE;
            break;
        }
        while (acmDevice->submit == 0) {
            OsalMSleep(WAIT_100MS);
        }
    }
    ret1 = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret1) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return ret;
}

static void WriteComplete(uint8_t pipe, struct UsbFnRequest *req)
{
    struct AcmDevice *acmDevice = UsbGetAcmDevice();
    if (acmDevice == NULL) {
        return;
    }
    dprintf("write data status = %d\n", req->status);
    acmDevice->submit = 1;
}

int32_t UsbFnDviceTestRequestAsync004(void)
{
    struct UsbFnRequest *req = NULL;
    int32_t ret;
    struct AcmDevice *acmDevice = UsbGetAcmDevice();

    if (acmDevice == NULL || acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocRequest(acmDevice->dataIface.handle, acmDevice->dataInPipe.id,
        acmDevice->dataInPipe.maxPacketSize);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    req->complete = WriteComplete;
    req->context = acmDevice;
    acmDevice->submit = 0;
    dprintf("------send \"abc\" to host------\n");
    if (acmDevice->dataInPipe.maxPacketSize < strlen("abc")) {
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: free Request error", __func__);
        }
        return HDF_FAILURE;
    }
    if (memcpy_s(req->buf, acmDevice->dataInPipe.maxPacketSize, "abc", strlen("abc")) != EOK) {
        HDF_LOGE("%s:%d memcpy_s fail", __func__, __LINE__);
    }
    req->length = strlen("abc");
    ret = UsbFnSubmitRequestAsync(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: async Request error", __func__);
        return HDF_FAILURE;
    }
    while (acmDevice->submit == 0) {
        OsalMSleep(1);
    }
    acmDevice->submit = 0;
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRequestAsync005(void)
{
    struct UsbFnRequest *req = NULL;
    int32_t loopTime = TEST_TIMES;
    int32_t ret;
    struct AcmDevice *acmDevice = UsbGetAcmDevice();

    if (acmDevice == NULL || acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    dprintf("------send \"xyz\" 10 times to host------\n");
    while (loopTime--) {
        req = UsbFnAllocRequest(acmDevice->dataIface.handle, acmDevice->dataInPipe.id,
            acmDevice->dataInPipe.maxPacketSize);
        if (req == NULL) {
            HDF_LOGE("%s: alloc req fail", __func__);
            return HDF_FAILURE;
        }
        req->complete = WriteComplete;
        req->context = acmDevice;
        acmDevice->submit = 0;
        if (memcpy_s(req->buf, acmDevice->dataInPipe.maxPacketSize, "xyz", strlen("xyz")) != EOK) {
            HDF_LOGE("%s:%d memcpy_s fail", __func__, __LINE__);
        }
        req->length = strlen("xyz");
        ret = UsbFnSubmitRequestAsync(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: async Request error", __func__);
            return HDF_FAILURE;
        }
        while (acmDevice->submit == 0) {
            OsalMSleep(1);
        }
        acmDevice->submit = 0;
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: free Request error", __func__);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRequestAsync006(void)
{
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRequestSync(void)
{
    struct UsbFnRequest *req = NULL;
    int32_t ret;
    ret = UsbFnSubmitRequestSync(req, 0);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: sync Request success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRequestSync002(void)
{
    struct UsbFnRequest *req = NULL;
    int32_t ret;
    uint8_t *data = NULL;
    struct AcmDevice *acmDevice = UsbGetAcmDevice();
    if (acmDevice == NULL || acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    dprintf("wait receiving data form host\n");
    req = UsbFnAllocRequest(acmDevice->dataIface.handle, acmDevice->dataOutPipe.id,
        acmDevice->dataOutPipe.maxPacketSize);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnSubmitRequestSync(req, 0);
    if (ret != 0 || req->actual <= 0 || req->status != USB_REQUEST_COMPLETED) {
        HDF_LOGE("%s: Sync Request error", __func__);
        return HDF_FAILURE;
    }
    data = (uint8_t *)req->buf;
    data[req->actual] = '\0';
    dprintf("receive data from host: %s\n", data);
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRequestSync003(void)
{
    struct UsbFnRequest *req = NULL;
    int32_t ret;
    int32_t submitExit = 0;
    uint8_t *data = NULL;
    struct AcmDevice *acmDevice = UsbGetAcmDevice();
    if (acmDevice == NULL || acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    dprintf("receive data until 'q' exit\n");
    req = UsbFnAllocRequest(acmDevice->dataIface.handle, acmDevice->dataOutPipe.id,
        acmDevice->dataOutPipe.maxPacketSize);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    while (submitExit == 0) {
        ret = UsbFnSubmitRequestSync(req, 0);
        if (ret != 0 || req->actual <= 0 || req->status != USB_REQUEST_COMPLETED) {
            HDF_LOGE("%s: Sync Request error", __func__);
            break;
        }
        data = (uint8_t *)req->buf;
        data[req->actual] = '\0';
        if (strcmp((const char *)data, "q") == 0 || \
            strcmp((const char *)data, "q\n") == 0) {
            submitExit = 1;
        }
        dprintf("receive data from host: %s\n", data);
    }
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRequestSync004(void)
{
    struct UsbFnRequest *req = NULL;
    int32_t ret;
    struct AcmDevice *acmDevice = UsbGetAcmDevice();

    if (acmDevice == NULL || acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocRequest(acmDevice->dataIface.handle, acmDevice->dataInPipe.id,
        acmDevice->dataInPipe.maxPacketSize);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    dprintf("------send \"abc\" to host------\n");
    if (memcpy_s(req->buf, acmDevice->dataInPipe.maxPacketSize, "abc", strlen("abc")) != EOK) {
        HDF_LOGE("%s:%d memcpy_s fail", __func__, __LINE__);
    }
    req->length = strlen("abc");
    ret = UsbFnSubmitRequestSync(req, 0);
    if (HDF_SUCCESS != ret || (req->actual != strlen("abc")) || \
        (req->status != USB_REQUEST_COMPLETED)) {
        HDF_LOGE("%s: async Request error", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRequestSync005(void)
{
    struct UsbFnRequest *req = NULL;
    int32_t loopTime = TEST_TIMES;
    int32_t ret;
    struct AcmDevice *acmDevice = UsbGetAcmDevice();

    if (acmDevice == NULL || acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    dprintf("------send \"abcdefg\" 10 times to host------\n");
    while (loopTime--) {
        req = UsbFnAllocRequest(acmDevice->dataIface.handle, acmDevice->dataInPipe.id,
            acmDevice->dataInPipe.maxPacketSize);
        if (req == NULL) {
            HDF_LOGE("%s: alloc req fail", __func__);
            return HDF_FAILURE;
        }
        if (memcpy_s(req->buf, acmDevice->dataInPipe.maxPacketSize, "abcdefg", strlen("abcdefg")) != EOK) {
            HDF_LOGE("%s:%d memcpy_s fail", __func__, __LINE__);
        }
        req->length = strlen("abcdefg");
        ret = UsbFnSubmitRequestSync(req, 0);
        if (HDF_SUCCESS != ret || (req->actual != strlen("abcdefg")) || \
            (req->status != USB_REQUEST_COMPLETED)) {
            HDF_LOGE("%s: async Request error", __func__);
            return HDF_FAILURE;
        }
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: free Request error", __func__);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRequestSync006(void)
{
    struct UsbFnRequest *req = NULL;
    int32_t ret;
    struct AcmDevice *acmDevice = UsbGetAcmDevice();
    if (acmDevice == NULL || acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    dprintf("test sync timeout 5s:\n");
    req = UsbFnAllocRequest(acmDevice->dataIface.handle, acmDevice->dataOutPipe.id,
        acmDevice->dataOutPipe.maxPacketSize);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnSubmitRequestSync(req, SYNC_5000MS);
    if (ret == 0) {
        HDF_LOGE("%s: Sync Request success", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRequestSync007(void)
{
    struct UsbFnRequest *req = NULL;
    int32_t ret;

    ret = UsbFnSubmitRequestSync(req, SYNC_5000MS);
    if (ret == 0) {
        HDF_LOGE("%s: Sync Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static void TestCancelComplete(uint8_t pipe, struct UsbFnRequest *req)
{
    struct AcmDevice *acmDevice = UsbGetAcmDevice();
    if (acmDevice == NULL) {
        return;
    }

    acmDevice->havedSubmit = true;
}

int32_t UsbFnDviceTestCancelRequest(void)
{
    int32_t ret;
    struct UsbFnRequest *notifyReq = NULL;
    ret = UsbFnCancelRequest(notifyReq);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: cancel request success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestCancelRequest002(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    struct AcmDevice *acmDevice = UsbGetAcmDevice();

    if (acmDevice == NULL || acmDevice->ctrlIface.handle == NULL) {
        HDF_LOGE("%s: CtrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocCtrlRequest(acmDevice->ctrlIface.handle,
        sizeof(struct UsbCdcNotification));
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnCancelRequest(req);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: cancel request success", __func__);
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: free Request error", __func__);
            return HDF_FAILURE;
        }
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestCancelRequest003(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    struct AcmDevice *acmDevice = UsbGetAcmDevice();

    if (acmDevice == NULL || acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocRequest(acmDevice->dataIface.handle, acmDevice->dataInPipe.id,
        acmDevice->dataInPipe.maxPacketSize);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnCancelRequest(req);
    if (HDF_SUCCESS == ret) {
        dprintf("%s: cancel request success\n", __func__);
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            dprintf("%s: free Request error", __func__);
            return HDF_FAILURE;
        }
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestCancelRequest004(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    struct AcmDevice *acmDevice = UsbGetAcmDevice();

    if (acmDevice == NULL || acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocRequest(acmDevice->dataIface.handle, acmDevice->dataInPipe.id,
        acmDevice->dataInPipe.maxPacketSize);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnCancelRequest(req);
    if (HDF_SUCCESS == ret) {
        dprintf("%s: cancel request success\n", __func__);
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            dprintf("%s: free Request error", __func__);
            return HDF_FAILURE;
        }
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestCancelRequest005(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    struct AcmDevice *acmDevice = UsbGetAcmDevice();

    if (acmDevice == NULL || acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocRequest(acmDevice->dataIface.handle, acmDevice->dataInPipe.id,
        acmDevice->dataInPipe.maxPacketSize);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    acmDevice->havedSubmit = false;
    req->complete = TestCancelComplete;
    req->context = acmDevice;
    dprintf("------send \"abc\" to host------\n");
    if (memcpy_s(req->buf, acmDevice->dataInPipe.maxPacketSize, "abc", strlen("abc")) != EOK) {
        HDF_LOGE("%s:%d memcpy_s fail", __func__, __LINE__);
    }
    req->length = strlen("abc");
    ret = UsbFnSubmitRequestAsync(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: request async error", __func__);
        return HDF_FAILURE;
    }
    while (acmDevice->havedSubmit == 0) {
        OsalMSleep(1);
    }
    ret = UsbFnCancelRequest(req);
    if (HDF_SUCCESS == ret) {
        dprintf("%s: cancel request error", __func__);
        return HDF_FAILURE;
    }
    acmDevice->havedSubmit = false;
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t TestCancelRequest(struct UsbFnRequest *req, struct UsbFnRequest *req2)
{
    int32_t ret;
    ret = UsbFnSubmitRequestAsync(req2);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: request async error", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnCancelRequest(req2);
    if (HDF_SUCCESS != ret) {
        dprintf("%s: cancel request error", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnCancelRequest(req);
    if (HDF_SUCCESS != ret) {
        dprintf("%s: cancel request error", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        dprintf("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(req2);
    if (HDF_SUCCESS != ret) {
        dprintf("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestCancelRequest006(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    struct UsbFnRequest *req2 = NULL;
    struct AcmDevice *acmDevice = UsbGetAcmDevice();

    if (acmDevice == NULL || acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocRequest(acmDevice->dataIface.handle, acmDevice->dataOutPipe.id,
        acmDevice->dataOutPipe.maxPacketSize);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    acmDevice->havedSubmit = false;
    req->complete = TestCancelComplete;
    req->context = acmDevice;

    req2 = UsbFnAllocRequest(acmDevice->dataIface.handle, acmDevice->dataOutPipe.id,
        acmDevice->dataOutPipe.maxPacketSize);
    if (req2 == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    acmDevice->submit = false;
    req2->complete = ReadComplete;
    req2->context = acmDevice;
    ret = UsbFnSubmitRequestAsync(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: request async error", __func__);
        return HDF_FAILURE;
    }
    return TestCancelRequest(req, req2);
}

