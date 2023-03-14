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

#ifndef UI_TEST_RENDER_H
#define UI_TEST_RENDER_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
namespace {
    constexpr char* UI_TEST_RENDER_ID = "Render";
    constexpr char* UI_TEST_RENDER_UPDATA_BUTTON_ID_01 = "test_render_updata_buttin_01";
}
class UITestRender : public UITest, public UIView::OnClickListener {
public:
    UITestRender() : container_(nullptr) {}
    ~UITestRender() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

    /**
     * @brief Test render Function
     */
    void UIKit_Render_Test_Render_001();

    /**
     * @brief Test render measure Function
     */
    void UIKit_Render_Test_RenderMeasure_001();

private:
    UIViewGroup* CreateTestCaseGroup() const;
    UILabel* CreateTitleLabel() const;
    UIScrollView* container_ = nullptr;
    UILabel* testLabel_ = nullptr;
    UILabelButton* labelButton_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_RENDER_H
