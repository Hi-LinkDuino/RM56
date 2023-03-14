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

#define HST_LOG_TAG "Minimp4DemuxerPlugin"

#include "minimp4_demuxer_plugin.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <new>

#include <securec.h>
#include "foundation/log.h"
#include "plugin/common/plugin_buffer.h"
#include "plugin/common/plugin_time.h"
#include "utils/constants.h"
#include "osal/utils/util.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Minimp4 {
namespace {
std::vector<int> sampleRateVec {
    96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350
};
uint32_t durationMs = 0;
uint32_t fileSize = 0;
uint32_t readDataSize = 0;
constexpr int8_t ADTS_HEADER_SIZE = 7;
constexpr int8_t MP4_HEADER_OFFSET = 4;
constexpr int8_t RANK_MAX = 100;
constexpr unsigned int DEFAULT_AUDIO_SAMPLE_PER_FRAME = 1024;
constexpr unsigned int MEDIA_IO_SIZE = 16 * 1024;
int Sniff(const std::string &name, std::shared_ptr<DataSource> dataSource);
Status RegisterPlugins(const std::shared_ptr<Register> &reg);
}

MiniMP4DemuxerPlugin::MiniMP4DemuxerPlugin(std::string name)
    : DemuxerPlugin(std::move(name)),
      ioContext_(),
      fileSize_(0),
      inIoBuffer_(nullptr),
      inIoBufferSize_(MEDIA_IO_SIZE),
      ioDataRemainSize_(0),
      sampleIndex_(0)
{
    (void)memset_s(&miniMP4_, sizeof(MP4D_demux_t), 0, sizeof(MP4D_demux_t));
    MEDIA_LOG_I("MiniMP4DemuxerPlugin, plugin name: " PUBLIC_LOG_S, pluginName_.c_str());
}

MiniMP4DemuxerPlugin::~MiniMP4DemuxerPlugin()
{
    MEDIA_LOG_I("~MiniMP4DemuxerPlugin");
}

Status MiniMP4DemuxerPlugin::SetDataSource(const std::shared_ptr<DataSource> &source)
{
    ioContext_.dataSource = source;
    if (ioContext_.dataSource != nullptr) {
        ioContext_.dataSource->GetSize(fileSize_);
    }
    fileSize = fileSize_;
    MEDIA_LOG_I("fileSize_ " PUBLIC_LOG_ZU, fileSize_);
    return Status::OK;
}

Status MiniMP4DemuxerPlugin::Init()
{
    MEDIA_LOG_I("Init called");
    inIoBuffer_ = static_cast<uint8_t *>(malloc(inIoBufferSize_));
    if (inIoBuffer_ == nullptr) {
        MEDIA_LOG_E("inIoBuffer_ malloc failed");
        return Status::ERROR_NO_MEMORY;
    }
    (void)memset_s(inIoBuffer_, inIoBufferSize_, 0x00, inIoBufferSize_);
    return Status::OK;
}

Status MiniMP4DemuxerPlugin::Deinit()
{
    if (inIoBuffer_) {
        free(inIoBuffer_);
        inIoBuffer_ = nullptr;
    }
    return Status::OK;
}

Status MiniMP4DemuxerPlugin::Prepare()
{
    return Status::OK;
}

Status MiniMP4DemuxerPlugin::Reset()
{
    MEDIA_LOG_D("Reset in");
    ioContext_.eos = false;
    ioContext_.offset = 0;
    ioContext_.dataSource.reset();
    ioDataRemainSize_ = 0;
    (void)memset_s(inIoBuffer_, inIoBufferSize_, 0x00, inIoBufferSize_);
    return Status::OK;
}

Status MiniMP4DemuxerPlugin::Stop()
{
    return Status::OK;
}

Status MiniMP4DemuxerPlugin::GetParameter(Tag tag, ValueType &value)
{
    (void)tag;
    (void)value;
    return Status::ERROR_UNIMPLEMENTED;
}

Status MiniMP4DemuxerPlugin::SetParameter(Tag tag, const ValueType &value)
{
    (void)tag;
    (void)value;
    return Status::ERROR_UNIMPLEMENTED;
}

std::shared_ptr<Allocator> MiniMP4DemuxerPlugin::GetAllocator()
{
    return nullptr;
}

Status MiniMP4DemuxerPlugin::SetCallback(Callback* cb)
{
    return Status::OK;
}

size_t MiniMP4DemuxerPlugin::GetTrackCount()
{
    size_t trackCnt = 0;
    return trackCnt;
}

Status MiniMP4DemuxerPlugin::SelectTrack(int32_t trackId)
{
    return Status::ERROR_UNIMPLEMENTED;
}

Status MiniMP4DemuxerPlugin::UnselectTrack(int32_t trackId)
{
    return Status::OK;
}

Status MiniMP4DemuxerPlugin::GetSelectedTracks(std::vector<int32_t> &trackIds)
{
    trackIds.clear();
    trackIds.push_back(1);
    return Status::OK;
}

Status MiniMP4DemuxerPlugin::DoReadFromSource(uint32_t readSize)
{
    if (readSize == 0) {
        return Status::OK;
    }
    auto buffer  = std::make_shared<Buffer>();
    auto bufData = buffer->AllocMemory(nullptr, readSize);
    int retryTimes = 0;
    MEDIA_LOG_D("readSize " PUBLIC_LOG_U32 " inIoBufferSize_ " PUBLIC_LOG_D32 "ioDataRemainSize_ "
                PUBLIC_LOG_U32 "", readSize, inIoBufferSize_, ioDataRemainSize_);
    do {
        auto result = ioContext_.dataSource->ReadAt(ioContext_.offset, buffer, static_cast<size_t>(readSize));
        MEDIA_LOG_D("ioContext_.offset " PUBLIC_LOG_D32, static_cast<uint32_t>(ioContext_.offset));
        if (result != Status::OK) {
            MEDIA_LOG_W("read data from source warning " PUBLIC_LOG_D32, static_cast<int>(result));
            return result;
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
            ioContext_.offset += bufData->GetSize();
            ioDataRemainSize_  += bufData->GetSize();
        }
        if (bufData->GetSize() == 0 && ioDataRemainSize_ == 0 && retryTimes < 200) { // 200
            OHOS::Media::OSAL::SleepFor(30); // 30
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

Status MiniMP4DemuxerPlugin::GetDataFromSource()
{
    uint32_t ioNeedReadSize = inIoBufferSize_ - ioDataRemainSize_;
    MEDIA_LOG_D("ioDataRemainSize_ " PUBLIC_LOG_D32 " ioNeedReadSize " PUBLIC_LOG_D32, ioDataRemainSize_,
        ioNeedReadSize);
    if (ioDataRemainSize_) {
        // 将剩余数据移动到buffer的起始位置
        auto ret = memmove_s(inIoBuffer_,
                             ioDataRemainSize_,
                             inIoBuffer_ + readDataSize,
                             ioDataRemainSize_);
        if (ret != 0) {
            MEDIA_LOG_E("copy buffer error(" PUBLIC_LOG_D32 ")", ret);
            return Status::ERROR_UNKNOWN;
        }
        ret = memset_s(inIoBuffer_ + ioDataRemainSize_, ioNeedReadSize, 0x00, ioNeedReadSize);
        if (ret != 0) {
            MEDIA_LOG_E("memset_s buffer error(" PUBLIC_LOG_D32 ")", ret);
            return Status::ERROR_UNKNOWN;
        }
    }
    if (ioContext_.offset >= fileSize_ && ioDataRemainSize_ == 0) {
        ioContext_.eos = true;
        return Status::END_OF_STREAM;
    }
    if (ioContext_.offset + ioNeedReadSize > fileSize_) {
        ioNeedReadSize = fileSize_ - ioContext_.offset; // 在读取文件即将结束时，剩余数据不足，更新读取长度
    }

    return DoReadFromSource(ioNeedReadSize);
}

Status MiniMP4DemuxerPlugin::GetMediaInfo(MediaInfo &mediaInfo)
{
    if (fileSize_ == 0 || ioContext_.dataSource == nullptr) {
        return Status::ERROR_UNKNOWN;
    }

    if (MP4D_open(&miniMP4_, ReadCallback, reinterpret_cast<void *>(this), fileSize_) == 0) {
        MEDIA_LOG_E("MP4D_open IS ERROR");
        return Status::ERROR_MISMATCHED_TYPE;
    }
    if (AudioAdapterForDecoder() != Status::OK) {
        return Status::ERROR_UNKNOWN;
    }
    mediaInfo.tracks.resize(1);
    mediaInfo.tracks[0].insert({Tag::AUDIO_SAMPLE_RATE,
        static_cast<uint32_t>(miniMP4_.track->SampleDescription.audio.samplerate_hz) });
    mediaInfo.tracks[0].insert({Tag::MEDIA_BITRATE, static_cast<int64_t>(miniMP4_.track->avg_bitrate_bps) });
    mediaInfo.tracks[0].insert({Tag::AUDIO_CHANNELS,
        static_cast<uint32_t>(miniMP4_.track->SampleDescription.audio.channelcount) });
    mediaInfo.tracks[0].insert({Tag::TRACK_ID, static_cast<uint32_t>(0) });
    mediaInfo.tracks[0].insert({Tag::MIME, std::string(MEDIA_MIME_AUDIO_AAC) });
    mediaInfo.tracks[0].insert({Tag::AUDIO_MPEG_VERSION, static_cast<uint32_t>(4) }); // 4
    mediaInfo.tracks[0].insert({Tag::AUDIO_AAC_PROFILE, AudioAacProfile::LC });
    mediaInfo.tracks[0].insert({Tag::AUDIO_AAC_STREAM_FORMAT, AudioAacStreamFormat::MP4ADTS });
    mediaInfo.tracks[0].insert({Tag::AUDIO_SAMPLE_FORMAT, AudioSampleFormat::S16 });
    mediaInfo.tracks[0].insert({Tag::AUDIO_SAMPLE_PER_FRAME, DEFAULT_AUDIO_SAMPLE_PER_FRAME });
    if (miniMP4_.track->SampleDescription.audio.channelcount == 1) {
        mediaInfo.tracks[0].insert({Tag::AUDIO_CHANNEL_LAYOUT, AudioChannelLayout::MONO });
    } else {
        mediaInfo.tracks[0].insert({Tag::AUDIO_CHANNEL_LAYOUT, AudioChannelLayout::STEREO });
    }

    unsigned int frameSize = 0;
    unsigned int timeStamp = 0;
    unsigned int duration = 0;
    int64_t offset = MP4D_frame_offset(&miniMP4_, 0, 0, &frameSize, &timeStamp, &duration);
    ioDataRemainSize_ = 0;
    ioContext_.offset = offset;
    MEDIA_LOG_D("samplerate_hz " PUBLIC_LOG_D32,
        static_cast<uint32_t>(miniMP4_.track->SampleDescription.audio.samplerate_hz));
    MEDIA_LOG_D("avg_bitrate_bps " PUBLIC_LOG_D32, static_cast<uint32_t>(miniMP4_.track->avg_bitrate_bps));
    MEDIA_LOG_D("channel num " PUBLIC_LOG_D32,
        static_cast<uint32_t>(miniMP4_.track->SampleDescription.audio.channelcount));
    return Status::OK;
}


void MiniMP4DemuxerPlugin::FillADTSHead(std::shared_ptr<Memory> &data, unsigned int frameSize)
{
    uint8_t adtsHeader[ADTS_HEADER_SIZE] = {0};
    unsigned int channelConfig = miniMP4_.track->SampleDescription.audio.channelcount;
    unsigned int packetLen = frameSize + 7;
    unsigned int samplerateIndex = 0;
    /* 按格式读取信息帧 */
    uint8_t objectTypeIndication = miniMP4_.track->object_type_indication;
    samplerateIndex = ((miniMP4_.track->dsi[0] & 0x7) << 1) + (miniMP4_.track->dsi[1] >> 7); // 1,7 按协议取信息帧
    adtsHeader[0] = static_cast<uint8_t>(0xFF);
    adtsHeader[1] = static_cast<uint8_t>(0xF1);
    adtsHeader[2] = static_cast<uint8_t>(objectTypeIndication) + (samplerateIndex << 2) + (channelConfig >> 2); // 2
    adtsHeader[3] = static_cast<uint8_t>(((channelConfig & 0x3) << 6) + (packetLen >> 11)); // 3,6,11 按协议取信息帧
    adtsHeader[4] = static_cast<uint8_t>((packetLen & 0x7FF) >> 3); // 4, 3 按协议取信息帧
    adtsHeader[5] = static_cast<uint8_t>(((packetLen & 0x7) << 5) + 0x1F); // 5 按协议取信息帧
    adtsHeader[6] = static_cast<uint8_t>(0xFC); // 6 按协议取信息帧
    data->Write(adtsHeader, ADTS_HEADER_SIZE, 0);
}

int MiniMP4DemuxerPlugin::ReadCallback(int64_t offset, void *buffer, size_t size, void *token)
{
    MiniMP4DemuxerPlugin *mp4Demuxer = (MiniMP4DemuxerPlugin *)token;
    unsigned int file_size = mp4Demuxer->GetFileSize();
    if (offset >= file_size) {
        MEDIA_LOG_E("ReadCallback offset is bigger");
        return -1;
    }

    if ((offset + size) <= mp4Demuxer->ioContext_.offset) {
        (void)memcpy_s(buffer, size, mp4Demuxer->inIoBuffer_ +
            (mp4Demuxer->ioDataRemainSize_ - (mp4Demuxer->ioContext_.offset - offset)), size);
        return 0;
    }
    while ((offset + size) > mp4Demuxer->ioContext_.offset) {
        MEDIA_LOG_D("offset " PUBLIC_LOG_D32 " size " PUBLIC_LOG_ZU,
            static_cast<uint32_t>(offset), static_cast<uint32_t>(size));
        MEDIA_LOG_D("mp4Demuxer->ioContext_.offset " PUBLIC_LOG_D32,
            static_cast<uint32_t>(mp4Demuxer->ioContext_.offset));
        mp4Demuxer->ioDataRemainSize_ = 0;
        mp4Demuxer->ioContext_.offset = offset;
        Status status = Status::ERROR_UNKNOWN;
        readDataSize = mp4Demuxer->inIoBufferSize_;
        status = mp4Demuxer->GetDataFromSource();
        if (status != Status::OK) {
            return (int)status;
        }
    }

    (void)memcpy_s(buffer, size, mp4Demuxer->inIoBuffer_, size);

    return 0;
}

Status MiniMP4DemuxerPlugin::ReadFrame(Buffer &outBuffer, int32_t timeOutMs)
{
    Status retResult = Status::OK;
    std::shared_ptr<Memory> mp4FrameData;
    if (sampleIndex_ >= miniMP4_.track->sample_count) {
        (void)memset_s(inIoBuffer_, MEDIA_IO_SIZE, 0, MEDIA_IO_SIZE);
        ioDataRemainSize_ = 0;
        MEDIA_LOG_DD("sampleIndex_ " PUBLIC_LOG_D32, sampleIndex_);
        MEDIA_LOG_DD("miniMP4_.track->sample_count " PUBLIC_LOG_D32, miniMP4_.track->sample_count);
        return Status::END_OF_STREAM;
    }
    unsigned int frameSize = 0;
    unsigned int timeStamp = 0;
    unsigned int duration = 0;
    uint64_t offset = MP4D_frame_offset(&miniMP4_, 0, sampleIndex_, &frameSize, &timeStamp, &duration);
    if (offset > fileSize_) {
        return Status::ERROR_UNKNOWN;
    }
    MEDIA_LOG_D("frameSize " PUBLIC_LOG_D32 " offset " PUBLIC_LOG_D32 " sampleIndex_ " PUBLIC_LOG_D32,
        frameSize, static_cast<uint32_t>(offset), sampleIndex_);
    if (outBuffer.IsEmpty()) {
        mp4FrameData = outBuffer.AllocMemory(nullptr, frameSize + ADTS_HEADER_SIZE);
    } else {
        mp4FrameData = outBuffer.GetMemory();
    }

    if (offset > ioContext_.offset) {
        (void)memset_s(inIoBuffer_, MEDIA_IO_SIZE, 0, MEDIA_IO_SIZE);
        ioDataRemainSize_ = 0;
        ioContext_.offset = offset;
    }
    retResult = GetDataFromSource();
    if (retResult != Status::OK) {
        return retResult;
    }
    FillADTSHead(mp4FrameData, frameSize);
    size_t writeSize = mp4FrameData->Write(inIoBuffer_, frameSize, ADTS_HEADER_SIZE);
    sampleIndex_++;
    MEDIA_LOG_D("writeSize " PUBLIC_LOG_ZU " mp4FrameData size " PUBLIC_LOG_ZU, writeSize, mp4FrameData->GetSize());
    ioDataRemainSize_ -= frameSize;
    readDataSize = frameSize;

    return Status::OK;
}

Status MiniMP4DemuxerPlugin::SeekTo(int32_t trackId, int64_t hstTime, SeekMode mode)
{
    unsigned int frameSize = 0;
    unsigned int timeStamp = 0;
    unsigned int duration = 0;
    uint64_t offsetStart = MP4D_frame_offset(&miniMP4_, 0, 0, &frameSize, &timeStamp, &duration);
    uint64_t offsetEnd =
        MP4D_frame_offset(&miniMP4_, 0, miniMP4_.track->sample_count - 1, &frameSize, &timeStamp, &duration);
    uint64_t targetPos = (Plugin::HstTime2Ms(hstTime) * static_cast<int64_t>(miniMP4_.track->avg_bitrate_bps)) / 8 +
        offsetStart;
    if (targetPos >= offsetEnd) {
        sampleIndex_ = miniMP4_.track->sample_count;
        return Status::OK;
    }
    sampleIndex_ = 0;
    uint64_t tempPos = 0;
    while (sampleIndex_ < miniMP4_.track->sample_count) {
        tempPos = MP4D_frame_offset(&miniMP4_, 0, sampleIndex_, &frameSize, &timeStamp, &duration);
        if (tempPos < targetPos) {
            sampleIndex_++;
        } else {
            break;
        }
    }
    ioContext_.offset = tempPos;
    ioDataRemainSize_ = 0;
    MEDIA_LOG_D("ioContext_.offset " PUBLIC_LOG_D32, static_cast<uint32_t>(ioContext_.offset));
    (void)memset_s(inIoBuffer_, inIoBufferSize_, 0x00, inIoBufferSize_);
    return Status::OK;
}

size_t MiniMP4DemuxerPlugin::GetFileSize()
{
    return fileSize_;
}

Status MiniMP4DemuxerPlugin::AudioAdapterForDecoder()
{
    if (miniMP4_.track == nullptr) {
        return Status::ERROR_UNKNOWN;
    }
    /* 适配解码协议 */
    size_t sampleRateIndex = (static_cast<unsigned int>(miniMP4_.track->dsi[0] & 0x7) << 1) +
        (static_cast<unsigned int>(miniMP4_.track->dsi[1]) >> 7);

    if ((sampleRateVec.size() <= sampleRateIndex) || (miniMP4_.track->dsi_bytes >= 20)) { // 20 按协议适配解码器
        return Status::ERROR_MISMATCHED_TYPE;
    }
    miniMP4_.track->SampleDescription.audio.samplerate_hz = sampleRateVec[sampleRateIndex];
    miniMP4_.track->SampleDescription.audio.channelcount = (miniMP4_.track->dsi[1] & 0x7F) >> 3; // 3 按协议适配解码器
    return Status::OK;
}

namespace {
int Sniff(const std::string &name, std::shared_ptr<DataSource> dataSource)
{
    unsigned char m4aCheck[] = {'f', 't', 'y', 'p'};
    auto buffer = std::make_shared<Buffer>();
    auto bufData = buffer->AllocMemory(nullptr, sizeof(m4aCheck));
    int retryTimes = 0;
    do {
        if (dataSource->ReadAt(MP4_HEADER_OFFSET, buffer, static_cast<size_t>(sizeof(m4aCheck))) != Status::OK) {
            return 0;
        }
        if (bufData->GetSize() < sizeof(m4aCheck) && retryTimes < 50) { // 50
            OSAL::SleepFor(100); // 100
            retryTimes++;
            continue;
        }
        if (memcmp(const_cast<uint8_t *>(bufData->GetReadOnlyData()), &m4aCheck, sizeof(m4aCheck)) != 0) {
            MEDIA_LOG_E("memcmp m4aCheck is error");
            return 0;
        }
        break;
    } while (true);
    return RANK_MAX;
}

Status RegisterPlugins(const std::shared_ptr<Register> &reg)
{
    MEDIA_LOG_D("RegisterPlugins called");
    if (!reg) {
        MEDIA_LOG_E("RegisterPlugins fail due to null pointer for reg");
        return Status::ERROR_INVALID_PARAMETER;
    }
    std::string pluginName = "MiniMP4DemuxerPlugin";
    DemuxerPluginDef regInfo;
    regInfo.name = pluginName;
    regInfo.description = "adapter for minimp4 demuxer plugin";
    regInfo.rank = RANK_MAX;
    regInfo.creator = [](const std::string &name) -> std::shared_ptr<DemuxerPlugin> {
        return std::make_shared<MiniMP4DemuxerPlugin>(name);
    };
    regInfo.sniffer = Sniff;
    auto ret = reg->AddPlugin(regInfo);
    if (ret != Status::OK) {
        MEDIA_LOG_E("RegisterPlugin AddPlugin failed with return " PUBLIC_LOG_D32, static_cast<int>(ret));
    }
    return Status::OK;
}
}

PLUGIN_DEFINITION(MiniMP4Demuxer, LicenseType::CC0, RegisterPlugins, [] {});
} // namespace Minimp4
} // namespace Plugin
} // namespace Media
} // namespace OHOS