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

#include "mpi_device_manager.h"
#include "venc_manager.h"
#include "vi_manager.h"
#include "vo_manager.h"
#include "vpss_manager.h"
#include "project_hardware.h"

namespace OHOS {
namespace Camera {
IMPLEMENT_DEVICEMANAGER(MpiDeviceManager)
MpiDeviceManager::MpiDeviceManager()
    :sysInitFlag(false)
{
}

MpiDeviceManager::~MpiDeviceManager()
{
}

RetCode MpiDeviceManager::Init()
{
    RetCode rc = RC_ERROR;

    for (auto iter = hardware.cbegin(); iter != hardware.cend(); iter++) {
        hardwareList_.push_back(*iter);
    }

    rc = CreateManager();
    if (rc != RC_OK) {
        CAMERA_LOGE("CreateManager fail");
        return rc;
    }

    sysObject_ = ISysObject::CreateSysObject();
    if (sysObject_ == nullptr) {
        CAMERA_LOGE("Create SysObject fail");
        return RC_ERROR;
    }

    rc = sysObject_->InitSys();
    if (rc != RC_OK) {
        CAMERA_LOGE("InitSys fail");
    }

    return rc;
}

std::vector<CameraId> MpiDeviceManager::GetCameraId()
{
    std::vector<CameraId> sensor_list;
    for (auto iter = hardwareList_.cbegin(); iter != hardwareList_.cend(); iter++) {
        if ((*iter).controllerId == DM_C_SENSOR) {
            sensor_list.push_back((*iter).cameraId);
        }
    }
    return sensor_list;
};

RetCode MpiDeviceManager::PowerUp(CameraId cameraId)
{
    if (sysInitFlag == false) {
        sysObject_->StartSys();
        sysInitFlag = true;
    }
    if (CheckCameraIdList(cameraId) == false) {
        return RC_ERROR;
    }
    RetCode rc = RC_OK;
    for (auto iter = managerList_.cbegin(); iter != managerList_.cend(); iter++) {
        rc = (*iter)->PowerUp(cameraId);
        if (rc == RC_ERROR) {
            return RC_ERROR;
        }
    }
    return rc;
}

RetCode MpiDeviceManager::PowerDown(CameraId cameraId)
{
    RetCode rc = RC_OK;
    if (CheckCameraIdList(cameraId) == false) {
        return RC_ERROR;
    }
    for (auto iter = managerList_.cbegin(); iter != managerList_.cend(); iter++) {
        rc = (*iter)->PowerDown(cameraId);
        if (rc == RC_ERROR) {
            return RC_ERROR;
        }
    }
    if (sysObject_ != nullptr) {
        sysObject_->StopSys();
        sysInitFlag = false;
    }
    return rc;
}

std::shared_ptr<ISensor> MpiDeviceManager::GetSensor(CameraId cameraId)
{
    for (auto iter = hardwareList_.cbegin(); iter != hardwareList_.cend(); iter++) {
        if ((*iter).controllerId == DM_C_SENSOR && (*iter).managerId == DM_M_VI && (*iter).cameraId == cameraId) {
            return (std::static_pointer_cast<ViManager>(GetManager(DM_M_VI)))->GetSensor((*iter).hardwareName);
        }
    }
    return nullptr;
}

std::shared_ptr<IManager> MpiDeviceManager::GetManager(ManagerId managerId)
{
    for (auto iter = managerList_.cbegin(); iter != managerList_.cend(); iter++) {
        if ((*iter)->GetManagerId() == managerId) {
            return (*iter);
        }
    }
    return nullptr;
}
RetCode MpiDeviceManager::CreateManager()
{
    RetCode rc = RC_OK;
    for (auto iter = hardwareList_.cbegin(); iter != hardwareList_.cend(); iter++) {
        if (CheckManagerList((*iter).managerId) == false) {
            std::shared_ptr<IManager> manager = nullptr;
            switch ((*iter).managerId) {
                case DM_M_VENC:
                    manager = std::make_shared<VencManager>(DM_M_VENC);
                    CHECK_IF_PTR_NULL_RETURN_VALUE(manager, RC_ERROR);
                    rc = CreateController(manager, DM_M_VENC);
                    break;
                case DM_M_VI:
                    manager = std::make_shared<ViManager>(DM_M_VI);
                    CHECK_IF_PTR_NULL_RETURN_VALUE(manager, RC_ERROR);
                    rc = CreateController(manager, DM_M_VI);
                    break;
                case DM_M_VO:
                    manager = std::make_shared<VoManager>(DM_M_VO);
                    CHECK_IF_PTR_NULL_RETURN_VALUE(manager, RC_ERROR);
                    rc = CreateController(manager, DM_M_VO);
                    break;
                case DM_M_VPSS:
                    manager = std::make_shared<VpssManager>(DM_M_VPSS);
                    CHECK_IF_PTR_NULL_RETURN_VALUE(manager, RC_ERROR);
                    rc = CreateController(manager, DM_M_VPSS);
                    break;
                default:
                    break;
            }
            managerList_.push_back(manager);
            manager = nullptr;
        }
        if (rc == RC_ERROR) {
            return RC_ERROR;
        }
    }
    return rc;
}
RetCode MpiDeviceManager::DestroyManager()
{
    return RC_OK;
}
std::shared_ptr<IController> MpiDeviceManager::GetController(ManagerId managerId, ControllerId controllerId)
{
    for (auto iter = managerList_.cbegin(); iter != managerList_.cend(); iter++) {
        if ((*iter)->GetManagerId() == managerId) {
            return (*iter)->GetController(controllerId);
        }
    }
    return nullptr;
}
RetCode MpiDeviceManager::CreateController(std::shared_ptr<IManager> manager, ManagerId managerId)
{
    RetCode rc = RC_OK;
    for (auto iter = hardwareList_.cbegin(); iter != hardwareList_.cend(); iter++) {
        if ((*iter).managerId == managerId) {
            switch (managerId) {
                case DM_M_VENC:
                    rc = (std::static_pointer_cast<VencManager>(manager))
                             ->CreateController((*iter).controllerId, (*iter).hardwareName);
                    break;
                case DM_M_VI:
                    rc = (std::static_pointer_cast<ViManager>(manager))
                             ->CreateController((*iter).controllerId, (*iter).hardwareName);
                    break;
                case DM_M_VO:
                    rc = (std::static_pointer_cast<VoManager>(manager))
                             ->CreateController((*iter).controllerId, (*iter).hardwareName);
                    break;
                case DM_M_VPSS:
                    rc = (std::static_pointer_cast<VpssManager>(manager))
                             ->CreateController((*iter).controllerId, (*iter).hardwareName);
                    break;
                default:
                    break;
            }
        }
        if (rc == RC_ERROR) {
            return RC_ERROR;
        }
    }
    return rc;
}
RetCode MpiDeviceManager::DestroyController()
{
    return RC_OK;
}

bool MpiDeviceManager::CheckCameraIdList(CameraId cameraId)
{
    if (hardwareList_.empty()) {
        return false;
    }
    for (auto iter = hardwareList_.cbegin(); iter != hardwareList_.cend(); iter++) {
        if ((*iter).cameraId == cameraId) {
            return true;
        }
    }
    return false;
}

bool MpiDeviceManager::CheckManagerList(ManagerId managerId)
{
    if (managerList_.size() == 0) {
        return false;
    }
    for (auto it = managerList_.cbegin(); it != managerList_.cend(); it++) {
        if ((*it)->GetManagerId() == managerId) {
            return true;
        }
    }
    return false;
}

ControllerId MpiDeviceManager::CheckControllerId(std::string controllerName)
{
    if (controllerName.find("vi") != controllerName.npos && GetController(DM_M_VI, DM_C_VI) != nullptr) {
        return DM_C_VI;
    }
    if (controllerName.find("vo") != controllerName.npos && GetController(DM_M_VO, DM_C_VO) != nullptr) {
        return DM_C_VO;
    }
    if (controllerName.find("vpss") != controllerName.npos && GetController(DM_M_VPSS, DM_C_VPSS) != nullptr) {
        return DM_C_VPSS;
    }
    if (controllerName.find("venc") != controllerName.npos && GetController(DM_M_VENC, DM_C_VENC) != nullptr) {
        return DM_C_VENC;
    }
    return DM_C_MAX;
}

RetCode MpiDeviceManager::Connect(std::string controller,
                                  std::string portNum,
                                  std::string connectController,
                                  std::string connectPortNum)
{
    ControllerId controllerId = CheckControllerId(controller);
    int32_t port = CheckNodePortNum(portNum);
    ControllerId connectControllerId = CheckControllerId(connectController);
    int32_t connectPort = CheckNodePortNum(connectPortNum);
    if (controllerId == DM_C_MAX || connectControllerId == DM_C_MAX) {
        CAMERA_LOGW("controller is not hardware device");
        return RC_OK;
    }
    RetCode rc = RC_ERROR;
    constexpr uint32_t coefficient = 2;
    switch (static_cast<uint32_t>(controllerId) << (coefficient + connectControllerId)) {
        case DM_C_VI << (coefficient + DM_C_VPSS): {
            std::shared_ptr<VpssController> vpss =
                std::static_pointer_cast<VpssController>(GetController(DM_M_VPSS,
                                                                       DM_C_VPSS));
            if (vpss != nullptr) {
                if (vpss->GetStartVpssState() != true) {
                    CHECK_IF_EQUAL_RETURN_VALUE(StartNode(DM_C_VPSS), RC_ERROR, RC_ERROR);
                }
                sysObject_->ViBindVpss(port, port, connectPort, connectPort);
                rc = RC_OK;
            }
            break;
        }
        case DM_C_VI << (coefficient + DM_C_VO): {
            std::shared_ptr<VoController> vo = std::static_pointer_cast<VoController>(GetController(DM_M_VO, DM_C_VO));
            if (vo != nullptr) {
                if (vo->GetStartVoState() != true) {
                    CHECK_IF_EQUAL_RETURN_VALUE(StartNode(DM_C_VO), RC_ERROR, RC_ERROR);
                }
                rc = RC_OK;
            }
            break;
        }
        case DM_C_VI << (coefficient + DM_C_VENC): {
            std::shared_ptr<VencController> venc =
                std::static_pointer_cast<VencController>(GetController(DM_M_VENC,
                                                                       DM_C_VENC));
            if (venc != nullptr) {
                if (venc->GetStartVencState() != true) {
                    CHECK_IF_EQUAL_RETURN_VALUE(StartNode(DM_C_VENC), RC_ERROR, RC_ERROR);
                }
                rc = RC_OK;
            }
            break;
        }
        case DM_C_VPSS << (coefficient + DM_C_VO): {
            std::shared_ptr<VoController> vo = std::static_pointer_cast<VoController>(GetController(DM_M_VO, DM_C_VO));
            if (vo != nullptr) {
                if (vo->GetStartVoState() != true) {
                    CHECK_IF_EQUAL_RETURN_VALUE(StartNode(DM_C_VO), RC_ERROR, RC_ERROR);
                }
                sysObject_->VpssBindVo(port, port, connectPort, connectPort);
                rc = RC_OK;
            }
            break;
        }
        case DM_C_VPSS << (coefficient + DM_C_VENC): {
            std::shared_ptr<VencController> venc =
                std::static_pointer_cast<VencController>(GetController(DM_M_VENC,
                                                                       DM_C_VENC));
            if (venc != nullptr) {
                if ((std::static_pointer_cast<VencController>(GetController(DM_M_VENC,
                    DM_C_VENC)))->GetStartVencState() != true) {
                    CHECK_IF_EQUAL_RETURN_VALUE(StartNode(DM_C_VENC), RC_ERROR, RC_ERROR);
                }
                rc = RC_OK;
            }
            break;
        }
        default: {
            return RC_ERROR;
        }
    }
    return rc;
}

RetCode MpiDeviceManager::UnConnect(std::string controller,
                                    std::string portNum,
                                    std::string connectController,
                                    std::string connectPortNum)
{
    ControllerId unControllerId = CheckControllerId(controller);
    int32_t unPort = CheckNodePortNum(portNum);
    ControllerId unConnectControllerId = CheckControllerId(connectController);
    int32_t unConnectPort = CheckNodePortNum(connectPortNum);
    if (unControllerId == DM_C_MAX || unConnectControllerId == DM_C_MAX) {
        CAMERA_LOGW("controller is not hardware device");
        return RC_OK;
    }
    RetCode rc = RC_ERROR;
    constexpr uint32_t unCoefficient = 2;
    switch (static_cast<uint32_t>(unControllerId) << (unCoefficient + unConnectControllerId)) {
        case DM_C_VI << (unCoefficient + DM_C_VPSS): {
            sysObject_->ViUnBindVpss(unPort, unPort, unConnectPort, unConnectPort);
            rc = RC_OK;
            break;
        }
        case DM_C_VI << (unCoefficient + DM_C_VO): {
            rc = RC_OK;
            break;
        }
        case DM_C_VI << (unCoefficient + DM_C_VENC): {
            rc = RC_OK;
            break;
        }
        case DM_C_VPSS << (unCoefficient + DM_C_VO): {
            sysObject_->VpssUnBindVo(unPort, unPort, unConnectPort, unConnectPort);
            rc = RC_OK;
            break;
        }
        case DM_C_VPSS << (unCoefficient + DM_C_VENC): {
            rc = RC_OK;
            break;
        }
        default: {
            return RC_ERROR;
        }
    }
    return rc;
}

RetCode MpiDeviceManager::StartNode(ControllerId controllerId)
{
    RetCode rc = RC_OK;
    switch (controllerId) {
        case DM_C_VI: {
            rc = std::static_pointer_cast<ViController>(GetController(DM_M_VI, DM_C_VI))->ConfigVi();
            if (rc == RC_ERROR) {
                return rc;
            }
            rc = std::static_pointer_cast<ViController>(GetController(DM_M_VI, DM_C_VI))->StartVi();
            if (rc == RC_ERROR) {
                return rc;
            }
            break;
        }
        case DM_C_VO: {
            rc = std::static_pointer_cast<VoController>(GetController(DM_M_VO, DM_C_VO))->ConfigVo();
            if (rc == RC_ERROR) {
                return rc;
            }
            rc = std::static_pointer_cast<VoController>(GetController(DM_M_VO, DM_C_VO))->StartVo();
            if (rc == RC_ERROR) {
                return rc;
            }
            break;
        }
        case DM_C_VPSS: {
            rc = std::static_pointer_cast<VpssController>(GetController(DM_M_VPSS, DM_C_VPSS))->ConfigVpss();
            if (rc == RC_ERROR) {
                return rc;
            }
            rc = std::static_pointer_cast<VpssController>(GetController(DM_M_VPSS, DM_C_VPSS))->StartVpss();
            if (rc == RC_ERROR) {
                return rc;
            }
            break;
        }
        case DM_C_VENC: {
            rc = std::static_pointer_cast<VencController>(GetController(DM_M_VENC, DM_C_VENC))->ConfigVenc();
            if (rc == RC_ERROR) {
                return rc;
            }
            rc = std::static_pointer_cast<VencController>(GetController(DM_M_VENC, DM_C_VENC))->StartVenc();
            if (rc == RC_ERROR) {
                return rc;
            }
            break;
        }
        default:
            break;
        }
    return rc;
}

int32_t MpiDeviceManager::CheckNodePortNum(std::string controllerName)
{
    if (controllerName.length() == 0) {
        CAMERA_LOGW("controllerName length is 0");
        return 0;
    }
    std::string num = controllerName.substr((controllerName.find("#")) + 1);
    if (num.length() == 0) {
        CAMERA_LOGW("num length is 0");
        return 0;
    }
    return atoi(num.c_str());
}

void MpiDeviceManager::SetAbilityMetaDataTag(std::vector<int32_t> abilityMetaDataTag)
{
    if (managerList_.size() != 0) {
        for (auto iter = managerList_.cbegin(); iter != managerList_.cend(); iter++) {
            (*iter)->SetAbilityMetaDataTag(abilityMetaDataTag);
        }
    }
}

RetCode MpiDeviceManager::GetAbilityMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta, bool &sendFlag)
{
    if (managerList_.size() != 0) {
        for (auto iter = managerList_.cbegin(); iter != managerList_.cend(); iter++) {
            RetCode rc = (*iter)->GetAbilityMetaData(meta);
            if (rc == RC_ERROR) {
                CAMERA_LOGE("Get %{public}d AbilityMetaData fail", (*iter)->GetManagerId());
                return rc;
            }
            if ((*iter)->GetMetaDataFlag()) {
                sendFlag = true;
                (*iter)->SetMetaDataFlag(false);
            }
        }
    }
    return RC_OK;
}

void MpiDeviceManager::SetSendflag(bool flag)
{
    metaSendflag_ = flag;
}

void MpiDeviceManager::BufferCallback(std::shared_ptr<FrameSpec> buffer)
{
    CAMERA_LOGE("MpiDeviceManager::BufferCallback %{public}p enter", &nodeBufferCb_);
    nodeBufferCb_(buffer);

    const int ENTRY_CAPACITY = 30;
    const int DATA_CAPACITY = 2000;
    std::shared_ptr<CameraStandard::CameraMetadata> meta =
        std::make_shared<CameraStandard::CameraMetadata>(ENTRY_CAPACITY, DATA_CAPACITY);
    RetCode rc;
    bool sendFlag = metaSendflag_;
    rc = GetAbilityMetaData(meta, sendFlag);
    if (rc == RC_OK && sendFlag == true) {
        metaDataCb_(meta);
    } else {
        if (rc == RC_ERROR) {
            CAMERA_LOGE("GetAbilityMetaData error");
        } else {
            CAMERA_LOGI("no send");
        }
    }

    CAMERA_LOGE("MpiDeviceManager::BufferCallback exit");
}

RetCode MpiDeviceManager::SendFrameBuffer(std::shared_ptr<FrameSpec> buffer, CameraId cameraId)
{
    (void)cameraId;
    return sysObject_->RequestBuffer(buffer);
}

void MpiDeviceManager::SetNodeCallBack(const NodeBufferCb cb, CameraId cameraId)
{
    CAMERA_LOGI("entry");
    (void)cameraId;
    nodeBufferCb_ = cb;
    sysObject_->SetCallback([&](std::shared_ptr<FrameSpec> buffer) {
        BufferCallback(buffer);
    });
    sysObject_->SetDevStatusCallback([&]() {
        deviceStatusCb_();
    });
}

void MpiDeviceManager::Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta)
{
    if (managerList_.size() != 0) {
        for (auto iter = managerList_.cbegin(); iter != managerList_.cend(); iter++) {
            (*iter)->Configure(meta);
        }
    }
}

void MpiDeviceManager::SetMetaDataCallBack(const MetaDataCb cb, CameraId cameraId)
{
    (void)cameraId;
    metaDataCb_ = cb;
}

void MpiDeviceManager::SetDevStatusCallBack(const DeviceStatusCb cb)
{
    deviceStatusCb_ = cb;
}

RetCode MpiDeviceManager::SetFlashlight(FlashMode flashMode, bool enable, CameraId cameraId)
{
    (void)cameraId;
    return std::static_pointer_cast<ViController>(GetController(DM_M_VI, DM_C_VI))->SetFlashlight(flashMode, enable);
}

RetCode MpiDeviceManager::PreConfig(const ModeMeta& meta, const std::vector<DeviceStreamSetting>& settings)
{
    return sysObject_->PreConfig(meta, settings);
}

RetCode MpiDeviceManager::Flush(int32_t streamId)
{
    return sysObject_->Flush(streamId);
}

RetCode MpiDeviceManager::StartRecvFrame(int32_t streamId)
{
    return sysObject_->StartRecvFrame(streamId);
}

RetCode MpiDeviceManager::StopRecvFrame(int32_t streamId)
{
    return sysObject_->StopRecvFrame(streamId);
}
} // namespace Camera
} // namespace OHOS
