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

#include "recorder_client.h"

#include "media_log.h"
#include "pms_interface.h"
#include "securec.h"

namespace OHOS {
namespace Media {
Recorder::Recorder() : client_(new (std::nothrow) RecorderClient()) {}

Recorder::~Recorder() {}

int32_t Recorder::SetVideoSource(VideoSourceType source, int32_t &sourceId)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetVideoSource(source, sourceId);
}

int32_t Recorder::SetVideoEncoder(int32_t sourceId, VideoCodecFormat encoder)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetVideoEncoder(sourceId, encoder);
}

int32_t Recorder::SetVideoSize(int32_t sourceId, int32_t width, int32_t height)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetVideoSize(sourceId, width, height);
}
int32_t Recorder::SetVideoFrameRate(int32_t sourceId, int32_t frameRate)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetVideoFrameRate(sourceId, frameRate);
}

int32_t Recorder::SetVideoEncodingBitRate(int32_t sourceId, int32_t rate)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetVideoEncodingBitRate(sourceId, rate);
}

int32_t Recorder::SetCaptureRate(int32_t sourceId, double fps)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetCaptureRate(sourceId, fps);
}

std::shared_ptr<OHOS::Surface> Recorder::GetSurface(int32_t sourceId)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return nullptr;
    }
    return client_->GetSurface(sourceId);
}

int32_t Recorder::SetAudioSource(AudioSourceType source, int32_t &sourceId)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetAudioSource(source, sourceId);
}

int32_t Recorder::SetAudioEncoder(int32_t sourceId, AudioCodecFormat encoder)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetAudioEncoder(sourceId, encoder);
}

int32_t Recorder::SetAudioSampleRate(int32_t sourceId, int32_t rate)
{
    return client_->SetAudioSampleRate(sourceId, rate);
}

int32_t Recorder::SetAudioChannels(int32_t sourceId, int32_t num)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetAudioChannels(sourceId, num);
}

int32_t Recorder::SetAudioEncodingBitRate(int32_t sourceId, int32_t bitRate)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetAudioEncodingBitRate(sourceId, bitRate);
}

int32_t Recorder::SetDataSource(DataSourceType dataType, int32_t &sourceId)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetDataSource(dataType, sourceId);
}

int32_t Recorder::SetMaxDuration(int32_t duration)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetMaxDuration(duration);
}

int32_t Recorder::SetOutputFormat(OutputFormatType format)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetOutputFormat(format);
}

int32_t Recorder::SetOutputPath(const string &path)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetOutputPath(path);
}

int32_t Recorder::SetOutputFile(int32_t fd)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetOutputFile(fd);
}

int32_t Recorder::SetNextOutputFile(int32_t fd)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetNextOutputFile(fd);
}

int32_t Recorder::SetMaxFileSize(int64_t size)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetMaxFileSize(size);
}

int32_t Recorder::SetRecorderCallback(const std::shared_ptr<RecorderCallback> &callback)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetRecorderCallback(callback);
}

int32_t Recorder::Prepare()
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    if (CheckSelfPermission("ohos.permission.MICROPHONE") != GRANTED) {
        MEDIA_WARNING_LOG("Process can not access microphone.");
        return MEDIA_PERMISSION_DENIED;
    }
    if (CheckSelfPermission("ohos.permission.WRITE_MEDIA") != GRANTED) {
        MEDIA_WARNING_LOG("Process can not write media.");
        return MEDIA_PERMISSION_DENIED;
    }
    return client_->Prepare();
}

int32_t Recorder::Start()
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->Start();
}

int32_t Recorder::Pause()
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->Pause();
}

int32_t Recorder::Resume()
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->Resume();
}

int32_t Recorder::Stop(bool block)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->Stop(block);
}

int32_t Recorder::Reset()
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->Reset();
}

int32_t Recorder::Release()
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->Release();
}

int32_t Recorder::SetFileSplitDuration(FileSplitType type, int64_t timestamp, uint32_t duration)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetFileSplitDuration(type, timestamp, duration);
}

int32_t Recorder::SetParameter(int32_t sourceId, const Format &format)
{
    if (client_ == nullptr) {
        MEDIA_ERR_LOG("client_ null");
        return -1;
    }
    return client_->SetParameter(sourceId, format);
}
} /* namespace Media */
} /* namespace OHOS */
