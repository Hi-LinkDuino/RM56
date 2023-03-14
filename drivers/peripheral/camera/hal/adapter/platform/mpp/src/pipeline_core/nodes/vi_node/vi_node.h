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

#ifndef VI_NODE_H
#define VI_NODE_H

#include <vector>
#include "vi_controller.h"
#include "vi_manager.h"
#include "mpi_node.h"
#include "device_manager_adapter.h"
#include "mpi_device_manager.h"
#include "camera.h"

namespace OHOS::Camera {
class ViNode : public MpiNode {
public:
    ViNode(const std::string& name, const std::string& type);
    ~ViNode() override = default;
    RetCode Start(const int32_t streamId) override;
    RetCode Stop(const int32_t streamId) override;
    RetCode GetDeviceController();
    RetCode Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta);
private:
    std::shared_ptr<ViController>       viController_ = nullptr;
    std::atomic_bool                    streamRunning_ = false;
};
} // namespace OHOS::Camera
#endif
