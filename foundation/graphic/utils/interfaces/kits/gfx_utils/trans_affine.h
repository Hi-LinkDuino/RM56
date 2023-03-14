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
#ifndef GRAPHIC_LTE_TRANS_AFFINE_H
#define GRAPHIC_LTE_TRANS_AFFINE_H

#include <cmath>

#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/graphic_math.h"
namespace OHOS {
const float affineEpsilon = 1e-14;
const uint16_t parlIndexSize = 6;
/**
 * @brief Map source transformation
 * @since 1.0
 * @version 1.0
 */
class TransAffine : public Matrix3<float> {
public:
    /**
     * @brief Initialize to identity matrix
     * @since 1.0
     * @version 1.0
     */
    TransAffine() : Matrix3<float>() {}
    /**
     * @brief Custom matrix
     * @since 1.0
     * @version 1.0
     */
    TransAffine(float v0, float v1, float v2, float v3, float v4, float v5)
        : Matrix3<float>(v0, v2, v4, v1, v3, v5, 0, 0, 1) {}
    /**
     * @brief Converts a rectangle to a parallelogram
     * @since 1.0
     * @version 1.0
     */
    TransAffine(float x1, float y1, float x2, float y2, const float* parl)
    {
        RectToParl(x1, y1, x2, y2, parl);
    }
    void SetData(int32_t index, float value)
    {
        data_[index] = value;
    }
    /**
     * @brief Convert the original parallelogram to the target parallelogram
     * @param src Original parallelogram
     * @param dst Target parallelogram
     * @since 1.0
     * @version 1.0
     */
    const TransAffine& ParlToParl(const float* src, const float* dst);
    /**
     * @brief Convert the original parallelogram to the target parallelogram
     * @param x1 x-axis coordinate of the upper left corner of the rectangle
     * @param y1 y-axis coordinate of the upper left corner of the rectangle
     * @param x2 x-axis coordinates of the lower right corner of the rectangle
     * @param y2 y-axis coordinates of the lower right corner of the rectangle
     * @param parl Target parallelogram
     * @since 1.0
     * @version 1.0
     */
    const TransAffine& RectToParl(float x1, float y1,
                                  float x2, float y2,
                                  const float* parl);

    /**
     * @brief reset TransAffine
     * @since 1.0
     * @version 1.0
     */
    const TransAffine& Reset();

    /**
     * @brief Element offset
     * @param deltaX x-axis offset
     * @param deltaY y-axis offset
     * @since 1.0
     * @version 1.0
     */
    const TransAffine& Translate(float x, float y);
    /**
     * @brief Element rotation
     * @param angle Rotate angle
     * @since 1.0
     * @version 1.0
     */
    const TransAffine& Rotate(float angle);
    /**
     * @brief Element Scale
     * @param scale Overall amplification factor
     * @since 1.0
     * @version 1.0
     */
    const TransAffine& Scale(float scale);
    /**
     * @brief Element Scale
     * @param x x-axis magnification factor
     * @param y y-axis magnification factor
     * @since 1.0
     * @version 1.0
     */
    const TransAffine& Scale(float x, float y);

    /**
     * @brief Transformation matrix multiplication
     * @param metrix
     * @since 1.0
     * @version 1.0
     */
    const TransAffine& Multiply(const TransAffine& metrix);

    /**
     * @brief Matrix flip
     * @since 1.0
     * @version 1.0
     */
    const TransAffine& Invert();

    const TransAffine& operator*=(const TransAffine& metrix)
    {
        return Multiply(metrix);
    }

    /**
     * @brief Convert x, y
     * @param x x-coordinate
     * @param y y-coordinate
     * @since 1.0
     * @version 1.0
     */
    void Transform(float* x, float* y) const;

    /**
     * @brief inverse transformation
     * @param x x-coordinate
     * @param y y-coordinate
     * @since 1.0
     * @version 1.0
     */
    void InverseTransform(float* x, float* y) const;

    /**
     * @brief Computes the reciprocal of a determinant
     * @since 1.0
     * @version 1.0
     */
    float DeterminantReciprocal() const
    {
        float reciprocal = data_[0] * data_[4] - data_[3] * data_[1];
        if (reciprocal == 0) {
            return VERTEX_DIST_EPSILON;
        }
        return 1.0 / reciprocal;
    }

    /**
     * @brief Check the matrix for degradation
     * @since 1.0
     * @version 1.0
     */
    bool IsValid(float epsilon = affineEpsilon) const;

    /**
     * @brief Check whether it is an identity matrix
     * @since 1.0
     * @version 1.0
     */
    bool IsIdentity() const;

    /**
     * @brief zoom
     * @param x x-coordinate
     * @param y y-coordinate
     * @since 1.0
     * @version 1.0
     */
    void ScalingAbs(float* x, float* y) const;

    /**
     * @brief Set rotation matrix
     * @since 1.0
     * @version 1.0
     */
    static TransAffine TransAffineRotation(float angle)
    {
        return TransAffine(std::cos(angle), std::sin(angle), -std::sin(angle), std::cos(angle), 0.0f, 0.0f);
    }

    /**
     * @brief Set scaling matrix
     * @since 1.0
     * @version 1.0
     */
    static TransAffine TransAffineScaling(float x, float y)
    {
        return TransAffine(x, 0.0f, 0.0f, y, 0.0f, 0.0f);
    }

    /**
     * @brief Set scaling matrix
     * @since 1.0
     * @version 1.0
     */
    static TransAffine TransAffineScaling(float scale)
    {
        return TransAffine(scale, 0.0f, 0.0f, scale, 0.0f, 0.0f);
    }
    /**
     * @brief Set translation matrix
     * @since 1.0
     * @version 1.0
     */
    static TransAffine TransAffineTranslation(float x,float y)
    {
        return TransAffine(1.0f, 0.0f, 0.0f, 1.0f, x, y);
    }
};

inline void TransAffine::Transform(float* x, float* y) const
{
    float tmp = *x;
    *x = tmp * data_[0] + *y * data_[1] + data_[2];
    *y = tmp * data_[3] + *y * data_[4] + data_[5];
}

inline void TransAffine::InverseTransform(float* x, float* y) const
{
    float reciprocal = DeterminantReciprocal();
    float a = (*x - data_[2]) * reciprocal;
    float b = (*y - data_[5]) * reciprocal;
    *x = a * data_[4] - b * data_[1];
    *y = b * data_[0] - a * data_[3];
}

inline const TransAffine& TransAffine::Translate(float deltaX, float deltaY)
{
    data_[2] += deltaX;
    data_[5] += deltaY;
    return *this;
}

inline const TransAffine& TransAffine::Rotate(float angle)
{
    float scaleXTemp = data_[0] * std::cos(angle) - data_[3] * std::sin(angle);
    float shearXTemp = data_[1] * std::cos(angle) - data_[4] * std::sin(angle);
    float translateXTemp = data_[2] * std::cos(angle) - data_[5] * std::sin(angle);
    data_[3] = data_[0] * std::sin(angle) + data_[3] * std::cos(angle);
    data_[4] = data_[1] * std::sin(angle) + data_[4] * std::cos(angle);
    data_[5] = data_[2] * std::sin(angle) + data_[5] * std::cos(angle);
    data_[0] = scaleXTemp;
    data_[1] = shearXTemp;
    data_[2] = translateXTemp;
    return *this;
}

inline const TransAffine& TransAffine::Scale(float scaleX, float scaleY)
{
    data_[0] *= scaleX;
    data_[1] *= scaleX;
    data_[2] *= scaleX;
    data_[3] *= scaleY;
    data_[4] *= scaleY;
    data_[5] *= scaleY;
    return *this;
}

inline const TransAffine& TransAffine::Scale(float scale)
{
    data_[0] *= scale;
    data_[1] *= scale;
    data_[2] *= scale;
    data_[3] *= scale;
    data_[4] *= scale;
    data_[5] *= scale;
    return *this;
}

inline void TransAffine::ScalingAbs(float* x, float* y) const
{
    *x = Sqrt(data_[0] * data_[0] + data_[1] * data_[1]);
    *y = Sqrt(data_[3] * data_[3] + data_[4] * data_[4]);
}
} // namespace OHOS
#endif
