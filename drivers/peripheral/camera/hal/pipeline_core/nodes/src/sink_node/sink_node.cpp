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

#include "sink_node.h"

namespace OHOS::Camera {
SinkNode::SinkNode(const std::string& name, const std::string& type)
    : NodeBase(name, type)
{
    CAMERA_LOGV("%{public}s enter, type(%{public}s)\n", name_.c_str(), type_.c_str());
}

RetCode SinkNode::Start(const int32_t streamId)
{
    (void)streamId;
    return RC_OK;
}

RetCode SinkNode::Stop(const int32_t streamId)
{
    (void)streamId;
    return RC_OK;
}

void SinkNode::DeliverBuffer(std::shared_ptr<IBuffer>& buffer)
{
    cb_(buffer);
    return;
}

REGISTERNODE(SinkNode, {"sink"})
}// namespace OHOS::Camera
