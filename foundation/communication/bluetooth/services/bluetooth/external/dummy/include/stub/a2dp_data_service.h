/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef A2DP_DATA_SERVICE_H
#define A2DP_DATA_SERVICE_H

#include <chrono>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <string>
#include <time.h>
#include <cstring>
#include <sys/mman.h>

namespace stub {
static const int PCM_HEADER_SIZE = 44;

enum PcmChannelMode : uint8_t {
    PCM_CHANNEL_MODE_SINGLE = 0x00, /* single channel */
    PCM_CHANNEL_MODE_DOUBLE = 0x01, /* couple channel */
    PCM_CHANNEL_MODE_MAX = 0x02,    /* Max count      */
};

enum PcmBitsPerSample : uint8_t {
    PCM_SAMPLE_BITS_16 = 0x0,  /* BitsPerSample:16bit */
    PCM_SAMPLE_BITS_24 = 0x1,  /* BitsPerSample:16bit */
    PCM_SAMPLE_BITS_32 = 0x2,  /* BitsPerSample:16bit */
    PCM_SAMPLE_BITS_MAX = 0x3, /* Max count           */
};

enum PcmCodecSampleRate : uint8_t {
    PCM_SAMPLE_RATE_44100 = 0x0, /* SampleRate 44.1 KHz */
    PCM_SAMPLE_RATE_48000 = 0x1, /* SampleRate 48   KHz */
    PCM_SAMPLE_RATE_MAX = 0x2,   /* Max count           */
};

enum SinkCodecType : uint8_t {
    Sink_CODEC_TYPE_SBC = 0x00,
    Sink_CODEC_TYPE_AAC = 0x01,
    Sink_CODEC_TYPE_MAX = 0x02,
};

enum ErrorType : uint16_t {
    ERROR_TYPE_NONE = 0x00,
    ERROR_TYPE_TIMEOUT = 0x01,
    ERROR_TYPE_EXTRA_GETDATA = 0x02,
    ERROR_TYPE_DATASIZE_ERROR = 0x04,
    ERROR_TYPE_COMPARE_DATA_ERROR = 0x08,
};

class A2dpServiceObserver {
public:
    virtual ~A2dpServiceObserver() = default;
    virtual void OnSourceSendPcmStreamError(uint16_t errorType, uint32_t data)
    {}
    virtual void OnSourceSendComplete(uint8_t result)
    {}
    virtual void OnSinkGetPcmStreamError(uint16_t errorType, uint32_t data)
    {}
    virtual void OnSinkGetComplete(uint8_t result)
    {}
};

class A2dpService {
public:
    static A2dpService *GetInstance();

    char *getPCMStream(uint32_t &dataSize, bool &result)
    {
        return nullptr;
    }
    void setPCMStream(char *dataStream, uint32_t dataSize)
    {}
    void RegisterObserver(A2dpServiceObserver *observer)
    {}
    void DeregisterObserver(A2dpServiceObserver *observer)
    {}
    bool SourceInitPcmStream(uint8_t sampleRate, uint8_t channelMode, uint8_t bitsPerSample = PCM_SAMPLE_BITS_16)
    {
        return false;
    }
    void SourceStartSendPcmStream(uint32_t sendMSTime, bool isSbc, bool fromPCMBegin = true)
    {}
    bool SinkInitPcmStream(uint8_t sampleRate, uint8_t channelMode, uint8_t codecType,
        uint8_t bitsPerSample = PCM_SAMPLE_BITS_16, bool writeFileNoCheck = false)
    {
        return false;
    }
    void SinkStartGetPcmStream(uint32_t sendMSTime, bool isSbc, bool fromPCMBegin = true)
    {}
    int getSourceMemFd()
    {
        return -1;
    }
    int getSinkMemFd()
    {
        return -1;
    }

private:
    A2dpService() = default;
    ~A2dpService() = default;

    A2dpService(const A2dpService &) = delete;
    A2dpService &operator=(const A2dpService &) = delete;
};
}  // namespace stub
#endif  // A2DP_SERVICE_H