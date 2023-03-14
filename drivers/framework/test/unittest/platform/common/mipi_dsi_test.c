/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "mipi_dsi_test.h"
#include "hdf_log.h"

#define HDF_LOG_TAG mipi_dsi_test

static int32_t MipiDsiTestMallocBuf(struct MipiDsiTest *test)
{
    static uint8_t buf = 0x29;

    test->msgs.dataType = 0x05;    /* 0x05: data type */
    test->msgs.dataLen  = 1;       /* 1: data len */
    test->msgs.delay = 0;
    test->msgs.payload = &buf;

    return HDF_SUCCESS;
}

static int32_t MipiDsiTestSetUp(struct MipiDsiTest *test)
{
    if (test == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (test->handle == NULL) {
        test->handle = MipiDsiOpen(test->devNo);
    }
    if (test->handle == NULL) {
        HDF_LOGE("%s: fail!", __func__);
        return HDF_FAILURE;
    }

    test->fails = 0;
    HDF_LOGD("%s: devNo:0x%x", __func__, test->devNo);
    return MipiDsiTestMallocBuf(test);
}

static void MipiDsiTestTearDown(struct MipiDsiTest *test)
{
    if (test == NULL) {
        return;
    }
    if (test->handle != NULL) {
        MipiDsiClose(test->handle);
        test->handle = NULL;
    }
}

static int32_t CmpStVal(struct MipiCfg *a, struct MipiCfg *b)
{
    if (a->lane == b->lane &&
        a->mode == b->mode &&
        a->format == b->format &&
        a->burstMode == b->burstMode &&
        a->pixelClk == b->pixelClk &&
        a->phyDataRate == b->phyDataRate &&
        a->timing.xPixels == b->timing.xPixels &&
        a->timing.hsaPixels == b->timing.hsaPixels &&
        a->timing.hbpPixels == b->timing.hbpPixels &&
        a->timing.hlinePixels == b->timing.hlinePixels &&
        a->timing.vsaLines == b->timing.vsaLines &&
        a->timing.vbpLines == b->timing.vbpLines &&
        a->timing.vfpLines == b->timing.vfpLines &&
        a->timing.ylines == b->timing.ylines &&
        a->timing.edpiCmdSize == b->timing.edpiCmdSize) {
            return true;
    }
    return false;
}

static int32_t MipiDsiGetCfgTest(struct MipiDsiTest *test)
{
    int32_t ret;
    struct MipiCfg cfg;

    ret = MipiDsiGetCfg(test->handle, &cfg);
    if (ret == HDF_SUCCESS) {
        if (CmpStVal(&cfg, &test->cfg)) {
            return HDF_SUCCESS;
        }
    }
    return HDF_FAILURE;
}

static int32_t MipiDsiTxRxTest(struct MipiDsiTest *test)
{
    int32_t ret;
    uint8_t data = 0x0A;   /* 0x0A: lcd status reg */
    struct DsiCmdDesc cmd = {
        .dataType = 0x06,     /* 0x06: read data type */
        .dataLen = 1,         /* 1: data len */
        .delay = 0,
        .payload = &data,
    };
    uint8_t buf = 0;

    ret = MipiDsiTx(test->handle, &test->msgs);
    if (ret == HDF_SUCCESS) {
        ret = MipiDsiRx(test->handle, &cmd, 1, &buf); /* 1: read len */
        if (ret == HDF_SUCCESS || ret == HDF_ERR_NOT_SUPPORT) {
            return HDF_SUCCESS;
        }
    } else if (ret == HDF_ERR_NOT_SUPPORT) {
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: fail", __func__);
    return HDF_FAILURE;
}

static int32_t MipiDsiToLpToHsTest(struct MipiDsiTest *test)
{
    MipiDsiSetHsMode(test->handle);
    MipiDsiSetLpMode(test->handle);
    return HDF_SUCCESS;
}

static int32_t MipiDsiTestByCmd(struct MipiDsiTest *test, int32_t cmd)
{
    int32_t ret = HDF_FAILURE;

    switch (cmd) {
        case MIPI_DSI_TEST_SET_CFG:
            ret = MipiDsiSetCfg(test->handle, &test->cfg);
            break;
        case MIPI_DSI_TEST_GET_CFG:
            ret = MipiDsiGetCfgTest(test);
            break;
        case MIPI_DSI_TEST_TX_RX:
            ret = MipiDsiTxRxTest(test);
            break;
        case MIPI_DSI_TEST_TO_LP_TO_HS:
            ret = MipiDsiToLpToHsTest(test);
            break;
        default:
            HDF_LOGE("%s: not support", __func__);
            break;
    }
    HDF_LOGI("test cmd %d, ret %d", cmd, ret);
    return ret;
}

static int32_t MipiDsiTestDoTest(struct MipiDsiTest *test, int32_t cmd)
{
    int32_t ret;

    if (test == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = MipiDsiTestSetUp(test);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: setup fail!", __func__);
        return ret;
    }
#ifdef MIPI_DSI_TEST_ON_INIT
    int32_t i;
    for (i = 0; i < test->total; i++) {
        if (MipiDsiTestByCmd(test, i) != HDF_SUCCESS) {
            test->fails++;
        }
    }
    HDF_LOGI("\n\n%s: **********PASS:%u  FAIL:%u**************\n\n",
        __func__, test->total - test->fails, test->fails);
    ret = (test->fails > 0) ? HDF_FAILURE : HDF_SUCCESS;
#else
    ret = MipiDsiTestByCmd(test, cmd);
#endif
    MipiDsiTestTearDown(test);
    return ret;
}

static int32_t MipiDsiTestBind(struct HdfDeviceObject *device)
{
    static struct MipiDsiTest test;

    test.cfg.lane = DSI_2_LANES;
    test.cfg.mode = DSI_VIDEO_MODE;
    test.cfg.format = FORMAT_RGB_24_BIT;
    test.cfg.burstMode = VIDEO_BURST_MODE;
    test.cfg.timing.xPixels = 480;       /* 480: width */
    test.cfg.timing.hsaPixels = 10;      /* 10: horizontal sync porch */
    test.cfg.timing.hbpPixels = 20;      /* 20: horizontal back porch */
    test.cfg.timing.hlinePixels = 530;   /* 530: horizontal total width */
    test.cfg.timing.vsaLines = 2;        /* 2: vertiacl sync width */
    test.cfg.timing.vbpLines = 14;       /* 14: vertiacl back porch */
    test.cfg.timing.vfpLines = 16;       /* 16: vertiacl front porch */
    test.cfg.timing.ylines = 960;        /* 960: height */
    /* 0 : no care */
    test.cfg.timing.edpiCmdSize = 0;
    /*
     * pixel clk(kHZ):
     * calculate: (width + hbp + hfp + hsw) * (height + vbp + vfp + vsw) * frameRate / 1000
     */
    test.cfg.pixelClk = 31546;           /* 31546: pixel clk */
    /*
     * phy data rate(Mbps):
     * calculate: (pixelClk / 1000) * 24 / laneNum
     */
    test.cfg.phyDataRate = 379;          /* 379: mipi clk */
    test.devNo = 0;
    test.total = MIPI_DSI_TEST_MAX;
    test.doTest = MipiDsiTestDoTest;
    device->service = &test.service;
#ifdef MIPI_DSI_TEST_ON_INIT
    HDF_LOGI("%s: test on init!", __func__);
    test.doTest(&test, -1);
#endif
    return HDF_SUCCESS;
}

static int32_t MipiDsiTestInit(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

struct HdfDriverEntry g_mipiDsiTestEntry = {
    .moduleVersion = 1,
    .Bind = MipiDsiTestBind,
    .Init = MipiDsiTestInit,
    .moduleName = "PLATFORM_MIPI_DSI_TEST",
};
HDF_INIT(g_mipiDsiTestEntry);
