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
 * @file icamera_device_callback.h
 *
 * @brief Declares callbacks for reporting camera device errors and metadata.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDI_CAMERA_DEVICE_CALLBACK_SERVER_H
#define HDI_CAMERA_DEVICE_CALLBACK_SERVER_H

#include <list>
#include <map>
#include <vector>
#include "types.h"
#include "icamera_interface.h"

namespace OHOS::Camera {
class ICameraDeviceCallback : public ICameraInterface {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"HDI.Camera.V1_0.DeviceCallback");
    virtual ~ICameraDeviceCallback() {}

public:
    /**
     * @brief Called when an error occurs on the camera device. The caller needs to implement this function.
     *
     * @param type Indicates the error type. For details, see {@link ErrorType}.
     * @param errorCode Indicates the error code. This parameter is not used currently.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void OnError(ErrorType type, int32_t errorCode) = 0;

    /**
     * @brief Called to report metadata related to the camera device.
     * For details about the reporting mode, see {@link SetResultMode}.
     *
     * @param timestamp Indicates the timestamp when the metadata is reported.
     * @param result Indicates the metadata reported. The reported metadata is specified by {@link EnableResult}.
     * You can call {@link GetEnabledResults} to obtain enabled metadata and
     * call {@link DisableResult} to disable metadata reporting.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void OnResult(uint64_t timestamp, const std::shared_ptr<CameraStandard::CameraMetadata> &result) = 0;
};
}
#endif // HDI_CAMERA_DEVICE_CALLBACK_SERVER_H