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

#include "core/components/declaration/svg/svg_fe_gaussianblur_declaration.h"
#include "core/components/svg/render_svg_fe_gaussianblur.h"
#include "core/components/svg/svg_fe_gaussianblur_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class SvgFeGaussianBlurTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

class SvgFeGaussianBlurDeclarationTest : public SvgFeGaussianBlurDeclaration {
public:
    SvgFeGaussianBlurDeclarationTest() = default;
    ~SvgFeGaussianBlurDeclarationTest() override = default;

    bool SetFeAttr(const std::string& key, const std::string& value)
    {
        return SetSpecializedValue(std::make_pair(key, value));
    }
};

class RenderSvgFeGaussianBlurTest : public RenderSvgFeGaussianBlur {
    DECLARE_ACE_TYPE(RenderSvgFeGaussianBlurTest, RenderSvgFeGaussianBlur);

public:
    bool IsStdDeviationEqual(double stdDeviation) const
    {
        return NearEqual(stdDeviation, deviationX_);
    }

    bool IsEdgeModeEqual(const FeEdgeMode& edgeMode) const
    {
        return edgeMode == edgeMode_;
    }
};

/**
 * @tc.name: SvgFeGaussianBlurTest001
 * @tc.desc: Verify the Update Interface of SvgFeDeclaration work correctly.
 * @tc.type: FUNC
 * @tc.require: AR000FQJGI
 * @tc.author: huye
 */
HWTEST_F(SvgFeGaussianBlurTest, SvgFeGaussianBlurTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgFeGaussianBlurDeclaration and check fe common attrs.
     */
    SvgFeGaussianBlurDeclarationTest declaration;
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
     * @tc.steps: step2. check fe gaussian blur special attrs.
     */
    EXPECT_EQ(0.0, declaration.GetStdDeviation());
    EXPECT_EQ(FeEdgeMode::EDGE_DUPLICATE, declaration.GetEdgeMode());
}

/**
 * @tc.name: SvgFeGaussianBlurTest002
 * @tc.desc: Verify the Update Interface of SvgFeDeclaration work correctly.
 * @tc.type: FUNC
 * @tc.require: AR000FQJGI
 * @tc.author: huye
 */
HWTEST_F(SvgFeGaussianBlurTest, SvgFeGaussianBlurTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgFeGaussianBlurDeclaration and set fe gaussian blur attr.
     */
    auto declaration = AceType::MakeRefPtr<SvgFeGaussianBlurDeclarationTest>();
    declaration->Init();
    declaration->InitializeStyle();
    auto component = AceType::MakeRefPtr<SvgFeGaussianBlurComponent>();
    auto render = AceType::MakeRefPtr<RenderSvgFeGaussianBlurTest>();
    if (!component || !render) {
        return;
    }

    EXPECT_TRUE(declaration->SetFeAttr("stdDeviation", "5"));
    EXPECT_EQ(5.0, declaration->GetStdDeviation());
    component->SetDeclaration(declaration);
    render->Update(component);
    render->IsStdDeviationEqual(5.0);

    EXPECT_TRUE(declaration->SetFeAttr("edgeMode", "none"));
    EXPECT_EQ(FeEdgeMode::EDGE_NONE, declaration->GetEdgeMode());
    component->SetDeclaration(declaration);
    render->Update(component);
    render->IsEdgeModeEqual(FeEdgeMode::EDGE_NONE);

    EXPECT_TRUE(declaration->SetFeAttr("edgeMode", "wrap"));
    EXPECT_EQ(FeEdgeMode::EDGE_WRAP, declaration->GetEdgeMode());
    component->SetDeclaration(declaration);
    render->Update(component);
    render->IsEdgeModeEqual(FeEdgeMode::EDGE_WRAP);

    EXPECT_TRUE(declaration->SetFeAttr("edgeMode", "duplicate"));
    EXPECT_EQ(FeEdgeMode::EDGE_DUPLICATE, declaration->GetEdgeMode());
    component->SetDeclaration(declaration);
    render->Update(component);
    render->IsEdgeModeEqual(FeEdgeMode::EDGE_DUPLICATE);
}

} // namespace OHOS::Ace