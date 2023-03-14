/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "base/log/log.h"
#include "core/components/flex/flex_component.h"
#include "core/components/tab_bar/render_tab_bar.h"
#include "core/components/tab_bar/render_tab_bar_item.h"
#include "core/components/tab_bar/render_tab_content.h"
#include "core/components/test/unittest/mock/mock_render_depend.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

using TabContentEventCallback = std::function<void(const std::string&, const std::string&)>;
using UpdateTabContentCallback = std::function<void(const RefPtr<TabContentComponent>&)>;
using UpdateTabBarCallback = std::function<void(const RefPtr<TabBarComponent>&)>;

constexpr int32_t WAIT_TIME = 212;
constexpr int32_t CONTENT_WIDTH = 1080;
constexpr int64_t SEC_TO_NANOSEC = 1000000000;
constexpr int64_t MILLISEC_TO_MICROSEC = 1000;
constexpr int64_t VSYNC_INTERVAL_MILLISEC = 16;
constexpr int64_t VSYNC_INTERVAL_MICROSEC = VSYNC_INTERVAL_MILLISEC * MILLISEC_TO_MICROSEC;

static int64_t GetTickCount()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * SEC_TO_NANOSEC + ts.tv_nsec);
}

const TouchEvent MOCK_DOWN_TOUCH_EVENT { 10, 540.0f, 500.0f, TouchType::DOWN };
const TouchEvent MOCK_MOVE_TOUCH_EVENT { 10, 540.0f, 500.0f, TouchType::MOVE };
const TouchEvent MOCK_UP_TOUCH_EVENT { 10, 540.0f, 500.0f, TouchType::UP };

std::function<void(const RefPtr<RenderTabContent>&)> hookContent;
std::function<void(const RefPtr<RenderTabBar>&)> hookBar;

enum class DragDirection {
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

class MockRenderTabBar : public RenderTabBar {};
class MockRenderTabContent : public RenderTabContent {};
class MockRenderTabBarItem : public RenderTabBarItem {};

class TestTabContentEventHandler : public MockEventHandler {
public:
    explicit TestTabContentEventHandler(TabContentEventCallback eventCallback) : eventCallback_(eventCallback) {}
    ~TestTabContentEventHandler() = default;

    void HandleAsyncEvent(const EventMarker& eventMarker, const std::string& param)
    {
        if (eventCallback_) {
            eventCallback_(eventMarker.GetData().eventId, param);
        }
    }

private:
    TabContentEventCallback eventCallback_;
};

} // namespace

class RenderTabContentTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    int32_t index_ = 0;
    int32_t totalEventsNum_ = 0;

    RefPtr<RenderTabContent> renderTabContent_;
    RefPtr<PipelineContext> context_;
    RefPtr<Frontend> frontend_;

protected:
    void CreateAndRenderBarAndContent(UpdateTabBarCallback updateBar);
    void CreateAndRenderContent(UpdateTabContentCallback callback);
    void CreateBarAndContent(RefPtr<TabBarComponent>& tabBarComponent, UpdateTabContentCallback callback);
    void DragTabContent(const DragDirection& dragDirection);
    void WaitAndMockVsync(int64_t waitFor);
    bool TestContentPosition(int32_t index, double position);
};

RefPtr<RenderNode> RenderTabBar::Create()
{
    const auto& tabBar = AceType::MakeRefPtr<MockRenderTabBar>();
    if (hookBar) {
        hookBar(tabBar);
    }
    return tabBar;
}

RefPtr<RenderNode> RenderTabBarItem::Create()
{
    const auto& tabBarItem = AceType::MakeRefPtr<MockRenderTabBarItem>();
    return tabBarItem;
}

RefPtr<RenderNode> RenderTabContent::Create()
{
    const auto& tabContent = AceType::MakeRefPtr<MockRenderTabContent>();
    if (hookContent) {
        hookContent(tabContent);
    }
    return tabContent;
}

void RenderTabContentTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "TabContentTest SetUpTestCase";
}

void RenderTabContentTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TabContentTest TearDownTestCase";
}

void RenderTabContentTest::SetUp()
{
    auto platformWindow = PlatformWindow::Create(nullptr);
    auto window = std::make_unique<Window>(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    frontend_ = Frontend::CreateDefault();
    context_ =
        AceType::MakeRefPtr<PipelineContext>(std::move(window), taskExecutor, assetManager, resRegister, frontend_, 0);

    // used for creating tabContent
    hookContent = [this](const RefPtr<RenderTabContent>& tabContent) {
        tabContent->Attach(context_);
        renderTabContent_ = tabContent;
    };

    hookBar = [this](const RefPtr<RenderTabBar>& tabBar) { tabBar->Attach(context_); };
}

void RenderTabContentTest::TearDown()
{
}

void RenderTabContentTest::WaitAndMockVsync(int64_t waitFor)
{
    int64_t runningTime = 0;
    while (runningTime <= waitFor) {
        runningTime += VSYNC_INTERVAL_MILLISEC;
        context_->OnVsyncEvent(GetTickCount(), 0);
        usleep(VSYNC_INTERVAL_MICROSEC);
    }
    context_->OnVsyncEvent(GetTickCount(), 0);
}

void RenderTabContentTest::DragTabContent(const DragDirection& dragDirection)
{
    context_->OnTouchEvent(MOCK_DOWN_TOUCH_EVENT);
    TouchEvent touchPoint = MOCK_MOVE_TOUCH_EVENT;
    for (int32_t i = 0; i < 3; i++) {
        context_->OnTouchEvent(touchPoint);
        context_->OnVsyncEvent(GetTickCount(), 0);
        if (dragDirection == DragDirection::LEFT) {
            touchPoint.x -= 200.0f;
        } else if (dragDirection == DragDirection::RIGHT) {
            touchPoint.x += 200.0f;
        } else if (dragDirection == DragDirection::DOWN) {
            touchPoint.y += 300.0f;
        } else if (dragDirection == DragDirection::UP) {
            touchPoint.y -= 300.0f;
        }
    }
    TouchEvent touchEndPoint = MOCK_UP_TOUCH_EVENT;
    touchEndPoint.x = touchPoint.x;
    touchEndPoint.y = touchPoint.y;
    context_->OnTouchEvent(touchEndPoint);
    usleep(VSYNC_INTERVAL_MICROSEC);
    context_->OnVsyncEvent(GetTickCount(), 0);
    WaitAndMockVsync(WAIT_TIME);
}

void RenderTabContentTest::CreateAndRenderBarAndContent(UpdateTabBarCallback updateBar)
{
    std::list<RefPtr<Component>> barChildren;
    for (uint32_t i = 0; i < 5; i++) {
        barChildren.emplace_back(AceType::MakeRefPtr<BoxComponent>());
    }

    auto tabController = TabController::GetController(0);

    RefPtr<TabBarComponent> tabBarComponent =
        AceType::MakeRefPtr<TabBarComponent>(barChildren, tabController);

    std::list<RefPtr<Component>> componentChildren;
    for (uint32_t i = 0; i < 5; i++) {
        componentChildren.emplace_back(AceType::MakeRefPtr<BoxComponent>());
    }

    RefPtr<TabContentComponent> tabContentComponent =
        AceType::MakeRefPtr<TabContentComponent>(componentChildren, tabController);

    if (updateBar) {
        updateBar(tabBarComponent);
    }

    std::list<RefPtr<Component>> children;
    children.push_back(tabBarComponent);
    children.push_back(tabContentComponent);

    RefPtr<ColumnComponent> column =
        AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::FLEX_START, children);
    auto pageComponent = AceType::MakeRefPtr<PageComponent>(0, "", column);

    context_->SetupRootElement();
    context_->PushPage(pageComponent);
    context_->OnVsyncEvent(GetTickCount(), 0);
    context_->OnSurfaceChanged(1080, 2244);
}

void RenderTabContentTest::CreateAndRenderContent(UpdateTabContentCallback callback)
{
    std::list<RefPtr<Component>> componentChildren;
    for (uint32_t i = 0; i < 5; i++) {
        componentChildren.emplace_back(AceType::MakeRefPtr<BoxComponent>());
    }

    auto tabController = TabController::GetController(0);

    RefPtr<TabContentComponent> tabContentComponent =
        AceType::MakeRefPtr<TabContentComponent>(componentChildren, tabController);

    if (callback) {
        callback(tabContentComponent);
    }

    auto pageComponent = AceType::MakeRefPtr<PageComponent>(0, "", tabContentComponent);

    context_->SetupRootElement();
    context_->PushPage(pageComponent);
    context_->OnVsyncEvent(GetTickCount(), 0);
    context_->OnSurfaceChanged(1080, 2244);
}

void RenderTabContentTest::CreateBarAndContent(
    RefPtr<TabBarComponent>& tabBarComponent, UpdateTabContentCallback callback)
{
    std::list<RefPtr<Component>> barChildren;
    for (uint32_t i = 0; i < 5; i++) {
        barChildren.emplace_back(AceType::MakeRefPtr<BoxComponent>());
    }

    auto tabController = TabController::GetController(0);

    tabBarComponent = AceType::MakeRefPtr<TabBarComponent>(barChildren, tabController);

    std::list<RefPtr<Component>> componentChildren;
    for (uint32_t i = 0; i < 5; i++) {
        componentChildren.emplace_back(AceType::MakeRefPtr<BoxComponent>());
    }

    RefPtr<TabContentComponent> tabContentComponent =
        AceType::MakeRefPtr<TabContentComponent>(componentChildren, tabController);

    if (callback) {
        callback(tabContentComponent);
    }
    std::list<RefPtr<Component>> children;
    children.push_back(tabBarComponent);
    children.push_back(tabContentComponent);

    RefPtr<ColumnComponent> column =
        AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::FLEX_START, children);
    auto pageComponent = AceType::MakeRefPtr<PageComponent>(0, "", column);

    context_->SetupRootElement();
    context_->PushPage(pageComponent);
    context_->OnVsyncEvent(GetTickCount(), 0);
    context_->OnSurfaceChanged(1080, 2244);
}

bool RenderTabContentTest::TestContentPosition(int32_t index, double position)
{
    const auto& contents = renderTabContent_->GetChildContents();
    auto it = contents.find(index);
    if (it == contents.end()) {
        return false;
    }

    if (!NearEqual((*it).second->GetPosition().GetX(), position)) {
        return false;
    }
    return true;
}

/**
 * @tc.name: RenderTabContentTest001
 * @tc.desc: Test the invalid index value to create a tab component. The component can be successfully created, but
 * the initial tabContent display page is the default value.
 * @tc.type: FUNC
 * @tc.require: AR000DAR32 AR000DHJ5Q
 * @tc.author: liujinwei
 */
HWTEST_F(RenderTabContentTest, RenderTabContentTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest001 start";

    /**
     * @tc.steps: step1. Build a component tree containing sub-components, the index field of the
     * component is -1.
     * @tc.expected: step1. TabContent component displays the first page.
     */
    CreateAndRenderBarAndContent([this](const RefPtr<TabBarComponent>& tabBar) { tabBar->SetIndex(-1); });
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 0);
    EXPECT_TRUE(TestContentPosition(0, 0.0));

    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest001 stop";
}

/**
 * @tc.name: RenderTabContentTest002
 * @tc.desc: Test the invalid index value to create a tab component. The component can be successfully created, but
 * the initial tabContent display page is the default value.
 * @tc.type: FUNC
 * @tc.require: AR000DAR32 AR000DHJ5Q
 * @tc.author: liujinwei
 */
HWTEST_F(RenderTabContentTest, RenderTabContentTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest002 start";

    /**
     * @tc.steps: step1. Build a component tree containing sub-components, the index field of the
     * component is 10.
     * @tc.expected: step1. TabContent component displays the first page.
     */
    CreateAndRenderBarAndContent([this](const RefPtr<TabBarComponent>& tabBar) { tabBar->SetIndex(10); });

    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 0);
    EXPECT_TRUE(TestContentPosition(0, 0.0));

    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest002 stop";
}

/**
 * @tc.name: RenderTabContentTest003
 * @tc.desc: Test the valid index value to create a tab component. The component can be successfully created,
 * the initial tabContent display page is the index value.
 * @tc.type: FUNC
 * @tc.require: AR000DAR32 AR000DHJ5Q
 * @tc.author: liujinwei
 */
HWTEST_F(RenderTabContentTest, RenderTabContentTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest003 start";

    /**
     * @tc.steps: step1. Build a component tree containing sub-components, the index field of the
     * component is 2.
     * @tc.expected: step1. TabContent component displays the third page.
     */
    CreateAndRenderBarAndContent([this](const RefPtr<TabBarComponent>& tabBar) { tabBar->SetIndex(2); });

    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 2);
    EXPECT_TRUE(TestContentPosition(2, 0.0));

    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest003 stop";
}

/**
 * @tc.name: RenderTabContentTest004
 * @tc.desc: Create a tab component. The component can be successfully created,
 * then test the link of bar and content
 * @tc.type: FUNC
 * @tc.require: AR000DAR32 AR000DHJ5Q
 * @tc.author: liujinwei
 */
HWTEST_F(RenderTabContentTest, RenderTabContentTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest004 start";

    /**
     * @tc.steps: step1. Build a component tree containing sub-components
     * @tc.expected: step1. TabContent component displays the default page.
     */
    RefPtr<TabBarComponent> tabBarComponent;
    CreateBarAndContent(tabBarComponent, [this](const RefPtr<TabContentComponent>& tabContent) {});

    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 0);
    EXPECT_TRUE(TestContentPosition(0, 0.0));

    /**
     * @tc.steps: step2. Change the index from 0 to 1
     * @tc.expected: step2. TabContent component displays the second page.
     */
    tabBarComponent->SetIndex(1);
    WaitAndMockVsync(WAIT_TIME);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 1);

    EXPECT_TRUE(TestContentPosition(0, -CONTENT_WIDTH));
    EXPECT_TRUE(TestContentPosition(1, 0.0));

    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest004 stop";
}

/**
 * @tc.name: RenderTabContentTest005
 * @tc.desc: Test the event
 * @tc.type: FUNC
 * @tc.require: AR000DAR32 AR000DHJ5Q
 * @tc.author: liujinwei
 */
HWTEST_F(RenderTabContentTest, RenderTabContentTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest005 start";
    /**
     * @tc.steps: step1. Build a component tree containing sub-components
     * @tc.expected: step1. TabContent component create successfully
     */
    std::string changeEventId = "tab_change_id";
    RefPtr<TabBarComponent> tabBarComponent;
    CreateBarAndContent(tabBarComponent, [this, changeEventId](const RefPtr<TabContentComponent>& tabContent) {
        tabContent->SetChangeEventId(EventMarker(changeEventId));
    });

    RefPtr<TestTabContentEventHandler> eventHandler = AceType::MakeRefPtr<TestTabContentEventHandler>(
        [this, changeEventId](const std::string& eventId, const std::string& param) {
            index_++;
            std::string expectChangeResult =
                std::string("\"change\",{\"index\":").append(std::to_string(index_).append("},null"));
            EXPECT_EQ(param, expectChangeResult);
            EXPECT_EQ(eventId, changeEventId);
        });

    context_->RegisterEventHandler(eventHandler);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 0);
    EXPECT_TRUE(TestContentPosition(0, 0.0));

    /**
     * @tc.steps: step2. Set the index of bar from 0 to 1.
     * @tc.expected: step2. TabContent component change index successfully, and fire the event successfully.
     */
    tabBarComponent->SetIndex(1);
    WaitAndMockVsync(WAIT_TIME);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 1);
    EXPECT_TRUE(TestContentPosition(1, 0.0));

    /**
     * @tc.steps: step3. Set the index of bar from 1 to 2.
     * @tc.expected: step3. TabContent component change index successfully, and fire the event successfully.
     */
    tabBarComponent->SetIndex(2);
    WaitAndMockVsync(WAIT_TIME);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 2);
    EXPECT_TRUE(TestContentPosition(2, 0.0));

    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest005 stop";
}

/**
 * @tc.name: RenderTabContentTest006
 * @tc.desc: Test the drag
 * @tc.type: FUNC
 * @tc.require: AR000DAR32 AR000DHJ5Q
 * @tc.author: liujinwei
 */
HWTEST_F(RenderTabContentTest, RenderTabContentTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest006 start";

    /**
     * @tc.steps: step1. Build a component tree containing sub-components.
     * @tc.expected: step1. TabContent component create successfully.
     */
    std::string changeEventId = "tab_change_id";

    CreateAndRenderContent([this, changeEventId](const RefPtr<TabContentComponent>& tabContent) {
        tabContent->SetChangeEventId(EventMarker(changeEventId));
    });

    RefPtr<TestTabContentEventHandler> eventHandler = AceType::MakeRefPtr<TestTabContentEventHandler>(
        [this, changeEventId](const std::string& eventId, const std::string& param) {
            std::string expectChangeResult =
                std::string("\"change\",{\"index\":").append(std::to_string(index_).append("},null"));
            EXPECT_EQ(param, expectChangeResult);
            EXPECT_EQ(eventId, changeEventId);
        });

    context_->RegisterEventHandler(eventHandler);

    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 0);
    EXPECT_TRUE(TestContentPosition(0, 0.0));

    /**
     * @tc.steps: step2. Drag left
     * @tc.expected: step2. TabContent change the index from 0 to 1, and fire the change event.
     */
    index_ = 1;
    DragTabContent(DragDirection::LEFT);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 1);
    EXPECT_TRUE(TestContentPosition(1, 0.0));

    /**
     * @tc.steps: step3. Drag left
     * @tc.expected: step3. TabContent change the index from 1 to 2, and fire the change event.
     */
    index_ = 2;
    DragTabContent(DragDirection::LEFT);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 2);
    EXPECT_TRUE(TestContentPosition(2, 0.0));

    /**
     * @tc.steps: step4. Drag left and to the last, and then drag right
     * @tc.expected: step4. TabContent change the index, and fire the change event.
     */
    index_ = 3;
    DragTabContent(DragDirection::LEFT);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 3);
    EXPECT_TRUE(TestContentPosition(3, 0.0));

    index_ = 4;
    DragTabContent(DragDirection::LEFT);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 4);
    EXPECT_TRUE(TestContentPosition(4, 0.0));

    index_ = 4;
    DragTabContent(DragDirection::LEFT);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 4);
    EXPECT_TRUE(TestContentPosition(4, 0.0));

    index_ = 3;
    DragTabContent(DragDirection::RIGHT);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 3);
    EXPECT_TRUE(TestContentPosition(3, 0.0));

    index_ = 2;
    DragTabContent(DragDirection::RIGHT);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 2);
    EXPECT_TRUE(TestContentPosition(2, 0.0));

    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest006 stop";
}

/**
 * @tc.name: RenderTabContentTest007
 * @tc.desc: Create a tab component. The component can be successfully created,
 * then test the lazy build of the content
 * @tc.type: FUNC
 * @tc.require: AR000DAR32
 * @tc.author: liujinwei
 */
HWTEST_F(RenderTabContentTest, RenderTabContentTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest007 start";

    /**
     * @tc.steps: step1. Build a component tree containing sub-components
     * @tc.expected: step1. TabContent component displays the default page.
     */
    RefPtr<TabBarComponent> tabBarComponent;
    CreateBarAndContent(tabBarComponent, [this](const RefPtr<TabContentComponent>& tabContent) {});

    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 0);
    EXPECT_TRUE(TestContentPosition(0, 0.0));
    EXPECT_TRUE(renderTabContent_->GetChildContents().size() - 1 == 0);

    /**
     * @tc.steps: step2. Change the index
     * @tc.expected: step2. TabContent component size is correct.
     */
    tabBarComponent->SetIndex(1);
    WaitAndMockVsync(WAIT_TIME);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 1);
    EXPECT_TRUE(TestContentPosition(1, 0.0));
    EXPECT_TRUE(renderTabContent_->GetChildContents().size() - 2 == 0);

    tabBarComponent->SetIndex(4);
    WaitAndMockVsync(WAIT_TIME);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 4);
    EXPECT_TRUE(TestContentPosition(4, 0.0));
    EXPECT_TRUE(renderTabContent_->GetChildContents().size() - 3 == 0);

    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest007 stop";
}

/**
 * @tc.name: RenderTabContentTest008
 * @tc.desc: Test the event of the first tab and last one
 * @tc.type: FUNC
 * @tc.require: AR000DAR32
 * @tc.author: liujinwei
 */
HWTEST_F(RenderTabContentTest, RenderTabContentTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest008 start";

    /**
     * @tc.steps: step1. Build a component tree containing sub-components.
     * @tc.expected: step1. TabContent component create successfully.
     */
    std::string changeEventId = "tab_change_id";

    CreateAndRenderContent([this, changeEventId](const RefPtr<TabContentComponent>& tabContent) {
        tabContent->SetChangeEventId(EventMarker(changeEventId));
    });

    RefPtr<TestTabContentEventHandler> eventHandler = AceType::MakeRefPtr<TestTabContentEventHandler>(
        [this, changeEventId](const std::string& eventId, const std::string& param) {
            std::string expectChangeResult =
                std::string("\"change\",{\"index\":").append(std::to_string(index_).append("},null"));
            EXPECT_EQ(param, expectChangeResult);
            EXPECT_EQ(eventId, changeEventId);
            totalEventsNum_++;
        });

    context_->RegisterEventHandler(eventHandler);

    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 0);
    EXPECT_TRUE(TestContentPosition(0, 0.0));

    /**
     * @tc.steps: step2. Drag left
     * @tc.expected: step2. TabContent change the index from 0 to 1, and fire the change event.
     */
    index_ = 1;
    DragTabContent(DragDirection::LEFT);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 1);
    EXPECT_TRUE(TestContentPosition(1, 0.0));

    /**
     * @tc.steps: step3. Drag right
     * @tc.expected: step3. TabContent change the index from 1 to 0, and fire the change event.
     */
    index_ = 0;
    DragTabContent(DragDirection::RIGHT);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 0);
    EXPECT_TRUE(TestContentPosition(0, 0.0));
    EXPECT_EQ(totalEventsNum_, 2);

    /**
     * @tc.steps: step4. Drag to the first, and then drag right
     * @tc.expected: step4. The index is 0.
     */
    index_ = 0;
    DragTabContent(DragDirection::RIGHT);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 0);
    EXPECT_TRUE(TestContentPosition(0, 0.0));
    EXPECT_EQ(totalEventsNum_, 2);

    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest008 stop";
}

/**
 * @tc.name: RenderTabContentTest009
 * @tc.desc: Test the event of the first tab and last one
 * @tc.type: FUNC
 * @tc.require: AR000DAR32
 * @tc.author: liujinwei
 */
HWTEST_F(RenderTabContentTest, RenderTabContentTest009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest009 start";

    /**
     * @tc.steps: step1. Build a component tree containing sub-components.
     * @tc.expected: step1. TabContent component create successfully.
     */
    std::string changeEventId = "tab_change_id";
    RefPtr<TabBarComponent> tabBarComponent;
    CreateBarAndContent(tabBarComponent, [this, changeEventId](const RefPtr<TabContentComponent>& tabContent) {
        tabContent->SetChangeEventId(EventMarker(changeEventId));
    });

    RefPtr<TestTabContentEventHandler> eventHandler = AceType::MakeRefPtr<TestTabContentEventHandler>(
        [this, changeEventId](const std::string& eventId, const std::string& param) {
            std::string expectChangeResult =
                std::string("\"change\",{\"index\":").append(std::to_string(index_).append("},null"));
            EXPECT_EQ(param, expectChangeResult);
            EXPECT_EQ(eventId, changeEventId);
            totalEventsNum_++;
        });

    context_->RegisterEventHandler(eventHandler);

    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 0);
    EXPECT_TRUE(TestContentPosition(0, 0.0));

    /**
     * @tc.steps: step2. Set index = 4
     * @tc.expected: step2. TabContent change the index from 0 to 4, and fire the change event.
     */
    index_ = 4;
    tabBarComponent->SetIndex(4);
    WaitAndMockVsync(WAIT_TIME);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 4);
    EXPECT_TRUE(TestContentPosition(4, 0.0));
    EXPECT_EQ(totalEventsNum_, 1);

    /**
     * @tc.steps: step3. Drag left
     * @tc.expected: step3. Index of tab is 4
     */
    DragTabContent(DragDirection::LEFT);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 4);
    EXPECT_TRUE(TestContentPosition(4, 0.0));
    EXPECT_EQ(totalEventsNum_, 1);

    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest009 stop";
}

/**
 * @tc.name: RenderTabContentTest010
 * @tc.desc: Test the drag
 * @tc.type: FUNC
 * @tc.require: AR000DQ1V2 AR000DQ1V3
 * @tc.author: liujinwei
 */
HWTEST_F(RenderTabContentTest, RenderTabContentTest010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest010 start";

    /**
     * @tc.steps: step1. Build a component tree containing sub-components.
     * @tc.expected: step1. TabContent component create successfully.
     */
    std::string changeEventId = "tab_change_id";

    CreateAndRenderContent([this, changeEventId](const RefPtr<TabContentComponent>& tabContent) {
        tabContent->SetChangeEventId(EventMarker(changeEventId));
        tabContent->SetVertical(true);
    });

    RefPtr<TestTabContentEventHandler> eventHandler = AceType::MakeRefPtr<TestTabContentEventHandler>(
        [this, changeEventId](const std::string& eventId, const std::string& param) {
            std::string expectChangeResult =
                std::string("\"change\",{\"index\":").append(std::to_string(index_).append("},null"));
            EXPECT_EQ(param, expectChangeResult);
            EXPECT_EQ(eventId, changeEventId);
        });

    context_->RegisterEventHandler(eventHandler);

    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 0);
    EXPECT_TRUE(TestContentPosition(0, 0.0));

    /**
     * @tc.steps: step2. Drag up
     * @tc.expected: step2. TabContent change the index from 0 to 1, and fire the change event.
     */
    index_ = 1;
    DragTabContent(DragDirection::UP);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 1);
    EXPECT_TRUE(TestContentPosition(1, 0.0));

    /**
     * @tc.steps: step3. Drag up
     * @tc.expected: step3. TabContent change the index from 1 to 2, and fire the change event.
     */
    index_ = 2;
    DragTabContent(DragDirection::UP);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 2);
    EXPECT_TRUE(TestContentPosition(2, 0.0));

    /**
     * @tc.steps: step4. Drag up and to the last, and then drag right
     * @tc.expected: step4. TabContent change the index, and fire the change event.
     */
    index_ = 3;
    DragTabContent(DragDirection::UP);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 3);
    EXPECT_TRUE(TestContentPosition(3, 0.0));

    index_ = 4;
    DragTabContent(DragDirection::UP);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 4);
    EXPECT_TRUE(TestContentPosition(4, 0.0));

    index_ = 4;
    DragTabContent(DragDirection::UP);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 4);
    EXPECT_TRUE(TestContentPosition(4, 0.0));

    index_ = 3;
    DragTabContent(DragDirection::DOWN);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 3);
    EXPECT_TRUE(TestContentPosition(3, 0.0));

    index_ = 2;
    DragTabContent(DragDirection::DOWN);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 2);
    EXPECT_TRUE(TestContentPosition(2, 0.0));

    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest010 stop";
}

/**
 * @tc.name: RenderTabContentTest011
 * @tc.desc: Create a tab component. The component can be successfully created,
 * then test the link of bar and content when vertical is true
 * @tc.type: FUNC
 * @tc.require: AR000DQ1V2 AR000DQ1V3
 * @tc.author: liujinwei
 */
HWTEST_F(RenderTabContentTest, RenderTabContentTest011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest011 start";

    /**
     * @tc.steps: step1. Build a component tree containing sub-components
     * @tc.expected: step1. TabContent component displays the default page.
     */
    RefPtr<TabBarComponent> tabBarComponent;
    CreateBarAndContent(
        tabBarComponent, [this](const RefPtr<TabContentComponent>& tabContent) { tabContent->SetVertical(true); });

    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 0);
    EXPECT_TRUE(TestContentPosition(0, 0.0));

    /**
     * @tc.steps: step2. Change the index from 0 to 1
     * @tc.expected: step2. TabContent component displays the second page.
     */
    tabBarComponent->SetIndex(2);
    WaitAndMockVsync(WAIT_TIME);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 2);

    EXPECT_TRUE(TestContentPosition(2, 0.0));

    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest011 stop";
}

/**
 * @tc.name: RenderTabContentTest012
 * @tc.desc: Create a tab component. The component can be successfully created,
 * then test the link of bar and content
 * @tc.type: FUNC
 * @tc.require: AR000DQ4E3
 * @tc.author: liujinwei
 */
HWTEST_F(RenderTabContentTest, RenderTabContentTest012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest012 start";

    /**
     * @tc.steps: step1. Build a component tree containing sub-components
     * @tc.expected: step1. TabContent component displays the default page.
     */
    RefPtr<TabBarComponent> tabBarComponent;
    CreateBarAndContent(tabBarComponent, [this](const RefPtr<TabContentComponent>& tabContent) {});

    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 0);

    /**
     * @tc.steps: step2. Change the index from 0 to 1
     * @tc.expected: step2. TabContent component displays the second page.
     */
    tabBarComponent->SetIndex(1);
    WaitAndMockVsync(WAIT_TIME);
    EXPECT_EQ(renderTabContent_->GetCurrentIndex(), 1);

    GTEST_LOG_(INFO) << "RenderTabContentTest RenderTabContentTest012 stop";
}

} // namespace OHOS::Ace
