/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "test_play_file_h265.h"

#include "codec_interface.h"
#include "securec.h"

#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

void PlayManager::Display(CODEC_HANDLETYPE decoderHdl)
{
    OutputInfo outInfo = {};
    CodecBufferInfo outBuf = {};
    outInfo.bufferCnt = 1;
    outInfo.buffers = &outBuf;
    constexpr uint32_t timeoutMs = 100; // timeout:100ms
    int32_t ret = CodecDequeueOutput(decoderHdl, timeoutMs, nullptr, &outInfo);
    if (ret == CODEC_ERR_FRAME_BUF_EMPTY) {
        DEMO_LOG("CodecDequeueOutput deque empty.(ret=%d)", ret);
        return;
    } else if (ret != 0) {
        DEMO_LOG("CodecDequeueOutput error.(ret=%d)", ret);
        return;
    }

    ret = Write2VideoDevice(outInfo);
    if (ret != 0) {
        DEMO_LOG("Write video device failed.");
    }

    frameCnt_++;
    ret = CodecQueueOutput(decoderHdl, &outInfo, timeoutMs, -1);
    if (ret != 0) {
        DEMO_LOG("CodecQueueOutput failed.(ret=%d)", ret);
        return;
    }
}

PlayManager::PlayManager()
{
    InitVideoOutput();
}

PlayManager::~PlayManager()
{
    DeinitVideoOutput();
}

int32_t PlayManager::InitVideoOutput()
{
    int32_t ret = HalPlayerSysInit();
    if (ret != 0) {
        DEMO_LOG("hal system init failed.");
        return DEMO_ERR;
    }

    ret = HalPlayerVoInit(&voHdl_);
    if (ret != 0) {
        DEMO_LOG("video output init failed.");
        return DEMO_ERR;
    }

    ret = HalStartVideoOutput(voHdl_);
    if (ret != 0) {
        DEMO_LOG("video output start failed.");
        return DEMO_ERR;
    }

    return DEMO_OK;
}

int32_t PlayManager::DeinitVideoOutput()
{
    int32_t ret = HalStopVideoOutput(voHdl_);
    if (ret != 0) {
        DEMO_LOG("video output start failed.");
        return DEMO_ERR;
    }

    HalPlayerVoDeinit(voHdl_);

    return DEMO_OK;
}

int32_t PlayManager::Write2VideoDevice(OutputInfo &outputInfo)
{
    int32_t x = 200;
    int32_t y = 200;
    int32_t w = 960;
    int32_t h = 540;

    HalVideoOutputAttr voAttr = {x, y, w, h, 0};
    int32_t ret = HalConfigVideoOutput(voHdl_, voAttr);
    if (ret != 0) {
        DEMO_LOG("HalConfigVideoOutput failed.");
        return DEMO_ERR;
    }

    ret = HalWriteVo(voHdl_, outputInfo.vendorPrivate);
    if (ret != 0) {
        DEMO_LOG("HalWriteVo failed.");
        return DEMO_ERR;
    }

    return DEMO_OK;
}

DecodeManager::DecodeManager()
{
    DecoderCreate();
}

DecodeManager::~DecodeManager()
{
    DecoderDestroy();
}

int32_t DecodeManager::DecoderCreate()
{
    int index = 0;
    Param param[PARAM_MAX_NUM];
    AvCodecMime mime = MEDIA_MIMETYPE_VIDEO_HEVC;
    param[index].key = KEY_MIMETYPE;
    param[index].val = (void *)&mime;
    param[index].size = sizeof(AvCodecMime);
    index++;
    uint32_t width = 1920;
    param[index].key = KEY_WIDTH;
    param[index].val = (void *)&width;
    param[index].size = sizeof(uint32_t);
    index++;
    uint32_t height = 1080;
    param[index].key = KEY_HEIGHT;
    param[index].val = (void *)&height;
    param[index].size = sizeof(uint32_t);
    index++;
    uint32_t bufSize = 0;
    param[index].key = KEY_BUFFERSIZE;
    param[index].val = (void *)&bufSize;
    param[index].size = sizeof(uint32_t);
    index++;
    CodecType domain = VIDEO_DECODER;
    param[index].key = KEY_CODEC_TYPE;
    param[index].val = (void *)&domain;
    param[index].size = sizeof(CodecType);
    index++;

    int32_t ret = CodecInit();
    if (ret != 0) {
        DEMO_LOG("Codec init failed.(ret=%d)", ret);
        return DEMO_ERR;
    }

    ret = CodecCreate("codec.avc.soft.decoder", param, index, &decoderHdl_);
    if (ret != 0) {
        DEMO_LOG("Codec create failed.(ret=%d)", ret);
        return DEMO_ERR;
    }

    ret = CodecStart(decoderHdl_);
    if (ret != 0) {
        DEMO_LOG("Codec start failed.(ret=%d)", ret);
        return DEMO_ERR;
    }

    return DEMO_OK;
}

void DecodeManager::DecoderDestroy()
{
    CodecStop(decoderHdl_);
    CodecDestroy(decoderHdl_);
}

int32_t DecodeManager::DecodePack(uint8_t *addr, uint32_t len, uint64_t timeStampUs)
{
    InputInfo inputData = {};
    CodecBufferInfo inBufInfo = {};
    uint32_t timeoutMs = 100; // 100ms timeout
    int32_t ret = CodecDequeInput(decoderHdl_, timeoutMs, &inputData);
    if (ret != 0) {
        DEMO_LOG("CodecDequeInput error.(ret=%d)", ret);
        return DEMO_ERR;
    }

    inBufInfo.addr = addr;
    inBufInfo.length = len;

    inputData.bufferCnt = 1;
    inputData.buffers = &inBufInfo;
    inputData.pts = timeStampUs;
    inputData.flag = 0;
    while ((ret = CodecQueueInput(decoderHdl_, &inputData, timeoutMs)) == CODEC_ERR_STREAM_BUF_FULL) {
        this_thread::sleep_for(chrono::milliseconds(timeoutMs));
    }
    if (ret != 0) {
        DEMO_LOG("CodecQueueInput error.(ret=%d)", ret);
        return DEMO_ERR;
    }
    frameCnt_++;
    return DEMO_OK;
}

static int32_t ReadH265File(ifstream &file, char *buf, int32_t len)
{
    file.read(buf, len);
    if (file) {
        return len;
    } else {
        return file.gcount();
    }
}

static int32_t GetNal(const uint8_t *bsData, int32_t dataSize, int32_t *startCodeSize, int32_t *nalSize)
{
    int32_t curNalOffset = 0;
    int32_t nextNalOffset = 0;
    int32_t offset;
    uint8_t byte0 = 0;
    uint8_t byte1 = 1;
    uint8_t offset1 = 1;
    uint8_t offset2 = 2;
    uint8_t offset3 = 3;

    for (offset = 0; offset < dataSize; offset++) {
        if ((bsData[offset] == byte0) && (bsData[offset + offset1] == byte0) && (bsData[offset + offset2] == byte1)) {
            *startCodeSize = 3; // h265 start frame length: 3->001
            curNalOffset = offset;
            break;
        } else if ((bsData[offset] == byte0) && (bsData[offset + offset1] == byte0) &&
                   (bsData[offset + offset2] == byte0) && (bsData[offset + offset3] == byte1)) {
            *startCodeSize = 4; // h265 start frame length: 4->0001
            curNalOffset = offset;
            break;
        }
    }

    if (offset == dataSize) {
        return -1;
    }

    for (offset = curNalOffset + offset3; offset < dataSize; offset++) {
        if ((bsData[offset] == byte0) && (bsData[offset + offset1] == byte0) && (bsData[offset + offset2] == byte1)) {
            nextNalOffset = offset;
            break;
        } else if ((bsData[offset] == byte0) && (bsData[offset + offset1] == byte0) &&
                   (bsData[offset + offset2] == byte0) && (bsData[offset + offset3] == 0x01)) {
            nextNalOffset = offset;
            break;
        }
    }

    if (offset == dataSize) {
        *nalSize = dataSize - curNalOffset;
    } else {
        *nalSize = nextNalOffset - curNalOffset;
    }

    return curNalOffset;
}

static void DisplayThrd(PlayManager *playMng, CODEC_HANDLETYPE decoderHdl, bool *stop, uint32_t timeStep)
{
    while (!(*stop)) {
        this_thread::sleep_for(chrono::microseconds(timeStep));
        playMng->Display(decoderHdl);
    }
    DEMO_LOG("play end.");
}

int main()
{
    DEMO_LOG("Demo begin");

    PlayManager playMng;
    DEMO_LOG("Init video output succeed.");

    DecodeManager decodeMng;
    CODEC_HANDLETYPE decoderHdl = decodeMng.GetHdl();
    DEMO_LOG("Init decoder succeed.");

    cout << "Type in file path:";
    string fileName;
    getline(cin, fileName);
    if (fileName.empty()) {
        fileName.assign("/tmp/1080P.h265");
    }
    ifstream file(fileName, ios::binary | ios::ate);
    if (!file) {
        DEMO_LOG("Openfile %s failed.", fileName.c_str());
        return 0;
    }
    int32_t fileSize = file.tellg();
    file.seekg(0, file.beg);
    DEMO_LOG("Open file succeed.(size=%d)", fileSize);

    constexpr uint32_t bufSize = 1024 * 1024;
    uint8_t *buf = (uint8_t *)malloc(bufSize);
    if (buf == nullptr) {
        DEMO_LOG("Memory not enough.");
        return 0;
    }

    constexpr uint32_t timeStep = 33333; // 33333us, 30fps
    bool displayStop = false;
    thread displayThrd(DisplayThrd, &playMng, decoderHdl, &displayStop, timeStep);
    uint64_t timeStamp = 0;
    int32_t bufLen = 0;
    while (file) {
        bufLen += ReadH265File(file, static_cast<char *>(buf + bufLen), bufSize - bufLen);

        int32_t frameSize;
        int32_t codeSize;
        int32_t offset = 0;
        offset = GetNal(buf, bufLen, &codeSize, &frameSize);
        if (offset < 0) {
            DEMO_LOG("Get nal frame failed.");
            break;
        }
        int32_t ret = decodeMng.DecodePack(buf + offset, frameSize, timeStamp);
        timeStamp += timeStep;
        int result = memmove_s(buf, bufSize, buf + offset + frameSize, bufLen - offset - frameSize);
        if (result != 0) {
            DEMO_LOG("memmove_s failed.");
            break;
        }
        bufLen = bufLen - offset - frameSize;
        if (ret != DEMO_OK) {
            DEMO_LOG("Decode one frame failed.");
            continue;
        }
    }
    while (playMng.GetCnt() != decodeMng.GetCnt()) {
        this_thread::sleep_for(chrono::seconds(1)); // sleep 1s
    }
    displayStop = true;
    displayThrd.join();
    if (buf != nullptr) {
        free(buf);
    }

    DEMO_LOG("Demo end");
}
