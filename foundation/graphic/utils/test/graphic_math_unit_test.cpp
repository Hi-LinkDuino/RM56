/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "gfx_utils/graphic_math.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;

namespace OHOS {
namespace {
    const Point POINT1 = { 5, 4 };
    const Point POINT2 = { 3, 2 };
    const uint16_t MAX_NUM_8BIT = 256;
    const uint32_t MAX_NUM_16BIT = 32768;
}
class MathTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.name: MathSin_001
 * @tc.desc: Verify Sin function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, MathSin_001, TestSize.Level0)
{
    EXPECT_EQ(Sin(0), 0);
    EXPECT_EQ(Sin(QUARTER_IN_DEGREE), 1);
    EXPECT_EQ(Sin(THREE_QUARTER_IN_DEGREE), -1);
}

/**
 * @tc.name: MathFastAtan2_001
 * @tc.desc: Verify FastAtan2 function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, MathFastAtan2_001, TestSize.Level0)
{
    EXPECT_EQ(FastAtan2(0, 1), 0);
    EXPECT_EQ(FastAtan2(1, 0), QUARTER_IN_DEGREE);
    EXPECT_EQ(FastAtan2(0, -1), SEMICIRCLE_IN_DEGREE);
    EXPECT_EQ(FastAtan2(-1, 0), THREE_QUARTER_IN_DEGREE);
}


/**
 * @tc.name: MathFloatToInt64_001
 * @tc.desc: Verify FloatToInt64 function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, MathFloatToInt64_001, TestSize.Level0)
{
    EXPECT_EQ(FloatToInt64(1), MAX_NUM_8BIT);
}

/**
 * @tc.name: MathSqrt_001
 * @tc.desc: Verify Sqrt function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, MathSqrt_001, TestSize.Level0)
{
    const float testInteger = 2.0;
    const float testIntegerSquared = testInteger * testInteger;
    const float testFloat = 2.121320; // 2.121320: 4.5 squaring results
    const float testFloatSquared = 4.5;
    const float accuracy = 0.000001;

    EXPECT_EQ(Sqrt(0), 0);
    float ret = Sqrt(testIntegerSquared);
    if (ret > testInteger - accuracy && ret < testInteger + accuracy) {
        EXPECT_EQ(0, 0);
    } else {
        EXPECT_NE(0, 0);
    }

    ret = Sqrt(testFloatSquared);
    if (ret > testFloat - accuracy && ret < testFloat + accuracy) {
        EXPECT_EQ(0, 0);
    } else {
        EXPECT_NE(0, 0);
    }
}

/**
 * @tc.name: Vector2Dot_001
 * @tc.desc: Verify Dot function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Vector2Dot_001, TestSize.Level0)
{
    Vector2<uint16_t>* vector1 = new Vector2<uint16_t>(POINT1.x, POINT1.y);
    if (vector1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<uint16_t>* vector2 = new Vector2<uint16_t>(POINT2.x, POINT2.y);
    if (vector2 == nullptr) {
        delete vector1;
        EXPECT_EQ(1, 0);
        return;
    }
    uint16_t value = POINT1.x * POINT2.x + POINT1.y * POINT2.y;

    EXPECT_EQ(vector1->Dot(*vector2), value);

    delete vector1;
    delete vector2;
}

/**
 * @tc.name: Vector2Cross_001
 * @tc.desc: Verify Cross function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Vector2Cross_001, TestSize.Level0)
{
    Vector2<uint16_t>* vector1 = new Vector2<uint16_t>(POINT1.x, POINT1.y);
    if (vector1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<uint16_t>* vector2 = new Vector2<uint16_t>(POINT2.x, POINT2.y);
    if (vector2 == nullptr) {
        delete vector1;
        EXPECT_EQ(1, 0);
        return;
    }
    uint16_t value = POINT1.x * POINT2.y - POINT1.y * POINT2.x;

    EXPECT_EQ(vector1->Cross(*vector2), value);

    delete vector1;
    delete vector2;
}

/**
 * @tc.name: Vector2Operator_001
 * @tc.desc: Verify negative operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Vector2Operator_001, TestSize.Level0)
{
    Vector2<uint16_t>* vector1 = new Vector2<uint16_t>(POINT1.x, POINT1.y);
    if (vector1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<uint16_t> vector3 = vector1->operator-();

    EXPECT_EQ(vector3.x_, 65531); // 65531: -POINT1.x
    EXPECT_EQ(vector3.y_, 65532); // 65532: -POINT1.y
    delete vector1;
}

/**
 * @tc.name: Vector2Operator_002
 * @tc.desc: Verify minus operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Vector2Operator_002, TestSize.Level0)
{
    Vector2<uint16_t>* vector1 = new Vector2<uint16_t>(POINT1.x, POINT1.y);
    if (vector1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<uint16_t>* vector2 = new Vector2<uint16_t>(POINT2.x, POINT2.y);
    if (vector2 == nullptr) {
        delete vector1;
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<uint16_t> vector3 = vector1->operator-(*vector2);

    EXPECT_EQ(vector3.x_, POINT1.x - POINT2.x);
    EXPECT_EQ(vector3.y_, POINT1.y - POINT2.y);
    delete vector1;
    delete vector2;
}

/**
 * @tc.name: Vector2Operator_003
 * @tc.desc: Verify addition operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Vector2Operator_003, TestSize.Level0)
{
    Vector2<uint16_t>* vector1 = new Vector2<uint16_t>(POINT1.x, POINT1.y);
    if (vector1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<uint16_t>* vector2 = new Vector2<uint16_t>(POINT2.x, POINT2.y);
    if (vector2 == nullptr) {
        delete vector1;
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<uint16_t> vector3 = vector1->operator+(*vector2);

    EXPECT_EQ(vector3.x_, POINT1.x + POINT2.x);
    EXPECT_EQ(vector3.y_, POINT1.y + POINT2.y);
    delete vector1;
    delete vector2;
}

/**
 * @tc.name: Vector2Operator_004
 * @tc.desc: Verify multiplication operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Vector2Operator_004, TestSize.Level0)
{
    Vector2<uint16_t>* vector1 = new Vector2<uint16_t>(POINT1.x, POINT1.y);
    if (vector1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<uint16_t> vector3 = vector1->operator*(2);
    EXPECT_EQ(vector3.x_, POINT1.x * 2);
    EXPECT_EQ(vector3.y_, POINT1.y * 2);
    delete vector1;
}

/**
 * @tc.name: Vector2Operator_005
 * @tc.desc: Verify equal operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Vector2Operator_005, TestSize.Level0)
{
    Vector2<uint16_t>* vector1 = new Vector2<uint16_t>(POINT1.x, POINT1.y);
    if (vector1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<uint16_t>* vector2 = new Vector2<uint16_t>(POINT2.x, POINT2.y);
    if (vector2 == nullptr) {
        delete vector1;
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<uint16_t>* vector3 = new Vector2<uint16_t>(POINT1.x, POINT1.y);
    if (vector3 == nullptr) {
        delete vector1;
        delete vector2;
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(vector1->operator==(*vector2), false);
    EXPECT_EQ(vector1->operator==(*vector3), true);

    delete vector1;
    delete vector2;
    delete vector3;
}

/**
 * @tc.name: Vector2Operator_006
 * @tc.desc: Verify assignment operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Vector2Operator_006, TestSize.Level0)
{
    Vector2<uint16_t>* vector2 = new Vector2<uint16_t>(POINT2.x, POINT2.y);
    if (vector2 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<uint16_t>* vector3 = new Vector2<uint16_t>(POINT1.x, POINT1.y);
    if (vector3 == nullptr) {
        delete vector2;
        EXPECT_EQ(1, 0);
        return;
    }
    vector3->operator=(*vector2);
    EXPECT_EQ(vector3->x_, POINT2.x);
    EXPECT_EQ(vector3->y_, POINT2.y);

    delete vector2;
    delete vector3;
}

/**
 * @tc.name: Vector2Operator_007
 * @tc.desc: Verify plus equal operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Vector2Operator_007, TestSize.Level0)
{
    Vector2<uint16_t>* vector2 = new Vector2<uint16_t>(POINT2.x, POINT2.y);
    if (vector2 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<uint16_t>* vector3 = new Vector2<uint16_t>(POINT1.x, POINT1.y);
    if (vector3 == nullptr) {
        delete vector2;
        EXPECT_EQ(1, 0);
        return;
    }

    vector3->operator+=(*vector2);
    EXPECT_EQ(vector3->x_, POINT1.x + POINT2.x);
    EXPECT_EQ(vector3->y_, POINT1.y + POINT2.y);

    delete vector2;
    delete vector3;
}

/**
 * @tc.name: Vector2Operator_008
 * @tc.desc: Verify minus equal operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Vector2Operator_008, TestSize.Level0)
{
    Vector2<uint16_t>* vector2 = new Vector2<uint16_t>(POINT2.x, POINT2.y);
    if (vector2 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<uint16_t>* vector3 = new Vector2<uint16_t>(POINT1.x, POINT1.y);
    if (vector3 == nullptr) {
        delete vector2;
        EXPECT_EQ(1, 0);
        return;
    }

    vector3->operator-=(*vector2);
    EXPECT_EQ(vector3->x_, POINT1.x - POINT2.x);
    EXPECT_EQ(vector3->y_, POINT1.y - POINT2.y);

    delete vector2;
    delete vector3;
}

/**
 * @tc.name: Vector3Operator_001
 * @tc.desc: Verify index operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Vector3Operator_001, TestSize.Level0)
{
    const uint16_t posX = 3;
    const uint16_t posY = 5;
    const uint16_t posZ = 7;
    Vector3<uint16_t>* vector1 = new Vector3<uint16_t>(posX, posY, posZ);
    if (vector1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    uint16_t i = 0;
    EXPECT_EQ(vector1->operator[](i++), posX);
    EXPECT_EQ(vector1->operator[](i++), posY);
    EXPECT_EQ(vector1->operator[](i++), posZ);

    delete vector1;
}

/**
 * @tc.name: Vector3Operator_002
 * @tc.desc: Verify equal operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Vector3Operator_002, TestSize.Level0)
{
    const uint16_t posX = 3;
    const uint16_t posY = 5;
    const uint16_t posZ = 7;
    Vector3<uint16_t>* vector1 = new Vector3<uint16_t>(posX, posY, posZ);
    if (vector1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Vector3<uint16_t>* vector2 = new Vector3<uint16_t>(posX, posY, posZ);
    if (vector2 == nullptr) {
        delete vector1;
        EXPECT_EQ(1, 0);
        return;
    }
    Vector3<uint16_t>* vector3 = new Vector3<uint16_t>(posX - 1, posY, posZ);
    if (vector3 == nullptr) {
        delete vector1;
        delete vector2;
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(vector1->operator==(*vector2), true);
    EXPECT_EQ(vector1->operator==(*vector3), false);

    delete vector1;
    delete vector2;
    delete vector3;
}

/**
 * @tc.name: Matrix3GetData_001
 * @tc.desc: Verify GetData function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Matrix3GetData_001, TestSize.Level0)
{
    Matrix3<uint16_t>* matrix = new Matrix3<uint16_t>(1, 1, 1, 1, 1, 1, 1, 1, 1);
    if (matrix == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(matrix->GetData()[0], 1);

    delete matrix;
}

/**
 * @tc.name: Matrix3Operator_001
 * @tc.desc: Verify multiplication operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Matrix3Operator_001, TestSize.Level0)
{
    Matrix3<uint16_t> matrix1(1, 1, 1, 1, 1, 1, 1, 1, 1);
    /*
     * 1: m00 Indicates the element in row 1 and column 1 of the matrix.
     * 2: m01 Indicates the element in row 1 and column 2 of the matrix.
     * 3: m02 Indicates the element in row 1 and column 3 of the matrix.
     * 4: m10 Indicates the element in row 2 and column 1 of the matrix.
     * 5: m11 Indicates the element in row 2 and column 2 of the matrix.
     * 6: m12 Indicates the element in row 2 and column 3 of the matrix.
     * 7: m20 Indicates the element in row 3 and column 1 of the matrix.
     * 8: m21 Indicates the element in row 3 and column 2 of the matrix.
     * 9: m22 Indicates the element in row 3 and column 3 of the matrix.
     */
    Matrix3<uint16_t> matrix2(1, 2, 3, 4, 5, 6, 7, 8, 9);
    Matrix3<uint16_t> matrix3 = matrix1 * matrix2;

    uint16_t i = 0;
    // 6: data_[0] * oData[0] + data_[3] * oData[1] + data_[6] * oData[2];
    EXPECT_EQ(matrix3.GetData()[i++], 6);
    // 6: data_[1] * oData[0] + data_[4] * oData[1] + data_[7] * oData[2];
    EXPECT_EQ(matrix3.GetData()[i++], 6);
    // 6: data_[2] * oData[0] + data_[5] * oData[1] + data_[8] * oData[2];
    EXPECT_EQ(matrix3.GetData()[i++], 6);
    // 15: data_[0] * oData[3] + data_[3] * oData[4] + data_[6] * oData[5];
    EXPECT_EQ(matrix3.GetData()[i++], 15);
    // 15: data_[1] * oData[3] + data_[4] * oData[4] + data_[7] * oData[5];
    EXPECT_EQ(matrix3.GetData()[i++], 15);
    // 15: data_[2] * oData[3] + data_[5] * oData[4] + data_[8] * oData[5];
    EXPECT_EQ(matrix3.GetData()[i++], 15);
    // 24: data_[0] * oData[6] + data_[3] * oData[7] + data_[6] * oData[8];
    EXPECT_EQ(matrix3.GetData()[i++], 24);
    // 24: data_[1] * oData[6] + data_[4] * oData[7] + data_[7] * oData[8];
    EXPECT_EQ(matrix3.GetData()[i++], 24);
    // 24: data_[2] * oData[6] + data_[5] * oData[7] + data_[8] * oData[8];
    EXPECT_EQ(matrix3.GetData()[i++], 24);
}

/**
 * @tc.name: Matrix3Operator_002
 * @tc.desc: Verify multiplication operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Matrix3Operator_002, TestSize.Level0)
{
    Matrix3<uint16_t> matrix1(1, 1, 1, 1, 1, 1, 1, 1, 1);
    /*
     * 1: Indicates the X coordinate.
     * 2: Indicates the Y coordinate.
     * 3: Indicates the Z coordinate.
     */
    Vector3<uint16_t> vector1(1, 2, 3);
    Vector3<uint16_t> vector2 = matrix1 * vector1;

    // 6: data_[0] * oData[0] + data_[3] * oData[1] + data_[6] * oData[2];
    EXPECT_EQ(vector2.x_, 6);
    // 6: data_[1] * oData[0] + data_[4] * oData[1] + data_[7] * oData[2];
    EXPECT_EQ(vector2.y_, 6);
    // 6: data_[2] * oData[0] + data_[5] * oData[1] + data_[8] * oData[2];
    EXPECT_EQ(vector2.z_, 6);
}

/**
 * @tc.name: Matrix3Operator_003
 * @tc.desc: Verify index operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Matrix3Operator_003, TestSize.Level0)
{
    Matrix3<uint16_t>* matrix = new Matrix3<uint16_t>(1, 0, 0, 1, 0, 0, 1, 0, 0);
    if (matrix == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    uint16_t i = 0;
    EXPECT_EQ(*matrix->operator[](i++), 1);
    EXPECT_EQ(*matrix->operator[](i++), 1);
    EXPECT_EQ(*matrix->operator[](i++), 1);

    delete matrix;
}

/**
 * @tc.name: Matrix3Operator_004
 * @tc.desc: Verify assignment operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Matrix3Operator_004, TestSize.Level0)
{
    Matrix3<uint16_t>* matrix = new Matrix3<uint16_t>(1, 0, 0, 1, 0, 0, 1, 0, 0);
    if (matrix == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Matrix3<uint16_t>* matrix2 = new Matrix3<uint16_t>(1, 1, 1, 1, 1, 1, 1, 1, 1);
    if (matrix2 == nullptr) {
        delete matrix;
        EXPECT_EQ(1, 0);
        return;
    }
    matrix->operator=(*matrix2);
    uint16_t i = 0;
    EXPECT_EQ(matrix->GetData()[i++], 1);
    EXPECT_EQ(matrix->GetData()[i++], 1);
    EXPECT_EQ(matrix->GetData()[i++], 1);
    EXPECT_EQ(matrix->GetData()[i++], 1);
    EXPECT_EQ(matrix->GetData()[i++], 1);
    EXPECT_EQ(matrix->GetData()[i++], 1);
    EXPECT_EQ(matrix->GetData()[i++], 1);
    EXPECT_EQ(matrix->GetData()[i++], 1);
    EXPECT_EQ(matrix->GetData()[i++], 1);

    delete matrix;
    delete matrix2;
}

/**
 * @tc.name: Matrix3Determinant_001
 * @tc.desc: Verify Determinant operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Matrix3Determinant_001, TestSize.Level0)
{
    Matrix3<uint16_t>* matrix2 = new Matrix3<uint16_t>(1, 1, 1, 1, 1, 1, 1, 1, 1);
    if (matrix2 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(matrix2->Determinant(), 0);

    delete matrix2;
}

/**
 * @tc.name: Matrix3Inverse_001
 * @tc.desc: Verify Inverse operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Matrix3Inverse_001, TestSize.Level0)
{
    Matrix3<uint16_t>* matrix1 = new Matrix3<uint16_t>(1, 1, 1, 1, 1, 1, 1, 1, 1);
    if (matrix1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Matrix3<uint16_t> matrix2 = matrix1->Inverse();

    uint16_t i = 0;
    EXPECT_EQ(matrix2.GetData()[i++], 1);
    EXPECT_EQ(matrix2.GetData()[i++], 1);
    EXPECT_EQ(matrix2.GetData()[i++], 1);
    EXPECT_EQ(matrix2.GetData()[i++], 1);
    EXPECT_EQ(matrix2.GetData()[i++], 1);
    EXPECT_EQ(matrix2.GetData()[i++], 1);
    EXPECT_EQ(matrix2.GetData()[i++], 1);
    EXPECT_EQ(matrix2.GetData()[i++], 1);
    EXPECT_EQ(matrix2.GetData()[i++], 1);

    delete matrix1;
}

/**
 * @tc.name: Matrix3Operator_005
 * @tc.desc: Verify equal operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Matrix3Operator_005, TestSize.Level0)
{
    Matrix3<uint16_t>* matrix1 = new Matrix3<uint16_t>(1, 1, 1, 1, 1, 1, 1, 1, 1);
    if (matrix1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Matrix3<uint16_t>* matrix2 = new Matrix3<uint16_t>(1, 1, 1, 1, 1, 1, 1, 1, 1);
    if (matrix2 == nullptr) {
        delete matrix1;
        EXPECT_EQ(1, 0);
        return;
    }
    Matrix3<uint16_t>* matrix3 = new Matrix3<uint16_t>(1, 1, 1, 1, 1, 1, 0, 1, 1);
    if (matrix3 == nullptr) {
        delete matrix1;
        delete matrix2;
        EXPECT_EQ(1, 0);
        return;
    }

    EXPECT_EQ(matrix1->operator==(*matrix2), true);
    EXPECT_EQ(matrix1->operator==(*matrix3), false);

    delete matrix1;
    delete matrix2;
    delete matrix3;
}

/**
 * @tc.name: Matrix3Rotate_001
 * @tc.desc: Verify Rotate function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Matrix3Rotate_001, TestSize.Level0)
{
    Matrix3<uint64_t> rotate = Matrix3<uint64_t>::Rotate(0, Vector2<uint64_t>(0, 0));
    uint16_t i = 0;
    EXPECT_EQ(rotate.GetData()[i++], 1);
    EXPECT_EQ(rotate.GetData()[i++], 0);
    EXPECT_EQ(rotate.GetData()[i++], 0);
    EXPECT_EQ(rotate.GetData()[i++], 0);
    EXPECT_EQ(rotate.GetData()[i++], 1);
    EXPECT_EQ(rotate.GetData()[i++], 0);
    EXPECT_EQ(rotate.GetData()[i++], 0);
    EXPECT_EQ(rotate.GetData()[i++], 0);
    EXPECT_EQ(rotate.GetData()[i++], 1);
}

/**
 * @tc.name: Matrix3Scale_001
 * @tc.desc: Verify Scale function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Matrix3Scale_001, TestSize.Level0)
{
    Matrix3<int64_t> scale =
        Matrix3<int64_t>::Scale(Vector2<int64_t>(1, 1), Vector2<int64_t>(0, 0));
    uint16_t i = 0;
    EXPECT_EQ(scale.GetData()[i++], 1);
    EXPECT_EQ(scale.GetData()[i++], 0);
    EXPECT_EQ(scale.GetData()[i++], 0);
    EXPECT_EQ(scale.GetData()[i++], 0);
    EXPECT_EQ(scale.GetData()[i++], 1);
    EXPECT_EQ(scale.GetData()[i++], 0);
    EXPECT_EQ(scale.GetData()[i++], 0);
    EXPECT_EQ(scale.GetData()[i++], 0);
    EXPECT_EQ(scale.GetData()[i++], 1);
}

/**
 * @tc.name: Matrix3Translate_001
 * @tc.desc: Verify Translate function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(MathTest, Matrix3Translate_001, TestSize.Level0)
{
    Matrix3<int64_t> translate = Matrix3<int64_t>::Translate(Vector2<int64_t>(0, 0));
    uint16_t i = 0;
    EXPECT_EQ(translate.GetData()[i++], 1);
    EXPECT_EQ(translate.GetData()[i++], 0);
    EXPECT_EQ(translate.GetData()[i++], 0);
    EXPECT_EQ(translate.GetData()[i++], 0);
    EXPECT_EQ(translate.GetData()[i++], 1);
    EXPECT_EQ(translate.GetData()[i++], 0);
    EXPECT_EQ(translate.GetData()[i++], 0);
    EXPECT_EQ(translate.GetData()[i++], 0);
    EXPECT_EQ(translate.GetData()[i++], 1);
}
} // namespace OHOS
