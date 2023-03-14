/*
 * Copyright (c) 2021 Shenzhen Kaihong DID Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <servmgr_hdi.h>
#include <gtest/gtest.h>
#include <osal_mem.h>
#include <hdf_log.h>
#include <securec.h>
#include "codec_callback_type_stub.h"
#include "codec_component_type.h"
#include "codec_component_manager.h"
#include "hdf_io_service_if.h"

#define HDF_LOG_TAG codec_hdi_test

using namespace std;
using namespace testing::ext;

namespace {
struct CodecComponentManager *g_manager = nullptr;
struct CodecComponentType *g_component = nullptr;
int32_t g_count = 0;

const int32_t DATA_BUFFERID = 10;
const int32_t DATA_SIZE = 20;
const int32_t DATA_VERSION_NVERSION = 30;
const int32_t DATA_BUFFERTYPE = 40;
const int32_t DATA_BUFFERLEN = 50;
const int32_t DATA_ALLOCLEN = 60;
const int32_t DATA_FILLEDLEN = 70;
const int32_t DATA_OFFSET = 80;
const int32_t DATA_FENCEFD = 90;
const int32_t DATA_TYPE = 100;
const int32_t DATA_PTS = 200;
const int32_t DATA_FLAG = 300;
const int32_t ARRAY_TO_STR_LEN = 1000;
const int32_t INT_TO_STR_LEN = 32;

class CodecHdiOmxTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

static void FillDataOmxCodecBuffer(struct OmxCodecBuffer *data)
{
    data->bufferId = DATA_BUFFERID;
    data->size = DATA_SIZE;
    data->version.nVersion = DATA_VERSION_NVERSION;
    data->bufferType = (enum BufferType)DATA_BUFFERTYPE;
    data->buffer = (uint8_t*)OsalMemAlloc(DATA_BUFFERLEN);
    data->bufferLen = DATA_BUFFERLEN;
    data->allocLen = DATA_ALLOCLEN;
    data->filledLen = DATA_FILLEDLEN;
    data->offset = DATA_OFFSET;
    data->fenceFd = DATA_FENCEFD;
    data->type = (enum ShareMemTypes)DATA_TYPE;
    data->pts = DATA_PTS;
    data->flag = DATA_FLAG;
}

static char arrayStr[ARRAY_TO_STR_LEN];
static char* GetArrayStr(int32_t *array, int32_t arrayLen, int32_t endValue)
{
    int32_t len = 0;
    int32_t totalLen = 0;
    int32_t ret;
    char value[INT_TO_STR_LEN];
    ret = memset_s(arrayStr, sizeof(arrayStr), 0, sizeof(arrayStr));
    if (ret != EOK) {
        HDF_LOGE("%{public}s: memset_s arrayStr failed, error code: %{public}d", __func__, ret);
        return arrayStr;
    }
    for (int32_t i = 0; i < arrayLen; i++) {
        if (array[i] == endValue) {
            break;
        }
        ret = memset_s(value, sizeof(value), 0, sizeof(value));
        if (ret != EOK) {
            HDF_LOGE("%{public}s: memset_s value failed, error code: %{public}d", __func__, ret);
            return arrayStr;
        }
        ret = sprintf_s(value, sizeof(value) - 1, "0x0%X, ", array[i]);
        if (ret < 0) {
            HDF_LOGE("%{public}s: sprintf_s value failed, error code: %{public}d", __func__, ret);
            return arrayStr;
        }
        len = strlen(value);
        ret = memcpy_s(arrayStr + totalLen, len, value, len);
        if (ret != EOK) {
            HDF_LOGE("%{public}s: memcpy_s arrayStr failed, error code: %{public}d", __func__, ret);
            return arrayStr;
        }
        totalLen += len;
    }
    return arrayStr;
}

static void PrintCapability(CodecCompCapability *cap, int32_t index)
{
    HDF_LOGI("---------------------- component capability %{public}d ---------------------", index + 1);
    HDF_LOGI("role:%{public}d", cap->role);
    HDF_LOGI("type:%{public}d", cap->type);
    HDF_LOGI("compName:%{public}s", cap->compName);
    HDF_LOGI("supportProfiles:%{public}s", GetArrayStr(cap->supportProfiles, PROFILE_NUM, INVALID_PROFILE));
    HDF_LOGI("maxInst:%{public}d", cap->maxInst);
    HDF_LOGI("isSoftwareCodec:%{public}d", cap->isSoftwareCodec);
    HDF_LOGI("processModeMask:0x0%{public}x", cap->processModeMask);
    HDF_LOGI("capsMask:0x0%{public}x", cap->capsMask);
    HDF_LOGI("bitRate.min:%{public}d", cap->bitRate.min);
    HDF_LOGI("bitRate.max:%{public}d", cap->bitRate.max);
    if (strstr(cap->compName, "video") != NULL) {
        HDF_LOGI("minSize.width:%{public}d", cap->port.video.minSize.width);
        HDF_LOGI("minSize.height:%{public}d", cap->port.video.minSize.height);
        HDF_LOGI("maxSize.width:%{public}d", cap->port.video.maxSize.width);
        HDF_LOGI("maxSize.height:%{public}d", cap->port.video.maxSize.height);
        HDF_LOGI("widthAlignment:%{public}d", cap->port.video.whAlignment.widthAlignment);
        HDF_LOGI("heightAlignment:%{public}d", cap->port.video.whAlignment.heightAlignment);
        HDF_LOGI("blockCount.min:%{public}d", cap->port.video.blockCount.min);
        HDF_LOGI("blockCount.max:%{public}d", cap->port.video.blockCount.max);
        HDF_LOGI("blocksPerSecond.min:%{public}d", cap->port.video.blocksPerSecond.min);
        HDF_LOGI("blocksPerSecond.max:%{public}d", cap->port.video.blocksPerSecond.max);
        HDF_LOGI("blockSize.width:%{public}d", cap->port.video.blockSize.width);
        HDF_LOGI("blockSize.height:%{public}d", cap->port.video.blockSize.height);
        HDF_LOGI("supportPixFmts:%{public}s", GetArrayStr(cap->port.video.supportPixFmts, PIX_FORMAT_NUM, 0));
    } else {
        HDF_LOGI(":%{public}s", GetArrayStr(cap->port.audio.sampleFormats, SAMPLE_FMT_NUM, AUDIO_SAMPLE_FMT_INVALID));
        HDF_LOGI(":%{public}s", GetArrayStr(cap->port.audio.sampleRate, SAMPLE_RATE_NUM, AUD_SAMPLE_RATE_INVALID));
        HDF_LOGI(":%{public}s", GetArrayStr(cap->port.audio.channelLayouts, CHANNEL_NUM, -1));
        HDF_LOGI(":%{public}s", GetArrayStr(cap->port.audio.channelCount, CHANNEL_NUM, -1));
    }
    HDF_LOGI("-------------------------------------------------------------------");
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_001, TestSize.Level1)
{
    const int32_t componentCount = 9;
    g_manager = GetCodecComponentManager();
    ASSERT_TRUE(g_manager != nullptr);

    g_count = g_manager->GetComponentNum();
    ASSERT_EQ(g_count, componentCount);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_002, TestSize.Level1)
{
    ASSERT_TRUE(g_count > 0);
    CodecCompCapability *capList = (CodecCompCapability *)OsalMemAlloc(sizeof(CodecCompCapability) * g_count);
    g_manager->GetComponentCapabilityList(capList, g_count);
    for (int32_t i = 0; i < g_count; i++) {
        PrintCapability(&capList[i], i);
    }
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_003, TestSize.Level1)
{
    const int32_t testingAppData = 33;
    CodecCallbackType* callback = CodecCallbackTypeStubGetInstance();
    int32_t appData = testingAppData;
    int32_t ret = g_manager->CreateComponent(&g_component, (char*)"compName", &appData, sizeof(appData), callback);
    HDF_LOGE("%{public}s: after CreateComponent!", __func__);
    ASSERT_EQ(ret, HDF_SUCCESS);
    ASSERT_TRUE(g_component != nullptr);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_004, TestSize.Level1)
{
    const int32_t componentNameLength = 128;
    const int32_t componentUuidLength = 128;
    const uint32_t compVersionNum = 10;
    const uint32_t specVersionNum = 1;
    const uint8_t uuidValue = 127;
    char compName[componentNameLength];
    OMX_VERSIONTYPE compVersion;
    OMX_VERSIONTYPE specVersion;
    unsigned char compUuid[componentUuidLength];
    int32_t ret = g_component->GetComponentVersion(g_component, compName, &compVersion, &specVersion,
        compUuid, componentUuidLength);
    ASSERT_EQ(ret, HDF_SUCCESS);
    ASSERT_EQ(compVersion.nVersion, compVersionNum);
    ASSERT_EQ(specVersion.nVersion, specVersionNum);
    ASSERT_EQ(compUuid[0], uuidValue);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_005, TestSize.Level1)
{
    const uint32_t param = 101;
    const int8_t dataLen = 10;
    OMX_COMMANDTYPE cmd = OMX_CommandStateSet;
    int8_t cmdData[dataLen];
    for (int8_t i = 0; i < dataLen; i++) {
        cmdData[i] = i;
    }
    int32_t ret = g_component->SendCommand(g_component, cmd, param, cmdData, (uint32_t)dataLen);
    ASSERT_EQ(ret, HDF_SUCCESS);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_006, TestSize.Level1)
{
    const uint32_t paramLen = 5;
    const int8_t paramValue = 3;
    int8_t param[paramLen];
    param[1] = 1;
    int32_t ret = g_component->GetParameter(g_component, OMX_IndexParamAudioInit, param, paramLen);
    ASSERT_EQ(param[0], paramValue);
    ASSERT_EQ(ret, HDF_SUCCESS);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_007, TestSize.Level1)
{
    const uint32_t paramLen = 5;
    int8_t param[paramLen];
    param[1] = 1;
    int32_t ret = g_component->SetParameter(g_component, OMX_IndexParamAudioInit, param, paramLen);
    ASSERT_EQ(ret, HDF_SUCCESS);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_008, TestSize.Level1)
{
    const uint32_t configLen = 5;
    const int8_t configValue = 3;
    int8_t config[configLen];
    config[1] = 1;
    int32_t ret = g_component->GetConfig(g_component, OMX_IndexParamAudioInit, config, configLen);
    ASSERT_EQ(config[0], configValue);
    ASSERT_EQ(ret, HDF_SUCCESS);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_009, TestSize.Level1)
{
    const uint32_t configLen = 5;
    int8_t config[configLen];
    config[1] = 1;
    int32_t ret = g_component->SetConfig(g_component, OMX_IndexParamAudioInit, config, configLen);
    ASSERT_EQ(ret, HDF_SUCCESS);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_010, TestSize.Level1)
{
    OMX_INDEXTYPE indexType;
    int32_t ret = g_component->GetExtensionIndex(g_component, "prarmName", (uint32_t *)&indexType);
    ASSERT_EQ(indexType, OMX_IndexParamNumAvailableStreams);
    ASSERT_EQ(ret, HDF_SUCCESS);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_011, TestSize.Level1)
{
    OMX_STATETYPE state;
    int32_t ret = g_component->GetState(g_component, &state);
    ASSERT_EQ(state, OMX_StateLoaded);
    ASSERT_EQ(ret, HDF_SUCCESS);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_012, TestSize.Level1)
{
    const uint32_t portNum = 11;
    const int32_t tunneledComp = 1002;
    const uint32_t tunneledPort = 101;
    const uint32_t tunnelFlags = 1200;
    OMX_TUNNELSETUPTYPE tunnelSetup;
    tunnelSetup.eSupplier = OMX_BufferSupplyInput;

    int32_t ret = g_component->ComponentTunnelRequest(g_component, portNum, tunneledComp, tunneledPort, &tunnelSetup);
    ASSERT_EQ(tunnelSetup.nTunnelFlags, tunnelFlags);
    ASSERT_EQ(ret, HDF_SUCCESS);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_013, TestSize.Level1)
{
    const uint32_t portIndex = 101;
    const uint32_t bufferId = 11;
    struct OmxCodecBuffer buffer;
    FillDataOmxCodecBuffer(&buffer);
    int32_t ret = g_component->UseBuffer(g_component, portIndex, &buffer);
    ASSERT_EQ(buffer.bufferId, bufferId);
    ASSERT_EQ(ret, HDF_SUCCESS);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_014, TestSize.Level1)
{
    const uint32_t portIndex = 101;
    const uint32_t bufferId = 11;
    struct OmxCodecBuffer buffer;
    FillDataOmxCodecBuffer(&buffer);
    int32_t ret = g_component->AllocateBuffer(g_component, &buffer, portIndex);
    ASSERT_EQ(buffer.bufferId, bufferId);
    ASSERT_EQ(ret, HDF_SUCCESS);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_015, TestSize.Level1)
{
    const uint32_t portIndex = 101;
    struct OmxCodecBuffer buffer;
    FillDataOmxCodecBuffer(&buffer);
    int32_t ret = g_component->FreeBuffer(g_component, portIndex, &buffer);
    ASSERT_EQ(ret, HDF_SUCCESS);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_016, TestSize.Level1)
{
    struct OmxCodecBuffer buffer;
    FillDataOmxCodecBuffer(&buffer);
    int32_t ret = g_component->EmptyThisBuffer(g_component, &buffer);
    ASSERT_EQ(ret, HDF_SUCCESS);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_017, TestSize.Level1)
{
    struct OmxCodecBuffer buffer;
    FillDataOmxCodecBuffer(&buffer);
    int32_t ret = g_component->FillThisBuffer(g_component, &buffer);
    ASSERT_EQ(ret, HDF_SUCCESS);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_018, TestSize.Level1)
{
    const int32_t appDataLen = 10;
    CodecCallbackType* callback = CodecCallbackTypeStubGetInstance();
    int8_t appData[appDataLen];
    for (int8_t i = 0; i < appDataLen; i++) {
        appData[i] = i;
    }
    int32_t ret = g_component->SetCallbacks(g_component, callback, appData, (uint32_t)appDataLen);
    ASSERT_EQ(ret, HDF_SUCCESS);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_019, TestSize.Level1)
{
    int32_t ret = g_component->ComponentDeInit(g_component);
    ASSERT_EQ(ret, HDF_SUCCESS);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_020, TestSize.Level1)
{
    const int32_t eglImgDataLen = 10;
    const uint32_t portIndex = 101;
    const uint32_t bufferId = 11;
    struct OmxCodecBuffer buffer;
    FillDataOmxCodecBuffer(&buffer);
    int8_t eglImg[eglImgDataLen];
    for (int8_t i = 0; i < eglImgDataLen; i++) {
        eglImg[i] = i;
    }
    int32_t ret = g_component->UseEglImage(g_component, &buffer, portIndex, eglImg, (uint32_t)eglImgDataLen);
    ASSERT_EQ(buffer.bufferId, bufferId);
    ASSERT_EQ(ret, HDF_SUCCESS);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_021, TestSize.Level1)
{
    const uint32_t roleLen = 3;
    const uint32_t roleIndex = 102;
    uint8_t role[roleLen];
    int32_t ret = g_component->ComponentRoleEnum(g_component, role, roleLen, roleIndex);
    int32_t index = 0;
    ASSERT_EQ(role[index], index + 1);
    index++;
    ASSERT_EQ(role[index], index + 1);
    index++;
    ASSERT_EQ(role[index], index + 1);
    ASSERT_EQ(ret, HDF_SUCCESS);
}

HWTEST_F(CodecHdiOmxTest, HdfCodecHdiOmxTest_022, TestSize.Level1)
{
    int32_t ret = g_manager->DestoryComponent(g_component);
    ASSERT_EQ(ret, HDF_SUCCESS);
    CodecComponentManagerRelease();
}
}
