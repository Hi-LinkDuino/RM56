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

#ifndef UI_TEST_LABEL_H
#define UI_TEST_LABEL_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestLabel : public UITest, public UIView::OnClickListener {
public:
    UITestLabel() {}
    ~UITestLabel() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    void CreateLabels(UIViewGroup* uiViewGroup);

    /**
     * @brief Test display dynamic text
     */
    void UIKit_UILabel_Test_Display_001();
    void UIKit_UILabel_Test_Display_002();
    void UIKit_UILabel_Test_Display_003();

    bool OnClick(UIView& view, const ClickEvent& event) override;
    void ExpandClick(UIView& view, const ClickEvent& event) const;

private:
    UIScrollView* container_ = nullptr;
    UILabel* uiLabel = nullptr;
    UILabelButton* labelFontSizeBtn1_ = nullptr;
    UILabelButton* labelFontSizeBtn2_ = nullptr;
    UILabelButton* labelFontSizeBtn3_ = nullptr;
    UILabelButton* labelHorAlignBtn1_ = nullptr;
    UILabelButton* labelHorAlignBtn2_ = nullptr;
    UILabelButton* labelHorAlignBtn3_ = nullptr;
    UILabelButton* labelColorBtn1_ = nullptr;
    UILabelButton* labelColorBtn2_ = nullptr;
    UILabelButton* labelColorBtn3_ = nullptr;
    UILabelButton* labelBeyondBtn1_ = nullptr;
    UILabelButton* labelBeyondBtn2_ = nullptr;
    UILabelButton* labelBeyondBtn3_ = nullptr;
    UILabelButton* labelLineBtn1_ = nullptr;
    UILabelButton* labelLineBtn2_ = nullptr;
    UILabelButton* labelVerAlignlBtn1_ = nullptr;
    UILabelButton* labelVerAlignlBtn2_ = nullptr;
    UILabelButton* labelVerAlignlBtn3_ = nullptr;
    UILabelButton* labelDirectionBtn1_ = nullptr;
    UILabelButton* labelDirectionBtn2_ = nullptr;
    UILabelButton* labelSizeBtn1_ = nullptr;
    UILabelButton* labelSizeBtn2_ = nullptr;

    void InnerTestTitle(const char* title);
    UILabelButton* SetUpButton(const char* title, int16_t x, int16_t y, UIViewGroup* uiViewGroup,
        int16_t width = 80, int16_t height = 40)
    {
        if (uiViewGroup == nullptr) {
            return nullptr;
        }
        UILabelButton* btn = new UILabelButton();
        btn->SetPosition(x, y, width, height);
        btn->SetText(title);
        btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
        btn->SetOnClickListener(this);
        btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
        btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
        btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
        btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
        btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
        btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
        uiViewGroup->Add(btn);
        return btn;
    }
};
} // namespace OHOS
#endif // UI_TEST_LABEL_H
