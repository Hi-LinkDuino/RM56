/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "mipi_csi_test.h"
#include "hdf_log.h"
#include "osal_time.h"
#include "securec.h"

#define HDF_LOG_TAG mipi_csi_test
#define SEQ_OUTPUT_DELAY 100 /* Delay time of sequential output, unit: ms */

// Input interface type of RX.
enum InterfaceType {
    INTERFACE_MIPI = 0,
    INTERFACE_LVDS,
    INTERFACE_CMOS,
    INTERFACE_BUTT
};

static int32_t MipiCsiTestSetUp(struct MipiCsiTest *test)
{
    HDF_LOGI("%s: enter!", __func__);

    if (test == NULL) {
        HDF_LOGE("%s: test is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (test->handle == NULL) {
        HDF_LOGE("%s: enter! cntlrId:0x%x", __func__, test->cntlrId);
        test->cntlrId = 0;
        test->handle = MipiCsiOpen(test->cntlrId);
        if (test->handle == NULL) {
            HDF_LOGE("%s: failed.", __func__);
            return HDF_FAILURE;
        }
    }

    test->fails = 0;
    HDF_LOGD("%s: cntlrId:0x%x", __func__, test->cntlrId);
    return HDF_SUCCESS;
}

static void MipiCsiTestTearDown(struct MipiCsiTest *test)
{
    if (test == NULL) {
        return;
    }
    if (test->handle != NULL) {
        MipiCsiClose(test->handle);
        test->handle = NULL;
    }
}

/**
 * @brief Initialize the device parameters applying LVDS protocol.
 *
 * This is not fully tested!
 *
 * @since 1.0
 */
static void InitLvdsDevAttr(LvdsDevAttr *lvdsAttr)
{
    int i;
    int j;
    int k;
    LvdsDevAttr attr;

    if (lvdsAttr == NULL) {
        return;
    }

    (void)memset_s(&attr, sizeof(LvdsDevAttr), 0, sizeof(LvdsDevAttr));
    // Hi3516CV500 (DATA_TYPE_RAW_12BIT), Hi3516EV200 (DATA_TYPE_RAW_12BIT)
    attr.inputDataType = DATA_TYPE_RAW_12BIT;
    attr.wdrMode = HI_WDR_MODE_NONE;
    // LVDS synchronization mode. LVDS_SYNC_MODE_SOF, LVDS_SYNC_MODE_SAV
    attr.syncMode = LVDS_SYNC_MODE_SOF;
    // LVDS Vsync type. LVDS_VSYNC_NORMAL, LVDS_VSYNC_SHARE, LVDS_VSYNC_HCONNECT
    attr.vsyncAttr.syncType = LVDS_VSYNC_NORMAL;
    // hconnect vsync blanking len, valid when the syncType is LVDS_VSYNC_HCONNECT
    // This is not fully tested!
    if (attr.vsyncAttr.syncType == LVDS_VSYNC_HCONNECT) {
        attr.vsyncAttr.hblank1 = 0;
        attr.vsyncAttr.hblank2 = 0;
    }
    // frame identification code: LVDS_FID_NONE, LVDS_FID_IN_SAV, LVDS_FID_IN_DATA
    attr.fidAttr.fidType = LVDS_FID_NONE;
    // Sony DOL has the Frame Information Line, in DOL H-Connection mode, should
    // configure this flag as false to disable output the Frame Information Line.
    // This is not fully tested!
    attr.fidAttr.outputFil = 'm';
    // LVDS bit size end mode: LVDS_ENDIAN_LITTLE, LVDS_ENDIAN_BIG
    attr.dataEndian = LVDS_ENDIAN_LITTLE;
    // sync code endian: little/big, LVDS_ENDIAN_LITTLE, LVDS_ENDIAN_BIG
    attr.syncCodeEndian = LVDS_ENDIAN_LITTLE;
    // laneId: -1 - disable
    attr.laneId[0] = 0; /* 0 -- laneId 0 */
    attr.laneId[1] = 1; /* 1 -- laneId 1 */
    attr.laneId[2] = 2; /* 2 -- laneId 2 */
    attr.laneId[3] = 3; /* 3 -- laneId 3 */

    /** each vc has 4 params, syncCode[i]:
        syncMode is SYNC_MODE_SOF: SOF, EOF, SOL, EOL
        syncMode is SYNC_MODE_SAV: invalid sav, invalid eav, valid sav, valid eav */
    // This is not fully tested!
    for (i = 0; i < LVDS_LANE_NUM; i++) {
        for (j = 0; j < WDR_VC_NUM; j++) {
            for (k = 0; k < SYNC_CODE_NUM; k++) {
                attr.syncCode[i][j][k] = 0; /* 0 -- frame0 sof */
            }
        }
    }

    *lvdsAttr = attr;
}

/**
 * @brief Initialize MIPI device attributes.
 *
 * Data type (DT: data type)
 * The data type describes the format and content of load data. Up to 64 data types are supported.
 * There are only two data types for short packets, and the others belong to long packets.
 *
 * @Data type describe
 * 0x00 – 0x07 Synchronous short packet data type
 * 0x08 – 0x0F General short packet data type
 * 0x10 – 0x17 General long packet data type
 * 0x18 – 0x1F YUV data
 * 0x20 – 0x27 RGB data
 * 0x28 – 0x2F Raw data
 * 0x30 – 0x37 User defined byte based data type
 * 0x38 – 0x3F Reserved
 *
 * @since 1.0
 */
static void InitMipiDevAttr(MipiDevAttr *mipiAttr)
{
    MipiDevAttr attr;
    if (mipiAttr == NULL) {
        return;
    }

    HDF_LOGI("%s: enter.", __func__);
    (void)memset_s(&attr, sizeof(MipiDevAttr), 0, sizeof(MipiDevAttr));
    // Hi3516CV500 (DATA_TYPE_RAW_12BIT), Hi3516EV200 (DATA_TYPE_RAW_12BIT)
    attr.inputDataType = DATA_TYPE_RAW_12BIT;
    // Hi3516CV500 (HI_MIPI_WDR_MODE_NONE), Hi3516EV200 (HI_MIPI_WDR_MODE_NONE)
    attr.wdrMode = HI_MIPI_WDR_MODE_NONE;
    // laneId: -1 - disable
    // Hi3516CV500 (0, 1, 2, 3), Hi3516EV200 (0, 1, -1, -1)
    attr.laneId[0] = 0; /* 0 -- laneId 0 */
    attr.laneId[1] = 1; /* 1 -- laneId 1 */
    attr.laneId[2] = 2; /* 2 -- laneId 2 */
    attr.laneId[3] = 3; /* 3 -- laneId 3 */

    // Used by the HI_MIPI_WDR_MODE_DT, This is not fully tested!
    if (attr.wdrMode == HI_MIPI_WDR_MODE_DT) {
        attr.dataType[0] = 0x39; /* 0x39 -- data type reserved */
        attr.dataType[1] = 0x39; /* 0x39 -- data type reserved */
        attr.dataType[2] = 0x39; /* 0x39 -- data type reserved */
        attr.dataType[3] = 0x39; /* 0x39 -- data type reserved */
    }

    *mipiAttr = attr;
}

static int MipiGetIntputModeType(InputMode inputMode)
{
    switch (inputMode) {
        case INPUT_MODE_SUBLVDS:
        case INPUT_MODE_LVDS:
        case INPUT_MODE_HISPI:
            return INTERFACE_LVDS;
        case INPUT_MODE_MIPI:
            return INTERFACE_MIPI;
        case INPUT_MODE_CMOS:
        case INPUT_MODE_BT1120:
        case INPUT_MODE_BT656:
        case INPUT_MODE_BYPASS:
            return INTERFACE_CMOS;
        default:
            break;
    }

    return INTERFACE_BUTT;
}

static void InitComboDevAttr(struct MipiCsiTest *test)
{
    enum InterfaceType interType;
    ComboDevAttr attr;

    (void)memset_s(&attr, sizeof(ComboDevAttr), 0, sizeof(ComboDevAttr));
    // Hi3516CV500(0), Hi3516EV200(0)
    attr.devno = 0;
    // Hi3516CV500(INPUT_MODE_MIPI), Hi3516EV200(INPUT_MODE_MIPI)
    attr.inputMode = INPUT_MODE_MIPI;
    // Hi3516CV500(MIPI_DATA_RATE_X1), Hi3516EV200(MIPI_DATA_RATE_X1)
    attr.dataRate = MIPI_DATA_RATE_X1;
    // Hi3516CV500 (0, 204, 2592, 1944), Hi3516EV200 (0, 204, 2304, 1296), Hi3516DV300-imx335 (0, 0, 2592, 1944)
    attr.imgRect.x = 0; /* 0: image sensor left-top position */
    attr.imgRect.y = 0; /* 0: image sensor right-top position */
    attr.imgRect.width = 2592; /* 2592: image sensor width size */
    attr.imgRect.height = 1944; /* 1944: image sensor height size */

    interType = MipiGetIntputModeType(attr.inputMode);
    if (interType == INTERFACE_MIPI) {
        HDF_LOGI("%s: call[InitMipiDevAttr].", __func__);
        InitMipiDevAttr(&attr.mipiAttr);
    } else if (interType == INTERFACE_LVDS) {
        HDF_LOGI("%s: call[InitLvdsDevAttr].", __func__);
        InitLvdsDevAttr(&attr.lvdsAttr);
    } else {
        HDF_LOGE("%s: interType = %d is error!", __func__, attr.inputMode);
    }

    test->attr = attr;
}

static int32_t MipiCsiSetComboDevAttrTest(struct MipiCsiTest *test)
{
    int32_t ret;

    InitComboDevAttr(test);
    ret = MipiCsiSetComboDevAttr(test->handle, &(test->attr));
    if ((ret == HDF_SUCCESS) || (ret == HDF_ERR_NOT_SUPPORT)) {
        return HDF_SUCCESS;
    }

    HDF_LOGE("%s: failed.", __func__);
    return ret;
}

static int32_t MipiCsiSetPhyCmvmodeTest(struct MipiCsiTest *test)
{
    int32_t ret;

    test->devno = 0;
    // PHY_CMV_GE1200MV or PHY_CMV_LT1200MV
    test->cmvMode = PHY_CMV_GE1200MV;
    ret = MipiCsiSetPhyCmvmode(test->handle, test->devno, test->cmvMode);
    if ((ret == HDF_SUCCESS) || (ret == HDF_ERR_NOT_SUPPORT)) {
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: failed.", __func__);

    return ret;
}

static int32_t MipiCsiResetSensorTest(struct MipiCsiTest *test)
{
    int32_t ret;

    // snsResetSource select 0 or 1, and must be less than SNS_MAX_RST_SOURCE_NUM
    test->snsResetSource = 0;
    ret = MipiCsiResetSensor(test->handle, test->snsResetSource);
    if ((ret == HDF_SUCCESS) || (ret == HDF_ERR_NOT_SUPPORT)) {
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: failed.", __func__);

    return ret;
}

static int32_t MipiCsiUnresetSensorTest(struct MipiCsiTest *test)
{
    int32_t ret;

    // snsResetSource select 0 or 1, and must be less than SNS_MAX_RST_SOURCE_NUM
    test->snsResetSource = 0;
    ret = MipiCsiUnresetSensor(test->handle, test->snsResetSource);
    if ((ret == HDF_SUCCESS) || (ret == HDF_ERR_NOT_SUPPORT)) {
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: failed.", __func__);

    return ret;
}

static int32_t MipiCsiResetRxTest(struct MipiCsiTest *test)
{
    int32_t ret;

    // comboDev select 0 or 1, and must be less than MIPI_RX_MAX_DEV_NUM
    test->comboDev = 0;
    ret = MipiCsiResetRx(test->handle, test->comboDev);
    if ((ret == HDF_SUCCESS) || (ret == HDF_ERR_NOT_SUPPORT)) {
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: failed.", __func__);

    return ret;
}

static int32_t MipiCsiUnresetRxTest(struct MipiCsiTest *test)
{
    int32_t ret;

    // comboDev select 0 or 1, and must be less than MIPI_RX_MAX_DEV_NUM
    test->comboDev = 0;
    ret = MipiCsiUnresetRx(test->handle, test->comboDev);
    if ((ret == HDF_SUCCESS) || (ret == HDF_ERR_NOT_SUPPORT)) {
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: failed.", __func__);

    return ret;
}

static int32_t MipiCsiSetHsModeTest(struct MipiCsiTest *test)
{
    int32_t ret;

    // laneDivideMode: Hi3516CV500(LANE_DIVIDE_MODE_0), Hi3516EV200(LANE_DIVIDE_MODE_0)
    test->laneDivideMode = LANE_DIVIDE_MODE_0;
    ret = MipiCsiSetHsMode(test->handle, test->laneDivideMode);
    if ((ret == HDF_SUCCESS) || (ret == HDF_ERR_NOT_SUPPORT)) {
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: failed.", __func__);

    return ret;
}

static int32_t MipiCsiEnableMipiClockTest(struct MipiCsiTest *test)
{
    int32_t ret;

    // comboDev select 0 or 1, and must be less than MIPI_RX_MAX_DEV_NUM
    test->comboDev = 0;
    ret = MipiCsiEnableClock(test->handle, test->comboDev);
    if ((ret == HDF_SUCCESS) || (ret == HDF_ERR_NOT_SUPPORT)) {
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: failed.", __func__);

    return ret;
}

static int32_t MipiCsiDisableMipiClockTest(struct MipiCsiTest *test)
{
    int32_t ret;

    // comboDev select 0 or 1, and must be less than MIPI_RX_MAX_DEV_NUM
    test->comboDev = 0;
    ret = MipiCsiDisableClock(test->handle, test->comboDev);
    if ((ret == HDF_SUCCESS) || (ret == HDF_ERR_NOT_SUPPORT)) {
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: failed.", __func__);

    return ret;
}

static int32_t MipiCsiEnableSensorClockTest(struct MipiCsiTest *test)
{
    int32_t ret;

    // snsClkSource select 0 or 1, and must be less than SNS_MAX_CLK_SOURCE_NUM
    test->snsClkSource = 0;
    ret = MipiCsiEnableSensorClock(test->handle, test->snsClkSource);
    if ((ret == HDF_SUCCESS) || (ret == HDF_ERR_NOT_SUPPORT)) {
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: failed.", __func__);

    return ret;
}

static int32_t MipiCsiDisableSensorClockTest(struct MipiCsiTest *test)
{
    int32_t ret;

    // snsClkSource select 0 or 1, and must be less than SNS_MAX_CLK_SOURCE_NUM
    test->snsClkSource = 0;
    ret = MipiCsiDisableSensorClock(test->handle, test->snsClkSource);
    if ((ret == HDF_SUCCESS) || (ret == HDF_ERR_NOT_SUPPORT)) {
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: failed.", __func__);

    return ret;
}

static int32_t MipiCsiSetExtDataTypeTest(struct MipiCsiTest *test)
{
    int32_t ret;
    ExtDataType dataType;
    dataType.devno = 0;
    dataType.num = 0;

    // extDataBitWidth set 8, 10, 12, 14 or 16.
    dataType.extDataBitWidth[0] = 12; /* 12:magic bit width */
    dataType.extDataBitWidth[1] = 12; /* 12:magic bit width */
    dataType.extDataBitWidth[2] = 12; /* 12:magic bit width */

    dataType.extDataType[0] = 0x39; /* 0x39 -- data type reserved */
    dataType.extDataType[1] = 0x39; /* 0x39 -- data type reserved */
    dataType.extDataType[2] = 0x39; /* 0x39 -- data type reserved */
    test->dataType = dataType;

    ret = MipiCsiSetExtDataType(test->handle, &test->dataType);
    if ((ret == HDF_SUCCESS) || (ret == HDF_ERR_NOT_SUPPORT)) {
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: failed.", __func__);

    return ret;
}

static int32_t MipiCsiTestByCmd(struct MipiCsiTest *test, uint8_t cmd)
{
    int32_t ret = HDF_FAILURE;

    if (test == NULL) {
        HDF_LOGE("%s: test is NULL", __func__);
        return ret;
    }

    HDF_LOGI("%s: test cmd %u, enter.", __func__, cmd);
    switch (cmd) {
        case MIPI_CSI_TEST_SET_HS_MODE:
            ret = MipiCsiSetHsModeTest(test);
            break;
        case MIPI_CSI_TEST_DISABLE_MIPI_CLOCK:
            ret = MipiCsiDisableMipiClockTest(test);
            break;
        case MIPI_CSI_TEST_ENABLE_MIPI_CLOCK:
            ret = MipiCsiEnableMipiClockTest(test);
            break;
        case MIPI_CSI_TEST_RESET_RX:
            ret = MipiCsiResetRxTest(test);
            break;
        case MIPI_CSI_TEST_DISABLE_SENSOR_CLOCK:
            ret = MipiCsiDisableSensorClockTest(test);
            break;
        case MIPI_CSI_TEST_ENABLE_SENSOR_CLOCK:
            ret = MipiCsiEnableSensorClockTest(test);
            break;
        case MIPI_CSI_TEST_RESET_SENSOR:
            ret = MipiCsiResetSensorTest(test);
            break;
        case MIPI_CSI_TEST_SET_DEV_ATTR:
            ret = MipiCsiSetComboDevAttrTest(test);
            break;
        case MIPI_CSI_TEST_SET_PHY_CMVMODE:
            ret = MipiCsiSetPhyCmvmodeTest(test);
            break;
        case MIPI_CSI_TEST_SET_EXT_DATA_TYPE:
            ret = MipiCsiSetExtDataTypeTest(test);
            break;
        case MIPI_CSI_TEST_UNRESET_RX:
            ret = MipiCsiUnresetRxTest(test);
            break;
        case MIPI_CSI_TEST_UNRESET_SENSOR:
            ret = MipiCsiUnresetSensorTest(test);
            break;
        default:
            HDF_LOGE("%s: not support", __func__);
            break;
    }
    HDF_LOGI("%s: test cmd %u, ret %d done.", __func__, cmd, ret);

    return ret;
}

static int32_t MipiCsiTestDoTest(struct MipiCsiTest *test, uint8_t cmd)
{
    int32_t ret;
    OsalMSleep(SEQ_OUTPUT_DELAY);

    if (test == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

#ifdef MIPI_CSI_TEST_SELF
    int32_t i;
    if (cmd != 0) {
        return HDF_SUCCESS;
    }
    ret = MipiCsiTestSetUp(test);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: setup fail!", __func__);
        return ret;
    }

    for (i = 0; i < test->total; i++) {
        if (MipiCsiTestByCmd(test, i) != HDF_SUCCESS) {
            test->fails++;
        }
    }
    HDF_LOGI("\n\n%s: **********PASS:%u  FAIL:%u**************\n\n",
        __func__, test->total - test->fails, test->fails);
    ret = (test->fails > 0) ? HDF_FAILURE : HDF_SUCCESS;
    MipiCsiTestTearDown(test);
#else
    if (cmd == 0) {
        ret = MipiCsiTestSetUp(test);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: [MipiCsiTestSetUp] failed!", __func__);
            return ret;
        }
    }
    ret = MipiCsiTestByCmd(test, cmd);
    if (cmd == (test->total - 1)) {
        MipiCsiTestTearDown(test);
    }
#endif
    OsalMSleep(SEQ_OUTPUT_DELAY);

    return ret;
}

static int32_t MipiCsiTestBind(struct HdfDeviceObject *device)
{
    static struct MipiCsiTest test;

    test.cntlrId = 0;
    test.total = MIPI_CSI_TEST_MAX;
    test.doTest = MipiCsiTestDoTest;
    device->service = &test.service;
#ifdef MIPI_CSI_TEST_SELF
    HDF_LOGI("%s: test on init!", __func__);
    test.doTest(&test, -1);
#endif
    return HDF_SUCCESS;
}

static int32_t MipiCsiTestInit(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

struct HdfDriverEntry g_mipiCsiTestEntry = {
    .moduleVersion = 1,
    .Bind = MipiCsiTestBind,
    .Init = MipiCsiTestInit,
    .moduleName = "PLATFORM_MIPI_CSI_TEST",
};
HDF_INIT(g_mipiCsiTestEntry);
