/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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
#define private public
#define protected public

#include "osal/utils/util.h"
#include "scene/player/internal/state_machine.h"
#include "scene/player/play_executor.h"

namespace OHOS {
namespace Media {
namespace Test {
class PlayExecutorStub : public PlayExecutor {
public:
    PlayExecutorStub() noexcept : isLooping_(false), stateMachine_(nullptr)
    {
    }
    bool IsSingleLoop() override
    {
        return isLooping_;
    }
    void SetLooping(bool looping)
    {
        isLooping_ = looping;
    }
    void SetStateMachine(const StateMachine* stateMachine)
    {
        stateMachine_ = stateMachine;
    }
    ErrorCode DoSetSource(const std::shared_ptr<MediaSource>& source) override
    {
        return source ? ErrorCode::SUCCESS : ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    ErrorCode DoOnReady() override
    {
        return ErrorCode::SUCCESS;
    }
    ErrorCode DoOnComplete() override
    {
        if (!isLooping_ && stateMachine_) {
            stateMachine_->SendEventAsync(Intent::STOP);
        }
        return ErrorCode::SUCCESS;
    }
    ErrorCode DoOnError(ErrorCode) override
    {
        return ErrorCode::SUCCESS;
    }

private:
    bool isLooping_;
    const StateMachine* stateMachine_;
};

PlayExecutorStub g_playExecutorStub;

class TestStateMachine : public ::testing::Test {
protected:
    void SetUp() override
    {
        g_playExecutorStub.SetStateMachine(&stateMachine);
        stateMachine.Start();
    }

    void TearDown() override
    {
    }

    static StateMachine stateMachine;
};

StateMachine TestStateMachine::stateMachine(g_playExecutorStub);

TEST_F(TestStateMachine, test_init_state)
{
    EXPECT_TRUE(stateMachine.GetCurrentState() == "InitState");
}

TEST_F(TestStateMachine, test_set_invalid_source)
{
    EXPECT_TRUE(stateMachine.GetCurrentState() == "InitState");
    EXPECT_EQ(ErrorCode::ERROR_INVALID_PARAMETER_TYPE, stateMachine.SendEvent(Intent::SET_SOURCE));
    EXPECT_TRUE(stateMachine.GetCurrentState() == "InitState");
}

TEST_F(TestStateMachine, test_set_valid_source)
{
    EXPECT_TRUE(stateMachine.GetCurrentState() == "InitState");
    auto source = std::make_shared<MediaSource>("FakeUri");
    EXPECT_EQ(ErrorCode::SUCCESS, stateMachine.SendEvent(Intent::SET_SOURCE, source));
    EXPECT_TRUE(stateMachine.GetCurrentState() == "PreparingState");
}

TEST_F(TestStateMachine, test_set_notify_error_in_preparing)
{
    EXPECT_TRUE(stateMachine.GetCurrentState() == "PreparingState");
    EXPECT_EQ(ErrorCode::SUCCESS, stateMachine.SendEvent(Intent::NOTIFY_ERROR));
    EXPECT_TRUE(stateMachine.GetCurrentState() == "InitState");
}

TEST_F(TestStateMachine, test_notify_ready)
{
    EXPECT_TRUE(stateMachine.GetCurrentState() == "InitState");
    auto source = std::make_shared<MediaSource>("FakeUri");
    EXPECT_EQ(ErrorCode::SUCCESS, stateMachine.SendEvent(Intent::SET_SOURCE, source));
    EXPECT_TRUE(stateMachine.GetCurrentState() == "PreparingState");
    EXPECT_EQ(ErrorCode::SUCCESS, stateMachine.SendEvent(Intent::NOTIFY_READY));
    EXPECT_TRUE(stateMachine.GetCurrentState() == "ReadyState");
}

TEST_F(TestStateMachine, test_play_after_ready)
{
    EXPECT_TRUE(stateMachine.GetCurrentState() == "ReadyState");
    EXPECT_EQ(ErrorCode::SUCCESS, stateMachine.SendEvent(Intent::PLAY));
    EXPECT_TRUE(stateMachine.GetCurrentState() == "PlayingState");
}

TEST_F(TestStateMachine, test_pause)
{
    EXPECT_TRUE(stateMachine.GetCurrentState() == "PlayingState");
    EXPECT_EQ(ErrorCode::SUCCESS, stateMachine.SendEvent(Intent::PAUSE));
    EXPECT_TRUE(stateMachine.GetCurrentState() == "PauseState");
}

TEST_F(TestStateMachine, test_play_after_pause)
{
    EXPECT_TRUE(stateMachine.GetCurrentState() == "PauseState");
    EXPECT_EQ(ErrorCode::SUCCESS, stateMachine.SendEvent(Intent::PLAY));
    EXPECT_TRUE(stateMachine.GetCurrentState() == "PlayingState");
}

TEST_F(TestStateMachine, test_play_complete_looping)
{
    g_playExecutorStub.SetLooping(true);
    EXPECT_TRUE(stateMachine.GetCurrentState() == "PlayingState");
    EXPECT_EQ(ErrorCode::SUCCESS, stateMachine.SendEvent(Intent::NOTIFY_COMPLETE));
    EXPECT_TRUE(stateMachine.GetCurrentState() == "PlayingState");
}

TEST_F(TestStateMachine, test_play_complete_nolooping)
{
    g_playExecutorStub.SetLooping(false);
    EXPECT_TRUE(stateMachine.GetCurrentState() == "PlayingState");
    EXPECT_EQ(ErrorCode::SUCCESS, stateMachine.SendEvent(Intent::NOTIFY_COMPLETE));
    OSAL::SleepFor(100);
    EXPECT_TRUE(stateMachine.GetCurrentState() == "InitState");
}
} // namespace Test
} // namespace Media
} // namespace OHOS