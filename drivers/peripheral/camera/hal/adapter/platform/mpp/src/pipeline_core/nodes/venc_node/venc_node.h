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

#ifndef HOS_CAMERA_VENC_NODE_H
#define HOS_CAMERA_VENC_NODE_H

#include <cstdint>
#include "mpi_node.h"
#include "sink_node.h"
#include "device_manager_adapter.h"
#include "mpi_device_manager.h"
#include "camera.h"
#include "venc_controller.h"
#include "venc_manager.h"

namespace OHOS::Camera {
class VencNode : public MpiNode, public SinkNode {
public:
    VencNode(const std::string& name, const std::string& type);
    ~VencNode() override;
    RetCode Start(const int32_t streamId) override;
    RetCode Stop(const int32_t streamId) override;
    RetCode GetDeviceController() const;
};
} // namespace OHOS::Camera
#endif
