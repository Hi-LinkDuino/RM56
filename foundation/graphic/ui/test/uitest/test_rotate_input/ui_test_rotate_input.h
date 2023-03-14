/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef UI_TEST_ROTATE_INPUT_H
#define UI_TEST_ROTATE_INPUT_H

#include "components/ui_list.h"
#include "components/ui_scroll_view.h"
#include "components/ui_slider.h"
#include "components/ui_swipe_view.h"
#include "components/ui_time_picker.h"
#include "ui_test.h"

#if ENABLE_ROTATE_INPUT
namespace OHOS {
class UITestRotateInput : public UITest, UIView::OnTouchListener {
public:
    UITestRotateInput() {}
    ~UITestRotateInput() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    void UIKit_Rotate_Event_List_001();
    void UIKit_Rotate_Event_Scroll_View_002();
    void UIKit_Rotate_Event_Swipe_View_003();
    void UIKit_Rotate_Event_Slider_004();
    void UIKit_Rotate_Event_Time_Picker_005();
    void UIKit_Rotate_Event_Picker_006();
    bool OnPress(UIView& view, const PressEvent& event) override;

private:
    void SetLastPos(UIView* view);
    void CreateLabelButton(int16_t x, int16_t y, UIView::OnClickListener* listener, const char* text);
    void SetTestLabelButton(UILabel* label, UIView* view);
    UIScrollView* container_ = nullptr;
    UIList* list_ = nullptr;
    UIScrollView* scroll_ = nullptr;
    UISwipeView* swipe_ = nullptr;
    UITimePicker* timePicker_ = nullptr;
    UISlider* slider_ = nullptr;
    List<const char*>* adapterData_  = nullptr;
    TextAdapter* adapter_ = nullptr;
    int16_t lastX_ = 0;
    int16_t lastY_ = 0;
    UIPicker* picker_ = nullptr;
    UIView::OnClickListener* listener1_ = nullptr;
    UIView::OnClickListener* listener2_ = nullptr;
    UIView::OnClickListener* listener3_ = nullptr;
    UIView::OnClickListener* listener4_ = nullptr;
    UIView::OnClickListener* listener5_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_ROTATE_INPUT_H
#endif
