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

#include <base/memory/ace_type.h>

#include "core/event/multimodal/multimodal_manager.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class MultimodalTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: MultimodalTest001
 * @tc.desc: Verify multi-mode page logic.
 * @tc.type: FUNC
 * @tc.require: AR000DQ3VB
 * @tc.author: jiangyingjie
 */
HWTEST_F(MultimodalTest, MultimodalTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Multi-mode manager for building two pages
     */
    auto manager = AceType::MakeRefPtr<MultiModalManager>();
    manager->PushActiveScene(1);
    manager->PushActiveScene(2);

    /**
     * @tc.steps: step2. get current multimodalScene
     * @tc.expected: step2. pageId of multimodalScene is 2.
     */
    auto multimodalScene = manager->GetCurrentMultiModalScene();
    EXPECT_EQ(multimodalScene->GetPageId(), 2);

    /**
     * @tc.steps: step3. pop current multimodalScene
     * @tc.expected: step3. pageId of multimodalScene is 1.
     */
    manager->PopActiveScene(2, 1);
    multimodalScene = manager->GetCurrentMultiModalScene();
    EXPECT_EQ(multimodalScene->GetPageId(), 1);

    /**
     * @tc.steps: step4. replace current multimodalScene
     * @tc.expected: step4. pageId of multimodalScene is 3.
     */
    manager->ReplaceActiveScene(3, 1);
    multimodalScene = manager->GetCurrentMultiModalScene();
    EXPECT_EQ(multimodalScene->GetPageId(), 3);
}

/**
 * @tc.name: MultimodalTest002
 * @tc.desc: Verification of incremental logic.
 * @tc.type: FUNC
 * @tc.require: AR000DQ3VE
 * @tc.author: jiangyingjie
 */
HWTEST_F(MultimodalTest, MultimodalTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Multi-mode manager for building two pages
     */
    auto manager = AceType::MakeRefPtr<MultiModalManager>();
    manager->PushActiveScene(1);
    manager->PushActiveScene(2);

    /**
     * @tc.steps: step2. Get current corner number
     * @tc.expected: step2. Corner number increment.
     */
    auto multimodalScene = manager->GetCurrentMultiModalScene();
    EXPECT_EQ(multimodalScene->GetAvailableSubscriptId(), "1");
    EXPECT_EQ(multimodalScene->GetAvailableSubscriptId(), "2");
    EXPECT_EQ(multimodalScene->GetAvailableSubscriptId(), "3");
    EXPECT_EQ(multimodalScene->GetAvailableSubscriptId(), "4");

    /**
     * @tc.steps: step3. Remove corner number 3
     * @tc.expected: step3. Minus one for the largest corner number.
     */
    multimodalScene->RemoveSubscriptId("3");
    EXPECT_EQ(multimodalScene->GetAvailableSubscriptId(), "3");

    /**
     * @tc.steps: step4. Get the corner of a new page
     * @tc.expected: step4. The subscript increases from 1.
     */
    multimodalScene = manager->GetMultiModalScene(1);
    EXPECT_EQ(multimodalScene->GetAvailableSubscriptId(), "1");
    EXPECT_EQ(multimodalScene->GetAvailableSubscriptId(), "2");
    EXPECT_EQ(multimodalScene->GetAvailableSubscriptId(), "3");

}

} // namespace OHOS::Ace