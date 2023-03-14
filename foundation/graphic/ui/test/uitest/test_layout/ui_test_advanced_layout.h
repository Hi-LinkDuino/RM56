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

#ifndef UI_TEST_ADVANCED_LAYOUT_H
#define UI_TEST_ADVANCED_LAYOUT_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "layout/flex_layout.h"
#include "layout/grid_layout.h"
#include "ui_test.h"

namespace OHOS {
class UITestAdvancedLayout : public UITest, public UIView::OnClickListener {
public:
    UITestAdvancedLayout() : withMargin_(false) {}
    ~UITestAdvancedLayout() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    /**
     * @brief Test flex layout function.
     */
    void UIKit_Layout_Test_FlexLayout_001();
    /**
     * @brief Test grid layout function.
     */
    void UIKit_Layout_Test_GridLayout_001();

    bool OnClick(UIView& view, const ClickEvent& event) override;
    void OnClickButton(const UIView& view);

private:
    UIScrollView* container_ = nullptr;
    GridLayout* flexController_ = nullptr;
    GridLayout* gridController_ = nullptr;
    FlexLayout* fTarget_ = nullptr;
    GridLayout* gTarget_ = nullptr;

    UILabelButton* resetfBtn_ = nullptr;
    UILabelButton* horfBtn_ = nullptr;
    UILabelButton* horfRBtn_ = nullptr;
    UILabelButton* verfBtn_ = nullptr;
    UILabelButton* verfRBtn_ = nullptr;
    UILabelButton* wrapBtn_ = nullptr;
    UILabelButton* addElefBtn_ = nullptr;
    UILabelButton* addTextElefBtn_ = nullptr;
    UILabelButton* majorStartBtn_ = nullptr;
    UILabelButton* majorEndBtn_ = nullptr;
    UILabelButton* majorCenterBtn_ = nullptr;
    UILabelButton* majorEvenBtn_ = nullptr;
    UILabelButton* majorAroundBtn_ = nullptr;
    UILabelButton* majorBetBtn_ = nullptr;
    UILabelButton* secStartBtn_ = nullptr;
    UILabelButton* secEndBtn_ = nullptr;
    UILabelButton* secCenterBtn_ = nullptr;
    UILabelButton* secInvalidBtn_ = nullptr;
    UILabelButton* marginfBtn_ = nullptr;
    UILabelButton* layoutChildrenfBtn_ = nullptr;
    UILabelButton* horgBtn_ = nullptr;
    UILabelButton* horgRBtn_ = nullptr;
    UILabelButton* vergBtn_ = nullptr;
    UILabelButton* vergRBtn_ = nullptr;
    UILabelButton* incRowsBtn_ = nullptr;
    UILabelButton* decRowsBtn_ = nullptr;
    UILabelButton* incColsBtn_ = nullptr;
    UILabelButton* decColsBtn_ = nullptr;
    UILabelButton* addElegBtn_ = nullptr;
    UILabelButton* resetgBtn_ = nullptr;
    UILabelButton* margingBtn_ = nullptr;
    UILabelButton* layoutChildrengBtn_ = nullptr;

    int16_t w_ = 10; // 10: width
    int16_t h_ = 10; // 10: height
    int16_t rows_ = 0;
    int16_t cols_ = 0;
    bool withMargin_;

    void SetUpButton(Layout* controller, UILabelButton* btn, const char* title)
    {
        controller->Add(btn);
        btn->Resize(BUTTON_WIDHT2, BUTTON_HEIGHT2);
        btn->SetText(title);
        btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        btn->SetOnClickListener(this);
        btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
        btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
        btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
        btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
        btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
        btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
    }

    void AddElement(Layout* layout)
    {
        UIView* view = new UIView();
        view->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
        view->Resize(w_++, h_++);
        if (withMargin_) {
            view->SetStyle(STYLE_MARGIN_LEFT, 10);   // 10: margin left
            view->SetStyle(STYLE_MARGIN_RIGHT, 15);  // 15: margin right
            view->SetStyle(STYLE_MARGIN_TOP, 20);    // 20: margin top
            view->SetStyle(STYLE_MARGIN_BOTTOM, 25); // 25: margin bottom
        }
        layout->Add(view);
    }

    void AddTextElement(Layout* layout)
    {
        UILabel* label = new UILabel();
        label->Resize(80, 30);
        label->SetLineBreakMode(0);
        label->SetText("Hello, OHOS!");
        layout->Add(label);
    }

    void Clear(Layout* layout) const
    {
        if (layout == nullptr) {
            return;
        }
        UIView* child = layout->GetChildrenHead();
        while (child != nullptr) {
            UIView* temp = child;
            child = child->GetNextSibling();
            layout->Remove(temp);
            delete temp;
        }
    }
};
} // namespace OHOS
#endif // UI_TEST_INPUT_EVENT_H
