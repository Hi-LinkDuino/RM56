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

/**
 * @file istream_operator_callback.h
 *
 * @brief Declares callbacks related to {@link IStreamOperator}. The caller needs to implement these callbacks.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDI_STREAM_OPERATOR_CALLBACK_SERVER_H
#define HDI_STREAM_OPERATOR_CALLBACK_SERVER_H

#include <list>
#include <map>
#include <vector>
#include "types.h"
#include "icamera_interface.h"

namespace OHOS::Camera {
class IStreamOperatorCallback : public ICameraInterface {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"HDI.Camera.V1_0.StreamOperatorCallback");
    virtual ~IStreamOperatorCallback() {}

public:
    /**
     * @brief Called when the capture starts.
     *
     * @param captureId Indicates the ID of the capture request corresponding to the callback.
     * @param streamIds Indicates the IDs of the streams corresponding to the callback.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void OnCaptureStarted(int32_t captureId, const std::vector<int32_t> &streamIds) = 0;

    /**
     * @brief Called when the capture ends.
     *
     * @param captureId Indicates the ID of the capture request corresponding to the callback.
     * @param infos Indicates information related to the capture when it ends.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void OnCaptureEnded(int32_t captureId,
        const std::vector<std::shared_ptr<CaptureEndedInfo>> &infos) = 0;

    /**
     * @brief Called when an error occurs during the capture.
     *
     * @param captureId Indicates the ID of the capture request corresponding to the callback.
     * @param infos Indicates a list of capture error messages.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void OnCaptureError(int32_t captureId,
        const std::vector<std::shared_ptr<CaptureErrorInfo>> &infos) = 0;

    /**
     * @brief Called when a frame is captured.
     *
     * This callback is enabled by using <b>enableShutterCallback_</b> in the {@link CaptureInfo} parameter of {@link Capture}.
     * When <b>enableShutterCallback_</b> is set to <b>true</b>,
     * this callback is triggered each time a frame is captured.
     *
     * @param captureId Indicates the ID of the capture request corresponding to the callback.
     * @param streamIds Indicates the IDs of the streams corresponding to the callback.
     * @param timestamp Indicates the timestamp when the callback is invoked.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void OnFrameShutter(int32_t captureId,
        const std::vector<int32_t> &streamIds, uint64_t timestamp) = 0;
};
}
#endif // HDI_STREAM_OPERATOR_CALLBACK_SERVER_H
