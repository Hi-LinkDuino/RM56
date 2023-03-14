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

#include "core/components/declaration/svg/svg_fe_func_declaration.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class SvgFeFuncTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

class SvgFeFuncDeclarationTest : public SvgFeFuncDeclaration {
public:
    SvgFeFuncDeclarationTest() = default;
    ~SvgFeFuncDeclarationTest() override = default;

    bool SetFeAttr(const std::string& key, const std::string& value)
    {
        return SetSpecializedValue(std::make_pair(key, value));
    }
};

/**
 * @tc.name: SvgFeFuncTest001
 * @tc.desc: Verify the Update Interface of SvgFeFuncDeclaration work correctly.
 * @tc.type: FUNC
 * @tc.require: AR000FQJGI
 * @tc.author: huye
 */
HWTEST_F(SvgFeFuncTest, SvgFeFuncTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgFeFuncDeclaration and check fe common attrs.
     */
    SvgFeFuncDeclarationTest declaration;
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
     * @tc.steps: step2. check fe func special attrs.
     */
    EXPECT_EQ(FeFuncType::IDENTITY, declaration.GetType());
    EXPECT_TRUE(declaration.GetTableValues().empty());
    EXPECT_EQ(1.0, declaration.GetAmplitude());
    EXPECT_EQ(1.0, declaration.GetExponent());
    EXPECT_EQ(0.0, declaration.GetIntercept());
    EXPECT_EQ(0.0, declaration.GetOffset());
    EXPECT_EQ(0.0, declaration.GetSlope());
}

/**
 * @tc.name: SvgFeFuncTest002
 * @tc.desc: Verify the Update Interface of SvgFeFuncDeclaration work correctly.
 * @tc.type: FUNC
 * @tc.require: AR000FQJGI
 * @tc.author: huye
 */
HWTEST_F(SvgFeFuncTest, SvgFeFuncTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgFeFuncDeclaration and set fe func attr.
     */
    auto declaration = AceType::MakeRefPtr<SvgFeFuncDeclarationTest>();
    declaration->Init();
    declaration->InitializeStyle();

    EXPECT_TRUE(declaration->SetFeAttr("type", "discrete"));
    EXPECT_EQ(FeFuncType::DISCRETE, declaration->GetType());
    EXPECT_TRUE(declaration->SetFeAttr("type", "gamma"));
    EXPECT_EQ(FeFuncType::GAMMA, declaration->GetType());
    EXPECT_TRUE(declaration->SetFeAttr("type", "identity"));
    EXPECT_EQ(FeFuncType::IDENTITY, declaration->GetType());
    EXPECT_TRUE(declaration->SetFeAttr("type", "linear"));
    EXPECT_EQ(FeFuncType::LINEAR, declaration->GetType());
    EXPECT_TRUE(declaration->SetFeAttr("type", "table"));
    EXPECT_EQ(FeFuncType::TABLE, declaration->GetType());

    EXPECT_TRUE(declaration->SetFeAttr("tableValues", "1 0 1 1 0"));
    EXPECT_TRUE(declaration->GetTableValues().size() == 5);

    EXPECT_TRUE(declaration->SetFeAttr("amplitude", "5.0"));
    EXPECT_EQ(5.0, declaration->GetAmplitude());

    EXPECT_TRUE(declaration->SetFeAttr("exponent", "5.0"));
    EXPECT_EQ(5.0, declaration->GetExponent());

    EXPECT_TRUE(declaration->SetFeAttr("intercept", "5.0"));
    EXPECT_EQ(5.0, declaration->GetIntercept());

    EXPECT_TRUE(declaration->SetFeAttr("offset", "5.0"));
    EXPECT_EQ(5.0, declaration->GetOffset());

    EXPECT_TRUE(declaration->SetFeAttr("slope", "5.0"));
    EXPECT_EQ(5.0, declaration->GetSlope());
}

} // namespace OHOS::Ace