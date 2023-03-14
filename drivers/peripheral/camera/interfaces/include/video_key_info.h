/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file video_key_info.h
 *
 * @brief Declares the key-value pairs used to record key information of a video frame.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef VIDEO_KEY_INFO_H
#define VIDEO_KEY_INFO_H

#include <string>

namespace OHOS::Camera {
/**
 * @brief Indicates the data length. The value type is int32_t.
 */
const std::string dataSize = "dataSize";
const int32_t VIDEO_KEY_INFO_DATA_SIZE = 0x01;

/**
 * @brief Indicates the timestamp, in nanoseconds. The value type is int64_t.
 */
const std::string timeStamp = "timeStamp";
const int32_t VIDEO_KEY_INFO_TIMESTAMP = 0x02;

/**
 * @brief Specifies whether the frame is a key frame.
 * The value type is int32_t. The value <b>1</b> means that the frame is a key frame, and <b>0</b> means the opposite.
 */
const std::string isKeyFrame = "isKeyFrame";
const int32_t VIDEO_KEY_INFO_IS_KEY_FRAME = 0x03;
} // end namespace OHOS::Camera
#endif
