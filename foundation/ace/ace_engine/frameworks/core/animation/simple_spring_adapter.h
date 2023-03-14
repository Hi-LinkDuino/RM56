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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SIMPLE_SPRING_ADAPTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SIMPLE_SPRING_ADAPTER_H

#include <unordered_map>

#include "core/animation/simple_spring_node.h"
#include "core/animation/spring_adapter.h"

namespace OHOS::Ace {

class SimpleSpringAdapter : public SpringAdapter {
    DECLARE_ACE_TYPE(SimpleSpringAdapter, SpringAdapter);

public:
    void AddNode(RefPtr<SpringNode> node);

    RefPtr<SpringNode> GetNext(const RefPtr<SpringNode>& node) override;

    int32_t GetSize() const override
    {
        return nodes_.size();
    }

    RefPtr<SpringNode> GetControlNode() const override
    {
        return GetNode(GetControlIndex());
    }

    RefPtr<SpringNode> GetNode(int32_t index) const override;

    int32_t GetControlIndex() const override
    {
        return 0;
    }

    std::string DumpNodes();

    void FlushAnimation() override;

    virtual void SetDeltaValue(double delta)
    {
        for (int32_t index = 0; index < GetSize(); index++) {
            auto node = GetNode(index);
            if (node) {
                node->SetDeltaValue(delta);
            }
        }
    }

protected:
    std::unordered_map<int32_t, RefPtr<SpringNode>> nodes_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SIMPLE_SPRING_ADAPTER_H
