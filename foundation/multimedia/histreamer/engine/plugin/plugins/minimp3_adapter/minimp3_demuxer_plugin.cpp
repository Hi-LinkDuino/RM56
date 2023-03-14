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

#define HST_LOG_TAG "Minimp3DemuxerPlugin"

#include "minimp3_demuxer_plugin.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <new>
#include "foundation/log.h"
#include "osal/thread/scoped_lock.h"
#include "plugin/common/plugin_buffer.h"
#include "plugin/common/plugin_time.h"
#include "osal/utils/util.h"
#include "utils/constants.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Minimp3 {
namespace {
constexpr uint32_t MAX_SAMPLES_PERFRAME    = 1152 * 2;
constexpr uint32_t MP3_SEEK_DISCARD_ITEMS  = 2;
constexpr uint32_t ID3_DETECT_SIZE         = 10;
constexpr uint32_t PROBE_READ_LENGTH       = 16 * 1024;
constexpr uint32_t MAX_RANK                = 100;
constexpr uint32_t MEDIA_IO_SIZE           = 12 * 1024;
constexpr uint32_t MAX_FRAME_SIZE          = MEDIA_IO_SIZE;
constexpr uint32_t AUDIO_DEMUXER_SOURCE_ONCE_LENGTH_MAX = 1024;
uint32_t durationMs = 0;
uint32_t fileSize = 0;
AudioDemuxerMp3Attr mp3ProbeAttr;
AudioDemuxerRst mp3ProbeRst;
std::vector<uint32_t> infoLayer         = {1, 2, 3};
std::vector<uint32_t> infoSampleRate    = {8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000};
std::vector<uint32_t> infoBitrateKbps   = {8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176,
                                           192, 224, 256, 288, 320, 352, 384, 416, 448};
size_t AudioDecmuxerMp3Id3v2SizeCalculate(const uint8_t *buf);
bool AudioDemuxerMp3HasId3v2(const uint8_t *buf);
size_t AudioDemuxerMp3GetId3v2Size(const uint8_t *buf, size_t bufSize);
int AudioDemuxerMp3ProbeDecodeCheck(Mp3DemuxerFrameInfo *info);
int AudioDemuxerMp3IterateCallbackForProbe(void *userData, const uint8_t *frame, int frameSize, int freeFormatBytes,
                                           size_t bufSize, uint64_t offset, Mp3DemuxerFrameInfo *info);
Status AudioDemuxerMp3Probe(AudioDemuxerMp3Attr *mp3DemuxerAttr, uint8_t *inputBuffer, uint32_t inputLength,
                            AudioDemuxerRst *mp3DemuxerRst);
int Sniff(const std::string& pluginName, std::shared_ptr<DataSource> dataSource);
Status RegisterPlugin(const std::shared_ptr<Register>& reg);
}

Minimp3DemuxerPlugin::Minimp3DemuxerPlugin(std::string name)
    : DemuxerPlugin(std::move(name)),
      inIoBufferSize_(MEDIA_IO_SIZE),
      fileSize_(0),
      inIoBuffer_(nullptr),
      ioDataRemainSize_(0),
      currentDemuxerPos_(0),
      durationMs_(0),
      ioContext_()
{
    FALSE_LOG(memset_s(&mp3DemuxerAttr_, sizeof(mp3DemuxerAttr_), 0x00, sizeof(AudioDemuxerMp3Attr)) == 0);
    FALSE_LOG(memset_s(&mp3DemuxerRst_, sizeof(mp3DemuxerRst_), 0x00, sizeof(AudioDemuxerRst)) == 0);
    FALSE_LOG(memset_s(&mp3ProbeAttr, sizeof(mp3ProbeAttr), 0x00, sizeof(AudioDemuxerMp3Attr)) == 0);
    FALSE_LOG(memset_s(&mp3ProbeRst, sizeof(mp3ProbeRst), 0x00, sizeof(AudioDemuxerRst)) == 0);
    FALSE_LOG(memset_s(&minimp3DemuxerImpl_, sizeof(minimp3DemuxerImpl_), 0x00, sizeof(Minimp3DemuxerOp)) == 0);
    MEDIA_LOG_I("Minimp3DemuxerPlugin, plugin name: " PUBLIC_LOG_S, pluginName_.c_str());
}

Minimp3DemuxerPlugin::~Minimp3DemuxerPlugin()
{
    MEDIA_LOG_I("~Minimp3DemuxerPlugin");
}

Status Minimp3DemuxerPlugin::SetDataSource(const std::shared_ptr<DataSource>& source)
{
    ioContext_.dataSource = source;
    if (ioContext_.dataSource != nullptr) {
        ioContext_.dataSource->GetSize(fileSize_);
    }
    mp3DemuxerAttr_.fileSize = fileSize_;
    fileSize = fileSize_;
    seekable_ = source->GetSeekable();
    MEDIA_LOG_I("fileSize_ " PUBLIC_LOG_ZU, fileSize_);
    return Status::OK;
}

Status Minimp3DemuxerPlugin::DoReadFromSource(uint32_t readSize)
{
    auto buffer = std::make_shared<Buffer>();
    auto bufData = buffer->AllocMemory(nullptr, readSize);
    int retryTimes = 0;
    MEDIA_LOG_DD("ioNeedReadSize " PUBLIC_LOG_U32 " inIoBufferSize_ " PUBLIC_LOG_D32 " ioDataRemainSize_ "
                PUBLIC_LOG_U32, readSize, inIoBufferSize_, ioDataRemainSize_);
    do {
        auto res = ioContext_.dataSource->ReadAt(ioContext_.offset, buffer, static_cast<size_t>(readSize));
        FALSE_RETURN_V_MSG_W(res == Status::OK, res, "read data from source error " PUBLIC_LOG_D32, (int)res);
        if (bufData->GetSize() == 0 && retryTimes < 200 && ioDataRemainSize_ == 0) { // 200
            MEDIA_LOG_DD("bufData->GetSize() == 0 retryTimes = " PUBLIC_LOG_D32, retryTimes);
            OSAL::SleepFor(30); // 30
            retryTimes++;
            continue;
        }
        FALSE_RETURN_V_MSG_E(retryTimes < 200, Status::ERROR_NOT_ENOUGH_DATA, // 200 times
                             "not eof, but doesn't have enough data");
        MEDIA_LOG_DD("bufData->GetSize() " PUBLIC_LOG "d", bufData->GetSize());
        if (bufData->GetSize() > 0) {
            if (readSize < bufData->GetSize()) {
                MEDIA_LOG_E("Error: ioNeedReadSize < bufData->GetSize()");
                return Status::ERROR_UNKNOWN;
            }
            auto ret = memcpy_s(inIoBuffer_ + ioDataRemainSize_, readSize,
                                const_cast<uint8_t *>(bufData->GetReadOnlyData()), bufData->GetSize());
            if (ret != EOK) {
                MEDIA_LOG_W("memcpy into buffer failed with code " PUBLIC_LOG_D32, ret);
                return Status::ERROR_UNKNOWN;
            }
            ioContext_.offset += bufData->GetSize();
            ioDataRemainSize_ += bufData->GetSize();
        }
        break;
    } while (true);
    return Status::OK;
}

Status Minimp3DemuxerPlugin::GetDataFromSource()
{
    uint32_t ioNeedReadSize = inIoBufferSize_ - ioDataRemainSize_;
    MEDIA_LOG_DD("remain size_ " PUBLIC_LOG_D32 " need read size " PUBLIC_LOG_D32, ioDataRemainSize_, ioNeedReadSize);
    if (ioDataRemainSize_) {
        // 将剩余数据移动到buffer的起始位置
        auto ret = memmove_s(inIoBuffer_, ioDataRemainSize_, inIoBuffer_ + mp3DemuxerRst_.usedInputLength,
            ioDataRemainSize_);
        FALSE_RETURN_V_MSG_W(ret == 0, Status::ERROR_UNKNOWN, "copy buffer error " PUBLIC_LOG_D32, ret);
        ret = memset_s(inIoBuffer_ + ioDataRemainSize_, ioNeedReadSize, 0x00, ioNeedReadSize);
        FALSE_RETURN_V_MSG_W(ret == 0, Status::ERROR_UNKNOWN, "memset_s buffer error " PUBLIC_LOG_D32, ret);
    }
    if (ioContext_.offset >= fileSize_ && ioDataRemainSize_ == 0) {
        ioContext_.eos = true;
        return Status::END_OF_STREAM;
    }
    if (ioContext_.offset + ioNeedReadSize > fileSize_) {
        ioNeedReadSize = fileSize_ - ioContext_.offset; // 在读取文件即将结束时，剩余数据不足，更新读取长度
    }
    if (ioNeedReadSize == 0) {
        return Status::OK;
    }
    return DoReadFromSource(ioNeedReadSize);
}

void Minimp3DemuxerPlugin::FillInMediaInfo(MediaInfo& mediaInfo) const
{
    mediaInfo.tracks.resize(1);
    if (mp3DemuxerRst_.frameChannels == 1) {
        mediaInfo.tracks[0].Insert<Tag::AUDIO_CHANNEL_LAYOUT>(AudioChannelLayout::MONO);
    } else {
        mediaInfo.tracks[0].Insert<Tag::AUDIO_CHANNEL_LAYOUT>(AudioChannelLayout::STEREO);
    }
    int64_t durationHst;
    Ms2HstTime(durationMs, durationHst);
    mediaInfo.tracks[0].Insert<Tag::AUDIO_SAMPLE_RATE>(mp3DemuxerRst_.frameSampleRate);
    mediaInfo.tracks[0].Insert<Tag::MEDIA_BITRATE>(mp3DemuxerRst_.frameBitrateKbps);
    mediaInfo.tracks[0].Insert<Tag::AUDIO_CHANNELS>(mp3DemuxerRst_.frameChannels);
    mediaInfo.tracks[0].Insert<Tag::TRACK_ID>(0);
    mediaInfo.tracks[0].Insert<Tag::MIME>(MEDIA_MIME_AUDIO_MPEG);
    mediaInfo.tracks[0].Insert<Tag::AUDIO_MPEG_VERSION>(1);
    mediaInfo.tracks[0].Insert<Tag::AUDIO_MPEG_LAYER>(mp3DemuxerRst_.audioLayer);
    mediaInfo.tracks[0].Insert<Tag::AUDIO_SAMPLE_PER_FRAME>(mp3DemuxerRst_.samplesPerFrame);
    mediaInfo.tracks[0].Insert<Tag::MEDIA_DURATION>(durationHst);
}

Status Minimp3DemuxerPlugin::GetMediaInfo(MediaInfo& mediaInfo)
{
    int processLoop = 1;
    Status status = Status::ERROR_UNKNOWN;
    while (processLoop) {
        status = GetDataFromSource();
        if (status != Status::OK) {
            return status;
        }
        status = AudioDemuxerMp3Prepare(&mp3DemuxerAttr_, inIoBuffer_, ioDataRemainSize_, &mp3DemuxerRst_);
        switch (status) {
            case Status::ERROR_NOT_ENOUGH_DATA:
                MEDIA_LOG_D("GetMediaInfo: need more data usedInputLength " PUBLIC_LOG_U64,
                            mp3DemuxerRst_.usedInputLength);
                ioDataRemainSize_ -= mp3DemuxerRst_.usedInputLength;
                currentDemuxerPos_ += mp3DemuxerRst_.usedInputLength;
                processLoop = 1;
                break;
            case Status::OK:
                MEDIA_LOG_D("GetMediaInfo: OK usedInputLength " PUBLIC_LOG_U64, mp3DemuxerRst_.usedInputLength);
                ioDataRemainSize_ -= mp3DemuxerRst_.usedInputLength;
                currentDemuxerPos_ += mp3DemuxerRst_.usedInputLength;
                FillInMediaInfo(mediaInfo);
                processLoop = 0;
                break;
            case Status::ERROR_UNSUPPORTED_FORMAT:
                return Status::ERROR_UNSUPPORTED_FORMAT;
            case Status::ERROR_UNKNOWN:
            default:
                processLoop = 0;
                MEDIA_LOG_I("AUDIO_DEMUXER_PREPARE_UNMATCHED_FORMAT " PUBLIC_LOG_D32, status);
                return Status::ERROR_UNKNOWN;
        }
    }

    mp3DemuxerAttr_.bitRate = mp3DemuxerRst_.frameBitrateKbps;
    MEDIA_LOG_D("mp3DemuxerAttr_.bitRate " PUBLIC_LOG_U32 "kbps durationMs " PUBLIC_LOG_U32 " ms",
                mp3DemuxerRst_.frameBitrateKbps, durationMs);
    return Status::OK;
}

uint64_t Minimp3DemuxerPlugin::GetCurrentPositionTimeS(void)
{
    uint64_t currentTime = (static_cast<uint64_t>(currentDemuxerPos_ - mp3DemuxerAttr_.id3v2Size) * 8 * HST_MSECOND) /
        mp3DemuxerAttr_.bitRate;
    return currentTime;
}

Status Minimp3DemuxerPlugin::ReadFrame(Buffer& outBuffer, int32_t timeOutMs)
{
    int  status  = -1;
    Status retResult = Status::OK;
    std::shared_ptr<Memory> mp3FrameData;
    retResult = GetDataFromSource();
    if (retResult != Status::OK) {
        return retResult;
    }
    MEDIA_LOG_DD("ioDataRemainSize_ = " PUBLIC_LOG_D32, ioDataRemainSize_);
    status = AudioDemuxerMp3Process(inIoBuffer_, ioDataRemainSize_);
    if (outBuffer.IsEmpty()) {
        mp3FrameData = outBuffer.AllocMemory(nullptr, mp3DemuxerRst_.frameLength);
    } else {
        mp3FrameData = outBuffer.GetMemory();
    }
    MEDIA_LOG_DD("status = " PUBLIC_LOG_D32, status);
    switch (status) {
        case AUDIO_DEMUXER_SUCCESS:
            MEDIA_LOG_DD("ReadFrame: success usedInputLength " PUBLIC_LOG_D32 " ioDataRemainSize_ " PUBLIC_LOG_D32,
                        (uint32_t)mp3DemuxerRst_.usedInputLength, ioDataRemainSize_);
            if (mp3DemuxerRst_.frameLength) {
                mp3FrameData->Write(mp3DemuxerRst_.frameBuffer, mp3DemuxerRst_.frameLength);
                ioDataRemainSize_ -= mp3DemuxerRst_.usedInputLength;
                currentDemuxerPos_ += mp3DemuxerRst_.usedInputLength;
            } else if (mp3DemuxerRst_.usedInputLength == 0) {
                if (ioDataRemainSize_ > AUDIO_DEMUXER_SOURCE_ONCE_LENGTH_MAX) {
                    ioDataRemainSize_ = ioDataRemainSize_ - AUDIO_DEMUXER_SOURCE_ONCE_LENGTH_MAX;
                    currentDemuxerPos_ += AUDIO_DEMUXER_SOURCE_ONCE_LENGTH_MAX;
                } else {
                    currentDemuxerPos_ += ioDataRemainSize_;
                    ioDataRemainSize_ = 0;
                }
            } else {
                ioDataRemainSize_ -= mp3DemuxerRst_.usedInputLength;
                currentDemuxerPos_ += mp3DemuxerRst_.usedInputLength;
            }
            outBuffer.pts = GetCurrentPositionTimeS();
            MEDIA_LOG_DD("ReadFrame: mp3DemuxerRst_.frameLength " PUBLIC_LOG_U32 ", pts " PUBLIC_LOG_U64,
                        mp3DemuxerRst_.frameLength, outBuffer.pts);
            if (mp3DemuxerRst_.frameBuffer) {
                free(mp3DemuxerRst_.frameBuffer);
                mp3DemuxerRst_.frameBuffer = nullptr;
            }
            break;
        case AUDIO_DEMUXER_PROCESS_NEED_MORE_DATA:
            ioDataRemainSize_ -= mp3DemuxerRst_.usedInputLength;
            currentDemuxerPos_ += mp3DemuxerRst_.usedInputLength;
            MEDIA_LOG_D("ReadFrame: need more data usedInputLength " PUBLIC_LOG_U64 " ioDataRemainSize_ "
                        PUBLIC_LOG_U32, mp3DemuxerRst_.usedInputLength, ioDataRemainSize_);
            break;
        case AUDIO_DEMUXER_ERROR:
        default:
            MEDIA_LOG_E("ReadFrame error");
            if (mp3DemuxerRst_.frameBuffer) {
                free(mp3DemuxerRst_.frameBuffer);
                mp3DemuxerRst_.frameBuffer = nullptr;
            }
            retResult = Status::ERROR_UNKNOWN;
            break;
    }
    return retResult;
}

Status Minimp3DemuxerPlugin::SeekTo(int32_t trackId, int64_t hstTime, SeekMode mode)
{
    uint64_t pos = 0;
    uint32_t targetTimeMs = static_cast<uint32_t>(HstTime2Ms(hstTime));
    if (AudioDemuxerMp3GetSeekPosition(targetTimeMs, &pos) == 0) {
        ioContext_.offset = pos;
        ioDataRemainSize_ = 0;
        currentDemuxerPos_ = pos;
        MEDIA_LOG_D("ioContext_.offset " PUBLIC_LOG_D32, static_cast<uint32_t>(ioContext_.offset));
        (void)memset_s(inIoBuffer_, inIoBufferSize_, 0x00, inIoBufferSize_);
    } else {
        return Status::ERROR_INVALID_PARAMETER;
    }
    return Status::OK;
}

Status Minimp3DemuxerPlugin::Init()
{
    minimp3DemuxerImpl_ = MiniMp3GetOpt();
    AudioDemuxerMp3Open();
    inIoBuffer_ = (uint8_t *)(malloc(inIoBufferSize_));
    if (inIoBuffer_ == nullptr) {
        MEDIA_LOG_E("inIoBuffer_ malloc failed");
        return Status::ERROR_NO_MEMORY;
    }
    (void)memset_s(inIoBuffer_, inIoBufferSize_, 0x00, inIoBufferSize_);
    return Status::OK;
}

Status Minimp3DemuxerPlugin::Deinit()
{
    if (inIoBuffer_) {
        free(inIoBuffer_);
        inIoBuffer_ = nullptr;
    }
    return Status::OK;
}

Status Minimp3DemuxerPlugin::Prepare()
{
    return Status::OK;
}

Status Minimp3DemuxerPlugin::Reset()
{
    ioContext_.eos = false;
    ioContext_.dataSource.reset();
    ioContext_.offset = 0;
    ioDataRemainSize_ = 0;
    currentDemuxerPos_ = 0;
    (void)memset_s(inIoBuffer_, inIoBufferSize_, 0x00, inIoBufferSize_);
    return Status::OK;
}

Status Minimp3DemuxerPlugin::Start()
{
    return Status::OK;
}

Status Minimp3DemuxerPlugin::Stop()
{
    return Status::OK;
}

Status Minimp3DemuxerPlugin::GetParameter(Tag tag, ValueType &value)
{
    return Status::ERROR_UNIMPLEMENTED;
}

Status Minimp3DemuxerPlugin::SetParameter(Tag tag, const ValueType &value)
{
    return Status::ERROR_UNIMPLEMENTED;
}

std::shared_ptr<Allocator> Minimp3DemuxerPlugin::GetAllocator()
{
    return nullptr;
}

Status Minimp3DemuxerPlugin::SetCallback(Callback* cb)
{
    return Status::OK;
}

size_t Minimp3DemuxerPlugin::GetTrackCount()
{
    return 0;
}
Status Minimp3DemuxerPlugin::SelectTrack(int32_t trackId)
{
    return Status::OK;
}
Status Minimp3DemuxerPlugin::UnselectTrack(int32_t trackId)
{
    return Status::OK;
}
Status Minimp3DemuxerPlugin::GetSelectedTracks(std::vector<int32_t>& trackIds)
{
    return Status::OK;
}

void Minimp3DemuxerPlugin::AudioDemuxerMp3IgnoreTailZero(uint8_t *data, uint32_t *dataLen)
{
    if ((data == nullptr) || (dataLen == nullptr) || (*dataLen == 0)) {
        return;
    }

    uint32_t len = *dataLen;
    uint8_t  *ptr = data + len - 1;

    do {
        if (*ptr == 0) {
            ptr--;
            len--;
        } else {
            break;
        }
    } while (len);

    *dataLen = len;
}

int Minimp3DemuxerPlugin::AudioDemuxerMp3IterateCallback(void *userData, const uint8_t *frame, int frameSize,
                                                         int freeFormatBytes, size_t bufSize, uint64_t offset,
                                                         Mp3DemuxerFrameInfo *info)
{
    AudioDemuxerMp3Attr *mp3Demuxer = static_cast<AudioDemuxerMp3Attr *>(userData);
    AudioDemuxerRst *rst = mp3Demuxer->rst;
    uint64_t usedInputLength = 0;

    if (mp3Demuxer->internalRemainLen >= offset + frameSize) {
        usedInputLength = offset + frameSize;
    } else if (mp3Demuxer->internalRemainLen >= offset) {
        usedInputLength = offset;
    } else {
        usedInputLength = 0;
    }
    MEDIA_LOG_DD("offset = " PUBLIC_LOG_U64 " internalRemainLen " PUBLIC_LOG_U32 " frameSize "
                PUBLIC_LOG_D32, offset, mp3Demuxer->internalRemainLen, frameSize);

    if (frameSize == 0) {
        rst->usedInputLength = 0;
        rst->frameBuffer = nullptr;
        rst->frameLength = 0;
        return 0;
    }

    if (frameSize >= MAX_FRAME_SIZE) {
        return AUDIO_DEMUXER_ERROR;
    }

    uint8_t *rstFrame = static_cast<uint8_t *>(calloc(frameSize, sizeof(uint8_t)));
    if (!rstFrame) {
        MEDIA_LOG_E("rstFrame null error");
        return AUDIO_DEMUXER_ERROR;
    }

    (void)memcpy_s(rstFrame, frameSize, frame, frameSize);
    rst->frameBuffer = rstFrame;
    rst->frameLength = frameSize;
    rst->frameBitrateKbps = info->bitrate_kbps;
    rst->frameChannels    = info->channels;
    rst->frameSampleRate  = info->hz;
    rst->usedInputLength  = usedInputLength;
    return 1;
}

int Minimp3DemuxerPlugin::AudioDemuxerMp3IterateCallbackForPrepare(void *userData, const uint8_t *frame,
                                                                   int frameSize, int freeFormatBytes,
                                                                   size_t bufSize, uint64_t offset,
                                                                   Mp3DemuxerFrameInfo *info)
{
    return AudioDemuxerMp3IterateCallbackForProbe(userData, frame, frameSize, freeFormatBytes, bufSize, offset, info);
}

void Minimp3DemuxerPlugin::AudioDemuxerMp3Open()
{
    minimp3DemuxerImpl_.init(&mp3DemuxerAttr_.mp3DemuxerHandle);
    return;
}

int  Minimp3DemuxerPlugin::AudioDemuxerMp3Close()
{
    return 0;
}

Status Minimp3DemuxerPlugin::AudioDemuxerMp3Prepare(AudioDemuxerMp3Attr *mp3DemuxerAttr, uint8_t *buf, uint32_t len,
                                                    AudioDemuxerRst *mp3DemuxerRst)
{
    return AudioDemuxerMp3Probe(mp3DemuxerAttr, buf, len, mp3DemuxerRst);
}

int Minimp3DemuxerPlugin::AudioDemuxerMp3Process(uint8_t *buf, uint32_t len)
{
    if ((buf == nullptr) || (len < 0)) {
        MEDIA_LOG_E(PUBLIC_LOG_S " arg error", __func__);
        return AUDIO_DEMUXER_ERROR;
    }
    if (len == 0) {
        MEDIA_LOG_W("len == 0");
        return AUDIO_DEMUXER_PROCESS_NEED_MORE_DATA;
    }
    int ret = 0;
    uint32_t processLen = len;
    AudioDemuxerMp3IgnoreTailZero(buf, &processLen);
    // this memset_s will always success
    (void)memset_s(&mp3DemuxerRst_, sizeof(AudioDemuxerRst), 0x00, sizeof(AudioDemuxerRst));
    mp3DemuxerAttr_.rst = &mp3DemuxerRst_;
    mp3DemuxerAttr_.internalRemainLen = processLen;
    ret = minimp3DemuxerImpl_.iterateBuf(buf, processLen, AudioDemuxerMp3IterateCallback, &mp3DemuxerAttr_);
    if (mp3DemuxerAttr_.mp3SeekFlag == 1 && mp3DemuxerAttr_.discardItemCount < MP3_SEEK_DISCARD_ITEMS) {
        (void)memset_s(mp3DemuxerRst_.frameBuffer, mp3DemuxerRst_.frameLength, 0x00, mp3DemuxerRst_.frameLength);
        mp3DemuxerAttr_.discardItemCount++;
    } else {
        mp3DemuxerAttr_.discardItemCount = 0;
        mp3DemuxerAttr_.mp3SeekFlag = 0;
    }
    if (ret == 0 || ret == 1) {
        return AUDIO_DEMUXER_SUCCESS;
    } else {
        return AUDIO_DEMUXER_ERROR;
    }
}

int Minimp3DemuxerPlugin::AudioDemuxerMp3FreeFrame(uint8_t *frame)
{
    if (frame) {
        free(frame);
        return 0;
    } else {
        return -1;
    }
}

int Minimp3DemuxerPlugin::AudioDemuxerMp3Seek(uint32_t pos, uint8_t *buf, uint32_t len, AudioDemuxerRst *rst)
{
    return 0;
}

int Minimp3DemuxerPlugin::AudioDemuxerMp3GetSeekPosition(uint32_t targetTimeMs, uint64_t *pos)
{
    if (!pos) {
        MEDIA_LOG_I("pos nullptr error");
        return AUDIO_DEMUXER_ERROR;
    }
    uint32_t targetPos = targetTimeMs * mp3DemuxerAttr_.bitRate / 8 + mp3DemuxerAttr_.id3v2Size;
    if (targetPos > mp3DemuxerAttr_.fileSize) {
        *pos = 0;
        return -1;
    }
    *pos = static_cast<uint64_t>(targetPos);
    mp3DemuxerAttr_.mp3SeekFlag = 1;
    return 0;
}

namespace {
size_t AudioDecmuxerMp3Id3v2SizeCalculate(const uint8_t *buf)
{
    return (((buf[6] & 0x7f) << 21) | ((buf[7] & 0x7f) << 14) | ((buf[8] & 0x7f) << 7) | (buf[9] & 0x7f)) + 10;
}

bool AudioDemuxerMp3HasId3v2(const uint8_t *buf)
{
    return !memcmp(buf, "ID3", 3) && !((buf[5] & 15) || (buf[6] & 0x80) || (buf[7] & 0x80) ||
                  (buf[8] & 0x80) || (buf[9] & 0x80));
}

size_t AudioDemuxerMp3GetId3v2Size(const uint8_t *buf, size_t bufSize)
{
    if (bufSize >= ID3_DETECT_SIZE && AudioDemuxerMp3HasId3v2(buf)) {
        size_t id3v2Size = AudioDecmuxerMp3Id3v2SizeCalculate(buf);
        if ((buf[5] & 16)) { // 5, 16
            id3v2Size += 10; // 10
        }
        return id3v2Size;
    }
    return 0;
}

int AudioDemuxerMp3ProbeDecodeCheck(Mp3DemuxerFrameInfo *info)
{
    if (!info) {
        return -1;
    }

    std::vector<uint32_t>::iterator it = find (infoLayer.begin(), infoLayer.end(), info->layer);
    if (it == infoLayer.end()) {
        return -1;
    }

    it = find (infoSampleRate.begin(), infoSampleRate.end(), info->hz);
    if (it == infoSampleRate.end()) {
        return -1;
    }

    it = find (infoBitrateKbps.begin(), infoBitrateKbps.end(), info->bitrate_kbps);
    if (it == infoBitrateKbps.end()) {
        return -1;
    }

    return 0;
}

int AudioDemuxerMp3IterateCallbackForProbe(void *userData, const uint8_t *frame, int frameSize, int freeFormatBytes,
                                           size_t bufSize, uint64_t offset, Mp3DemuxerFrameInfo *info)
{
    int sampleCount;
    Minimp3WrapperMp3decFrameInfo frameInfo;
    AudioDemuxerMp3Attr *mp3Demuxer = static_cast<AudioDemuxerMp3Attr *>(userData);
    AudioDemuxerRst *rst  = mp3Demuxer->rst;
    rst->frameBitrateKbps = info->bitrate_kbps;
    rst->frameChannels    = info->channels;
    rst->frameSampleRate  = info->hz;
    rst->audioLayer       = info->layer;
    rst->samplesPerFrame  = info->samples_per_frame;
    sampleCount = Minimp3WrapperMp3decDecodeFrame(&mp3Demuxer->mp3DemuxerHandle, frame, frameSize,
                                                  mp3Demuxer->probePcmBuf, &frameInfo);
    if (sampleCount <= 0 && AudioDemuxerMp3ProbeDecodeCheck(info) != 0) {
        return -1;
    }
    return 1;
}

Status AudioDemuxerMp3Probe(AudioDemuxerMp3Attr* mp3DemuxerAttr, uint8_t* inputBuffer, uint32_t inputLength,
                            AudioDemuxerRst* mp3DemuxerRst)
{
    FALSE_RETURN_V_MSG_W(inputBuffer != nullptr && inputLength >= 0, Status::ERROR_INVALID_PARAMETER, "invalid parameter");
    if (inputLength == 0) {
        return Status::ERROR_NOT_ENOUGH_DATA;
    }
    int ret = -1;
    if (mp3DemuxerAttr->id3v2SkipFlag == 0) {
        if (mp3DemuxerAttr->id3v2Offset == 0) {
            if (inputLength < ID3_DETECT_SIZE) {
                mp3DemuxerRst->usedInputLength = 0;
                return Status::ERROR_NOT_ENOUGH_DATA;
            } else {
                mp3DemuxerAttr->id3v2Size = AudioDemuxerMp3GetId3v2Size(inputBuffer, inputLength);
                mp3DemuxerAttr->id3v2Offset = mp3DemuxerAttr->id3v2Size;
            }
        }

        if (mp3DemuxerAttr->id3v2Offset) {
            MEDIA_LOG_D("mp3 id3v2Offset = " PUBLIC_LOG_U32 ", input data inputLength " PUBLIC_LOG_U32,
                        mp3DemuxerAttr->id3v2Offset, inputLength);
            if (inputLength >= mp3DemuxerAttr->id3v2Offset) {
                mp3DemuxerRst->usedInputLength = mp3DemuxerAttr->id3v2Offset;
                mp3DemuxerAttr->id3v2SkipFlag  = 1;
                inputLength -= mp3DemuxerAttr->id3v2Offset;
                inputBuffer += mp3DemuxerAttr->id3v2Offset;
                mp3DemuxerAttr->id3v2Offset = 0;
            } else {
                mp3DemuxerRst->usedInputLength = inputLength;
                mp3DemuxerAttr->id3v2Offset = mp3DemuxerAttr->id3v2Offset - inputLength;
                return Status::ERROR_NOT_ENOUGH_DATA;
            }
        }
    }
    mp3DemuxerAttr->rst = mp3DemuxerRst;
    mp3DemuxerAttr->internalRemainLen = inputLength;
    ret = Minimp3WrapperMp3decIterateBuf(inputBuffer, inputLength, AudioDemuxerMp3IterateCallbackForProbe,
                                         mp3DemuxerAttr);
    if (ret != 1) {
        if (mp3DemuxerAttr->id3v2SkipFlag) {
            return Status::ERROR_NOT_ENOUGH_DATA;
        }
        return Status::ERROR_UNSUPPORTED_FORMAT;
    }
    if (mp3DemuxerRst->frameBitrateKbps != 0) {
        durationMs = static_cast<uint64_t>(fileSize * 8 / mp3DemuxerRst->frameBitrateKbps); // 8
    }
    MEDIA_LOG_I("bitrate_kbps = " PUBLIC_LOG_U32 " info->channels = " PUBLIC_LOG_U8 " info->hz = "
                PUBLIC_LOG_U32, mp3DemuxerRst->frameBitrateKbps, mp3DemuxerRst->frameChannels,
                mp3DemuxerRst->frameSampleRate);
    return Status::OK;
}

int Sniff(const std::string& name, std::shared_ptr<DataSource> dataSource)
{
    MEDIA_LOG_I("Sniff in");
    Status status = Status::ERROR_UNKNOWN;
    auto buffer = std::make_shared<Buffer>();
    auto bufData = buffer->AllocMemory(nullptr, PROBE_READ_LENGTH);
    int processLoop = 1;
    uint8_t *inputDataPtr = nullptr;
    int offset = 0;
    int readSize = PROBE_READ_LENGTH;
    size_t sourceSize = 0;
    dataSource->GetSize(sourceSize);
    while (processLoop) {
        if (sourceSize < PROBE_READ_LENGTH && sourceSize != 0) {
            readSize = sourceSize;
        }
        status = dataSource->ReadAt(offset, buffer, static_cast<size_t>(readSize));
        if (status != Status::OK) {
            MEDIA_LOG_E("Sniff Read Data Error");
            return 0;
        }
        inputDataPtr = const_cast<uint8_t *>(bufData->GetReadOnlyData());

        status = AudioDemuxerMp3Probe(&mp3ProbeAttr, inputDataPtr, bufData->GetSize(), &mp3ProbeRst);
        switch (status) {
            case Status::ERROR_NOT_ENOUGH_DATA:
                OSAL::SleepFor(100); // 100
                offset += mp3ProbeRst.usedInputLength;
                MEDIA_LOG_D("offset " PUBLIC_LOG_D32, offset);
                processLoop = 1;
                break;
            case Status::OK:
                processLoop = 0;
                break;
            case Status::ERROR_UNSUPPORTED_FORMAT:
                return 0;
            case Status::ERROR_UNKNOWN:
            default:
                processLoop = 0;
                MEDIA_LOG_I("AUDIO_DEMUXER_PREPARE_UNMATCHED_FORMAT " PUBLIC_LOG_D32, status);
                return 0;
        }
    }
    return MAX_RANK;
}

Status RegisterPlugin(const std::shared_ptr<Register>& reg)
{
    MEDIA_LOG_I("RegisterPlugin called.");
    if (!reg) {
        MEDIA_LOG_I("RegisterPlugin failed due to nullptr pointer for reg.");
        return Status::ERROR_INVALID_PARAMETER;
    }

    std::string pluginName = "Minimp3DemuxerPlugin";
    DemuxerPluginDef regInfo;
    regInfo.name = pluginName;
    regInfo.description = "adapter for minimp3 demuxer plugin";
    regInfo.rank = MAX_RANK;
    regInfo.creator = [](const std::string &name) -> std::shared_ptr<DemuxerPlugin> {
        return std::make_shared<Minimp3DemuxerPlugin>(name);
    };
    regInfo.sniffer = Sniff;
    auto rtv = reg->AddPlugin(regInfo);
    if (rtv != Status::OK) {
        MEDIA_LOG_I("RegisterPlugin AddPlugin failed with return " PUBLIC_LOG_D32, static_cast<int>(rtv));
    }
    return Status::OK;
}
}

PLUGIN_DEFINITION(Minimp3Demuxer, LicenseType::CC0, RegisterPlugin, [] {});
} // namespace Minimp3
} // namespace Plugin
} // namespace Media
} // namespace OHOS
