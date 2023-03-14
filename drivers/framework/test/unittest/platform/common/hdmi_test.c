/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdmi_test.h"
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdmi_if.h"
#include "osal_time.h"

#define HDF_LOG_TAG hdmi_test_c

struct HdmiTestFunc {
    enum HdmiTestCmd type;
    int32_t (*Func)(struct HdmiTester *tester);
};

static DevHandle HdmiTestGetHandle(struct HdmiTester *tester)
{
    if (tester == NULL) {
        HDF_LOGE("%s: tester is null", __func__);
        return NULL;
    }
    return HdmiOpen(tester->busNum);
}

static void HdmiTestReleaseHandle(DevHandle handle)
{
    if (handle == NULL) {
        HDF_LOGE("%s: sdio handle is null", __func__);
        return;
    }
    HdmiClose(handle);
}

static int32_t TestHdmiStartAndStop(struct HdmiTester *tester)
{
    int32_t ret;

    ret = HdmiStart(tester->handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HdmiStart failed ret = %d.", __func__, ret);
        return ret;
    }
    ret = HdmiStop(tester->handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HdmiStop failed ret = %d.", __func__, ret);
    }
    return ret;
}

static int32_t TestHdmiSetAudioAttr(struct HdmiTester *tester)
{
    int32_t ret;
    struct HdmiAudioAttr attr = {0};

    attr.codingType = HDMI_AUDIO_CODING_TYPE_STREAM;
    attr.ifType = HDMI_AUDIO_IF_TYPE_I2S;
    attr.bitDepth = HDMI_ADIO_BIT_DEPTH_20;
    attr.sampleRate = HDMI_SAMPLE_RATE_12K;
    attr.channels = HDMI_AUDIO_FORMAT_CHANNEL_5;
    ret = HdmiSetAudioAttribute(tester->handle, &attr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HdmiSetAudioAttribute failed ret = %d.", __func__, ret);
    }
    return ret;
}

static int32_t TestHdmiSetVideoAttr(struct HdmiTester *tester)
{
    int32_t ret;
    struct HdmiVideoAttr attr = {0};

    attr.xvycc = true;
    attr.nups = HDMI_NUPS_HORIZONTAL;
    attr.colorSpace = HDMI_COLOR_SPACE_YCBCR420;
    attr.colorimetry = HDMI_COLORIMETRY_ITU709;
    attr.quantization = HDMI_QUANTIZATION_RANGE_FULL;
    ret = HdmiSetVideoAttribute(tester->handle, &attr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HdmiSetAudioAttribute failed ret = %d.", __func__, ret);
    }
    return ret;
}

static int32_t TestHdmiSetHdrAttr(struct HdmiTester *tester)
{
    int32_t ret;
    struct HdmiHdrAttr attr = {0};

    attr.mode = HDMI_HDR_MODE_DOLBY_NORMAL;
    attr.userMode = HDMI_HDR_USERMODE_DOLBY;
    attr.eotfType = HDMI_EOTF_HLG;
    attr.colorimetry = HDMI_HDR_EXTENDED_COLORIMETRY_S_YCC_601;
    attr.metadataType = HDMI_DRM_STATIC_METADATA_TYPE_1;
    ret = HdmiSetHdrAttribute(tester->handle, &attr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HdmiSetHdrAttribute failed ret=%d.", __func__, ret);
    }
    return ret;
}

static int32_t TestHdmiSetAvmute(struct HdmiTester *tester)
{
    int32_t ret;

    ret = HdmiAvmuteSet(tester->handle, true);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HdmiAvmuteSet failed ret = %d.", __func__, ret);
        return ret;
    }

    ret = HdmiAvmuteSet(tester->handle, false);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HdmiAvmuteSet failed ret = %d.", __func__, ret);
    }
    return ret;
}

static int32_t TestHdmiEdidRawDataGet(struct HdmiTester *tester)
{
    int32_t len;
    uint8_t edid[HDMI_EDID_MAX_LEN] = {0};

    len = HdmiReadSinkEdid(tester->handle, edid, HDMI_EDID_MAX_LEN);
    if (len < 0) {
        HDF_LOGE("%s: HdmiReadSinkEdid failed len = %d.", __func__, len);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: HdmiReadSinkEdid success, len = %d, edid[1] = 0x%x.", __func__, len, edid[1]);
    return HDF_SUCCESS;
}

static int32_t TestHdmiDeepColorSetAndGet(struct HdmiTester *tester)
{
    int32_t ret;
    enum HdmiDeepColor color = HDMI_DEEP_COLOR_BUTT;

    ret = HdmiDeepColorSet(tester->handle, HDMI_DEEP_COLOR_48BITS);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HdmiDeepColorSet failed ret = %d.", __func__, ret);
    }
    ret = HdmiDeepColorGet(tester->handle, &color);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HdmiDeepColorGet failed ret = %d.", __func__, ret);
    } else {
        HDF_LOGE("%s: HdmiDeepColorGet success, color = %d.", __func__, color);
    }
    return ret;
}

static void TestHdmiHdpHandle(void *data, bool hdp)
{
    if (data == NULL) {
        printf("data is null");
    }

    if (hdp == true) {
        HDF_LOGE("TestHdmiHdpHandle: hdp.");
    } else {
        HDF_LOGE("TestHdmiHdpHandle: unhdp.");
    }
}

static int32_t TestHdmiHpdRegisterAndUnregister(struct HdmiTester *tester)
{
    int32_t ret;
    struct HdmiHpdCallbackInfo info;

    info.data = NULL;
    info.callbackFunc = TestHdmiHdpHandle;
    ret = HdmiRegisterHpdCallbackFunc(tester->handle, &info);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: TestHdmiHpdRegisterAndUnregister failed ret = %d.", __func__, ret);
    }
    ret = HdmiUnregisterHpdCallbackFunc(tester->handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HdmiUnregisterHpdCallbackFunc failed ret = %d.", __func__, ret);
    }
    return ret;
}

struct HdmiTestFunc g_hdmiTestFunc[] = {
    { HDMI_START_AND_STOP_01, TestHdmiStartAndStop },
    { HDMI_SET_AUDIO_ATTR_01, TestHdmiSetAudioAttr },
    { HDMI_SET_VIDEO_ATTR_01, TestHdmiSetVideoAttr },
    { HDMI_SET_HDR_ATTR_01, TestHdmiSetHdrAttr },
    { HDMI_SET_AVMUTE_01, TestHdmiSetAvmute },
    { HDMI_EDID_RAW_DATA_GET_01, TestHdmiEdidRawDataGet },
    { HDMI_DEEP_COLOR_SET_AND_GET_01, TestHdmiDeepColorSetAndGet },
    { HDMI_HPD_REGISTER_AND_UNREGISTER_01, TestHdmiHpdRegisterAndUnregister },
};

static int32_t HdmiTestEntry(struct HdmiTester *tester, int32_t cmd)
{
    int32_t i;
    int32_t ret = HDF_SUCCESS;
    bool isFind = false;

    if (tester == NULL) {
        HDF_LOGE("%s: tester is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    tester->handle = HdmiTestGetHandle(tester);
    if (tester->handle == NULL) {
        HDF_LOGE("%s: hdmi test get handle failed", __func__);
        return HDF_FAILURE;
    }
    for (i = 0; i < sizeof(g_hdmiTestFunc) / sizeof(g_hdmiTestFunc[0]); i++) {
        if (cmd == g_hdmiTestFunc[i].type && g_hdmiTestFunc[i].Func != NULL) {
            ret = g_hdmiTestFunc[i].Func(tester);
            isFind = true;
            break;
        }
    }
    if (!isFind) {
        ret = HDF_ERR_NOT_SUPPORT;
        HDF_LOGE("%s: cmd %d not supported", __func__, cmd);
    }
    HdmiTestReleaseHandle(tester->handle);
    return ret;
}

static int32_t HdmiTestFillConfig(struct HdmiTester *tester, const struct DeviceResourceNode *node)
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

static int32_t HdmiTestBind(struct HdfDeviceObject *device)
{
    static struct HdmiTester tester;

    if (device == NULL) {
        HDF_LOGE("%s: device or config is null!", __func__);
        return HDF_ERR_IO;
    }

    device->service = &tester.service;
    HDF_LOGE("%s: HDMI_TEST service init success!", __func__);
    return HDF_SUCCESS;
}

static int32_t HdmiTestInit(struct HdfDeviceObject *device)
{
    struct HdmiTester *tester = NULL;
    int32_t ret;

    if (device == NULL || device->service == NULL || device->property == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    tester = (struct HdmiTester *)device->service;
    if (tester == NULL) {
        HDF_LOGE("%s: tester is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = HdmiTestFillConfig(tester, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed", __func__);
        return ret;
    }
    tester->TestEntry = HdmiTestEntry;
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static void HdmiTestRelease(struct HdfDeviceObject *device)
{
    if (device != NULL) {
        device->service = NULL;
    }
}

struct HdfDriverEntry g_hdmiTestEntry = {
    .moduleVersion = 1,
    .Bind = HdmiTestBind,
    .Init = HdmiTestInit,
    .Release = HdmiTestRelease,
    .moduleName = "PLATFORM_HDMI_TEST",
};
HDF_INIT(g_hdmiTestEntry);
