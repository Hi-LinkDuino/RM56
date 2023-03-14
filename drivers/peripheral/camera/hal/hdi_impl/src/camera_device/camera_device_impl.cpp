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

#include "camera_device_impl.h"
#include <chrono>
#include "ipipeline_core.h"
#include "camera_host_config.h"
#include "idevice_manager.h"
#include "camera_metadata_info.h"
#include "watchdog.h"

#define HDI_DEVICE_PLACE_A_WATCHDOG \
    PLACE_A_NOKILL_WATCHDOG(std::bind(&CameraDeviceImpl::OnRequestTimeout, this));

namespace OHOS::Camera {
CameraDeviceImpl::CameraDeviceImpl(const std::string &cameraId,
    const std::shared_ptr<IPipelineCore> &pipelineCore)
    : isOpened_(false),
      cameraId_(cameraId),
      pipelineCore_(pipelineCore),
      cameraDeciceCallback_(nullptr),
      spStreamOperator_(nullptr),
      metaResultMode_(PER_FRAME),
      metadataResultsBase_(nullptr),
      metadataResults_(nullptr)
{
}

CamRetCode CameraDeviceImpl::GetStreamOperator(
    const OHOS::sptr<IStreamOperatorCallback> &callback,
    OHOS::sptr<IStreamOperator> &streamOperator)
{
    HDI_DEVICE_PLACE_A_WATCHDOG;
    DFX_LOCAL_HITRACE_BEGIN;
    if (callback == nullptr) {
        CAMERA_LOGW("input callback is null.");
        return INVALID_ARGUMENT;
    }

    spCameraDeciceCallback_ = callback;
    if (spStreamOperator_ == nullptr) {
#ifdef CAMERA_BUILT_ON_OHOS_LITE
        spStreamOperator_ = std::make_shared<StreamOperator>(spCameraDeciceCallback_, shared_from_this());
#else
        spStreamOperator_ = new(std::nothrow) StreamOperator(spCameraDeciceCallback_, shared_from_this());
#endif
        if (spStreamOperator_ == nullptr) {
            CAMERA_LOGW("create stream operator failed.");
            return DEVICE_ERROR;
        }
        spStreamOperator_->Init();
        ismOperator_ = spStreamOperator_;
    }
    streamOperator = ismOperator_;

    DFX_LOCAL_HITRACE_END;
    return NO_ERROR;
}

CamRetCode CameraDeviceImpl::UpdateSettings(const std::shared_ptr<CameraSetting> &settings)
{
    HDI_DEVICE_PLACE_A_WATCHDOG;
    DFX_LOCAL_HITRACE_BEGIN;
    if (settings == nullptr) {
        CAMERA_LOGE("input settings is null.");
        return INVALID_ARGUMENT;
    }

    if (pipelineCore_ == nullptr) {
        CAMERA_LOGE("pipeline core is null.");
        return CAMERA_CLOSED;
    }

    pipelineCore_->UpdateMetadata(settings);
    DFX_LOCAL_HITRACE_END;
    return NO_ERROR;
}

CamRetCode CameraDeviceImpl::SetResultMode(const ResultCallbackMode &mode)
{
    CAMERA_LOGD("entry.");
    if (mode < PER_FRAME || mode > ON_CHANGED) {
        CAMERA_LOGE("parameter out of range.");
        return INVALID_ARGUMENT;
    } else if (mode == PER_FRAME) {
        std::shared_ptr<IDeviceManager> deviceManager = IDeviceManager::GetInstance();
        deviceManager->SetSendflag(true);
    }

    metaResultMode_ = mode;
    return NO_ERROR;
}

ResultCallbackMode CameraDeviceImpl::GetMetaResultMode() const
{
    return metaResultMode_;
}

CamRetCode CameraDeviceImpl::GetEnabledResults(std::vector<MetaType> &results)
{
    HDI_DEVICE_PLACE_A_WATCHDOG;
    DFX_LOCAL_HITRACE_BEGIN;
    if (deviceMetaTypes_.empty()) {
        RetCode rc = GetEnabledFromCfg();
        if (rc != RC_OK) {
            CAMERA_LOGE("get enabled results from device manager failed.");
            return DEVICE_ERROR;
        }
    }

    std::unique_lock<std::mutex> l(enabledRstMutex_);
    results = enabledResults_;
    DFX_LOCAL_HITRACE_END;
    return NO_ERROR;
}

RetCode CameraDeviceImpl::GetEnabledFromCfg()
{
    // Get devicemanager
    std::shared_ptr<IDeviceManager> deviceManager = IDeviceManager::GetInstance();
    if (deviceManager == nullptr) {
        CAMERA_LOGW("device manager is null.");
        return RC_ERROR;
    }

    CameraHostConfig *config = CameraHostConfig::GetInstance();
    if (config == nullptr) {
        return RC_ERROR;
    }
    std::shared_ptr<CameraStandard::CameraMetadata> ability;
    RetCode rc = config->GetCameraAbility(cameraId_, ability);
    if (rc != RC_OK || ability == nullptr) {
        CAMERA_LOGD("GetCameraAbility failed.");
        return RC_ERROR;
    }

    common_metadata_header_t *metadata = ability->get();
    if (metadata == nullptr) {
        CAMERA_LOGD("ability get metadata is null.");
        return RC_ERROR;
    }

    camera_metadata_item_t entry;
    int ret = CameraStandard::FindCameraMetadataItem(metadata,
        OHOS_ABILITY_STREAM_AVAILABLE_BASIC_CONFIGURATIONS, &entry);
    if (ret == 0) {
        CAMERA_LOGD("FindCameraMetadataIte tags = %{public}d. type = %{public}d", entry.count, entry.data_type);
        for (uint32_t i = 0; i < entry.count; i++) {
            deviceMetaTypes_.push_back(*(entry.data.i32 + i));
        }
    }

    return RC_OK;
}

CamRetCode CameraDeviceImpl::EnableResult(const std::vector<MetaType> &results)
{
    HDI_DEVICE_PLACE_A_WATCHDOG;
    DFX_LOCAL_HITRACE_BEGIN;
    std::unique_lock<std::mutex> l(enabledRstMutex_);
    for (auto &metaType : results) {
        auto itr = std::find(enabledResults_.begin(), enabledResults_.end(), metaType);
        if (itr == enabledResults_.end()) {
            enabledResults_.push_back(metaType);
        } else {
            CAMERA_LOGW("enabled result is existed. [metaType = %{public}d]", metaType);
        }
    }

    std::shared_ptr<IDeviceManager> deviceManager = IDeviceManager::GetInstance();
    if (deviceManager == nullptr) {
        CAMERA_LOGW("device manager is null [dm name MpiDeviceManager].");
        return INVALID_ARGUMENT;
    }
    deviceManager->SetAbilityMetaDataTag(enabledResults_);
    DFX_LOCAL_HITRACE_END;
    return NO_ERROR;
}

CamRetCode CameraDeviceImpl::DisableResult(const std::vector<MetaType> &results)
{
    HDI_DEVICE_PLACE_A_WATCHDOG;
    DFX_LOCAL_HITRACE_BEGIN;
    CamRetCode ret = NO_ERROR;
    std::unique_lock<std::mutex> l(enabledRstMutex_);
    for (auto &metaType : results) {
        auto itr = std::find(enabledResults_.begin(), enabledResults_.end(), metaType);
        if (itr != enabledResults_.end()) {
            enabledResults_.erase(itr);
        } else {
            CAMERA_LOGW("enabled result is not found. [metaType = %{public}d]", metaType);
            ret = INVALID_ARGUMENT;
        }
    }

    std::shared_ptr<IDeviceManager> deviceManager1 = IDeviceManager::GetInstance();
    if (deviceManager1 == nullptr) {
        CAMERA_LOGW("device manager is null [dm name MpiDeviceManager].");
        return INVALID_ARGUMENT;
    }
    deviceManager1->SetAbilityMetaDataTag(enabledResults_);
    DFX_LOCAL_HITRACE_END;
    return NO_ERROR;
}

RetCode CameraDeviceImpl::GetMetadataResults(std::shared_ptr<CameraStandard::CameraMetadata> &metadata)
{
    // the value of metadataResults_ comes from event or directly from devicemanager
    RetCode rc = RC_OK;
    if (metadataResultsBase_ == nullptr) {
        std::unique_lock<std::mutex> lock(metaRstMutex_);
        if (metadataResults_ == nullptr) {
            CAMERA_LOGE("new metadata results is null.");
            rc = RC_ERROR;
        } else {
            metadataResultsBase_ = metadataResults_;
        }
    } else {
        rc = UpdataMetadataResultsBase();
    }
    metadata = metadataResultsBase_;

    return rc;
}

RetCode CameraDeviceImpl::UpdataMetadataResultsBase()
{
    std::unique_lock<std::mutex> lock(metaRstMutex_);
    RetCode rc = RC_ERROR;
    if (metadataResultsBase_ == nullptr || metadataResults_ == nullptr) {
        CAMERA_LOGD("metadataResultsBase_ or metadataResults_ is null.");
        return rc;
    }

    common_metadata_header_t *metadataBase = metadataResultsBase_->get();
    common_metadata_header_t *metadataNew = metadataResults_->get();
    if (metadataBase == nullptr || metadataNew == nullptr) {
        lock.unlock();
        CAMERA_LOGE("get metadata failed.");
        return rc;
    }

    for (auto &metaType : enabledResults_) {
        camera_metadata_item_t baseEntry;
        int ret = CameraStandard::FindCameraMetadataItem(metadataBase, metaType, &baseEntry);
        if (ret == -ENOENT) {
            CAMERA_LOGE("metadata base not found tag.[metaType = %{public}d]", metaType);
            continue;
        }
        camera_metadata_item_t newEntry;
        ret = CameraStandard::FindCameraMetadataItem(metadataNew, metaType, &newEntry);
        if (ret == -ENOENT) {
            CAMERA_LOGE("metadata result not found tag.[metaType = %{public}d]", metaType);
            continue;
        }

        if (!CompareTagData(baseEntry, newEntry)) {
            metadataResultsBase_ = metadataResults_;
            rc = RC_OK;
        }
    }

    return rc;
}

bool CameraDeviceImpl::CompareTagData(const camera_metadata_item_t &baseEntry,
    const camera_metadata_item_t &newEntry)
{
    if (baseEntry.data_type == META_TYPE_BYTE && newEntry.data_type == META_TYPE_BYTE) {
        if (*(baseEntry.data.u8) != *(newEntry.data.u8)) {
            return false;
        }
    } else if (baseEntry.data_type == META_TYPE_INT32 && newEntry.data_type == META_TYPE_INT32) {
        if (*(baseEntry.data.i32) != *(newEntry.data.i32)) {
            return false;
        }
    } else if (baseEntry.data_type == META_TYPE_FLOAT && newEntry.data_type == META_TYPE_FLOAT) {
        if (*(baseEntry.data.f) != *(newEntry.data.f)) {
            return false;
        }
    } else if (baseEntry.data_type == META_TYPE_INT64 && newEntry.data_type == META_TYPE_INT64) {
        if (*(baseEntry.data.i64) != *(newEntry.data.i64)) {
            return false;
        }
    } else if (baseEntry.data_type == META_TYPE_DOUBLE && newEntry.data_type == META_TYPE_DOUBLE) {
        if (*(baseEntry.data.d) != *(newEntry.data.d)) {
            return false;
        }
    } else if (baseEntry.data_type == META_TYPE_RATIONAL && newEntry.data_type == META_TYPE_RATIONAL) {
        if (baseEntry.data.r->numerator != newEntry.data.r->numerator ||
            baseEntry.data.r->denominator != newEntry.data.r->denominator) {
            return false;
        }
    }
    return true;
}

void CameraDeviceImpl::Close()
{
    HDI_DEVICE_PLACE_A_WATCHDOG;
    DFX_LOCAL_HITRACE_BEGIN;

    if (spStreamOperator_ != nullptr) {
        spStreamOperator_->ReleaseStreams();
        spStreamOperator_ = nullptr;
    }

    std::shared_ptr<IDeviceManager> deviceManager = IDeviceManager::GetInstance();
    if (deviceManager == nullptr) {
        CAMERA_LOGW("device manager is null [dm name MpiDeviceManager].");
        return;
    }

    CameraHostConfig *config = CameraHostConfig::GetInstance();
    if (config == nullptr) {
        CAMERA_LOGD("CameraHostConfig get failed.");
        return;
    }

    std::vector<std::string> phyCameraIds;
    RetCode rc = config->GetPhysicCameraIds(cameraId_, phyCameraIds);
    if (rc != RC_OK) {
        CAMERA_LOGW("get physic cameraId failed.[cameraId = %{public}s]", cameraId_.c_str());
        return;
    }

    for (auto &phyCameraId : phyCameraIds) {
        auto itr = CameraHostConfig::enumCameraIdMap_.find(phyCameraId);
        if (itr == CameraHostConfig::enumCameraIdMap_.end()) {
            CAMERA_LOGW("config phyCameraId undefined in device manager.");
            continue;
        }

        rc = deviceManager->PowerDown(itr->second);
        if (rc != RC_OK) {
            CAMERA_LOGE("physic camera powerdown failed [phyCameraId = %{public}s].", phyCameraId.c_str());
            continue;
        }
        CAMERA_LOGD("[phyCameraId = %{public}s] powerdown success.", phyCameraId.c_str());
    }

    isOpened_ = false;
    DFX_LOCAL_HITRACE_END;
    CAMERA_LOGD("camera close success.");
}

CamRetCode CameraDeviceImpl::SetCallback(const OHOS::sptr<ICameraDeviceCallback> &callback)
{
    if (callback == nullptr) {
        return INVALID_ARGUMENT;
    }
    cameraDeciceCallback_ = callback;
    return NO_ERROR;
}

std::shared_ptr<IPipelineCore> CameraDeviceImpl::GetPipelineCore() const
{
    return pipelineCore_;
}

void CameraDeviceImpl::ResultMetadata()
{
    if (cameraDeciceCallback_ == nullptr) {
        CAMERA_LOGE("camera device callback is null.");
        return;
    }

    std::shared_ptr<CameraStandard::CameraMetadata> metadata;
    RetCode rc = GetMetadataResults(metadata);
    if (rc == RC_OK || metaResultMode_ == PER_FRAME) {
        uint64_t timestamp = GetCurrentLocalTimeStamp();
        cameraDeciceCallback_->OnResult(timestamp, metadata);
    }
}

uint64_t CameraDeviceImpl::GetCurrentLocalTimeStamp()
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp =
        std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    return static_cast<uint64_t>(tmp.count());
}

void CameraDeviceImpl::GetCameraId(std::string &cameraId) const
{
    cameraId = cameraId_;
}

void CameraDeviceImpl::OnRequestTimeout()
{
    CAMERA_LOGD("OnRequestTimeout callback success.");
    // request error
    cameraDeciceCallback_->OnError(REQUEST_TIMEOUT, 0);
}

void CameraDeviceImpl::OnMetadataChanged(const std::shared_ptr<CameraStandard::CameraMetadata> &metadata)
{
    CAMERA_LOGD("OnMetadataChanged callback success.");
    // device metadata changed callback
    {
        std::unique_lock<std::mutex> lock(metaRstMutex_);
        metadataResults_ = metadata;
    }
    if (metaResultMode_ == ON_CHANGED || metaResultMode_ == PER_FRAME) {
        ResultMetadata();
    }
}

void CameraDeviceImpl::OnDevStatusErr()
{
    CAMERA_LOGD("OnDevStatusErr callback success.");
    // device error
    cameraDeciceCallback_->OnError(FATAL_ERROR, 0);
}

bool CameraDeviceImpl::IsOpened() const
{
    return isOpened_;
}

void CameraDeviceImpl::SetStatus(bool isOpened)
{
    isOpened_ = isOpened;
}
} // end namespace OHOS::Camera
