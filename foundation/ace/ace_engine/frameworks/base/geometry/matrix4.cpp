/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "base/geometry/matrix4.h"

#include <algorithm>
#include <cmath>

#include "base/utils/utils.h"

namespace OHOS::Ace {
namespace {
constexpr int32_t MATRIX_LENGTH = Matrix4::DIMENSION * Matrix4::DIMENSION;
constexpr double ANGLE_UNIT = 0.017453f; // PI / 180

inline bool IsEqual(const double& left, const double& right)
{
    return NearEqual(left, right);
}

} // namespace

Matrix4 Matrix4::CreateIdentity()
{
    return Matrix4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::CreateTranslate(double x, double y, double z)
{
    return Matrix4(1.0f, 0.0f, 0.0f, x, 0.0f, 1.0f, 0.0f, y, 0.0f, 0.0f, 1.0f, z, 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::CreateScale(double x, double y, double z)
{
    return Matrix4(x, 0.0f, 0.0f, 0.0f, 0.0f, y, 0.0f, 0.0f, 0.0f, 0.0f, z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::CreateRotate(double angle, double dx, double dy, double dz)
{
    // (x,y,z) need normalize
    double sum = dx * dx + dy * dy + dz * dz;
    if (NearZero(sum)) {
        return Matrix4::CreateIdentity();
    }

    double x = dx / sqrt(sum);
    double y = dy / sqrt(sum);
    double z = dz / sqrt(sum);
    double redian = static_cast<double>(angle * (M_PI / 180.0f));
    double cosValue = cosf(redian);
    double sinValue = sinf(redian);

    return Matrix4(cosValue + (x * x * (1.0f - cosValue)), (x * y * (1.0f - cosValue)) - (z * sinValue),
        (x * z * (1.0f - cosValue)) + (y * sinValue), 0.0f, (y * x * (1.0f - cosValue)) + (z * sinValue),
        cosValue + (y * y * (1.0f - cosValue)), (y * z * (1.0f - cosValue)) - (x * sinValue), 0.0f,
        (z * x * (1.0f - cosValue)) - (y * sinValue), (z * y * (1.0f - cosValue)) + (x * sinValue),
        cosValue + (z * z * (1.0f - cosValue)), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::CreateMatrix2D(double m00, double m10, double m01, double m11, double m03, double m13)
{
    return Matrix4(m00, m01, 0.0f, m03, m10, m11, 0.0f, m13, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::CreateSkew(double x, double y)
{
    return Matrix4(1.0f, std::tan(x * ANGLE_UNIT), 0.0f, 0.0f, std::tan(y * ANGLE_UNIT), 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::CreatePerspective(double distance)
{
    auto result = CreateIdentity();
    if (GreatNotEqual(distance, 0.0f)) {
        result.matrix4x4_[2][3] = -1.0f / distance;
    }
    return result;
}

Matrix4 Matrix4::Invert(const Matrix4& matrix)
{
    Matrix4 inverted = CreateInvert(matrix);
    double determinant = matrix(0, 0) * inverted(0, 0) + matrix(0, 1) * inverted(1, 0) + matrix(0, 2) * inverted(2, 0) +
                         matrix(0, 3) * inverted(3, 0);

    if (!NearZero(determinant)) {
        inverted = inverted * (1.0f / determinant);
    } else {
        inverted = CreateIdentity();
    }

    return inverted;
}

Matrix4::Matrix4()
    : Matrix4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f)
{}

Matrix4::Matrix4(const Matrix4& matrix)
{
    std::copy_n(&matrix.matrix4x4_[0][0], MATRIX_LENGTH, &matrix4x4_[0][0]);
}

Matrix4::Matrix4(double m00, double m01, double m02, double m03, double m10, double m11, double m12, double m13,
    double m20, double m21, double m22, double m23, double m30, double m31, double m32, double m33)
{
    matrix4x4_[0][0] = m00;
    matrix4x4_[1][0] = m01;
    matrix4x4_[2][0] = m02;
    matrix4x4_[3][0] = m03;
    matrix4x4_[0][1] = m10;
    matrix4x4_[1][1] = m11;
    matrix4x4_[2][1] = m12;
    matrix4x4_[3][1] = m13;
    matrix4x4_[0][2] = m20;
    matrix4x4_[1][2] = m21;
    matrix4x4_[2][2] = m22;
    matrix4x4_[3][2] = m23;
    matrix4x4_[0][3] = m30;
    matrix4x4_[1][3] = m31;
    matrix4x4_[2][3] = m32;
    matrix4x4_[3][3] = m33;
}

void Matrix4::SetScale(double x, double y, double z)
{
    // The 4X4 matrix scale index is [0][0], [1][1], [2][2], [3][3].
    matrix4x4_[0][0] = x;
    matrix4x4_[1][1] = y;
    matrix4x4_[2][2] = z;
    matrix4x4_[3][3] = 1.0f;
}

double Matrix4::GetScaleX() const
{
    return matrix4x4_[0][0];
}

double Matrix4::GetScaleY() const
{
    return matrix4x4_[1][1];
}

void Matrix4::SetEntry(int32_t row, int32_t col, double value)
{
    if ((row < 0 || row >= DIMENSION) || (col < 0 || col >= DIMENSION)) {
        return;
    }
    matrix4x4_[row][col] = value;
}

bool Matrix4::IsIdentityMatrix() const
{
    return *this == CreateIdentity();
}

void Matrix4::Rotate(double angle, double dx, double dy, double dz)
{
    Matrix4 transform = *this;
    *this = transform * CreateRotate(angle, dx, dy, dz);
}

int32_t Matrix4::Count() const
{
    return MATRIX_LENGTH;
}

Matrix4 Matrix4::CreateInvert(const Matrix4& matrix)
{
    return Matrix4(
        matrix(1, 1) * matrix(2, 2) * matrix(3, 3) - matrix(1, 1) * matrix(2, 3) * matrix(3, 2) -
            matrix(2, 1) * matrix(1, 2) * matrix(3, 3) + matrix(2, 1) * matrix(1, 3) * matrix(3, 2) +
            matrix(3, 1) * matrix(1, 2) * matrix(2, 3) - matrix(3, 1) * matrix(1, 3) * matrix(2, 2),
        -matrix(1, 0) * matrix(2, 2) * matrix(3, 3) + matrix(1, 0) * matrix(2, 3) * matrix(3, 2) +
            matrix(2, 0) * matrix(1, 2) * matrix(3, 3) - matrix(2, 0) * matrix(1, 3) * matrix(3, 2) -
            matrix(3, 0) * matrix(1, 2) * matrix(2, 3) + matrix(3, 0) * matrix(1, 3) * matrix(2, 2),
        matrix(1, 0) * matrix(2, 1) * matrix(3, 3) - matrix(1, 0) * matrix(2, 3) * matrix(3, 1) -
            matrix(2, 0) * matrix(1, 1) * matrix(3, 3) + matrix(2, 0) * matrix(1, 3) * matrix(3, 1) +
            matrix(3, 0) * matrix(1, 1) * matrix(2, 3) - matrix(3, 0) * matrix(1, 3) * matrix(2, 1),
        -matrix(1, 0) * matrix(2, 1) * matrix(3, 2) + matrix(1, 0) * matrix(2, 2) * matrix(3, 1) +
            matrix(2, 0) * matrix(1, 1) * matrix(3, 2) - matrix(2, 0) * matrix(1, 2) * matrix(3, 1) -
            matrix(3, 0) * matrix(1, 1) * matrix(2, 2) + matrix(3, 0) * matrix(1, 2) * matrix(2, 1),
        -matrix(0, 1) * matrix(2, 2) * matrix(3, 3) + matrix(0, 1) * matrix(2, 3) * matrix(3, 2) +
            matrix(2, 1) * matrix(0, 2) * matrix(3, 3) - matrix(2, 1) * matrix(0, 3) * matrix(3, 2) -
            matrix(3, 1) * matrix(0, 2) * matrix(2, 3) + matrix(3, 1) * matrix(0, 3) * matrix(2, 2),
        matrix(0, 0) * matrix(2, 2) * matrix(3, 3) - matrix(0, 0) * matrix(2, 3) * matrix(3, 2) -
            matrix(2, 0) * matrix(0, 2) * matrix(3, 3) + matrix(2, 0) * matrix(0, 3) * matrix(3, 2) +
            matrix(3, 0) * matrix(0, 2) * matrix(2, 3) - matrix(3, 0) * matrix(0, 3) * matrix(2, 2),
        -matrix(0, 0) * matrix(2, 1) * matrix(3, 3) + matrix(0, 0) * matrix(2, 3) * matrix(3, 1) +
            matrix(2, 0) * matrix(0, 1) * matrix(3, 3) - matrix(2, 0) * matrix(0, 3) * matrix(3, 1) -
            matrix(3, 0) * matrix(0, 1) * matrix(2, 3) + matrix(3, 0) * matrix(0, 3) * matrix(2, 1),
        matrix(0, 0) * matrix(2, 1) * matrix(3, 2) - matrix(0, 0) * matrix(2, 2) * matrix(3, 1) -
            matrix(2, 0) * matrix(0, 1) * matrix(3, 2) + matrix(2, 0) * matrix(0, 2) * matrix(3, 1) +
            matrix(3, 0) * matrix(0, 1) * matrix(2, 2) - matrix(3, 0) * matrix(0, 2) * matrix(2, 1),
        matrix(0, 1) * matrix(1, 2) * matrix(3, 3) - matrix(0, 1) * matrix(1, 3) * matrix(3, 2) -
            matrix(1, 1) * matrix(0, 2) * matrix(3, 3) + matrix(1, 1) * matrix(0, 3) * matrix(3, 2) +
            matrix(3, 1) * matrix(0, 2) * matrix(1, 3) - matrix(3, 1) * matrix(0, 3) * matrix(1, 2),
        -matrix(0, 0) * matrix(1, 2) * matrix(3, 3) + matrix(0, 0) * matrix(1, 3) * matrix(3, 2) +
            matrix(1, 0) * matrix(0, 2) * matrix(3, 3) - matrix(1, 0) * matrix(0, 3) * matrix(3, 2) -
            matrix(3, 0) * matrix(0, 2) * matrix(1, 3) + matrix(3, 0) * matrix(0, 3) * matrix(1, 2),
        matrix(0, 0) * matrix(1, 1) * matrix(3, 3) - matrix(0, 0) * matrix(1, 3) * matrix(3, 1) -
            matrix(1, 0) * matrix(0, 1) * matrix(3, 3) + matrix(1, 0) * matrix(0, 3) * matrix(3, 1) +
            matrix(3, 0) * matrix(0, 1) * matrix(1, 3) - matrix(3, 0) * matrix(0, 3) * matrix(1, 1),
        -matrix(0, 0) * matrix(1, 1) * matrix(3, 2) + matrix(0, 0) * matrix(1, 2) * matrix(3, 1) +
            matrix(1, 0) * matrix(0, 1) * matrix(3, 2) - matrix(1, 0) * matrix(0, 2) * matrix(3, 1) -
            matrix(3, 0) * matrix(0, 1) * matrix(1, 2) + matrix(3, 0) * matrix(0, 2) * matrix(1, 1),
        -matrix(0, 1) * matrix(1, 2) * matrix(2, 3) + matrix(0, 1) * matrix(1, 3) * matrix(2, 2) +
            matrix(1, 1) * matrix(0, 2) * matrix(2, 3) - matrix(1, 1) * matrix(0, 3) * matrix(2, 2) -
            matrix(2, 1) * matrix(0, 2) * matrix(1, 3) + matrix(2, 1) * matrix(0, 3) * matrix(1, 2),
        matrix(0, 0) * matrix(1, 2) * matrix(2, 3) - matrix(0, 0) * matrix(1, 3) * matrix(2, 2) -
            matrix(1, 0) * matrix(0, 2) * matrix(2, 3) + matrix(1, 0) * matrix(0, 3) * matrix(2, 2) +
            matrix(2, 0) * matrix(0, 2) * matrix(1, 3) - matrix(2, 0) * matrix(0, 3) * matrix(1, 2),
        -matrix(0, 0) * matrix(1, 1) * matrix(2, 3) + matrix(0, 0) * matrix(1, 3) * matrix(2, 1) +
            matrix(1, 0) * matrix(0, 1) * matrix(2, 3) - matrix(1, 0) * matrix(0, 3) * matrix(2, 1) -
            matrix(2, 0) * matrix(0, 1) * matrix(1, 3) + matrix(2, 0) * matrix(0, 3) * matrix(1, 1),
        matrix(0, 0) * matrix(1, 1) * matrix(2, 2) - matrix(0, 0) * matrix(1, 2) * matrix(2, 1) -
            matrix(1, 0) * matrix(0, 1) * matrix(2, 2) + matrix(1, 0) * matrix(0, 2) * matrix(2, 1) +
            matrix(2, 0) * matrix(0, 1) * matrix(1, 2) - matrix(2, 0) * matrix(0, 2) * matrix(1, 1));
}

bool Matrix4::operator==(const Matrix4& matrix) const
{
    return std::equal(&matrix4x4_[0][0], &matrix4x4_[0][0] + MATRIX_LENGTH, &matrix.matrix4x4_[0][0], IsEqual);
}

Matrix4 Matrix4::operator*(double num)
{
    Matrix4 ret(*this);
    std::for_each_n(&ret.matrix4x4_[0][0], MATRIX_LENGTH, [num](double& v) { v *= num; });
    return ret;
}

Matrix4 Matrix4::operator*(const Matrix4& matrix)
{
    return Matrix4(
        matrix4x4_[0][0] * matrix(0, 0) + matrix4x4_[1][0] * matrix(0, 1) + matrix4x4_[2][0] * matrix(0, 2) +
            matrix4x4_[3][0] * matrix(0, 3),
        matrix4x4_[0][0] * matrix(1, 0) + matrix4x4_[1][0] * matrix(1, 1) + matrix4x4_[2][0] * matrix(1, 2) +
            matrix4x4_[3][0] * matrix(1, 3),
        matrix4x4_[0][0] * matrix(2, 0) + matrix4x4_[1][0] * matrix(2, 1) + matrix4x4_[2][0] * matrix(2, 2) +
            matrix4x4_[3][0] * matrix(2, 3),
        matrix4x4_[0][0] * matrix(3, 0) + matrix4x4_[1][0] * matrix(3, 1) + matrix4x4_[2][0] * matrix(3, 2) +
            matrix4x4_[3][0] * matrix(3, 3),
        matrix4x4_[0][1] * matrix(0, 0) + matrix4x4_[1][1] * matrix(0, 1) + matrix4x4_[2][1] * matrix(0, 2) +
            matrix4x4_[3][1] * matrix(0, 3),
        matrix4x4_[0][1] * matrix(1, 0) + matrix4x4_[1][1] * matrix(1, 1) + matrix4x4_[2][1] * matrix(1, 2) +
            matrix4x4_[3][1] * matrix(1, 3),
        matrix4x4_[0][1] * matrix(2, 0) + matrix4x4_[1][1] * matrix(2, 1) + matrix4x4_[2][1] * matrix(2, 2) +
            matrix4x4_[3][1] * matrix(2, 3),
        matrix4x4_[0][1] * matrix(3, 0) + matrix4x4_[1][1] * matrix(3, 1) + matrix4x4_[2][1] * matrix(3, 2) +
            matrix4x4_[3][1] * matrix(3, 3),
        matrix4x4_[0][2] * matrix(0, 0) + matrix4x4_[1][2] * matrix(0, 1) + matrix4x4_[2][2] * matrix(0, 2) +
            matrix4x4_[3][2] * matrix(0, 3),
        matrix4x4_[0][2] * matrix(1, 0) + matrix4x4_[1][2] * matrix(1, 1) + matrix4x4_[2][2] * matrix(1, 2) +
            matrix4x4_[3][2] * matrix(1, 3),
        matrix4x4_[0][2] * matrix(2, 0) + matrix4x4_[1][2] * matrix(2, 1) + matrix4x4_[2][2] * matrix(2, 2) +
            matrix4x4_[3][2] * matrix(2, 3),
        matrix4x4_[0][2] * matrix(3, 0) + matrix4x4_[1][2] * matrix(3, 1) + matrix4x4_[2][2] * matrix(3, 2) +
            matrix4x4_[3][2] * matrix(3, 3),
        matrix4x4_[0][3] * matrix(0, 0) + matrix4x4_[1][3] * matrix(0, 1) + matrix4x4_[2][3] * matrix(0, 2) +
            matrix4x4_[3][3] * matrix(0, 3),
        matrix4x4_[0][3] * matrix(1, 0) + matrix4x4_[1][3] * matrix(1, 1) + matrix4x4_[2][3] * matrix(1, 2) +
            matrix4x4_[3][3] * matrix(1, 3),
        matrix4x4_[0][3] * matrix(2, 0) + matrix4x4_[1][3] * matrix(2, 1) + matrix4x4_[2][3] * matrix(2, 2) +
            matrix4x4_[3][3] * matrix(2, 3),
        matrix4x4_[0][3] * matrix(3, 0) + matrix4x4_[1][3] * matrix(3, 1) + matrix4x4_[2][3] * matrix(3, 2) +
            matrix4x4_[3][3] * matrix(3, 3));
}

Matrix4N Matrix4::operator*(const Matrix4N& matrix) const
{
    int32_t columns = matrix.GetColNum();
    Matrix4N matrix4n { columns };
    for (auto i = 0; i < DIMENSION; i++) {
        for (auto j = 0; j < columns; j++) {
            double value = 0.0;
            for (auto k = 0; k < DIMENSION; k++) {
                value += matrix4x4_[i][k] * matrix[k][j];
            }
            matrix4n[i][j] = value;
        }
    }
    return matrix4n;
}

Point Matrix4::operator*(const Point& point)
{
    double x = point.GetX();
    double y = point.GetY();
    return Point(matrix4x4_[0][0] * x + matrix4x4_[1][0] * y + matrix4x4_[3][0],
        matrix4x4_[0][1] * x + matrix4x4_[1][1] * y + matrix4x4_[3][1]);
}

Matrix4& Matrix4::operator=(const Matrix4& matrix)
{
    if (this == &matrix) {
        return *this;
    }
    std::copy_n(&matrix.matrix4x4_[0][0], MATRIX_LENGTH, &matrix4x4_[0][0]);
    return *this;
}

double Matrix4::operator[](int32_t index) const
{
    if (index < 0 || index >= MATRIX_LENGTH) {
        return 0.0f;
    }
    int32_t row = index / DIMENSION;
    int32_t col = index % DIMENSION;
    return matrix4x4_[row][col];
}

double Matrix4::operator()(int32_t row, int32_t col) const
{
    // Caller guarantee row and col in range of [0, 3].
    return matrix4x4_[row][col];
}

double Matrix4::Determinant() const
{
    if (this->IsIdentityMatrix()) {
        return 1.0;
    }

    double m00 = matrix4x4_[0][0];
    double m01 = matrix4x4_[0][1];
    double m02 = matrix4x4_[0][2];
    double m03 = matrix4x4_[0][3];
    double m10 = matrix4x4_[1][0];
    double m11 = matrix4x4_[1][1];
    double m12 = matrix4x4_[1][2];
    double m13 = matrix4x4_[1][3];
    double m20 = matrix4x4_[2][0];
    double m21 = matrix4x4_[2][1];
    double m22 = matrix4x4_[2][2];
    double m23 = matrix4x4_[2][3];
    double m30 = matrix4x4_[3][0];
    double m31 = matrix4x4_[3][1];
    double m32 = matrix4x4_[3][2];
    double m33 = matrix4x4_[3][3];

    double b00 = m00 * m11 - m01 * m10;
    double b01 = m00 * m12 - m02 * m10;
    double b02 = m00 * m13 - m03 * m10;
    double b03 = m01 * m12 - m02 * m11;
    double b04 = m01 * m13 - m03 * m11;
    double b05 = m02 * m13 - m03 * m12;
    double b06 = m20 * m31 - m21 * m30;
    double b07 = m20 * m32 - m22 * m30;
    double b08 = m20 * m33 - m23 * m30;
    double b09 = m21 * m32 - m22 * m31;
    double b10 = m21 * m33 - m23 * m31;
    double b11 = m22 * m33 - m23 * m32;

    return b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
}

void Matrix4::Transpose()
{
    std::swap(matrix4x4_[0][1], matrix4x4_[1][0]);
    std::swap(matrix4x4_[0][2], matrix4x4_[2][0]);
    std::swap(matrix4x4_[0][3], matrix4x4_[3][0]);
    std::swap(matrix4x4_[1][2], matrix4x4_[2][1]);
    std::swap(matrix4x4_[1][3], matrix4x4_[3][1]);
    std::swap(matrix4x4_[2][3], matrix4x4_[3][2]);
}

void Matrix4::MapScalars(const double src[DIMENSION], double dst[DIMENSION]) const
{
    double storage[DIMENSION];

    double* result = (src == dst) ? storage : dst;

    for (int i = 0; i < DIMENSION; i++) {
        double value = 0;
        for (int j = 0; j < DIMENSION; j++) {
            value += matrix4x4_[j][i] * src[j];
        }
        result[i] = value;
    }

    if (storage == result) {
        std::copy_n(result, DIMENSION, dst);
    }
}

std::string Matrix4::ToString() const
{
    std::string out;
    for (auto& i : matrix4x4_) {
        for (double j : i) {
            out += std::to_string(j);
            out += ",";
        }
        out += "\n";
    }
    return out;
}

Matrix4N::Matrix4N(int32_t columns) : columns_(columns)
{
    matrix4n_.resize(DIMENSION, std::vector<double>(columns_, 0));
}

bool Matrix4N::SetEntry(int32_t row, int32_t col, double value)
{
    if (row >= DIMENSION || col >= columns_) {
        return false;
    }
    matrix4n_[row][col] = value;
    return true;
}

Matrix4 Matrix4N::operator*(const MatrixN4& matrix) const
{
    auto matrix4 = Matrix4::CreateIdentity();
    if (columns_ != matrix.GetRowNum()) {
        return matrix4;
    }
    for (auto i = 0; i < DIMENSION; i++) {
        for (auto j = 0; j < DIMENSION; j++) {
            double value = 0.0;
            for (auto k = 0; k < columns_; k++) {
                value += matrix4n_[i][k] * matrix[k][j];
            }
            matrix4.SetEntry(i, j, value);
        }
    }
    return matrix4;
}

MatrixN4 Matrix4N::Transpose() const
{
    MatrixN4 matrix { columns_ };
    for (auto i = 0; i < DIMENSION; i++) {
        for (auto j = 0; j < columns_; j++) {
            matrix[j][i] = matrix4n_[i][j];
        }
    }
    return matrix;
}

std::vector<double> Matrix4N::MapScalars(const std::vector<double>& src) const
{
    std::vector<double> value { DIMENSION, 0 };
    if (static_cast<int32_t>(src.size()) != columns_) {
        return value;
    }
    for (int32_t i = 0; i < DIMENSION; i++) {
        double item = 0.0;
        for (int32_t j = 0; j < columns_; j++) {
            item = item + matrix4n_[i][j] * src[j];
        }
        value[i] = item;
    }
    return value;
}

bool Matrix4N::MapScalars(const std::vector<double>& src, std::vector<double>& result) const
{
    if (static_cast<int32_t>(src.size()) != columns_) {
        return false;
    }
    result.resize(DIMENSION, 0);
    for (int32_t i = 0; i < DIMENSION; i++) {
        double item = 0.0;
        for (int32_t j = 0; j < columns_; j++) {
            item = item + matrix4n_[i][j] * src[j];
        }
        result[i] = item;
    }
    return true;
}

MatrixN4::MatrixN4(int32_t rows) : rows_(rows)
{
    matrixn4_.resize(rows, std::vector<double>(DIMENSION, 0));
}

bool MatrixN4::SetEntry(int32_t row, int32_t col, double value)
{
    if (row >= rows_ || col >= DIMENSION) {
        return false;
    }
    matrixn4_[row][col] = value;
    return true;
}

Matrix4N MatrixN4::Transpose() const
{
    Matrix4N matrix { rows_ };
    for (auto i = 0; i < DIMENSION; i++) {
        for (auto j = 0; j < rows_; j++) {
            matrix[i][j] = matrixn4_[j][i];
        }
    }
    return matrix;
}

std::vector<double> MatrixN4::MapScalars(const std::vector<double>& src) const
{
    std::vector<double> value { rows_, 0 };
    if (static_cast<int32_t>(src.size()) != DIMENSION) {
        return value;
    }
    for (int32_t i = 0; i < rows_; i++) {
        double item = 0.0;
        for (int32_t j = 0; j < DIMENSION; j++) {
            item = item + matrixn4_[i][j] * src[j];
        }
        value[i] = item;
    }
    return value;
}
} // namespace OHOS::Ace
