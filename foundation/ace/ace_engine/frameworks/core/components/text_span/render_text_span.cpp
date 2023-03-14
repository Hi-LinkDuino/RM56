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

#include "core/components/text_span/render_text_span.h"

#include "core/common/font_manager.h"

namespace OHOS::Ace {

RenderTextSpan::~RenderTextSpan()
{
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        return;
    }
    auto fontManager = pipelineContext->GetFontManager();
    if (fontManager) {
        fontManager->UnRegisterCallback(AceType::WeakClaim(this));
    }
}

void RenderTextSpan::Update(const RefPtr<Component>& component)
{
    spanComponent_ = AceType::DynamicCast<TextSpanComponent>(component);
    if (!spanComponent_) {
        return;
    }
    if (spanComponent_->HasNewStyle()) {
        spanStyle_ = spanComponent_->GetTextStyle();
    }

    MarkNeedLayout();

    // Register callback for fonts.
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        return;
    }
    auto callback = [span = AceType::WeakClaim(this)] {
        auto refPtr = span.Upgrade();
        if (refPtr) {
            refPtr->isCallbackCalled_ = true;
            refPtr->MarkNeedLayout();
        }
    };
    for (const auto& familyName : spanStyle_.GetFontFamilies()) {
        auto fontManager = pipelineContext->GetFontManager();
        if (fontManager) {
            fontManager->RegisterCallback(AceType::WeakClaim(this), familyName, callback);
        }
    }
}

bool RenderTextSpan::IsCallbackCalled() const
{
    return isCallbackCalled_;
}

const TextStyle& RenderTextSpan::GetSpanStyle() const
{
    return spanStyle_;
}

const std::string& RenderTextSpan::GetSpanData() const
{
    static std::string defaultData;
    return spanComponent_ ? spanComponent_->GetSpanData() : defaultData;
}

} // namespace OHOS::Ace
