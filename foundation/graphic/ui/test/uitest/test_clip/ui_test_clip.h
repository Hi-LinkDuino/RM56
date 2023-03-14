/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#ifndef UI_TEST_CLIP_H
#define UI_TEST_CLIP_H

#include "components/ui_image_view.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "draw/clip_utils.h"
#include "ui_test.h"

namespace OHOS {
class UITestClip : public UITest, public UIView::OnClickListener {
public:
    UITestClip() {}
    ~UITestClip() {}
    void SetUp() override;
    void TearDown() override;
    UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

    void UIKitClipTest001();
    void UIKitClipTest002();
    void UIKitClipTest003();
    void UIKitClipTest004();
    void UIKitClipTest005();

private:
    const static int32_t BUFFER_SIZE = 20;
    const static int16_t GAP = 10;
    const static int16_t TITLE_HEIGHT = 29;
    const static uint16_t BLOCK_WIDTH = 454;
    const static uint16_t BLOCK_HEIGHT = 200;
    const static uint16_t IMAGE_POSITION_X = 20;
    const static uint16_t IMAGE_POSITION_Y = 20;
    const static int16_t ANGLE_STEP = 5;
    const static int16_t RADIUS_STEP_1 = 1;
    const static int16_t RADIUS_STEP_5 = 5;

    void CreateTitleLabel(const char* title);
    UIImageView* CreateImageView();
    void ClipImage(UIImageView* imageView, ClipPath& path);
    void SetUpButton(UILabelButton* btn, const char* title, int16_t x, int16_t y);

    UILabelButton* btnRadiusInc1_ = nullptr;
    UILabelButton* btnRadiusInc5_ = nullptr;
    UILabelButton* btnRadiusDec1_ = nullptr;
    UILabelButton* btnRadiusDec5_ = nullptr;
    UILabelButton* btnStartAngleInc_ = nullptr;
    UILabelButton* btnStartAngleDec_ = nullptr;
    UILabelButton* btnEndAngleInc_ = nullptr;
    UILabelButton* btnEndAngleDec_ = nullptr;
    UILabel* radiusText_ = nullptr;
    UIImageView* imageView1_ = nullptr;
    UIImageView* imageView2_ = nullptr;
    UIScrollView* container_ = nullptr;
};
} // namespace OHOS
#endif
