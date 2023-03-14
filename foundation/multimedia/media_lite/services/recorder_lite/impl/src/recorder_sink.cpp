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

#include "recorder_sink.h"
#include <unistd.h>
#include <sys/prctl.h>
#include "format_interface.h"
#include "media_log.h"
#include "securec.h"

namespace OHOS {
namespace Media {
constexpr uint32_t RECORDER_PARAMS_CNT = 2;

static int32_t SinkOnError(CALLBACK_HANDLE privateDataHandle, int32_t errorType, int32_t errorCode);
static int32_t SinkOnInfo(CALLBACK_HANDLE privateDataHandle, int32_t type, int32_t extra);

RecorderSink::RecorderSink()
    :formatMuxerHandle_(nullptr),
     prepared_(false),
     started_(false),
     outputFormat_(OUTPUT_FORMAT_INVALID),
     outputFd_(-1),
     outputNextFd_(-1),
     path_("/userdata"),
     maxFileSize_(-1),
     maxDuration_(-1)
{
    FormatInit();
    MEDIA_INFO_LOG("RecorderSink");
}

RecorderSink::~RecorderSink()
{
    threadRunning = false;
    if (threadId != 0) {
        MEDIA_INFO_LOG("message thread is still running. kill thread.");
        pthread_cancel(threadId);
    }
}

int32_t RecorderSink::CheckPrepared()
{
    if (!prepared_) {
        MEDIA_ERR_LOG("RecorderSink not prepared yet");
        return ERR_ILLEGAL_STATE;
    }
    return SUCCESS;
}

int32_t RecorderSink::CheckStarted() const
{
    if (!started_) {
        MEDIA_ERR_LOG("RecorderSink not started yet");
        return ERR_ILLEGAL_STATE;
    }
    return SUCCESS;
}

int32_t RecorderSink::SetOutputPath(const string &path)
{
    path_ = path;
    return SUCCESS;
}

int32_t RecorderSink::Prepare()
{
    if (prepared_) {
        return SUCCESS;
    }

    FormatOutputConfig outputConfig = {};
    outputConfig.format = outputFormat_;
    if (outputFd_ != -1) {
        outputConfig.type = OUTPUT_TYPE_FD;
        outputConfig.fd = outputFd_;
    } else {
        outputConfig.type = OUTPUT_TYPE_URI;
        if (memcpy_s(outputConfig.url, URL_LEN, path_.c_str(), path_.length()) != EOK) {
            MEDIA_ERR_LOG("memcpy_s failed %s", path_.c_str());
            return ERR_INVALID_PARAM;
        }
    }
    int32_t ret = FormatMuxerCreate(&formatMuxerHandle_, &outputConfig);
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("FormatMuxerCreate failed 0x%x  outputFd_:%d", ret, outputFd_);
        return ret;
    }
    prepared_ = true;

    if (maxDuration_ != -1) {
        ret = FormatMuxerSetMaxFileDuration(formatMuxerHandle_, maxDuration_);
        if (ret != SUCCESS) {
            MEDIA_ERR_LOG("FormatMuxersetMaxFileDuration failed 0x%x", ret);
        }
    }

    if (maxFileSize_ != -1) {
        ret = FormatMuxerSetMaxFileSize(formatMuxerHandle_, maxFileSize_);
        if (ret != SUCCESS) {
            MEDIA_ERR_LOG("FormatMuxersetMaxFileSize failed 0x%x", ret);
        }
    }

    sinkCallback_ = std::make_shared<FormatCallback>();
    sinkCallback_->privateDataHandle = this;
    sinkCallback_->OnError = SinkOnError;
    sinkCallback_->OnInfo = SinkOnInfo;
    if (FormatMuxerSetCallBack(formatMuxerHandle_, sinkCallback_.get()) != 0) {
        MEDIA_ERR_LOG("FormatMuxerSetCallBack failed");
    }

    return SUCCESS;
}

int32_t RecorderSink::AddTrackSource(const TrackSource &trackSource, int32_t &trackId)
{
    int32_t trackIndex = FormatMuxerAddTrack(formatMuxerHandle_, &trackSource);
    if (trackIndex < 0) {
        MEDIA_ERR_LOG("FormatMuxerAddTrack failed 0x%x", trackIndex);
        return trackIndex;
    }
    trackId = trackIndex;
    return SUCCESS;
}

int32_t RecorderSink::WriteData(int32_t trackId, FormatFrame &frameData) const
{
    if (CheckStarted() != SUCCESS) {
        MEDIA_ERR_LOG("RecorderSink writeData checkStarted failed.");
        return ERR_ILLEGAL_STATE;
    }
    return FormatMuxerWriteFrame(formatMuxerHandle_, &frameData);
}

int32_t RecorderSink::SetOutputFormat(OutputFormat format)
{
    outputFormat_ = format;
    return SUCCESS;
}

int32_t RecorderSink::SetOutputFile(int32_t fd)
{
    outputFd_ = fd;
    return SUCCESS;
}

int32_t RecorderSink::SetNextOutputFile(int32_t fd)
{
    if (CheckPrepared() != SUCCESS) {
        return ERR_ILLEGAL_STATE;
    }
    outputNextFd_ = fd;
    return FormatMuxerSetNextOutputFile(formatMuxerHandle_, fd);
}

int32_t RecorderSink::SetMaxDuration(int64_t duration)
{
    if (started_) {
        MEDIA_ERR_LOG("RecorderSink is started ,SetMaxDuration must be setted before Prepare");
        return ERR_ILLEGAL_STATE;
    }
    if (duration <= 0) {
        MEDIA_ERR_LOG("invalid  MaxDuration size:%lld", duration);
        return ERR_INVALID_PARAM;
    }
    maxDuration_ = duration;
    if (prepared_) {
        return FormatMuxerSetMaxFileDuration(formatMuxerHandle_, duration);
    }
    return SUCCESS;
}

int32_t RecorderSink::SetMaxFileSize(int64_t size)
{
    if (started_) {
        MEDIA_ERR_LOG("RecorderSink is started , SetMaxFileSize must setted before Prepare");
        return ERR_ILLEGAL_STATE;
    }
    if (size <= 0) {
        MEDIA_ERR_LOG("invalid  MaxFileSize size:%lld", size);
        return ERR_INVALID_PARAM;
    }
    maxFileSize_ = size;
    if (prepared_) {
        return FormatMuxerSetMaxFileSize(formatMuxerHandle_, size);
    }
    return SUCCESS;
}

int32_t RecorderSink::SetOrientationHint(int degrees)
{
    if (CheckPrepared() != SUCCESS) {
        return ERR_ILLEGAL_STATE;
    }
    return FormatMuxerSetOrientation(formatMuxerHandle_, degrees);
}

int32_t RecorderSink::SetLocation(int latitude, int longitude)
{
    if (CheckPrepared() != SUCCESS) {
        return ERR_ILLEGAL_STATE;
    }
    return FormatMuxerSetLocation(formatMuxerHandle_, latitude, longitude);
}

int32_t RecorderSink::SendCallbackInfo(int32_t type, int32_t extra)
{
    if (recCallBack_ == nullptr || recCallBack_.get() == nullptr) {
        MEDIA_ERR_LOG("sink: is nullptr");
        return ERR_INVALID_PARAM;
    }
    switch (type) {
        case MUXER_INFO_MAX_DURATION_APPROACHING:
        case MUXER_INFO_MAX_FILESIZE_APPROACHING:
        case MUXER_INFO_MAX_DURATION_REACHED:
        case MUXER_INFO_MAX_FILESIZE_REACHED:
        case MUXER_INFO_NEXT_OUTPUT_FILE_STARTED:
        case MUXER_INFO_FILE_SPLIT_FINISHED:
        case MUXER_INFO_FILE_START_TIME_MS:
        case MUXER_INFO_NEXT_FILE_FD_NOT_SET:
        case MUXER_INFO_NO_FRAME_DATA:
#ifndef ENABLE_PASSTHROUGH_MODE
            if (type == MUXER_INFO_NEXT_OUTPUT_FILE_STARTED && outputFd_ != -1 && outputNextFd_ != -1) {
                close(outputFd_);
                outputFd_ = outputNextFd_;
            }
#endif
            recCallBack_->OnInfo(type, extra);
            return SUCCESS;
        default:
            MEDIA_ERR_LOG("pass event type %d", type);
            return ERR_INVALID_PARAM;
    }
}

int32_t RecorderSink::SendCallbackError(int32_t errorType, int32_t errorCode)
{
    MEDIA_INFO_LOG("errorType:%d", errorType);
    if (recCallBack_ == nullptr || recCallBack_.get() == nullptr) {
        MEDIA_ERR_LOG("sink: is nullptr");
        return ERR_INVALID_PARAM;
    }
    switch (errorType) {
        case ERROR_CREATE_FILE_FAIL:
        case ERROR_WRITE_FILE_FAIL:
        case ERROR_CLOSE_FILE_FAIL:
        case ERROR_READ_DATA_ERROR:
        case ERROR_INTERNAL_OPERATION_FAIL:
        case ERROR_UNKNOWN:
            MEDIA_ERR_LOG("recorder Callback error");
            recCallBack_->OnError(errorType, errorCode);
            return SUCCESS;
        default:
            MEDIA_ERR_LOG("pass event err %d", errorType);
            return ERR_INVALID_PARAM;
    }
}

static int32_t SinkOnError(CALLBACK_HANDLE privateDataHandle, int32_t errorType, int32_t errorCode)
{
    RecorderSink *sink = reinterpret_cast<RecorderSink *>(privateDataHandle);
    if (sink == nullptr) {
        MEDIA_ERR_LOG("sink: is nullptr");
        return ERR_INVALID_PARAM;
    }
    MessageData data;
    data.event = errorType;
    data.extra = errorCode;
    data.isInfo = false;
    sink->messageQueue.push(data);
    sem_post(&sink->sem);
    return 0;
}

static int32_t SinkOnInfo(CALLBACK_HANDLE privateDataHandle, int32_t type, int32_t extra)
{
    MEDIA_INFO_LOG("type:%d", type);
    RecorderSink *sink = reinterpret_cast<RecorderSink *>(privateDataHandle);
    if (sink == nullptr) {
        MEDIA_ERR_LOG("sink: is nullptr");
        return ERR_INVALID_PARAM;
    }
    MessageData data;
    data.event = type;
    data.extra = extra;
    data.isInfo = true;
    sink->messageQueue.push(data);
    sem_post(&sink->sem);
    return 0;
}


int32_t RecorderSink::SetRecorderCallback(const std::shared_ptr<RecorderCallback> &callback)
{
    recCallBack_ = callback;
    return SUCCESS;
}

static void *MessageThread(void *arg)
{
    RecorderSink *sink = (RecorderSink *)arg;
    sink->threadRunning = true;
    MEDIA_INFO_LOG("Message thread start.");
    pthread_detach(pthread_self());
    prctl(PR_SET_NAME, "RecorderMessageThread", 0, 0, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);
    int32_t ret = sem_init(&sink->sem, 0, 0);
    if (ret != 0) {
        MEDIA_ERR_LOG("sem_init failed.");
        return nullptr;
    }

    while (sink->threadRunning) {
        sem_wait(&sink->sem);
        if (sink->messageQueue.empty()) {
            continue;
        }

        MessageData data = sink->messageQueue.front();
        sink->messageQueue.pop();
        if (data.isInfo) {
            sink->SendCallbackInfo(data.event, data.extra);
        } else {
            sink->SendCallbackError(data.event, data.extra);
        }
    }

    sem_destroy(&sink->sem);

    sink->threadId = 0;
    MEDIA_INFO_LOG("Message thread exit.");
    return nullptr;
}

int32_t RecorderSink::Start()
{
    if (CheckPrepared() != SUCCESS) {
        return ERR_ILLEGAL_STATE;
    }
    if (started_) {
        return SUCCESS;
    }

    MEDIA_INFO_LOG("FormatMuxerStart");
    if (sinkCallback_ != nullptr && sinkCallback_.get() != nullptr) {
        int32_t ret = pthread_create(&threadId, nullptr, MessageThread, this);
        if (ret != 0) {
            MEDIA_ERR_LOG("create message thread failed %d", ret);
            return -1;
        }
    }

    int32_t ret = FormatMuxerStart(formatMuxerHandle_);
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("FormatMuxerStart failed 0x%x", ret);
        return ret;
    }
    started_ = true;
    return SUCCESS;
}

void RecorderSink::CloseFd()
{
#ifndef ENABLE_PASSTHROUGH_MODE
    if (outputFd_ > 0) {
        FILE *fp = fdopen(outputFd_, "r");
        if (fp == nullptr) {
            MEDIA_ERR_LOG("fdopen failed");
            return;
        }
        fflush(fp);
        fsync(outputFd_);
        fclose(fp);
        outputFd_ = -1;
    }
    if (outputNextFd_ > 0) {
        close(outputNextFd_);
        outputNextFd_ = -1;
    }
#endif
}

int32_t RecorderSink::Stop(bool block)
{
    if (!started_) {
        CloseFd();
        MEDIA_INFO_LOG("RecorderSink is stoped or not started");
        return SUCCESS;
    }
    int32_t ret = FormatMuxerStop(formatMuxerHandle_, block);
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("FormatMuxerStop failed 0x%x", ret);
        return ret;
    }
    if (threadId != 0) {
        threadRunning = false;
        sem_post(&sem);
    }
    CloseFd();
    started_ = false;

    return SUCCESS;
}

int32_t RecorderSink::Reset()
{
    int32_t ret;
    if (started_) {
        ret = Stop(false);
        if (ret != SUCCESS) {
            MEDIA_ERR_LOG("Stop failed 0x%x", ret);
            return ret;
        }
    }
    if (prepared_) {
        ret = Release();
        if (ret != SUCCESS) {
            MEDIA_ERR_LOG("Release failed 0x%x", ret);
            return ret;
        }
    }
    CloseFd();
    prepared_ = false;
    outputFormat_ = OUTPUT_FORMAT_INVALID;
    maxFileSize_ = -1;
    maxDuration_ = -1;
    return SUCCESS;
}

int32_t RecorderSink::Release()
{
    int32_t ret;
    if (!prepared_) {
        return SUCCESS;
    }
    if (started_) {
        ret = Stop(false);
        if (ret != SUCCESS) {
            MEDIA_ERR_LOG("Stop failed 0x%x", ret);
            return ret;
        }
        started_ = false;
    }
    CloseFd();
    ret = FormatMuxerDestroy(formatMuxerHandle_);
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("FormatMuxerDestroy failed ret:%d", ret);
        return ret;
    }
    formatMuxerHandle_ = nullptr;
    prepared_ = false;
    return SUCCESS;
}

int32_t RecorderSink::SetFileSplitDuration(ManualSplitType type, int64_t timestamp, uint32_t duration)
{
    if (CheckStarted() != SUCCESS) {
        return ERR_ILLEGAL_STATE;
    }
    return FormatMuxerSetFileSplitDuration(formatMuxerHandle_, type, timestamp, duration);
}

int32_t RecorderSink::SetParameter(int32_t trackId, const Format &format)
{
    int32_t itemNum = 0;
    ParameterItem items[RECORDER_PARAMS_CNT];
    memset_s(items, sizeof(ParameterItem) * RECORDER_PARAMS_CNT, 0x00,
             sizeof(ParameterItem) * RECORDER_PARAMS_CNT);
    int32_t value;
    if (format.GetIntValue(RECORDER_PRE_CACHE_DURATION, value)) {
        items[itemNum].key = KEY_TYPE_PRE_CACHE;
        items[itemNum].value.s32Value = value;
        items[itemNum].size = sizeof(int32_t);
        itemNum++;
    } else {
        return ERR_INVALID_PARAM;
    }
    return FormatMuxerSetParameter(formatMuxerHandle_, trackId, items, itemNum);
}
}  // namespace Media
}  // namespace OHOS
