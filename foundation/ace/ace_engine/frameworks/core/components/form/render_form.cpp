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

#include "core/components/form/render_form.h"

#include "base/log/event_report.h"

namespace OHOS::Ace {

void RenderForm::Update(const RefPtr<Component>& component)
{
    auto form = AceType::DynamicCast<FormComponent>(component);

    Dimension rootWidht = 0.0_vp;
    Dimension rootHeight = 0.0_vp;
    if (form) {
        rootWidht = form->GetWidth();
        rootHeight = form->GetHeight();
    }

    if (rootWidht_ == rootWidht && rootHeight_ == rootHeight) {
        LOGE("same size, not resize render form");
        return;
    }

    if (rootWidht.IsValid() && rootHeight.IsValid()) {
        drawSize_ = Size(NormalizeToPx(rootWidht), NormalizeToPx(rootHeight));
    }
    rootWidht_ = rootWidht;
    rootHeight_ = rootHeight;

    MarkNeedLayout();
}

void RenderForm::PerformLayout()
{
    if (!NeedLayout()) {
        return;
    }

    SetLayoutSize(drawSize_);
    SetNeedLayout(false);
    MarkNeedRender();
}

bool RenderForm::TouchTest(const Point& globalPoint,
    const Point& parentLocalPoint, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    LOGI("formcomponent begin touch event");
    Point transformPoint = GetTransformPoint(parentLocalPoint);
    if (!InTouchRectList(transformPoint, GetTouchRectList())) {
        return false;
    }
    auto subContext = GetSubPipelineContext();
    if (!subContext) {
        LOGE("subContext is null");
        return false;
    }
    subContext->SetPluginEventOffset(GetGlobalOffset());

    auto context = GetContext().Upgrade();
    if (context) {
        context->SetTouchPipeline(WeakPtr<PipelineContext>(subContext));
    }
    return true;
}

}; // namespace OHOS::Ace
