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

#ifndef UI_TEST_ANIMATOR_H
#define UI_TEST_ANIMATOR_H

#include "animator/easing_equation.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "gfx_utils/sys_info.h"
#include "ui_test.h"

namespace OHOS {
class ImageEaseAnimatorCallback : public AnimatorCallback {
public:
    ImageEaseAnimatorCallback(UIView* uiView, int16_t startPos, int16_t endPos)
        : startPos_(startPos),
          endPos_(endPos),
          easingFunc_(EasingEquation::LinearEaseNone),
          animator_(new Animator(this, uiView, 1000, true)) // 1000:duration of animator_, in milliseconds.
    {
    }

    virtual ~ImageEaseAnimatorCallback()
    {
        delete animator_;
    }

    Animator* GetAnimator() const
    {
        return animator_;
    }

    void SetEasingFunc(EasingFunc easingFunc)
    {
        easingFunc_ = easingFunc;
    }

    virtual void Callback(UIView* view)
    {
        if (view == nullptr) {
            return;
        }
        int16_t pos = easingFunc_(startPos_, endPos_, animator_->GetRunTime(), animator_->GetTime());
        Rect invalidatedArea = view->GetRect();
        view->SetPosition(pos, view->GetY());
        invalidatedArea.Join(invalidatedArea, view->GetRect());
        view->InvalidateRect(invalidatedArea);
    }

protected:
    int16_t startPos_;
    int16_t endPos_;
    EasingFunc easingFunc_;
    Animator* animator_;
};

class UITestAnimator : public UITest, public UIView::OnClickListener, public SysInfo::OnFPSChangedListener {
public:
    UITestAnimator() {}
    ~UITestAnimator(){}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void SetUpButton(UILabelButton* btn, const char* title, int16_t x, int16_t y);
    void SetUpLabel(const char* title, int16_t x, int16_t y);

    bool OnClick(UIView& view, const ClickEvent& event) override;
    void OnFPSChanged(float newFPS) override;

    void UIKit_Animator_Test_BackEasing_001();
    void UIKit_Animator_Test_CircEasing_002();
    void UIKit_Animator_Test_CubicEasing_003();
    void UIKit_Animator_Test_SineEasing_004();
    void UIKit_Animator_Test_QuadEasing_005();
    void UIKit_Animator_Test_QuintEasing_006();
    void UIKit_Animator_Test_LinearEasing_007();
    void UIKit_Animator_Test_FPS_008();

private:
    UIScrollView* container_ = nullptr;
    UIScrollView* scroll_ = nullptr;
    UIImageView* image_ = nullptr;
    ImageEaseAnimatorCallback* callback_ = nullptr;
    Animator* animator_ = nullptr;
    UIViewGroup* uiViewGroupFrame_ = nullptr;

    UILabelButton* backOvershootBtn_ = nullptr;
    UILabelButton* backEaseInBtn_ = nullptr;
    UILabelButton* backEaseOutBtn_ = nullptr;
    UILabelButton* backEaseInOutBtn_ = nullptr;
    UILabelButton* circEaseInBtn_ = nullptr;
    UILabelButton* circEaseOutBtn_ = nullptr;
    UILabelButton* circEaseInOutBtn_ = nullptr;
    UILabelButton* cubicEaseInBtn_ = nullptr;
    UILabelButton* cubicEaseOutBtn_ = nullptr;
    UILabelButton* cubicEaseInOutBtn_ = nullptr;
    UILabelButton* linearEaseNoneBtn_ = nullptr;
    UILabelButton* quadEaseInBtn_ = nullptr;
    UILabelButton* quadEaseOutBtn_ = nullptr;
    UILabelButton* quadEaseInOutBtn_ = nullptr;
    UILabelButton* quintEaseInBtn_ = nullptr;
    UILabelButton* quintEaseOutBtn_ = nullptr;
    UILabelButton* quintEaseInOutBtn_ = nullptr;
    UILabelButton* sineEaseInBtn_ = nullptr;
    UILabelButton* sineEaseOutBtn_ = nullptr;
    UILabelButton* sineEaseInOutBtn_ = nullptr;
    UILabel* fpsLabel_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_ANIMATOR_H
