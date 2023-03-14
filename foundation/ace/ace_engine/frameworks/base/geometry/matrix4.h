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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_MATRIX4_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_MATRIX4_H

#include <vector>

#include "base/geometry/point.h"

namespace OHOS::Ace {

class Matrix4N;
class MatrixN4;

class ACE_EXPORT Matrix4 {
public:
    // Matrix dimension is 4X4.
    static constexpr int32_t DIMENSION = 4;
    // Create an identity matrix.
    static Matrix4 CreateIdentity();
    // Multiplies this matrix by another that translates coordinates by the vector (x, y, z).
    static Matrix4 CreateTranslate(double x, double y, double z);
    // Multiplies this matrix by another that scales coordinates by the vector (x, y, z).
    static Matrix4 CreateScale(double x, double y, double z);
    // Multiplies this matrix by another that rotates coordinates through angle degrees about the vector (dx, dy, dz).
    static Matrix4 CreateRotate(double angle, double dx, double dy, double dz);

    static Matrix4 CreateMatrix2D(double m00, double m10, double m01, double m11, double m03, double m13);
    // Multiplies this matrix by another that skew through angle degrees.
    static Matrix4 CreateSkew(double x, double y);
    // Create an perspective matrix, the distance value represents the distance between the user and the z=0 plane. not
    // support percent
    static Matrix4 CreatePerspective(double distance);
    // Returns the inverse of this matrix. Returns the identity if this matrix cannot be inverted;
    static Matrix4 Invert(const Matrix4& matrix);

    Matrix4();
    Matrix4(const Matrix4& matrix);
    Matrix4(
        double m00, double m01, double m02, double m03,
        double m10, double m11, double m12, double m13,
        double m20, double m21, double m22, double m23,
        double m30, double m31, double m32, double m33);
    ~Matrix4() = default;
    void SetScale(double x, double y, double z);
    double GetScaleX() const;
    double GetScaleY() const;
    void Rotate(double angle, double dx, double dy, double dz);
    void SetEntry(int32_t row, int32_t col, double value);
    bool IsIdentityMatrix() const;
    int32_t Count() const;

    bool operator==(const Matrix4& matrix) const;
    Matrix4 operator*(double num);
    Matrix4 operator*(const Matrix4& matrix);

    Matrix4N operator*(const Matrix4N& matrix) const;

    // Transform point by the matrix
    Point operator*(const Point& point);
    Matrix4& operator=(const Matrix4& matrix);
    double operator[](int32_t index) const;
    inline double Get(int32_t row, int32_t col) const
    {
        ACE_DCHECK((unsigned)row < DIMENSION);
        ACE_DCHECK((unsigned)col < DIMENSION);
        return matrix4x4_[col][row];
    }
    inline void Set(int32_t row, int32_t col, double value)
    {
        ACE_DCHECK((unsigned)row < DIMENSION);
        ACE_DCHECK((unsigned)col < DIMENSION);
        matrix4x4_[col][row] = value;
    }
    double Determinant() const;
    void Transpose();
    void MapScalars(const double src[DIMENSION], double dst[DIMENSION]) const;
    inline void MapScalars(double vec[DIMENSION], int length = DIMENSION) const
    {
        if (length == DIMENSION) {
            this->MapScalars(vec, vec);
        }
    }
    std::string ToString() const;

private:
    static Matrix4 CreateInvert(const Matrix4& matrix);
    double operator()(int32_t row, int32_t col) const;

    double matrix4x4_[DIMENSION][DIMENSION] = {
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
    };
};

class ACE_EXPORT Matrix4N {
public:
    // Matrix dimension is 4X4.
    static constexpr int32_t DIMENSION = 4;

    explicit Matrix4N(int32_t columns);
    ~Matrix4N() = default;

    inline int32_t GetColNum() const
    {
        return columns_;
    }

    bool SetEntry(int32_t row, int32_t col, double value);

    inline Matrix4N& operator*(double num)
    {
        for (auto& vector : matrix4n_) {
            std::for_each(vector.begin(), vector.end(), [num](auto& item) { item = item * num; });
        }
        return *this;
    }

    // Make sure that the rows of matrixN4 is equal than the columns of matrix4N.
    Matrix4 operator*(const MatrixN4& matrix) const;

    // Make sure that the value of index is less than 4.
    inline std::vector<double>& operator[](int32_t index)
    {
        return matrix4n_[index];
    }

        // Make sure that the value of index is less than 4.
    inline const std::vector<double>& operator[](int32_t index) const
    {
        return matrix4n_[index];
    }

    // Make sure that the value of row is less than 4 and col is less than columns.
    inline double operator()(int32_t row, int32_t col) const
    {
        return matrix4n_[row][col];
    }

    MatrixN4 Transpose() const;

    // Make sure that the vector size is equal than column.
    std::vector<double> MapScalars(const std::vector<double>& src) const;

    // Make sure that the vector size is equal than column.
    bool MapScalars(const std::vector<double>& src, std::vector<double>& result) const;

private:
    std::vector<std::vector<double>> matrix4n_;
    int32_t columns_ = 0;
};

class ACE_EXPORT MatrixN4 {
public:
    // Matrix dimension is 4XN.
    static constexpr int32_t DIMENSION = 4;

    explicit MatrixN4(int32_t rows);
    ~MatrixN4() = default;

    inline int32_t GetRowNum() const
    {
        return rows_;
    }

    bool SetEntry(int32_t row, int32_t col, double value);

    inline MatrixN4& operator*(double num)
    {
        for (auto& vector : matrixn4_) {
            std::for_each(vector.begin(), vector.end(), [num](auto& item) { item = item * num; });
        }
        return *this;
    }

    // Make sure that the value of index is less than rows.
    inline std::vector<double>& operator[](int32_t index)
    {
        return matrixn4_[index];
    }

    // Make sure that the value of index is less than rows.
    inline const std::vector<double>& operator[](int32_t index) const
    {
        return matrixn4_[index];
    }

    // Make sure that the value of row is less than rows and col is less than 4.
    inline double operator()(int32_t row, int32_t col) const
    {
        return matrixn4_[row][col];
    }

    Matrix4N Transpose() const;

    // Make sure that the vector size is equal than column.
    std::vector<double> MapScalars(const std::vector<double>& src) const;

private:
    std::vector<std::vector<double>> matrixn4_;
    int32_t rows_ = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_MATRIX4_H
