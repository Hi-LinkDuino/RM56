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

#include "transform_node.h"

namespace OHOS::Camera {
TransformNode::TransformNode(const std::string& name, const std::string& type)
    :NodeBase(name, type, streamId)
{
    CAMERA_LOGV("%{public}s enter, type(%{public}s)\n", name_.c_str(), type_.c_str());
}

RetCode TransformNode::Start(const int32_t streamId)
{
    return RC_OK;
}

RetCode TransformNode::Stop(const int32_t streamId)
{
    return RC_OK;
}

void TransformNode::AllocateBuffers()
{
}
REGISTERNODE(TransformNode, {"transform"})
}// namespace OHOS::Camera
