/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_time.h"
#include "pcie_if.h"
#include "pcie_test.h"

#define HDF_LOG_TAG pcie_test_c

#define PCIE_TEST_DISABLE_ADDR 0xB7
#define PCIE_TEST_UPPER_ADDR 0x28
#define PCIE_TEST_CMD_ADDR 0x04

struct PcieTestFunc {
    enum PcieTestCmd type;
    int32_t (*Func)(struct PcieTester *tester);
};

static DevHandle PcieTestGetHandle(struct PcieTester *tester)
{
    if (tester == NULL) {
        HDF_LOGE("%s: tester is null", __func__);
        return NULL;
    }
    return PcieOpen(tester->busNum);
}

static void PcieTestReleaseHandle(DevHandle handle)
{
    if (handle == NULL) {
        HDF_LOGE("%s: pcie handle is null", __func__);
        return;
    }
    PcieClose(handle);
}

static int32_t TestPcieReadAndWrite(struct PcieTester *tester)
{
    int32_t ret;
    uint8_t disable;
    uint32_t upper;
    uint16_t cmd;

    ret = PcieRead(tester->handle, PCIE_TEST_DISABLE_ADDR, &disable, sizeof(disable));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PcieRead failed ret = %d.", __func__, ret);
        return ret;
    }
    HDF_LOGD("%s: disable is %d", __func__, disable);
    ret = PcieWrite(tester->handle, PCIE_TEST_DISABLE_ADDR, &disable, sizeof(disable));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PcieWrite failed ret = %d.", __func__, ret);
        return ret;
    }

    ret = PcieRead(tester->handle, PCIE_TEST_UPPER_ADDR, (uint8_t *)&upper, sizeof(upper));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PcieRead failed ret = %d.", __func__, ret);
        return ret;
    }
    HDF_LOGD("%s: upper is 0x%x", __func__, upper);
    ret = PcieWrite(tester->handle, PCIE_TEST_UPPER_ADDR, (uint8_t *)&upper, sizeof(upper));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PcieWrite failed ret = %d.", __func__, ret);
        return ret;
    }

    ret = PcieRead(tester->handle, PCIE_TEST_CMD_ADDR, (uint8_t *)&cmd, sizeof(cmd));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PcieRead failed ret = %d.", __func__, ret);
        return ret;
    }
    HDF_LOGD("%s: cmd is 0x%x", __func__, cmd);
    ret = PcieWrite(tester->handle, PCIE_TEST_CMD_ADDR, (uint8_t *)&cmd, sizeof(cmd));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PcieWrite failed ret = %d.", __func__, ret);
    }
    return ret;
}

struct PcieTestFunc g_pcieTestFunc[] = {
    { PCIE_READ_AND_WRITE_01, TestPcieReadAndWrite },
};

static int32_t PcieTestEntry(struct PcieTester *tester, int32_t cmd)
{
    int32_t i;
    int32_t ret = HDF_SUCCESS;
    bool isFind = false;

    if (tester == NULL) {
        HDF_LOGE("%s: tester is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    tester->handle = PcieTestGetHandle(tester);
    if (tester->handle == NULL) {
        HDF_LOGE("%s: pcie test get handle failed", __func__);
        return HDF_FAILURE;
    }
    for (i = 0; i < sizeof(g_pcieTestFunc) / sizeof(g_pcieTestFunc[0]); i++) {
        if (cmd == g_pcieTestFunc[i].type && g_pcieTestFunc[i].Func != NULL) {
            ret = g_pcieTestFunc[i].Func(tester);
            isFind = true;
            break;
        }
    }
    if (!isFind) {
        ret = HDF_ERR_NOT_SUPPORT;
        HDF_LOGE("%s: cmd %d not supported", __func__, cmd);
    }
    PcieTestReleaseHandle(tester->handle);
    return ret;
}

static int32_t PcieTestFillConfig(struct PcieTester *tester, const struct DeviceResourceNode *node)
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

    HDF_LOGE("%s: busNum:%d.", __func__, tester->busNum);
    return HDF_SUCCESS;
}

static int32_t PcieTestBind(struct HdfDeviceObject *device)
{
    static struct PcieTester tester;

    if (device == NULL) {
        HDF_LOGE("%s: device or config is null!", __func__);
        return HDF_ERR_IO;
    }

    device->service = &tester.service;
    HDF_LOGE("%s: PCIE_TEST service init success!", __func__);
    return HDF_SUCCESS;
}

static int32_t PcieTestInit(struct HdfDeviceObject *device)
{
    struct PcieTester *tester = NULL;
    int32_t ret;

    if (device == NULL || device->service == NULL || device->property == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    tester = (struct PcieTester *)device->service;
    if (tester == NULL) {
        HDF_LOGE("%s: tester is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = PcieTestFillConfig(tester, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed", __func__);
        return ret;
    }
    tester->TestEntry = PcieTestEntry;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static void PcieTestRelease(struct HdfDeviceObject *device)
{
    if (device != NULL) {
        device->service = NULL;
    }
}

struct HdfDriverEntry g_pcieTestEntry = {
    .moduleVersion = 1,
    .Bind = PcieTestBind,
    .Init = PcieTestInit,
    .Release = PcieTestRelease,
    .moduleName = "PLATFORM_PCIE_TEST",
};
HDF_INIT(g_pcieTestEntry);
