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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_MATRIX3_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_MATRIX3_H

#include <vector>

#include "base/geometry/point.h"

namespace OHOS::Ace {
class Matrix3N;
class MatrixN3;

class ACE_EXPORT Matrix3 {
public:
    // Matrix dimension is 3X3.
    static constexpr int32_t DIMENSION = 3;

    Matrix3() = default;
    ~Matrix3() = default;

    void SetEntry(int32_t row, int32_t col, double value);

    // Gets the inverse of this matrix;
    bool Invert(Matrix3& matrix) const;

    inline Matrix3& operator*(double num)
    {
        for (auto& vector : matrix3X3_) {
            std::for_each(vector.begin(), vector.end(), [num](auto& item) { item = item * num; });
        }
        return *this;
    }

    Matrix3N operator*(const Matrix3N& matrix) const;

    // Make sure that the value of index is less than 3.
    inline std::vector<double>& operator[](int32_t index)
    {
        return matrix3X3_[index];
    }

    // Make sure that the value of index is less than 3.
    inline const std::vector<double>& operator[](int32_t index) const
    {
        return matrix3X3_[index];
    }

    // Make sure that the value of row is less than 3 and col is less than 3.
    inline double operator()(int32_t row, int32_t col) const
    {
        return matrix3X3_[row][col];
    }

    Matrix3 Transpose() const;

    // Make sure that the vector size is equal than column.
    std::vector<double> MapScalars(const std::vector<double>& src) const;

    // Make sure that the vector size is equal than column.
    bool MapScalars(const std::vector<double>& src, std::vector<double>& result) const;

    std::string ToString() const
    {
        std::string val;
        for (auto& vector : matrix3X3_) {
            std::for_each(vector.begin(), vector.end(),
                [&val](auto& item) { val = val + "item: " + std::to_string(item) + " "; });
        }
        return val;
    }

private:
    std::vector<std::vector<double>> matrix3X3_ = { DIMENSION, std::vector<double>(DIMENSION, 0.0) };
};

class ACE_EXPORT Matrix3N {
public:
    // Matrix dimension is 3X3.
    static constexpr int32_t DIMENSION = 3;

    explicit Matrix3N(int32_t columns);
    ~Matrix3N() = default;

    inline int32_t GetColNum() const
    {
        return columns_;
    }

    bool SetEntry(int32_t row, int32_t col, double value);

    inline Matrix3N& operator*(double num)
    {
        for (auto& vector : Matrix3n_) {
            std::for_each(vector.begin(), vector.end(), [num](auto& item) { item = item * num; });
        }
        return *this;
    }

    // Make sure that the rows of MatrixN3 is equal than the columns of Matrix3N.
    Matrix3 operator*(const MatrixN3& matrix) const;

    // Make sure that the value of index is less than 3.
    inline std::vector<double>& operator[](int32_t index)
    {
        return Matrix3n_[index];
    }

    // Make sure that the value of index is less than 3.
    inline const std::vector<double>& operator[](int32_t index) const
    {
        return Matrix3n_[index];
    }

    // Make sure that the value of row is less than 3 and col is less than columns.
    inline double operator()(int32_t row, int32_t col) const
    {
        return Matrix3n_[row][col];
    }

    MatrixN3 Transpose() const;

    // Make sure that the vector size is equal than column.
    std::vector<double> MapScalars(const std::vector<double>& src) const;

    // Make sure that the vector size is equal than column.
    bool MapScalars(const std::vector<double>& src, std::vector<double>& result) const;

    std::string ToString() const
    {
        std::string val;
        for (auto& vector : Matrix3n_) {
            std::for_each(vector.begin(), vector.end(),
                [&val](auto& item) { val = val + "item: " + std::to_string(item) + " "; });
        }
        return val;
    }

private:
    std::vector<std::vector<double>> Matrix3n_;
    int32_t columns_ = 0;
};

class ACE_EXPORT MatrixN3 {
public:
    // Matrix dimension is 3XN.
    static constexpr int32_t DIMENSION = 3;

    explicit MatrixN3(int32_t rows);
    ~MatrixN3() = default;

    inline int32_t GetRowNum() const
    {
        return rows_;
    }

    bool SetEntry(int32_t row, int32_t col, double value);

    inline MatrixN3& operator*(double num)
    {
        for (auto& vector : Matrixn3_) {
            std::for_each(vector.begin(), vector.end(), [num](auto& item) { item = item * num; });
        }
        return *this;
    }

    // Make sure that the value of index is less than rows.
    inline std::vector<double>& operator[](int32_t index)
    {
        return Matrixn3_[index];
    }

    // Make sure that the value of index is less than rows.
    inline const std::vector<double>& operator[](int32_t index) const
    {
        return Matrixn3_[index];
    }

    // Make sure that the value of row is less than rows and col is less than 3.
    inline double operator()(int32_t row, int32_t col) const
    {
        return Matrixn3_[row][col];
    }

    Matrix3N Transpose() const;

    // Make sure that the vector size is equal than column.
    std::vector<double> MapScalars(const std::vector<double>& src) const;

    std::string ToString() const
    {
        std::string val;
        for (auto& vector : Matrixn3_) {
            std::for_each(vector.begin(), vector.end(),
                [&val](auto& item) { val = val + "item: " + std::to_string(item) + " "; });
        }
        return val;
    }

private:
    std::vector<std::vector<double>> Matrixn3_;
    int32_t rows_ = 0;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_MATRIX3_H
