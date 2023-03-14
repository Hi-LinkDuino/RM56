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

#include "core/components/svg/svg_transform.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

bool CompareVectorFloat(const std::vector<float>& f1, const std::vector<float>& f2)
{
    if (f1.size() != f2.size()) {
        GTEST_LOG_(INFO) << "CompareVectorFloat size. f1:" << f1.size() << "  f2:" << f2.size();
        return false;
    }

    for (int i = 0; i < (int)f1.size(); i++) {
        if (f1[i] != f2[i]) {
            GTEST_LOG_(INFO) << "CompareVectorFloat value. f1:" << f1[i] << "  f2:" << f2[i];
            return false;
        }
    }

    return true;
}

class TransformParseTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: SvgTransformParse001
 * @tc.desc: Parse single param of tranform.
 * @tc.type: FUNC
 * @tc.require: AR000FL0TT
 * @tc.author: huye
 */
HWTEST_F(TransformParseTest, SvgTransformParse001, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Parse single param of tranform.
     * @tc.expected: step1. Result is in anticipation.
     */
    ASSERT_EQ(SvgTransform::CreateMatrix4("rotate(-10, 50, 100)"), Matrix4::CreateRotate(-10.0f, 0.0f, 0.0f, 1.0f));
    ASSERT_EQ(SvgTransform::CreateMatrix4("translate(10)"), Matrix4::CreateTranslate(10.0f, 0.0f, 0.0f));
    ASSERT_EQ(SvgTransform::CreateMatrix4("translate(0, 9)"), Matrix4::CreateTranslate(0.0f, 9.0f, 0.0f));
    ASSERT_EQ(SvgTransform::CreateMatrix4("translate(-36, 45.5)"), Matrix4::CreateTranslate(-36.0f, 45.5f, 0.0f));
    ASSERT_EQ(SvgTransform::CreateMatrix4("skewX(40)"), Matrix4::CreateSkew(40.0f, 0.0f));
    ASSERT_EQ(SvgTransform::CreateMatrix4("skewY(50)"), Matrix4::CreateSkew(0.0f, 50.0f));
    ASSERT_EQ(SvgTransform::CreateMatrix4("scale(1, 0.5)"), Matrix4::CreateScale(1.0f, 0.5f, 1.0f));
    ASSERT_EQ(SvgTransform::CreateMatrix4("matrix(1, 2, 3, 4, 5, 6)"),
        Matrix4::CreateMatrix2D(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f));
}

/**
 * @tc.name: SvgTransformParse002
 * @tc.desc: Parse translate param of tranform.
 * @tc.type: FUNC
 * @tc.require: AR000FL0TT
 * @tc.author: huye
 */
HWTEST_F(TransformParseTest, SvgTransformParse002, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Parse x and y translate.
     * @tc.expected: step1. Result is in anticipation.
     */
    auto mat = SvgTransform::CreateMatrix4("translate(10, 9)");
    ASSERT_EQ(mat, Matrix4::CreateTranslate(10.0f, 0.0f, 0.0f) * Matrix4::CreateTranslate(0.0f, 9.0f, 0.0f));

    mat = SvgTransform::CreateMatrix4("translate(30)");
    ASSERT_EQ(mat, Matrix4::CreateTranslate(10.0f, 0.0f, 0.0f) * Matrix4::CreateTranslate(20.0f, 0.0f, 0.0f));
}

/**
 * @tc.name: SvgTransformParse003
 * @tc.desc: Parse skew param of tranform.
 * @tc.type: FUNC
 * @tc.require: AR000FL0TT
 * @tc.author: huye
 */
HWTEST_F(TransformParseTest, SvgTransformParse003, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Parse x and y skew.
     * @tc.expected: step1. Result is in anticipation.
     */
    auto mat = SvgTransform::CreateMatrix4("skewX(40) skewY(50)");
    ASSERT_EQ(mat, Matrix4::CreateSkew(40.0f, 0.0f) * Matrix4::CreateSkew(0.0f, 50.0f));
}

/**
 * @tc.name: SvgTransformParse004
 * @tc.desc: Parse completed param of tranform.
 * @tc.type: FUNC
 * @tc.require: AR000FL0TT
 * @tc.author: huye
 */
HWTEST_F(TransformParseTest, SvgTransformParse004, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Parse completed params of transform.
     * @tc.expected: step1. Result is in anticipation.
     */
    auto mat = SvgTransform::CreateMatrix4(
        "rotate(-10, 50, 100) translate(-36, 45.5) skewX(40) skewY(50) scale(1, 0.5) matrix(1, 2, 3, 4, 5, 6)");
    ASSERT_EQ(mat, Matrix4::CreateRotate(-10.0f, 0.0f, 0.0f, 1.0f) *
                   Matrix4::CreateTranslate(-36.0f, 45.5f, 0.0f) *
                   Matrix4::CreateSkew(40.0f, 0.0f) *
                   Matrix4::CreateSkew(0.0f, 50.0f) *
                   Matrix4::CreateScale(1.0f, 0.5f, 1.0f) *
                   Matrix4::CreateMatrix2D(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f));
}

/**
 * @tc.name: SvgTransformParse005
 * @tc.desc: Parse completed param of tranform.
 * @tc.type: FUNC
 * @tc.require: AR000FL0TT
 * @tc.author: huye
 */
HWTEST_F(TransformParseTest, SvgTransformParse005, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Parse completed params of transform.
     * @tc.expected: step1. Result is in anticipation.
     */
    auto transform = SvgTransform::CreateMap(
        "rotate(-10, 50, 100) translate(-36, 45.5) skewX(40) skewY(50) scale(1, 0.5) matrix(1, 2, 3, 4, 5, 6)");

    auto iter = transform.find("rotate");
    ASSERT_TRUE(iter != transform.end());
    if (iter != transform.end()) {
        std::vector<float> rotation = {-10.0f, 50.0f, 100.0f};
        ASSERT_TRUE(CompareVectorFloat(iter->second, rotation));
    }

    iter = transform.find("translate");
    ASSERT_TRUE(iter != transform.end());
    if (iter != transform.end()) {
        std::vector<float> translate = {-36.0f, 45.5f, 0.0f};
        ASSERT_TRUE(CompareVectorFloat(iter->second, translate));
    }

    iter = transform.find("skew");
    ASSERT_TRUE(iter != transform.end());
    if (iter != transform.end()) {
        std::vector<float> skew = {40.0f, 50.0f};
        ASSERT_TRUE(CompareVectorFloat(iter->second, skew));
    }

    iter = transform.find("matrix");
    ASSERT_TRUE(iter != transform.end());
    if (iter != transform.end()) {
        std::vector<float> matrix = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
        ASSERT_TRUE(CompareVectorFloat(iter->second, matrix));
    }

    auto transformInfo = SvgTransform::CreateMatrix4(transform);
    ASSERT_TRUE(transformInfo.hasRotateCenter);
    ASSERT_EQ(transformInfo.rotateCenter, Offset(50.0f, 100.0f));
}


/**
 * @tc.name: SvgTransformAlignment001
 * @tc.desc: Alignment attr from and to.
 * @tc.type: FUNC
 * @tc.require: AR000FL0TT
 * @tc.author: huye
 */
HWTEST_F(TransformParseTest, SvgTransformAlignment001, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Set and Alignment animateTransform type of other.
     * @tc.expected: step1. Result is in anticipation.
     */
    std::vector<float> fromEmpty = {};
    std::vector<float> toEmpty = {};
    ASSERT_FALSE(SvgTransform::AlignmentValues("other", fromEmpty, toEmpty));

    /**
     * @tc.steps: step2. Set and Alignment animateTransform type of scale.
     * @tc.expected: step2. Result is in anticipation.
     */
    std::vector<float> toScale = { 2.0 };
    std::vector<float> fromScale = {};
    ASSERT_TRUE(SvgTransform::AlignmentValues("scale", fromScale, toScale));
    ASSERT_TRUE(CompareVectorFloat((fromScale), std::vector<float>({ 0.0f, 0.0f })));
    ASSERT_TRUE(CompareVectorFloat((toScale), std::vector<float>({ 2.0f, 2.0f })));

    toScale = { 2.0 };
    fromScale = { 0.5 };
    ASSERT_TRUE(SvgTransform::AlignmentValues("scale", fromScale, toScale));
    ASSERT_TRUE(CompareVectorFloat((fromScale), std::vector<float>({ 0.5f, 0.5f })));
    ASSERT_TRUE(CompareVectorFloat((toScale), std::vector<float>({ 2.0f, 2.0f })));

    /**
     * @tc.steps: step3. Set and Alignment animateTransform type of skewX.
     * @tc.expected: step3. Result is in anticipation.
     */
    std::vector<float> fromSkewX = {};
    std::vector<float> toSkewX = { 10.0f };
    ASSERT_TRUE(SvgTransform::AlignmentValues("skewX", fromSkewX, toSkewX));
    ASSERT_TRUE(CompareVectorFloat((fromSkewX), std::vector<float>({ 0.0f })));
    ASSERT_TRUE(CompareVectorFloat((toSkewX), std::vector<float>({ 10.0f })));

    /**
     * @tc.steps: step4. Set and Alignment animateTransform type of skewY.
     * @tc.expected: step4. Result is in anticipation.
     */
    std::vector<float> fromSkewY = {};
    std::vector<float> toSkewY = { 10.0f };
    ASSERT_TRUE(SvgTransform::AlignmentValues("skewY", fromSkewY, toSkewY));
    ASSERT_TRUE(CompareVectorFloat((fromSkewY), std::vector<float>({ 0.0f })));
    ASSERT_TRUE(CompareVectorFloat((toSkewY), std::vector<float>({ 10.0f })));

    /**
     * @tc.steps: step5. Set and Alignment animateTransform type of translate.
     * @tc.expected: step5. Result is in anticipation.
     */
    std::vector<float> fromTranslate = { 10.0f };
    std::vector<float> toTranslate = { 20.0f };
    ASSERT_TRUE(SvgTransform::AlignmentValues("translate", fromTranslate, toTranslate));
    ASSERT_TRUE(CompareVectorFloat((fromTranslate), std::vector<float>({ 10.0f, 0.0f })));
    ASSERT_TRUE(CompareVectorFloat((toTranslate), std::vector<float>({ 20.0f, 0.0f })));

    fromTranslate = {};
    toTranslate = { 20.0f };
    ASSERT_TRUE(SvgTransform::AlignmentValues("translate", fromTranslate, toTranslate));
    ASSERT_TRUE(CompareVectorFloat((fromTranslate), std::vector<float>({ 0.0f, 0.0f })));
    ASSERT_TRUE(CompareVectorFloat((toTranslate), std::vector<float>({ 20.0f, 0.0f })));
}

/**
 * @tc.name: SvgTransformAlignment002
 * @tc.desc: Alignment attr values.
 * @tc.type: FUNC
 * @tc.require: AR000FL0TT
 * @tc.author: huye
 */
HWTEST_F(TransformParseTest, SvgTransformAlignment002, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Set and Alignment animateTransform type of other.
     * @tc.expected: step1. Result is in anticipation.
     */
    std::vector<float> frame = {};
    ASSERT_FALSE(SvgTransform::AlignmentFrame("other", frame));

    /**
     * @tc.steps: step2. Set and Alignment animateTransform type of scale.
     * @tc.expected: step2. Result is in anticipation.
     */
    std::vector<float> frameScale = { 2.0 };
    ASSERT_TRUE(SvgTransform::AlignmentFrame("scale", frameScale));
    ASSERT_TRUE(CompareVectorFloat((frameScale), std::vector<float>({2.0f, 2.0f})));

    frameScale = {};
    ASSERT_FALSE(SvgTransform::AlignmentFrame("scale", frameScale));

    /**
     * @tc.steps: step3. Set and Alignment animateTransform type of skewX.
     * @tc.expected: step3. Result is in anticipation.
     */
    std::vector<float> frameSkewX = {};
    ASSERT_FALSE(SvgTransform::AlignmentFrame("skewX", frameSkewX));

    frameSkewX = { 10.0f };
    ASSERT_TRUE(SvgTransform::AlignmentFrame("skewX", frameSkewX));
    ASSERT_TRUE(CompareVectorFloat((frameSkewX), std::vector<float>({ 10.0f })));

    /**
     * @tc.steps: step4. Set and Alignment animateTransform type of skewY.
     * @tc.expected: step4. Result is in anticipation.
     */
    std::vector<float> frameSkewY = {};
    ASSERT_FALSE(SvgTransform::AlignmentFrame("skewY", frameSkewY));

    frameSkewY = { 10.0f };
    ASSERT_TRUE(SvgTransform::AlignmentFrame("skewY", frameSkewY));
    ASSERT_TRUE(CompareVectorFloat((frameSkewY), std::vector<float>({ 10.0f })));

    /**
     * @tc.steps: step5. Set and Alignment animateTransform type of translate.
     * @tc.expected: step5. Result is in anticipation.
     */
    std::vector<float> frameTranslate = { 10.0f };
    ASSERT_TRUE(SvgTransform::AlignmentFrame("translate", frameTranslate));
    ASSERT_TRUE(CompareVectorFloat((frameTranslate), std::vector<float>({ 10.0f, 0.0f })));

    frameTranslate = {};
    ASSERT_FALSE(SvgTransform::AlignmentFrame("translate", frameTranslate));
}

} // namespace OHOS::Ace
