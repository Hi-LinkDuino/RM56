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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_RENDER_PICKER_BASE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_RENDER_PICKER_BASE_H

#include <functional>
#include <vector>

#include "core/components/box/render_box.h"
#include "core/components/button/render_button.h"
#include "core/components/checkable/render_checkbox.h"
#include "core/components/gesture_listener/render_gesture_listener.h"
#include "core/components/picker/picker_base_component.h"
#include "core/components/picker/picker_text_component.h"
#include "core/components/picker/render_picker_column.h"
#include "core/components/stack/render_stack.h"
#include "core/components/triangle/render_triangle.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/drag_recognizer.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderPickerBase : public RenderNode {
    DECLARE_ACE_TYPE(RenderPickerBase, RenderNode);

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

    void OnPaintFinish() override
    {
        UpdateAccessibility();
    }

    void StartAnimation()
    {
        if (!data_) {
            return;
        }
        data_->OnAnimationPlaying();
    }

    void UpdateRenders();

    void FlushColumnsOptions()
    {
        if (switch_ && data_ && data_->GetLunar()) {
            auto lunar = data_->GetLunar();
            lunar->SetValue(data_->IsShowLunar());
            switch_->Update(lunar);
            switch_->MarkNeedLayout();
        }
        for (const auto& column : columns_) {
            column->FlushCurrentOptions();
        }
    }

    bool TouchTest(const Point& globalPoint, const Point& parentLocalPoint, const TouchRestrict& touchRestrict,
        TouchTestResult& result) override;

    const RefPtr<PickerBaseComponent>& GetPickerBaseComponent() const
    {
        return data_;
    }

    const std::vector<RefPtr<RenderPickerColumn>>& GetRenderPickerColumn() const
    {
        return columns_;
    }

    const Dimension& GetColumnHeight() const
    {
        return columnHeight_;
    }

    const std::vector<std::string>& GetRange() const
    {
        return range_;
    }

    uint32_t GetSelected() const
    {
        return selectedIndex_;
    }

    const PickerDate& GetStartDate() const
    {
        return startDateSolar_;
    }

    const PickerDate& GetEndDate() const
    {
        return endDateSolar_;
    }
    
    const PickerDate& GetSelectedDate() const
    {
        return selectedDate_;
    }

    const std::string GetType() const
    {
        return type_;
    }

protected:
    void OnTouchTestHit(const Offset&, const TouchRestrict&, TouchTestResult& result) override
    {
        if (data_ && !data_->GetSubsidiary()) {
            result.emplace_back(rawDetect_);
        }
    }

    void SetTextOnCancel(const std::function<void(void)>& value)
    {
        onTextCancel_ = value;
    }

    void SetTextOnAccept(const std::function<void(const std::string&, double)>& value)
    {
        onTextAccept_ = value;
    }

    void SetTextOnChange(const std::function<void(const std::string&, double)>& value)
    {
        onTextChange_ = value;
    }

    RefPtr<PickerBaseComponent> data_;
    std::vector<RefPtr<RenderPickerColumn>> columns_;
    RefPtr<RenderBox> box_;
    RefPtr<RenderNode> columnParent_;

    std::function<void(void)> onTextCancel_;
    std::function<void(const std::string&, double)> onTextAccept_;
    std::function<void(const std::string&, double)> onTextChange_;

private:
    void HandleFinish(bool success);
    void HandleSwitch(bool checked);
    void HandleColumnChange(const std::string& tag, bool isAdd, uint32_t index, bool needNotify);

    void GetRenders();
    void GetRenders(const RefPtr<RenderNode>& render);
    void DealRenders(const RefPtr<RenderNode>& render);
    void ClearRenders();

    void InitPickerAnimation();

    void SetButtonHandler();
    void SetSwitchHandler();

    void LayoutBoxes();
    void LayoutColumns();

    void UpdateAccessibility();
    void UpdateTextAccessibility(const RefPtr<AccessibilityNode>& node, const RefPtr<RenderText>& text);
    void UpdateButtonAccessibility(const RefPtr<AccessibilityNode>& node, const RefPtr<RenderButton>& button);
    void UpdateSwitchAccessibility(const RefPtr<AccessibilityNode>& node, const RefPtr<RenderCheckbox>& renderSwitch);
    // used for inspector node in PC preview
    void UpdatePickerDialogAccessibility(const RefPtr<AccessibilityNode>& node, const RefPtr<RenderBox>& box);

    void SetTriangleHandler();
    void HandleTriangleClick(bool value);

    enum DialogStatus {
        ACCEPT = 0,
        CANCEL,
        UPDATE,
    };

    bool switchHandlerSetted_ = false;
    bool hasLayout_ = false;
    Dimension columnHeight_;
    uint32_t selectedIndex_ = 0;
    std::vector<std::string> range_ {""};
    std::string type_ = "DatePickerType.Date";
    PickerDate startDateSolar_ = PickerDate(1970, 1, 1);
    PickerDate endDateSolar_ = PickerDate(2100, 12, 31);
    PickerDate selectedDate_ = PickerDate::Current();

    std::function<void()> onCancelCallback_;
    std::function<void(const std::shared_ptr<BaseEventInfo>&)> onChangeCallback_;
    std::function<void(const std::string&)> onColumnChangeCallback_;
    std::function<void(const std::string&)> onDialogAccept_;
    std::function<void()> onDialogCancel_;
    std::function<void(const std::string&)> onDialogChange_;
    RefPtr<RenderText> title_;
    RefPtr<RenderTriangle> triangle_;
    RefPtr<RenderDisplay> lunarDisplay_;
    RefPtr<RenderDisplay> buttonsDisplay_;
    RefPtr<RenderStack> stack_;
    RefPtr<RenderBox> outBox_;
    RefPtr<RenderButton> cancel_;
    RefPtr<RenderButton> ok_;
    RefPtr<RenderCheckbox> switch_;
    RefPtr<RenderText> lunarText_;
    RefPtr<RawRecognizer> rawDetect_ = AceType::MakeRefPtr<RawRecognizer>();
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_RENDER_PICKER_BASE_H
