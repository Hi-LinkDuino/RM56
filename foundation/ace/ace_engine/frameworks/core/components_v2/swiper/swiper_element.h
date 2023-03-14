/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_SWIPER_SWIPER_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_SWIPER_SWIPER_ELEMENT_H

#include "core/components_v2/common/element_proxy.h"
#include "core/focus/focus_node.h"
#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace::V2 {

class SwiperElement : public RenderElement, public FocusGroup, public FlushEvent, private V2::ElementProxyHost {
    DECLARE_ACE_TYPE(SwiperElement, RenderElement, FocusGroup, FlushEvent);

public:
    void PerformBuild() override;
    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;

    bool BuildChildByIndex(int32_t index);
    void DeleteChildByIndex(int32_t index);
    void OnPostFlush() override;

private:
    RefPtr<RenderNode> CreateRenderNode() override;
    void ApplyRenderChild(const RefPtr<RenderElement>& renderChild) override;

    RefPtr<Element> OnUpdateElement(const RefPtr<Element>& element, const RefPtr<Component>& component) override;
    RefPtr<Component> OnMakeEmptyComponent() override;
    void OnDataSourceUpdated(size_t startIndex) override;
};

} // namespace OHOS::Ace::V2
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_SWIPER_SWIPER_ELEMENT_H
