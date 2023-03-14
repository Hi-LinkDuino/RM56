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

#include <climits>
#include <gtest/gtest.h>

#include "common/graphic_startup.h"
#include "common/task_manager.h"
#include "components/root_view.h"
#include "components/ui_view.h"
#include "components/ui_view_group.h"
#include "window/window.h"

using namespace testing::ext;
namespace OHOS {
static uint16_t g_measureCount = 0;
class RenderTest : public testing::Test {
public:
    RenderTest() {}
    virtual ~RenderTest() {}

    static void SetUpTestCase()
    {
        GraphicStartUp::Init();
    }
    static void TearDownTestCase() {}

    static void CreateDefaultWindow(RootView* rootView, int x, int y)
    {
        WindowConfig config = {};
        config.rect = rootView->GetRect();
        config.rect.SetPosition(x, y);
        Window* window = Window::CreateWindow(config);
        if (window != nullptr) {
            window->BindRootView(rootView);
        }
    }

    static void DestoryWindow(RootView* rootView)
    {
        Window::DestoryWindow(rootView->GetBoundWindow());
    }
};

class UITestView : public UIView {
public:
    UITestView() {}
    virtual ~UITestView() {}

    void ReMeasure() override
    {
        g_measureCount++;
    }
};

class UITestViewGroup : public UIViewGroup {
public:
    UITestViewGroup() {}
    virtual ~UITestViewGroup() {}

    void ReMeasure() override
    {
        g_measureCount++;
    }
};

/**
 * @tc.name: Graphic_RenderTest_Test_Measuer_001
 * @tc.desc: Verity measure call when render
 * @tc.type: FUNC
 * @tc.require: SR000FH555
 */
HWTEST_F(RenderTest, Graphic_RenderTest_Test_Measuer_001, TestSize.Level0)
{
    RootView* rootView = RootView::GetWindowRootView();
    rootView->SetWidth(600);  // 600: width
    rootView->SetHeight(500); // 500: height
    rootView->SetPosition(0, 0);
    UITestView* view1 = new UITestView();
    UITestViewGroup* vg1 = new UITestViewGroup();
    rootView->Add(vg1);
    vg1->Add(view1);
    vg1->Invalidate();
    rootView->Invalidate();
    g_measureCount = 0;

    RenderTest::CreateDefaultWindow(rootView, 0, 0);
    usleep(DEFAULT_TASK_PERIOD * 1000); // DEFAULT_TASK_PERIOD * 1000: wait next render task
    TaskManager::GetInstance()->TaskHandler();
    EXPECT_EQ(g_measureCount, 2); // 2: measure view
    rootView->RemoveAll();
    delete view1;
    delete vg1;
    RenderTest::DestoryWindow(rootView);
    RootView::DestoryWindowRootView(rootView);
}

/**
 * @tc.name: Graphic_RenderTest_Test_Measuer_02
 * @tc.desc: Verity measure call when view invisible
 * @tc.type: FUNC
 * @tc.require: AR000FH556
 */
HWTEST_F(RenderTest, Graphic_RenderTest_Test_Measuer_02, TestSize.Level0)
{
    RootView* rootView = RootView::GetWindowRootView();
    rootView->SetWidth(600);  // 600: width
    rootView->SetHeight(500); // 500: height
    rootView->SetPosition(0, 0);
    UITestView* view1 = new UITestView();
    UITestViewGroup* vg1 = new UITestViewGroup();
    rootView->Add(vg1);
    vg1->Add(view1);
    // invisible view not need to measure
    view1->SetVisible(false);
    rootView->Invalidate();
    g_measureCount = 0;

    RenderTest::CreateDefaultWindow(rootView, 0, 0);
    usleep(DEFAULT_TASK_PERIOD * 1000); // DEFAULT_TASK_PERIOD * 1000: wait next render task
    TaskManager::GetInstance()->TaskHandler();
    EXPECT_EQ(g_measureCount, 1);

    rootView->RemoveAll();
    delete view1;
    delete vg1;
    RenderTest::DestoryWindow(rootView);
    RootView::DestoryWindowRootView(rootView);
}
} // namespace OHOS
