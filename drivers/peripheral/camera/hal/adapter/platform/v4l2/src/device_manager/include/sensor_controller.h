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

#ifndef HOS_CAMERA_SENSOR_CONTROLLER_H
#define HOS_CAMERA_SENSOR_CONTROLLER_H

#include "icontroller.h"
#include "device_manager_adapter.h"
#include "v4l2_dev.h"
#include "v4l2_common.h"

namespace OHOS::Camera {
class SensorController : public IController {
public:
    SensorController();
    explicit SensorController(std::string hardwareName);
    virtual ~SensorController();
    RetCode Init();
    RetCode PowerUp();
    RetCode PowerDown();
    RetCode Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta);
    RetCode Start(int buffCont, DeviceFormat& format);
    RetCode Stop();

    RetCode SendFrameBuffer(std::shared_ptr<FrameSpec> buffer);

    void SetNodeCallBack(const NodeBufferCb cb);
    void SetMetaDataCallBack(const MetaDataCb cb);
    void BufferCallback(std::shared_ptr<FrameSpec> buffer);

    void SetAbilityMetaDataTag(std::vector<int32_t> abilityMetaDataTag);
    RetCode GetAbilityMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta);
    RetCode Flush(int32_t streamId);

private:
    RetCode SendSensorMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta);
    RetCode SendAEMetaData(common_metadata_header_t *data);
    RetCode SendAWBMetaData(common_metadata_header_t *data);
    RetCode GetSensorMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta);
    RetCode GetAEMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta);
    RetCode GetAWBMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta);
    template<typename T>
    bool CheckNumequal(T oldnum, T num, int size)
    {
        if (oldnum == nullptr || num == nullptr) {
            CAMERA_LOGE("oldnum or num is nullptr");
            return false;
        }
        for (int i = 0; size > 0; i++, size--) {
            if (oldnum[i] != num[i]) {
                return false;
            }
        }
        return true;
    };
    std::mutex startSensorLock_;
    bool startSensorState_ = false;
    NodeBufferCb nodeBufferCb_ = nullptr;
    MetaDataCb metaDataCb_ = nullptr;
    std::vector<int32_t> abilityMetaData_;
    std::mutex metaDataSetlock_;
    std::mutex metaDataFlaglock_;
    bool metaDataFlag_ = false;
    int buffCont_;
    std::shared_ptr<HosV4L2Dev> sensorVideo_;
};
} // namespace OHOS::Camera
#endif
