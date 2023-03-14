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

#include "base/log/log.h"
#include "base/utils/utils.h"

#include "core/components/declaration/svg/svg_fe_flood_declaration.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class SvgFeFloodTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

class SvgFeFloodDeclarationTest : public SvgFeFloodDeclaration {
public:
    SvgFeFloodDeclarationTest() = default;
    ~SvgFeFloodDeclarationTest() override = default;

    bool SetFeAttr(const std::string& key, const std::string& value)
    {
        return SetSpecializedValue(std::make_pair(key, value));
    }
};

/**
 * @tc.name: SvgFeFloodTest001
 * @tc.desc: Verify the Update Interface of SvgFeDeclaration work correctly.
 * @tc.type: FUNC
 * @tc.require: AR000FQJGI
 * @tc.author: huye
 */
HWTEST_F(SvgFeFloodTest, SvgFeFloodTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgFeFloodDeclaration and check fe common attrs.
     */
    SvgFeFloodDeclarationTest declaration;
    declaration.Init();
    declaration.InitializeStyle();
    EXPECT_EQ(Dimension(0.0, DimensionUnit::PERCENT), declaration.GetX());
    EXPECT_EQ(Dimension(0.0, DimensionUnit::PERCENT), declaration.GetY());
    EXPECT_EQ(Dimension(1.0, DimensionUnit::PERCENT), declaration.GetWidth());
    EXPECT_EQ(Dimension(1.0, DimensionUnit::PERCENT), declaration.GetHeight());
    EXPECT_EQ(FeInType::PRIMITIVE, declaration.GetIn());
    EXPECT_EQ(ColorInterpolationType::LINEAR_RGB, declaration.GetColorInterpolationType());
    EXPECT_TRUE(declaration.GetResult().empty());

    /**
     * @tc.steps: step2. check fe flood special attrs.
     */
    EXPECT_EQ(Color::BLACK, declaration.GetFloodColor());
    EXPECT_EQ(1.0, declaration.GetFloodOpacity());
}

/**
 * @tc.name: SvgFeFloodTest002
 * @tc.desc: Verify the Update Interface of SvgFeDeclaration work correctly.
 * @tc.type: FUNC
 * @tc.require: AR000FQJGI
 * @tc.author: huye
 */
HWTEST_F(SvgFeFloodTest, SvgFeFloodTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgFeFloodDeclaration and set fe flood attr.
     */
    SvgFeFloodDeclarationTest declaration;
    declaration.Init();
    declaration.InitializeStyle();

    EXPECT_TRUE(declaration.SetFeAttr("flood-color", "red"));
    EXPECT_EQ(Color::RED, declaration.GetFloodColor());
    EXPECT_TRUE(declaration.SetFeAttr("flood-opacity", "0.5"));
    EXPECT_EQ(0.5, declaration.GetFloodOpacity());
}

} // namespace OHOS::Ace