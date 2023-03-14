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

#include "core/components/theme/theme_constants_defines.h"
#include "core/components/theme/theme_manager.h"
#include "core/components/theme/theme_utils.h"
#include "frameworks/bridge/common/utils/utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const std::string DIMENSION_ID_NORMAL = "\"@id500\"";
const std::string DIMENSION_ID_NOT_FOUND = "\"@id9999\"";
const std::string DIMENSION_ID_ERROR[] = { "", "\"@id\"", "\"@idabc\"", "\"@idid003\"", "\"@003\"",
    "\"@id001@id003\"" };
const std::string OHOS_THEME_ID_NORMAL = "@ohos_id_500";

} // namespace

class JsUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void JsUtilsTest::SetUpTestCase() {}
void JsUtilsTest::TearDownTestCase() {}
void JsUtilsTest::SetUp() {}
void JsUtilsTest::TearDown() {}

/**
 * @tc.name: ParseDimensionId001
 * @tc.desc: Test parse string contains platform style id reference.
 * @tc.type: FUNC
 */
HWTEST_F(JsUtilsTest, ParseDimensionId001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Parse dimension string contains platform id reference.
     * @tc.expected: step1. Dimension parse correctly, equal with value in ThemeConstants.
     */
    RefPtr<ThemeManager> themeManager = AceType::MakeRefPtr<ThemeManager>();
    auto themeConstants = themeManager->GetThemeConstants();
    ASSERT_NE(themeConstants, nullptr);
    const auto& parseResult = ThemeUtils::ParseThemeIdReference(DIMENSION_ID_NORMAL);
    Dimension dimension = themeConstants->GetDimension(parseResult.id);
    ASSERT_EQ(dimension, themeConstants->GetDimension(THEME_OHOS_TEXT_SIZE_HEADLINE1));
}

/**
 * @tc.name: ParseDimensionId002
 * @tc.desc: Test parse string contains platform style id reference, id not found.
 * @tc.type: FUNC
 */
HWTEST_F(JsUtilsTest, ParseDimensionId002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Parse dimension string contains platform id reference, id not found.
     * @tc.expected: step1. Dimension parse get dimension 0dp.
     */
    RefPtr<ThemeManager> themeManager = AceType::MakeRefPtr<ThemeManager>();
    auto themeConstants = themeManager->GetThemeConstants();
    ASSERT_NE(themeConstants, nullptr);
    const auto& parseResult = ThemeUtils::ParseThemeIdReference(DIMENSION_ID_NOT_FOUND);
    Dimension dimension = themeConstants->GetDimension(parseResult.id);
    ASSERT_EQ(dimension, Dimension(0.0, DimensionUnit::VP));
}

/**
 * @tc.name: ParseDimensionId003
 * @tc.desc: Test parse string contains platform style id which format is error.
 * @tc.type: FUNC
 */
HWTEST_F(JsUtilsTest, ParseDimensionId003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Parse dimension string contains id which format is error.
     * @tc.expected: step1. Dimension parse result equal with 0px.
     */
    for (size_t i = 0; i < std::size(DIMENSION_ID_ERROR); i++) {
        Dimension dimension = StringToDimension(DIMENSION_ID_ERROR[i]);
        ASSERT_EQ(dimension, Dimension(0.0, DimensionUnit::PX));
    }
}

/**
 * @tc.name: ParseDimensionId004
 * @tc.desc: Test parse string contains platform style id reference, id not found.
 * @tc.type: FUNC
 */
HWTEST_F(JsUtilsTest, ParseDimensionId004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Parse dimension string contains platform id reference, id not found.
     * @tc.expected: step1. Dimension parse get dimension 0dp.
     */
    RefPtr<ThemeManager> themeManager = AceType::MakeRefPtr<ThemeManager>();
    auto themeConstants = themeManager->GetThemeConstants();
    ASSERT_NE(themeConstants, nullptr);
    const auto& parseResult = ThemeUtils::ParseThemeIdReference(OHOS_THEME_ID_NORMAL);
    uint32_t correctId = 117441012;
    ASSERT_EQ(parseResult.id, correctId);
}

} // namespace OHOS::Ace::Framework