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

#ifndef MULTIMEDIA_PLAYER_INTERFACE_H
#define MULTIMEDIA_PLAYER_INTERFACE_H

#include "format.h"
#include "player.h"
#include "source.h"
#ifndef SURFACE_DISABLED
#include "surface.h"
#endif

namespace OHOS {
namespace Media {
class PlayerInterface {
public:
    virtual ~PlayerInterface() = default;
    virtual int32_t SetSource(const Source &source) = 0;
    virtual int32_t Prepare() = 0;
    virtual int32_t Play() = 0;
    virtual bool IsPlaying() = 0;
    virtual int32_t Pause() = 0;
    virtual int32_t Stop() = 0;
    virtual int32_t Rewind(int64_t mSeconds, int32_t mode) = 0;
    virtual int32_t SetVolume(float leftVolume, float rightVolume) = 0;
#ifndef SURFACE_DISABLED
    virtual int32_t SetSurface(Surface *surface) = 0;
#endif
    virtual bool IsSingleLooping() = 0;
    virtual int32_t GetPlayerState(int32_t &state) = 0;
    virtual int32_t GetCurrentPosition(int64_t &currentPosition) = 0;
    virtual int32_t GetDuration(int64_t &duration) = 0;
    virtual int32_t GetVideoWidth(int32_t &videoWidth) = 0;
    virtual int32_t GetVideoHeight(int32_t &videoHeight) = 0;
    virtual int32_t SetPlaybackSpeed(float speed) = 0;
    virtual int32_t GetPlaybackSpeed(float &speed) = 0;
    virtual int32_t SetAudioStreamType(int32_t type) = 0;
    virtual void GetAudioStreamType(int32_t &type) = 0;
    virtual int32_t Reset(void) = 0;
    virtual int32_t Release() = 0;
    virtual int32_t SetLoop(bool loop) = 0;
    virtual void SetPlayerCallback(const std::shared_ptr<PlayerCallback> &cb) = 0;
    virtual int32_t Init(void) = 0;
    virtual int32_t DeInit(void) = 0;
    virtual int32_t SetParameter(const Format &params) = 0;
};
} // namespace Media
} // namespace OHOS
#endif // MULTIMEDIA_PLAYER_INTERFACE_H
