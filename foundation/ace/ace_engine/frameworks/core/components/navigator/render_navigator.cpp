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

#include "core/components/navigator/render_navigator.h"

#include "base/log/event_report.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/page/page_target.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {

RenderNavigator::RenderNavigator()
{
    Initialize();
}

RefPtr<RenderNode> RenderNavigator::Create()
{
    return AceType::MakeRefPtr<RenderNavigator>();
}

void RenderNavigator::Initialize()
{
    auto wp = AceType::WeakClaim(this);
    clickRecognizer_ = AceType::MakeRefPtr<ClickRecognizer>();
    clickRecognizer_->SetOnClick([wp](const ClickInfo& info) {
        LOGI("navigator OnClick called");
        auto navigator = wp.Upgrade();
        if (!navigator) {
            return;
        }
        const auto context = navigator->GetContext().Upgrade();
        if (context && context->GetIsDeclarative()) {
            navigator->HandleClickEvent(info);
        } else {
            navigator->HandleClickEvent();
        }
        navigator->NavigatePage();
    });
}

void RenderNavigator::HandleClickEvent(const ClickInfo& info)
{
    if (onClickWithInfo_) {
        onClickWithInfo_(info);
    }
}

void RenderNavigator::HandleClickEvent()
{
    if (onClick_) {
        onClick_();
    }
}

void RenderNavigator::NavigatePage()
{
    auto pipelineContext = GetContext().Upgrade();
    if (!pipelineContext) {
        LOGE("pipelineContext is null");
        return;
    }

    pipelineContext->NavigatePage(static_cast<uint8_t>(type_), PageTarget(uri_, targetContainer_), params_);
}

void RenderNavigator::Update(const RefPtr<Component>& component)
{
    const RefPtr<NavigatorComponent> navigator = AceType::DynamicCast<NavigatorComponent>(component);
    if (!navigator) {
        LOGE("Update error, navigator component is null");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }

    uri_ = navigator->GetUri();
    params_ = navigator->GetParams();
    type_ = navigator->GetType();
    active_ = navigator->GetActive();
    isDefHeight_ = navigator->IsDefHeight();
    isDefWidth_ = navigator->IsDefWidth();
    onClickWithInfo_ = AceAsyncEvent<void(const ClickInfo&)>::Create(navigator->GetClickedEventId(), context_);
    onClick_ = AceAsyncEvent<void()>::Create(navigator->GetClickedEventId(), context_);
    LOGI("navigator Update uri = %{public}s type = %{public}d", uri_.c_str(), type_);

    if (active_ == true) {
        NavigatePage();
    }
    MarkNeedLayout();
}

void RenderNavigator::PerformLayout()
{
    LOGD("RenderNavigator PerformLayout");
    if (!GetChildren().empty()) {
        LayoutParam innerLayout = GetLayoutParam();
        auto child = GetChildren().front();
        child->Layout(innerLayout);
        Size maxSize = innerLayout.GetMaxSize();
        double maxWidth = maxSize.Width();
        double maxHeight = maxSize.Height();
        if (!isDefHeight_) {
            maxHeight = child->GetLayoutSize().Height();
        }
        if (!isDefWidth_) {
            maxWidth = child->GetLayoutSize().Width();
        }
        SetLayoutSize(Size(maxWidth, maxHeight));
        child->SetPosition(Alignment::GetAlignPosition(GetLayoutSize(), child->GetLayoutSize(), Alignment::CENTER));
    } else {
        SetLayoutSize(GetLayoutParam().GetMaxSize());
    }
}

void RenderNavigator::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (!clickRecognizer_) {
        return;
    }
    clickRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(clickRecognizer_);
}

} // namespace OHOS::Ace
