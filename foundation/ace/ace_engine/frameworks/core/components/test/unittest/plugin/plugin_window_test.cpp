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

#include <gmock/gmock.h>
#include "gtest/gtest.h"

#include "core/common/frontend.h"
#include "core/mock/fake_asset_manager.h"
#include "core/mock/fake_task_executor.h"
#include "core/mock/mock_resource_register.h"
#include "core/components/plugin/plugin_window.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
class MockPluginWindow : public Window {
public:
    explicit MockPluginWindow(std::unique_ptr<PlatformWindow> platformWindow) : Window(nullptr) {}

    ~MockPluginWindow() = default;

    virtual void RequestFrame()
    {
        GTEST_LOG_(INFO) << "MockPluginWindow::RequestFrame is called";
        isRequestFrameCalled_ = true;
    };

    void SetVsyncCallback(AceVsyncCallback&& callback)
    {
        GTEST_LOG_(INFO) << "MockPluginWindow::SetVsyncCallback is called";
        isSetVsyncCallback_ = true;
    }

    static bool GetRequestFrameCalled()
    {
        return isRequestFrameCalled_;
    }

    static bool GetVsyncCallback()
    {
        return isSetVsyncCallback_;
    }

private:
    static bool isRequestFrameCalled_;
    static bool isSetVsyncCallback_;
};
bool MockPluginWindow::isRequestFrameCalled_ = false;
bool MockPluginWindow::isSetVsyncCallback_ = false;

class PluginWindowTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: PluginWindowRequestFrameTest001
 * @tc.desc: Verify the RequestFrame Interface of PluginWindow work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginWindowTest, PluginWindowRequestFrameTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin window.
     */
    auto platformWindow = PlatformWindow::Create(nullptr);
    auto mockPluginWindow = std::make_unique<MockPluginWindow>(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    auto fakeFrontend = Frontend::CreateDefault();

    auto pipelineContext = AceType::MakeRefPtr<PipelineContext>(
        std::move(mockPluginWindow), taskExecutor, assetManager, resRegister, fakeFrontend, 0);
    PluginWindow pluginWindow(pipelineContext);

    /**
     * @tc.steps: step2. Request Frame.
     * @tc.expected: step2. Request Frame success.
     */
    pluginWindow.RequestFrame();
    EXPECT_TRUE(MockPluginWindow::GetRequestFrameCalled());
}

/**
 * @tc.name: PluginWindowSetVsyncCallbackTest001
 * @tc.desc: Verify the SetVsyncCallback Interface of PluginWindow work correctly with plugin component.
 * @tc.type: FUNC
 */
HWTEST_F(PluginWindowTest, PluginWindowSetVsyncCallbackTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin window.
     */
    GTEST_LOG_(INFO) << "PluginWindowRequestFrameTest001 is begain";
    auto platformWindow = PlatformWindow::Create(nullptr);
    auto mockPluginWindow = std::make_unique<MockPluginWindow>(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    auto fakeFrontend = Frontend::CreateDefault();

    auto pipelineContext = AceType::MakeRefPtr<PipelineContext>(
        std::move(mockPluginWindow), taskExecutor, assetManager, resRegister, fakeFrontend, 0);
    PluginWindow pluginWindow(pipelineContext);

    /**
     * @tc.steps: step2. Set Vsync Callback.
     * @tc.expected: step2. Set Vsync Callback success.
     */
    auto&& vsyncCallback = [](const uint64_t a = 0, const uint32_t b = 0) {
        EXPECT_FALSE(MockPluginWindow::GetVsyncCallback());
    };
    pluginWindow.SetVsyncCallback(vsyncCallback);
    EXPECT_TRUE(MockPluginWindow::GetVsyncCallback());
}
} // namespace OHOS::Ace