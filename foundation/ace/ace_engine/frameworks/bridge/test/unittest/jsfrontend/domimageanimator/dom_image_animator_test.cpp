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

#include "frameworks/bridge/common/dom/dom_image_animator.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

    const int32_t ITERATION_TEST = 0;
    const int32_t IMAGES_SIZE_TEST = 0;

} // namespace

class DomImageAnimatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DomImageAnimatorTest::SetUpTestCase() {}
void DomImageAnimatorTest::TearDownTestCase() {}
void DomImageAnimatorTest::SetUp() {}
void DomImageAnimatorTest::TearDown() {}

/**
 * @tc.name: SetAttributes001
 * @tc.desc: Verify that DomImageAnimator can create imageAnimatorComponent correctly.
 * @tc.type: FUNC
 */
HWTEST_F(DomImageAnimatorTest, SetAttributes001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DomImageAnimatorTest SetAttributes001 start";
    /**
     * @tc.steps: step1. the json string of DomImageAnimator with empty attributes set.
     */
    const std::string jsonImageAnimatorStr = ""
                                             "{                                "
                                             "  \"tag\" : \"image-animator\",  "
                                             "  \"attr\" : []                  "
                                             "}";

    /**
     * @tc.steps: step2. call JsonUtil interface and create DomImageAnimator.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonImageAnimatorStr);
    auto imageAnimatorComponent = AceType::DynamicCast<ImageAnimatorComponent>(domNodeRoot->GetSpecializedComponent());

    /**
     * @tc.steps: step3. Verify whether the imageAnimatorComponent is created correctly.
     * @tc.expected: step3. imageAnimatorComponent is created as excepted.
     */
    ASSERT_TRUE(imageAnimatorComponent != nullptr);
}

/**
 * @tc.name: SetAttributes002
 * @tc.desc: Verify that the attributes of imageAnimatorComponent can be set correctly.
 * @tc.type: FUNC
 */
HWTEST_F(DomImageAnimatorTest, SetAttributes002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DomImageAnimatorTest SetAttributes002 start";
    /**
     * @tc.steps: step1. the json string of DomImageAnimator with some attributes set.
     */
    const std::string jsonImageAnimatorStr = ""
                                             "{                                               "
                                             "  \"tag\" : \"image-animator\",                 "
                                             "  \"attr\" : [{                                 "
                                             "                \"iteration\" : \"0\"           "
                                             "              },                                "
                                             "              {                                 "
                                             "                \"reverse\" : \"true\"          "
                                             "              }]                                "
                                             "}";

    /**
     * @tc.steps: step2. call JsonUtil interface and create DomImageAnimator.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonImageAnimatorStr);
    auto imageAnimatorComponent = AceType::DynamicCast<ImageAnimatorComponent>(domNodeRoot->GetSpecializedComponent());

    /**
     * @tc.steps: step3. Verify whether the attributes of imageAnimatorComponent is set correctly.
     * @tc.expected: step3. the attributes are set as excepted.
     */
    ASSERT_TRUE(imageAnimatorComponent->GetIteration() == ITERATION_TEST);
    ASSERT_TRUE(imageAnimatorComponent->GetIsReverse());
}

/**
 * @tc.name: SetAttributes003
 * @tc.desc: Verify that the attribute of images can be set correctly.
 * @tc.type: FUNC
 */
HWTEST_F(DomImageAnimatorTest, SetAttributes003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. the json string of DomImageAnimator with images set.
     */
    const std::string jsonImageAnimatorStr = ""
                                             "{                                                                   "
                                             "  \"tag\" : \"image-animator\",                                     "
                                             "  \"attr\" : [{                                                     "
                                             "                \"images\" : []                                     "
                                             "             }]                                                     "
                                             "}";

    /**
     * @tc.steps: step2. call JsonUtil interface and create DomImageAnimator.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonImageAnimatorStr);
    auto imageAnimatorComponent = AceType::DynamicCast<ImageAnimatorComponent>(domNodeRoot->GetSpecializedComponent());

    /**
     * @tc.steps: step3. Verify whether the attribute of images is set correctly.
     * @tc.expected: step3. the attribute is set as excepted.
     */
    ASSERT_TRUE(imageAnimatorComponent->GetImageProperties().size() == IMAGES_SIZE_TEST);
}

} // namespace OHOS::Ace::Framework
