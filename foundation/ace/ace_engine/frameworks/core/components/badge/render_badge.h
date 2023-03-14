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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BADGE_RENDER_BADGE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BADGE_RENDER_BADGE_H

#include "base/utils/system_properties.h"
#include "core/components/badge/badge_component.h"
#include "core/components/common/properties/color.h"
#include "core/components/text/render_text.h"
#include "core/components/text/text_component.h"
#include "core/gestures/click_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderBadge : public RenderNode {
    DECLARE_ACE_TYPE(RenderBadge, RenderNode);

public:
    RenderBadge();
    ~RenderBadge() override;

    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    virtual Size CalculateTextSize(const std::string& text, const TextStyle& textStyle,
        RefPtr<RenderText>& renderText) = 0;

    const RefPtr<BadgeComponent>& GetBadgeComponent() const
    {
        return badge_;
    }

protected:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    void HandleClickEvent();
    void InitialBadgeText();
    void UpdateBadgeText();
    bool ParseBadgeStatus(const std::string& label, int64_t messageCount, int64_t countLimit);

    double width_ = 0.0;
    double height_ = 0.0;
    Size badgeSize_;
    std::string textData_;
    bool showMessage_ = false;
    std::function<void()> onClick_;
    RefPtr<ClickRecognizer> clickRecognizer_;
    RefPtr<RenderText> badgeRenderText_;
    RefPtr<TextComponent> badgeTextComponent_;
    TextStyle textStyle_;
    RefPtr<BadgeComponent> badge_;
    Offset badgeChildInitialOffset_;
    double dipScale_ = 1.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BADGE_RENDER_BADGE_H
