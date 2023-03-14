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

#include "core/components/declaration/svg/svg_fe_colormatrix_declaration.h"
#include "core/components/svg/render_svg_fe_colormatrix.h"
#include "core/components/svg/svg_fe_colormatrix_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class SvgFeColorMatrixTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

class SvgFeColorMatrixDeclarationTest : public SvgFeColorMatrixDeclaration {
public:
    SvgFeColorMatrixDeclarationTest() = default;
    ~SvgFeColorMatrixDeclarationTest() override = default;

    bool SetFeAttr(const std::string& key, const std::string& value)
    {
        return SetSpecializedValue(std::make_pair(key, value));
    }
};

class RenderSvgFeColorMatrixTest : public RenderSvgFeColorMatrix {
    DECLARE_ACE_TYPE(RenderSvgFeColorMatrixTest, RenderSvgFeColorMatrix);

public:

    bool IsValueEqual(const std::string& value) const
    {
        return value == values_;
    }

    bool IsTypeEqual(const std::string& type) const
    {
        return type == type_;
    }

    bool IsMatrixEqual(const float matrix[], int arrayLen) const
    {
        int size = sizeof(matrix_) / sizeof(float);
        if (arrayLen != size) {
            return false;
        }
        for (int i = 0; i < size; i++) {
            if (!NearEqual(matrix_[i], matrix[i]) && !NearEqual(matrix_[i], matrix[i] * 255)) {
                return false;
            }
        }
        return true;
    }
};

/**
 * @tc.name: SvgFeColorMatrixTest001
 * @tc.desc: Verify the Update Interface of SvgFeDeclaration work correctly.
 * @tc.type: FUNC
 * @tc.require: AR000FQJGI
 * @tc.author: huye
 */
HWTEST_F(SvgFeColorMatrixTest, SvgFeColorMatrixTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgFeColorMatrixDeclaration and init.
     */
    SvgFeColorMatrixDeclarationTest declaration;
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
     * @tc.steps: step2. check fe color matrix special attrs.
     */
    EXPECT_EQ(declaration.GetIn(), FeInType::PRIMITIVE);
    EXPECT_EQ(declaration.GetColorInterpolationType(), ColorInterpolationType::LINEAR_RGB);
}

/**
 * @tc.name: SvgFeColorMatrixTest002
 * @tc.desc: Verify the Update Interface of SvgFeDeclaration work correctly.
 * @tc.type: FUNC
 * @tc.require: AR000FQJGI
 * @tc.author: huye
 */
HWTEST_F(SvgFeColorMatrixTest, SvgFeColorMatrixTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgFeColorMatrixDeclaration and set fe common params.
     */
    SvgFeColorMatrixDeclarationTest declaration;
    declaration.Init();
    declaration.InitializeStyle();

    EXPECT_TRUE(declaration.SetFeAttr("x", "1%"));
    EXPECT_EQ(Dimension(0.01, DimensionUnit::PERCENT), declaration.GetX());
    EXPECT_TRUE(declaration.SetFeAttr("x", "20"));
    EXPECT_EQ(Dimension(20), declaration.GetX());

    EXPECT_TRUE(declaration.SetFeAttr("y", "3%"));
    EXPECT_EQ(Dimension(0.03, DimensionUnit::PERCENT), declaration.GetY());
    EXPECT_TRUE(declaration.SetFeAttr("y", "40"));
    EXPECT_EQ(Dimension(40), declaration.GetY());

    EXPECT_TRUE(declaration.SetFeAttr("width", "98"));
    EXPECT_EQ(Dimension(98), declaration.GetWidth());
    EXPECT_TRUE(declaration.SetFeAttr("width", "99%"));
    EXPECT_EQ(Dimension(0.99, DimensionUnit::PERCENT), declaration.GetWidth());

    EXPECT_TRUE(declaration.SetFeAttr("height", "96"));
    EXPECT_EQ(Dimension(96), declaration.GetHeight());
    EXPECT_TRUE(declaration.SetFeAttr("height", "97%"));
    EXPECT_EQ(Dimension(0.97, DimensionUnit::PERCENT), declaration.GetHeight());

    EXPECT_TRUE(declaration.SetFeAttr("result", "123456"));
    EXPECT_EQ("123456", declaration.GetResult());

    EXPECT_TRUE(declaration.SetFeAttr("in", "SourceGraphic"));
    EXPECT_EQ(declaration.GetIn(), FeInType::SOURCE_GRAPHIC);
    EXPECT_TRUE(declaration.SetFeAttr("in", "SourceAlpha"));
    EXPECT_EQ(declaration.GetIn(), FeInType::SOURCE_ALPHA);
    EXPECT_TRUE(declaration.SetFeAttr("in", "BackgroundImage"));
    EXPECT_EQ(declaration.GetIn(), FeInType::BACKGROUND_IMAGE);
    EXPECT_TRUE(declaration.SetFeAttr("in", "BackgroundAlpha"));
    EXPECT_EQ(declaration.GetIn(), FeInType::BACKGROUND_ALPHA);
    EXPECT_TRUE(declaration.SetFeAttr("in", "FillPaint"));
    EXPECT_EQ(declaration.GetIn(), FeInType::FILL_PAINT);
    EXPECT_TRUE(declaration.SetFeAttr("in", "StrokePaint"));
    EXPECT_EQ(declaration.GetIn(), FeInType::STROKE_PAINT);

    EXPECT_TRUE(declaration.SetFeAttr("color-interpolation-filters", "sRGB"));
    EXPECT_EQ(declaration.GetColorInterpolationType(), ColorInterpolationType::SRGB);
    EXPECT_TRUE(declaration.SetFeAttr("color-interpolation-filters", "linearRGB"));
    EXPECT_EQ(declaration.GetColorInterpolationType(), ColorInterpolationType::LINEAR_RGB);
    EXPECT_TRUE(declaration.SetFeAttr("color-interpolation-filters", "auto"));
    EXPECT_EQ(declaration.GetColorInterpolationType(), ColorInterpolationType::AUTO);
}

/**
 * @tc.name: SvgFeColorMatrixTest003
 * @tc.desc: Verify the Update Interface of SvgFeDeclaration work correctly.
 * @tc.type: FUNC
 * @tc.require: AR000FQJGI
 * @tc.author: huye
 */
HWTEST_F(SvgFeColorMatrixTest, SvgFeColorMatrixTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgFeColorMatrixDeclaration and set color matrix params.
     */
    auto declaration = AceType::MakeRefPtr<SvgFeColorMatrixDeclarationTest>();
    declaration->Init();
    declaration->InitializeStyle();

    const char* type = "matrix";
    const char* value1 = "0, 0, 0, 0, 0.521972, 0, 0, 0, 0, 0.521972, 0, 0, 0, 0, 0.521972, 0, 0, 0, 1.0, 0";
    const char* value2 = "0, 0, 0, 0, 0.452367, 0, 0, 0, 0, 0.452367, 0, 0, 0, 0, 0.452367, 0, 0, 0, 1.0, 0";
    const char* value3 = "0 0 0 0 0 1 1 1 1 0 0 0 0 0 0 0 0 0 1 0";
    const float matrix1[20] = { 0, 0, 0, 0, 0.521972, 0, 0, 0, 0, 0.521972, 0, 0, 0, 0, 0.521972, 0, 0, 0, 1.0, 0 };
    const float matrix2[20] = { 0, 0, 0, 0, 0.452367, 0, 0, 0, 0, 0.452367, 0, 0, 0, 0, 0.452367, 0, 0, 0, 1.0, 0 };
    const float matrix3[20] = { 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 };

    auto component = AceType::MakeRefPtr<SvgFeColorMatrixComponent>();
    auto render = AceType::MakeRefPtr<RenderSvgFeColorMatrixTest>();
    if (!component || !render) {
        return;
    }

    EXPECT_TRUE(declaration->SetFeAttr("type", type));
    EXPECT_TRUE(declaration->SetFeAttr("values", value1));
    component->SetDeclaration(declaration);
    render->Update(component);
    EXPECT_TRUE(render->IsTypeEqual(type));
    EXPECT_TRUE(render->IsValueEqual(value1));
    EXPECT_TRUE(render->IsMatrixEqual(matrix1, sizeof(matrix1) / sizeof(float)));

    EXPECT_TRUE(declaration->SetFeAttr("type", type));
    EXPECT_TRUE(declaration->SetFeAttr("values", value2));
    component->SetDeclaration(declaration);
    render->Update(component);
    EXPECT_TRUE(render->IsTypeEqual(type));
    EXPECT_TRUE(render->IsValueEqual(value2));
    EXPECT_TRUE(render->IsMatrixEqual(matrix2, sizeof(matrix2) / sizeof(float)));

    EXPECT_TRUE(declaration->SetFeAttr("type", type));
    EXPECT_TRUE(declaration->SetFeAttr("values", value3));
    component->SetDeclaration(declaration);
    render->Update(component);
    EXPECT_TRUE(render->IsTypeEqual(type));
    EXPECT_TRUE(render->IsValueEqual(value3));
    EXPECT_TRUE(render->IsMatrixEqual(matrix3, sizeof(matrix3) / sizeof(float)));

    /**
     * @tc.steps: step2. construct SvgFeColorMatrixDeclaration and set not equal color matrix params.
     */
    EXPECT_TRUE(declaration->SetFeAttr("type", type));
    EXPECT_TRUE(declaration->SetFeAttr("values", value1));
    component->SetDeclaration(declaration);
    render->Update(component);
    EXPECT_TRUE(render->IsTypeEqual(type));
    EXPECT_FALSE(render->IsValueEqual(value3));
    EXPECT_FALSE(render->IsMatrixEqual(matrix3, sizeof(matrix3) / sizeof(float)));
}

} // namespace OHOS::Ace