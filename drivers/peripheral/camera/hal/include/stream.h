/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef HOS_CAMERA_STREAM_H
#define HOS_CAMERA_STREAM_H

#include "ibuffer.h"
#include "camera_metadata_info.h"
#include <memory>

namespace OHOS {
namespace Camera {
using ModeMeta = std::shared_ptr<OHOS::CameraStandard::CameraMetadata>;
using CaptureMeta = std::shared_ptr<OHOS::CameraStandard::CameraMetadata>;

struct StreamConfiguration {
    int32_t id;
    int32_t type;
    int32_t width;
    int32_t height;
    int32_t format;
    uint64_t usage;
    int32_t dataspace;
    bool tunnelMode;
    int32_t minFrameDuration;
    int32_t encodeType;
    int32_t bufferCount;
    int32_t maxBatchCaptureCount;
    int32_t maxCaptureCount;
};

struct DeviceStreamSetting {
    int32_t streamId;
    uint32_t bufferCount;
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint64_t usage;
    CameraEncodeType encodeType;
    /* other settings */
};

enum DynamicStreamSwitchMode {
    DYNAMIC_STREAM_SWITCH_NOT_SUPPORT = 0,
    DYNAMIC_STREAM_SWITCH_SUPPORT,
    DYNAMIC_STREAM_SWITCH_NEED_INNER_RESTART,
};

struct FrameSpec {
    int64_t    bufferPoolId_;
    uint32_t   bufferCount_;
    std::shared_ptr<IBuffer>    buffer_;
};
} // namespace Camera
} // namespace OHOS
#endif
