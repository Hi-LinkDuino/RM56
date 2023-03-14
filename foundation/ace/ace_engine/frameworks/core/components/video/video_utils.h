/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_VIDEO_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_VIDEO_UTILS_H

namespace OHOS::Ace {

enum class SeekMode {
    /* sync to keyframes before the time point. */
    SEEK_PREVIOUS_SYNC = 0,
    /* sync to keyframes after the time point. */
    SEEK_NEXT_SYNC,
    /* sync to closest keyframes. */
    SEEK_CLOSEST_SYNC,
    /* seek to frames closest the time point. */
    SEEK_CLOSEST,
};

// video playback status
enum class PlaybackStatus {
    ERROR = 0,
    IDLE,
    PREPARED,
    STARTED,
    PAUSED,
    STOPPED,
    PLAYBACK_COMPLETE,
    NONE
};

// video playback speed
enum class PlaybackSpeed {
    /* Video playback at 0.75x normal speed */
    SPEED_FORWARD_0_75_X = 0,
    /* Video playback at 1.00x normal speed */
    SPEED_FORWARD_1_00_X,
    /* Video playback at 1.25x normal speed */
    SPEED_FORWARD_1_25_X,
    /* Video playback at 1.75x normal speed */
    SPEED_FORWARD_1_75_X,
    /* Video playback at 2.00x normal speed */
    SPEED_FORWARD_2_00_X,
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_VIDEO_UTILS_H
