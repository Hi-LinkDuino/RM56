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

#ifndef UI_TEST_BOX_PROGRESS_H
#define UI_TEST_BOX_PROGRESS_H

#include "components/ui_box_progress.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestBoxProgress : public UITest, public UIView::OnClickListener {
public:
    UITestBoxProgress() {}
    ~UITestBoxProgress() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void SetUpButton(UILabelButton* btn, const char* title, int16_t x, int16_t y);
    void SetUpLabel(const char* title, int16_t x, int16_t y) const;

    bool OnClick(UIView& view, const ClickEvent& event) override;

    void UIKit_BoxProgress_Test_UIBoxProgress_001();
    void UIKit_BoxProgress_Test_SetRange_002();
    void UIKit_BoxProgress_Test_SetValue_003();
    void UIKit_BoxProgress_Test_SetImage_004();
    void UIKit_BoxProgress_Test_SetStyle_005();
    void UIKit_BoxProgress_Test_GetStyle_006();
    void UIKit_BoxProgress_Test_SetCapType_007();
    void UIKit_BoxProgress_Test_SetDirection_008();
    void UIKit_BoxProgress_Test_EnableBackground_009();
    void UIKit_BoxProgress_Test_SetStep_010();
    void UIKit_BoxProgress_Test_SetValidSize_011();
    void UIKit_BoxProgress_Test_SetBorderAndPadding_012();

private:
    UIScrollView* container_ = nullptr;
    UIScrollView* scroll_ = nullptr;
    UIBoxProgress* boxProgress_ = nullptr;
    UIViewGroup* uiViewGroupFrame_ = nullptr;

    UILabelButton* resetBtn_ = nullptr;
    UILabelButton* incWidthBtn_ = nullptr;
    UILabelButton* incHeightBtn_ = nullptr;
    UILabelButton* decWidthBtn_ = nullptr;
    UILabelButton* decHeightBtn_ = nullptr;
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
    UILabelButton* roundCapBtn_ = nullptr;
    UILabelButton* noneCapBtn_ = nullptr;
    UILabelButton* enableBgBtn_ = nullptr;
    UILabelButton* disableBgBtn_ = nullptr;
    UILabelButton* borderIncBtn_ = nullptr;
    UILabelButton* borderDecBtn_ = nullptr;
    UILabelButton* paddingLeftIncBtn_ = nullptr;
    UILabelButton* paddingRightIncBtn_ = nullptr;
    UILabelButton* paddingTopIncBtn_ = nullptr;
    UILabelButton* paddingBottomIncBtn_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_BOX_PROGRESS_H
