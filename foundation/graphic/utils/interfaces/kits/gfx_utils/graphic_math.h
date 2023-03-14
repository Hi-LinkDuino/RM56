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

/**
 * @addtogroup UI_Utils
 * @{
 *
 * @brief Defines basic UI utils.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file graphic_math.h
 *
 * @brief Defines mathematical operation functions and types, including trigonometric functions, two-dimensional
 *        vectors, three-dimensional vectors, and matrices.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_GRAPHIC_MATH_H
#define GRAPHIC_LITE_GRAPHIC_MATH_H

#include "gfx_utils/heap_base.h"
#include <cstdint>

namespace OHOS {
/**
 * @brief Larger of a and b.
 */
#define MATH_MAX(a, b) ((a) > (b) ? (a) : (b))
/**
 * @brief Smaller of a and b.
 */
#define MATH_MIN(a, b) ((a) < (b) ? (a) : (b))
/**
 * @brief Absolute value of x.
 */
#define MATH_ABS(x) ((x) > 0 ? (x) : (-(x)))
/**
 * @brief Difference between a and b.
 */
#define MATH_MINUS(a, b) ((a) < (b) ? ((b) - (a)) : ((a) - (b)))

#define MATH_ROUND(x) ((x) > 0 ? (int16_t)((x) + 0.5) : (int16_t)((x) - 0.5))

#define MATH_ROUND32(x) ((x) > 0 ? (int32_t)((x) + 0.5) : (int32_t)((x) - 0.5))

#define MATH_UROUND MATH_ROUND32

#define MATH_FLT_EQUAL(a, b) (MATH_ABS((a) - (b)) <= UI_FLT_EPSILON)

/** A quarter of a rotation: 90 degrees */
static constexpr uint16_t QUARTER_IN_DEGREE = 90;
/** A half of a rotation: 180 degrees */
static constexpr uint16_t SEMICIRCLE_IN_DEGREE = 180;
/** Three fourths of a rotation: 270 degrees */
static constexpr uint16_t THREE_QUARTER_IN_DEGREE = 270;
/** A full rotation: 360 degrees */
static constexpr uint16_t CIRCLE_IN_DEGREE = 360;
/** Pi value */
static constexpr double UI_PI = 3.14159265358979;
static constexpr float ATAN2_P3 = -0.3258775;
static constexpr float ATAN2_P5 = 0.1556117;
static constexpr float ATAN2_P7 = -0.0443360;
static constexpr float RADIAN_TO_ANGLE = 57.295779513;
static constexpr float UI_FLT_EPSILON = 1E-5;
static constexpr int16_t ORDER_MATRIX_4 = 4;

float Sin(float angle);
float Cos(float angle);
float Acos(float value);
float Fmod(float X, float Y);
uint16_t FastAtan2(int16_t x, int16_t y);
float FastAtan2F(float x, float y);
float Sqrt(float x);

/**
 * @brief Defines the two-dimensional vector, and provides basic mathematical operations such as vector assignment,
 *        scalar product, cross product, addition, and subtraction.
 *
 * @since 1.0
 * @version 1.0
 */
template<typename T>
class Vector2 : public HeapBase {
public:
    /**
     * @brief A constructor used to create a <b>Vector2</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    Vector2() : x_(0), y_(0) {}

    /**
     * @brief A constructor used to create a <b>Vector2</b> instance based on the X and Y coordinates.
     * @param x Indicates the X coordinate.
     * @param y Indicates the Y coordinate.
     * @since 1.0
     * @version 1.0
     */
    Vector2(T x, T y)
    {
        x_ = x;
        y_ = y;
    }

    /**
     * @brief A destructor used to delete the <b>Vector2</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    ~Vector2() {}

    /**
     * @brief Calculates the scalar product of the two-dimensional vector and another two-dimensional vector.
     * @param other Indicates the two-dimensional vector to be used for calculation.
     * @return Returns the scalar product.
     * @since 1.0
     * @version 1.0
     */
    T Dot(const Vector2<T>& other) const
    {
        T sum = x_ * other.x_;
        sum += y_ * other.y_;
        return sum;
    }

    /**
     * @brief Calculates the cross product of the two-dimensional vector and another two-dimensional vector.
     * @param other Indicates the two-dimensional vector to be used for calculation.
     * @return Returns the cross product.
     * @since 1.0
     * @version 1.0
     */
    T Cross(const Vector2<T>& other) const
    {
        return x_ * other.y_ - y_ * other.x_;
    }

    Vector2 operator-() const
    {
        return Vector2(-x_, -y_);
    }

    Vector2 operator-(const Vector2<T>& other) const
    {
        return Vector2(x_ - other.x_, y_ - other.y_);
    }

    Vector2 operator+(const Vector2<T>& other) const
    {
        return Vector2(x_ + other.x_, y_ + other.y_);
    }

    Vector2 operator*(T scale) const
    {
        return Vector2(x_ * scale, y_ * scale);
    }

    bool operator==(const Vector2& other) const
    {
        return (x_ == other.x_) && (y_ == other.y_);
    }

    Vector2& operator=(const Vector2& other)
    {
        x_ = other.x_;
        y_ = other.y_;
        return *this;
    }

    Vector2& operator+=(const Vector2& other)
    {
        x_ += other.x_;
        y_ += other.y_;
        return *this;
    }

    Vector2& operator-=(const Vector2& other)
    {
        x_ -= other.x_;
        y_ -= other.y_;
        return *this;
    }

    T x_;
    T y_;
};

/**
 * @brief Defines the 3-dimensional vector, and provides basic operators such as [] and ==.
 *
 * @since 1.0
 * @version 1.0
 */
template<typename T>
class Vector3 : public HeapBase {
public:
    union {
        struct {
            T x_;
            T y_;
            T z_;
        };
        T data_[3]; // 3: size for x,y,z
    };

    /**
     * @brief A constructor used to create a <b>Vector3</b> instance.
     * @since 1.0
     * @version 1.0
     */
    Vector3() : x_(0), y_(0), z_(1) {}

    /**
     * @brief Defines a <b>Vector3</b> instance and initializes the values of <b>x</b>, <b>y</b>, and <b>z</b>.
     * @param x Indicates the X coordinate.
     * @param y Indicates the Y coordinate.
     * @param z Indicates the Z coordinate.
     * @since 1.0
     * @version 1.0
     */
    Vector3(T x, T y, T z)
    {
        data_[0] = x;
        data_[1] = y;
        data_[2] = z; // 2: index of z
    }

    /**
     * @brief A destructor used to delete the <b>Vector3</b> instance.
     * @since 1.0
     * @version 1.0
     */
    ~Vector3() {}

    T operator[](uint8_t index) const
    {
        return data_[index];
    }

    T& operator[](uint8_t index)
    {
        return data_[index];
    }

    bool operator==(const Vector3& other) const
    {
        return (x_ == other.x_) && (y_ == other.y_) && (z_ == other.z_);
    }
};

/**
 * @brief Defines the 4-dimensional vector, and provides basic operators such as [] and ==.
 *
 * @since 5.0
 * @version 3.0
 */
template<typename T>
class Vector4 : public HeapBase {
public:
    union {
        struct {
            T x_;
            T y_;
            T z_;
            T w_;
        };
        T data_[4]; // 4: size for x,y,z,w
    };

    /**
     * @brief A constructor used to create a <b>Vector4</b> instance.
     * @since 5.0
     * @version 3.0
     */
    Vector4() : x_(0), y_(0), z_(0), w_(1) {}

    Vector4(T x, T y, T z, T w)
    {
        data_[0] = x;
        data_[1] = y;
        data_[2] = z; // 2: index of z
        data_[3] = w; // 3: index of w
    }

    /**
     * @brief A destructor used to delete the <b>Vector4</b> instance.
     * @since 5.0
     * @version 3.0
     */
    ~Vector4() {}

    T operator[](uint8_t index) const
    {
        return data_[index];
    }

    T& operator[](uint8_t index)
    {
        return data_[index];
    }

    bool operator==(const Vector4& other) const
    {
        return (x_ == other.x_) && (y_ == other.y_) && (z_ == other.z_) && (w_ == other.w_);
    }
};

/**
 * @brief Defines a 3 x 3 matrix.
 *
 * @since 1.0
 * @version 1.0
 */
template<typename T>
class Matrix3 : public HeapBase {
public:
    /**
     * @brief Defines a <b>Matrix3</b> instance and initializes the 3 x 3 matrix data.
     *
     * @since 1.0
     * @version 1.0
     */
    Matrix3();

    /**
     * @brief Defines a <b>Matrix3</b> instance and initializes the 3 x 3 matrix data.
     *
     * @param m00 Indicates the element in row 1 and column 1 of the matrix.
     * @param m01 Indicates the element in row 1 and column 2 of the matrix.
     * @param m02 Indicates the element in row 1 and column 3 of the matrix.
     * @param m10 Indicates the element in row 2 and column 1 of the matrix.
     * @param m11 Indicates the element in row 2 and column 2 of the matrix.
     * @param m12 Indicates the element in row 2 and column 3 of the matrix.
     * @param m20 Indicates the element in row 3 and column 1 of the matrix.
     * @param m21 Indicates the element in row 3 and column 2 of the matrix.
     * @param m22 Indicates the element in row 3 and column 3 of the matrix.
     * @since 1.0
     * @version 1.0
     */
    Matrix3(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22);

    /**
     * @brief A destructor used to delete the <b>Matrix3</b> instance.
     * @since 1.0
     * @version 1.0
     */
    ~Matrix3() {}

    /**
     * @brief Obtains the 3 x 3 matrix data.
     * @return Returns the 3 x 3 matrix data.
     * @since 1.0
     * @version 1.0
     */
    const T* GetData() const
    {
        return data_;
    }

    /**
     * @brief Obtains the determinant of the matrix.
     * @return Returns the determinant.
     * @since 1.0
     * @version 1.0
     */
    T Determinant() const;

    /**
     * @brief Obtains the inverse matrix.
     * @return Returns the inverse matrix.
     * @since 1.0
     * @version 1.0
     */
    Matrix3 Inverse() const;

    /**
     * @brief Obtains a rotation matrix. After a matrix is rotated, its data is shifted leftwards by 15 bits. Therefore,
     *        the result data needs to be shifted rightwards by 15 bits.
     * @param angle Indicates the angle to rotate.
     * @param pivot Indicates the rotation pivot.
     * @return Returns the matrix after rotation.
     * @since 1.0
     * @version 1.0
     */
    static Matrix3 Rotate(T angle, const Vector2<T>& pivot);

    /**
     * @brief Obtains the scaling matrix. After a matrix is scaled, its data is shifted leftwards by 8 bits. Therefore,
     *        the result data needs to be shifted rightwards by 8 bits.
     * @param scale Indicates the scaling factors of the x-axis and y-axis.
     * @param fixed Indicates the fixed scaling point.
     * @return Returns the matrix after scaling.
     * @since 1.0
     * @version 1.0
     */
    static Matrix3 Scale(const Vector2<T>& scale, const Vector2<T>& fixed);

    /**
     * @brief Obtains a matrix translation.
     * @param trans Indicates the distances to translate along the x-axis and y-axis.
     * @return Returns the matrix after translation.
     * @since 1.0
     * @version 1.0
     */
    static Matrix3<T> Translate(const Vector2<T>& trans);

    Matrix3 operator*(const Matrix3& other) const;

    Vector3<T> operator*(const Vector3<T>& other) const;

    T* operator[](uint8_t col)
    {
        return &data_[col * 3]; // 3: size of point
    }

    Matrix3& operator=(const Matrix3& other)
    {
        // 9: data_ value number
        data_[0] = other.data_[0];
        data_[1] = other.data_[1];
        data_[2] = other.data_[2];
        data_[3] = other.data_[3];
        data_[4] = other.data_[4];
        data_[5] = other.data_[5];
        data_[6] = other.data_[6];
        data_[7] = other.data_[7];
        data_[8] = other.data_[8];
        return *this;
    }

    bool operator==(const Matrix3& other) const;

protected:
    T data_[9]; // 9: 3 point with x,y,z
};

template<typename T>
Matrix3<T>::Matrix3()
{
    data_[1] = 0;
    data_[2] = 0;
    data_[3] = 0;
    data_[5] = 0;
    data_[6] = 0;
    data_[7] = 0;
    data_[0] = 1;
    data_[4] = 1;
    data_[8] = 1;
}

template<typename T>
Matrix3<T>::Matrix3(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22)
{
    data_[0] = m00;
    data_[1] = m01;
    data_[2] = m02;

    data_[3] = m10;
    data_[4] = m11;
    data_[5] = m12;

    data_[6] = m20;
    data_[7] = m21;
    data_[8] = m22;
}

template<typename T>
Matrix3<T> Matrix3<T>::operator*(const Matrix3<T>& other) const
{
    Matrix3<T> rMulti;
    T* rData = rMulti.data_;
    const T* oData = other.data_;

    rData[0] = data_[0] * oData[0] + data_[3] * oData[1] + data_[6] * oData[2];
    rData[3] = data_[0] * oData[3] + data_[3] * oData[4] + data_[6] * oData[5];
    rData[6] = data_[0] * oData[6] + data_[3] * oData[7] + data_[6] * oData[8];

    rData[1] = data_[1] * oData[0] + data_[4] * oData[1] + data_[7] * oData[2];
    rData[4] = data_[1] * oData[3] + data_[4] * oData[4] + data_[7] * oData[5];
    rData[7] = data_[1] * oData[6] + data_[4] * oData[7] + data_[7] * oData[8];

    rData[2] = data_[2] * oData[0] + data_[5] * oData[1] + data_[8] * oData[2];
    rData[5] = data_[2] * oData[3] + data_[5] * oData[4] + data_[8] * oData[5];
    rData[8] = data_[2] * oData[6] + data_[5] * oData[7] + data_[8] * oData[8];
    return rMulti;
}

template<typename T>
Vector3<T> Matrix3<T>::operator*(const Vector3<T>& other) const
{
    Vector3<T> rMulti;
    T* rData = rMulti.data_;
    const T* oData = other.data_;
    rData[0] = data_[0] * oData[0] + data_[3] * oData[1] + data_[6] * oData[2];
    rData[1] = data_[1] * oData[0] + data_[4] * oData[1] + data_[7] * oData[2];
    rData[2] = data_[2] * oData[0] + data_[5] * oData[1] + data_[8] * oData[2];
    return rMulti;
}

template<typename T>
T Matrix3<T>::Determinant() const
{
    T x = data_[0] * ((data_[4] * data_[8]) - (data_[5] * data_[7]));
    T y = data_[1] * ((data_[3] * data_[8]) - (data_[5] * data_[6]));
    T z = data_[2] * ((data_[3] * data_[7]) - (data_[4] * data_[6]));
    return (x - y) + z;
}

template<typename T>
Matrix3<T> Matrix3<T>::Inverse() const
{
    T det = this->Determinant();
    if (det == 0) {
        return Matrix3<T>(*this);
    }

    T invDet = 1.0f / det;
    const T* data = this->data_;

    T iX = invDet * (data[4] * data[8] - data[5] * data[7]);
    T iY = invDet * (data[2] * data[7] - data[1] * data[8]);
    T iZ = invDet * (data[1] * data[5] - data[2] * data[4]);
    T jX = invDet * (data[5] * data[6] - data[3] * data[8]);
    T jY = invDet * (data[0] * data[8] - data[2] * data[6]);
    T jZ = invDet * (data[2] * data[3] - data[0] * data[5]);
    T kX = invDet * (data[3] * data[7] - data[4] * data[6]);
    T kY = invDet * (data[1] * data[6] - data[0] * data[7]);
    T kZ = invDet * (data[0] * data[4] - data[1] * data[3]);

    return Matrix3<T>(iX, iY, iZ, jX, jY, jZ, kX, kY, kZ);
}

template<typename T>
bool Matrix3<T>::operator==(const Matrix3& other) const
{
    const T* oData = other.data_;

    return (data_[0] == oData[0]) && (data_[1] == oData[1]) && (data_[2] == oData[2]) && (data_[3] == oData[3]) &&
           (data_[4] == oData[4]) && (data_[5] == oData[5]) && (data_[6] == oData[6]) && (data_[7] == oData[7]) &&
           (data_[8] == oData[8]);
}

template<typename T>
Matrix3<T> Matrix3<T>::Rotate(T angle, const Vector2<T>& pivot)
{
    T s = static_cast<T>(Sin(angle));
    T c = static_cast<T>(Sin(angle + QUARTER_IN_DEGREE));

    Matrix3<T> rotateMat3;
    T* rData = rotateMat3.data_;
    rData[0] = c;
    rData[1] = s;

    rData[3] = -s;
    rData[4] = c;

    rData[6] = pivot.x_ * (1 - c) + pivot.y_ * s;
    rData[7] = pivot.y_ * (1 - c) - pivot.x_ * s;

    rData[8] = 1;

    return rotateMat3;
}

template<typename T>
Matrix3<T> Matrix3<T>::Scale(const Vector2<T>& scale, const Vector2<T>& fixed)
{
    Matrix3<T> scaleMat3;
    T* rData = scaleMat3.data_;
    rData[0] = scale.x_;

    rData[4] = scale.y_;
    rData[6] = fixed.x_ * (1 - scale.x_);
    rData[7] = fixed.y_ * (1 - scale.y_);

    rData[8] = 1;

    return scaleMat3;
}

template<typename T>
Matrix3<T> Matrix3<T>::Translate(const Vector2<T>& trans)
{
    Matrix3 transMat3;
    T* rData = transMat3.data_;

    rData[6] = trans.x_;
    rData[7] = trans.y_;

    return transMat3;
}

/**
 * @brief Defines a 4 x 4 matrix.
 *
 * @since 5.0
 * @version 3.0
 */
template<typename T>
class Matrix4 : public HeapBase {
public:
    /**
     * @brief Defines a <b>Matrix4</b> instance and initializes the 4 x 4 matrix data.
     *
     * @since 5.0
     * @version 3.0
     */
    Matrix4();

    Matrix4(T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13,
            T m20, T m21, T m22, T m23, T m30, T m31, T m32, T m33);

    /**
     * @brief A destructor used to delete the <b>Matrix3</b> instance.
     * @since 5.0
     * @version 3.0
     */
    ~Matrix4() {}

    /**
     * @brief Obtains the 4 x 4 matrix data.
     * @return Returns the 4 x 4 matrix data.
     * @since 5.0
     * @version 3.0
     */
    const T* GetData() const
    {
        return data_;
    }

    /**
     * @brief Obtains the determinant of the matrix.
     * @return Returns the determinant.
     * @since 5.0
     * @version 3.0
     */
    T Determinant() const;

    /**
     * @brief Obtains the inverse matrix.
     * @return Returns the inverse matrix.
     * @since 5.0
     * @version 3.0
     */
    Matrix4<T> Inverse() const;

    /**
     * @brief Obtains a rotation matrix.
     *
     * @param angle Indicates the angle to rotate.
     * @param pivot1 Indicates the rotation pivot.
     * @param pivot2 Indicates the rotation pivot.
     * @return Returns the matrix after rotation.
     * @since 5.0
     * @version 3.0
     */
    static Matrix4<T> Rotate(T angle, const Vector3<T>& pivot1, const Vector3<T>& pivot2);

    /**
     * @brief Obtains the scaling matrix.
     *
     * @param scale Indicates the scaling factors of the x-axis and y-axis.
     * @param fixed Indicates the fixed scaling point.
     * @return Returns the matrix after scaling.
     * @since 5.0
     * @version 3.0
     */
    static Matrix4<T> Scale(const Vector3<T>& scale, const Vector3<T>& fixed);

    /**
     * @brief Obtains a matrix translation.
     *
     * @param trans Indicates the distances to translate along the x-axis and y-axis and z-axis.
     * @return Returns the matrix after translation.
     * @since 5.0
     * @version 3.0
     */
    static Matrix4<T> Translate(const Vector3<T>& trans);

    static Matrix4<T> Shear(const Vector2<T>& shearX, const Vector2<T>& shearY, const Vector2<T>& shearZ);

    Matrix4 operator*(const Matrix4& other) const;

    Vector4<T> operator*(const Vector4<T>& other) const;

    T* operator[](uint8_t col)
    {
        return &data_[col * 4]; // 4: size of point
    }

    Matrix4& operator=(const Matrix4& other)
    {
        // 16: data_ value number
        for (int16_t i = 0; i < 16; i++) {
            data_[i] = other.data_[i];
        }
        return *this;
    }

    bool operator==(const Matrix4& other) const;

protected:
    Matrix3<T> ConomialMatrix(int16_t row, int16_t col) const;
    T data_[16]; // 16: 4 point with x,y,z,w
};

template<typename T>
Matrix4<T>::Matrix4()
{
    for (int16_t row = 0; row < ORDER_MATRIX_4; row++) {
        for (int16_t col = 0; col < ORDER_MATRIX_4; col++) {
            data_[row * ORDER_MATRIX_4 + col] = (row == col) ? 1 : 0;
        }
    }
}

template<typename T>
Matrix4<T>::Matrix4(T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13,
                    T m20, T m21, T m22, T m23, T m30, T m31, T m32, T m33)
{
    data_[0] = m00; // 0 : index
    data_[1] = m01; // 1 : index
    data_[2] = m02; // 2 : index
    data_[3] = m03; // 3 : index

    data_[4] = m10; // 4 : index
    data_[5] = m11; // 5 : index
    data_[6] = m12; // 6 : index
    data_[7] = m13; // 7 : index

    data_[8] = m20; // 8 : index
    data_[9] = m21; // 9 : index
    data_[10] = m22; // 10 : index
    data_[11] = m23; // 11 : index

    data_[12] = m30; // 12 : index
    data_[13] = m31; // 13 : index
    data_[14] = m32; // 14 : index
    data_[15] = m33; // 15 : index
}

template<typename T>
Matrix4<T> Matrix4<T>::operator*(const Matrix4& other) const
{
    Matrix4<T> rMulti;
    T* rData = rMulti.data_;
    const T* oData  = other.data_;
    for (int16_t row = 0; row < ORDER_MATRIX_4; row++) {
        for (int16_t col = 0; col < ORDER_MATRIX_4; col++) {
            /* 1 2 3 4 8 12 : offset */
            rData[row * 4 + col] = oData[row * 4] * data_[col] + oData[row * 4 + 1] * data_[4 + col] +
                                   oData[row * 4 + 2] * data_[8 + col] + oData[row * 4 + 3] * data_[12 + col];
        }
    }
    return rMulti;
}

template<typename T>
Vector4<T> Matrix4<T>::operator*(const Vector4<T>& other) const
{
    Vector4<T> rMulti;
    T* rData = rMulti.data_;
    const T* oData = other.data_;
    for (int16_t i = 0; i < ORDER_MATRIX_4; i++) {
        /* 1 2 3 4 8 12 : offset */
        rData[i] = data_[i] * oData[0] + data_[i + 4] * oData[1] + data_[i + 8] * oData[2] + data_[i + 12] * oData[3];
    }
    return rMulti;
}

template<typename T>
Matrix3<T> Matrix4<T>::ConomialMatrix(int16_t row, int16_t col) const
{
    T tmpData[9]; // 9: 3 * 3
    int index = 0;
    for (int16_t i = 0; i < ORDER_MATRIX_4; i++) {
        if (row == i) {
            continue;
        }
        for (int16_t j = 0; j < ORDER_MATRIX_4; j++) {
            if (col != j) {
                tmpData[index++] = data_[i  * ORDER_MATRIX_4 + j];
            }
        }
    }
    /* 0 1 2 3 4 5 6 7 8 : index */
    return Matrix3<T>(tmpData[0], tmpData[1], tmpData[2],
                      tmpData[3], tmpData[4], tmpData[5],
                      tmpData[6], tmpData[7], tmpData[8]);
}

template<typename T>
T Matrix4<T>::Determinant() const
{
    T tmpData[ORDER_MATRIX_4];
    for (int16_t i = 0; i < ORDER_MATRIX_4; i++) {
        tmpData[i] = ConomialMatrix(0, i).Determinant() * data_[i];
    }
    // 0  1 2 3 : index
    return tmpData[0] - tmpData[1] + tmpData[2] - tmpData[3];
}

template<typename T>
Matrix4<T> Matrix4<T>::Inverse() const
{
    T det = this->Determinant();
    if (det == 0) {
        return Matrix4<T>(*this);
    }
    T invDet = 1.0f / det;
    // algebraic conomial matrix;
    Matrix4<T> matrix;
    T* tmpData = matrix.data_;
    int16_t negative = 1;
    for (int16_t row = 0; row < ORDER_MATRIX_4; row++) {
        for (int16_t col = 0; col < ORDER_MATRIX_4; col++) {
            if ((row + col) % 2 == 0) { // 2 : half
                negative = 1;
            } else {
                negative = -1;
            }
            tmpData[row * ORDER_MATRIX_4 + col] = ConomialMatrix(row, col).Determinant() * negative * invDet;
        }
    }
    // transpose matrix
    for (int16_t row = 0; row < ORDER_MATRIX_4; row++) {
        for (int16_t col = row + 1; col < ORDER_MATRIX_4; col++) {
            T tmp = tmpData[row * ORDER_MATRIX_4 + col];
            tmpData[row * ORDER_MATRIX_4 + col] = tmpData[row +  col * ORDER_MATRIX_4];
            tmpData[row + col * ORDER_MATRIX_4] = tmp;
        }
    }
    return matrix;
}

template<typename T>
bool Matrix4<T>::operator==(const Matrix4& other) const
{
    const T* oData = other.data_;
    // 16 : 4 * 4 points
    for (int16_t i =  0; i < 16; i++) {
        if (data_[i] != oData[i]) {
            return false;
        }
    }
    return true;
}

template<typename T>
Matrix4<T> Matrix4<T>::Rotate(T angle, const Vector3<T>& pivot1, const Vector3<T>& pivot2)
{
    Matrix4<T> rotateMat4;
    T* rData = rotateMat4.data_;
    T length = Sqrt((pivot2.x_ - pivot1.x_) * (pivot2.x_ - pivot1.x_) +
                    (pivot2.y_ - pivot1.y_) * (pivot2.y_ - pivot1.y_) +
                    (pivot2.z_ - pivot1.z_) * (pivot2.z_ - pivot1.z_));
    if (length == 0) {
        return rotateMat4;
    }
    T cosA = static_cast<T>(Sin(angle + QUARTER_IN_DEGREE));
    T sinA = static_cast<T>(Sin(angle));
    T diff = 1 - cosA;
    T ux = (pivot2.x_ - pivot1.x_) / length;
    T uy = (pivot2.y_ - pivot1.y_) / length;
    T uz = (pivot2.z_ - pivot1.z_) / length;
    Matrix4<T> transToOrigin = Matrix4<T>::Translate(Vector3<T> { -pivot1.x_, -pivot1.y_, -pivot1.z_ });

    rData[0] = ux * ux * diff + cosA; // 0 : index
    rData[1] = uy  * ux * diff + uz * sinA; // 1 : index
    rData[2] = uz * ux * diff - uy * sinA; // 2 : index

    rData[4] = ux * uy * diff - uz * sinA; // 4 : index
    rData[5] = uy * uy * diff + cosA; // 5 : index
    rData[6] = uz * uy * diff + ux * sinA; // 6 : index

    rData[8] = ux * uz * diff + uy * sinA; // 8 : index
    rData[9] = uy * uz * diff - ux * sinA; // 9 : index
    rData[10] = uz * uz * diff + cosA; // 10 : index

    Matrix4<T> transToPivot = Matrix4<T>::Translate(pivot1);
    return transToPivot * (rotateMat4 * transToOrigin);
}

template<typename T>
Matrix4<T> Matrix4<T>::Scale(const Vector3<T>& scale, const Vector3<T>& fixed)
{
    Matrix4<T> scaleMat4;
    T* sData = scaleMat4.data_;

    sData[0] = scale.x_; // 0 : index
    sData[5] = scale.y_; // 5 : index
    sData[10] = scale.z_; // 10 : index
    sData[12] = (1 - scale.x_) * fixed.x_; // 12 : index
    sData[13] = (1 - scale.y_) * fixed.y_; // 13 : index
    sData[14] = (1 - scale.z_) * fixed.z_; // 14 : index

    return scaleMat4;
}

template<typename T>
Matrix4<T> Matrix4<T>::Translate(const Vector3<T>& trans)
{
    Matrix4<T> rotateMat4;
    T* rData = rotateMat4.data_;

    rData[12] = trans.x_; // 12 : index
    rData[13] = trans.y_; // 13 : index
    rData[14] = trans.z_; // 14 : index

    return rotateMat4;
}

template<typename T>
Matrix4<T> Matrix4<T>::Shear(const Vector2<T>& shearX, const Vector2<T>& shearY, const Vector2<T>& shearZ)
{
    Matrix4<T> shearMat4;
    shearMat4[1][0] = shearX.x_; // 1 0 : index
    shearMat4[2][0] = shearX.y_; // 2 0 : index
    shearMat4[0][1] = shearY.x_; // 0 1 : index
    shearMat4[2][1] = shearY.y_; // 2 1 : index
    shearMat4[0][2] = shearZ.x_; // 0 2 : index
    shearMat4[1][2] = shearZ.y_; // 1 2 : index
    return shearMat4;
}

inline int64_t FloatToInt64(float f)
{
    if (f > 127.0) { // 127.0: 2^7
        return 0x7FFF;
    }

    if (f < -127.0) { // 127.0: 2^7
        return -0x7FFF;
    }

    // 256.0: 2^8 left move 8 bit. 0.5: round up
    return static_cast<int64_t>(f * 256.0 + ((f < 0) ? -0.5 : 0.5));
}

inline bool FloatEqual(float lhs, float rhs, float precision = 0.00001f)
{
    return (MATH_ABS(lhs - rhs) < precision) ? true : false;
}

bool IsIdentity(Matrix3<float>& matrix);

bool IsIdentity(Matrix4<float>& matrix);
} // namespace OHOS
#endif // GRAPHIC_LITE_GRAPHIC_MATH_H
