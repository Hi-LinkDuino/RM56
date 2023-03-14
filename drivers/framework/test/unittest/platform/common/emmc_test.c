/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "emmc_if.h"
#include "emmc_test.h"

#define HDF_LOG_TAG emmc_test_c

struct EmmcTestFunc {
    enum EmmcTestCmd type;
    int32_t (*Func)(struct EmmcTester *tester);
};

static DevHandle EmmcTestGetHandle(struct EmmcTester *tester)
{
    if (tester == NULL) {
        HDF_LOGE("%s: tester is null", __func__);
        return NULL;
    }
    return EmmcOpen(tester->busNum);
}

static void EmmcTestReleaseHandle(DevHandle handle)
{
    if (handle == NULL) {
        HDF_LOGE("%s: sdio handle is null", __func__);
        return;
    }
    EmmcClose(handle);
}

static int32_t TestEmmcGetCid(struct EmmcTester *tester)
{
    int32_t ret, i;
    uint8_t cid[EMMC_CID_LEN] = {0};

    ret = EmmcGetCid(tester->handle, cid, EMMC_CID_LEN);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: EmmcGetCid failed ret=%d.", __func__, ret);
        return HDF_FAILURE;
    }
    for (i = 0; i < EMMC_CID_LEN; i++) {
        HDF_LOGE("%s: cid[%d] = 0x%x\n", __func__, i, cid[i]);
    }
    return HDF_SUCCESS;
}

struct EmmcTestFunc g_emmcTestFunc[] = {
    { EMMC_GET_CID_01, TestEmmcGetCid },
};

static int32_t EmmcTestEntry(struct EmmcTester *tester, int32_t cmd)
{
    int32_t i;
    int32_t ret = HDF_SUCCESS;
    bool isFind = false;

    if (tester == NULL) {
        HDF_LOGE("%s: tester is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    tester->handle = EmmcTestGetHandle(tester);
    if (tester->handle == NULL) {
        HDF_LOGE("%s: emmc test get handle failed", __func__);
        return HDF_FAILURE;
    }
    for (i = 0; i < sizeof(g_emmcTestFunc) / sizeof(g_emmcTestFunc[0]); i++) {
        if (cmd == g_emmcTestFunc[i].type && g_emmcTestFunc[i].Func != NULL) {
            ret = g_emmcTestFunc[i].Func(tester);
            isFind = true;
            break;
        }
    }
    if (!isFind) {
        ret = HDF_ERR_NOT_SUPPORT;
        HDF_LOGE("%s: cmd %d not supported", __func__, cmd);
    }
    EmmcTestReleaseHandle(tester->handle);
    return ret;
}

static int32_t EmmcTestFillConfig(struct EmmcTester *tester, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops", __func__);
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint32(node, "busNum", &(tester->busNum), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fill bus num failed", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "hostId", &(tester->hostId), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fill hostId failed", __func__);
        return ret;
    }

    HDF_LOGE("%s: busNum:%d, hostId:%d.", __func__, tester->busNum, tester->hostId);
    return HDF_SUCCESS;
}

static int32_t EmmcTestBind(struct HdfDeviceObject *device)
{
    static struct EmmcTester tester;

    if (device == NULL) {
        HDF_LOGE("%s: device or config is null!", __func__);
        return HDF_ERR_IO;
    }

    device->service = &tester.service;
    HDF_LOGE("%s: EMMC_TEST service init success!", __func__);
    return HDF_SUCCESS;
}

static int32_t EmmcTestInit(struct HdfDeviceObject *device)
{
    struct EmmcTester *tester = NULL;
    int32_t ret;

    if (device == NULL || device->service == NULL || device->property == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    tester = (struct EmmcTester *)device->service;
    if (tester == NULL) {
        HDF_LOGE("%s: tester is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = EmmcTestFillConfig(tester, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed", __func__);
        return ret;
    }
    tester->TestEntry = EmmcTestEntry;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static void EmmcTestRelease(struct HdfDeviceObject *device)
{
    if (device != NULL) {
        device->service = NULL;
    }
}

struct HdfDriverEntry g_emmcTestEntry = {
    .moduleVersion = 1,
    .Bind = EmmcTestBind,
    .Init = EmmcTestInit,
    .Release = EmmcTestRelease,
    .moduleName = "PLATFORM_EMMC_TEST",
};
HDF_INIT(g_emmcTestEntry);
