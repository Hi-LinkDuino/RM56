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
#include "sdio_if.h"
#include "sdio_test.h"

#define HDF_LOG_TAG sdio_test_c

#define TEST_DATA_LEN 3
#define TEST_BLOCKSIZE 512
#define TEST_SDIO_BASE_ADDR 0x100
#define TEST_FUNC_NUM 1
#define TEST_ADDR_OFFSET 0x10
#define TEST_FIXED_OFFSET 0x09
#define TEST_TIME_OUT 1000
#define TEST_ADDR_ADD 1
#define TEST_FUNC0_ADDR 0xFE

struct SdioTestFunc {
    enum SdioTestCmd type;
    int32_t (*Func)(struct SdioTester *tester);
};

static DevHandle SdioTestGetHandle(struct SdioTester *tester)
{
    if (tester == NULL) {
        HDF_LOGE("%s: tester is null", __func__);
        return NULL;
    }
    return SdioOpen((int16_t)(tester->busNum), &(tester->config));
}

static void SdioTestReleaseHandle(DevHandle handle)
{
    if (handle == NULL) {
        HDF_LOGE("%s: sdio handle is null", __func__);
        return;
    }
    SdioClose(handle);
}

static int32_t TestSdioIncrAddrReadAndWriteOtherBytes(struct SdioTester *tester)
{
    int32_t ret;
    uint8_t data[TEST_DATA_LEN] = {0};
    const uint32_t addr = TEST_SDIO_BASE_ADDR * TEST_FUNC_NUM + TEST_ADDR_OFFSET + TEST_ADDR_ADD;

    ret = SdioReadBytes(tester->handle, &data[0], addr, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioReadBytes fail! ret=%d.", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: read, data[0]:%d\n", __func__, data[0]);
    ret = SdioWriteBytes(tester->handle, &data[0], addr, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioWriteBytes fail! ret=%d.", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: write, data[0]:%u\n", __func__, data[0]);
    return HDF_SUCCESS;
}

static int32_t TestSdioIncrAddrReadAndWriteOneByte(struct SdioTester *tester)
{
    int32_t ret;
    uint8_t val;
    uint32_t addr;

    addr = TEST_SDIO_BASE_ADDR * TEST_FUNC_NUM + TEST_ADDR_OFFSET;
    /* read 1 bits */
    ret = SdioReadBytes(tester->handle, &val, addr, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioReadBytes fail! ret=%d.", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: read, val:%d\n", __func__, val);
    /* write 1 bits */
    ret = SdioWriteBytes(tester->handle, &val, addr, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioWriteBytes fail! ret=%d.", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: write, val:%d\n", __func__, val);
    /* read 1 bits */
    addr++;
    ret = SdioReadBytes(tester->handle, &val, addr, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioReadBytes fail! ret=%d.", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: read, val:%u.", __func__, val);
    /* read 1 bits */
    addr++;
    ret = SdioReadBytes(tester->handle, &val, addr, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioReadBytes fail! ret=%d.", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: read, val:%u.", __func__, val);
    return HDF_SUCCESS;
}

static int32_t TestSdioIncrAddrReadAndWriteBytes(struct SdioTester *tester)
{
    int32_t ret;

    SdioClaimHost(tester->handle);
    ret = TestSdioIncrAddrReadAndWriteOneByte(tester);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: TestSdioIncrAddrReadAndWriteOneByte fail! ret=%d.", __func__, ret);
        SdioReleaseHost(tester->handle);
        return HDF_FAILURE;
    }
    ret = TestSdioIncrAddrReadAndWriteOtherBytes(tester);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: TestSdioIncrAddrReadAndWriteOtherBytes fail! ret=%d.", __func__, ret);
        SdioReleaseHost(tester->handle);
        return HDF_FAILURE;
    }
    SdioReleaseHost(tester->handle);

    return ret;
}

static int32_t TestSdioFixedAddrReadAndWriteOtherBytes(struct SdioTester *tester)
{
    int32_t ret;
    uint8_t data[TEST_DATA_LEN] = {0};
    const uint32_t addr = TEST_SDIO_BASE_ADDR * TEST_FUNC_NUM + TEST_FIXED_OFFSET + TEST_ADDR_ADD;

    /* read bits */
    ret = SdioReadBytes(tester->handle, &data[0], addr, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioReadBytesFromFixedAddr fail! ret=%d.", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: read, data[0]:%u, data[1]:%u\n", __func__, data[0], data[1]);
    /* write bits */
    ret = SdioWriteBytes(tester->handle, &data[0], addr, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioWriteBytesToFixedAddr fail! ret=%d.", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: write, data[0]:%u, data[1]:%u.", __func__, data[0], data[1]);
    return ret;
}

static int32_t TestSdioFixedAddrReadAndWriteOneByte(struct SdioTester *tester)
{
    int32_t ret;
    uint32_t addr;
    uint8_t val;

    addr = TEST_SDIO_BASE_ADDR * TEST_FUNC_NUM + TEST_FIXED_OFFSET;
    /* read 1 bits */
    ret = SdioReadBytes(tester->handle, &val, addr, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioReadBytesFromFixedAddr fail! ret=%d.", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: read, val:%d\n", __func__, val);
    /* write 1 bits */
    ret = SdioWriteBytes(tester->handle, &val, addr, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioWriteBytesToFixedAddr fail! ret=%d.", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: write, val:%d.", __func__, val);
    /* read 1 bits */
    addr++;
    ret = SdioReadBytes(tester->handle, &val, addr, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioReadBytesFromFixedAddr fail! ret=%d.", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: read, val:%d.", __func__, val);
    /* read 1 bits */
    addr++;
    ret = SdioWriteBytes(tester->handle, &val, addr, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioReadBytesFromFixedAddr fail! ret=%d.", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: read, val:%d.", __func__, val);

    return ret;
}

static int32_t TestSdioFixedAddrReadAndWriteBytes(struct SdioTester *tester)
{
    int32_t ret;

    SdioClaimHost(tester->handle);
    ret = TestSdioFixedAddrReadAndWriteOtherBytes(tester);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: TestSdioFixedAddrReadAndWriteOtherBytes fail! ret=%d.", __func__, ret);
        SdioReleaseHost(tester->handle);
        return HDF_FAILURE;
    }
    ret = TestSdioFixedAddrReadAndWriteOneByte(tester);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: TestSdioFixedAddrReadAndWriteOneByte fail! ret=%d.", __func__, ret);
        SdioReleaseHost(tester->handle);
        return HDF_FAILURE;
    }
    SdioReleaseHost(tester->handle);

    return ret;
}

static int32_t TestSdioFunc0ReadAndWriteBytes(struct SdioTester *tester)
{
    int32_t ret;
    uint8_t val;

    SdioClaimHost(tester->handle);
    /* read sdio rev */
    ret = SdioReadBytesFromFunc0(tester->handle, &val, TEST_FUNC0_ADDR, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioReadBytesFromFunc0 fail! ret=%d.", __func__, ret);
        SdioReleaseHost(tester->handle);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: Func0 Read, val :%d.", __func__, val);

    /* write sdio rev */
    ret = SdioWriteBytesToFunc0(tester->handle, &val, TEST_FUNC0_ADDR, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioWriteBytesToFunc0 fail! ret=%d.", __func__, ret);
        SdioReleaseHost(tester->handle);
        return HDF_FAILURE;
    }

    /* read sdio rev again */
    ret = SdioReadBytesFromFunc0(tester->handle, &val, TEST_FUNC0_ADDR, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioReadBytesFromFunc0 fail! ret=%d.", __func__, ret);
        SdioReleaseHost(tester->handle);
        return HDF_FAILURE;
    }
    SdioReleaseHost(tester->handle);

    HDF_LOGE("%s: Func0 Read, val:%u.", __func__, val);
    return ret;
}

static int32_t TestSdioSetAndGetFuncInfo(struct SdioTester *tester)
{
    int32_t ret;
    SdioCommonInfo info = {0};

    ret = SdioGetCommonInfo(tester->handle, &info, SDIO_FUNC_INFO);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioGetCommonInfo fail! ret=%d.", __func__, ret);
        return ret;
    }
    HDF_LOGE("%s: succ! Timeout=%u.", __func__, info.funcInfo.enTimeout);

    info.funcInfo.enTimeout = TEST_TIME_OUT;
    ret = SdioSetCommonInfo(tester->handle, &info, SDIO_FUNC_INFO);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioSetCommonInfo fail! ret=%d.", __func__, ret);
        return ret;
    }

    ret = SdioGetCommonInfo(tester->handle, &info, SDIO_FUNC_INFO);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioGetCommonInfo fail! ret=%d.", __func__, ret);
        return ret;
    }
    HDF_LOGE("%s: again succ! Timeout=%u.", __func__, info.funcInfo.enTimeout);

    return HDF_SUCCESS;
}

static int32_t TestSdioSetAndGetCommonInfo(struct SdioTester *tester)
{
    int32_t ret;

    SdioClaimHost(tester->handle);
    ret = TestSdioSetAndGetFuncInfo(tester);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: TestSdioSetAndGetFuncInfo fail! ret=%d.", __func__, ret);
        SdioReleaseHost(tester->handle);
        return HDF_FAILURE;
    }
    SdioReleaseHost(tester->handle);
    return ret;
}

static int32_t TestSdioSetBlockSize(struct SdioTester *tester)
{
    int32_t ret;
    SdioClaimHost(tester->handle);
    ret = SdioSetBlockSize(tester->handle, TEST_BLOCKSIZE);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioSetBlockSize fail! ret=%d.", __func__, ret);
        SdioReleaseHost(tester->handle);
        return HDF_FAILURE;
    }
    SdioReleaseHost(tester->handle);
    return ret;
}

static int32_t TestSdioEnableFunc(struct SdioTester *tester)
{
    int32_t ret;

    SdioClaimHost(tester->handle);
    ret = SdioEnableFunc(tester->handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioEnableFunc fail! ret=%d.", __func__, ret);
        SdioReleaseHost(tester->handle);
        return HDF_FAILURE;
    }
    SdioReleaseHost(tester->handle);
    return ret;
}

static int32_t TestSdioDisableFunc(struct SdioTester *tester)
{
    int32_t ret;

    SdioClaimHost(tester->handle);
    ret = SdioDisableFunc(tester->handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SdioDisableFunc fail! ret=%d.", __func__, ret);
        SdioReleaseHost(tester->handle);
        return HDF_FAILURE;
    }
    SdioReleaseHost(tester->handle);
    return ret;
}

struct SdioTestFunc g_sdioTestFunc[] = {
    { SDIO_DISABLE_FUNC_01, TestSdioDisableFunc },
    { SDIO_ENABLE_FUNC_01, TestSdioEnableFunc },
    { SDIO_SET_BLOCK_SIZE_01, TestSdioSetBlockSize },
    { SDIO_INCR_ADDR_READ_AND_WRITE_BYTES_01, TestSdioIncrAddrReadAndWriteBytes },
    { SDIO_FIXED_ADDR_READ_AND_WRITE_BYTES_01, TestSdioFixedAddrReadAndWriteBytes },
    { SDIO_FUNC0_READ_AND_WRITE_BYTES_01, TestSdioFunc0ReadAndWriteBytes },
    { SDIO_SET_AND_GET_COMMON_INFO_01, TestSdioSetAndGetCommonInfo },
};

static int32_t SdioTestEntry(struct SdioTester *tester, int32_t cmd)
{
    int32_t i;
    int32_t ret = HDF_SUCCESS;
    bool isFind = false;

    if (tester == NULL) {
        HDF_LOGE("%s: tester is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    tester->handle = SdioTestGetHandle(tester);
    if (tester->handle == NULL) {
        HDF_LOGE("%s: sdio test get handle fail", __func__);
        return HDF_FAILURE;
    }
    for (i = 0; i < sizeof(g_sdioTestFunc) / sizeof(g_sdioTestFunc[0]); i++) {
        if (cmd == g_sdioTestFunc[i].type && g_sdioTestFunc[i].Func != NULL) {
            ret = g_sdioTestFunc[i].Func(tester);
            isFind = true;
            break;
        }
    }
    if (!isFind) {
        ret = HDF_ERR_NOT_SUPPORT;
        HDF_LOGE("%s: cmd %d not support", __func__, cmd);
    }
    SdioTestReleaseHandle(tester->handle);
    return ret;
}

static int32_t SdioTestFillConfig(struct SdioTester *tester, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint32(node, "busNum", &(tester->busNum), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fill bus num fail!", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "funcNum", &(tester->config.funcNr), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fill funcNum fail!", __func__);
        return ret;
    }
    ret = drsOps->GetUint16(node, "vendorId", &(tester->config.vendorId), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fill vendorId fail!", __func__);
        return ret;
    }
    ret = drsOps->GetUint16(node, "deviceId", &(tester->config.deviceId), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fill deviceId fail!", __func__);
        return ret;
    }

    HDF_LOGE("%s: busNum:%u, funcNum:%u, vendorId:0x%x, deviceId:0x%x.", __func__,
        tester->busNum, tester->config.funcNr, tester->config.vendorId, tester->config.deviceId);
    return HDF_SUCCESS;
}

static int32_t SdioTestBind(struct HdfDeviceObject *device)
{
    static struct SdioTester tester;

    if (device == NULL) {
        HDF_LOGE("%s: device or config is null!", __func__);
        return HDF_ERR_IO;
    }

    device->service = &tester.service;
    HDF_LOGE("%s: Sdio_TEST service init success!", __func__);
    return HDF_SUCCESS;
}

static int32_t SdioTestInit(struct HdfDeviceObject *device)
{
    struct SdioTester *tester = NULL;
    int32_t ret;

    if (device == NULL || device->service == NULL || device->property == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    tester = (struct SdioTester *)device->service;
    if (tester == NULL) {
        HDF_LOGE("%s: tester is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = SdioTestFillConfig(tester, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config fail!", __func__);
        return ret;
    }
    tester->TestEntry = SdioTestEntry;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static void SdioTestRelease(struct HdfDeviceObject *device)
{
    if (device != NULL) {
        device->service = NULL;
    }
}

struct HdfDriverEntry g_sdioTestEntry = {
    .moduleVersion = 1,
    .Bind = SdioTestBind,
    .Init = SdioTestInit,
    .Release = SdioTestRelease,
    .moduleName = "PLATFORM_SDIO_TEST",
};
HDF_INIT(g_sdioTestEntry);
