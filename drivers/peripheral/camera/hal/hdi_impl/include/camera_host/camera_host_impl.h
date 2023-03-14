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

#ifndef CAMERA_HOST_CAMERA_HOST_IMPL_H
#define CAMERA_HOST_CAMERA_HOST_IMPL_H

#include <map>
#include "camera_host.h"
#include "utils.h"
#include "icamera_device.h"

namespace OHOS::Camera {
class CameraDevice;
class CameraHostImpl : public CameraHost {
public:
    CamRetCode Init();
    virtual CamRetCode SetCallback(const OHOS::sptr<ICameraHostCallback> &callback) override;
    virtual CamRetCode GetCameraIds(std::vector<std::string> &cameraIds) override;
    virtual CamRetCode GetCameraAbility(const std::string &cameraId,
        std::shared_ptr<CameraAbility> &ability) override;
    virtual CamRetCode OpenCamera(const std::string &cameraId,
        const OHOS::sptr<ICameraDeviceCallback> &callback,
        OHOS::sptr<ICameraDevice> &pDevice) override;
    virtual CamRetCode SetFlashlight(const std::string &cameraId, bool &isEnable) override;

public:
    CameraHostImpl();
    virtual ~CameraHostImpl();
    CameraHostImpl(const CameraHostImpl &other) = delete;
    CameraHostImpl(CameraHostImpl &&other) = delete;
    CameraHostImpl& operator=(const CameraHostImpl &other) = delete;
    CameraHostImpl& operator=(CameraHostImpl &&other) = delete;

private:
    RetCode CameraPowerUp(const std::string &cameraId,
        const std::vector<std::string> &phyCameraIds);
    void CameraPowerDown(const std::vector<std::string> &phyCameraIds);
    RetCode CameraIdInvalid(const std::string &cameraId);
    RetCode SetFlashlight(const std::vector<std::string> &phyCameraIds,
        bool isEnable, FlashlightStatus &flashlightStatus);
    void OnCameraStatus(CameraId cameraId, CameraStatus status, const std::shared_ptr<CameraAbility> ability);

private:
    // key: cameraId, value: CameraDevice
    using CameraDeviceMap = std::map<std::string, std::shared_ptr<CameraDevice>>;
    CameraDeviceMap cameraDeviceMap_;
    OHOS::sptr<ICameraHostCallback> cameraHostCallback_;
    // to keep remote object OHOS::sptr<ICameraDevice> alive
    std::map<std::string, OHOS::sptr<ICameraDevice>> deviceBackup_ = {};
};
} // end namespace OHOS::Camera
#endif // CAMERA_HOST_CAMERA_HOST_IMPL_H
