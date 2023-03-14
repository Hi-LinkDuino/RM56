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

#ifndef HISTREAMER_MEDIA_UTILS_H
#define HISTREAMER_MEDIA_UTILS_H

#include "i_player_engine.h"
#include "pipeline/core/error_code.h"
#include "scene/player/internal/state.h"

namespace OHOS {
namespace Media {
int TransErrorCode(ErrorCode errorCode);
PlayerStates TransStateId2PlayerState(StateId state);
Plugin::SeekMode Transform2SeekMode(PlayerSeekMode mode);
const std::string& StringnessPlayerState(PlayerStates state);
inline float TransformPlayRate2Float(PlaybackRateMode rateMode);
inline PlaybackRateMode TransformFloat2PlayRate(float rate);
}  // namespace Media
}  // namespace OHOS

#endif  // HISTREAMER_MEDIA_UTILS_H
