/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "media_log.h"

namespace OHOS {
namespace Media {
Recorder::RecorderClient::RecorderClient()
{
    impl_ = new (nothrow) RecorderImpl();
}

Recorder::RecorderClient::~RecorderClient()
{
    if (impl_ != nullptr) {
        delete impl_;
        impl_ = nullptr;
    }
}

int32_t Recorder::RecorderClient::SetVideoSource(VideoSourceType source, int32_t &sourceId)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetVideoSource(source, sourceId);
}

int32_t Recorder::RecorderClient::SetVideoEncoder(int32_t sourceId, VideoCodecFormat encoder)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetVideoEncoder(sourceId, encoder);
}

int32_t Recorder::RecorderClient::SetVideoSize(int32_t sourceId, int32_t width, int32_t height)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetVideoSize(sourceId, width, height);
}

int32_t Recorder::RecorderClient::SetVideoFrameRate(int32_t sourceId, int32_t frameRate)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetVideoFrameRate(sourceId, frameRate);
}

int32_t Recorder::RecorderClient::SetVideoEncodingBitRate(int32_t sourceId, int32_t rate)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetVideoEncodingBitRate(sourceId, rate);
}

int32_t Recorder::RecorderClient::SetCaptureRate(int32_t sourceId, double fps)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetCaptureRate(sourceId, fps);
}

std::shared_ptr<OHOS::Surface> Recorder::RecorderClient::GetSurface(int32_t sourceId)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return nullptr;
    }
    return impl_->GetSurface(sourceId);
}

int32_t Recorder::RecorderClient::SetAudioSource(AudioSourceType source, int32_t &sourceId)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetAudioSource(source, sourceId);
}

int32_t Recorder::RecorderClient::SetAudioEncoder(int32_t sourceId, AudioCodecFormat encoder)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetAudioEncoder(sourceId, encoder);
}

int32_t Recorder::RecorderClient::SetAudioSampleRate(int32_t sourceId, int32_t rate)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetAudioSampleRate(sourceId, rate);
}

int32_t Recorder::RecorderClient::SetAudioChannels(int32_t sourceId, int32_t num)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetAudioChannels(sourceId, num);
}

int32_t Recorder::RecorderClient::SetAudioEncodingBitRate(int32_t sourceId, int32_t bitRate)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetAudioEncodingBitRate(sourceId, bitRate);
}

int32_t Recorder::RecorderClient::SetDataSource(DataSourceType source, int32_t &sourceId)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetDataSource(source, sourceId);
}

int32_t Recorder::RecorderClient::SetMaxDuration(int32_t duration)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetMaxDuration(duration);
}

int32_t Recorder::RecorderClient::SetOutputFormat(OutputFormatType format)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetOutputFormat(format);
}

int32_t Recorder::RecorderClient::SetOutputPath(const string &path)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetOutputPath(path);
}

int32_t Recorder::RecorderClient::SetOutputFile(int32_t fd)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetOutputFile(fd);
}

int32_t Recorder::RecorderClient::SetNextOutputFile(int32_t fd)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetNextOutputFile(fd);
}

int32_t Recorder::RecorderClient::SetMaxFileSize(int64_t size)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetMaxFileSize(size);
}

int32_t Recorder::RecorderClient::SetRecorderCallback(const std::shared_ptr<RecorderCallback> &callback)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetRecorderCallback(callback);
}

int32_t Recorder::RecorderClient::Prepare()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->Prepare();
}

int32_t Recorder::RecorderClient::Start()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->Start();
}

int32_t Recorder::RecorderClient::Pause()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->Pause();
}

int32_t Recorder::RecorderClient::Resume()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->Resume();
}

int32_t Recorder::RecorderClient::Stop(bool block)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->Stop(block);
}

int32_t Recorder::RecorderClient::Reset()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->Reset();
}

int32_t Recorder::RecorderClient::Release()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->Release();
}

int32_t Recorder::RecorderClient::SetFileSplitDuration(FileSplitType type, int64_t timestamp, uint32_t duration)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetFileSplitDuration(type, timestamp, duration);
}

int32_t Recorder::RecorderClient::SetParameter(int32_t sourceId, const Format &format)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("No operation object");
        return -1;
    }
    return impl_->SetParameter(sourceId, format);
}
} /* namespace Media */
} /* namespace OHOS */