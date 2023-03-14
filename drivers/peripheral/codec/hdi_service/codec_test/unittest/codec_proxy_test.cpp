/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "icodec.h"
#include <gtest/gtest.h>
#include <osal_mem.h>
#include <unistd.h>
#include "codec_callback_stub.h"

using namespace std;
using namespace testing::ext;

namespace {
constexpr const char *TEST_SERVICE_NAME = "codec_hdi_service";
ICodecCallback *g_callback = nullptr;

class CodecObjCTest : public testing::Test {
public:
    static void SetUpTestCase(){}
    static void TearDownTestCase(){}
    void SetUp(){}
    void TearDown(){}
};

HWTEST_F(CodecObjCTest, CodecObjCTest_001, TestSize.Level0)
{
    struct ICodec *codecObj = HdiCodecGet(TEST_SERVICE_NAME);
    ASSERT_TRUE(codecObj != nullptr);

    int32_t ec = codecObj->CodecInit(codecObj);

    ASSERT_EQ(ec, HDF_SUCCESS);

    HdiCodecRelease(codecObj);
}

HWTEST_F(CodecObjCTest, CodecObjCTest_002, TestSize.Level0)
{
    struct ICodec *codecObj = HdiCodecGet(TEST_SERVICE_NAME);
    ASSERT_TRUE(codecObj != nullptr);

    int32_t ec = codecObj->CodecDeinit(codecObj);

    ASSERT_EQ(ec, HDF_SUCCESS);

    HdiCodecRelease(codecObj);
}

HWTEST_F(CodecObjCTest, CodecObjCTest_003, TestSize.Level0)
{
    struct ICodec *codecObj = HdiCodecGet(TEST_SERVICE_NAME);
    ASSERT_TRUE(codecObj != nullptr);

    uint32_t index = 2;
    CodecCapbility cap;

    int32_t ec = codecObj->CodecEnumerateCapbility(codecObj, index, &cap);

    ASSERT_EQ(ec, HDF_SUCCESS);

    HdiCodecRelease(codecObj);
}

HWTEST_F(CodecObjCTest, CodecObjCTest_004, TestSize.Level0)
{
    struct ICodec *codecObj = HdiCodecGet(TEST_SERVICE_NAME);
    ASSERT_TRUE(codecObj != nullptr);

    uint32_t flags = 1;
    AvCodecMime mime = MEDIA_MIMETYPE_VIDEO_AVC;
    CodecType type = AUDIO_DECODER;
    CodecCapbility cap;

    int32_t ec = codecObj->CodecGetCapbility(codecObj, mime, type, flags, &cap);

    ASSERT_EQ(ec, HDF_SUCCESS);
    ASSERT_EQ(cap.mime, MEDIA_MIMETYPE_VIDEO_AVC);
    ASSERT_EQ(cap.type, AUDIO_DECODER);
    ASSERT_EQ(cap.whAlignment.widthAlignment, 10);
    ASSERT_EQ(cap.supportLevels.actualLen, 3);

    HdiCodecRelease(codecObj);
}

HWTEST_F(CodecObjCTest, CodecObjCTest_005, TestSize.Level0)
{
    struct ICodec *codecObj = HdiCodecGet(TEST_SERVICE_NAME);
    ASSERT_TRUE(codecObj != nullptr);
    int num = 10;
    const char* name = "codec.avc.hardware.decoder";
    Param params;
    params.key = KEY_CODEC_TYPE;
    params.val = (void *)&num;
    params.size = 20;
    int len = 3;
    uint32_t handle = 5;

    int32_t ec = codecObj->CodecCreate(codecObj, name, &params, len, (CODEC_HANDLETYPE *)&handle);

    ASSERT_EQ(ec, HDF_SUCCESS);
    ASSERT_EQ(params.key, KEY_CODEC_TYPE);
    ASSERT_EQ(params.size, 20);
    ASSERT_EQ(len, 3);

    HdiCodecRelease(codecObj);
}

HWTEST_F(CodecObjCTest, CodecObjCTest_006, TestSize.Level0)
{
    struct ICodec *codecObj = HdiCodecGet(TEST_SERVICE_NAME);
    ASSERT_TRUE(codecObj != nullptr);

    uint32_t handle = 5;

    int32_t ec = codecObj->CodecDestroy(codecObj, (CODEC_HANDLETYPE *)&handle);

    ASSERT_EQ(ec, HDF_SUCCESS);

    HdiCodecRelease(codecObj);
}

HWTEST_F(CodecObjCTest, CodecObjCTest_007, TestSize.Level0)
{
    struct ICodec *codecObj = HdiCodecGet(TEST_SERVICE_NAME);
    ASSERT_TRUE(codecObj != nullptr);

    uint32_t handle = 5;
    DirectionType type = OUTPUT_TYPE;
    BufferMode mode = EXTERNAL;

    int32_t ec = codecObj->CodecSetPortMode(codecObj, (CODEC_HANDLETYPE *)&handle, type, mode);

    ASSERT_EQ(ec, HDF_SUCCESS);
    ASSERT_EQ(type, OUTPUT_TYPE);
    ASSERT_EQ(mode, EXTERNAL);

    HdiCodecRelease(codecObj);
}

HWTEST_F(CodecObjCTest, CodecObjCTest_008, TestSize.Level0)
{
    struct ICodec *codecObj = HdiCodecGet(TEST_SERVICE_NAME);
    ASSERT_TRUE(codecObj != nullptr);

    uint32_t handle = 5;
    Param params;
    params.key = KEY_CODEC_TYPE;
    params.val = 0;
    params.size = 20;

    int paramCnt = 3;

    int32_t ec = codecObj->CodecSetParameter(codecObj, (CODEC_HANDLETYPE *)&handle, &params, paramCnt);

    ASSERT_EQ(ec, HDF_SUCCESS);
    ASSERT_EQ(params.key, KEY_CODEC_TYPE);
    ASSERT_EQ(params.size, 20);
    ASSERT_EQ(paramCnt, 3);

    HdiCodecRelease(codecObj);
}

HWTEST_F(CodecObjCTest, CodecObjCTest_009, TestSize.Level0)
{
    struct ICodec *codecObj = HdiCodecGet(TEST_SERVICE_NAME);
    ASSERT_TRUE(codecObj != nullptr);

    uint32_t handle = 5;
    int paramCnt = 3;
    Param *params;
    params = (Param *)OsalMemAlloc(sizeof(Param)*paramCnt);
    ASSERT_TRUE(params != nullptr);
    params->key = KEY_CODEC_TYPE;
    params->val = 0;
    params->size = 20;

    int32_t ec = codecObj->CodecGetParameter(codecObj, (CODEC_HANDLETYPE *)&handle, params, paramCnt);

    ASSERT_EQ(ec, HDF_SUCCESS);
    OsalMemFree(params);
    HdiCodecRelease(codecObj);
}

HWTEST_F(CodecObjCTest, CodecObjCTest_010, TestSize.Level0)
{
    struct ICodec *codecObj = HdiCodecGet(TEST_SERVICE_NAME);
    ASSERT_TRUE(codecObj != nullptr);

    uint32_t handle = 5;

    int32_t  ec = codecObj->CodecStart(codecObj, (CODEC_HANDLETYPE *)&handle);

    ASSERT_EQ(ec, HDF_SUCCESS);

    HdiCodecRelease(codecObj);
}

HWTEST_F(CodecObjCTest, CodecObjCTest_011, TestSize.Level0)
{
    struct ICodec *codecObj = HdiCodecGet(TEST_SERVICE_NAME);
    ASSERT_TRUE(codecObj != nullptr);

    uint32_t handle = 5;

    int32_t ec = codecObj->CodecStop(codecObj, (CODEC_HANDLETYPE *)&handle);

    ASSERT_EQ(ec, HDF_SUCCESS);

    HdiCodecRelease(codecObj);
}

HWTEST_F(CodecObjCTest, CodecObjCTest_012, TestSize.Level0)
{
    struct ICodec *codecObj = HdiCodecGet(TEST_SERVICE_NAME);
    ASSERT_TRUE(codecObj != nullptr);

    uint32_t handle = 5;
    DirectionType directType = OUTPUT_TYPE;

    int32_t ec = codecObj->CodecFlush(codecObj, (CODEC_HANDLETYPE *)&handle, directType);

    ASSERT_EQ(ec, HDF_SUCCESS);
    ASSERT_EQ(directType, OUTPUT_TYPE);

    HdiCodecRelease(codecObj);
}

HWTEST_F(CodecObjCTest, CodecObjCTest_013, TestSize.Level0)
{
    struct ICodec *codecObj = HdiCodecGet(TEST_SERVICE_NAME);
    ASSERT_TRUE(codecObj != nullptr);

    uint32_t handle = 5;
    InputInfo inputData = {0};
    inputData.bufferCnt = 1;
    inputData.buffers = (CodecBufferInfo *)OsalMemAlloc(sizeof(CodecBufferInfo) * inputData.bufferCnt);
    inputData.buffers->type = BUFFER_TYPE_FD;
    inputData.buffers->fd = 2;
    inputData.buffers->offset = 3;
    inputData.buffers->length = 4;
    inputData.buffers->size = 5;
    inputData.pts = 0;
    inputData.flag = STREAM_FLAG_CODEC_SPECIFIC_INF;
    uint32_t timeoutMs = 10;

    int32_t ec = codecObj->CodecQueueInput(codecObj, (CODEC_HANDLETYPE *)&handle, &inputData, timeoutMs);
    ASSERT_EQ(ec, HDF_SUCCESS);
    ASSERT_EQ(inputData.pts, 0);
    ASSERT_EQ(inputData.flag, STREAM_FLAG_CODEC_SPECIFIC_INF);
    ASSERT_EQ(ec, HDF_SUCCESS);
    OsalMemFree(inputData.buffers);
    HdiCodecRelease(codecObj);
}

HWTEST_F(CodecObjCTest, CodecObjCTest_014, TestSize.Level0)
{
    struct ICodec *codecObj = HdiCodecGet(TEST_SERVICE_NAME);
    ASSERT_TRUE(codecObj != nullptr);

    uint32_t handle = 5;
    uint32_t timeoutMs = 10;
    InputInfo inputData = {0};
    inputData.bufferCnt = 1;
    inputData.buffers = (CodecBufferInfo *)OsalMemAlloc(sizeof(CodecBufferInfo) * (inputData.bufferCnt));
    ASSERT_TRUE(inputData.buffers != nullptr);
    int32_t ec = codecObj->CodecDequeInput(codecObj, (CODEC_HANDLETYPE *)&handle, timeoutMs, &inputData);

    ASSERT_EQ(ec, HDF_SUCCESS);
    ASSERT_EQ((int)timeoutMs, 10);
    OsalMemFree(inputData.buffers);
    HdiCodecRelease(codecObj);
}

HWTEST_F(CodecObjCTest, CodecObjCTest_015, TestSize.Level0)
{
    struct ICodec *codecObj = HdiCodecGet(TEST_SERVICE_NAME);
    ASSERT_TRUE(codecObj != nullptr);

    uint32_t handle = 5;
    OutputInfo outInfo = {0};
    outInfo.bufferCnt = 1;
    outInfo.buffers = (CodecBufferInfo *)OsalMemAlloc(sizeof(CodecBufferInfo) * (outInfo.bufferCnt));
    outInfo.buffers->type = BUFFER_TYPE_FD;
    outInfo.buffers->fd = 1;
    outInfo.buffers->offset = 2;
    outInfo.buffers->length = 3;
    outInfo.buffers->size = 4;
    outInfo.timeStamp = 0;
    outInfo.sequence = 1;
    outInfo.flag = STREAM_FLAG_CODEC_SPECIFIC_INF;
    outInfo.type = AUDIO_DECODER;
    outInfo.vendorPrivate = 0;
    uint32_t timeoutMs = 10;
    int releaseFenceFd = 3;

    int32_t ec = codecObj->CodecQueueOutput(codecObj, (CODEC_HANDLETYPE *)&handle, &outInfo, timeoutMs, releaseFenceFd);
    
    ASSERT_EQ(ec, HDF_SUCCESS);
    ASSERT_EQ((int)timeoutMs, 10);
    ASSERT_EQ(releaseFenceFd, 3);
    OsalMemFree(outInfo.buffers);
    HdiCodecRelease(codecObj);
}

HWTEST_F(CodecObjCTest, CodecObjCTest_016, TestSize.Level0)
{
    struct ICodec *codecObj = HdiCodecGet(TEST_SERVICE_NAME);
    ASSERT_TRUE(codecObj != nullptr);

    uint32_t handle = 5;
    uint32_t timeoutMs = 10;
    int acquireFd;
    OutputInfo outInfo = {0};
    outInfo.bufferCnt = 1;
    outInfo.buffers = (CodecBufferInfo *)OsalMemAlloc(sizeof(CodecBufferInfo) * (outInfo.bufferCnt));
    ASSERT_TRUE(outInfo.buffers != nullptr);
    int32_t ec = codecObj->CodecDequeueOutput(codecObj, (CODEC_HANDLETYPE *)&handle, timeoutMs, &acquireFd, &outInfo);

    ASSERT_EQ(ec, HDF_SUCCESS);
    ASSERT_EQ((int)timeoutMs, 10);
    OsalMemFree(outInfo.buffers);
    HdiCodecRelease(codecObj);
}

HWTEST_F(CodecObjCTest, CodecObjCTest_017, TestSize.Level0)
{
    struct ICodec *codecObj = HdiCodecGet(TEST_SERVICE_NAME);
    ASSERT_TRUE(codecObj != nullptr);

    uint32_t handle = 5;
    UINTPTR instance = 0;
    g_callback = CodecCallbackStubObtain();
    ASSERT_TRUE(g_callback != nullptr);

    int32_t ec = codecObj->CodecSetCallback(codecObj, (CODEC_HANDLETYPE *)&handle, g_callback, instance);

    ASSERT_EQ(ec, HDF_SUCCESS);
    HdiCodecRelease(codecObj);
}
}