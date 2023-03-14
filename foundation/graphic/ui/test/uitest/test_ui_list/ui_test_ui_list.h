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

#ifndef UI_TEST_UI_LIST_H
#define UI_TEST_UI_LIST_H

#include "components/text_adapter.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_list.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestUIList : public UITest, UIView::OnClickListener, ListScrollListener {
public:
    UITestUIList() {}
    ~UITestUIList() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    bool OnClick(UIView& view, const ClickEvent& event) override;
    void OnScrollStart(int16_t index, UIView* view) override;
    void OnScrollEnd(int16_t index, UIView* view) override;
    void OnScrollTop(int16_t index, UIView* view) override;
    void OnScrollBottom(int16_t index, UIView* view) override;
    void OnItemSelected(int16_t index, UIView* view) override;
    void UIKit_List_Init_Test_Full_Screen_001();
    void UIKit_List_Init_Test_halh_Screen_001();
    void UIKit_List_Scroll_Test_Blank_Set_001();

private:
    static constexpr int16_t ALINE_TIME_CHANGE_VALUE = 100;
    void SetLastPos(UIView* view);
    void SetUpButton(UILabelButton* btn, const char* title);
    void SetControlButton();
    UILabelButton* setBlankBtn_ = nullptr;
    UILabelButton* setBlankOffBtn_ = nullptr;
    UILabelButton* setThrowDragBtn_ = nullptr;
    UILabelButton* setThrowDragOffBtn_ = nullptr;
    UILabelButton* setRefreshBtn_ = nullptr;
    UILabelButton* setLoopBtn_ = nullptr;
    UILabelButton* setLoopOffBtn_ = nullptr;
    UILabelButton* setListPaddingBtn_ = nullptr;
    UILabelButton* setListBorderBtn_ = nullptr;
    UILabelButton* setRebondSizeBtn_ = nullptr;
    UILabelButton* setSelectBtn_ = nullptr;
    UILabelButton* setSelectOffBtn_ = nullptr;
    UILabelButton* setAutoAlignBtn_ = nullptr;
    UILabelButton* setAutoAlignOffBtn_ = nullptr;
    UILabelButton* setBackAlignBtn_ = nullptr;
    UILabelButton* setBackAlignOffBtn_ = nullptr;
    UILabelButton* setAutoAlignACCIncBtn_ = nullptr;
    UILabelButton* setAutoAlignACCDncBtn_ = nullptr;
    UILabelButton* setYScrollBarVisableBtn_ = nullptr;
    UILabelButton* setYScrollBarInvisableBtn_ = nullptr;
    UILabelButton* setItemHeightMinBtn_ = nullptr;
    UILabelButton* setItemHeightMaxBtn_ = nullptr;
    UILabelButton* setAdapterEmptyBtn_ = nullptr;
    UILabelButton* setSwitchDirectionBtn_ = nullptr;
    UILabel* scrollStateLabel_ = nullptr;
    UILabel* scrollSelectLabel_ = nullptr;
    UIScrollView* container_ = nullptr;
    List<const char*>* adapterData1_ = nullptr;
    List<const char*>* adapterData2_ = nullptr;
    TextAdapter* adapter1_ = nullptr;
    TextAdapter* adapter2_ = nullptr;
    TextAdapter* adapter3_ = nullptr;
    TextAdapter* adapter4_ = nullptr;
    UIList* currentList_ = nullptr;
    UIList* currentList2_ = nullptr;
    UILabel* lastSelectLabel_ = nullptr;
    int16_t lastX_ = 0;
    int16_t lastY_ = 0;
    uint16_t autoAlignTime_ = 100; // 100: default list auto aling ACC
};
} // namespace OHOS
#endif // UI_TEST_UI_LIST_H
