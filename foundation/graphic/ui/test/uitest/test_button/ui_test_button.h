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

#ifndef UI_TEST_BUTTON_H
#define UI_TEST_BUTTON_H

#include "components/ui_button.h"
#include "components/ui_checkbox.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestBUTTON : public UITest {
public:
    UITestBUTTON() {}
    ~UITestBUTTON() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    /**
     * @brief Test Checkbox Function
     */
    void UIKit_Check_Box_Test_001();

    /**
     * @brief Test Checkbox's SetImage Function
     */
    void UIKit_Check_Box_Test_002() const;

    /**
     * @brief Test Radiobutton Function
     */
    void UIKit_Radio_Button_Test_001();

    /**
     * @brief Test Radiobutton's SetImage Function
     */
    void UIKit_Radio_Button_Test_002() const;

    /**
     * @brief Test Togglebutton Function
     */
    void UIKit_Toggle_Button_Test_001();

    /**
     * @brief Test Togglebutton's SetImage Function
     */
    void UIKit_Toggle_Button_Test_002();

    /**
     * @brief Test button Function
     */
    void UIKit_Button_Test_001();

private:
    void UIKit_Button_Test_002(UIScrollView* container, UIButton* button);
    UIViewGroup* CreateButtonGroup(int16_t posX, int16_t posY, int16_t width, int16_t height,
                                    UICheckBox::OnChangeListener** listener,
                                    UIViewType type = UI_CHECK_BOX,
                                    const char* name = "aa");
    static constexpr int16_t CHANGE_SIZE = 10;
    UIScrollView* container_ = nullptr;

    UICheckBox::OnChangeListener* checkBoxChangeListener_ = nullptr;
    UICheckBox::OnChangeListener* checkBoxChangeListener1_ = nullptr;
    UICheckBox::OnChangeListener* checkBoxChangeListener2_ = nullptr;
    UICheckBox::OnChangeListener* radioChangeListener_ = nullptr;
    UICheckBox::OnChangeListener* radioChangeListener1_ = nullptr;
    UICheckBox::OnChangeListener* radioChangeListener2_ = nullptr;
    UICheckBox::OnChangeListener* toggleChangeListener_ = nullptr;
    UICheckBox::OnChangeListener* toggleChangeListener1_ = nullptr;
    UICheckBox::OnChangeListener* toggleChangeListener2_ = nullptr;

    UIView::OnClickListener* clickEnableVisiableListener_ = nullptr;
    UIView::OnClickListener* clickDisableVisiableListener_ = nullptr;
    UIView::OnClickListener* clickEnableTouchableListener_ = nullptr;
    UIView::OnClickListener* clickDisableTouchableListener_ = nullptr;
    UIView::OnClickListener* clickColorToWhiteListener_ = nullptr;
    UIView::OnClickListener* clickColorToRedListener_ = nullptr;
    UIView::OnClickListener* clickRevetColorListener_ = nullptr;
    UIView::OnClickListener* clickRevetToOriginListener_ = nullptr;
    UIView::OnClickListener* clickBigListener_ = nullptr;
    UIView::OnClickListener* clickLeftListener_ = nullptr;
    UIView::OnClickListener* clickRightListener_ = nullptr;
    UIView::OnClickListener* clickUpListener_ = nullptr;
    UIView::OnClickListener* clickDownListener_ = nullptr;
    UIView::OnClickListener* clickSmallListener_ = nullptr;
    UIView::OnClickListener* enableAnimationListener_ = nullptr;
    UIView::OnClickListener* disableAnimationListener_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_BUTTON_H
