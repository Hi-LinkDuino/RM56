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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_CLOCK_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_CLOCK_H

#include "core/components/clock/clock_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMClock final : public DOMNode {
    DECLARE_ACE_TYPE(DOMClock, DOMNode);

public:
    DOMClock(NodeId nodeId, const std::string& nodeName);
    ~DOMClock() override = default;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return clockChild_;
    }
    void ResetInitializedStyle() override;
    void InitializeStyle() override;
    void SetClockConfig(const ClockConfig& clockConfig)
    {
        clockConfig_ = clockConfig;
    }

protected:
    void PrepareSpecializedComponent() override;

    bool IsSubscriptEnable() const override
    {
        if (declaration_) {
            declaration_->SetIsSubscriptEnable(true);
        }
        return true;
    }

private:
    RefPtr<ClockComponent> clockChild_;
    ClockConfig clockConfig_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_CLOCK_H
