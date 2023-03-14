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

#ifndef CAMERA_HOST_CONFIG_MANAGER_H
#define CAMERA_HOST_CONFIG_MANAGER_H

#include <vector>
#include <map>
#include <list>
#include "utils.h"

namespace OHOS::Camera {
class HcsDeal;
class CameraHostConfig {
public:
    static CameraHostConfig *GetInstance();
    RetCode ReadConfigFile();
    RetCode GetCameraIds(std::vector<std::string> &cameraIds);
    RetCode GetPhysicCameraIds(const std::string &lCameraId, std::vector<std::string> &pCameraIds);
    RetCode GetCameraAbility(const std::string &cameraId, std::shared_ptr<CameraAbility> &ability);
    RetCode AddCameraId(const std::string &logicalCameraId,
        const std::vector<std::string> &physicalCameraIds, const std::shared_ptr<CameraAbility> ability);
    std::string SubtractCameraId(const std::vector<std::string> &physicalCameraIds);
    std::string ReturnEnableLogicalCameraId();
    std::string ReturnPhysicalCameraIdToString(const CameraId &physicalCameraId);
    std::string ReturnLogicalCameraIdToString(const std::string &physicalCameraId);

public:
    virtual ~CameraHostConfig();
    CameraHostConfig(const CameraHostConfig &other) = delete;
    CameraHostConfig(CameraHostConfig &&other) = delete;
    CameraHostConfig& operator=(const CameraHostConfig &other) = delete;
    CameraHostConfig& operator=(CameraHostConfig &&other) = delete;

public:
    // key: config cameraId; value: DeviceManager cameraId enum
    static std::map<std::string, CameraId> enumCameraIdMap_;

private:
    CameraHostConfig();
    class AutoRelease {
    public:
        AutoRelease() {};
        ~AutoRelease()
        {
            if (CameraHostConfig::instance_ != nullptr) {
                delete CameraHostConfig::instance_;
                CameraHostConfig::instance_ = nullptr;
            }
        };
    };
    static CameraHostConfig *instance_;
    static AutoRelease autoRelease_;

private:
    // key: cameraId, value: CameraAbility
    using CameraAbilityMap = std::map<std::string, std::shared_ptr<CameraAbility>>;
    CameraAbilityMap cameraAbilityMap_;
    // key: logicCameraId, value: physicsCameraIds
    using CameraIdMap = std::map<std::string, std::vector<std::string>>;
    CameraIdMap cameraIdMap_;
};
} // end namespace OHOS::Camera
#endif // CAMERA_HOST_CONFIG_MANAGER_H
