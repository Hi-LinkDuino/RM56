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

#include "core/components/declaration/svg/svg_fe_offset_declaration.h"
#include "core/components/svg/render_svg_fe_offset.h"
#include "core/components/svg/svg_fe_offset_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class SvgFeOffsetTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

class SvgFeOffsetDeclarationTest : public SvgFeOffsetDeclaration {
public:
    SvgFeOffsetDeclarationTest() = default;
    ~SvgFeOffsetDeclarationTest() override = default;

    bool SetFeAttr(const std::string& key, const std::string& value)
    {
        return SetSpecializedValue(std::make_pair(key, value));
    }
};

class RenderSvgFeOffsetTest : public RenderSvgFeOffset {
    DECLARE_ACE_TYPE(RenderSvgFeOffsetTest, RenderSvgFeOffset);

public:

    bool IsDxEqual(double dx) const
    {
        return dx == dx_;
    }

    bool IsDyEqual(double dy) const
    {
        return dy == dy_;
    }
};

/**
 * @tc.name: SvgFeOffsetTest001
 * @tc.desc: Verify the Update Interface of SvgFeDeclaration work correctly.
 * @tc.type: FUNC
 * @tc.require: AR000FQJGI
 * @tc.author: huye
 */
HWTEST_F(SvgFeOffsetTest, SvgFeOffsetTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgFeOffsetDeclaration and check fe common attrs.
     */
    SvgFeOffsetDeclarationTest declaration;
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
     * @tc.steps: step2. check fe offset special attrs.
     */
    EXPECT_EQ(0.0, declaration.GetDx());
    EXPECT_EQ(0.0, declaration.GetDy());
}

/**
 * @tc.name: SvgFeOffsetTest002
 * @tc.desc: Verify the Update Interface of SvgFeDeclaration work correctly.
 * @tc.type: FUNC
 * @tc.require: AR000FQJGI
 * @tc.author: huye
 */
HWTEST_F(SvgFeOffsetTest, SvgFeOffsetTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgFeOffsetDeclaration and set fe offset attr.
     */
    auto declaration = AceType::MakeRefPtr<SvgFeOffsetDeclarationTest>();
    declaration->Init();
    declaration->InitializeStyle();
    auto component = AceType::MakeRefPtr<SvgFeOffsetComponent>();
    auto render = AceType::MakeRefPtr<RenderSvgFeOffsetTest>();
    if (!component || !render) {
        return;
    }

    EXPECT_TRUE(declaration->SetFeAttr("dx", "10"));
    EXPECT_EQ(10.0, declaration->GetDx());
    component->SetDeclaration(declaration);
    render->Update(component);
    render->IsDxEqual(10.0);

    EXPECT_TRUE(declaration->SetFeAttr("dx", "15px"));
    EXPECT_EQ(15.0, declaration->GetDx());
    component->SetDeclaration(declaration);
    render->Update(component);
    render->IsDxEqual(15.0);

    EXPECT_TRUE(declaration->SetFeAttr("dx", "50%"));
    EXPECT_EQ(0.0, declaration->GetDx());
    component->SetDeclaration(declaration);
    render->Update(component);
    render->IsDxEqual(0.0);

    EXPECT_TRUE(declaration->SetFeAttr("dy", "20"));
    EXPECT_EQ(20.0, declaration->GetDy());
    component->SetDeclaration(declaration);
    render->Update(component);
    render->IsDyEqual(20.0);

    EXPECT_TRUE(declaration->SetFeAttr("dy", "25px"));
    EXPECT_EQ(25.0, declaration->GetDy());
    component->SetDeclaration(declaration);
    render->Update(component);
    render->IsDyEqual(25.0);

    EXPECT_TRUE(declaration->SetFeAttr("dy", "60%"));
    EXPECT_EQ(0.0, declaration->GetDy());
    component->SetDeclaration(declaration);
    render->Update(component);
    render->IsDyEqual(0.0);
}

} // namespace OHOS::Ace