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

#include "core/animation/bilateral_spring_adapter.h"

namespace OHOS::Ace {

double BilateralSpringAdapter::ResetControl(int32_t delta)
{
    auto controlNode = GetControlNode();
    if (!controlNode) {
        return 0.0;
    }
    double lastValue = controlNode->GetValue();
    LOGD("BeforeReset: %{public}s", DumpNodes().c_str());
    RefPtr<SpringNode> srcNode;
    int32_t nodeSize = static_cast<int32_t>(nodes_.size());
    if (delta < 0) {
        for (int32_t index = nodeSize - 1; index >= 0; index--) {
            if (index + delta < 0) {
                srcNode = nodes_.at(0);
            } else {
                srcNode = nodes_.at(index + delta);
            }
            MoveNode(srcNode, index);
        }
    } else {
        for (int32_t index = 0; index < nodeSize; index++) {
            if (index + delta >= nodeSize) {
                srcNode = nodes_.at(nodeSize - 1);
            } else {
                srcNode = nodes_.at(index + delta);
            }
            MoveNode(srcNode, index);
        }
    }
    double ret = controlNode->GetValue() - lastValue;
    controlNode->ResetNode(0.0, controlNode->GetVelocity());
    LOGD("AfterReset: %{public}s", DumpNodes().c_str());
    return ret;
}

void BilateralSpringAdapter::FlushAnimation()
{
    // flush from control to edge.
    int32_t controlIndex = GetControlIndex();
    for (int32_t index = 1; index <= controlIndex; index++) {
        if (controlIndex + index < GetSize()) {
            auto node = GetNode(controlIndex + index);
            if (node) {
                node->OnAnimation();
            }
        }
        if (controlIndex - index >= 0) {
            auto node = GetNode(controlIndex - index);
            if (node) {
                node->OnAnimation();
            }
        }
    }
}

void BilateralSpringAdapter::MoveNode(const RefPtr<SpringNode>& srcNode, int32_t dstIndex)
{
    RefPtr<SpringNode> dstNode;
    if (srcNode) {
        dstNode = nodes_[dstIndex];
        if (dstNode) {
            dstNode->ResetNode(srcNode->GetValue(), srcNode->GetVelocity());
        }
    } else {
        LOGD("Move node. index: %{public}d, control: %{public}d", dstIndex, GetControlIndex());
        dstNode = nodes_[dstIndex];
        if (dstNode) {
            dstNode->ResetNode(0.0, 0.0);
        }
    }
}

void BilateralSpringAdapter::SetDeltaValue(double delta)
{
    int32_t controlIndex = GetControlIndex();
    RefPtr<SpringNode> node;
    for (int32_t index = 1; index <= controlIndex; index++) {
        if (controlIndex + index < GetSize()) {
            node = GetNode(controlIndex + index);
            if (node) {
                node->SetDeltaValue(delta);
            }
        }
        if (controlIndex - index >= 0) {
            node = GetNode(controlIndex - index);
            if (node) {
                node->SetDeltaValue(delta);
            }
        }
    }
}

}; // namespace OHOS::Ace