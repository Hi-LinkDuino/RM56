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
#include "venc_node.h"

namespace OHOS::Camera {
VencNode::VencNode(const std::string& name, const std::string& type)
    : NodeBase(name, type), MpiNode(name, type), SinkNode(name, type)
{
    CAMERA_LOGV("%{public}s enter, type(%{public}s)\n", name_.c_str(), type_.c_str());
}

RetCode VencNode::GetDeviceController() const
{
    return RC_OK;
}

RetCode VencNode::Start(const int32_t streamId)
{
    (void)streamId;
    return RC_OK;
}
RetCode VencNode::Stop(const int32_t streamId)
{
    (void)streamId;
    return RC_OK;
}

VencNode::~VencNode()
{
    CAMERA_LOGV("%{public}s, venc node dtor.", __FUNCTION__);
}

REGISTERNODE(VencNode, {"venc"})
} // namespace OHOS::Camera
