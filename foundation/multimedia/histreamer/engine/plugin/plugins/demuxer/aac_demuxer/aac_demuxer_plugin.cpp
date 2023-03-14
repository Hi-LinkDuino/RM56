/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#define HST_LOG_TAG "AACDemuxerPlugin"

#include "aac_demuxer_plugin.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <new>
#include <securec.h>
#include "foundation/log.h"
#include "osal/thread/scoped_lock.h"
#include "plugin/common/plugin_buffer.h"
#include "constants.h"
#include "osal/utils/util.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace AacDemuxer {
namespace {
    constexpr uint32_t PROBE_READ_LENGTH = 2;
    constexpr uint32_t GET_INFO_READ_LEN = 7;
    constexpr uint32_t MEDIA_IO_SIZE = 2048;
    constexpr uint32_t MAX_RANK = 100;
    uint32_t usedDataSize_ = 0;
    int samplingRateMap[] = {96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350};
    int IsAACPattern(const uint8_t *data);
    int Sniff(const std::string& name, std::shared_ptr<DataSource> dataSource);
    Status RegisterPlugin(const std::shared_ptr<Register>& reg);
}

AACDemuxerPlugin::AACDemuxerPlugin(std::string name)
    : DemuxerPlugin(std::move(name)),
      ioContext_(),
      fileSize_(0),
      isSeekable_(false),
      inIoBuffer_(nullptr),
      inIoBufferSize_(MEDIA_IO_SIZE),
      ioDataRemainSize_(0)
{
    FALSE_LOG(memset_s(&aacDemuxerRst_, sizeof(aacDemuxerRst_), 0x00, sizeof(AACDemuxerRst)) == 0);
    MEDIA_LOG_I("AACDemuxerPlugin, plugin name: " PUBLIC_LOG_S, pluginName_.c_str());
}

AACDemuxerPlugin::~AACDemuxerPlugin()
{
    MEDIA_LOG_I("~AACDemuxerPlugin");
}

Status AACDemuxerPlugin::SetDataSource(const std::shared_ptr<DataSource>& source)
{
    ioContext_.dataSource = source;
    if (ioContext_.dataSource != nullptr) {
        ioContext_.dataSource->GetSize(fileSize_);
    }
    MEDIA_LOG_I("fileSize_ " PUBLIC_LOG_ZU, fileSize_);
    isSeekable_ = fileSize_ > 0 ? true : false;
    return Status::OK;
}

Status AACDemuxerPlugin::DoReadFromSource(uint32_t readSize)
{
    if (readSize == 0) {
        return Status::OK;
    }
    auto buffer  = std::make_shared<Buffer>();
    auto bufData = buffer->AllocMemory(nullptr, readSize);
    int retryTimes = 0;
    MEDIA_LOG_D("readSize " PUBLIC_LOG_U32 " inIoBufferSize_ " PUBLIC_LOG_D32 "ioDataRemainSize_ "
                PUBLIC_LOG_U32, readSize, inIoBufferSize_, ioDataRemainSize_);
    do {
        if (isSeekable_) {
            auto result = ioContext_.dataSource->ReadAt(ioContext_.offset, buffer, static_cast<size_t>(readSize));
            MEDIA_LOG_D("ioContext_.offset " PUBLIC_LOG_U32, static_cast<uint32_t>(ioContext_.offset));
            if (result != Status::OK) {
                MEDIA_LOG_W("read data from source warning " PUBLIC_LOG_D32, static_cast<int>(result));
                return result;
            }
        } else {
            auto result = ioContext_.dataSource->ReadAt(0, buffer, static_cast<size_t>(readSize));
            if (result != Status::OK) {
                MEDIA_LOG_W("read data from source warning " PUBLIC_LOG_D32, static_cast<int>(result));
                return result;
            }
        }

        MEDIA_LOG_D("bufData->GetSize() " PUBLIC_LOG_ZU, bufData->GetSize());
        if (bufData->GetSize() > 0) {
            if (readSize >= bufData->GetSize()) {
                (void)memcpy_s(inIoBuffer_ + ioDataRemainSize_, readSize,
                    const_cast<uint8_t *>(bufData->GetReadOnlyData()), bufData->GetSize());
            } else {
                MEDIA_LOG_E("Error: readSize < bufData->GetSize()");
                return Status::ERROR_UNKNOWN;
            }
            if (isSeekable_) {
                ioContext_.offset += bufData->GetSize();
            }
            ioDataRemainSize_  += bufData->GetSize();
        }
        if (bufData->GetSize() == 0 && ioDataRemainSize_ == 0 && retryTimes < 200) { // 200
            OSAL::SleepFor(30); // 30
            retryTimes++;
            continue;
        }
        if (retryTimes >= 200) { // 200
            MEDIA_LOG_E("Warning: not end of file, but do not have enough data");
            return Status::ERROR_NOT_ENOUGH_DATA;
        }
        break;
    } while (true);
    return Status::OK;
}

Status AACDemuxerPlugin::GetDataFromSource()
{
    uint32_t ioNeedReadSize = inIoBufferSize_ - ioDataRemainSize_;
    MEDIA_LOG_D("ioDataRemainSize_ " PUBLIC_LOG_U32, " ioNeedReadSize " PUBLIC_LOG_U32, ioDataRemainSize_,
                ioNeedReadSize);
    if (ioDataRemainSize_) {
        // 将剩余数据移动到buffer的起始位置
        auto ret = memmove_s(inIoBuffer_,
                             ioDataRemainSize_,
                             inIoBuffer_ + usedDataSize_,
                             ioDataRemainSize_);
        if (ret != 0) {
            MEDIA_LOG_E("copy buffer error(" PUBLIC_LOG_D32, ret);
            return Status::ERROR_UNKNOWN;
        }
        ret = memset_s(inIoBuffer_ + ioDataRemainSize_, ioNeedReadSize, 0x00, ioNeedReadSize);
        if (ret != 0) {
            MEDIA_LOG_E("memset_s buffer error(" PUBLIC_LOG_D32, ret);
            return Status::ERROR_UNKNOWN;
        }
    }
    if (isSeekable_) {
        if (ioContext_.offset >= fileSize_ && ioDataRemainSize_ == 0) {
            ioContext_.eos = true;
            ioContext_.offset = 0;
            return Status::END_OF_STREAM;
        }
        if (ioContext_.offset + ioNeedReadSize > fileSize_) {
            ioNeedReadSize = fileSize_ - ioContext_.offset; // 在读取文件即将结束时，剩余数据不足，更新读取长度
        }
    }

    return DoReadFromSource(ioNeedReadSize);
}

Status AACDemuxerPlugin::GetMediaInfo(MediaInfo& mediaInfo)
{
    Status retStatus = GetDataFromSource();
    if (retStatus != Status::OK) {
        return retStatus;
    }
    int ret = AudioDemuxerAACPrepare(inIoBuffer_, ioDataRemainSize_, &aacDemuxerRst_);
    if (ret == 0) {
        mediaInfo.tracks.resize(1);
        if (aacDemuxerRst_.frameChannels == 1) {
            mediaInfo.tracks[0].insert({Tag::AUDIO_CHANNEL_LAYOUT, AudioChannelLayout::MONO});
        } else {
            mediaInfo.tracks[0].insert({Tag::AUDIO_CHANNEL_LAYOUT, AudioChannelLayout::STEREO});
        }
        mediaInfo.tracks[0].insert({Tag::AUDIO_SAMPLE_RATE, static_cast<uint32_t>(aacDemuxerRst_.frameSampleRate)});
        mediaInfo.tracks[0].insert({Tag::MEDIA_BITRATE, static_cast<int64_t>(aacDemuxerRst_.frameBitrateKbps)});
        mediaInfo.tracks[0].insert({Tag::AUDIO_CHANNELS, static_cast<uint32_t>(aacDemuxerRst_.frameChannels)});
        mediaInfo.tracks[0].insert({Tag::TRACK_ID, static_cast<uint32_t>(0)});
        mediaInfo.tracks[0].insert({Tag::MIME, std::string(MEDIA_MIME_AUDIO_AAC)});
        mediaInfo.tracks[0].insert({Tag::AUDIO_MPEG_VERSION, static_cast<uint32_t>(aacDemuxerRst_.mpegVersion)});
        mediaInfo.tracks[0].insert({Tag::AUDIO_SAMPLE_FORMAT, AudioSampleFormat::S16});
        mediaInfo.tracks[0].insert({Tag::AUDIO_SAMPLE_PER_FRAME, static_cast<uint32_t>(1024)});   // 1024
        mediaInfo.tracks[0].insert({Tag::AUDIO_AAC_PROFILE, AudioAacProfile::LC});
        mediaInfo.tracks[0].insert({Tag::AUDIO_AAC_STREAM_FORMAT, AudioAacStreamFormat::MP4ADTS});
        return Status::OK;
    } else {
        return Status::ERROR_UNSUPPORTED_FORMAT;
    }
}

Status AACDemuxerPlugin::ReadFrame(Buffer& outBuffer, int32_t timeOutMs)
{
    int status  = -1;
    std::shared_ptr<Memory> aacFrameData;
    Status retStatus = GetDataFromSource();
    if (retStatus != Status::OK) {
        return retStatus;
    }
    status = AudioDemuxerAACProcess(inIoBuffer_, ioDataRemainSize_, &aacDemuxerRst_);

    if (outBuffer.IsEmpty()) {
        aacFrameData = outBuffer.AllocMemory(nullptr, aacDemuxerRst_.frameLength);
    } else {
        aacFrameData = outBuffer.GetMemory();
    }
    switch (status) {
        case 0:
            aacFrameData->Write(aacDemuxerRst_.frameBuffer, aacDemuxerRst_.frameLength);
            if (aacDemuxerRst_.frameBuffer) {
                free(aacDemuxerRst_.frameBuffer);
                aacDemuxerRst_.frameBuffer = nullptr;
            }
            usedDataSize_ = aacDemuxerRst_.usedInputLength;
            ioDataRemainSize_ -= aacDemuxerRst_.usedInputLength;
            break;
        case -1:
        default:
            if (aacDemuxerRst_.frameBuffer) {
                free(aacDemuxerRst_.frameBuffer);
                aacDemuxerRst_.frameBuffer = nullptr;
            }
            return Status::ERROR_UNKNOWN;
    }

    return Status::OK;
}

Status AACDemuxerPlugin::SeekTo(int32_t trackId, int64_t hstTime, SeekMode mode)
{
    return Status::OK;
}

Status AACDemuxerPlugin::Init()
{
    inIoBuffer_ = static_cast<uint8_t *>(malloc(inIoBufferSize_));
    if (inIoBuffer_ == nullptr) {
        MEDIA_LOG_E("inIoBuffer_ malloc failed");
        return Status::ERROR_NO_MEMORY;
    }
    (void)memset_s(inIoBuffer_, inIoBufferSize_, 0x00, inIoBufferSize_);
    return Status::OK;
}
Status AACDemuxerPlugin::Deinit()
{
    if (inIoBuffer_) {
        free(inIoBuffer_);
        inIoBuffer_ = nullptr;
    }
    return Status::OK;
}

Status AACDemuxerPlugin::Prepare()
{
    return Status::OK;
}

Status AACDemuxerPlugin::Reset()
{
    ioContext_.eos = false;
    ioContext_.dataSource.reset();
    ioContext_.offset = 0;
    ioContext_.dataSource.reset();
    ioDataRemainSize_ = 0;
    (void)memset_s(inIoBuffer_, inIoBufferSize_, 0x00, inIoBufferSize_);
    return Status::OK;
}

Status AACDemuxerPlugin::Start()
{
    return Status::OK;
}

Status AACDemuxerPlugin::Stop()
{
    return Status::OK;
}

Status AACDemuxerPlugin::GetParameter(Tag tag, ValueType &value)
{
    return Status::ERROR_UNIMPLEMENTED;
}

Status AACDemuxerPlugin::SetParameter(Tag tag, const ValueType &value)
{
    return Status::ERROR_UNIMPLEMENTED;
}

std::shared_ptr<Allocator> AACDemuxerPlugin::GetAllocator()
{
    return nullptr;
}

Status AACDemuxerPlugin::SetCallback(Callback* cb)
{
    return Status::OK;
}

size_t AACDemuxerPlugin::GetTrackCount()
{
    return 0;
}

Status AACDemuxerPlugin::SelectTrack(int32_t trackId)
{
    return Status::OK;
}

Status AACDemuxerPlugin::UnselectTrack(int32_t trackId)
{
    return Status::OK;
}

Status AACDemuxerPlugin::GetSelectedTracks(std::vector<int32_t>& trackIds)
{
    return Status::OK;
}

int AACDemuxerPlugin::GetFrameLength(const uint8_t *data)
{
    return ((data[3] & 0x03) << 11) | (data[4] << 3) | ((data[5] & 0xE0) >> 5); // 根据协议计算帧长
}

int AACDemuxerPlugin::AudioDemuxerAACOpen(AudioDemuxerUserArg *userArg)
{
    return 0;
}

int AACDemuxerPlugin::AudioDemuxerAACClose()
{
    return 0;
}

int AACDemuxerPlugin::AudioDemuxerAACPrepare(const uint8_t *buf, uint32_t len, AACDemuxerRst *rst)
{
    if (IsAACPattern(buf)) {
        int mpegVersionIndex  = ((buf[1] & 0x0F) >> 3); // 根据协议计算 mpegVersionIndex
        int mpegVersion = -1;
        if (mpegVersionIndex == 0) {
            mpegVersion = 4; // 4
        } else if (mpegVersionIndex == 1) {
            mpegVersion = 2; // 2
        } else {
            return -1;
        }

        int sampleIndex = ((buf[2] & 0x3C) >> 2); // 根据协议计算 sampleIndex
        int channelCount = ((buf[2] & 0x01) << 2) | ((buf[3] & 0xC0) >> 6); // 根据协议计算 channelCount

        int sample = samplingRateMap[sampleIndex];

        rst->frameChannels = channelCount;
        rst->frameSampleRate = sample;
        rst->mpegVersion = mpegVersion;
        MEDIA_LOG_D("channel " PUBLIC_LOG_U8 " sample " PUBLIC_LOG_U32, rst->frameChannels, rst->frameSampleRate);
        return 0;
    } else {
        MEDIA_LOG_D("Err:IsAACPattern");
        return -1;
    }
}

int AACDemuxerPlugin::AudioDemuxerAACProcess(const uint8_t *buffer, uint32_t bufferLen, AACDemuxerRst *rst)
{
    if (rst == nullptr || buffer == nullptr) {
        return -1;
    }
    rst->frameLength = 0;
    rst->frameBuffer = nullptr;
    rst->usedInputLength = 0;

    unsigned int length = 0;
    do {
        if (IsAACPattern(buffer) == 0) {
            MEDIA_LOG_D("Err: IsAACPattern");
            break;
        }

        length = static_cast<unsigned int>(GetFrameLength(buffer));
        if (length + 2 > bufferLen) { // 2
            rst->usedInputLength = bufferLen;
            return 0;
        }

        if (length == 0) {
            MEDIA_LOG_D("length = 0 error");
            return -1;
        }

        if (IsAACPattern(buffer + length)) {
            rst->frameBuffer = static_cast<uint8_t *>(malloc(length));
            if (rst->frameBuffer) {
                FALSE_LOG(memcpy_s(rst->frameBuffer, length, buffer, length) == 0);
                rst->frameLength = length;
                rst->usedInputLength = length;
            } else {
                MEDIA_LOG_E("malloc error, length " PUBLIC_LOG_U32, length);
            }
        } else {
            MEDIA_LOG_D("can't find next aac, length " PUBLIC_LOG_U32 " is error", length);
            break;
        }

        return 0;
    } while (0);

    rst->usedInputLength = 1;
    return 0;
}

int AACDemuxerPlugin::AudioDemuxerAACFreeFrame(uint8_t *frame)
{
    if (frame) {
        free(frame);
    }
    return 0;
}

namespace {
    int IsAACPattern(const uint8_t *data)
    {
        return data[0] == 0xff && (data[1] & 0xf0) == 0xf0 && (data[1] & 0x06) == 0x00; // 根据协议判断是否为AAC帧
    }

    int Sniff(const std::string& name, std::shared_ptr<DataSource> dataSource)
    {
        Status status = Status::ERROR_UNKNOWN;
        auto buffer = std::make_shared<Buffer>();
        auto bufData = buffer->AllocMemory(nullptr, PROBE_READ_LENGTH);
        int processLoop = 1;
        uint8_t *inputDataPtr = nullptr;
        auto result = dataSource->ReadAt(0, buffer, static_cast<size_t>(PROBE_READ_LENGTH));
        if (result != Status::OK) {
            return 0;
        }
        inputDataPtr = const_cast<uint8_t *>(bufData->GetReadOnlyData());
        if (IsAACPattern(inputDataPtr) == 0) {
            MEDIA_LOG_W("Not AAC format");
            return 0;
        }
        return MAX_RANK;
    }

    Status RegisterPlugin(const std::shared_ptr<Register>& reg)
    {
        MEDIA_LOG_I("RegisterPlugin called.");
        if (!reg) {
            MEDIA_LOG_E("RegisterPlugin failed due to nullptr pointer for reg.");
            return Status::ERROR_INVALID_PARAMETER;
        }

        std::string pluginName = "AACDemuxerPlugin";
        DemuxerPluginDef regInfo;
        regInfo.name = pluginName;
        regInfo.description = "adapter for aac demuxer plugin";
        regInfo.rank = MAX_RANK;
        regInfo.creator = [](const std::string &name) -> std::shared_ptr<DemuxerPlugin> {
            return std::make_shared<AACDemuxerPlugin>(name);
        };
        regInfo.sniffer = Sniff;
        auto rtv = reg->AddPlugin(regInfo);
        if (rtv != Status::OK) {
            MEDIA_LOG_I("RegisterPlugin AddPlugin failed with return " PUBLIC_LOG_D32, static_cast<int>(rtv));
        }
        return Status::OK;
    }
}

PLUGIN_DEFINITION(AACDemuxer, LicenseType::APACHE_V2, RegisterPlugin, [] {});
} // namespace AacDemuxer
} // namespace Plugin
} // namespace Media
} // namespace OHOS