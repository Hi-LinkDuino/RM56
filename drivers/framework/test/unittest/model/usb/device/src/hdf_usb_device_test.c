/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include "usb_device_lite_cdcacm_test.h"
#include "hdf_module_test.h"
#include "hdf_log.h"
#include "hdf_message_test.h"
#include "osal.h"
#include "hdf_usb_device_test.h"

// add test case entry
static HdfTestCaseList g_hdfUsbDeviceTestCaseList[] = {
    {USB_DEVICE_CREATE_DEVICE, UsbFnDviceTestCreate},
    {USB_DEVICE_CREATE_DEVICE002, UsbFnDviceTestCreate002},
    {USB_DEVICE_CREATE_DEVICE003, UsbFnDviceTestCreate003},
    {USB_DEVICE_CREATE_DEVICE004, UsbFnDviceTestCreate004},
    {USB_DEVICE_CREATE_DEVICE005, UsbFnDviceTestCreate005},
    {USB_DEVICE_CREATE_DEVICE006, UsbFnDviceTestCreate006},
    {USB_DEVICE_DEVICE_STATUS, UsbFnDviceTestStatus},
    {USB_DEVICE_DEVICE_STATUS002, UsbFnDviceTestStatus002},
    {USB_DEVICE_DEVICE_STATUS003, UsbFnDviceTestStatus003},
    {USB_DEVICE_DEVICE_STATUS004, UsbFnDviceTestStatus004},
    {USB_DEVICE_DEVICE_STATUS005, UsbFnDviceTestStatus005},
    {USB_DEVICE_GET_DEVICE, UsbFnDviceTestGetDevice},
    {USB_DEVICE_GET_DEVICE002, UsbFnDviceTestGetDevice002},
    {USB_DEVICE_GET_DEVICE003, UsbFnDviceTestGetDevice003},
    {USB_DEVICE_GET_DEVICE004, UsbFnDviceTestGetDevice004},
    {USB_DEVICE_GET_DEVICE005, UsbFnDviceTestGetDevice005},
    {USB_DEVICE_GET_DEVICE006, UsbFnDviceTestGetDevice006},
    {USB_DEVICE_GET_INTERFACE, UsbFnDviceTestGetInterface},
    {USB_DEVICE_GET_INTERFACE002, UsbFnDviceTestGetInterface002},
    {USB_DEVICE_GET_INTERFACE003, UsbFnDviceTestGetInterface003},
    {USB_DEVICE_GET_INTERFACE004, UsbFnDviceTestGetInterface004},
    {USB_DEVICE_GET_INTERFACE005, UsbFnDviceTestGetInterface005},
    {USB_DEVICE_GET_INTERFACE006, UsbFnDviceTestGetInterface006},
    {USB_DEVICE_GET_PIPEINFO, UsbFnDviceTestGetPipeInfo},
    {USB_DEVICE_GET_PIPEINFO002, UsbFnDviceTestGetPipeInfo002},
    {USB_DEVICE_GET_PIPEINFO003, UsbFnDviceTestGetPipeInfo003},
    {USB_DEVICE_GET_PIPEINFO004, UsbFnDviceTestGetPipeInfo004},
    {USB_DEVICE_GET_PIPEINFO005, UsbFnDviceTestGetPipeInfo005},
    {USB_DEVICE_GET_PIPEINFO006, UsbFnDviceTestGetPipeInfo006},
    {USB_DEVICE_REQUEST_ASYNC, UsbFnDviceTestRequestAsync},
    {USB_DEVICE_REQUEST_ASYNC002, UsbFnDviceTestRequestAsync002},
    {USB_DEVICE_REQUEST_ASYNC003, UsbFnDviceTestRequestAsync003},
    {USB_DEVICE_REQUEST_ASYNC004, UsbFnDviceTestRequestAsync004},
    {USB_DEVICE_REQUEST_ASYNC005, UsbFnDviceTestRequestAsync005},
    {USB_DEVICE_REQUEST_ASYNC006, UsbFnDviceTestRequestAsync006},
    {USB_DEVICE_REQUEST_SYNC, UsbFnDviceTestRequestSync},
    {USB_DEVICE_REQUEST_SYNC002, UsbFnDviceTestRequestSync002},
    {USB_DEVICE_REQUEST_SYNC003, UsbFnDviceTestRequestSync003},
    {USB_DEVICE_REQUEST_SYNC004, UsbFnDviceTestRequestSync004},
    {USB_DEVICE_REQUEST_SYNC005, UsbFnDviceTestRequestSync005},
    {USB_DEVICE_REQUEST_SYNC006, UsbFnDviceTestRequestSync006},
    {USB_DEVICE_REQUEST_SYNC007, UsbFnDviceTestRequestSync007},
    {USB_DEVICE_REGIST_PROP, UsbFnDviceTestRegistProp},
    {USB_DEVICE_REGIST_PROP002, UsbFnDviceTestRegistProp002},
    {USB_DEVICE_REGIST_PROP003, UsbFnDviceTestRegistProp003},
    {USB_DEVICE_REGIST_PROP004, UsbFnDviceTestRegistProp004},
    {USB_DEVICE_REGIST_PROP005, UsbFnDviceTestRegistProp005},
    {USB_DEVICE_REGIST_PROP006, UsbFnDviceTestRegistProp006},
    {USB_DEVICE_REGIST_PROP007, UsbFnDviceTestRegistProp007},
    {USB_DEVICE_GET_PROP, UsbFnDviceTestGetProp},
    {USB_DEVICE_GET_PROP002, UsbFnDviceTestGetProp002},
    {USB_DEVICE_GET_PROP003, UsbFnDviceTestGetProp003},
    {USB_DEVICE_GET_PROP004, UsbFnDviceTestGetProp004},
    {USB_DEVICE_GET_PROP005, UsbFnDviceTestGetProp005},
    {USB_DEVICE_GET_PROP006, UsbFnDviceTestGetProp006},
    {USB_DEVICE_GET_PROP007, UsbFnDviceTestGetProp007},
    {USB_DEVICE_GET_PROP008, UsbFnDviceTestGetProp008},
    {USB_DEVICE_SET_PROP, UsbFnDviceTestSetProp},
    {USB_DEVICE_SET_PROP002, UsbFnDviceTestSetProp002},
    {USB_DEVICE_SET_PROP003, UsbFnDviceTestSetProp003},
    {USB_DEVICE_SET_PROP004, UsbFnDviceTestSetProp004},
    {USB_DEVICE_SET_PROP005, UsbFnDviceTestSetProp005},
    {USB_DEVICE_SET_PROP006, UsbFnDviceTestSetProp006},
    {USB_DEVICE_SET_PROP007, UsbFnDviceTestSetProp007},
    {USB_DEVICE_ALLOC_CTRLREQUEST, UsbFnDviceTestAllocCtrlRequest},
    {USB_DEVICE_ALLOC_CTRLREQUEST002, UsbFnDviceTestAllocCtrlRequest002},
    {USB_DEVICE_ALLOC_CTRLREQUEST003, UsbFnDviceTestAllocCtrlRequest003},
    {USB_DEVICE_ALLOC_CTRLREQUEST004, UsbFnDviceTestAllocCtrlRequest004},
    {USB_DEVICE_ALLOC_CTRLREQUEST005, UsbFnDviceTestAllocCtrlRequest005},
    {USB_DEVICE_ALLOC_CTRLREQUEST006, UsbFnDviceTestAllocCtrlRequest006},
    {USB_DEVICE_ALLOC_CTRLREQUEST007, UsbFnDviceTestAllocCtrlRequest007},
    {USB_DEVICE_ALLOC_CTRLREQUEST008, UsbFnDviceTestAllocCtrlRequest008},
    {USB_DEVICE_ALLOC_REQUEST, UsbFnDviceTestAllocRequest},
    {USB_DEVICE_ALLOC_REQUEST002, UsbFnDviceTestAllocRequest002},
    {USB_DEVICE_ALLOC_REQUEST003, UsbFnDviceTestAllocRequest003},
    {USB_DEVICE_ALLOC_REQUEST004, UsbFnDviceTestAllocRequest004},
    {USB_DEVICE_ALLOC_REQUEST005, UsbFnDviceTestAllocRequest005},
    {USB_DEVICE_ALLOC_REQUEST006, UsbFnDviceTestAllocRequest006},
    {USB_DEVICE_ALLOC_REQUEST007, UsbFnDviceTestAllocRequest007},
    {USB_DEVICE_ALLOC_REQUEST008, UsbFnDviceTestAllocRequest008},
    {USB_DEVICE_ALLOC_REQUEST009, UsbFnDviceTestAllocRequest009},
    {USB_DEVICE_FREE_REQUEST, UsbFnDviceTestFreeRequest},
    {USB_DEVICE_FREE_REQUEST002, UsbFnDviceTestFreeRequest002},
    {USB_DEVICE_FREE_REQUEST003, UsbFnDviceTestFreeRequest003},
    {USB_DEVICE_FREE_REQUEST004, UsbFnDviceTestFreeRequest004},
    {USB_DEVICE_FREE_REQUEST005, UsbFnDviceTestFreeRequest005},
    {USB_DEVICE_FREE_REQUEST006, UsbFnDviceTestFreeRequest006},
    {USB_DEVICE_GET_REQUEST_STATUS, UsbFnDviceTestGetRequestStatus},
    {USB_DEVICE_GET_REQUEST_STATUS002, UsbFnDviceTestGetRequestStatus002},
    {USB_DEVICE_GET_REQUEST_STATUS003, UsbFnDviceTestGetRequestStatus003},
    {USB_DEVICE_GET_REQUEST_STATUS004, UsbFnDviceTestGetRequestStatus004},
    {USB_DEVICE_GET_REQUEST_STATUS005, UsbFnDviceTestGetRequestStatus005},
    {USB_DEVICE_GET_REQUEST_STATUS006, UsbFnDviceTestGetRequestStatus006},
    {USB_DEVICE_CANCEL_REQUEST, UsbFnDviceTestCancelRequest},
    {USB_DEVICE_CANCEL_REQUEST002, UsbFnDviceTestCancelRequest002},
    {USB_DEVICE_CANCEL_REQUEST003, UsbFnDviceTestCancelRequest003},
    {USB_DEVICE_CANCEL_REQUEST004, UsbFnDviceTestCancelRequest004},
    {USB_DEVICE_CANCEL_REQUEST005, UsbFnDviceTestCancelRequest005},
    {USB_DEVICE_CANCEL_REQUEST006, UsbFnDviceTestCancelRequest006},
    {USB_DEVICE_STOP_EVENT, UsbFnDviceTestStopReceEvent},
    {USB_DEVICE_STOP_EVENT002, UsbFnDviceTestStopReceEvent002},
    {USB_DEVICE_STOP_EVENT003, UsbFnDviceTestStopReceEvent003},
    {USB_DEVICE_STOP_EVENT004, UsbFnDviceTestStopReceEvent004},
    {USB_DEVICE_START_EVENT, UsbFnDviceTestStartReceEvent},
    {USB_DEVICE_START_EVENT002, UsbFnDviceTestStartReceEvent002},
    {USB_DEVICE_START_EVENT003, UsbFnDviceTestStartReceEvent003},
    {USB_DEVICE_START_EVENT004, UsbFnDviceTestStartReceEvent004},
    {USB_DEVICE_START_EVENT005, UsbFnDviceTestStartReceEvent005},
    {USB_DEVICE_START_EVENT006, UsbFnDviceTestStartReceEvent006},
    {USB_DEVICE_START_EVENT007, UsbFnDviceTestStartReceEvent007},
    {USB_DEVICE_CLOSE_INTERFACE, UsbFnDviceTestCloseInterface},
    {USB_DEVICE_CLOSE_INTERFACE002, UsbFnDviceTestCloseInterface002},
    {USB_DEVICE_CLOSE_INTERFACE003, UsbFnDviceTestCloseInterface003},
    {USB_DEVICE_CLOSE_INTERFACE004, UsbFnDviceTestCloseInterface004},
    {USB_DEVICE_OPEN_INTERFACE, UsbFnDviceTestOpenInterface},
    {USB_DEVICE_OPEN_INTERFACE002, UsbFnDviceTestOpenInterface002},
    {USB_DEVICE_OPEN_INTERFACE003, UsbFnDviceTestOpenInterface003},
    {USB_DEVICE_OPEN_INTERFACE004, UsbFnDviceTestOpenInterface004},
    {USB_DEVICE_OPEN_INTERFACE005, UsbFnDviceTestOpenInterface005},
    {USB_DEVICE_DELETE_DEVICE, UsbFnDviceTestRemove},
    {USB_DEVICE_DELETE_DEVICE002, UsbFnDviceTestRemove002},
};

int32_t HdfUsbDeviceEntry(HdfTestMsg *msg)
{
    int32_t result, i;

    if (msg == NULL) {
        HDF_LOGE("%s is fail: HdfTestMsg is NULL!", __func__);
        return HDF_SUCCESS;
    }

    for (i = 0; i < sizeof(g_hdfUsbDeviceTestCaseList) / sizeof(g_hdfUsbDeviceTestCaseList[0]); ++i) {
        if ((msg->subCmd == g_hdfUsbDeviceTestCaseList[i].subCmd) && (g_hdfUsbDeviceTestCaseList[i].testFunc != NULL)) {
            result = g_hdfUsbDeviceTestCaseList[i].testFunc();
            HDF_LOGE("HdfTest:usb device test result[%s-%u]", ((result == 0) ? "pass" : "fail"), msg->subCmd);
            msg->result = (result == 0) ? HDF_SUCCESS : HDF_FAILURE;
            return HDF_SUCCESS;
        }
    }
    return HDF_SUCCESS;
}