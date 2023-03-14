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

#ifndef UI_TEST_EVENT_INJECTOR_H
#define UI_TEST_EVENT_INJECTOR_H

#include "graphic_config.h"

#if ENABLE_DEBUG
#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "dock/input_device.h"
#include "ui_test.h"
#include "layout/grid_layout.h"

namespace OHOS {
class TestEventInjectorView;
class UITestEventInjector : public UITest, public UIView::OnClickListener {
public:
    UITestEventInjector() {}
    ~UITestEventInjector() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

    void UIKit_Event_Injector_Click_Event_001();
    void UIKit_Event_Injector_Drag_Event_002();
    void UIKit_Event_Injector_Long_Press_Event_003();
    void UIKit_Event_Injector_Key_Event_004();
    void UIKit_Event_Injector_Up_to_Down_005();
    void UIKit_Event_Injector_Down_to_Up_006();
    void UIKit_Event_Injector_Left_to_Right_007();
    void UIKit_Event_Injector_Right_to_Left_008();
    void UIKit_Event_Injector_ULeft_to_LRight_009();
    void UIKit_Event_Injector_LRight_to_ULeft_010();

private:
    UIScrollView* container_ = nullptr;
    GridLayout* layout_ = nullptr;
    UILabelButton* clickBtn_ = nullptr;
    UILabelButton* dragBtn_ = nullptr;
    UILabelButton* longPressBtn_ = nullptr;
    UILabelButton* keyBtn_ = nullptr;
    UILabelButton* upToDownBtn_ = nullptr;
    UILabelButton* downToUpBtn_ = nullptr;
    UILabelButton* leftToRightBtn_ = nullptr;
    UILabelButton* rightToLeftBtn_ = nullptr;
    UILabelButton* uLeftTolRightBtn_ = nullptr;
    UILabelButton* lRightTouLeftBtn_ = nullptr;
    UILabelButton* increaseDragTimeBtn_ = nullptr;
    UILabelButton* decreaseDragTimeBtn_ = nullptr;
    UILabelButton* DragTimeDisplayBtn_ = nullptr;
    TestEventInjectorView* clickTestView_ = nullptr;
    TestEventInjectorView* dragTestView_ = nullptr;
    TestEventInjectorView* longPressTestView_ = nullptr;
    TestEventInjectorView* KeyEventTestView_ = nullptr;
    UIScrollView* scrollTestView_ = nullptr;

    int16_t lastX_ = 0;
    int16_t lastY_ = 0;
    uint16_t dragTime_ = 100; // 100: 100ms

    void InnerTest(const char* title, bool touchable, bool draggable, bool dragParent,
        const char* btnTitle, UILabelButton* btn, TestEventInjectorView*& testView);
    void SetLastPos(UIView* view);
    void SetUpButton(UILabelButton* btn, const char* title);
    void SetUpScrollView();
    void IncreaseDragTime();
    void DecreaseDragTime();
    void DragTimeDisplay();
    void SetDragTimeDisplay(uint16_t dragTime);
};
} // namespace OHOS
#endif // UI_TEST_EVENT_INJECTOR_H
#endif // ENABLE_DEBUG
