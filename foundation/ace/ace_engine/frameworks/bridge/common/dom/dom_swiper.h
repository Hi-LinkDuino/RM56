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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SWIPER_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SWIPER_H

#include "core/components/swiper/swiper_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMSwiper final : public DOMNode {
    DECLARE_ACE_TYPE(DOMSwiper, DOMNode);

public:
    DOMSwiper(NodeId nodeId, const std::string& nodeName);
    ~DOMSwiper() override = default;

    void InitializeStyle() override;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return swiperChild_;
    }
    void AdjustSpecialParamInLiteMode() override;

protected:
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    void PrepareSpecializedComponent() override;

private:
    RefPtr<SwiperComponent> swiperChild_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SWIPER_H
