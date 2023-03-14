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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PADDING_PADDING_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PADDING_PADDING_COMPONENT_H

#include "base/utils/macros.h"
#include "core/components/common/properties/edge.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

// A padding component.
class ACE_EXPORT PaddingComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(PaddingComponent, SoleChildComponent);

public:
    PaddingComponent() = default;
    ~PaddingComponent() override = default;

    RefPtr<Element> CreateElement() override;

    RefPtr<RenderNode> CreateRenderNode() override;

    const Edge& GetPadding() const
    {
        return padding_;
    }

    void SetPadding(const Edge& padding)
    {
        padding_ = padding;
    }

    void SetPaddingLeft(const Dimension& value)
    {
        padding_.SetLeft(value);
    }

    void SetPaddingTop(const Dimension& value)
    {
        padding_.SetTop(value);
    }

    void SetPaddingRight(const Dimension& value)
    {
        padding_.SetRight(value);
    }

    void SetPaddingBottom(const Dimension& value)
    {
        padding_.SetBottom(value);
    }

private:
    Edge padding_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PADDING_PADDING_COMPONENT_H
