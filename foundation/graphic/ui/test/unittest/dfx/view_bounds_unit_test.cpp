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

#include "dfx/ui_view_bounds.h"

#if ENABLE_DEBUG
#include <climits>
#include <gtest/gtest.h>

#include "common/graphic_startup.h"
#include "common/task_manager.h"
#include "components/root_view.h"
#include "components/ui_view.h"
#include "components/ui_view_group.h"
#if ENABLE_WINDOW
#include "window/window.h"
#endif

using namespace testing::ext;
namespace OHOS {
static uint16_t g_drawCount = 0;
class ViewBoundsTest : public testing::Test {
public:
    ViewBoundsTest() {}
    virtual ~ViewBoundsTest() {}

    static void SetUpTestCase()
    {
        GraphicStartUp::Init();
    }

    static void TearDownTestCase() {}

#if ENABLE_WINDOW
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
#endif // ENABLE_WINDOW
};

class UIBoundsTestView : public UIView {
public:
    UIBoundsTestView() {}
    virtual ~UIBoundsTestView() {}

    void OnPostDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override
    {
        g_drawCount++;
    }
};

class UIBoundsViewGroup : public UIViewGroup {
public:
    UIBoundsViewGroup() {}
    virtual ~UIBoundsViewGroup() {}

    void OnPostDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override
    {
        g_drawCount++;
    }
};

/**
 * @tc.name: ViewBoundsSetState001
 * @tc.desc: Verity if view bounds state equal
 * @tc.type: FUNC
 * @tc.require: AR000FQNFN
 */
HWTEST_F(ViewBoundsTest, ViewBoundsSetState001, TestSize.Level0)
{
    UIViewBounds::GetInstance()->SetShowState(true);
    bool state = UIViewBounds::GetInstance()->GetShowState();
    EXPECT_EQ(state, true);

    UIViewBounds::GetInstance()->SetShowState(false);
    state = UIViewBounds::GetInstance()->GetShowState();
    EXPECT_EQ(state, false);
}

/**
 * @tc.name: ViewBoundsSetState002
 * @tc.desc: Verity if view bounds state equal when calling multi times
 * @tc.type: FUNC
 * @tc.require: AR000FQNFN
 */
HWTEST_F(ViewBoundsTest, ViewBoundsSetState002, TestSize.Level1)
{
    uint16_t count = 10000; // 10000: call times
    for (uint16_t i = 0; i < count; i++) {
        UIViewBounds::GetInstance()->SetShowState(true);
    }
    bool state = UIViewBounds::GetInstance()->GetShowState();
    EXPECT_EQ(state, true);

    for (uint16_t i = 0; i < count; i++) {
        UIViewBounds::GetInstance()->SetShowState(false);
    }
    state = UIViewBounds::GetInstance()->GetShowState();
    EXPECT_EQ(state, false);

    for (uint16_t i = 0; i < count; i++) {
        UIViewBounds::GetInstance()->SetShowState(true);
        UIViewBounds::GetInstance()->SetShowState(false);
        UIViewBounds::GetInstance()->SetShowState(true);
    }
    state = UIViewBounds::GetInstance()->GetShowState();
    EXPECT_EQ(state, true);

    for (uint16_t i = 0; i < count; i++) {
        UIViewBounds::GetInstance()->SetShowState(false);
        UIViewBounds::GetInstance()->SetShowState(true);
        UIViewBounds::GetInstance()->SetShowState(false);
    }
    state = UIViewBounds::GetInstance()->GetShowState();
    EXPECT_EQ(state, false);
}

/**
 * @tc.name: ViewBoundsOnPostDraw001
 * @tc.desc: Test if trigger redraw when change view bounds state with sigle window
 * @tc.type: FUNC
 * @tc.require: AR000FQNFN
 */
#if ENABLE_WINDOW
HWTEST_F(ViewBoundsTest, ViewBoundsOnPostDraw001, TestSize.Level0)
{
    RootView* rootView = RootView::GetWindowRootView();
    rootView->SetWidth(600);  // 600: width
    rootView->SetHeight(500); // 500: height
    rootView->SetPosition(0, 0);
    UIBoundsTestView* view1 = new UIBoundsTestView();
    UIBoundsViewGroup* vg1 = new UIBoundsViewGroup();
    rootView->Add(vg1);
    vg1->Add(view1);
    rootView->Invalidate();
    ViewBoundsTest::CreateDefaultWindow(rootView, 0, 0);
    // clear invalidate area
    TaskManager::GetInstance()->TaskHandler();

    // init show state false
    UIViewBounds::GetInstance()->SetShowState(false);

    usleep(DEFAULT_TASK_PERIOD * 1000); // DEFAULT_TASK_PERIOD * 1000: wait next render task
    g_drawCount = 0;
    UIViewBounds::GetInstance()->SetShowState(true);
    TaskManager::GetInstance()->TaskHandler();
    EXPECT_EQ(g_drawCount, 2); // 2: redraw view count

    usleep(DEFAULT_TASK_PERIOD * 1000); // DEFAULT_TASK_PERIOD * 1000: wait next render task
    g_drawCount = 0;
    UIViewBounds::GetInstance()->SetShowState(false);
    TaskManager::GetInstance()->TaskHandler();
    EXPECT_EQ(g_drawCount, 2); // 2: redraw view count

    rootView->RemoveAll();
    delete view1;
    delete vg1;
    ViewBoundsTest::DestoryWindow(rootView);
    RootView::DestoryWindowRootView(rootView);
}

/**
 * @tc.name: ViewBoundsOnPostDraw001
 * @tc.desc: Test if trigger redraw when change view bounds state with multi window
 * @tc.type: FUNC
 * @tc.require: AR000FQNFN
 */
HWTEST_F(ViewBoundsTest, ViewBoundsOnPostDraw002, TestSize.Level0)
{
    RootView* rootView1 = RootView::GetWindowRootView();
    rootView1->SetWidth(600);  // 600: width
    rootView1->SetHeight(500); // 500: height
    rootView1->SetPosition(0, 0);
    UIBoundsTestView* view1 = new UIBoundsTestView();
    UIBoundsViewGroup* vg1 = new UIBoundsViewGroup();
    rootView1->Add(vg1);
    vg1->Add(view1);
    rootView1->Invalidate();
    ViewBoundsTest::CreateDefaultWindow(rootView1, 0, 0);

    RootView* rootView2 = RootView::GetWindowRootView();
    rootView2->SetWidth(600);  // 600: width
    rootView2->SetHeight(500); // 500: height
    rootView2->SetPosition(0, 0);
    UIBoundsTestView* view2 = new UIBoundsTestView();
    UIBoundsViewGroup* vg2 = new UIBoundsViewGroup();
    rootView2->Add(vg2);
    vg2->Add(view2);
    rootView2->Invalidate();
    ViewBoundsTest::CreateDefaultWindow(rootView2, 0, 0);
    // clear invalidate area
    TaskManager::GetInstance()->TaskHandler();

    usleep(DEFAULT_TASK_PERIOD * 1000); // DEFAULT_TASK_PERIOD * 1000: wait next render task
    g_drawCount = 0;
    UIViewBounds::GetInstance()->SetShowState(true);
    TaskManager::GetInstance()->TaskHandler();
    EXPECT_EQ(g_drawCount, 4); // 4: redraw view count

    usleep(DEFAULT_TASK_PERIOD * 1000); // DEFAULT_TASK_PERIOD * 1000: wait next render task
    g_drawCount = 0;
    UIViewBounds::GetInstance()->SetShowState(false);
    TaskManager::GetInstance()->TaskHandler();
    EXPECT_EQ(g_drawCount, 4); // 4: redraw view count

    rootView1->RemoveAll();
    delete view1;
    delete vg1;
    ViewBoundsTest::DestoryWindow(rootView1);
    RootView::DestoryWindowRootView(rootView1);
    rootView2->RemoveAll();
    delete view2;
    delete vg2;
    ViewBoundsTest::DestoryWindow(rootView2);
    RootView::DestoryWindowRootView(rootView2);
}
#endif // ENABLE_WINDOW
} // namespace OHOS
#endif // ENABLE_DEBUG
