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

#ifndef RECORDER_CLIENT_H
#define RECORDER_CLIENT_H

#include "recorder_impl.h"

namespace OHOS {
namespace Media {
class Recorder::RecorderClient {
public:
    RecorderClient();
    ~RecorderClient();
    int32_t SetVideoSource(VideoSourceType source, int32_t &sourceId);
    int32_t SetVideoEncoder(int32_t sourceId, VideoCodecFormat encoder);
    int32_t SetVideoSize(int32_t sourceId, int32_t width, int32_t height);
    int32_t SetVideoFrameRate(int32_t sourceId, int32_t frameRate);
    int32_t SetVideoEncodingBitRate(int32_t sourceId, int32_t rate);
    int32_t SetCaptureRate(int32_t sourceId, double fps);
    std::shared_ptr<OHOS::Surface> GetSurface(int32_t sourceId);
    int32_t SetAudioSource(AudioSourceType source, int32_t &sourceId);
    int32_t SetAudioEncoder(int32_t sourceId, AudioCodecFormat encoder);
    int32_t SetAudioSampleRate(int32_t sourceId, int32_t rate);
    int32_t SetAudioChannels(int32_t sourceId, int32_t num);
    int32_t SetAudioEncodingBitRate(int32_t sourceId, int32_t bitRate);
    int32_t SetMaxDuration(int32_t duration);
    int32_t SetOutputFormat(OutputFormatType format);
    int32_t SetOutputPath(const string &path);
    int32_t SetOutputFile(int32_t fd);
    int32_t SetNextOutputFile(int32_t fd);
    int32_t SetMaxFileSize(int64_t size);
    int32_t SetRecorderCallback(const std::shared_ptr<RecorderCallback> &callback);
    int32_t Prepare();
    int32_t Start();
    int32_t Pause();
    int32_t Resume();
    int32_t Stop(bool block);
    int32_t Reset();
    int32_t Release();
    int32_t SetFileSplitDuration(FileSplitType type, int64_t timestamp, uint32_t duration);
    int32_t SetParameter(int32_t sourceId, const Format &format);
    int32_t SetDataSource(DataSourceType source, int32_t &sourceId);

private:
    RecorderImpl *impl_ = nullptr;
};
} /* namespace Media */
} /* namespace OHOS */
#endif // RECORDER_CLIENT_H