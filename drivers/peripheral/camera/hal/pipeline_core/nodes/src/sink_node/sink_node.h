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

#ifndef HOS_CAMERA_SINK_NODE_H
#define HOS_CAMERA_SINK_NODE_H

#include <vector>
#include "device_manager_adapter.h"
#include "utils.h"
#include "camera.h"
#include "node_base.h"

namespace OHOS::Camera {
class SinkNode : virtual public NodeBase {
public:
    SinkNode(const std::string& name, const std::string& type);
    ~SinkNode() override = default;
    RetCode Start(const int32_t streamId) override;
    RetCode Stop(const int32_t streamId) override;
    void DeliverBuffer(std::shared_ptr<IBuffer>& buffer) override;
    void SetCallBack(BufferCb c) override
    {
        cb_ = c;
    }

protected:
    BufferCb  cb_;
};
}// namespace OHOS::Camera
#endif
