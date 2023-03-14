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

#ifndef UI_TEST_SLIDER_H
#define UI_TEST_SLIDER_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_slider.h"
#include "ui_test.h"

namespace OHOS {
class TestUISliderEventListener : public UISlider::UISliderEventListener,
                                  public UIView::OnClickListener,
                                  public UIView::OnDragListener {
public:
    explicit TestUISliderEventListener(UIViewGroup* viewGroup)
    {
        viewGroup_ = viewGroup;
    }
    virtual ~TestUISliderEventListener() {}
    void OnChange(int32_t progress) override;
    void OnRelease(int32_t progress) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool MallocLabel();

private:
    UILabel* label_ = nullptr;
    UIViewGroup* viewGroup_ = nullptr;
    int32_t value_ = 0;
};

class UITestSlider : public UITest, public UIView::OnClickListener {
public:
    UITestSlider() {}
    ~UITestSlider() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void SetUpButton(UILabelButton* btn, const char* title, int16_t x, int16_t y);
    void SetUpLabel(const char* title, int16_t x, int16_t y) const;

    bool OnClick(UIView& view, const ClickEvent& event) override;
    bool ExpandClick1(UIView& view, const ClickEvent& event);
    bool ExpandClick2(UIView& view, const ClickEvent& event);

    void UIKit_Slider_Test_UISlider_001();
    void UIKit_Slider_Test_SetRange_002();
    void UIKit_Slider_Test_SetValue_003();
    void UIKit_Slider_Test_SetImage_004();
    void UIKit_Slider_Test_SetStyle_005();
    void UIKit_Slider_Test_GetStyle_006();
    void UIKit_Slider_Test_SetKnobWidth_007();
    void UIKit_Slider_Test_SetStep_008();
    void UIKit_Slider_Test_SetColor_009();
    void UIKit_Slider_Test_SetDirection_010();
    void UIKit_Slider_Test_SetValidSize_011();
    void UIKit_Slider_Test_SetRadius_012();
    void UIKit_Slider_Test_SetOnChangeListener_013();
    void UIKit_Slider_Test_SetOnReleaseListener_014();
    void UIKit_Slider_Test_SetOnClickCallback_015();
    void UIKit_Slider_Test_SetOnDragCallback_016();
    void UIKit_Slider_Test_SetCircularStyle_017();

private:
    static constexpr int32_t DEFAULT_VALUE = 20;
    static constexpr int16_t DEFAULT_WIDTH = 50;
    static constexpr int16_t DEFAULT_HEIGHT = 250;
    static constexpr int16_t DEFAULT_RADIUS = 10;
    static constexpr int32_t MAX_VALUE = 100;
    UIScrollView* container_ = nullptr;
    UIScrollView* scroll_ = nullptr;
    UISlider* slider_ = nullptr;
    TestUISliderEventListener* listener_ = nullptr;
    UIViewGroup* uiViewGroupFrame_ = nullptr;

    UILabelButton* resetBtn_ = nullptr;
    UILabelButton* widthBtn_ = nullptr;
    UILabelButton* heightBtn_ = nullptr;
    UILabelButton* incProgressBtn_ = nullptr;
    UILabelButton* decProgressBtn_ = nullptr;
    UILabelButton* incMinProgressBtn_ = nullptr;
    UILabelButton* decMinProgressBtn_ = nullptr;
    UILabelButton* incMaxProgressBtn_ = nullptr;
    UILabelButton* decMaxProgressBtn_ = nullptr;
    UILabelButton* stepBtn_ = nullptr;
    UILabelButton* dirL2RBtn_ = nullptr;
    UILabelButton* dirR2LBtn_ = nullptr;
    UILabelButton* dirT2BBtn_ = nullptr;
    UILabelButton* dirB2TBtn_ = nullptr;
    UILabelButton* imageBtn_ = nullptr;
    UILabelButton* noImageBtn_ = nullptr;
    UILabelButton* setStyleBtn_ = nullptr;
    UILabelButton* getStyleBtn_ = nullptr;
    UILabelButton* incKnobWidthBtn_ = nullptr;
    UILabelButton* decKnobWidthBtn_ = nullptr;
    UILabelButton* colorBtn_ = nullptr;
    UILabelButton* radiusBtn_ = nullptr;
    UILabelButton* onChangeBtn_ = nullptr;
    UILabelButton* onReleaseBtn_ = nullptr;
    UILabelButton* onClickBtn_ = nullptr;
    UILabelButton* onDragBtn_ = nullptr;
    UILabelButton* circularStyleBtn_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_SLIDER_H
