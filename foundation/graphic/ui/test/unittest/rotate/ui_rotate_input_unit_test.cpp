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
#include "components/ui_abstract_progress.h"
#include "components/ui_list.h"
#include "components/ui_picker.h"
#include "components/ui_scroll_view.h"
#include "components/ui_slider.h"
#include "components/ui_swipe_view.h"
#include "components/ui_time_picker.h"
#include "dock/focus_manager.h"
#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
class UIRotateInputTest : public testing::Test {
public:
    UIRotateInputTest() {}
    virtual ~UIRotateInputTest() {}

    static void SetUpTestCase(){}
    static void TearDownTestCase(){}
};

/**
 * @tc.name: Graphic_RotateInput_Test_SetRotateFactor_001
 * @tc.desc: Verify UIRotateInput::SetRotateFactor function.
 * @tc.type: FUNC
 * @tc.require: SR000ERCQF
 */
HWTEST_F(UIRotateInputTest, Graphic_RotateInput_Test_SetRotateFactor_001, TestSize.Level1)
{
    UIScrollView* scroll = new UIScrollView();
    scroll->SetRotateFactor(10); // 10 : rotate factor
    EXPECT_EQ(scroll->GetRotateFactor(), 10);
    delete scroll;
}

/**
 * @tc.name: Graphic_RotateInput_Test_SetRotateFactor_002
 * @tc.desc: Verify UIRotateInput::SetRotateFactor function.
 * @tc.type: FUNC
 * @tc.require: SR000ERCQF
 */
HWTEST_F(UIRotateInputTest, Graphic_RotateInput_Test_SetRotateFactor_002, TestSize.Level1)
{
    UISwipeView* swipe = new UISwipeView();
    swipe->SetRotateFactor(10); // 10 : rotate factor
    EXPECT_EQ(swipe->GetRotateFactor(), 10);
    delete swipe;
}

/**
 * @tc.name: Graphic_RotateInput_Test_SetRotateFactor_003
 * @tc.desc: Verify UIRotateInput::SetRotateFactor function.
 * @tc.type: FUNC
 * @tc.require: SR000ERCQF
 */
HWTEST_F(UIRotateInputTest, Graphic_RotateInput_Test_SetRotateFactor_003, TestSize.Level1)
{
    UIList* list = new UIList();
    list->SetRotateFactor(10); // 10 : rotate factor
    EXPECT_EQ(list->GetRotateFactor(), 10);
    delete list;
}

/**
 * @tc.name: Graphic_RotateInput_Test_SetRotateFactor_004
 * @tc.desc: Verify UIRotateInput::SetRotateFactor function.
 * @tc.type: FUNC
 * @tc.require: SR000ERCQF
 */
HWTEST_F(UIRotateInputTest, Graphic_RotateInput_Test_SetRotateFactor_004, TestSize.Level1)
{
    UIList* list = new UIList();
    list->SetRotateFactor(20); // 10 : rotate factor
    EXPECT_NE(list->GetRotateFactor(), 10);
    delete list;
}

/**
 * @tc.name: Graphic_RotateInput_Test_SetRotateFactor_005
 * @tc.desc: Verify UIRotateInput::SetRotateFactor function.
 * @tc.type: FUNC
 * @tc.require: AR000EVI2R
 */
HWTEST_F(UIRotateInputTest, Graphic_RotateInput_Test_SetRotateFactor_005, TestSize.Level1)
{
    UISlider* slider = new UISlider();
    slider->SetRotateFactor(10); // 10 : rotate factor
    EXPECT_EQ(slider->GetRotateFactor(), 10);
    delete slider;
}

/**
 * @tc.name: Graphic_RotateInput_Test_RequestFocus_001
 * @tc.desc: Verify UIRotateInput::RequestFocus function.
 * @tc.type: FUNC
 * @tc.require: SR000FH553
 */
HWTEST_F(UIRotateInputTest, Graphic_RotateInput_Test_RequestFocus_001, TestSize.Level0)
{
    UIScrollView* scroll = new UIScrollView();
    scroll->RequestFocus();
    EXPECT_EQ(scroll, FocusManager::GetInstance()->GetFocusedView());
    scroll->ClearFocus();
    delete scroll;
}
/**
 * @tc.name: Graphic_RotateInput_Test_RequestFocus_002
 * @tc.desc: Verify UIRotateInput::RequestFocus function.
 * @tc.type: FUNC
 * @tc.require: AR000FH554
 */
HWTEST_F(UIRotateInputTest, Graphic_RotateInput_Test_RequestFocus_002, TestSize.Level1)
{
    UITimePicker* timePicker = new UITimePicker();
    timePicker->RequestFocus();
    EXPECT_EQ(timePicker, FocusManager::GetInstance()->GetFocusedView());
    timePicker->ClearFocus();
    delete timePicker;
}
/**
 * @tc.name: Graphic_RotateInput_Test_RequestFocus_003
 * @tc.desc: Verify UIRotateInput::RequestFocus function.
 * @tc.type: FUNC
 * @tc.require: AR000FH554
 */
HWTEST_F(UIRotateInputTest, Graphic_RotateInput_Test_RequestFocus_003, TestSize.Level1)
{
    UISwipeView* swipe = new UISwipeView();
    swipe->RequestFocus();
    EXPECT_EQ(swipe, FocusManager::GetInstance()->GetFocusedView());
    swipe->ClearFocus();
    delete swipe;
}
/**
 * @tc.name: Graphic_RotateInput_Test_RequestFocus_004
 * @tc.desc: Verify UIList::RequestFocus function.
 * @tc.type: FUNC
 * @tc.require: SR000ERCQG
 */
HWTEST_F(UIRotateInputTest, Graphic_RotateInput_Test_RequestFocus_004, TestSize.Level1)
{
    UIList* list = new UIList();
    list->RequestFocus();
    EXPECT_EQ(list, FocusManager::GetInstance()->GetFocusedView());
    list->ClearFocus();
    delete list;
}
/**
 * @tc.name: Graphic_RotateInput_Test_RequestFocus_005
 * @tc.desc: Verify UIRotateInput::RequestFocus function.
 * @tc.type: FUNC
 * @tc.require: AR000EVI2S
 */
HWTEST_F(UIRotateInputTest, Graphic_RotateInput_Test_RequestFocus_005, TestSize.Level1)
{
    UISlider* slider = new UISlider();
    slider->RequestFocus();
    EXPECT_EQ(slider, FocusManager::GetInstance()->GetFocusedView());
    slider->ClearFocus();
    delete slider;
}
}

