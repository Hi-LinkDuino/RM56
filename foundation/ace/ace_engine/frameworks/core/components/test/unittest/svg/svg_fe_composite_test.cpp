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

#include "core/components/declaration/svg/svg_fe_composite_declaration.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class SvgFeCompositeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

class SvgFeCompositeDeclarationTest : public SvgFeCompositeDeclaration {
public:
    SvgFeCompositeDeclarationTest() = default;
    ~SvgFeCompositeDeclarationTest() override = default;

    bool SetFeAttr(const std::string& key, const std::string& value)
    {
        return SetSpecializedValue(std::make_pair(key, value));
    }
};

/**
 * @tc.name: SvgFeCompositeTest001
 * @tc.desc: Verify the Update Interface of SvgFeCompositeDeclaration work correctly.
 * @tc.type: FUNC
 * @tc.require: AR000FQJGI
 * @tc.author: huye
 */
HWTEST_F(SvgFeCompositeTest, SvgFeCompositeTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgFeCompositeDeclaration and check fe common attrs.
     */
    SvgFeCompositeDeclarationTest declaration;
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
     * @tc.steps: step2. check fe composite special attrs.
     */
    EXPECT_EQ(FeInType::PRIMITIVE, declaration.GetIn2());
    EXPECT_EQ(FeOperatorType::FE_OVER, declaration.GetOperatorType());
    EXPECT_EQ(0.0, declaration.GetK1());
    EXPECT_EQ(0.0, declaration.GetK2());
    EXPECT_EQ(0.0, declaration.GetK3());
    EXPECT_EQ(0.0, declaration.GetK4());
}

/**
 * @tc.name: SvgFeCompositeTest002
 * @tc.desc: Verify the Update Interface of SvgFeDeclaration work correctly.
 * @tc.type: FUNC
 * @tc.require: AR000FQJGI
 * @tc.author: huye
 */
HWTEST_F(SvgFeCompositeTest, SvgFeCompositeTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgFeCompositeDeclaration and set fe composite attr.
     */
    auto declaration = AceType::MakeRefPtr<SvgFeCompositeDeclarationTest>();
    declaration->Init();
    declaration->InitializeStyle();

    EXPECT_TRUE(declaration->SetFeAttr("in2", "SourceGraphic"));
    EXPECT_EQ(declaration->GetIn2(), FeInType::SOURCE_GRAPHIC);
    EXPECT_TRUE(declaration->SetFeAttr("in2", "SourceAlpha"));
    EXPECT_EQ(declaration->GetIn2(), FeInType::SOURCE_ALPHA);
    EXPECT_TRUE(declaration->SetFeAttr("in2", "BackgroundImage"));
    EXPECT_EQ(declaration->GetIn2(), FeInType::BACKGROUND_IMAGE);
    EXPECT_TRUE(declaration->SetFeAttr("in2", "BackgroundAlpha"));
    EXPECT_EQ(declaration->GetIn2(), FeInType::BACKGROUND_ALPHA);
    EXPECT_TRUE(declaration->SetFeAttr("in2", "FillPaint"));
    EXPECT_EQ(declaration->GetIn2(), FeInType::FILL_PAINT);
    EXPECT_TRUE(declaration->SetFeAttr("in2", "StrokePaint"));
    EXPECT_EQ(declaration->GetIn2(), FeInType::STROKE_PAINT);

    EXPECT_TRUE(declaration->SetFeAttr("k1", "1"));
    EXPECT_EQ(1.0, declaration->GetK1());

    EXPECT_TRUE(declaration->SetFeAttr("k2", "2"));
    EXPECT_EQ(2.0, declaration->GetK2());

    EXPECT_TRUE(declaration->SetFeAttr("k3", "3"));
    EXPECT_EQ(3.0, declaration->GetK3());

    EXPECT_TRUE(declaration->SetFeAttr("k4", "4"));
    EXPECT_EQ(4.0, declaration->GetK4());
}

} // namespace OHOS::Ace