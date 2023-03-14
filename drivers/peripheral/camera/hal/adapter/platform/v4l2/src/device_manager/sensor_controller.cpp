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

#include "sensor_controller.h"
#include <cinttypes>
#include "securec.h"

namespace OHOS::Camera {
SensorController::SensorController() {}

SensorController::SensorController(std::string hardwareName) : IController(hardwareName), buffCont_(0) {}

SensorController::~SensorController() {}

RetCode SensorController::Init()
{
    sensorVideo_ = std::make_shared<HosV4L2Dev>();
    if (sensorVideo_ == nullptr) {
        CAMERA_LOGE("%s Create HosV4L2Dev fail", __FUNCTION__);
        return RC_ERROR;
    }
    return RC_OK;
}

RetCode SensorController::PowerUp()
{
    RetCode rc = RC_OK;
    if (GetPowerOnState() == false) {
        SetPowerOnState(true);
        CAMERA_LOGI("%s Sensor Powerup", __FUNCTION__);
        return rc;
    }
    return rc;
}

RetCode SensorController::PowerDown()
{
    RetCode rc = RC_OK;
    if (GetPowerOnState() == true) {
        SetPowerOnState(false);
        sensorVideo_->StopStream(GetName());
        sensorVideo_->ReleaseBuffers(GetName());
        sensorVideo_->stop(GetName());
        CAMERA_LOGI("%s Sensor PowerDown", __FUNCTION__);
        return rc;
    }
    return rc;
}

RetCode SensorController::Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta)
{
    return SendSensorMetaData(meta);
};

RetCode SensorController::Start(int buffCont, DeviceFormat& format)
{
    CAMERA_LOGI("%s Start", __FUNCTION__);
    std::lock_guard<std::mutex> l(startSensorLock_);
    RetCode rc = RC_OK;
    if (startSensorState_ == false) {
        buffCont_ = buffCont;
        sensorVideo_->start(GetName());
        sensorVideo_->ConfigSys(GetName(), CMD_V4L2_SET_FORMAT, format);
        sensorVideo_->ReqBuffers(GetName(), buffCont_);
        startSensorState_ = true;
    }
    return rc;
};

RetCode SensorController::Stop()
{
    CAMERA_LOGI("%s Stop", __FUNCTION__);
    std::lock_guard<std::mutex> l(startSensorLock_);
    RetCode rc = RC_OK;
    if (startSensorState_ == true) {
        startSensorState_ = false;
    }
    return rc;
};

RetCode SensorController::SendFrameBuffer(std::shared_ptr<FrameSpec> buffer)
{
    if (buffCont_ >= 1) {
        CAMERA_LOGI("%s buffCont_ %d", __FUNCTION__, buffCont_);
        sensorVideo_->CreatBuffer(GetName(), buffer);
        if (buffCont_ == 1) {
            sensorVideo_->StartStream(GetName());
        }
        buffCont_--;
    } else {
        sensorVideo_->QueueBuffer(GetName(), buffer);
    }
    return RC_OK;
}

void SensorController::SetNodeCallBack(const NodeBufferCb cb)
{
    CAMERA_LOGI("SensorController SetNodeCallBack entry");
    nodeBufferCb_ = cb;
    sensorVideo_->SetCallback([&](std::shared_ptr<FrameSpec> buffer) {
        BufferCallback(buffer);
    });
}

void SensorController::SetMetaDataCallBack(const MetaDataCb cb)
{
    metaDataCb_ = cb;
}

void SensorController::BufferCallback(std::shared_ptr<FrameSpec> buffer)
{
    if (nodeBufferCb_ == nullptr) {
        CAMERA_LOGE("nodeBufferCb_ is nullptr");
        return;
    }
    nodeBufferCb_(buffer);

    const int ENTRY_CAPACITY = 30; // 30:entry capacity
    const int DATA_CAPACITY = 2000; // 2000:data capacity
    std::shared_ptr<CameraStandard::CameraMetadata> meta =
        std::make_shared<CameraStandard::CameraMetadata>(ENTRY_CAPACITY, DATA_CAPACITY);
    RetCode rc = GetAbilityMetaData(meta);
    std::lock_guard<std::mutex> l(metaDataFlaglock_);
    if (rc == RC_OK && metaDataFlag_ == true) {
        if (metaDataCb_ == nullptr) {
            CAMERA_LOGE("metaDataCb_ is nullptr");
            return;
        }
        metaDataCb_(meta);
        metaDataFlag_ = false;
    } else {
        if (rc == RC_ERROR) {
            CAMERA_LOGE("%s GetAbilityMetaData error", __FUNCTION__);
        } else {
            CAMERA_LOGI("%s no send", __FUNCTION__);
        }
    }
}

RetCode SensorController::GetAbilityMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta)
{
    return GetSensorMetaData(meta);
}

void SensorController::SetAbilityMetaDataTag(std::vector<int32_t> abilityMetaDataTag)
{
    std::lock_guard<std::mutex> l(metaDataSetlock_);
    std::vector<int32_t>().swap(abilityMetaData_);
    for (auto it = abilityMetaDataTag.cbegin(); it != abilityMetaDataTag.cend(); it++) {
        switch (*it) {
            case OHOS_SENSOR_EXPOSURE_TIME: {
                abilityMetaData_.push_back(*it);
                break;
            }
            case OHOS_SENSOR_COLOR_CORRECTION_GAINS: {
                abilityMetaData_.push_back((*it));
                break;
            }
            default:
                break;
        }
    }
}

RetCode SensorController::GetSensorMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta)
{
    RetCode rc = GetAEMetaData(meta);
    if (rc == RC_ERROR) {
        CAMERA_LOGE("%s GetAEMetaData fail", __FUNCTION__);
        return rc;
    }
    rc = GetAWBMetaData(meta);
    if (rc == RC_ERROR) {
        CAMERA_LOGE("%s GetAWBMetaData fail", __FUNCTION__);
    }
    return rc;
}

RetCode SensorController::GetAEMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta)
{
    static int64_t oldExpoTime = 0;
    int64_t expoTime = 0;
    RetCode rc = RC_ERROR;
    std::lock_guard<std::mutex> l(metaDataSetlock_);
    for (auto iter = abilityMetaData_.cbegin(); iter != abilityMetaData_.cend(); iter++) {
        switch (*iter) {
            case OHOS_SENSOR_EXPOSURE_TIME: {
                rc = sensorVideo_->QuerySetting(GetName(), CMD_AE_EXPO, (int*)&expoTime);
                CAMERA_LOGD("%s Get CMD_AE_EXPOTIME [%" PRId64 "]", __FUNCTION__, expoTime);
                if (rc == RC_ERROR) {
                    CAMERA_LOGE("%s CMD_AE_EXPO QuerySetting fail", __FUNCTION__);
                    return rc;
                }
                if (oldExpoTime != expoTime) {
                    std::lock_guard<std::mutex> l(metaDataFlaglock_);
                    metaDataFlag_ = true;
                    oldExpoTime = expoTime;
                }
                meta->addEntry(OHOS_SENSOR_EXPOSURE_TIME, &expoTime, 1);
                break;
            }
            default:
                break;
        }
    }
    return rc;
}

RetCode SensorController::GetAWBMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta)
{
    static float oldColorGains[4] = {0};
    float colorGains[4] = {0};
    int value = 0;
    RetCode rc = RC_ERROR;
    std::lock_guard<std::mutex> l(metaDataSetlock_);
    for (auto iter = abilityMetaData_.cbegin(); iter != abilityMetaData_.cend(); iter++) {
        switch (*iter) {
            case OHOS_SENSOR_COLOR_CORRECTION_GAINS: {
                rc = sensorVideo_->QuerySetting(GetName(), CMD_AWB_COLORGAINS, &value);
                if (rc == RC_ERROR) {
                    CAMERA_LOGE("%s CMD_AWB_COLORGAINS QuerySetting fail", __FUNCTION__);
                    return rc;
                }
                colorGains[0] = (float)value;
                int gainsSize = 4;
                if (!CheckNumequal(oldColorGains, colorGains, gainsSize)) {
                    std::lock_guard<std::mutex> l(metaDataFlaglock_);
                    metaDataFlag_ = true;
                    (void)memcpy_s(oldColorGains, sizeof(oldColorGains) / sizeof(float), colorGains,
                        gainsSize * sizeof(float));
                }
                meta->addEntry(OHOS_SENSOR_COLOR_CORRECTION_GAINS, &colorGains, 4); // 4:data size
                break;
            }
            default:
                break;
        }
    }
    return rc;
}

RetCode SensorController::SendSensorMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta)
{
    common_metadata_header_t *data = meta->get();
    if (data == nullptr) {
        CAMERA_LOGE("%s data is nullptr", __FUNCTION__);
        return RC_ERROR;
    }
    RetCode rc = SendAEMetaData(data);
    if (rc == RC_ERROR) {
        CAMERA_LOGE("%s SendAEMetaData fail", __FUNCTION__);
        return rc;
    }
    rc = SendAWBMetaData(data);
    if (rc == RC_ERROR) {
        CAMERA_LOGE("%s SendAWBMetaData fail", __FUNCTION__);
    }
    return rc;
}

RetCode SensorController::SendAEMetaData(common_metadata_header_t *data)
{
    int32_t expo = 0;
    RetCode rc = RC_OK;
    camera_metadata_item_t entry;
    int ret = CameraStandard::FindCameraMetadataItem(data, OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &entry);
    if (ret == 0) {
        expo = *(entry.data.i32);
        if (expo != 0) {
            int32_t aemode = 1;
            rc = sensorVideo_->UpdateSetting(GetName(), CMD_AE_EXPO, (int*)&aemode);
            rc = sensorVideo_->UpdateSetting(GetName(), CMD_AE_EXPOTIME, (int*)&expo);
            CAMERA_LOGD("%s Set CMD_AE_EXPO EXPOTIME[%d] EXPO[%d]", __FUNCTION__, expo, aemode);
        } else {
            int32_t aemode = 0;
            rc = sensorVideo_->UpdateSetting(GetName(), CMD_AE_EXPO, (int*)&aemode);
            CAMERA_LOGD("%s Set CMD_AE_EXPOTIME [%d]", __FUNCTION__, aemode);
        }
        if (rc == RC_ERROR) {
            CAMERA_LOGE("%s Send CMD_AE_EXPOTIME fail", __FUNCTION__);
            return rc;
        }
    }
    return rc;
}

RetCode SensorController::SendAWBMetaData(common_metadata_header_t *data)
{
    uint8_t awbMode = 0;
    RetCode rc = RC_OK;
    camera_metadata_item_t entry;
    int ret = CameraStandard::FindCameraMetadataItem(data, OHOS_CONTROL_AWB_MODE, &entry);
    if (ret == 0) {
        awbMode = *(entry.data.u8);
        rc = sensorVideo_->UpdateSetting(GetName(), CMD_AWB_MODE, (int*)&awbMode);
        CAMERA_LOGD("%s Set CMD_AWB_MODE [%d]", __FUNCTION__, awbMode);
        if (rc == RC_ERROR) {
            CAMERA_LOGE("%s Send CMD_AWB_MODE fail", __FUNCTION__);
            return rc;
        }
    }
    return rc;
}

RetCode SensorController::Flush(int32_t streamId)
{
    return sensorVideo_->Flush(GetName());
}
} // namespace OHOS::Camera
