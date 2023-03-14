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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_RENDER_SELECT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_RENDER_SELECT_H

#include "core/components/common/properties/color.h"
#include "core/components/select/select_component.h"
#include "core/components/touch_listener/render_touch_listener.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderSelect : public RenderNode {
    DECLARE_ACE_TYPE(RenderSelect, RenderNode);

public:
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

    void OnPaintFinish() override
    {
        if (onFocusCallback_) {
            onFocusCallback_();
        }
    }

    RefPtr<SelectComponent> GetSelectComponent() const
    {
        return data_;
    }

    void SetFocusCallback(const std::function<void()>& value)
    {
        onFocusCallback_ = value;
    }

    void SetClickRecognizer(const RefPtr<ClickRecognizer>& clickRecognizer)
    {
        clickRecognizer_ = clickRecognizer;
    }

    void SetRawRecognizer(const RefPtr<RawRecognizer>& rawRecognizer)
    {
        rawRecognizer_ = rawRecognizer;
    }

protected:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

    Size selectSize_;
    RefPtr<ClickRecognizer> clickRecognizer_;
    RefPtr<RawRecognizer> rawRecognizer_;
    RefPtr<SelectComponent> data_;
    std::function<void()> onFocusCallback_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_RENDER_SELECT_H
