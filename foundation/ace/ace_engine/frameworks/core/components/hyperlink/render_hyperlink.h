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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_HYPERLINK_RENDER_HYPERLINK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_HYPERLINK_RENDER_HYPERLINK_H

#include "core/components/common/properties/color.h"
#include "core/components/flex/render_flex.h"
#include "core/components/hyperlink/hyperlink_component.h"
#include "core/components/hyperlink/hyperlink_resources.h"
#include "core/gestures/click_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderHyperlink : public RenderNode {
    DECLARE_ACE_TYPE(RenderHyperlink, RenderNode);

public:
    RenderHyperlink();
    ~RenderHyperlink() override = default;

    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;

    void PerformLayout() override;

    RefPtr<HyperlinkComponent> GetHyperlinkComponent() const
    {
        return hyperlinkComponent_;
    }
    const Color& GetColor() const
    {
        return color_;
    }

private:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    void Initialize();
    void JumpToAddress();
    void PerformAddress();
    void SetChildMainColor(Color color);
    void SetImageChildColor(const RefPtr<Component> node);

    RefPtr<ClickRecognizer> clickRecognizer_;
    RefPtr<HyperlinkResources> hyperlinkResources_;
    RefPtr<HyperlinkComponent> hyperlinkComponent_;
    std::string address_;
    Color color_ = Color::TRANSPARENT;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_HYPERLINK_RENDER_HYPERLINK_H
