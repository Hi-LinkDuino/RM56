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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MULTIMODAL_MULTIMODAL_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MULTIMODAL_MULTIMODAL_COMPONENT_H

#include "core/components/declaration/common/attribute.h"
#include "core/components/multimodal/multimodal_element.h"
#include "core/components/multimodal/render_multimodal.h"
#include "core/event/multimodal/multimodal_properties.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class MultimodalComponent final : public SoleChildComponent {
    DECLARE_ACE_TYPE(MultimodalComponent, SoleChildComponent)

public:
    explicit MultimodalComponent(int32_t pageId) : pageId_(pageId) {}
    ~MultimodalComponent() override = default;

    const CommonMultimodalAttribute& GetMultimodalProperties() const
    {
        return multimodalProperties_;
    }
    void SetMultimodalProperties(const CommonMultimodalAttribute& multimodalProperties)
    {
        multimodalProperties_ = multimodalProperties;
    }

    int32_t GetPageId() const
    {
        return pageId_;
    }

    const EventMarker& GetOnClickId() const
    {
        return onClickId_;
    }
    void SetOnClickId(const EventMarker& onClickId)
    {
        onClickId_ = onClickId;
    }

    RefPtr<Element> CreateElement() override
    {
        return MakeRefPtr<MultimodalElement>();
    }

    RefPtr<RenderNode> CreateRenderNode() override
    {
        return RenderMultimodal::Create();
    }

private:
    CommonMultimodalAttribute multimodalProperties_;
    EventMarker onClickId_;
    int32_t pageId_ = -1;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MULTIMODAL_MULTIMODAL_COMPONENT_H
