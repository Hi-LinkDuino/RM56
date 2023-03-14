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

#ifndef CAMERA_DEVICE_DRIVER_UTILS_H
#define CAMERA_DEVICE_DRIVER_UTILS_H

#include <types.h>
#include <camera.h>
#include <device_manager_adapter.h>
#include "surface.h"

namespace OHOS::Camera {
using StreamSupported = struct _StreamSupported {
    OperationMode operationMode_;
    StreamSupportType streamSupportType_;
    std::vector<std::shared_ptr<StreamInfo>> streamInfos_;
};

using CaptureCallback = struct _CaptureCallback {
    std::function<void (int32_t captureId, const std::vector<int32_t> &streamId)> OnCaptureStarted;
    std::function<void (int32_t captureId, const std::vector<std::shared_ptr<CaptureEndedInfo>> &info)> OnCaptureEnded;
    std::function<void (int32_t captureId, const std::vector<std::shared_ptr<CaptureErrorInfo>> &info)> OnCaptureError;
    std::function<void (int32_t captureId, const std::vector<int> &streamIds)> OnFrameShutter;
};

using OperationType = enum _StreamOperationType : uint8_t {
    STREAM_BUFFER_FLUSH = 0,
    STREAM_BUFFER_CANCEL = 1,
    STREAM_BUFFER_ERROR = 2
};

using BufferClientConfig = struct _BufferClientConfig {
    OHOS::sptr<OHOS::SurfaceBuffer> buffer_;
    int32_t fence_;
    int32_t usage_;
};
}
#endif /* CAMERA_DEVICE_DRIVER_UTILS_H */
