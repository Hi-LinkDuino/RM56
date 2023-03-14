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

#include "core/components/declaration/svg/svg_fe_component_transfer_declaration.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class SvgFeComponentTransferTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

class SvgFeComponentTransferDeclarationTest : public SvgFeComponentTransferDeclaration {
public:
    SvgFeComponentTransferDeclarationTest() = default;
    ~SvgFeComponentTransferDeclarationTest() override = default;

    bool SetFeAttr(const std::string& key, const std::string& value)
    {
        return SetSpecializedValue(std::make_pair(key, value));
    }
};

/**
 * @tc.name: SvgFeComponentTransferTest001
 * @tc.desc: Verify the Update Interface of SvgFeComponentTransferDeclaration work correctly.
 * @tc.type: FUNC
 * @tc.require: AR000FQJGI
 * @tc.author: huye
 */
HWTEST_F(SvgFeComponentTransferTest, SvgFeComponentTransferTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgFeComponentTransferDeclaration and check fe common attrs.
     */
    SvgFeComponentTransferDeclarationTest declaration;
    declaration.Init();
    declaration.InitializeStyle();
    EXPECT_EQ(Dimension(0.0, DimensionUnit::PERCENT), declaration.GetX());
    EXPECT_EQ(Dimension(0.0, DimensionUnit::PERCENT), declaration.GetY());
    EXPECT_EQ(Dimension(1.0, DimensionUnit::PERCENT), declaration.GetWidth());
    EXPECT_EQ(Dimension(1.0, DimensionUnit::PERCENT), declaration.GetHeight());
    EXPECT_EQ(FeInType::PRIMITIVE, declaration.GetIn());
    EXPECT_EQ(ColorInterpolationType::LINEAR_RGB, declaration.GetColorInterpolationType());
    EXPECT_TRUE(declaration.GetResult().empty());
}

} // namespace OHOS::Ace