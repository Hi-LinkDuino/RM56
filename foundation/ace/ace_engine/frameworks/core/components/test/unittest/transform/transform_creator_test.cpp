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

#include "gtest/gtest.h"

#include "base/json/json_util.h"
#include "base/utils/utils.h"
#include "core/components/test/json/component_factory.h"
#include "core/components/test/json/transform_creator.h"
#include "core/components/text/text_component.h"
#include "core/components/transform/transform_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const std::string TEXT_VALUE = "Hi Ace";

} // namespace

class TransformCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void TransformCreatorTest::SetUpTestCase() {}
void TransformCreatorTest::TearDownTestCase() {}
void TransformCreatorTest::SetUp() {}
void TransformCreatorTest::TearDown() {}

/**
 * @tc.name: TransformCreator001
 * @tc.desc: Verify TransformCreator can create transform component with a translate operation.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUJ
 * @tc.author: jinwuwen
 */
HWTEST_F(TransformCreatorTest, TransformCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of transform component with a translate operation.
     */
    const std::string content = ""
                                "{                                "
                                "  \"className\": \"Transform\",  "
                                "  \"transform\": \"translate\",  "
                                "  \"parameter\": {               "
                                "    \"px\": 100,                 "
                                "    \"py\": 150,                 "
                                "    \"pz\": 0                    "
                                "  },                             "
                                "  \"child\": {                   "
                                "    \"className\": \"Text\",     "
                                "    \"value\": \"Hi Ace\"        "
                                "  }                              "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get transform component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<TransformComponent> transform = AceType::DynamicCast<TransformComponent>(component);
    ASSERT_TRUE(transform != nullptr);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(transform->GetChild());
    ASSERT_TRUE(text != nullptr);
    ASSERT_TRUE(text->GetData() == TEXT_VALUE);
    Matrix4 transformMatrix = transform->GetTransform();
    ASSERT_TRUE(transformMatrix == Matrix4::CreateTranslate(100.0f, 150.0f, 0.0f));
}

/**
 * @tc.name: TransformCreator002
 * @tc.desc: Verify TransformCreator can create transform component with a rotate operation.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUH
 * @tc.author: jinwuwen
 */
HWTEST_F(TransformCreatorTest, TransformCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of transform component with a rotate operation.
     */
    const std::string content = ""
                                "{                                "
                                "  \"className\": \"Transform\",  "
                                "  \"transform\": \"rotate\",     "
                                "  \"angleValue\": 45,            "
                                "  \"parameter\": {               "
                                "    \"px\": 0,                   "
                                "    \"py\": 0,                   "
                                "    \"pz\": 1                    "
                                "  },                             "
                                "  \"child\": {                   "
                                "    \"className\": \"Text\",     "
                                "    \"value\": \"Hi Ace\"        "
                                "  }                              "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get transform component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<TransformComponent> transform = AceType::DynamicCast<TransformComponent>(component);
    ASSERT_TRUE(transform != nullptr);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(transform->GetChild());
    ASSERT_TRUE(text != nullptr);
    ASSERT_TRUE(text->GetData() == TEXT_VALUE);
    Matrix4 transformMatrix = transform->GetTransform();
    Matrix4 rotateMatrix = Matrix4::CreateRotate(45.0f, 0.0f, 0.0f, 1.0f);
    rotateMatrix.SetEntry(3, 2, PERSPECTIVE);
    ASSERT_TRUE(transformMatrix == rotateMatrix);
}

/**
 * @tc.name: TransformCreator003
 * @tc.desc: Verify TransformCreator can create transform component with a scale operation.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUI
 * @tc.author: jinwuwen
 */
HWTEST_F(TransformCreatorTest, TransformCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of transform component with a scale operation.
     */
    const std::string content = ""
                                "{                                "
                                "  \"className\": \"Transform\",  "
                                "  \"transform\": \"scale\",  "
                                "  \"parameter\": {               "
                                "    \"px\": 2,                 "
                                "    \"py\": 0.5,                 "
                                "    \"pz\": 1                    "
                                "  },                             "
                                "  \"child\": {                   "
                                "    \"className\": \"Text\",     "
                                "    \"value\": \"Hi Ace\"        "
                                "  }                              "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get transform component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<TransformComponent> transform = AceType::DynamicCast<TransformComponent>(component);
    ASSERT_TRUE(transform != nullptr);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(transform->GetChild());
    ASSERT_TRUE(text != nullptr);
    ASSERT_TRUE(text->GetData() == TEXT_VALUE);
    Matrix4 transformMatrix = transform->GetTransform();
    ASSERT_TRUE(transformMatrix == Matrix4::CreateScale(2.0f, 0.5f, 1.0f));
}

/**
 * @tc.name: TransformCreator004
 * @tc.desc: Verify TransformCreator can create transform component with a scale operation.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUI
 * @tc.author: jinwuwen
 */
HWTEST_F(TransformCreatorTest, TransformCreator004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of transform component with a scale operation.
     */
    const std::string content = ""
                                "{                                "
                                "  \"className\": \"Transform\",  "
                                "  \"transform\": \"scale\",      "
                                "  \"scaleValue\": 1.5,           "
                                "  \"parameter\": {               "
                                "    \"px\": 2,                   "
                                "    \"py\": 0.5,                 "
                                "    \"pz\": 1                    "
                                "  },                             "
                                "  \"child\": {                   "
                                "    \"className\": \"Text\",     "
                                "    \"value\": \"Hi Ace\"        "
                                "  }                              "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get transform component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<TransformComponent> transform = AceType::DynamicCast<TransformComponent>(component);
    ASSERT_TRUE(transform != nullptr);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(transform->GetChild());
    ASSERT_TRUE(text != nullptr);
    ASSERT_TRUE(text->GetData() == TEXT_VALUE);
    Matrix4 transformMatrix = transform->GetTransform();
    ASSERT_TRUE(transformMatrix == Matrix4::CreateScale(2.0f, 0.5f, 1.0f));
}

/**
 * @tc.name: TransformCreator005
 * @tc.desc: Verify TransformCreator can create transform component with a scale operation.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUI
 * @tc.author: jinwuwen
 */
HWTEST_F(TransformCreatorTest, TransformCreator005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of transform component with a scale operation.
     */
    const std::string content = ""
                                "{                                "
                                "  \"className\": \"Transform\",  "
                                "  \"transform\": \"scale\",      "
                                "  \"scaleValue\": 1.5,           "
                                "  \"child\": {                   "
                                "    \"className\": \"Text\",     "
                                "    \"value\": \"Hi Ace\"        "
                                "  }                              "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get transform component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<TransformComponent> transform = AceType::DynamicCast<TransformComponent>(component);
    ASSERT_TRUE(transform != nullptr);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(transform->GetChild());
    ASSERT_TRUE(text != nullptr);
    ASSERT_TRUE(text->GetData() == TEXT_VALUE);
    Matrix4 transformMatrix = transform->GetTransform();
    ASSERT_TRUE(transformMatrix == Matrix4::CreateScale(1.5f, 1.5f, 1.0f));
}

} // namespace OHOS::Ace