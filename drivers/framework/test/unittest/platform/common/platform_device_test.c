/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "platform_device_test.h"
#include "platform_assert.h"
#include "platform_manager.h"

#define HDF_LOG_TAG platform_device_test

#define PLAT_DEV_WAIT_TIMEOUT     10

int32_t PlatformDeviceTestSetUpAll(void)
{
    return HDF_SUCCESS;
}

int32_t PlatformDeviceTestTearDownAll(void)
{
    return HDF_SUCCESS;
}

static int32_t PlatformDeviceTestSetName(struct PlatformDevice *device)
{
    int32_t ret;

    PLAT_LOGD("%s: enter", __func__);
    // should set name success
    ret = PlatformDeviceSetName(device, "platform_device_name_%d", 1);
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, ret);
    ret = strcmp(device->name, "platform_device_name_1");
    CHECK_EQ_RETURN(ret, 0, HDF_FAILURE);

    // name should be null after clear
    PlatformDeviceClearName(device);
    CHECK_EQ_RETURN(device->name, NULL, HDF_FAILURE);

    // should set name success
    ret = PlatformDeviceSetName(device, "platform_device_name");
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, ret);
    ret = strcmp(device->name, "platform_device_name");
    CHECK_EQ_RETURN(ret, 0, HDF_FAILURE);

    // clean the name after test
    PlatformDeviceClearName(device);

    PLAT_LOGD("%s: exit", __func__);
    return HDF_SUCCESS;
}

static int32_t PlatformDeviceTestGetDevice(struct PlatformDevice *device)
{
    int32_t ret;
    int32_t refCntBeforeGet;
    int32_t refCntAfterGet;
    int32_t refCntAfterPut;

    PLAT_LOGD("%s: enter", __func__);
    device->name = "platform_device_test_get";
    refCntBeforeGet = PlatformDeviceRefCount(device);
    ret = PlatformDeviceGet(device);
    // should get device success
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, ret);

    refCntAfterGet = PlatformDeviceRefCount(device);
    // ref count should increase by 1 after get
    CHECK_EQ_RETURN(refCntAfterGet, refCntBeforeGet + 1, ret);

    ret = PlatformDeviceGet(device);
    // should get device success again
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, ret);
    refCntAfterGet = PlatformDeviceRefCount(device);
    // ref count should increase by 2 after double get
    CHECK_EQ_RETURN(refCntAfterGet, refCntBeforeGet + 2, ret);

    PlatformDevicePut(device);
    refCntAfterPut = PlatformDeviceRefCount(device);
    // ref count should decrease by 1 after put
    CHECK_EQ_RETURN(refCntAfterPut, refCntBeforeGet + 1, ret);

    PlatformDevicePut(device);
    refCntAfterPut = PlatformDeviceRefCount(device);
    // ref count should decrease by 2 after put
    CHECK_EQ_RETURN(refCntAfterPut, refCntBeforeGet, ret);

    PLAT_LOGD("%s: exit", __func__);
    return HDF_SUCCESS;
}

static int32_t PlatformDeviceTestWaitEvent(struct PlatformDevice *device)
{
    int32_t ret;
    uint32_t eventA = 0x1;
    uint32_t eventB = 0x4;
    uint32_t mask = eventA | eventB;
    uint32_t events;

    PLAT_LOGD("%s: enter", __func__);
    device->name = "platform_device_test_event";
    // should not wait success before post
    ret = PlatformDeviceWaitEvent(device, mask, PLAT_DEV_WAIT_TIMEOUT, &events);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);

    // should post event success
    ret = PlatformDevicePostEvent(device, eventA | eventB);
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, ret);

    // should wait success after post
    ret = PlatformDeviceWaitEvent(device, mask, PLAT_DEV_WAIT_TIMEOUT, &events);
    PLAT_LOGD("%s: events:%x", __func__, events);
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, ret);
    CHECK_EQ_RETURN(events, eventA | eventB, ret);

    PLAT_LOGD("%s: exit", __func__);
    return HDF_SUCCESS;
}

static int32_t PlatformDeviceTestAddDevice(struct PlatformDevice *device)
{
    int32_t ret;
    struct PlatformManager *manager = NULL;
    struct PlatformDevice *deviceGet = NULL;

    PLAT_LOGD("%s: enter", __func__);
    device->name = "platform_device_test_add";
    // should create manager success
    ret = PlatformManagerCreate("platform_test_manager", &manager);
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, ret);

    device->number = 0x5A; // a random number just for testing
    device->manager = manager;
    // should add platform device success
    ret = PlatformDeviceAdd(device);
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, ret);

    // should get the device added
    deviceGet = PlatformManagerGetDeviceByNumber(manager, device->number);
    CHECK_EQ_RETURN(deviceGet, device, ret);
    PlatformDevicePut(deviceGet);

    PlatformDeviceDel(device);
    // should not get the device after del
    deviceGet = PlatformManagerGetDeviceByNumber(manager, device->number);
    CHECK_EQ_RETURN(deviceGet, NULL, ret);

    PLAT_LOGD("%s: exit", __func__);
    return HDF_SUCCESS;
}

static int32_t TestDispatch(struct HdfDeviceIoClient *client, int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;
    return HDF_SUCCESS;
}

static int32_t PlatformDeviceTestCreateService(struct PlatformDevice *device)
{
    int32_t ret;

    PLAT_LOGD("%s: enter", __func__);
    device->name = "platform_device_test_create_service";
    ret = PlatformDeviceCreateService(device, TestDispatch);
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, ret);

    CHECK_NE_RETURN(device->service, NULL, HDF_FAILURE);
    CHECK_EQ_RETURN(device->service->Dispatch, TestDispatch, HDF_FAILURE);

    PlatformDeviceDestroyService(device);
    CHECK_EQ_RETURN(device->service, NULL, HDF_FAILURE);

    PLAT_LOGD("%s: exit", __func__);
    return HDF_SUCCESS;
}

static int32_t PlatformDeviceTestBindDevice(struct PlatformDevice *device)
{
    int32_t ret;
    struct HdfDeviceObject hdfDev;
    struct IDeviceIoService service;
    struct PlatformDevice *devFromHdf = NULL;

    PLAT_LOGD("%s: enter", __func__);
    device->name = "platform_device_test_bind";
    device->service = &service;
    ret = PlatformDeviceBind(device, &hdfDev);
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, ret);
    CHECK_EQ_RETURN(device->hdfDev, &hdfDev, HDF_FAILURE);
    CHECK_EQ_RETURN(device->service, hdfDev.service, HDF_FAILURE);

    devFromHdf = PlatformDeviceFromHdfDev(&hdfDev);
    CHECK_EQ_RETURN(device, devFromHdf, HDF_FAILURE);

    PlatformDeviceUnbind(device, &hdfDev);
    CHECK_EQ_RETURN(device->hdfDev, NULL, ret);
    CHECK_EQ_RETURN(hdfDev.service, NULL, ret);

    PLAT_LOGD("%s: exit", __func__);
    return HDF_SUCCESS;
}

static int32_t PlatformDeviceTestReliability(struct PlatformDevice *device)
{
    int32_t ret;
    uint32_t events;
    struct HdfDeviceObject hdfDev;
    struct PlatformDevice *devGet = NULL;

    PLAT_LOGD("%s: enter", __func__);
    device->name = "platform_device_test_reliability";
    ret = PlatformDeviceInit(NULL);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);

    PlatformDeviceUninit(NULL);

    ret = PlatformDeviceSetName(NULL, "device_name");
    CHECK_NE_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);

    ret = PlatformDeviceSetName(device, NULL);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);
    PlatformDeviceClearName(device);

    ret = PlatformDeviceGet(NULL);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);
    PlatformDevicePut(NULL);

    ret = PlatformDeviceRefCount(NULL);
    CHECK_LT_RETURN(ret, 0, HDF_FAILURE);

    ret = PlatformDevicePostEvent(NULL, 0x1);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);

    ret = PlatformDeviceWaitEvent(NULL, 0x1, 1, &events);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);

    ret = PlatformDeviceAdd(NULL);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);
    PlatformDeviceDel(NULL);

    ret = PlatformDeviceCreateService(NULL, TestDispatch);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);
    PlatformDeviceDestroyService(NULL);

    ret = PlatformDeviceBind(device, NULL);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);
    ret = PlatformDeviceBind(NULL, &hdfDev);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);
    PlatformDeviceUnbind(device, NULL);
    PlatformDeviceUnbind(NULL, NULL);

    devGet = PlatformDeviceFromHdfDev(NULL);
    CHECK_NULL_RETURN(devGet, HDF_FAILURE);

    PLAT_LOGD("%s: exit", __func__);
    return HDF_SUCCESS;
}

struct PlatformDeviceTestEntry {
    int cmd;
    int32_t (*func)(struct PlatformDevice *device);
    const char *name;
};

static struct PlatformDeviceTestEntry g_entry[] = {
    { PLAT_DEVICE_TEST_SET_NAME, PlatformDeviceTestSetName, "PlatformDeviceTestReliability" },
    { PLAT_DEVICE_TEST_GET_DEVICE, PlatformDeviceTestGetDevice, "PlatformDeviceTestGetDevice" },
    { PLAT_DEVICE_TEST_WAIT_EVENT, PlatformDeviceTestWaitEvent, "PlatformDeviceTestWaitEvent" },
    { PLAT_DEVICE_TEST_ADD_DEVICE, PlatformDeviceTestAddDevice, "PlatformDeviceTestAddDevice" },
    { PLAT_DEVICE_TEST_CREATE_SERVICE, PlatformDeviceTestCreateService, "PlatformDeviceTestCreateService" },
    { PLAT_DEVICE_TEST_BIND_DEVICE, PlatformDeviceTestBindDevice, "PlatformDeviceTestBindDevice" },
    { PLAT_DEVICE_TEST_RELIABILITY, PlatformDeviceTestReliability, "PlatformDeviceTestReliability" },
};

int PlatformDeviceTestExecute(int cmd)
{
    uint32_t i;
    int32_t ret = HDF_ERR_NOT_SUPPORT;
    struct PlatformDevice device;
    struct PlatformDeviceTestEntry *entry = NULL;

    if (cmd > PLAT_DEVICE_TEST_CMD_MAX) {
        PLAT_LOGE("PlatformDeviceTestExecute: invalid cmd:%d", cmd);
        ret = HDF_ERR_NOT_SUPPORT;
        PLAT_LOGE("[PlatformDeviceTestExecute][======cmd:%d====ret:%d======]", cmd, ret);
        return ret;
    }

    for (i = 0; i < (sizeof(g_entry) / sizeof(g_entry[0])); i++) {
        if (g_entry[i].cmd != cmd || g_entry[i].func == NULL) {
            continue;
        }
        entry = &g_entry[i];
        break;
    }

    if (entry == NULL) {
        PLAT_LOGE("%s: no entry matched, cmd = %d", __func__, cmd);
        return HDF_ERR_NOT_SUPPORT;
    }

    if ((ret = PlatformDeviceInit(&device)) != HDF_SUCCESS) {
        PLAT_LOGE("%s: init failed, ret = %d", __func__, ret);
        return ret;
    }

    ret = entry->func(&device);
    PlatformDeviceUninit(&device);

    PLAT_LOGE("[PlatformDeviceTestExecute][======cmd:%d====ret:%d======]", cmd, ret);
    return ret;
}

void PlatformDeviceTestExecuteAll(void)
{
    int32_t i;
    int32_t ret;
    int32_t fails = 0;

    for (i = 0; i < PLAT_DEVICE_TEST_CMD_MAX; i++) {
        ret = PlatformDeviceTestExecute(i);
        fails += (ret != HDF_SUCCESS) ? 1 : 0;
    }

    PLAT_LOGE("PlatformDeviceTestExecuteALL: **********PASS:%d  FAIL:%d************\n\n",
        PLAT_DEVICE_TEST_CMD_MAX - fails, fails);
}
