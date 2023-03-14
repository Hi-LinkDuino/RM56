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

#include "camera_host_config.h"
#include "hcs_deal.h"

namespace {
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    const std::string CONFIG_PATH_NAME = HDF_ETC_DIR"/camera/camera_host_config.hcb";
#else
    const std::string CONFIG_PATH_NAME = HDF_CONFIG_DIR"/camera_host_config.hcb";
#endif
}

namespace OHOS::Camera {
std::map<std::string, CameraId> CameraHostConfig::enumCameraIdMap_ = {
    { "CAMERA_FIRST", CAMERA_FIRST },
    { "CAMERA_SECOND", CAMERA_SECOND },
    { "CAMERA_THIRD", CAMERA_THIRD },
    { "CAMERA_FOURTH", CAMERA_FOURTH },
    { "CAMERA_FIFTH", CAMERA_FIFTH },
    { "CAMERA_SIXTH", CAMERA_SIXTH },
};
CameraHostConfig *CameraHostConfig::instance_ = nullptr;
CameraHostConfig::AutoRelease CameraHostConfig::autoRelease_;

CameraHostConfig *CameraHostConfig::GetInstance()
{
    if (instance_ == nullptr) {
        instance_ = new (std::nothrow) CameraHostConfig();
        if (instance_ != nullptr) {
            instance_->ReadConfigFile();
        }
    }
    return instance_;
}

CameraHostConfig::CameraHostConfig()
{
}

CameraHostConfig::~CameraHostConfig()
{
}

RetCode CameraHostConfig::ReadConfigFile()
{
    std::unique_ptr<HcsDeal> hcsDeal = std::make_unique<HcsDeal>(CONFIG_PATH_NAME);
    if (hcsDeal == nullptr) {
        CAMERA_LOGE("make HcsDeal failed. [pathname = %{public}s]", CONFIG_PATH_NAME.c_str());
        return RC_ERROR;
    }

    RetCode rc = hcsDeal->Init();
    if (rc != RC_OK) {
        CAMERA_LOGE("hcs deal init failed. [pathname = %{public}s]", CONFIG_PATH_NAME.c_str());
        return rc;
    }

    rc = hcsDeal->GetCameraId(cameraIdMap_);
    if (rc != RC_OK || cameraIdMap_.empty()) {
        CAMERA_LOGE("config camera id not found. [pathname = %{public}s]", CONFIG_PATH_NAME.c_str());
        return rc;
    }

    rc = hcsDeal->GetMetadata(cameraAbilityMap_);
    if (rc != RC_OK || cameraAbilityMap_.empty()) {
        CAMERA_LOGE("config camera ability not found. [pathname = %{public}s]", CONFIG_PATH_NAME.c_str());
        return rc;
    }

    return RC_OK;
}

RetCode CameraHostConfig::GetCameraIds(std::vector<std::string> &cameraIds)
{
    auto itr = cameraAbilityMap_.begin();
    for (; itr != cameraAbilityMap_.end(); itr++) {
        cameraIds.push_back(itr->first);
    }

    return RC_OK;
}

RetCode CameraHostConfig::GetPhysicCameraIds(const std::string &lCameraId, std::vector<std::string> &pCameraIds)
{
    auto itr = cameraIdMap_.find(lCameraId);
    if (itr != cameraIdMap_.end()) {
        pCameraIds = itr->second;
        return RC_OK;
    }
    return RC_ERROR;
}

RetCode CameraHostConfig::GetCameraAbility(
    const std::string &cameraId, std::shared_ptr<CameraAbility> &ability)
{
    auto itr = cameraAbilityMap_.find(cameraId);
    if (itr != cameraAbilityMap_.end()) {
        ability = itr->second;
        return RC_OK;
    }

    return RC_ERROR;
}

RetCode CameraHostConfig::AddCameraId(const std::string &logicalCameraId,
    const std::vector<std::string> &physicalCameraIds, const std::shared_ptr<CameraAbility> ability)
{
    auto itrCameraId = cameraIdMap_.find(logicalCameraId);
    auto itrCameraAbility = cameraAbilityMap_.find(logicalCameraId);
    if (itrCameraId != cameraIdMap_.end() || itrCameraAbility != cameraAbilityMap_.end()) {
        CAMERA_LOGE("logicalCameraId %{public}s Add Error", logicalCameraId.c_str());
        return RC_ERROR;
    } else {
        cameraIdMap_.insert(std::make_pair(logicalCameraId, physicalCameraIds));
        cameraAbilityMap_.insert(std::make_pair(logicalCameraId, ability));
        return RC_OK;
    }
}

std::string CameraHostConfig::SubtractCameraId(const std::vector<std::string> &physicalCameraIds)
{
    if (physicalCameraIds.size() > 1) {
         CAMERA_LOGE("physicalCameraIds %{public}d >1 Error", physicalCameraIds.size());
    }
    std::string logicalCameraId = ReturnLogicalCameraIdToString(physicalCameraIds[0]);
    if (logicalCameraId.size() == 0) {
        return std::string("");
    }
    auto itrCameraId = cameraIdMap_.find(logicalCameraId);
    auto itrCameraAbility = cameraAbilityMap_.find(logicalCameraId);
    if (itrCameraId == cameraIdMap_.end() || itrCameraAbility == cameraAbilityMap_.end()) {
        CAMERA_LOGE("physicalCameraIds %{public}s Subtract Error", physicalCameraIds[0].c_str());
        return std::string("");
    } else {
        cameraIdMap_.erase(itrCameraId);
        cameraAbilityMap_.erase(itrCameraAbility);
        return logicalCameraId;
    }
}

std::string CameraHostConfig::ReturnEnableLogicalCameraId()
{
    std::string logicalCameraId;
    bool logicalCameraIdStatus = false;
    for (int32_t id = 1; id < CAMERA_MAX+1; id++) {
        logicalCameraId = "lcam00";
        logicalCameraId = logicalCameraId + std::to_string(id);
        auto itr = cameraIdMap_.find(logicalCameraId);
        if (itr == cameraIdMap_.end()) {
            logicalCameraIdStatus = true;
            break;
        }
    }
    if (logicalCameraIdStatus) {
        return logicalCameraId;
    } else {
        return std::string("");
    }
}

std::string CameraHostConfig::ReturnPhysicalCameraIdToString(const CameraId &physicalCameraId)
{
    for (auto iter = enumCameraIdMap_.begin(); iter != enumCameraIdMap_.end(); iter++) {
        if (iter->second == physicalCameraId) {
            return iter->first;
        }
    }
    return std::string("");
}

std::string CameraHostConfig::ReturnLogicalCameraIdToString(const std::string &physicalCameraId)
{
    for (auto iter = cameraIdMap_.begin(); iter != cameraIdMap_.end(); iter++) {
        if (iter->second[0] == physicalCameraId) {
            return iter->first;
        }
    }
    return std::string("");
}
} // end namespace OHOS::Camera
