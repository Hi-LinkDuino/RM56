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

#include "hcs_deal.h"
#include <vector>
#include <stdlib.h>
#include "hcs_dm_parser.h"
#include "metadata_enum_map.h"

namespace OHOS::Camera {
HcsDeal::HcsDeal(const std::string &pathName)
    : sPathName(pathName), pDevResIns(nullptr), pRootNode(nullptr)
{
}

HcsDeal::~HcsDeal()
{
    ReleaseHcsTree();
    pDevResIns = nullptr;
    pRootNode = nullptr;
}

void HcsDeal::SetHcsPathName(const std::string &pathName)
{
    sPathName = pathName;
}

RetCode HcsDeal::Init()
{
    ReleaseHcsTree();
    pDevResIns = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (pDevResIns == nullptr) {
        CAMERA_LOGE("get hcs interface failed.");
        return RC_ERROR;
    }

    CAMERA_LOGD("pathname = %{public}s", sPathName.c_str());
    SetHcsBlobPath(sPathName.c_str());
    pRootNode = pDevResIns->GetRootNode();
    if (pRootNode == nullptr) {
        CAMERA_LOGE("GetRootNode failed");
        return RC_ERROR;
    }
    if (pRootNode->name != nullptr) {
        CAMERA_LOGI("pRootNode = %{public}s", pRootNode->name);
    }

    DealHcsData();

    return RC_OK;
}

RetCode HcsDeal::DealHcsData()
{
    const struct DeviceResourceNode *cameraHostConfig =
        pDevResIns->GetChildNode(pRootNode, "camera_host_config");
    if (cameraHostConfig == nullptr) {
        return RC_ERROR;
    }
    if (pRootNode->name != nullptr) {
        CAMERA_LOGI("pRootNode = %{public}s", pRootNode->name);
    }
    if (cameraHostConfig->name == nullptr) {
        CAMERA_LOGW("cameraHostConfig->name is null");
        return RC_ERROR;
    }
    CAMERA_LOGD("cameraHostConfig = %{public}s", cameraHostConfig->name);

    const struct DeviceResourceNode *childNodeTmp = nullptr;
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(cameraHostConfig, childNodeTmp) {
        if (childNodeTmp != nullptr && childNodeTmp->name != nullptr) {
            std::string nodeName = std::string(childNodeTmp->name);
            CAMERA_LOGI("cameraHostConfig subnode name = %{public}s", nodeName.c_str());
            if (nodeName.find(std::string("ability"), 0) != std::string::npos) {
                DealCameraAbility(*childNodeTmp);
            }
        }
    }

    return RC_OK;
}

RetCode HcsDeal::DealCameraAbility(const struct DeviceResourceNode &node)
{
    CAMERA_LOGI("nodeName = %{public}s", node.name);

    const char *cameraId = nullptr;
    int32_t ret = pDevResIns->GetString(&node, "logicCameraId", &cameraId, nullptr);
    if (ret != 0) {
        CAMERA_LOGW("get logic cameraid failed");
        return RC_ERROR;
    }
    CAMERA_LOGD("logic cameraid is %{public}s", cameraId);

    std::vector<std::string> phyCameraIds;
    (void)DealPhysicsCameraId(node, phyCameraIds);
    if (!phyCameraIds.empty() && cameraId != nullptr) {
        cameraIdMap_.insert(std::make_pair(std::string(cameraId), phyCameraIds));
    }

    const struct DeviceResourceNode *metadataNode = pDevResIns->GetChildNode(&node, "metadata");
    if (metadataNode == nullptr || cameraId == nullptr) {
        CAMERA_LOGW("metadataNode is null or cameraId is null");
        return RC_ERROR;
    }
    RetCode rc = DealMetadata(cameraId, *metadataNode);
    if (rc != RC_OK) {
        CAMERA_LOGW("deal metadata failed");
        return RC_ERROR;
    }

    for (CameraIdMap::iterator itr = cameraIdMap_.begin(); itr != cameraIdMap_.end(); ++itr) {
        CAMERA_LOGD("cameraId = %{public}s", itr->first.c_str());
        for (auto &str : itr->second) {
            CAMERA_LOGD("phyCameraId = %{public}s", str.c_str());
        }
    }

    return RC_OK;
}

RetCode HcsDeal::DealPhysicsCameraId(const struct DeviceResourceNode &node, std::vector<std::string> &cameraIds)
{
    const char *nodeValue = nullptr;
    int32_t elemNum = pDevResIns->GetElemNum(&node, "physicsCameraIds");
    for (int i = 0; i < elemNum; i++) {
        pDevResIns->GetStringArrayElem(&node, "physicsCameraIds", i, &nodeValue, nullptr);
        cameraIds.push_back(std::string(nodeValue));
    }

    return RC_OK;
}

RetCode HcsDeal::DealMetadata(const std::string &cameraId, const struct DeviceResourceNode &node)
{
    struct DeviceResourceAttr *drAttr = nullptr;
    DEV_RES_NODE_FOR_EACH_ATTR(&node, drAttr) {
    }

    CAMERA_LOGD("metadata = %{public}s", node.name);
    std::string cmpTmp;
    const int ENTRY_CAPACITY = 30;
    const int DATA_CAPACITY = 2000;
    std::shared_ptr<CameraStandard::CameraMetadata> metadata = std::make_shared<CameraStandard::CameraMetadata>(ENTRY_CAPACITY, DATA_CAPACITY);
    DealAeAvailableAntiBandingModes(node, metadata);
    DealAeAvailableModes(node, metadata);
    DealAvailableAeFpsTargets(node, metadata);
    DealAeCompensationRange(node, metadata);
    DealAeCompensationSteps(node, metadata);
    DealAvailableAwbModes(node, metadata);
    DealSensitivityRange(node, metadata);
    DealFaceDetectMode(node, metadata);
    DealAvailableResultKeys(node, metadata);
    cameraMetadataMap_.insert(std::make_pair(cameraId, metadata));

    return RC_OK;
}

RetCode HcsDeal::DealAeAvailableAntiBandingModes(
    const struct DeviceResourceNode &metadataNode,
    std::shared_ptr<CameraStandard::CameraMetadata> &metadata)
{
    const char *nodeValue = nullptr;
    std::vector<uint8_t> aeAvailableAntiBandingModeUint8s;
    int32_t elemNum = pDevResIns->GetElemNum(&metadataNode, "aeAvailableAntiBandingModes");
    for (int i = 0; i < elemNum; i++) {
        pDevResIns->GetStringArrayElem(&metadataNode, "aeAvailableAntiBandingModes", i, &nodeValue, nullptr);
        aeAvailableAntiBandingModeUint8s.push_back(AeAntibandingModeMap[std::string(nodeValue)]);
        CAMERA_LOGD("aeAvailableAntiBandingModes = %{public}s", nodeValue);
    }
    bool ret = metadata->addEntry(OHOS_CONTROL_AE_AVAILABLE_ANTIBANDING_MODES,
        aeAvailableAntiBandingModeUint8s.data(), aeAvailableAntiBandingModeUint8s.size());
    if (!ret) {
        CAMERA_LOGE("aeAvailableAntiBandingModes add failed");
        return RC_ERROR;
    }
    CAMERA_LOGD("aeAvailableAntiBandingModes add success");
    return RC_OK;
}

RetCode HcsDeal::DealAeAvailableModes(
    const struct DeviceResourceNode &metadataNode,
    std::shared_ptr<CameraStandard::CameraMetadata> &metadata)
{
    int32_t hcbRet = -1;
    const char *nodeValue = nullptr;
    std::vector<uint8_t> aeAvailableModesU8;
    int32_t elemNum = pDevResIns->GetElemNum(&metadataNode, "aeAvailableModes");
    for (int i = 0; i < elemNum; i++) {
        hcbRet = pDevResIns->GetStringArrayElem(&metadataNode, "aeAvailableModes", i, &nodeValue, nullptr);
        if (hcbRet != 0) {
            CAMERA_LOGD("get aeAvailableModes failed");
            continue;
        }
        aeAvailableModesU8.push_back(AeModeMap[std::string(nodeValue)]);
        CAMERA_LOGD("aeAvailableModes = %{public}s", nodeValue);
    }
    bool ret = metadata->addEntry(OHOS_CONTROL_AE_AVAILABLE_MODES,
        aeAvailableModesU8.data(), aeAvailableModesU8.size());
    if (!ret) {
        CAMERA_LOGE("aeAvailableModes add failed");
        return RC_ERROR;
    }
    CAMERA_LOGD("aeAvailableModes add success");
    return RC_OK;
}

RetCode HcsDeal::DealAvailableAeFpsTargets(
    const struct DeviceResourceNode &metadataNode,
    std::shared_ptr<CameraStandard::CameraMetadata> &metadata)
{
    int32_t hcbRet = -1;
    uint32_t nodeValue;
    std::vector<int32_t> availableAeFpsTargets;
    int32_t elemNum = pDevResIns->GetElemNum(&metadataNode, "availableAeFpsTargets");
    for (int i = 0; i < elemNum; i++) {
        hcbRet = pDevResIns->GetUint32ArrayElem(&metadataNode, "availableAeFpsTargets", i, &nodeValue, -1);
        if (hcbRet != 0) {
            CAMERA_LOGD("get availableAeFpsTargets failed");
            continue;
        }
        availableAeFpsTargets.push_back(static_cast<int32_t>(nodeValue));
        CAMERA_LOGD("get availableAeFpsTargets:%{public}d", nodeValue);
    }
    bool ret = metadata->addEntry(OHOS_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES,
        availableAeFpsTargets.data(), availableAeFpsTargets.size());
    if (!ret) {
        CAMERA_LOGE("availableAeFpsTargets add failed");
        return RC_ERROR;
    }
    CAMERA_LOGD("availableAeFpsTargets add success");
    return RC_OK;
}

RetCode HcsDeal::DealAeCompensationRange(
    const struct DeviceResourceNode &metadataNode,
    std::shared_ptr<CameraStandard::CameraMetadata> &metadata)
{
    std::vector<int32_t> aeCompensationRange;
    int32_t elemNum = pDevResIns->GetElemNum(&metadataNode, "aeCompensationRange");
    uint32_t nodeValue;
    for (int i = 0; i < elemNum; i++) {
        pDevResIns->GetUint32ArrayElem(&metadataNode, "aeCompensationRange", i, &nodeValue, -1);
        aeCompensationRange.push_back(static_cast<int32_t>(nodeValue));
    }

    bool ret = metadata->addEntry(OHOS_CONTROL_AE_COMPENSATION_RANGE,
        aeCompensationRange.data(), aeCompensationRange.size());
    if (!ret) {
        CAMERA_LOGD("aeCompensationRange add failed");
        return RC_ERROR;
    }
    CAMERA_LOGI("aeCompensationRange add success");
    return RC_OK;
}

RetCode HcsDeal::DealAeCompensationSteps(
    const struct DeviceResourceNode &metadataNode,
    std::shared_ptr<CameraStandard::CameraMetadata> &metadata)
{
    constexpr const char *AE_COMPENSATION_STEPS = "aeCompensationSteps";
    int32_t elemNum = pDevResIns->GetElemNum(&metadataNode, AE_COMPENSATION_STEPS);
    uint32_t nodeValue;
    camera_rational_t aeCompensationStep;
    constexpr uint32_t groupLen = 2;

    if (elemNum != groupLen) {
        CAMERA_LOGE("aeCompensationSteps hcs file configuration error");
        return RC_ERROR;
    }

    pDevResIns->GetUint32ArrayElem(&metadataNode, AE_COMPENSATION_STEPS, 0, &nodeValue, -1);
    aeCompensationStep.numerator = (int32_t)nodeValue;
    pDevResIns->GetUint32ArrayElem(&metadataNode, AE_COMPENSATION_STEPS, 1, &nodeValue, -1);
    aeCompensationStep.denominator = (int32_t)nodeValue;

    bool ret = metadata->addEntry(OHOS_CONTROL_AE_COMPENSATION_STEP, &aeCompensationStep, 1);
    if (!ret) {
        CAMERA_LOGE("aeCompensationSteps add failed");
        return RC_ERROR;
    }
    CAMERA_LOGD("aeCompensationSteps add success");
    return RC_OK;
}

RetCode HcsDeal::DealAvailableAwbModes(
    const struct DeviceResourceNode &metadataNode,
    std::shared_ptr<CameraStandard::CameraMetadata> &metadata)
{
    int32_t hcbRet = -1;
    const char *nodeValue = nullptr;
    std::vector<uint8_t> availableAwbModes;
    int32_t elemNum = pDevResIns->GetElemNum(&metadataNode, "availableAwbModes");
    for (int i = 0; i < elemNum; i++) {
        hcbRet = pDevResIns->GetStringArrayElem(&metadataNode, "availableAwbModes", i, &nodeValue, nullptr);
        if (hcbRet != 0) {
            CAMERA_LOGD("get availableAwbModes failed");
            continue;
        }
        availableAwbModes.push_back(AwbModeMap[std::string(nodeValue)]);
    }
    bool ret = metadata->addEntry(OHOS_CONTROL_AWB_AVAILABLE_MODES,
        availableAwbModes.data(), availableAwbModes.size());
    if (!ret) {
        CAMERA_LOGE("availableAwbModes add failed");
        return RC_ERROR;
    }
    CAMERA_LOGD("availableAwbModes add success");
    return RC_OK;
}

RetCode HcsDeal::DealSensitivityRange(
    const struct DeviceResourceNode &metadataNode,
    std::shared_ptr<CameraStandard::CameraMetadata> &metadata)
{
    std::vector<int32_t> sensitivityRange;
    int32_t elemNum = pDevResIns->GetElemNum(&metadataNode, "sensitivityRange");
    CAMERA_LOGD("sensitivityRange elemNum = %{public}d", elemNum);
    uint32_t nodeValue;
    for (int i = 0; i < elemNum; i++) {
        pDevResIns->GetUint32ArrayElem(&metadataNode, "sensitivityRange", i, &nodeValue, -1);
        sensitivityRange.push_back(static_cast<int32_t>(nodeValue));
    }

    bool ret = metadata->addEntry(OHOS_SENSOR_INFO_SENSITIVITY_RANGE,
        sensitivityRange.data(), sensitivityRange.size());
    if (!ret) {
        CAMERA_LOGI("sensitivityRange add failed");
        return RC_ERROR;
    }
    CAMERA_LOGD("sensitivityRange add success");
    return RC_OK;
}

RetCode HcsDeal::DealFaceDetectMode(
    const struct DeviceResourceNode &metadataNode,
    std::shared_ptr<CameraStandard::CameraMetadata> &metadata)
{
    const char *pNodeValue = nullptr;
    int32_t rc = pDevResIns->GetString(&metadataNode, "faceDetectMode", &pNodeValue, nullptr);
    if (rc != 0) {
        CAMERA_LOGI("get faceDetectMode failed");
        return RC_ERROR;
    }

    bool ret = metadata->addEntry(OHOS_STATISTICS_FACE_DETECT_MODE,
        &(FaceDetectModeMap[std::string(pNodeValue)]), 1);
    if (!ret) {
        CAMERA_LOGI("faceDetectMode add failed");
        return RC_ERROR;
    }
    CAMERA_LOGD("faceDetectMode add success");
    return RC_OK;
}

RetCode HcsDeal::DealAvailableResultKeys(
    const struct DeviceResourceNode &metadataNode,
    std::shared_ptr<CameraStandard::CameraMetadata> &metadata)
{
    int32_t hcbRet = -1;
    const char *nodeValue = nullptr;
    std::vector<int32_t> availableResultKeys;
    int32_t elemNum = pDevResIns->GetElemNum(
        &metadataNode, "availableResultKeys");
    for (int i = 0; i < elemNum; i++) {
        hcbRet = pDevResIns->GetStringArrayElem(
            &metadataNode, "availableResultKeys", i, &nodeValue, nullptr);
        if (hcbRet != 0) {
            CAMERA_LOGI("get availableResultKeys failed");
            continue;
        }
        availableResultKeys.push_back(MetadataTagMap[std::string(nodeValue)]);
    }
    bool ret = metadata->addEntry(OHOS_ABILITY_STREAM_AVAILABLE_BASIC_CONFIGURATIONS,
        availableResultKeys.data(), availableResultKeys.size());
    if (!ret) {
        CAMERA_LOGI("availableResultKeys add failed");
        return RC_ERROR;
    }
    CAMERA_LOGD("availableResultKeys add success");
    return RC_OK;
}

RetCode HcsDeal::GetMetadata(CameraMetadataMap &metadataMap) const
{
    metadataMap = cameraMetadataMap_;
    return RC_OK;
}

RetCode HcsDeal::GetCameraId(CameraIdMap &cameraIdMap) const
{
    cameraIdMap = cameraIdMap_;
    return RC_OK;
}
} // namespace OHOS::CameraHost