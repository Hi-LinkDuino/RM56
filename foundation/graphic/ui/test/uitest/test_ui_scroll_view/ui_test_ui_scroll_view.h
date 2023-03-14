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

#ifndef UI_TEST_UI_SCROLL_VIEW_H
#define UI_TEST_UI_SCROLL_VIEW_H

#include "components/ui_label.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestUIScrollView : public UITest {
public:
    UITestUIScrollView() {}
    ~UITestUIScrollView() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void UIKit_UIScrollView_Test_bar_001();
    void UIKit_UIScrollView_Test_bar_002();
    void UIKit_UIScrollView_Test_bar_003();
    void UIKit_UIScrollView_Test_bar_004();
    void UIKit_UIScrollView_Test_Scrollable_Set_001();
    void UIKit_UIScrollView_Test_Scrollable_Set_002();
    void UIKit_UIScrollView_Test_Scroll_Blank_Set_001();
    void UIKit_UIScrollView_Test_Scroll_listener_001();

private:
    void SetLastPos(UIView* view);
    UIScrollView* container_ = nullptr;
    int16_t lastX_ = 0;
    int16_t lastY_ = 0;
};
} // namespace OHOS
#endif // UI_TEST_UI_SCROLL_VIEW_H
