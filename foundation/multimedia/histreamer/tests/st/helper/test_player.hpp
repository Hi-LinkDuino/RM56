/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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
#ifndef HISTREAMER_ST_TEST_PLAYER
#define HISTREAMER_ST_TEST_PLAYER
#include <format.h>
#include <memory>
#include <string>
#include <vector>

namespace OHOS::Media::Test {
class TestSource {
public:
    explicit TestSource(std::string url) : url_(std::move(url)) {}
    // To Fix: support stream, TestSource(stream)

    std::string url_;
};

class TestPlayer {
public:
    static std::unique_ptr<TestPlayer> Create();
    virtual ~TestPlayer() = default;
    virtual int32_t SetSource(const TestSource& source) = 0;
    virtual int32_t SetSingleLoop(bool loop) = 0;
    virtual bool IsPlaying() = 0;
    virtual int32_t Prepare() = 0;
    virtual int32_t Play() = 0;
    virtual int32_t Pause() = 0;
    virtual int32_t Stop() = 0;
    virtual int32_t Reset() = 0;
    virtual int32_t Release() = 0;
    virtual int32_t Seek(int64_t timeMs) = 0;
    virtual int32_t GetCurrentTime(int64_t& currentMs) = 0;
    virtual int32_t GetDuration(int64_t& durationMs) = 0;
    virtual int32_t SetVolume(float leftVolume, float rightVolume) = 0;
    virtual int32_t GetAudioTrackInfo(std::vector<Format> &audioTrack) = 0;
};
}
#endif