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

#ifndef UI_TEST_VIEW_PERCENT_H
#define UI_TEST_VIEW_PERCENT_H

#include <string>
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_view.h"
#include "components/ui_view_group.h"
#include "graphic_config.h"
#include "layout/flex_layout.h"
#include "layout/layout.h"
#include "ui_test.h"

namespace OHOS {
class UITestViewPercent : public UITest, public UIView::OnClickListener {
public:
    UITestViewPercent() {}
    ~UITestViewPercent() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    void SetUpButton(Layout& controller, UILabelButton& btn, const std::string title)
    {
        controller.Add(&btn);
        btn.Resize(BUTTON_WIDHT1, BUTTON_HEIGHT1);
        btn.SetText(title.c_str());
        btn.SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
        btn.SetOnClickListener(this);
        btn.SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
        btn.SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
        btn.SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
        btn.SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
        btn.SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
        btn.SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
    }
    void InitView();
    void InitFlexLayout();
    void InitViewGroup() const;
    void InitContainer();
    void AddElement(int16_t index);
    UIScrollView* container_ = nullptr;
    FlexLayout* flexLayout_ = nullptr;
    UIViewGroup* viewGroup = nullptr;
    UIView* view1_ = nullptr;
    UIView* view2_ = nullptr;
    UIView* view3_ = nullptr;
    UILabelButton* resetfBtn_ = nullptr;
    UILabelButton* BiggerBtn_ = nullptr;
    UILabelButton* SmallerBtn_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_VIEW_PERCENT_H

