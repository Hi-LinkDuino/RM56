/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "platform_manager_test.h"
#include "osal_mem.h"
#include "platform_assert.h"
#include "platform_manager.h"

#define HDF_LOG_TAG platform_manager_test

#define PLAT_MGR_TEST_DEV_NUM           3
#define PLAT_MGR_TEST_DEV_NUM_START     0x5A
#define PLAT_DEV_NUMBER_0               0
#define PLAT_DEV_NUMBER_1               1
#define PLAT_DEV_NUMBER_2               2

static struct PlatformDevice *g_platDevices[PLAT_MGR_TEST_DEV_NUM];

static int32_t PlatformManagerTestCreateDevices(void)
{
    int i;
    int j;
    int32_t ret;

    for (i = 0; i < PLAT_MGR_TEST_DEV_NUM; i++) {
        g_platDevices[i] = (struct PlatformDevice *)OsalMemCalloc(sizeof(struct PlatformDevice));
        if (g_platDevices[i] == NULL) {
            ret = HDF_ERR_MALLOC_FAIL;
            goto CREATE_ERR__;
        }
        g_platDevices[i]->number = i + PLAT_MGR_TEST_DEV_NUM_START;
        ret = PlatformDeviceInit(g_platDevices[i]);
        if (ret != HDF_SUCCESS) {
            OsalMemFree(g_platDevices[i]);
            g_platDevices[i] = NULL;
            goto CREATE_ERR__;
        }
    }

    return HDF_SUCCESS;

CREATE_ERR__:
    for (j = 0; j < i; j++) {
        PlatformDeviceUninit(g_platDevices[j]);
        OsalMemFree(g_platDevices[j]);
        g_platDevices[j] = 0;
    }

    return ret;
}

static void PlatformManagerTestDestroyDevices(void)
{
    int i;

    for (i = 0; i < PLAT_MGR_TEST_DEV_NUM; i++) {
        PlatformDeviceUninit(g_platDevices[i]);
        OsalMemFree(g_platDevices[i]);
        g_platDevices[i] = NULL;
    }
}

static int32_t PlatformManagerTestAddAndDel(struct PlatformManager *manager)
{
    int32_t ret;
    struct PlatformDevice *device0 = g_platDevices[PLAT_DEV_NUMBER_0];
    struct PlatformDevice *device1 = g_platDevices[PLAT_DEV_NUMBER_1];
    struct PlatformDevice *device2 = g_platDevices[PLAT_DEV_NUMBER_2];
    struct PlatformDevice *device0Get = NULL;
    struct PlatformDevice *device1Get = NULL;
    int32_t refCntBeforeAdd;
    int32_t refCntAfterAdd;
    int32_t refCntBeforeDel;
    int32_t refCntAfterDel;
    int32_t number;

    PLAT_LOGD("%s: enter", __func__);
    // should add first device success
    device0->name = "platform_device0";
    refCntBeforeAdd = PlatformDeviceRefCount(device0);
    ret = PlatformManagerAddDevice(manager, device0);
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, ret);
    refCntAfterAdd = PlatformDeviceRefCount(device0);
    // ref count should increase by 1 after add
    CHECK_EQ_RETURN(refCntAfterAdd, refCntBeforeAdd + 1, HDF_FAILURE);

    // should add another success
    device1->name = "platform_device1";
    ret = PlatformManagerAddDevice(manager, device1);
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, ret);

    // should find the device added
    device0Get = PlatformManagerGetDeviceByNumber(manager, device0->number);
    CHECK_EQ_RETURN(device0Get, device0, HDF_FAILURE);
    PlatformDevicePut(device0Get);

    // should find another device added
    device1Get = PlatformManagerGetDeviceByNumber(manager, device1->number);
    CHECK_EQ_RETURN(device1Get, device1, HDF_FAILURE);
    PlatformDevicePut(device1Get);

    // should add fail on repeated number
    number = device2->number;
    device2->number = device0->number;
    device2->name = "platform_device2";
    ret = PlatformManagerAddDevice(manager, device2);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, ret);

    // should add fail on repeated name
    device2->number = number; // give it a right number
    device2->name = device0->name;
    ret = PlatformManagerAddDevice(manager, device2);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, ret);

    refCntBeforeDel = PlatformDeviceRefCount(device0);
    (void)PlatformManagerDelDevice(manager, device0);
    refCntAfterDel = PlatformDeviceRefCount(device0);
    // ref count should decrease by 1 after del
    CHECK_EQ_RETURN(refCntAfterDel + 1, refCntBeforeDel, HDF_FAILURE);

    // should not get the device removed
    device0Get = PlatformManagerGetDeviceByNumber(manager, device0->number);
    CHECK_EQ_RETURN(device0Get, NULL, HDF_FAILURE);

    PLAT_LOGD("%s: exit", __func__);
    return HDF_SUCCESS;
}

static int32_t PlatformManagerTestGetDevice(struct PlatformManager *manager)
{
    int32_t ret;
    struct PlatformDevice *device0 = g_platDevices[PLAT_DEV_NUMBER_0];
    struct PlatformDevice *device1 = g_platDevices[PLAT_DEV_NUMBER_1];
    struct PlatformDevice *device2 = g_platDevices[PLAT_DEV_NUMBER_2];
    struct PlatformDevice *device0Get = NULL;
    struct PlatformDevice *device1Get = NULL;
    int32_t refCntBeforeGet;
    int32_t refCntAfterGet;
    int32_t refCntBeforePut;
    int32_t refCntAfterPut;

    PLAT_LOGD("%s: enter", __func__);
    // we add some devices first
    device0->name = "platform_device0";
    ret = PlatformManagerAddDevice(manager, device0);
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, ret);
    device1->name = "platform_device1";
    ret = PlatformManagerAddDevice(manager, device1);
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, ret);
    device2->name = "platform_device2";
    ret = PlatformManagerAddDevice(manager, device2);
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, ret);

    refCntBeforeGet = PlatformDeviceRefCount(device0);
    // should get the device by number
    device0Get = PlatformManagerGetDeviceByNumber(manager, device0->number);
    CHECK_EQ_RETURN(device0Get, device0, HDF_FAILURE);

    refCntAfterGet = PlatformDeviceRefCount(device0);
    // should increase refcount after get
    CHECK_EQ_RETURN(refCntBeforeGet + 1, refCntAfterGet, HDF_FAILURE);

    refCntBeforePut = PlatformDeviceRefCount(device0);
    PlatformDevicePut(device0Get);
    refCntAfterPut = PlatformDeviceRefCount(device0);
    // should dcrease refcount after put
    CHECK_EQ_RETURN(refCntBeforePut, refCntAfterPut + 1, HDF_FAILURE);

    refCntBeforeGet = PlatformDeviceRefCount(device1);
    // should get the device by name
    device1Get = PlatformManagerGetDeviceByName(manager, "platform_device1");

    CHECK_EQ_RETURN(device1Get, device1, HDF_FAILURE);
    refCntAfterGet = PlatformDeviceRefCount(device1);
    // should increase refcount after get
    CHECK_EQ_RETURN(refCntBeforeGet + 1, refCntAfterGet, HDF_FAILURE);

    refCntBeforePut = PlatformDeviceRefCount(device1);
    PlatformDevicePut(device1Get);
    refCntAfterPut = PlatformDeviceRefCount(device1);
    // should dcrease refcount after put
    CHECK_EQ_RETURN(refCntBeforePut, refCntAfterPut + 1, HDF_FAILURE);

    PLAT_LOGD("%s: exit", __func__);
    return HDF_SUCCESS;
}

static int32_t PlatformManagerTestReliability(struct PlatformManager *manager)
{
    int32_t ret;
    struct PlatformDevice *device = g_platDevices[0];
    int32_t number = device->number;

    PLAT_LOGD("%s: enter", __func__);
    // should not add success when manager is NULL
    ret = PlatformManagerAddDevice(NULL, device);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);

    // should not add success when device is NULL
    ret = PlatformManagerAddDevice(manager, NULL);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);

    // will not crash ...
    PlatformManagerAddDevice(manager, NULL);
    PlatformManagerAddDevice(NULL, device);

    // should not get device by wrong parms
    device = PlatformManagerGetDeviceByNumber(NULL, number);
    CHECK_EQ_RETURN(device, NULL, HDF_FAILURE);
    device = PlatformManagerGetDeviceByName(NULL, "platform_device0");
    CHECK_EQ_RETURN(device, NULL, HDF_FAILURE);
 
    PLAT_LOGD("%s: exit", __func__);
    return HDF_SUCCESS;
}

struct PlatformManagerTestEntry {
    int cmd;
    int32_t (*func)(struct PlatformManager *manager);
    const char *name;
};

static struct PlatformManagerTestEntry g_entry[] = {
    { PLAT_MANAGER_TEST_ADD_DEVICE, PlatformManagerTestAddAndDel, "PlatformManagerTestAddAndDel" },
    { PLAT_MANAGER_TEST_GET_DEVICE, PlatformManagerTestGetDevice, "PlatformManagerTestGetDevice" },
    { PLAT_MANAGER_TEST_RELIABILITY, PlatformManagerTestReliability, "PlatformManagerTestReliability" },
};

int PlatformManagerTestExecute(int cmd)
{
    uint32_t i;
    int32_t ret = HDF_ERR_NOT_SUPPORT;
    struct PlatformManager *manager = NULL;
    struct PlatformManagerTestEntry *entry = NULL;

    if (cmd > PLAT_MANAGER_TEST_CMD_MAX) {
        PLAT_LOGE("PlatformManagerTestExecute: invalid cmd:%d", cmd);
        ret = HDF_ERR_NOT_SUPPORT;
        PLAT_LOGE("[PlatformManagerTestExecute][======cmd:%d====ret:%d======]", cmd, ret);
        return ret;
    }

    for (i = 0; i < sizeof(g_entry) / sizeof(g_entry[0]); i++) {
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

    ret = PlatformManagerCreate("platform_test_manager", &manager);
    if (manager == NULL) {
        PLAT_LOGE("%s: create manager failed:%d", __func__, ret);
        return HDF_FAILURE;
    }

    ret = PlatformManagerTestCreateDevices();
    if (ret != HDF_SUCCESS) {
        PLAT_LOGE("%s: create devices failed:%d", __func__, ret);
        PlatformManagerDestroy(manager);
        return HDF_FAILURE;
    }

    ret = entry->func(manager);
    PlatformManagerDestroy(manager);
    PlatformManagerTestDestroyDevices();

    PLAT_LOGE("[PlatformManagerTestExecute][======cmd:%d====ret:%d======]", cmd, ret);
    return ret;
}

void PlatformManagerTestExecuteAll(void)
{
    int32_t i;
    int32_t ret;
    int32_t fails = 0;

    for (i = 0; i < PLAT_MANAGER_TEST_CMD_MAX; i++) {
        ret = PlatformManagerTestExecute(i);
        fails += (ret != HDF_SUCCESS) ? 1 : 0;
    }

    PLAT_LOGE("PlatformManagerTestExecuteALL: **********PASS:%d  FAIL:%d************\n\n",
        PLAT_MANAGER_TEST_CMD_MAX - fails, fails);
}
