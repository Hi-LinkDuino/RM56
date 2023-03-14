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

#include "core/components/select/render_select.h"

#include "base/log/log.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {

void RenderSelect::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    LOGD("RenderSelect::OnTouchTestHit(%{public}lf, %{public}lf).", coordinateOffset.GetX(), coordinateOffset.GetY());
    if (clickRecognizer_) {
        clickRecognizer_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(clickRecognizer_);
        clickRecognizer_->SetIsExternalGesture(true);
    }

    if (rawRecognizer_) {
        rawRecognizer_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(rawRecognizer_);
    }
}

void RenderSelect::Update(const RefPtr<Component>& component)
{
    data_ = AceType::DynamicCast<SelectComponent>(component);
    MarkNeedLayout();
}

void RenderSelect::PerformLayout()
{
    auto children = GetChildren();
    if (children.empty()) {
        LOGE("select: there has no child.");
        return;
    }

    auto child = children.front();
    if (!child) {
        LOGE("select: there has no child now in render select.");
        return;
    }

    LOGD("RenderSelect::PerformLayout: %{public}s.", AceType::TypeName(child));
    child->Layout(GetLayoutParam());
    selectSize_ = child->GetLayoutSize();
    selectSize_ = GetLayoutParam().Constrain(selectSize_);
    SetLayoutSize(selectSize_);
}

} // namespace OHOS::Ace
