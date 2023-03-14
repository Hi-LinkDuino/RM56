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
#ifndef TEST_PLAY_FILE_H265_H
#define TEST_PLAY_FILE_H265_H

#include <cstdio>
#include <fstream>
#include <mutex>
#include <queue>

#include "codec_type.h"
#include "hal_display.h"

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#define DEMO_LOG(format, ...) printf("%s[%s:%d]" format "\n", __func__, (__FILENAME__), __LINE__, ##__VA_ARGS__)

constexpr uint32_t PARAM_MAX_NUM = 10;
constexpr int32_t DEMO_OK = 0;
constexpr int32_t DEMO_ERR = -1;

class PlayManager {
public:
    PlayManager();
    ~PlayManager();
    void Display(CODEC_HANDLETYPE decoderHdl);
    bool GetCnt() const
    {
        return frameCnt_;
    }

private:
    std::queue<OutputInfo *> playQ_ = {};
    HalVideoOutputHdl voHdl_ = 0;
    int64_t frameCnt_ = 0;

    int32_t InitVideoOutput();
    int32_t DeinitVideoOutput();
    int32_t Write2VideoDevice(OutputInfo &outputInfo);
};

class DecodeManager {
public:
    DecodeManager();
    ~DecodeManager();
    int32_t DecodePack(uint8_t *addr, uint32_t len, uint64_t timeStampUs);
    CODEC_HANDLETYPE GetHdl() const
    {
        return decoderHdl_;
    }
    bool GetCnt() const
    {
        return frameCnt_;
    }

private:
    CODEC_HANDLETYPE decoderHdl_;
    int64_t frameCnt_ = 0;

    int32_t DecoderCreate();
    void DecoderDestroy();
};

#endif // TEST_PLAY_FILE_H265_H