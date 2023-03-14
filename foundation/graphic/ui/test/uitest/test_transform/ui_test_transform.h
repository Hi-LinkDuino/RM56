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

#ifndef UI_TEST_TRANSFORM_H
#define UI_TEST_TRANSFORM_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_checkbox.h"
#include "components/ui_radio_button.h"
#include "layout/grid_layout.h"
#include "ui_test.h"

namespace OHOS {
enum ImageScaleMode {
    AUTO,
    TILING,
    COVER,
    CONTAIN,
    FILL,
    CENTER,
    SCALE_DOWN,
};

class UITestTransform : public UITest, public UIView::OnClickListener {
public:
    UITestTransform() {}
    ~UITestTransform() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void SetUpButton(UILabelButton* btn, const char* title);

    bool OnClick(UIView& view, const ClickEvent& event) override;

    void UIKit_Transform_Test_Rotate_001();
    void UIKit_Transform_Test_Scale_002();
    void UIKit_Transform_Test_Translate_003();

    void SetScaleMode(ImageScaleMode mode);

private:
    void AddRadioGroup();
    UILabel* AddLable(int16_t x, int16_t y, const char* data);
    UIRadioButton* AddRadioButton(Rect rect, const char* name, UICheckBox::OnChangeListener* listener);
    Rect GetRect(int16_t x, int16_t y, int16_t w, int16_t h) const
    {
        return Rect(x, y, x + w - 1, y + h - 1);
    }
    void SetTransMap(int16_t angle, float scale, int16_t trans, Vector2<float> pivot);

    UIScrollView* container_ = nullptr;
    GridLayout* layout_ = nullptr;
    UIImageView* imageView_ = nullptr;
    UIViewGroup* uiViewGroupFrame_ = nullptr;

    UILabelButton* rotateBtn_ = nullptr;
    UILabelButton* scaleBtn_ = nullptr;
    UILabelButton* translateBtn_ = nullptr;
    int16_t angle_ = 0;
    float scale_ = 1.0;
    int16_t trans_ = 0;
};

class StateChangeListener : public UICheckBox::OnChangeListener {
public:
    explicit StateChangeListener(ImageScaleMode mode, UITestTransform* testInstance);
    ~StateChangeListener() {}
    bool OnChange(UICheckBox::UICheckBoxState state) override;
private:
    ImageScaleMode mode_ = ImageScaleMode::AUTO;
    UITestTransform* testInstance_ = nullptr;
};

class UITestRadioButton : public UIRadioButton {
public:
    explicit UITestRadioButton(const char* name);
    ~UITestRadioButton();
    void SetListener(UICheckBox::OnChangeListener* listener);
private:
    UICheckBox::OnChangeListener* listener_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_TRANSFORM_H
