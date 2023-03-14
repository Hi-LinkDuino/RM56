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

#ifndef UI_TEST_VIEW_GROUP_H
#define UI_TEST_VIEW_GROUP_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_view_group.h"
#include "ui_test.h"

namespace OHOS {
class UITestViewGroup : public UITest, public UIView::OnClickListener {
public:
    UITestViewGroup() : container_(nullptr) {}
    ~UITestViewGroup() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

    /**
     * @brief Test Add/Rmove Function
     */
    void UIKit_ViewGroup_Test_AddRemove_001();

    /**
     * @brief Test add child multi time
     */
    void UIKit_ViewGroup_Test_Add_Error_001();

    /**
     * @brief Test insert child multi time
     */
    void UIKit_ViewGroup_Test_Insert_Error_001();

private:
    UIViewGroup* CreateTestCaseGroup() const;
    UILabel* CreateTitleLabel() const;
    UILabelButton* CreateButton(const char* text, int16_t width, int16_t height) const;
    void AddView();
    void RemoveView();
    void RemoveAndAddView();
    void AddMultiParent();
    void InsertMultiParent();

    UIScrollView* container_ = nullptr;
    UILabelButton* addBtn_ = nullptr;
    UILabelButton* removeBtn_ = nullptr;
    UILabelButton* removeAddBtn_ = nullptr;
    UILabelButton* addTwiceBtn_ = nullptr;
    UILabelButton* addMultiParentBtn_ = nullptr;
    UILabelButton* addSelfBtn_ = nullptr;
    UILabelButton* insertTwiceBtn_ = nullptr;
    UILabelButton* insertSelfBtn_ = nullptr;
    UILabelButton* insertMultiParentBtn_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_VIEW_GROUP_H
