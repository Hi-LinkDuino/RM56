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

#include "common/screen.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
class ScreenTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.name: ScreenGetWidth_001
 * @tc.desc: Verify GetWidth function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ScreenTest, ScreenGetWidth_001, TestSize.Level1)
{
    EXPECT_EQ(Screen::GetInstance().GetWidth(), HORIZONTAL_RESOLUTION);
}

/**
 * @tc.name: ScreenGetHeight_001
 * @tc.desc: Verify GetHeight function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ScreenTest, ScreenGetHeight_001, TestSize.Level1)
{
    EXPECT_EQ(Screen::GetInstance().GetHeight(), VERTICAL_RESOLUTION);
}
} // namespace OHOS
