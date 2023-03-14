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
 * @file icamera_host_callback.h
 *
 * @brief Declares callbacks for status changes of cameras and flashes. The caller needs to implement the callbacks.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDI_CAMERA_HOST_CALLBACK_SERVER_H
#define HDI_CAMERA_HOST_CALLBACK_SERVER_H

#include <list>
#include <map>
#include <vector>
#include "types.h"
#include "icamera_interface.h"

namespace OHOS::Camera {
class ICameraHostCallback : public ICameraInterface {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"HDI.Camera.V1_0.HostCallback");
    virtual ~ICameraHostCallback() {}

public:
    /**
     * @brief Called when the camera status changes to report the latest status.
     *
     * @param cameraId Indicates the ID of the camera whose status changes.
     * @param status Indicates the latest status of the camera.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void OnCameraStatus(const std::string &cameraId, CameraStatus status) = 0;

    /**
     * @brief Called when the flash status changes to report the latest status.
     *
     * @param cameraId Indicates the ID of the camera to which the flash whose status changes is bound.
     * @param status Indicates the latest status of the flash.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void OnFlashlightStatus(const std::string &cameraId, FlashlightStatus status) = 0;

    /**
     * @brief Called when the camera event occurs.
     *
     * @param cameraId Indicates the ID of the camera to which the camera event is bound.
     * @param event Indicates the camera event type.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void OnCameraEvent(const std::string &cameraId, CameraEvent event) = 0;
};
}
#endif // HDI_CAMERA_HOST_CALLBACK_SERVER_H