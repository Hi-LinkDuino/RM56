/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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
#include "camera_service.h"
#include "hal_camera.h"
#include "media_log.h"
#include "codec_interface.h"

using namespace std;
namespace OHOS {
namespace Media {
CameraService::CameraService() {}

CameraService::~CameraService()
{
    auto iter = deviceMap_.begin();
    while (iter != deviceMap_.end()) {
        if (iter->second != nullptr) {
            iter->second->StopLoopingCapture(-1);
            int32_t ret = HalCameraDeviceClose((uint32_t)std::atoi(iter->first.c_str()));
            if (ret != 0) {
                MEDIA_ERR_LOG("HalCameraDeviceClose failed. ret(%d)", ret);
            }
            deviceMap_.erase(iter++);
        } else {
            ++iter;
        }
    }
    int32_t ret = HalCameraDeinit();
    if (ret != 0) {
        MEDIA_ERR_LOG("HiCameraDeInit return failed ret(%d).", ret);
    }
}

CameraService *CameraService::GetInstance()
{
    static CameraService instance;
    return &instance;
}

void CameraService::Initialize()
{
    int32_t ret = HalCameraInit();
    if (ret != 0) {
        MEDIA_ERR_LOG("HiCameraInit failed. ret(%d)", ret);
    }
}

CameraAbility *CameraService::GetCameraAbility(std::string &cameraId)
{
    std::map<string, CameraAbility*>::iterator iter = deviceAbilityMap_.find(cameraId);
    if (iter != deviceAbilityMap_.end()) {
        return iter->second;
    }
    CameraAbility *ability = new (nothrow) CameraAbility;
    if (ability == nullptr) {
        return nullptr;
    }
    uint32_t streamCapNum;
    StreamCap *streamCap = nullptr;
    int32_t ret = HalCameraGetStreamCapNum(atoi(cameraId.c_str()), &streamCapNum);
    streamCap = new StreamCap[streamCapNum];
    for (uint32_t pos = 0; pos < streamCapNum; pos++) {
        streamCap[pos].type = CAP_DESC_ENUM;
    }
    ret = HalCameraGetStreamCap(atoi(cameraId.c_str()), streamCap, streamCapNum);
    list<CameraPicSize> range;
    for (int pos = 0; pos < streamCapNum; pos++) {
        CameraPicSize tmpSize = {.width = (uint32_t)streamCap[pos].u.formatEnum.width,
            .height = (uint32_t)streamCap[pos].u.formatEnum.height};
        range.emplace_back(tmpSize);
    }
    ability->SetParameterRange(CAM_FORMAT_YVU420, range);
    ability->SetParameterRange(CAM_FORMAT_JPEG, range);
    ability->SetParameterRange(CAM_FORMAT_H264, range);
    ability->SetParameterRange(CAM_FORMAT_H265, range);
    AbilityInfo cameraAbility = {};
    HalCameraGetAbility(atoi(cameraId.c_str()), &cameraAbility);
    list<int32_t> afModes;
    for (int i = 0; i < cameraAbility.afModeNum; i++) {
        afModes.emplace_back(cameraAbility.afModes[i]);
    }
    ability->SetParameterRange(CAM_AF_MODE, afModes);
    list<int32_t> aeModes;
    for (int i = 0; i < cameraAbility.aeModeNum; i++) {
        aeModes.emplace_back(cameraAbility.aeModes[i]);
    }
    ability->SetParameterRange(CAM_AE_MODE, aeModes);
    delete[] streamCap;
    deviceAbilityMap_.insert(pair<string, CameraAbility*>(cameraId, ability));
    return ability;
}

CameraInfo *CameraService::GetCameraInfo(std::string &cameraId)
{
    std::map<string, CameraInfo*>::iterator iter = deviceInfoMap_.find(cameraId);
    if (iter != deviceInfoMap_.end()) {
        return iter->second;
    }
    AbilityInfo deviceAbility;
    int32_t ret = HalCameraGetAbility((uint32_t)std::atoi(cameraId.c_str()), &deviceAbility);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("HalCameraGetAbility failed. ret(%d)", ret);
        return nullptr;
    }
    CameraInfo *info = new (nothrow) CameraInfoImpl(deviceAbility.type, deviceAbility.orientation);
    if (info == nullptr) {
        return nullptr;
    }
    deviceInfoMap_.insert(pair<string, CameraInfo*>(cameraId, info));
    return info;
}

CameraDevice *CameraService::GetCameraDevice(std::string &cameraId)
{
    std::map<string, CameraDevice*>::iterator iter = deviceMap_.find(cameraId);
    if (iter != deviceMap_.end()) {
        return iter->second;
    }
    return nullptr;
}

list<string> CameraService::GetCameraIdList()
{
    uint8_t camNum = 0;
    HalCameraGetDeviceNum(&camNum);
    uint32_t *cameraList = new uint32_t[camNum];
    HalCameraGetDeviceList(cameraList, camNum);
    list<string> cameraStrList;
    for (uint32_t pos = 0; pos < camNum; pos++) {
        cameraStrList.push_back(to_string(cameraList[pos]));
    }
    delete[] cameraList;
    return cameraStrList;
}

uint8_t CameraService::GetCameraModeNum()
{
    uint8_t num;
    int32_t ret = HalCameraGetModeNum(&num);
    if (ret == MEDIA_OK) {
        return num;
    }
    return 0;
}

int32_t CameraService::CreateCamera(string cameraId)
{
    int32_t ret = HalCameraDeviceOpen((uint32_t)std::atoi(cameraId.c_str()));
    if (ret != 0) {
        MEDIA_ERR_LOG("HalCameraDeviceOpen failed. ret(%d)", ret);
        return CameraServiceCallback::CAMERA_STATUS_CREATE_FAILED;
    }
    CameraDevice *device = new (nothrow) CameraDevice((uint32_t)std::atoi(cameraId.c_str()));
    if (device == nullptr) {
        MEDIA_FATAL_LOG("New device object failed.");
        return MEDIA_ERR;
    }
    if (device->Initialize() != MEDIA_OK) {
        MEDIA_FATAL_LOG("device Initialize failed.");
        delete device;
        return MEDIA_ERR;
    }
    deviceMap_.insert(pair<string, CameraDevice*>(cameraId, device));
    return CameraServiceCallback::CAMERA_STATUS_CREATED;
}

int32_t CameraService::CloseCamera(string cameraId)
{
    CameraDevice *device = GetCameraDevice(cameraId);
    if (device != NULL) {
        device->StopLoopingCapture(-1);
        deviceMap_.erase(cameraId);
    }
    int32_t ret = HalCameraDeviceClose((uint32_t)std::atoi(cameraId.c_str()));
    if (ret != 0) {
        MEDIA_ERR_LOG("HalCameraDeviceClose failed. ret(%d)", ret);
    }
    return CameraServiceCallback::CAMERA_STATUS_CLOSE;
}

int32_t CameraService::SetCameraMode(uint8_t modeIndex)
{
    CodecDeinit();
    int32_t ret = HalCameraSetMode(modeIndex);
    CodecInit();
    return ret;
}
} // namespace Media
} // namespace OHOS