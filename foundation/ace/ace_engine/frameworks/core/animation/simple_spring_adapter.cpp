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

#include "core/animation/simple_spring_adapter.h"

namespace OHOS::Ace {

void SimpleSpringAdapter::AddNode(RefPtr<SpringNode> node)
{
    if (!node) {
        LOGE("AddNode failed. node is null.");
        return;
    }
    nodes_[node->GetIndex()] = node;
    NotifyNodeAdd(node);
}

RefPtr<SpringNode> SimpleSpringAdapter::GetNext(const RefPtr<SpringNode>& node)
{
    if (!node) {
        LOGE("Get next failed. node is null.");
        return nullptr;
    }
    return GetNode(node->GetIndex() + 1);
}

RefPtr<SpringNode> SimpleSpringAdapter::GetNode(int32_t index) const
{
    auto iter = nodes_.find(index);
    if (iter == nodes_.end()) {
        return nullptr;
    }
    return iter->second;
}

std::string SimpleSpringAdapter::DumpNodes()
{
    int32_t nodeSize = static_cast<int32_t>(nodes_.size());
    int32_t zeroCount = 0;
    std::string dumpInfo;
    for (int32_t index = 0; index < nodeSize; index++) {
        auto node = AceType::DynamicCast<SimpleSpringNode>(nodes_.at(index));
        if (!node) {
            continue;
        }
        dumpInfo += std::to_string(node->GetIndex()) + ":" + std::to_string(node->GetValue()) + ", " +
                    std::to_string(node->GetTransferParams()->Stiffness()) + "; ";
        if (NearZero(node->GetValue())) {
            zeroCount++;
        }
    }
    if (zeroCount == nodeSize) {
        return std::string(" All Zeros");
    } else {
        return dumpInfo;
    }
}

void SimpleSpringAdapter::FlushAnimation()
{
    // flush from control to edge.
    for (int32_t index = 0; index < GetSize(); index++) {
        auto node = GetNode(index);
        if (node) {
            node->OnAnimation();
        }
    }
}

} // namespace OHOS::Ace