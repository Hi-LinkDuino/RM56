/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "calculator.h"
#include <gtest/gtest.h>

using namespace testing::ext;

/**
 * @tc.name: integer_add_001
 * @tc.desc: Verify the add function.
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST(CalculatorAddTest, integer_add_001, TestSize.Level1)
{
    EXPECT_EQ(4, Add(4, 0));
}

/**
 * @tc.name: integer_add_002
 * @tc.desc: Verify the add function.
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST(CalculatorAddTest, integer_add_002, TestSize.Level0)
{
    EXPECT_EQ(12, Add(5, 7));
}
