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

#ifndef UI_TEST_SCREENSHOT_H
#define UI_TEST_SCREENSHOT_H

#include "graphic_config.h"
#if ENABLE_DEBUG
#include "components/ui_scroll_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "dfx/ui_screenshot.h"
#include "ui_test.h"

namespace OHOS {
class UITestScreenshot : public UITest {
public:
    UITestScreenshot() {}
    ~UITestScreenshot() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    /**
     * @brief Test screenshot Function
     */
    void UIKit_SCREENSHOT_TEST_001();

private:
    UIScrollView* container_ = nullptr;
    UIView::OnClickListener* clickLeftListener_ = nullptr;
    UIView::OnClickListener* clickRightListener_ = nullptr;
    UILabelButton* SetUpButton(const char* title)
    {
        UILabelButton* btn = new UILabelButton();
        btn->SetText(title);
        btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
        btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
        btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
        btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
        btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
        btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
        btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
        return btn;
    }
}; // namespace OHOS
}
#endif // ENABLE_DEBUG
#endif // UI_TEST_SCREENSHOT_H
