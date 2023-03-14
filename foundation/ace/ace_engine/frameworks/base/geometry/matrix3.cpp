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

#include "base/geometry/matrix3.h"

#include "base/utils/utils.h"

namespace OHOS::Ace {
void Matrix3::SetEntry(int32_t row, int32_t col, double value)
{
    if ((row < 0 || row >= DIMENSION) || (col < 0 || col >= DIMENSION)) {
        return;
    }
    matrix3X3_[row][col] = value;
}

bool Matrix3::Invert(Matrix3& matrix) const
{
    static const double diff = 1e-20;
    double val1 = matrix3X3_[0][0] * matrix3X3_[1][1] * matrix3X3_[2][2];
    double val2 = matrix3X3_[0][0] * matrix3X3_[1][2] * matrix3X3_[2][1];
    double val3 = matrix3X3_[1][0] * matrix3X3_[0][1] * matrix3X3_[2][2];
    double val4 = matrix3X3_[1][0] * matrix3X3_[0][2] * matrix3X3_[2][1];
    double val5 = matrix3X3_[2][0] * matrix3X3_[0][1] * matrix3X3_[1][2];
    double val6 = matrix3X3_[2][0] * matrix3X3_[0][2] * matrix3X3_[1][1];
    double detA = val1 - val2 - val3 + val4 + val5 - val6;
    if (NearZero(detA, diff)) {
        return false;
    }
    detA = 1.0 / detA;
    // a11a22 - a12a21
    matrix[0][0] = matrix3X3_[1][1] * matrix3X3_[2][2] - matrix3X3_[1][2] * matrix3X3_[2][1];
    // a20a21 - a01a22
    matrix[0][1] = matrix3X3_[0][2] * matrix3X3_[2][1] - matrix3X3_[0][1] * matrix3X3_[2][2];
    // a01a12 - a02a11
    matrix[0][2] = matrix3X3_[0][1] * matrix3X3_[1][2] - matrix3X3_[0][2] * matrix3X3_[1][1];
    // a12a20 - a10a22
    matrix[1][0] = matrix3X3_[1][2] * matrix3X3_[2][0] - matrix3X3_[1][0] * matrix3X3_[2][2];
    // a00a22 - a02a20
    matrix[1][1] = matrix3X3_[0][0] * matrix3X3_[2][2] - matrix3X3_[0][2] * matrix3X3_[2][0];
    // a10a02 - a00a12
    matrix[1][2] = matrix3X3_[1][0] * matrix3X3_[0][2] - matrix3X3_[0][0] * matrix3X3_[1][2];
    // a10a21 - a11a20
    matrix[2][0] = matrix3X3_[1][0] * matrix3X3_[2][1] - matrix3X3_[1][1] * matrix3X3_[2][0];
    // a01a20 - a00a21
    matrix[2][1] = matrix3X3_[0][1] * matrix3X3_[2][0] - matrix3X3_[0][0] * matrix3X3_[2][1];
    // a00a11 - a10a01
    matrix[2][2] = matrix3X3_[0][0] * matrix3X3_[1][1] - matrix3X3_[1][0] * matrix3X3_[0][1];
    // invert
    matrix* detA;
    return true;
}

Matrix3N Matrix3::operator*(const Matrix3N& matrix) const
{
    int32_t columns = matrix.GetColNum();
    Matrix3N Matrix3n { columns };
    for (auto i = 0; i < DIMENSION; i++) {
        for (auto j = 0; j < columns; j++) {
            double value = 0.0;
            for (auto k = 0; k < DIMENSION; k++) {
                value += matrix3X3_[i][k] * matrix[k][j];
            }
            Matrix3n[i][j] = value;
        }
    }
    return Matrix3n;
}

Matrix3 Matrix3::Transpose() const
{
    Matrix3 matrix;
    for (auto i = 0; i < DIMENSION; i++) {
        for (auto j = 0; j < DIMENSION; j++) {
            matrix[j][i] = matrix3X3_[i][j];
        }
    }
    return matrix;
}

std::vector<double> Matrix3::MapScalars(const std::vector<double>& src) const
{
    std::vector<double> value { DIMENSION, 0 };
    if (static_cast<int32_t>(src.size()) != DIMENSION) {
        return value;
    }
    for (int32_t i = 0; i < DIMENSION; i++) {
        double item = 0.0;
        for (int32_t j = 0; j < DIMENSION; j++) {
            item = item + matrix3X3_[i][j] * src[j];
        }
        value[i] = item;
    }
    return value;
}

bool Matrix3::MapScalars(const std::vector<double>& src, std::vector<double>& result) const
{
    if (static_cast<int32_t>(src.size()) != DIMENSION) {
        return false;
    }
    result.resize(DIMENSION, 0);
    for (int32_t i = 0; i < DIMENSION; i++) {
        double item = 0.0;
        for (int32_t j = 0; j < DIMENSION; j++) {
            item = item + matrix3X3_[i][j] * src[j];
        }
        result[i] = item;
    }
    return true;
}

Matrix3N::Matrix3N(int32_t columns) : columns_(columns)
{
    Matrix3n_.resize(DIMENSION, std::vector<double>(columns_, 0));
}

bool Matrix3N::SetEntry(int32_t row, int32_t col, double value)
{
    if (row >= DIMENSION || col >= columns_) {
        return false;
    }
    Matrix3n_[row][col] = value;
    return true;
}

Matrix3 Matrix3N::operator*(const MatrixN3& matrix) const
{
    Matrix3 Matrix3;
    if (columns_ != matrix.GetRowNum()) {
        return Matrix3;
    }
    for (auto i = 0; i < DIMENSION; i++) {
        for (auto j = 0; j < DIMENSION; j++) {
            double value = 0.0;
            for (auto k = 0; k < columns_; k++) {
                value += Matrix3n_[i][k] * matrix[k][j];
            }
            Matrix3[i][j] = value;
        }
    }
    return Matrix3;
}

MatrixN3 Matrix3N::Transpose() const
{
    MatrixN3 matrix { columns_ };
    for (auto i = 0; i < DIMENSION; i++) {
        for (auto j = 0; j < columns_; j++) {
            matrix[j][i] = Matrix3n_[i][j];
        }
    }
    return matrix;
}

std::vector<double> Matrix3N::MapScalars(const std::vector<double>& src) const
{
    std::vector<double> value { DIMENSION, 0 };
    if (static_cast<int32_t>(src.size()) != columns_) {
        return value;
    }
    for (int32_t i = 0; i < DIMENSION; i++) {
        double item = 0.0;
        for (int32_t j = 0; j < columns_; j++) {
            item = item + Matrix3n_[i][j] * src[j];
        }
        value[i] = item;
    }
    return value;
}

bool Matrix3N::MapScalars(const std::vector<double>& src, std::vector<double>& result) const
{
    if (static_cast<int32_t>(src.size()) != columns_) {
        LOGE("failt to MapScalars, due to %{public}d, %{public}d", static_cast<int32_t>(src.size()), columns_);
        return false;
    }
    result.resize(DIMENSION, 0);
    for (int32_t i = 0; i < DIMENSION; i++) {
        double item = 0.0;
        for (int32_t j = 0; j < columns_; j++) {
            item = item + Matrix3n_[i][j] * src[j];
        }
        result[i] = item;
    }
    return true;
}

MatrixN3::MatrixN3(int32_t rows) : rows_(rows)
{
    Matrixn3_.resize(rows, std::vector<double>(DIMENSION, 0));
}

bool MatrixN3::SetEntry(int32_t row, int32_t col, double value)
{
    if (row >= rows_ || col >= DIMENSION) {
        return false;
    }
    Matrixn3_[row][col] = value;
    return true;
}

Matrix3N MatrixN3::Transpose() const
{
    Matrix3N matrix { rows_ };
    for (auto i = 0; i < DIMENSION; i++) {
        for (auto j = 0; j < rows_; j++) {
            matrix[i][j] = Matrixn3_[j][i];
        }
    }
    return matrix;
}

std::vector<double> MatrixN3::MapScalars(const std::vector<double>& src) const
{
    std::vector<double> value { rows_, 0 };
    if (static_cast<int32_t>(src.size()) != DIMENSION) {
        return value;
    }
    for (int32_t i = 0; i < rows_; i++) {
        double item = 0.0;
        for (int32_t j = 0; j < DIMENSION; j++) {
            item = item + Matrixn3_[i][j] * src[j];
        }
        value[i] = item;
    }
    return value;
}
} // namespace OHOS::Ace
