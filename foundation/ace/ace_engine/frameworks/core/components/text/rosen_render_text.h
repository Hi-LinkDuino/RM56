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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_ROSEN_RENDER_TEXT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_ROSEN_RENDER_TEXT_H

#include "flutter/lib/ui/text/paragraph_builder.h"

#include "core/components/text/render_text.h"
#include "core/components/text_span/render_text_span.h"

namespace OHOS::Ace {

class RosenRenderText final : public RenderText {
    DECLARE_ACE_TYPE(RosenRenderText, RenderText);

public:
    RosenRenderText() = default;
    ~RosenRenderText() override = default;

    void Update(const RefPtr<Component>& component) override;
    void Paint(RenderContext& context, const Offset& offset) override;
    double GetBaselineDistance(TextBaseline textBaseline) override;
    double GetTextWidth() override;

protected:
    bool MaybeRelease() override;
    void ClearRenderObject() override;

private:
    Size Measure() override;
    uint32_t GetTextLines() override;
    int32_t GetTouchPosition(const Offset& offset) override;
    bool UpdateParagraph();
    bool UpdateParagraphAndLayout(double paragraphMaxWidth);
    Size GetSize();
    bool NeedPaint();
    bool AdaptTextSize(double paragraphMaxWidth);
    bool AdaptMinTextSize(double paragraphMaxWidth);
    bool AdaptPreferTextSize(double paragraphMaxWidth);
    bool AdaptPreferTextSizeGroup(double paragraphMaxWidth);
    bool CheckMeasureFlag();
    bool DidExceedMaxLines(double paragraphMaxWidth);
    bool IsCompatibleVersion();
    void EffectAutoMaxLines();
    // Change text direction accroding to first strong direction letter when developer set direction auto.
    void ChangeDirectionIfNeeded(const std::string& data);
    std::string ApplyWhiteSpace();
    void ApplyIndents(double width);
    std::unique_ptr<txt::Paragraph> paragraph_;

    double paragraphNewWidth_ = 0.0;
    double lastLayoutMaxWidth_ = 0.0;
    double lastLayoutMinWidth_ = 0.0;
    double lastLayoutMaxHeight_ = 0.0;
    double lastLayoutMinHeight_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_ROSEN_RENDER_TEXT_H
