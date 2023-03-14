/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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
#ifdef OHOS_LITE
#define HST_LOG_TAG "LiteStreamPlayer"

#define LOG_TAG "main"

#include <chrono>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "scene/lite/hiplayer.h"
#include "securec.h"
#include "foundation/log.h"
#include "foundation/multimedia/utils/lite/interfaces/kits/data_stream.h"
#include "foundation/osal/utils/util.h"
#include "foundation/osal/thread/task.h"

using namespace OHOS::Media;

#define INPUT_BUFFER_SIZE           10 * 1024
#define INPUT_BUFFER_ITEM           3

static uint32_t readPos = 0;
std::vector<uint32_t> testData;
uint32_t testDataSize = 0;
std::shared_ptr<DataStream> stream = nullptr;
std::shared_ptr<OSAL::Task> task = nullptr;

enum SourceFlag {
    SOURCE_EOS,
};

uint8_t *GetDataFromSource(int8_t *flag, uint32_t *getDataSize)
{
    uint32_t inputSize = INPUT_BUFFER_SIZE;
    MEDIA_LOG_I("testDataSize:" PUBLIC_LOG_U32 " ,readPos:" PUBLIC_LOG_U32, testDataSize, readPos);
    uint8_t *outDataPtr = nullptr;
    if (readPos == testDataSize) {
        *flag = SOURCE_EOS;
        *getDataSize = 0;
        return outDataPtr;
    }
    outDataPtr = (uint8_t *)malloc(inputSize);
    (void)memset_s(outDataPtr, INPUT_BUFFER_SIZE, 0, inputSize);
    if (readPos + INPUT_BUFFER_SIZE > testDataSize) {
        inputSize = testDataSize - readPos;
    }
    (void)memcpy_s(outDataPtr, INPUT_BUFFER_SIZE, &testData[0] + readPos / sizeof(uint32_t), inputSize);
    readPos += inputSize;
    MEDIA_LOG_I("readPo:" PUBLIC_LOG_U32, readPos);
    *getDataSize = inputSize;
    return outDataPtr;
}

bool g_playFinished = false;

void DataProcessThread()
{
    MEDIA_LOG_I("DataProcessThread in");
    while (!g_playFinished) {
        int8_t sourceFlag = -1;
        uint32_t realGetSize = 0;
        uint8_t *sourceData = GetDataFromSource(&sourceFlag, &realGetSize);
        std::shared_ptr<DataBuffer> buffer;
        (void) stream->GetEmptyBuffer(buffer);
        if (buffer == nullptr) {
            MEDIA_LOG_E("buffer null error.");
            if (sourceData != nullptr) {
                free(sourceData);
            }
            return;
        } else {
            MEDIA_LOG_I("realGetSize:" PUBLIC_LOG_U32, realGetSize);
            if (sourceData != nullptr) { // get data
                (void) memcpy_s(buffer->GetAddress(), buffer->GetCapacity(), sourceData, realGetSize);
                buffer->SetSize(realGetSize);
                free(sourceData);
            } else { // not get data, must be eos
                buffer->SetSize(0);
                FALSE_LOG(sourceFlag == SOURCE_EOS);
                MEDIA_LOG_I("SourceEos");
                buffer->SetEos(true);
                stream->QueueDataBuffer(buffer);
                break;
            }
            stream->QueueDataBuffer(buffer);
        }
    }
    task->StopAsync();
}

class PlayerCallbackImpl : public PlayerCallback {
    void OnPlaybackComplete() override
    {
        g_playFinished = true;
        MEDIA_LOG_I("OnPlaybackComplete called, g_playFinished is true now.");
    }
    void OnError(int32_t errorType, int32_t errorCode) override
    {
    }
    void OnInfo(int type, int extra) override
    {
    }
    void OnVideoSizeChanged(int width, int height) override
    {
    }
    void OnRewindToComplete() override
    {
    }
};

int ReadDataFromFile(std::string dataPath)
{
    std::string dataFullPath;
    if (OSAL::ConvertFullPath(dataPath, dataFullPath) && !dataFullPath.empty()) {
        dataPath = dataFullPath;
    }
    std::fstream fs(dataPath);
    if (!fs.is_open()) {
        std::cout << "failed to open " << dataPath << '\n';
        return 0;
    }
    std::stringstream ss;
    while (!fs.eof()) {
        std::string s;
        fs >> s;
        ss << s;
    }
    std::string data = ss.str();
    const char* split = ",";
    char* s_input = (char *)data.c_str();
    char* p = strtok(s_input, split);
    while (p != nullptr) {
        uint32_t data;
        (void)sscanf_s(p, "%x", &data);
        testData.push_back(data);
        p=strtok(nullptr, split);
    }
    return testData.size() * 4; // 4
}

int StartLiteStreamPlayer(std::string dataPath)
{
    MEDIA_LOG_I("Use media_lite interface player.");
    g_playFinished = false;
    auto player = OHOS::Media::CreateHiPlayer();
    player->Init();
    auto callback = std::make_shared<PlayerCallbackImpl>();
    player->SetPlayerCallback(callback);
    stream = CreateDataStream(INPUT_BUFFER_SIZE, INPUT_BUFFER_ITEM);
    if (stream == nullptr) {
        MEDIA_LOG_E("Create data stream fail.");
        return -1;
    }
    OHOS::Media::Source source(stream);
    player->SetSource(source);
    player->SetLoop(false);
    testDataSize = ReadDataFromFile(dataPath);
    if (!testDataSize) {
        MEDIA_LOG_E("Get data size fail.");
        return -1;
    }
    MEDIA_LOG_I("testDataSize:" PUBLIC_LOG_U32, testDataSize);
    task = std::make_shared<OSAL::Task>("DataProcessThread");
    task->RegisterHandler(DataProcessThread);
    task->Start();
    player->Prepare();
    player->Play();
    while (!g_playFinished) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 100
        MEDIA_LOG_I("stream player thread running...");
    }
    readPos = 0;
    stream = nullptr;
    return 0;
}
#endif