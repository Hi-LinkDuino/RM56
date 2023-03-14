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

#include "events/key_event.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const uint16_t KEY_ID = 10;
    const uint16_t STATE = 20;
}

class KeyEventTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static KeyEvent* keyEvent_;
};

KeyEvent* KeyEventTest::keyEvent_ = nullptr;

void KeyEventTest::SetUpTestCase(void)
{
    if (keyEvent_ == nullptr) {
        keyEvent_ = new KeyEvent(KEY_ID, STATE);
    }
}

void KeyEventTest::TearDownTestCase(void)
{
    if (keyEvent_ != nullptr) {
        delete keyEvent_;
        keyEvent_ = nullptr;
    }
}

/**
 * @tc.name: KeyEventGetKeyId_001
 * @tc.desc: Verify GetKeyId function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(KeyEventTest, KeyEventGetKeyId_001, TestSize.Level1)
{
    if (keyEvent_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(keyEvent_->GetKeyId(), KEY_ID);
}

/**
 * @tc.name: KeyEventGetState_001
 * @tc.desc: Verify GetState function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(KeyEventTest, KeyEventGetState_001, TestSize.Level0)
{
    if (keyEvent_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(keyEvent_->GetState(), STATE);
}
} // namespace OHOS
