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
 * @file icamera_device.h
 *
 * @brief Declares APIs for camera device operations.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDI_CAMERA_DEVICE_CLIENT_INF_H
#define HDI_CAMERA_DEVICE_CLIENT_INF_H

#include <list>
#include <map>
#include <vector>
#include "types.h"
#include "icamera_device_callback.h"
#include "istream_operator.h"
#include "istream_operator_callback.h"
#include "icamera_interface.h"

namespace OHOS::Camera {
class ICameraDevice : public ICameraInterface {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"HDI.Camera.V1_0.Device");
    virtual ~ICameraDevice() {}

    /**
     * @brief Obtains the stream operation handle.
     *
     * @param callback Indicates a stream callback. For details, see {@link IStreamOperatorCallback}.
     * {@link OnCaptureStarted} and {@link OnCaptureEnded} are used to report the start and end of capture,
     * and {@link OnCaptureError} is used to report a capture error.
     *
     * @param streamOperator Indicates the stream operation handle.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful; returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode GetStreamOperator(
        const OHOS::sptr<IStreamOperatorCallback> &callback,
        OHOS::sptr<IStreamOperator> &streamOperator) = 0;

    /**
     * @brief Updates camera device control parameters.
     *
     * @param settings Indicates the camera parameters, including the sensor frame rate and 3A parameters. 3A stands for automatic focus (AF), automatic exposure (AE), and automatic white-balance (​AWB).
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful; returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode UpdateSettings(const std::shared_ptr<CameraSetting> &settings) = 0;

    /**
     * @brief Sets the metadata reporting mode.
     *
     * @param mode Indicates the metadata reporting mode to set, which can be frame-by-frame reporting or reporting upon device status change. For details, see {@link ResultCallbackMode}.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful; returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode SetResultMode(const ResultCallbackMode &mode) = 0;

    /**
     * @brief Obtains enabled metadata.
     *
     * Metadata to be reported is enabled by calling {@link EnableResult}.
     *
     * @param results Indicates all enabled metadata.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful; returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode GetEnabledResults(std::vector<MetaType> &results) = 0;

    /**
     * @brief Enables metadata reporting.
     *
     * Only metadata that is enabled can be reported by using {@link OnResult}.
     *
     * @param results Indicates the metadata for which reporting is to be enabled.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful; returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode EnableResult(const std::vector<MetaType> &results) = 0;

    /**
     * @brief Disables metadata reporting.
     *
     * After metadata reporting is disabled, the metadata is not reported by calling {@link OnResult}. To enable metadata reporting, you must call {@link EnableResult}.
     *
     * @param results Indicates the metadata for which reporting is to be disabled.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful; returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode DisableResult(const std::vector<MetaType> &results) = 0;

    /**
     * @brief Closes the camera device.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void Close() = 0;
};
}
#endif // HDI_CAMERA_DEVICE_CLIENT_INF_H