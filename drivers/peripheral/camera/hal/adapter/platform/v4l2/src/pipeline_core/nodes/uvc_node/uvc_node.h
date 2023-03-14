/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HOS_CAMERA_UVC_NODE_H
#define HOS_CAMERA_UVC_NODE_H

#include <vector>
#include "device_manager_adapter.h"
#include "v4l2_device_manager.h"
#include "utils.h"
#include "source_node.h"
#include "sensor_controller.h"
#include "sensor_manager.h"

namespace OHOS::Camera {
class UvcNode : public SourceNode {
public:
    UvcNode(const std::string& name, const std::string& type);
    ~UvcNode() override;
    RetCode Init(const int32_t streamId) override;
    RetCode Flush(const int32_t streamId) override;
    RetCode Start(const int32_t streamId) override;
    RetCode Stop(const int32_t streamId) override;
    RetCode GetDeviceController();
    void SetBufferCallback();
    RetCode ProvideBuffers(std::shared_ptr<FrameSpec> frameSpec) override;
protected:
    RetCode StartCheck(int64_t &bufferPoolId);
private:
    std::shared_ptr<SensorController>       sensorController_ = nullptr;
    std::shared_ptr<IBufferPool>            bufferPool_ = nullptr;
    std::shared_ptr<IDeviceManager>     deviceManager_ = nullptr;
};
} // namespace OHOS::Camera
#endif
