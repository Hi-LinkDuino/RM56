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

#include <array>

#include "gtest/gtest.h"

#include "base/utils/system_properties.h"
#include "core/components/common/properties/color.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"
#include "core/components/theme/theme_manager.h"
#include "core/components/theme/theme_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const std::string COLOR_ID_NORMAL = "\"@id003\"";
const std::string COLOR_ID_NOT_FOUND = "\"@id9999\"";
const Color COLOR_ERROR = Color(0xff000000);
const std::string COLOR_ID_ERROR[] = { "", "\"@id\"", "\"@idabc\"", "\"@idid003\"", "\"@003\"", "\"@id001@id003\"" };
const RefPtr<ThemeManager> THEME_MANAGER = AceType::MakeRefPtr<ThemeManager>();

Color ParseColor(const std::string& id)
{
    auto themeConstants = THEME_MANAGER->GetThemeConstants();
    const auto& parseResult = ThemeUtils::ParseThemeIdReference(id);
    return themeConstants->GetColor(parseResult.id);
}

} // namespace

class ColorPropertyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ColorPropertyTest::SetUpTestCase() {}
void ColorPropertyTest::TearDownTestCase() {}
void ColorPropertyTest::SetUp() {}
void ColorPropertyTest::TearDown() {}

/**
 * @tc.name: ParseColorId001
 * @tc.desc: Parse string refer to platform style id.
 * @tc.type: FUNC
 * @tc.require: AR000DVLH5
 */
HWTEST_F(ColorPropertyTest, ParseColorId001, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Parse color string contains id reference.
     * @tc.expected: step1. Color parse correctly, equal with color in ThemeConstants.
     */
    auto themeConstants = THEME_MANAGER->GetThemeConstants();
    Color color = ParseColor(COLOR_ID_NORMAL);
    Color platformColor = themeConstants->GetColor(THEME_OHOS_COLOR_BG);
    ASSERT_EQ(color.GetValue(), platformColor.GetValue());
}

/**
 * @tc.name: ParseColorId002
 * @tc.desc: Parse string refer to platform style id, id not found.
 * @tc.type: FUNC
 * @tc.require: AR000DVLH5
 */
HWTEST_F(ColorPropertyTest, ParseColorId002, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Parse color string contains id which is not exists.
     * @tc.expected: step1. Color parse correctly, equal with error color(black).
     */
    Color color = ParseColor(COLOR_ID_NOT_FOUND);
    ASSERT_EQ(color.GetValue(), COLOR_ERROR.GetValue());
}

/**
 * @tc.name: ParseColorId004
 * @tc.desc: Parse string refer to platform style id, id format is error.
 * @tc.type: FUNC
 * @tc.require: AR000DVLH5
 */
HWTEST_F(ColorPropertyTest, ParseColorId004, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Parse color string contains id which format is error.
     * @tc.expected: step1. Color parse correctly, equal with error color(black).
     */
    Color color = ParseColor(COLOR_ID_ERROR[0]);
    ASSERT_EQ(color.GetValue(), COLOR_ERROR.GetValue());
    for (size_t i = 1; i < std::size(COLOR_ID_ERROR); i++) {
        Color color = ParseColor(COLOR_ID_ERROR[i]);
        ASSERT_EQ(color.GetValue(), COLOR_ERROR.GetValue());
    }
}

} // namespace OHOS::Ace