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

#include "recorder_impl.h"
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/io.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include "media_log.h"
#include "securec.h"

namespace OHOS {
namespace Media {
constexpr float RECORDER_DEFAULT_SPEED = 1.0;
constexpr uint32_t RECORDER_AUDIO_THREAD_PRIORITY = 19;
constexpr uint32_t RECORDER_VIDEO_THREAD_PRIORITY = 20;

constexpr uint32_t RECORDER_VIDEO_SOURCE_ID_MASK = 0;
constexpr uint32_t RECORDER_AUDIO_SOURCE_ID_MASK = 0x100;
constexpr uint32_t RECORDER_DATA_SOURCE_ID_MASK = 0x200;

static int64_t CalcDiffTimeMs(struct timeval begin, struct timeval end)
{
    const int32_t us2Ms = 1000;
    const int32_t us2MsHalf = 500;
    const int32_t s2Ms = 1000;
    int64_t diffSec = static_cast<int64_t> (end.tv_sec - begin.tv_sec);
    int64_t diffMsec = (static_cast<int64_t> (end.tv_usec - begin.tv_usec) + us2MsHalf) / us2Ms;
    const int64_t diffSecMax = INT64_MAX / s2Ms;
    diffSec = (diffSec > diffSecMax) ? diffSecMax : diffSec;
    return diffSec * s2Ms + diffMsec;
}

static void SetDefaultVideoConfig(RecorderVideoSourceConfig &config)
{
    const int32_t width = 1920;
    const int32_t height = 1080;
    const int32_t frameRate = 30;
    const int32_t bitRate = 4 * 1024; /* kbps */

    config.videoFormat = HEVC;
    config.width = width;
    config.height = height;
    config.frameRate = frameRate;
    config.bitRate = bitRate;
    config.captureRate = frameRate;
    config.speed = RECORDER_DEFAULT_SPEED;
}

static void SetDefaultAudioConfig(RecorderAudioSourceConfig &config)
{
    const int32_t bitRate = 64000;
    const int32_t sampleRate = 48000;

    config.inputSource = AUDIO_MIC;
    config.audioFormat = AAC_LC;
    config.sampleRate = sampleRate;
    config.channelCount = 1;
    config.bitRate = bitRate;
    config.streamType = TYPE_MEDIA;
    config.bitWidth = BIT_WIDTH_16;
}

RecorderImpl::RecorderImpl()
{
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        sourceManager_[i].videoSource = nullptr;
        sourceManager_[i].videoSourceStarted = false;
        sourceManager_[i].videoSourcePaused = false;
        sourceManager_[i].videoTrackId = -1;
        sourceManager_[i].audioSource = nullptr;
        sourceManager_[i].audioSourceStarted = false;
        sourceManager_[i].audioSourcePaused = false;
        sourceManager_[i].audioTrackId = -1;
        sourceManager_[i].dataSource = nullptr;
        sourceManager_[i].dataSourceStarted = false;
        sourceManager_[i].dataSourcePaused = false;
        sourceManager_[i].dataTrackId = -1;

        SetDefaultVideoConfig(sourceManager_[i].videoSourceConfig);
        SetDefaultAudioConfig(sourceManager_[i].audioSourceConfig);
        sourceManager_[i].dataSourceConfig = {};
    }
    recorderSink_ = new(std::nothrow) RecorderSink();
    if (recorderSink_ != nullptr) {
        status_ = INITIALIZED;
    }
    MEDIA_INFO_LOG("ctor status_:%d", status_);
}

RecorderImpl::~RecorderImpl()
{
    if (status_ != RELEASED) {
        (void)Release();
    }
    ResetConfig();
    if (recorderSink_ != nullptr) {
        delete recorderSink_;
        recorderSink_ = nullptr;
    }
    MEDIA_INFO_LOG("RecorderImpl dctor");
}

int32_t RecorderImpl::ResetConfig()
{
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        sourceManager_[i].videoSourceStarted = false;
        sourceManager_[i].videoSourcePaused = false;
        sourceManager_[i].videoTrackId = -1;
        sourceManager_[i].audioSourceStarted = false;
        sourceManager_[i].audioSourcePaused = false;
        sourceManager_[i].audioTrackId = -1;
        sourceManager_[i].dataSourceStarted = false;
        sourceManager_[i].dataSourcePaused = false;
        sourceManager_[i].dataTrackId = -1;
        if (sourceManager_[i].videoSource != nullptr) {
            delete sourceManager_[i].videoSource;
            sourceManager_[i].videoSource = nullptr;
        }
        if (sourceManager_[i].audioSource != nullptr) {
            delete sourceManager_[i].audioSource;
            sourceManager_[i].audioSource = nullptr;
        }
        if (sourceManager_[i].dataSource != nullptr) {
            delete sourceManager_[i].dataSource;
            sourceManager_[i].dataSource = nullptr;
        }
        SetDefaultVideoConfig(sourceManager_[i].videoSourceConfig);
        SetDefaultAudioConfig(sourceManager_[i].audioSourceConfig);

        if (memset_s(&sourceManager_[i].dataSourceConfig, sizeof(RecorderDataSourceConfig),
            0x00, sizeof(RecorderDataSourceConfig)) != EOK) {
            MEDIA_ERR_LOG("memset dataSourceConfig failed");
            return ERR_UNKNOWN;
        }
    }
    MEDIA_INFO_LOG("ResetConfig SUCCESS");
    return SUCCESS;
}

int32_t RecorderImpl::GetFreeVideoSourceID(int32_t &sourceId, uint32_t &freeIndex)
{
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        if (sourceManager_[i].videoSource == nullptr) {
            sourceId = static_cast<int32_t>(RECORDER_VIDEO_SOURCE_ID_MASK + i);
            freeIndex = i;
            return SUCCESS;
        }
    }
    MEDIA_ERR_LOG("get free video sourceId failed");
    return ERROR;
}

int32_t RecorderImpl::GetFreeAudioSourceID(int32_t &sourceId, uint32_t &freeIndex)
{
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        if (sourceManager_[i].audioSource == nullptr) {
            sourceId = static_cast<int32_t>(RECORDER_AUDIO_SOURCE_ID_MASK + i);
            freeIndex = i;
            return SUCCESS;
        }
    }
    MEDIA_ERR_LOG("get free Audio sourceId failed");
    return ERROR;
}

int32_t RecorderImpl::GetFreeDataSourceID(int32_t &sourceId, uint32_t &freeIndex)
{
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        if (sourceManager_[i].dataSource == nullptr) {
            sourceId = static_cast<int32_t>(RECORDER_DATA_SOURCE_ID_MASK + i);
            freeIndex = i;
            return SUCCESS;
        }
    }
    MEDIA_ERR_LOG("get free Data sourceId failed");
    return ERROR;
}

bool RecorderImpl::GetIndexBySourceID(int32_t sourceId, uint32_t &validIndex)
{
    uint32_t index;
    if (sourceId >= static_cast<int32_t>(RECORDER_DATA_SOURCE_ID_MASK)) { /* source type is data. */
        index = static_cast<uint32_t>(sourceId) - RECORDER_DATA_SOURCE_ID_MASK;
    } else if (sourceId >= static_cast<int32_t>(RECORDER_AUDIO_SOURCE_ID_MASK)) { /* source type is audio. */
        index = static_cast<uint32_t>(sourceId) - RECORDER_AUDIO_SOURCE_ID_MASK;
    } else { /* source type is VIDEO. */
        index = static_cast<uint32_t>(sourceId) - RECORDER_VIDEO_SOURCE_ID_MASK;
    }

    if (index >= RECORDER_SOURCE_MAX_CNT) {
        MEDIA_ERR_LOG("InValidSourceID sourceId:%x", sourceId);
        return false;
    }

    if (sourceManager_[index].videoSource != nullptr) {
        validIndex = index;
        return true;
    } else if (sourceManager_[index].audioSource != nullptr) {
        validIndex = index;
        return true;
    } else if (sourceManager_[index].dataSource != nullptr) {
        validIndex = index;
        return true;
    } else {
        validIndex = 0xffffffff; /* INVALID_SOURCE_INDEX */
        MEDIA_ERR_LOG("InValid source type.");
    }

    MEDIA_ERR_LOG("IsValidSourceID sourceId:%x", sourceId);
    return false;
}

int32_t RecorderImpl::SetVideoSource(VideoSourceType source, int32_t &sourceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    if (source < VIDEO_SOURCE_SURFACE_YUV || source >= VIDEO_SOURCE_BUTT) {
        MEDIA_ERR_LOG("only support VIDEO_SOURCE_SURFACE_ES source: %x is invalid", source);
        return ERR_INVALID_PARAM;
    }
    uint32_t freeIndex;
    int32_t ret = GetFreeVideoSourceID(sourceId, freeIndex);
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("GetFreeVideoSourceID  failed Ret: %d", ERR_NOFREE_CHANNEL);
        return ERR_NOFREE_CHANNEL;
    }

    sourceManager_[freeIndex].videoSource = new(std::nothrow) RecorderVideoSource();
    if (sourceManager_[freeIndex].videoSource == nullptr) {
        ret = ERR_UNKNOWN;
    }
    MEDIA_INFO_LOG("Video Source :%d Set SUCCESS sourceId:%x, ret:%d", source, sourceId, ret);
    return ret;
}

int32_t RecorderImpl::SetVideoEncoder(int32_t sourceId, VideoCodecFormat encoder)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    uint32_t validIndex;
    if (!GetIndexBySourceID(sourceId, validIndex)) {
        MEDIA_ERR_LOG("input sourceId:%x is invalid", sourceId);
        return ERR_INVALID_PARAM;
    }
    if (encoder != VIDEO_DEFAULT && encoder != H264 && encoder != HEVC) {
        MEDIA_ERR_LOG("input VideoCodecFormat : %d is invalid", encoder);
        return ERR_INVALID_PARAM;
    }
    sourceManager_[validIndex].videoSourceConfig.videoFormat = encoder;
    MEDIA_INFO_LOG("Video Encoder:%d Set SUCCESS", encoder);
    return SUCCESS;
}

int32_t RecorderImpl::SetVideoSize(int32_t sourceId, int32_t width, int32_t height)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    uint32_t validIndex;
    if (!GetIndexBySourceID(sourceId, validIndex)) {
        MEDIA_ERR_LOG("input sourceId: %x is invalid", sourceId);
        return ERR_INVALID_PARAM;
    }
    if (width <= 0 || height <= 0) {
        MEDIA_ERR_LOG("input VideoSize width:%d height:%d is invalid", width, height);
        return ERR_INVALID_PARAM;
    }
    sourceManager_[validIndex].videoSourceConfig.width = width;
    sourceManager_[validIndex].videoSourceConfig.height = height;
    int ret = sourceManager_[validIndex].videoSource->SetSurfaceSize(width, height);
    MEDIA_INFO_LOG("Video Size width:%d height:%d", width, height);
    return ret;
}
int32_t RecorderImpl::SetVideoFrameRate(int32_t sourceId, int32_t frameRate)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    uint32_t validIndex;
    if (!GetIndexBySourceID(sourceId, validIndex)) {
        MEDIA_ERR_LOG("input sourceId : %x is invalid", sourceId);
        return ERR_INVALID_PARAM;
    }
    if (frameRate <= 0) {
        MEDIA_ERR_LOG("input frameRate %d is invalid", frameRate);
        return ERR_INVALID_PARAM;
    }
    sourceManager_[validIndex].videoSourceConfig.frameRate = frameRate;
    MEDIA_INFO_LOG("Video frameRate:%d ", frameRate);
    return SUCCESS;
}

int32_t RecorderImpl::SetVideoEncodingBitRate(int32_t sourceId, int32_t rate)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    uint32_t validIndex;
    if (!GetIndexBySourceID(sourceId, validIndex)) {
        MEDIA_ERR_LOG("input sourceId: %x is invalid", sourceId);
        return ERR_INVALID_PARAM;
    }
    if (rate <= 0) {
        MEDIA_ERR_LOG("input rate %d is invalid", rate);
        return ERR_INVALID_PARAM;
    }
    sourceManager_[validIndex].videoSourceConfig.bitRate = rate;
    MEDIA_INFO_LOG("Video Encoding BitRate:%d ", rate);
    return SUCCESS;
}

int32_t RecorderImpl::SetCaptureRate(int32_t sourceId, double fps)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    uint32_t validIndex;
    if (!GetIndexBySourceID(sourceId, validIndex)) {
        MEDIA_ERR_LOG("input sourceId: %x is invalid", sourceId);
        return ERR_INVALID_PARAM;
    }
    if (fps <= 0.0) {
        MEDIA_ERR_LOG("input rate %lf is invalid", fps);
        return ERR_INVALID_PARAM;
    }
    sourceManager_[validIndex].videoSourceConfig.captureRate = fps;
    MEDIA_INFO_LOG("Video Capture Rate:%lf ", fps);
    return SUCCESS;
}

int32_t RecorderImpl::SetOrientationHint(int32_t sourceId, int32_t degree)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    uint32_t validIndex;
    if (!GetIndexBySourceID(sourceId, validIndex)) {
        MEDIA_ERR_LOG("input sourceId: %x is invalid", sourceId);
        return ERR_INVALID_PARAM;
    }
    if (degree <= 0) {
        MEDIA_ERR_LOG("input rate %d is invalid", degree);
        return ERR_INVALID_PARAM;
    }
    sourceManager_[validIndex].videoSourceConfig.degree = degree;
    return SUCCESS;
}

std::shared_ptr<OHOS::Surface> RecorderImpl::GetSurface(int32_t sourceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    uint32_t validIndex;
    if (!GetIndexBySourceID(sourceId, validIndex)) {
        MEDIA_ERR_LOG("input sourceId: %x is invalid", sourceId);
        return nullptr;
    }

    if (sourceId >= static_cast<int32_t>(RECORDER_DATA_SOURCE_ID_MASK)) { /* source type is data. */
        return sourceManager_[validIndex].dataSource->GetSurface();
    } else { /* source type is VIDEO. */
        return sourceManager_[validIndex].videoSource->GetSurface();
    }
}

bool RecorderImpl::IsValidAudioSource(AudioSourceType source)
{
    if (source <= AUDIO_SOURCE_INVALID || source > AUDIO_VOICE_PERFORMANCE) {
        MEDIA_ERR_LOG("input AudioSourceType : %d is invalid", source);
        return false;
    }
    return true;
}

bool RecorderImpl::IsPrepared()
{
    if (status_ != INITIALIZED) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return true;
    }
    return false;
}

int32_t RecorderImpl::SetAudioSource(AudioSourceType source, int32_t &sourceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    uint32_t freeIndex;
    int32_t ret = GetFreeAudioSourceID(sourceId, freeIndex);
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("GetFreeAudioSourceID failed Ret: %d", ERR_NOFREE_CHANNEL);
        return ERR_NOFREE_CHANNEL;
    }
    if (!IsValidAudioSource(source)) {
        return ERR_INVALID_PARAM;
    }
    sourceManager_[freeIndex].audioSource = new(std::nothrow) RecorderAudioSource();
    if (sourceManager_[freeIndex].audioSource == nullptr) {
        MEDIA_INFO_LOG("new RecorderAudioSource failed, Source :%d sourceId:%x", source, sourceId);
        return ERR_UNKNOWN;
    }
    sourceManager_[freeIndex].audioSourceConfig.inputSource = source;

    MEDIA_INFO_LOG("Audio Source :%d Set sourceId:%x SUCCESS", source, sourceId);
    return SUCCESS;
}

int32_t RecorderImpl::SetAudioEncoder(int32_t sourceId, AudioCodecFormat encoder)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    uint32_t validIndex;
    if (!GetIndexBySourceID(sourceId, validIndex)) {
        MEDIA_ERR_LOG("input sourceId: %x is invalid", sourceId);
        return ERR_INVALID_PARAM;
    }
    if (encoder >= FORMAT_BUTT || encoder <= AUDIO_DEFAULT) {
        MEDIA_ERR_LOG("input AudioCodecFormat:%d is invalid", encoder);
        return ERR_INVALID_PARAM;
    }

    AudioCodecFormat audioFormat;
    switch (encoder) {
        case AAC_LC:
        case AAC_HE_V1:
        case AAC_HE_V2:
        case AAC_LD:
        case AAC_ELD:
            audioFormat = encoder;
            break;
        case AUDIO_DEFAULT:
            audioFormat = AAC_LC;
            break;
        default:
            MEDIA_ERR_LOG("input AudioCodecFormat:%d is invalid", encoder);
            return ERR_INVALID_PARAM;
    }
    sourceManager_[validIndex].audioSourceConfig.audioFormat = audioFormat;
    MEDIA_INFO_LOG("Set audio encoder:%d success", encoder);
    return SUCCESS;
}

int32_t RecorderImpl::SetAudioSampleRate(int32_t sourceId, int32_t rate)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    uint32_t validIndex;
    if (!GetIndexBySourceID(sourceId, validIndex)) {
        MEDIA_ERR_LOG("input sourceId : %x is invalid", sourceId);
        return ERR_INVALID_PARAM;
    }
    if (rate <= 0) {
        MEDIA_ERR_LOG("input AudioSampleRate %d is invalid", rate);
        return ERR_INVALID_PARAM;
    }
    sourceManager_[validIndex].audioSourceConfig.sampleRate = rate;
    MEDIA_INFO_LOG("Audio Sample Rate :%d Set", rate);
    return SUCCESS;
}

int32_t RecorderImpl::SetAudioChannels(int32_t sourceId, int32_t num)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    uint32_t validIndex;
    if (!GetIndexBySourceID(sourceId, validIndex)) {
        MEDIA_ERR_LOG("input sourceId : %x is invalid", sourceId);
        return ERR_INVALID_PARAM;
    }
    if (num <= 0) {
        MEDIA_ERR_LOG("input AudioChannels %d is invalid", num);
        return ERR_INVALID_PARAM;
    }
    sourceManager_[validIndex].audioSourceConfig.channelCount = num;
    MEDIA_INFO_LOG("Audio Channels :%d Set", num);
    return SUCCESS;
}

int32_t RecorderImpl::SetAudioEncodingBitRate(int32_t sourceId, int32_t bitRate)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    uint32_t validIndex;
    if (!GetIndexBySourceID(sourceId, validIndex)) {
        MEDIA_ERR_LOG("input sourceId : %x is invalid", sourceId);
        return ERR_INVALID_PARAM;
    }
    if (bitRate <= 0) {
        MEDIA_ERR_LOG("input AudioEncodingBitRate %d is invalid", bitRate);
        return ERR_INVALID_PARAM;
    }
    sourceManager_[validIndex].audioSourceConfig.bitRate = bitRate;
    MEDIA_INFO_LOG("Audio Encoding BitRate :%d Set", bitRate);
    return SUCCESS;
}

int32_t RecorderImpl::SetDataSource(DataSourceType source, int32_t &sourceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }

    if (source > METADATA) {
        MEDIA_ERR_LOG("dataSource source = %d is invalid", source);
        return ERR_INVALID_PARAM;
    }
    uint32_t freeIndex;
    int32_t ret = GetFreeDataSourceID(sourceId, freeIndex);
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("GetFreeVideoSourceID failed Ret: %d", ERR_NOFREE_CHANNEL);
        return ERR_NOFREE_CHANNEL;
    }
    sourceManager_[freeIndex].dataSource = new(std::nothrow) RecorderDataSource();
    if (sourceManager_[freeIndex].dataSource == nullptr) {
        MEDIA_INFO_LOG("new RecorderDataSource failed, Source :%d sourceId:%x", source, sourceId);
        return ERR_UNKNOWN;
    }
    sourceManager_[freeIndex].dataSourceConfig.dataType = (DataType)source;

    MEDIA_INFO_LOG("Data Source Set SUCCESS sourceId:%x", sourceId);
    return SUCCESS;
}

int32_t RecorderImpl::SetLocation(int32_t latitude, int32_t longitude)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    if (recorderSink_ == nullptr) {
        MEDIA_ERR_LOG("recorderSink_ is null");
        return ERR_UNKNOWN;
    }

    return recorderSink_->SetLocation(latitude, longitude);
}

int32_t RecorderImpl::SetMaxDuration(int32_t duration)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    if (recorderSink_ == nullptr) {
        MEDIA_ERR_LOG("recorderSink_ is null");
        return ERR_UNKNOWN;
    }

    MEDIA_INFO_LOG("Max Duration :%d Set", duration);
    return recorderSink_->SetMaxDuration(duration);
}

int32_t RecorderImpl::SetOutputFormat(OutputFormatType format)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MEDIA_ERR_LOG("in");
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    MEDIA_INFO_LOG("Output Format :%d Set", format);
    OutputFormat outPutFormat;
    switch (format) {
        case FORMAT_MPEG_4:
            outPutFormat = OUTPUT_FORMAT_MPEG_4;
            break;
        case FORMAT_TS:
            outPutFormat = OUTPUT_FORMAT_TS;
            break;
        case FORMAT_DEFAULT:
            outPutFormat = OUTPUT_FORMAT_MPEG_4;
            MEDIA_WARNING_LOG("format: %d use default OUTPUT_FORMAT_MPEG_4", format);
            break;
        default:
            MEDIA_ERR_LOG("invalid OutputFormatType: %d ", format);
            return ERR_INVALID_PARAM;
    }
    if (recorderSink_ == nullptr) {
        MEDIA_ERR_LOG("recorderSink_ is null");
        return ERR_UNKNOWN;
    }

    MEDIA_INFO_LOG("Output Format:%d Set", format);
    return recorderSink_->SetOutputFormat(outPutFormat);
}

int32_t RecorderImpl::SetOutputPath(const string &path)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (recorderSink_ == nullptr) {
        MEDIA_ERR_LOG("recorderSink_ is null");
        return ERR_UNKNOWN;
    }

    MEDIA_ERR_LOG("in");
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    if (path.data() == nullptr) {
        MEDIA_ERR_LOG("The path is nullptr");
        return ERR_INVALID_PARAM;
    }
    if (access(path.c_str(), F_OK) == -1) {
        MEDIA_ERR_LOG("The path :%s doesn't exisit", path.c_str());
        return ERR_INVALID_PARAM;
    }
    struct stat fileStatus;
    stat(path.c_str(), &fileStatus);
    if (!S_ISDIR(fileStatus.st_mode)) {
        MEDIA_ERR_LOG("The path :%s is a file!", path.c_str());
        return ERR_INVALID_PARAM;
    }
    if (access(path.c_str(),  W_OK) == -1) {
        MEDIA_ERR_LOG("The path :%s No write permission", path.c_str());
        return ERR_INVALID_PARAM;
    }
    recorderSink_->SetOutputPath(path);
    return SUCCESS;
}

int32_t RecorderImpl::IsValidFileFd(int32_t fd)
{
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        MEDIA_ERR_LOG("Fail to get File Status Flags err: %d", errno);
        return ERR_INVALID_OPERATION;
    }
    // fd must be in read-write mode or write-only mode.
    uint32_t flagsCheck = static_cast<uint32_t>(flags);
    if ((flagsCheck & (O_RDWR | O_WRONLY)) == 0) {
        MEDIA_ERR_LOG("File descriptor is not in read-write mode or write-only mode fd:%d flag:%x", fd, flagsCheck);
        return ERR_INVALID_OPERATION;
    }
    return SUCCESS;
}

int32_t RecorderImpl::SetOutputFile(int32_t fd)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MEDIA_INFO_LOG("Output File :%d Set", fd);
    if (status_ != INITIALIZED && status_ != STOPPED) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    if (fd < 0 || IsValidFileFd(fd) != SUCCESS) {
        MEDIA_ERR_LOG("Fail to get File Status Flags from fd: %d", fd);
        return ERR_INVALID_PARAM;
    }
    if (recorderSink_ == nullptr) {
        MEDIA_ERR_LOG("recorderSink_ is null");
        return ERR_UNKNOWN;
    }

    return recorderSink_->SetOutputFile(fd);
}

int32_t RecorderImpl::SetNextOutputFile(int32_t fd)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ == RELEASED) {
        MEDIA_ERR_LOG("already RELEASED");
        return ERR_ILLEGAL_STATE;
    }

    MEDIA_INFO_LOG("Next Output File :%d Set", fd);
    if (fd < 0 || IsValidFileFd(fd) != SUCCESS) {
        MEDIA_ERR_LOG("Fail to get File Status Flags from fd: %d", fd);
        return ERR_INVALID_PARAM;
    }
    if (recorderSink_ == nullptr) {
        MEDIA_ERR_LOG("recorderSink_ is null");
        return ERR_UNKNOWN;
    }

    return recorderSink_->SetNextOutputFile(fd);
}

int32_t RecorderImpl::SetMaxFileSize(int64_t size)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    MEDIA_INFO_LOG("Max File Size :%lld Set", size);
    if (recorderSink_ == nullptr) {
        MEDIA_ERR_LOG("recorderSink_ is null");
        return ERR_UNKNOWN;
    }

    return recorderSink_->SetMaxFileSize(size);
}

int32_t RecorderImpl::SetRecorderCallback(const std::shared_ptr<RecorderCallback> &callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ == RELEASED) {
        MEDIA_ERR_LOG("illegal status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    if (callback == nullptr || callback.get() == nullptr) {
        MEDIA_ERR_LOG("SetRecorderCallback callback is nullptr");
        return ERR_INVALID_PARAM;
    }
    MEDIA_INFO_LOG("RecorderCallback :%p Set", callback.get());
    if (recorderSink_ == nullptr) {
        MEDIA_ERR_LOG("recorderSink_ is null");
        return ERR_UNKNOWN;
    }

    return recorderSink_->SetRecorderCallback(callback);
}

int32_t RecorderImpl::PrepareAudioSource()
{
    int32_t ret = 0;
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        if (sourceManager_[i].audioSource != nullptr) {
            TrackSource trackSource;
            ret = GetAudioTrackSource(sourceManager_[i].audioSourceConfig, trackSource);
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("GetAudioTrackSource  failed Ret: 0x%x", ret);
                return ret;
            }
            ret = sourceManager_[i].audioSource->Init(sourceManager_[i].audioSourceConfig);
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("audioSource Init failed Ret: 0x%x", ret);
                return ret;
            }

            if (recorderSink_ == nullptr) {
                MEDIA_ERR_LOG("recorderSink_ is null");
                return ERR_UNKNOWN;
            }

            int32_t trackId;
            ret = recorderSink_->AddTrackSource(trackSource, trackId);
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("AddTrackSource failed Ret:0x%x", ret);
                return ret;
            }
            sourceManager_[i].audioTrackId = trackId;
            MEDIA_INFO_LOG("recorderSink_ AddTrackSource .audioTrackId :0x%x", sourceManager_[i].audioTrackId);
        }
    }
    MEDIA_INFO_LOG("SUCCESS");
    return SUCCESS;
}

int32_t RecorderImpl::PrepareDataSource()
{
    int32_t ret = 0;
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        if (sourceManager_[i].dataSource != nullptr) {
            TrackSource trackSource;
            ret = GetDataTrackSource(trackSource);
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("GetDataTrackSource failed ret:0x%x", ret);
                return ret;
            }

            if (recorderSink_ == nullptr) {
                MEDIA_ERR_LOG("recorderSink_ is null");
                return ERR_UNKNOWN;
            }

            int32_t trackId;
            ret = recorderSink_->AddTrackSource(trackSource, trackId);
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("AddTrackSource failed Ret: 0x%x", ret);
                return ret;
            }
            sourceManager_[i].dataTrackId = trackId;
            MEDIA_INFO_LOG("recorderSink_ AddTrackSource dataTrackId :0x%x", sourceManager_[i].dataTrackId);
        }
    }
    MEDIA_INFO_LOG("SUCCESS");
    return SUCCESS;
}

int32_t RecorderImpl::PrepareVideoSource()
{
    int32_t ret = 0;
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        if (sourceManager_[i].videoSource != nullptr) {
            TrackSource trackSource;
            ret = GetVideoTrackSource(sourceManager_[i].videoSourceConfig, trackSource);
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("GetVideoTrackSource failed. index:%d ret:0x%x", i, ret);
                return ret;
            }

            if (recorderSink_ == nullptr) {
                MEDIA_ERR_LOG("recorderSink_ is null");
                return ERR_UNKNOWN;
            }

            int32_t trackId;
            ret = recorderSink_->AddTrackSource(trackSource, trackId);
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("AddTrackSource failed ret:0x%x", ret);
                return ret;
            }
            sourceManager_[i].videoTrackId = trackId;
            MEDIA_INFO_LOG("recorderSink_ AddTrackSource videoTrackId :0x%x",
                           sourceManager_[i].videoTrackId);
        }
    }
    MEDIA_INFO_LOG("SUCCESS");
    return SUCCESS;
}

int32_t RecorderImpl::GetVideoTrackSource(const RecorderVideoSourceConfig &videoSourceConfig,
                                          TrackSource &trackSource)
{
    trackSource.trackSourceType = TRACK_SOURCE_TYPE_VIDEO;
    switch (videoSourceConfig.videoFormat) {
        case H264:
            trackSource.trackSourceInfo.videoInfo.codecType = CODEC_H264;
            break;
        case HEVC:
            trackSource.trackSourceInfo.videoInfo.codecType = CODEC_H265;
            break;
        default:
            MEDIA_ERR_LOG("unsupport videoFormat:%d", videoSourceConfig.videoFormat);
            return ERR_INVALID_PARAM;
    }
    if (videoSourceConfig.width <= 0 || videoSourceConfig.height <= 0 || videoSourceConfig.bitRate <= 0 ||
        videoSourceConfig.frameRate <= 0) {
        MEDIA_ERR_LOG("VideoTrackSource not prepared, width:%d, height:%d, bitRate:%d, frameRate:%d",
                      videoSourceConfig.width, videoSourceConfig.height, videoSourceConfig.bitRate,
                      videoSourceConfig.frameRate);
        return ERR_INVALID_PARAM;
    }
    trackSource.trackSourceInfo.videoInfo.width = videoSourceConfig.width;
    trackSource.trackSourceInfo.videoInfo.height = videoSourceConfig.height;
    trackSource.trackSourceInfo.videoInfo.bitRate = videoSourceConfig.bitRate;
    trackSource.trackSourceInfo.videoInfo.frameRate = videoSourceConfig.frameRate;
    trackSource.trackSourceInfo.videoInfo.keyFrameInterval = videoSourceConfig.frameRate;
    trackSource.trackSourceInfo.videoInfo.speed = videoSourceConfig.speed;
    return SUCCESS;
}

int32_t RecorderImpl::GetAudioTrackSource(const RecorderAudioSourceConfig &audioSourceConfig,
                                          TrackSource &trackSource)
{
    trackSource.trackSourceType = TRACK_SOURCE_TYPE_AUDIO;
    switch (audioSourceConfig.audioFormat) {
        case AAC_LC:
        case AAC_HE_V1:
        case AAC_HE_V2:
        case AAC_LD:
        case AAC_ELD:
        case AUDIO_DEFAULT:
            trackSource.trackSourceInfo.audioInfo.codecType = CODEC_AAC;
            break;
        default:
            MEDIA_ERR_LOG("unsupport audiFormat: %d", audioSourceConfig.audioFormat);
            return ERR_INVALID_PARAM;
    }
    if (audioSourceConfig.bitRate == 0 ||
        audioSourceConfig.sampleRate == 0 ||
        audioSourceConfig.channelCount == 0) {
        MEDIA_ERR_LOG("not set bitRate:%d sampleRate:%d channelCount:%d", audioSourceConfig.bitRate,
                      audioSourceConfig.sampleRate, audioSourceConfig.channelCount);
        return ERR_INVALID_PARAM;
    }
    trackSource.trackSourceInfo.audioInfo.sampleRate = audioSourceConfig.sampleRate;
    trackSource.trackSourceInfo.audioInfo.channelCount = audioSourceConfig.channelCount;
    switch (audioSourceConfig.bitWidth) {
        case BIT_WIDTH_8:
            trackSource.trackSourceInfo.audioInfo.sampleBitWidth = AUDIO_SAMPLE_FMT_S8;
            break;
        case BIT_WIDTH_16:
            trackSource.trackSourceInfo.audioInfo.sampleBitWidth = AUDIO_SAMPLE_FMT_S16;
            break;
        case BIT_WIDTH_24:
            trackSource.trackSourceInfo.audioInfo.sampleBitWidth = AUDIO_SAMPLE_FMT_S24;
            break;
        default:
            MEDIA_WARNING_LOG("default sampleBitWidth: %d", audioSourceConfig.bitWidth);
            trackSource.trackSourceInfo.audioInfo.sampleBitWidth = AUDIO_SAMPLE_FMT_S16;
            break;
    }
    int frame = 1024;
    trackSource.trackSourceInfo.audioInfo.samplesPerFrame = frame;
    trackSource.trackSourceInfo.audioInfo.avgBytesPerSec = audioSourceConfig.bitRate;
    return SUCCESS;
}

int32_t RecorderImpl::GetDataTrackSource(TrackSource &trackSource)
{
    const int32_t frameRate = 30;
    const int32_t bitrate = 4 * 1024;
    trackSource.trackSourceType = TRACK_SOURCE_TYPE_DATA;
    trackSource.trackSourceInfo.dataInfo.frameRate = frameRate;
    trackSource.trackSourceInfo.dataInfo.bitRate = bitrate;
    return SUCCESS;
}

int32_t RecorderImpl::PrepareRecorderSink()
{
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    if (recorderSink_ == nullptr) {
        MEDIA_ERR_LOG("recorderSink_ is null");
        return ERR_UNKNOWN;
    }

    return recorderSink_->Prepare();
}

int32_t RecorderImpl::Prepare()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsPrepared()) {
        MEDIA_ERR_LOG("IsPrepared status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }

    int32_t ret = PrepareRecorderSink();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("PrepareRecorderSink  failed Ret: %d", ret);
        return ret;
    }
    ret = PrepareVideoSource();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("PrepareVideoSource  failed Ret: %d", ret);
        return ret;
    }
    ret = PrepareAudioSource();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("PrepareAudioSource  failed Ret: %d", ret);
        return ret;
    }

    ret = PrepareDataSource();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("PrepareDataSource  failed Ret: %d", ret);
        return ret;
    }
    status_ = PREPARED;
    MEDIA_INFO_LOG("Prepare SUCCESS");
    return SUCCESS;
}

void DataSourceProcess(const SourceManager *dataSourceManager, const RecorderSink *recorderSink)
{
    MEDIA_INFO_LOG("in");
    if (dataSourceManager == nullptr || recorderSink == nullptr) {
        MEDIA_INFO_LOG("dataSourceManager or recorderSink null");
        return;
    }
    if (dataSourceManager->dataSource == nullptr) {
        MEDIA_ERR_LOG("dataSource is nullptr");
        return;
    }
    prctl(PR_SET_NAME, "DataSourceProcess", 0, 0, 0);
    struct sched_param param = {};
    pthread_attr_t attr;
    pthread_attr_getschedparam(&attr, &param);
    param.sched_priority = RECORDER_AUDIO_THREAD_PRIORITY;
    pthread_setschedparam(pthread_self(), SCHED_RR, &param);
    MEDIA_INFO_LOG("sched_priority:%d", param.sched_priority);
    while (dataSourceManager->dataSourceStarted) {
        RecorderSourceBuffer buffer;
        int32_t ret = dataSourceManager->dataSource->AcquireBuffer(buffer, true);
        if (ret != SUCCESS) {
            MEDIA_ERR_LOG("Read failed ret:0x%x", ret);
            continue;
        }
        if (!dataSourceManager->dataSourcePaused) {
            FormatFrame frameData;
            frameData.frameType = FRAME_TYPE_DATA;
            frameData.trackId = dataSourceManager->dataTrackId;
            frameData.isKeyFrame = false;
            frameData.timestampUs = buffer.timeStamp;
            frameData.data = buffer.dataAddr;
            frameData.len = buffer.dataLen;
            ret = recorderSink->WriteData(dataSourceManager->dataTrackId, frameData);
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("WriteData failed 0x%x", ret);
            }
        }
        dataSourceManager->dataSource->ReleaseBuffer(buffer);
    }
    MEDIA_DEBUG_LOG("over");
}

int32_t RecorderImpl::StartDataSource()
{
    int32_t ret = 0;
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        if (sourceManager_[i].dataSource != nullptr) {
            ret = sourceManager_[i].dataSource->Start();
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("DataSource Start failed ret:0x%x", ret);
                return ret;
            }
            sourceManager_[i].dataSourceStarted = true;
            sourceManager_[i].dataProcessThread = std::thread(DataSourceProcess, &sourceManager_[i], recorderSink_);
        }
    }
    MEDIA_INFO_LOG("SUCCESS");
    return SUCCESS;
}

int32_t RecorderImpl::StopDataSource()
{
    int32_t ret = 0;
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        if (sourceManager_[i].dataSource != nullptr) {
            MEDIA_DEBUG_LOG("dataSource->Stop");
            sourceManager_[i].dataSourceStarted = false;
            ret = sourceManager_[i].dataSource->Stop();
            MEDIA_DEBUG_LOG("dataSource->Stop out");
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("dataSource Stop failed Ret: 0x%x", ret);
                return ret;
            }
            MEDIA_DEBUG_LOG("dataProcessThread.join");
            sourceManager_[i].dataProcessThread.join();
            MEDIA_DEBUG_LOG("dataProcessThread.join out");
        }
    }
    MEDIA_DEBUG_LOG("SUCCESS");
    return SUCCESS;
}

void AudioSourceProcess(const SourceManager *audioSourceManager, const RecorderSink *recorderSink)
{
    MEDIA_INFO_LOG("audioSourceManager:%p", audioSourceManager);
    if (audioSourceManager == nullptr || recorderSink == nullptr) {
        MEDIA_ERR_LOG("audioSourceManager or recorderSink null");
        return;
    }
    if (audioSourceManager->audioSource == nullptr) {
        MEDIA_ERR_LOG("audioSource is nullptr");
        return;
    }
    prctl(PR_SET_NAME, "AudioSourceProcess", 0, 0, 0);
    struct sched_param param = {};
    pthread_attr_t attr;
    pthread_attr_getschedparam(&attr, &param);
    param.sched_priority = RECORDER_AUDIO_THREAD_PRIORITY;
    pthread_setschedparam(pthread_self(), SCHED_RR, &param);
    MEDIA_INFO_LOG("sched_priority:%d", param.sched_priority);
    while (audioSourceManager->audioSourceStarted) {
        RecorderSourceBuffer buffer;
        int32_t ret = audioSourceManager->audioSource->AcquireBuffer(buffer, false);
        if (ret != SUCCESS) {
            MEDIA_ERR_LOG("audioSource Read failed ret:0x%x", ret);
            continue;
        }
        if (!audioSourceManager->audioSourcePaused) {
            FormatFrame frameData;
            frameData.frameType = FRAME_TYPE_AUDIO;
            frameData.trackId = audioSourceManager->audioTrackId;
            frameData.isKeyFrame = false;
            frameData.timestampUs = buffer.timeStamp;
            frameData.data = buffer.dataAddr;
            frameData.len = buffer.dataLen;
            ret = recorderSink->WriteData(audioSourceManager->audioTrackId, frameData);
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("Audio WriteData failed 0x%x", ret);
            }
        }
        audioSourceManager->audioSource->ReleaseBuffer(buffer);
    }
    MEDIA_INFO_LOG("audioSourceManager:%p  over", audioSourceManager);
}

int32_t RecorderImpl::StartAudioSource()
{
    int32_t ret = 0;
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        if (sourceManager_[i].audioSource != nullptr) {
            ret = sourceManager_[i].audioSource->Start();
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("audioSource Start  failed Ret: 0x%x", ret);
                return ret;
            }
            sourceManager_[i].audioSourceStarted = true;
            sourceManager_[i].audioProcessThread = std::thread(AudioSourceProcess, &sourceManager_[i], recorderSink_);
        }
    }
    MEDIA_INFO_LOG("Start Audio Source SUCCESS");
    return SUCCESS;
}

int32_t RecorderImpl::StopAudioSource()
{
    int32_t ret = 0;
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        if (sourceManager_[i].audioSource != nullptr) {
            MEDIA_DEBUG_LOG("audioSource->Stop");
            sourceManager_[i].audioSourceStarted = false;
            ret = sourceManager_[i].audioSource->Stop();
            MEDIA_DEBUG_LOG("audioSource->Stop out");
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("audioSource Stop  failed Ret: 0x%x", ret);
                return ret;
            }
            MEDIA_DEBUG_LOG("audioProcessThread.join");
            sourceManager_[i].audioProcessThread.join();
            MEDIA_DEBUG_LOG("audioProcessThread.join out");
        }
    }
    MEDIA_INFO_LOG("Stop Audio Source SUCCESS");
    return SUCCESS;
}

void VideoSourceProcess(const SourceManager *videoSourceManager, const RecorderSink *recorderSink)
{
    MEDIA_INFO_LOG("videoSourceManager:%p", videoSourceManager);
    if (videoSourceManager == nullptr) {
        MEDIA_ERR_LOG("videoSourceManager null");
        return;
    }
    if (videoSourceManager->videoSource == nullptr || recorderSink == nullptr) {
        MEDIA_ERR_LOG("videoSource or recorderSink is nullptr");
        return;
    }
    prctl(PR_SET_NAME, "VideoSourceProcess", 0, 0, 0);
    struct sched_param param = {};
    pthread_attr_t attr;
    pthread_attr_getschedparam(&attr, &param);
    param.sched_priority = RECORDER_VIDEO_THREAD_PRIORITY;
    pthread_setschedparam(pthread_self(), SCHED_RR, &param);
    MEDIA_INFO_LOG("sched_priority:%d", param.sched_priority);
    while (videoSourceManager->videoSourceStarted) {
        RecorderSourceBuffer buffer;
        int32_t ret = videoSourceManager->videoSource->AcquireBuffer(buffer, true);
        if (ret != SUCCESS) {
            MEDIA_ERR_LOG("videoSource AcquireBuffer failed ret:0x%x", ret);
            continue;
        }
        if (!videoSourceManager->videoSourcePaused) {
            FormatFrame frameData;
            frameData.frameType = FRAME_TYPE_VIDEO;
            frameData.trackId = videoSourceManager->videoTrackId;
            frameData.isKeyFrame = buffer.keyFrameFlag;
            frameData.timestampUs = buffer.timeStamp;
            frameData.data = buffer.dataAddr;
            frameData.len = buffer.dataLen;
            ret = recorderSink->WriteData(videoSourceManager->videoTrackId, frameData);
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("video WriteData failed 0x%x", ret);
            }
        }
        videoSourceManager->videoSource->ReleaseBuffer(buffer);
    }
    MEDIA_INFO_LOG("videoSourceManager:%p over", videoSourceManager);
}

int32_t RecorderImpl::StartVideoSource()
{
    int32_t ret = 0;
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        if (sourceManager_[i].videoSource != nullptr) {
            ret = sourceManager_[i].videoSource->Start();
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("videoSource->Start  failed Ret: 0x%x", ret);
                return ret;
            }
            sourceManager_[i].videoSourceStarted = true;
            sourceManager_[i].videoProcessThread = std::thread(VideoSourceProcess, &sourceManager_[i], recorderSink_);
        }
    }
    MEDIA_INFO_LOG("SUCCESS");
    return SUCCESS;
}

int32_t RecorderImpl::StopVideoSource()
{
    int32_t ret = 0;
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        if (sourceManager_[i].videoSource != nullptr) {
            sourceManager_[i].videoSourceStarted = false;
            ret = sourceManager_[i].videoSource->Stop();
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("videoSource->Start  failed Ret: 0x%x", ret);
                return ret;
            }
            MEDIA_DEBUG_LOG("videoProcessThread.join");
            sourceManager_[i].videoProcessThread.join();
            MEDIA_DEBUG_LOG("videoProcessThread.join over");
        }
    }
    MEDIA_INFO_LOG("SUCCESS");
    return SUCCESS;
}

int32_t RecorderImpl::Start()
{
    std::lock_guard<std::mutex> lock(mutex_);
    int64_t startTime;
    int64_t startVideoTime;
    int64_t startAudioAndDataTime;
    struct timeval begin = { 0, 0 };
    struct timeval end = { 0, 0 };

    gettimeofday(&begin, nullptr);
    if (status_ != PREPARED && status_ != PAUSED && status_ != STOPPED) {
        MEDIA_ERR_LOG("Start ILLEGAL_STATE  status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }

    if (recorderSink_ == nullptr) {
        MEDIA_ERR_LOG("recorderSink_ is null");
        return ERR_UNKNOWN;
    }
    int32_t ret = recorderSink_->Start();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("recorderSink_  Start failed Ret: %d", ret);
        return ret;
    }
    gettimeofday(&end, nullptr);
    startTime = CalcDiffTimeMs(begin, end);

    gettimeofday(&begin, nullptr);
    ret = StartVideoSource();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("StartVideoSource  Start failed Ret: %d", ret);
        return ret;
    }
    gettimeofday(&end, nullptr);
    startVideoTime = CalcDiffTimeMs(begin, end);

    gettimeofday(&begin, nullptr);
    ret = StartAudioSource();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("StartAudioSource  Start failed Ret: %d", ret);
        return ret;
    }
    ret = StartDataSource();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("StartDataSource failed:%d", ret);
        return ret;
    }
    gettimeofday(&end, nullptr);
    startAudioAndDataTime = CalcDiffTimeMs(begin, end);
    MEDIA_INFO_LOG("SUCCESS. cost: startTime:%lld, startVideoTime:%lld, startAudioAndDataTime:%lld", startTime,
        startVideoTime, startAudioAndDataTime);
    status_ = RECORDING;
    return SUCCESS;
}

int32_t RecorderImpl::PauseAudioSource()
{
    int32_t ret = 0;
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        if (sourceManager_[i].audioSource != nullptr) {
            ret = sourceManager_[i].audioSource->Pause();
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("audioSource Pause  failed Ret: 0x%x", ret);
                return ret;
            }
            sourceManager_[i].audioSourcePaused = true;
        }
    }
    return SUCCESS;
}

int32_t RecorderImpl::PauseVideoSource()
{
    int32_t ret = 0;
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        if (sourceManager_[i].videoSource != nullptr) {
            ret = sourceManager_[i].videoSource->Pause();
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("videoSource->Pause  failed Ret: 0x%x", ret);
                return ret;
            }
            sourceManager_[i].videoSourcePaused = true;
        }
    }
    return SUCCESS;
}

int32_t RecorderImpl::PauseDataSource()
{
    int32_t ret = 0;
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        if (sourceManager_[i].dataSource != nullptr) {
            ret = sourceManager_[i].dataSource->Pause();
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("dataSource->Pause failed: 0x%x", ret);
                return ret;
            }
            sourceManager_[i].dataSourcePaused = true;
        }
    }
    return SUCCESS;
}

int32_t RecorderImpl::ResumeAudioSource()
{
    int32_t ret = 0;
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        if (sourceManager_[i].audioSource != nullptr) {
            ret = sourceManager_[i].audioSource->Resume();
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("audioSource Resume failed ret:0x%x", ret);
                return ret;
            }
            sourceManager_[i].audioSourcePaused = false;
        }
    }
    return SUCCESS;
}

int32_t RecorderImpl::ResumeVideoSource()
{
    int32_t ret = 0;
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        if (sourceManager_[i].videoSource != nullptr) {
            ret = sourceManager_[i].videoSource->Resume();
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("videoSource->Resume failed ret:0x%x", ret);
                return ret;
            }
            sourceManager_[i].videoSourcePaused = false;
        }
    }
    return SUCCESS;
}

int32_t RecorderImpl::ResumeDataSource()
{
    int32_t ret = 0;
    for (uint32_t i = 0; i < RECORDER_SOURCE_MAX_CNT; i++) {
        if (sourceManager_[i].dataSource != nullptr) {
            ret = sourceManager_[i].dataSource->Resume();
            if (ret != SUCCESS) {
                MEDIA_ERR_LOG("dataSource->Resume failed ret:0x%x", ret);
                return ret;
            }
            sourceManager_[i].dataSourcePaused = false;
        }
    }
    return SUCCESS;
}

int32_t RecorderImpl::Pause()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ != RECORDING) {
        MEDIA_ERR_LOG("Pause ILLEGAL_STATE status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    int32_t ret = PauseVideoSource();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("StopVideoSource Pause failed ret:%d", ret);
        return ret;
    }
    ret = PauseAudioSource();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("StopAudioSource Pause failed ret:%d", ret);
        return ret;
    }
    ret = PauseDataSource();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("StopDataSource Pause failed ret:%d", ret);
        return ret;
    }

    if (recorderSink_ == nullptr) {
        MEDIA_ERR_LOG("recorderSink_ is null");
        return ERR_UNKNOWN;
    }
    ret = recorderSink_->Stop(false);
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("recorderSink_ Stop! ret:0x%x", ret);
        return ret;
    }
    status_ = PAUSED;
    MEDIA_INFO_LOG("Pause Recorder SUCCESS");
    return SUCCESS;
}

int32_t RecorderImpl::Resume()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ != PAUSED) {
        MEDIA_ERR_LOG("Resume ILLEGAL_STATE status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    if (recorderSink_ == nullptr) {
        MEDIA_ERR_LOG("recorderSink_ is null");
        return ERR_UNKNOWN;
    }

    int32_t ret = recorderSink_->Start();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("recorderSink_ Start! ret: 0x%x", ret);
        return ret;
    }
    ret = ResumeVideoSource();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("VideoSource Pause failed ret:%d", ret);
        return ret;
    }
    ret = ResumeAudioSource();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("pAudioSource Resume failed ret:%d", ret);
        return ret;
    }
    ret = ResumeDataSource();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("pAudioSource Resume failed Ret: %d", ret);
        return ret;
    }
    status_ = RECORDING;
    MEDIA_INFO_LOG("Resume Recorder SUCCESS");
    return SUCCESS;
}

int32_t RecorderImpl::StopInternal(bool block)
{
    MEDIA_DEBUG_LOG("StopInternal");

    int64_t time;
    struct timeval begin = { 0, 0 };
    struct timeval end = { 0, 0 };

    gettimeofday(&begin, nullptr);

    int32_t ret = StopVideoSource();
    MEDIA_DEBUG_LOG("StopVideoSource");
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("StopVideoSource  Start failed ret:%d", ret);
        return ret;
    }
    gettimeofday(&end, nullptr);
    time = CalcDiffTimeMs(begin, end);
    MEDIA_INFO_LOG("StopVideoSource cost:%lld", time);
    gettimeofday(&begin, nullptr);
    MEDIA_DEBUG_LOG("StopAudioSource");
    ret = StopAudioSource();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("StopAudioSource Start failed ret:%d", ret);
        return ret;
    }
    gettimeofday(&end, nullptr);
    time = CalcDiffTimeMs(begin, end);
    MEDIA_INFO_LOG("StopAudioSource cost:%lld", time);

    ret = StopDataSource();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("StopDataSource failed ret:%d", ret);
        return ret;
    }

    if (recorderSink_ == nullptr) {
        MEDIA_ERR_LOG("recorderSink_ is null");
        return ERR_UNKNOWN;
    }
    ret = recorderSink_->Stop(block);
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("recorderSink_ Stop! ret:0x%x", ret);
        return ret;
    }
    gettimeofday(&end, nullptr);
    time = CalcDiffTimeMs(begin, end);
    MEDIA_INFO_LOG("recorderSink Stop cost:%lld", time);
    status_ = STOPPED;
    MEDIA_INFO_LOG("Stop Recorder SUCCESS");
    return SUCCESS;
}

int32_t RecorderImpl::Stop(bool block)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ != RECORDING &&
        status_ != PAUSED) {
        MEDIA_ERR_LOG("Stop ILLEGAL_STATE status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    return StopInternal(block);
}

int32_t RecorderImpl::Reset()
{
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t ret;
    if (status_ == RELEASED) {
        MEDIA_ERR_LOG("already RELEASED");
        return ERR_ILLEGAL_STATE;
    }
    if (status_ == RECORDING ||
        status_ == PAUSED) {
        if ((ret = StopInternal(false)) != SUCCESS) {
            MEDIA_ERR_LOG("Reset StopInternal err");
            return ret;
        }
    }

    if (recorderSink_ == nullptr) {
        MEDIA_ERR_LOG("recorderSink_ is null");
        return ERR_UNKNOWN;
    }
    ret = recorderSink_->Reset();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("recorderSink Reset err:0x%x", ret);
        return ret;
    }
    ret = ResetConfig();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("ResetConfig err:0x%x", ret);
        return ret;
    }
    status_ = INITIALIZED;
    MEDIA_INFO_LOG("Reset Recorder SUCCESS");
    return SUCCESS;
}

int32_t RecorderImpl::Release()
{
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t ret;
    if (status_ == RELEASED) {
        MEDIA_ERR_LOG("already Released");
        return ERR_ILLEGAL_STATE;
    }
    if (status_ == RECORDING ||
        status_ == PAUSED) {
        if ((ret = StopInternal(false)) != SUCCESS) {
            MEDIA_ERR_LOG("Release StopInternal err");
            return ret;
        }
    }

    if (recorderSink_ == nullptr) {
        MEDIA_ERR_LOG("recorderSink_ is null");
        return ERR_UNKNOWN;
    }
    ret = recorderSink_->Release();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("recorderSink_ Release failed:%d", ret);
        return ret;
    }
    status_ = RELEASED;
    MEDIA_INFO_LOG("Recorder Released");
    return SUCCESS;
}

int32_t RecorderImpl::SetFileSplitDuration(FileSplitType type, int64_t timestamp, uint32_t duration)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ != RECORDING) {
        MEDIA_ERR_LOG("ILLEGAL_STATE status:%u", status_);
        return ERR_ILLEGAL_STATE;
    }
    MEDIA_INFO_LOG("Set File Split Duration type:%d", type);
    ManualSplitType manualSplitType;
    switch (type) {
        case FILE_SPLIT_POST:
            manualSplitType = MANUAL_SPLIT_POST;
            break;
        case FILE_SPLIT_PRE:
            manualSplitType = MANUAL_SPLIT_PRE;
            break;
        case FILE_SPLIT_NORMAL:
            manualSplitType = MANUAL_SPLIT_NORMAL;
            break;
        default:
            MEDIA_ERR_LOG("unsupport FileSplitType type: %d", type);
            return ERR_INVALID_PARAM;
    }
    if (recorderSink_ == nullptr) {
        MEDIA_ERR_LOG("recorderSink_ is null");
        return ERR_UNKNOWN;
    }

    return recorderSink_->SetFileSplitDuration(manualSplitType, timestamp, duration);
}

int32_t RecorderImpl::SetParameter(int32_t sourceId, const Format &format)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ == RELEASED) {
        MEDIA_ERR_LOG("when RELEASED can not Set Parameter");
        return ERR_ILLEGAL_STATE;
    }
    if (recorderSink_ == nullptr) {
        MEDIA_ERR_LOG("recorderSink_ is null");
        return ERR_UNKNOWN;
    }

    float value;
    if (format.GetFloatValue(RECORDER_RECORD_SPEED, value)) {
        MEDIA_INFO_LOG("SetParameter RCORDER_RECORD_SPEED value = %f\n", value);
        sourceManager_[sourceId].videoSourceConfig.speed = value;
        return SUCCESS;
    }
    return recorderSink_->SetParameter(sourceId, format);
}
}  // namespace Media
}  // namespace OHOS