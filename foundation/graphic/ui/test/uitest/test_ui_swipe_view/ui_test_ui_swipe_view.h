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

#ifndef UI_TEST_UI_SWIPE_VIEW_H
#define UI_TEST_UI_SWIPE_VIEW_H

#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_swipe_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestUISwipeView : public UITest, UIView::OnClickListener {
public:
    UITestUISwipeView() {}
    ~UITestUISwipeView() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

    void UIKit_Swipe_View_Test_Horizontal_001();
    void UIKit_Swipe_View_Test_Horizontal_002();
    void UIKit_Swipe_View_Test_Horizontal_003();
    void UIKit_Swipe_View_Test_Ver_001();
    void UIKit_Swipe_View_Test_Ver_002();
    void UIKit_Swipe_View_Test_Remove_001();
    void UIKit_Swipe_View_Test_Align_001(UISwipeView::AlignMode alignMode);
    void UIKit_Swipe_View_Test_SetCurrentPage();

private:
    void SetUpButton(UILabelButton* btn, const char* title);
    void SetLastPos(UIView* view);
    UIScrollView* container_ = nullptr;
    UISwipeView* currentSwipe_ = nullptr;
    UILabelButton* addBtnInHead_ = nullptr;
    UILabelButton* addBtnInTail_ = nullptr;
    UILabelButton* addBtnInMid_ = nullptr;
    UILabelButton* removeHeadBtn_ = nullptr;
    UILabelButton* removeMidBtn_ = nullptr;
    UILabelButton* removeAllBtn_ = nullptr;
    UILabelButton* loopBtn_ = nullptr;
    UILabelButton* changePageBtn_ = nullptr;
    bool loop_ = false;
    int16_t lastX_ = 0;
    int16_t lastY_ = 0;
    int16_t btnNum_ = 0;
};
} // namespace OHOS
#endif // UI_TEST_UI_SWIPE_VIEW_H
