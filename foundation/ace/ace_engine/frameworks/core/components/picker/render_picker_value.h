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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_RENDER_PICKER_VALUE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_RENDER_PICKER_VALUE_H

#include <functional>

#include "core/components/box/render_box.h"
#include "core/gestures/click_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

using PickerValueCallback = std::function<void()>;

class RenderPickerValue : public RenderNode {
    DECLARE_ACE_TYPE(RenderPickerValue, RenderNode);

public:
    explicit RenderPickerValue(const PickerValueCallback& clickCallback);
    ~RenderPickerValue() override = default;

    void Update(const RefPtr<Component>& component) override {}

    void PerformLayout() override;

    void OnPaintFinish() override
    {
        if (focus_) {
            HandleAnimation();
        }
    }

    void HandleClickCallback() const;

    void HandleFocus(const RefPtr<RenderBox>& box)
    {
        HandleFocus(box, focus_);
    }

    void HandleFocus(const RefPtr<RenderBox>& box, bool focus);

    void HandleTextFocus(bool focus);

    void HandleAnimation()
    {
        HandleAnimation(box_);
    }

    void HandleAnimation(const RefPtr<RenderBox>& box);

protected:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override
    {
        result.emplace_back(clickRecognizer_);
    }

private:
    PickerValueCallback clickCallback_;
    RefPtr<ClickRecognizer> clickRecognizer_;
    RefPtr<RenderBox> box_;
    bool focus_ = false;
    Color normalBackColor_;
    Color focusBackColor_;
    Color focusAnimationColor_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_RENDER_PICKER_VALUE_H
