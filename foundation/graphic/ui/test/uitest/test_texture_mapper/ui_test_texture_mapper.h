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

#ifndef UI_TEST_TEXTURE_MAPPER_H
#define UI_TEST_TEXTURE_MAPPER_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_texture_mapper.h"
#include "layout/grid_layout.h"
#include "ui_test.h"

namespace OHOS {
class TestTextureMapperStopListener : public UITextureMapper::AnimatorStopListener {
public:
    explicit TestTextureMapperStopListener(UIViewGroup* viewGroup)
    {
        if (viewGroup != nullptr) {
            viewGroup_ = viewGroup;
            viewGroup_->Add(&label_);
            label_.SetPosition(0, 0, 200, TITLE_LABEL_DEFAULT_HEIGHT); // 200: width;
            label_.SetText("OnAnimatorStop");
            label_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
            label_.SetStyle(STYLE_TEXT_COLOR, Color::Black().full);
            label_.SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
            label_.SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
            label_.SetVisible(false);
        }
    }

    ~TestTextureMapperStopListener()
    {
        if (viewGroup_ != nullptr) {
            viewGroup_->Remove(&label_);
        }
    };

    void OnAnimatorStop(UIView& view) override
    {
        label_.SetVisible(true);
        label_.Invalidate();
    }

private:
    UILabel label_;
    UIViewGroup* viewGroup_ = nullptr;
};

class UITestTextureMapper : public UITest, public UIView::OnClickListener {
public:
    UITestTextureMapper() {}
    ~UITestTextureMapper() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void SetUpButton(UILabelButton* btn, const char* title);

    bool OnClick(UIView& view, const ClickEvent& event) override;
    bool ExpandClick(UIView& view, const ClickEvent& event);

    void UIKit_TextureMapper_Test_SetRotateEnd_001();
    void UIKit_TextureMapper_Test_SetRotateStart_002();
    void UIKit_TextureMapper_Test_SetScaleEnd_003();
    void UIKit_TextureMapper_Test_SetScaleStart_004();
    void UIKit_TextureMapper_Test_SetPivot_005();
    void UIKit_TextureMapper_Test_Start_006();
    void UIKit_TextureMapper_Test_Reset_007();
    void UIKit_TextureMapper_Test_CancelAnimation_008();
    void UIKit_TextureMapper_Test_SetAnimatorStopListener_009();

private:
    UIScrollView* container_ = nullptr;
    GridLayout* layout_ = nullptr;
    UITextureMapper* textureMapper_ = nullptr;
    TestTextureMapperStopListener* listener_ = nullptr;

    UILabelButton* resetBtn_ = nullptr;

    UILabelButton* incRotateEndBtn_ = nullptr;
    UILabelButton* decRotateEndBtn_ = nullptr;
    UILabelButton* startAngleBtn_ = nullptr;

    UILabelButton* incScaleEndBtn_ = nullptr;
    UILabelButton* decScaleEndBtn_ = nullptr;
    UILabelButton* startScaleBtn_ = nullptr;

    UILabelButton* durationTimeBtn_ = nullptr;
    UILabelButton* delayTimeBtn_ = nullptr;
    UILabelButton* easingBtn_ = nullptr;

    UILabelButton* incPivotBtn_ = nullptr;
    UILabelButton* decPivotBtn_ = nullptr;
    UILabelButton* startBtn_ = nullptr;
    UILabelButton* cancelBtn_ = nullptr;
    UILabelButton* resetImageBtn_ = nullptr;
    UILabelButton* listenerBtn_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_TEXTURE_MAPPER_H
