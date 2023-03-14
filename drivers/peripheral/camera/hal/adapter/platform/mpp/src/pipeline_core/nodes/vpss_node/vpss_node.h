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

#ifndef HOS_CAMERA_VPSS_NODE_H
#define HOS_CAMERA_VPSS_NODE_H

#include <vector>
#include <cstdint>
#include "vpss_controller.h"
#include "vpss_manager.h"
#include "mpi_node.h"
#include "source_node.h"
#include "device_manager_adapter.h"
#include "mpi_device_manager.h"
#include "idevice_manager.h"
#include "camera.h"

namespace OHOS::Camera {
class VpssNode : public MpiNode, public SourceNode {
public:
    VpssNode(const std::string& name, const std::string& type);
    ~VpssNode() override;
    RetCode GetDeviceController();
    RetCode Init(const int32_t streamId) override;
    RetCode Start(const int32_t streamId) override;
    RetCode Flush(const int32_t streamId) override;
    RetCode Stop(const int32_t streamId) override;
    void SetBufferCallback() override;
    RetCode ProvideBuffers(std::shared_ptr<FrameSpec> frameSpec) override;
    RetCode Capture(const int32_t streamId, const int32_t captureId) override;
    RetCode CancelCapture(const int32_t streamId) override;

private:
    std::shared_ptr<VpssController>         vpssController_ = nullptr;
    std::map<int32_t, int32_t>              ids_ = {};
};
} // namespace OHOS::Camera
#endif
