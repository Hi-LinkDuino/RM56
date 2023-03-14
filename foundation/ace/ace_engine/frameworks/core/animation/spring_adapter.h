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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_ADAPTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_ADAPTER_H

#include "base/memory/ace_type.h"

namespace OHOS::Ace {

class SpringNode;

class SpringAdapter : public AceType {
    DECLARE_ACE_TYPE(SpringAdapter, AceType);

public:
    class AdapterObserve : public AceType {
        DECLARE_ACE_TYPE(AdapterObserve, AceType);

    public:
        virtual void OnControlNodeChange() = 0;
        virtual void OnNodeAdd(RefPtr<SpringNode>& node) = 0;
        virtual void OnNodeDelete(RefPtr<SpringNode>& node) = 0;
    };

    void SetObserve(WeakPtr<AdapterObserve> observe)
    {
        observe_ = observe;
    }

    virtual RefPtr<SpringNode> GetNext(const RefPtr<SpringNode>& node) = 0;

    virtual int32_t GetSize() const = 0;

    virtual RefPtr<SpringNode> GetControlNode() const = 0;

    virtual int32_t GetControlIndex() const = 0;

    virtual RefPtr<SpringNode> GetNode(int32_t index) const = 0;

    void NotifyControlIndexChange()
    {
        if (observe_.Upgrade()) {
            observe_.Upgrade()->OnControlNodeChange();
        }
    }

    void NotifyNodeAdd(RefPtr<SpringNode>& node)
    {
        if (observe_.Upgrade()) {
            observe_.Upgrade()->OnNodeAdd(node);
        }
    }

    void NotifyNodeDelete(RefPtr<SpringNode>& node)
    {
        if (observe_.Upgrade()) {
            observe_.Upgrade()->OnNodeDelete(node);
        }
    }

    virtual void FlushAnimation() = 0;

    void TickAnimation(uint64_t timestamp)
    {
        if (timestamp_ == timestamp) {
            return;
        }
        FlushAnimation();
        timestamp_ = timestamp;
    }

private:
    WeakPtr<AdapterObserve> observe_;
    uint64_t timestamp_ = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_ADAPTER_H
