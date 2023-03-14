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
 * @file icamera_host.h
 *
 * @brief Management class of the camera service that provides Hardware Driver Interfaces (HDIs) for the upper layer.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDI_CAMERA_HOST_CLIENT_INF_H
#define HDI_CAMERA_HOST_CLIENT_INF_H

#include <list>
#include <map>
#include <vector>
#include "icamera_device_callback.h"
#include "types.h"
#include "icamera_interface.h"

namespace OHOS::Camera {
class ICameraDevice;
class ICameraHostCallback;
class ICameraDeviceCallback;
class ICameraHost : public ICameraInterface {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"HDI.Camera.V1_0.Host");
    virtual ~ICameraHost() {}
    /**
     * @brief Obtains an <b>ICameraHost</b> instance.
     *
     * This function provides the entry to the camera service. You must use this function to obtain an <b>ICameraHost</b> instance before performing other operations.
     *
     * @param serviceName Indicates the name of the <b>ICameraHost</b> instance to obtain. Currently, the name is fixed at <b>camera_service</b>.
     *
     * @return Returns the <b>ICameraHost</b> instance if the operation is successful; returns <b>nullptr</b> otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    static sptr<ICameraHost> Get(const char *serviceName);

    /**
     * @brief Sets callbacks. For details about the callbacks, see {@link ICameraHostCallback}.
     *
     * @param callback Indicates the callbacks to set.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful; returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode SetCallback(const OHOS::sptr<ICameraHostCallback> &callback) = 0;

    /**
     * @brief Obtains the IDs of available camera devices.
     *
     * @param cameraIds Indicates the IDs of available camera devices.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful; returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode GetCameraIds(std::vector<std::string> &cameraIds) = 0;

    /**
     * @brief Obtains the abilities of a camera device.
     *
     * @param cameraId Indicates the ID of the camera device, which can be obtained by calling {@link GetCameraIds}.
     *
     * @param ability Returns the abilities of the camera device.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful; returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode GetCameraAbility(const std::string &cameraId,
        std::shared_ptr<CameraAbility> &ability) = 0;

    /**
     * @brief Opens a camera device.
     *
     * By calling this function, you can obtain the <b>ICameraDevice</b> instance and operate the specific camera device mapping to the instance.
     *
     * @param cameraId Indicates the ID of the camera device, which can be obtained by calling {@link GetCameraIds}.
     * @param callback Indicates the callback related to the camera. For details, see {@link ICameraDeviceCallback}.
     * @param device Indicates the <b>ICameraDevice</b> instance corresponding to the ID of the camera device.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful; returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode OpenCamera(const std::string &cameraId,
        const OHOS::sptr<ICameraDeviceCallback> &callback,
        OHOS::sptr<ICameraDevice> &device) = 0;

    /**
     * @brief Turns on or off the flash.
     *
     * This function can be used only by the caller who has opened the camera device specified by <b>cameraId</b>.
     *
     * @param cameraId Indicates the ID of the camera whose flash is to be turned on or off.
     * @param isEnable Specifies whether to turn on or off the flash. The value <b>true</b> means to turn on the flash, and <b>false</b> means the opposite.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful; returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode SetFlashlight(const std::string &cameraId, bool &isEnable) = 0;
};
}
#endif // HDI_CAMERA_HOST_CLIENT_INF_H