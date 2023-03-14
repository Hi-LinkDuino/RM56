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

#ifndef CAMERA_HOST_HCS_DEAL_H
#define CAMERA_HOST_HCS_DEAL_H

#include <string>
#include <list>
#include <map>
#include "utils.h"
#include "camera_metadata_info.h"
#include "device_resource_if.h"

namespace OHOS::Camera {
class HcsDeal {
using CameraIdMap = std::map<std::string, std::vector<std::string>>;
using CameraMetadataMap = std::map<std::string, std::shared_ptr<CameraStandard::CameraMetadata>>;
public:
    HcsDeal(const std::string &pathName);
    virtual ~HcsDeal();
    HcsDeal(const HcsDeal &other) = delete;
    HcsDeal(HcsDeal &&other) = delete;
    HcsDeal& operator=(const HcsDeal &other) = delete;
    HcsDeal& operator=(HcsDeal &&other) = delete;

public:
    void SetHcsPathName(const std::string &pathName);
    RetCode Init();
    RetCode GetMetadata(CameraMetadataMap &metadataMap) const;
    RetCode GetCameraId(CameraIdMap &cameraIdMap) const;

private:
    RetCode DealHcsData();
    void ChangeToMetadata();
    RetCode DealCameraAbility(const struct DeviceResourceNode &node);
    RetCode DealPhysicsCameraId(const struct DeviceResourceNode &node, std::vector<std::string> &cameraIds);
    RetCode DealMetadata(const std::string &cameraId, const struct DeviceResourceNode &node);

    RetCode DealAeAvailableAntiBandingModes(
        const struct DeviceResourceNode &metadataNode,
        std::shared_ptr<CameraStandard::CameraMetadata> &metadata);
    RetCode DealAeAvailableModes(
        const struct DeviceResourceNode &metadataNode,
        std::shared_ptr<CameraStandard::CameraMetadata> &metadata);
    RetCode DealAvailableAeFpsTargets(
        const struct DeviceResourceNode &metadataNode,
        std::shared_ptr<CameraStandard::CameraMetadata> &metadata);
    RetCode DealAeCompensationRange(
        const struct DeviceResourceNode &metadataNode,
        std::shared_ptr<CameraStandard::CameraMetadata> &metadata);
    RetCode DealAeCompensationSteps(
        const struct DeviceResourceNode &metadataNode,
        std::shared_ptr<CameraStandard::CameraMetadata> &metadata);
    RetCode DealAvailableAwbModes(
        const struct DeviceResourceNode &metadataNode,
        std::shared_ptr<CameraStandard::CameraMetadata> &metadata);
    RetCode DealSensitivityRange(
        const struct DeviceResourceNode &metadataNode,
        std::shared_ptr<CameraStandard::CameraMetadata> &metadata);
    RetCode DealFaceDetectMode(
        const struct DeviceResourceNode &metadataNode,
        std::shared_ptr<CameraStandard::CameraMetadata> &metadata);
    RetCode DealAvailableResultKeys(
        const struct DeviceResourceNode &metadataNode,
        std::shared_ptr<CameraStandard::CameraMetadata> &metadata);

private:
    std::string sPathName;
    const struct DeviceResourceIface *pDevResIns;
    const struct DeviceResourceNode *pRootNode;
    CameraIdMap cameraIdMap_;
    CameraMetadataMap cameraMetadataMap_;
};
} // namespace OHOS::Camera
#endif /* CAMERA_HOST_HCS_DEAL_H */
