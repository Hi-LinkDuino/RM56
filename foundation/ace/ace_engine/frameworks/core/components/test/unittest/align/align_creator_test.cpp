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

#include "base/json/json_util.h"
#include "base/utils/utils.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/test/json/align_creator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const double LEFT = -1.0;
const double DEFAULT_CENTER = 0.0;
const double RIGHT = 1.0;
const double TOP = -1.0;
const double BOTTOM = 1.0;

} // namespace

class AlignCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AlignCreatorTest::SetUpTestCase() {}
void AlignCreatorTest::TearDownTestCase() {}
void AlignCreatorTest::SetUp() {}
void AlignCreatorTest::TearDown() {}

/**
 * @tc.name: AlignCreator001
 * @tc.desc: Verify that AlignCreator can parse json when horizontal is "left" and vertical is "top".
 * @tc.type: FUNC
 */
HWTEST_F(AlignCreatorTest, AlignCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of align in which horizontal is "left" and vertical is "top".
     */
    const std::string jsonAlignStr = ""
                                     "{                               "
                                     "  \"className\": \"Align\",     "
                                     "  \"child\": {                  "
                                     "    \"className\": \"Image\",   "
                                     "    \"src\": \"/example.png\" },"
                                     "  \"horizontal\": \"left\",     "
                                     "  \"vertical\": \"top\"         "
                                     "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get AlignComponent.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonAlignStr.c_str());
    size_t dslLen = jsonAlignStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    RefPtr<AlignComponent> alignComponent = AceType::DynamicCast<AlignComponent>(component);

    /**
     * @tc.steps: step3. Verify whether the alignment attribute of AlignComponent is Alignment(-1, -1).
     * @tc.expected: step3. The alignment attribute of AlignComponent is Alignment(-1, -1).
     */
    ASSERT_TRUE(alignComponent != nullptr);
    ASSERT_TRUE(NearEqual(alignComponent->GetAlignment().GetHorizontal(), LEFT));
    ASSERT_TRUE(NearEqual(alignComponent->GetAlignment().GetVertical(), TOP));
}

/**
 * @tc.name: AlignCreator002
 * @tc.desc: Verify that AlignCreator can parse json when horizontal is "center" and vertical is "center".
 * @tc.type: FUNC
 */
HWTEST_F(AlignCreatorTest, AlignCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of align in which horizontal is "center" and vertical is "center".
     */
    const std::string jsonAlignStr = ""
                                     "{                               "
                                     "  \"className\": \"Align\",     "
                                     "  \"child\": {                  "
                                     "    \"className\": \"Image\",   "
                                     "    \"src\": \"/example.png\" },"
                                     "  \"horizontal\": \"center\",   "
                                     "  \"vertical\": \"center\"      "
                                     "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get AlignComponent.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonAlignStr.c_str());
    size_t dslLen = jsonAlignStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    RefPtr<AlignComponent> alignComponent = AceType::DynamicCast<AlignComponent>(component);

    /**
     * @tc.steps: step3. Verify whether the alignment attribute of AlignComponent is Alignment(0, 0).
     * @tc.expected: step3. The alignment attribute of AlignComponent is Alignment(0, 0).
     */
    ASSERT_TRUE(alignComponent != nullptr);
    ASSERT_TRUE(NearEqual(alignComponent->GetAlignment().GetHorizontal(), DEFAULT_CENTER));
    ASSERT_TRUE(NearEqual(alignComponent->GetAlignment().GetVertical(), DEFAULT_CENTER));
}

/**
 * @tc.name: AlignCreator003
 * @tc.desc: Verify that AlignCreator can parse json when horizontal is "right" and vertical is "bottom".
 * @tc.type: FUNC
 */
HWTEST_F(AlignCreatorTest, AlignCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of align in which horizontal is "right" and vertical is "bottom".
     */
    const std::string jsonAlignStr = ""
                                     "{                               "
                                     "  \"className\": \"Align\",     "
                                     "  \"child\": {                  "
                                     "    \"className\": \"Image\",   "
                                     "    \"src\": \"/example.png\" },"
                                     "  \"horizontal\": \"right\",    "
                                     "  \"vertical\": \"bottom\"      "
                                     "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get AlignComponent.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonAlignStr.c_str());
    size_t dslLen = jsonAlignStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    RefPtr<AlignComponent> alignComponent = AceType::DynamicCast<AlignComponent>(component);

    /**
     * @tc.steps: step3. Verify whether the alignment attribute of AlignComponent is Alignment(1, 1).
     * @tc.expected: step3. The alignment attribute of AlignComponent is Alignment(1, 1).
     */
    ASSERT_TRUE(alignComponent != nullptr);
    ASSERT_TRUE(NearEqual(alignComponent->GetAlignment().GetHorizontal(), RIGHT));
    ASSERT_TRUE(NearEqual(alignComponent->GetAlignment().GetVertical(), BOTTOM));
}

/**
 * @tc.name: AlignCreator004
 * @tc.desc: Verify that AlignCreator can handle the situation when horizontal or vertical is missing .
 * @tc.type: FUNC
 */
HWTEST_F(AlignCreatorTest, AlignCreator004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of align without horizontal and vertical.
     */
    const std::string jsonAlignStr = ""
                                     "{                              "
                                     "  \"className\": \"Align\",    "
                                     "  \"child\": {                 "
                                     "    \"className\": \"Image\",  "
                                     "    \"src\": \"/example.png\" }"
                                     "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get AlignComponent.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonAlignStr.c_str());
    size_t dslLen = jsonAlignStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    RefPtr<AlignComponent> alignComponent = AceType::DynamicCast<AlignComponent>(component);

    /**
     * @tc.steps: step3. Verify whether the alignment attribute of AlignComponent is Alignment(0, 0).
     * @tc.expected: step3. The alignment attribute of AlignComponent is Alignment(0, 0).
     */
    ASSERT_TRUE(alignComponent != nullptr);
    ASSERT_TRUE(NearEqual(alignComponent->GetAlignment().GetHorizontal(), DEFAULT_CENTER));
    ASSERT_TRUE(NearEqual(alignComponent->GetAlignment().GetVertical(), DEFAULT_CENTER));
}

} // namespace OHOS::Ace