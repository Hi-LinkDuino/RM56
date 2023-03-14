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

#ifndef UI_TEST_CIRCLE_PROGRESS_H
#define UI_TEST_CIRCLE_PROGRESS_H

#include "components/ui_circle_progress.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestCircleProgress : public UITest, public UIView::OnClickListener {
public:
    UITestCircleProgress() {}
    ~UITestCircleProgress() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void SetUpButton(UILabelButton* btn, const char* title, int16_t x, int16_t y);
    void SetUpLabel(const char* title, int16_t x, int16_t y) const;

    bool OnClick(UIView& view, const ClickEvent& event) override;

    void UIKit_CircleProgress_Test_UICircleProgress_001();
    void UIKit_CircleProgress_Test_SetValue_002();
    void UIKit_CircleProgress_Test_SetImage_003();
    void UIKit_CircleProgress_Test_SetStyle_004();
    void UIKit_CircleProgress_Test_GetStyle_005();
    void UIKit_CircleProgress_Test_SetCapType_006();
    void UIKit_CircleProgress_Test_SetStep_007();
    void UIKit_CircleProgress_Test_SetRange_008();
    void UIKit_CircleProgress_Test_EnableBackground_009();
    void UIKit_CircleProgress_Test_SetCenter_010();
    void UIKit_CircleProgress_Test_SetRadius_011();
    void UIKit_CircleProgress_Test_SetLineWidth_012();
    void UIKit_CircleProgress_Test_SetImagePosition_013();
    void UIKit_CircleProgress_Test_SetLineColor_014();
    void UIKit_CircleProgress_Test_SetAngle_015();

private:
    UIScrollView* container_ = nullptr;
    UIScrollView* scroll_ = nullptr;
    UICircleProgress* circleProgress_ = nullptr;
    UIViewGroup* uiViewGroupFrame_ = nullptr;

    UILabelButton* resetBtn_ = nullptr;
    UILabelButton* incProgressBtn_ = nullptr;
    UILabelButton* decProgressBtn_ = nullptr;
    UILabelButton* incMinProgressBtn_ = nullptr;
    UILabelButton* decMinProgressBtn_ = nullptr;
    UILabelButton* incMaxProgressBtn_ = nullptr;
    UILabelButton* decMaxProgressBtn_ = nullptr;
    UILabelButton* centerXBtn_ = nullptr;
    UILabelButton* centerYBtn_ = nullptr;
    UILabelButton* incRadiusBtn_ = nullptr;
    UILabelButton* decRadiusBtn_ = nullptr;
    UILabelButton* incWidthBtn_ = nullptr;
    UILabelButton* decWidthBtn_ = nullptr;
    UILabelButton* incStartAngleBtn_ = nullptr;
    UILabelButton* decStartAngleBtn_ = nullptr;
    UILabelButton* incEndAngleBtn_ = nullptr;
    UILabelButton* decEndAngleBtn_ = nullptr;
    UILabelButton* swapAngleBtn_ = nullptr;
    UILabelButton* stepBtn_ = nullptr;
    UILabelButton* imageBtn_ = nullptr;
    UILabelButton* noImageBtn_ = nullptr;
    UILabelButton* setStyleBtn_ = nullptr;
    UILabelButton* getStyleBtn_ = nullptr;
    UILabelButton* roundCapBtn_ = nullptr;
    UILabelButton* noneCapBtn_ = nullptr;
    UILabelButton* enableBgBtn_ = nullptr;
    UILabelButton* disableBgBtn_ = nullptr;
    UILabelButton* imgPosXBtn_ = nullptr;
    UILabelButton* imgPosYBtn_ = nullptr;
    UILabelButton* lineColorBtn_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_CIRCLE_PROGRESS_H
