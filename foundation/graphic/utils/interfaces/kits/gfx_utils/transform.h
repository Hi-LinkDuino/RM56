/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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
 * @file transform.h
 *
 * @brief Provides functions to transform components, points, and line segments, including rotation and scaling.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_TRANSFORM_H
#define GRAPHIC_LITE_TRANSFORM_H

#include "gfx_utils/geometry2d.h"
#include "gfx_utils/graphic_math.h"
namespace OHOS {
/**
 * @brief Transforms a rectangle, including rotation and scaling.
 * @since 1.0
 * @version 1.0
 */
class TransformMap : public HeapBase {
public:
    /**
     * @brief The default constructor used to create a <b>TransformMap</b> instance.
     * @since 1.0
     * @version 1.0
     */
    TransformMap();

    /**
     * @brief A constructor used to create a <b>TransformMap</b> instance.
     *
     * @param rect Indicates the rectangle to transform.
     * @since 1.0
     * @version 1.0
     */
    explicit TransformMap(const Rect& rect);

    /**
     * @brief A destructor used to delete the <b>TransformMap</b> instance.
     * @since 1.0
     * @version 1.0
     */
    virtual ~TransformMap() {}

    /**
     * @brief Checks whether the vertex coordinates of a polygon are clockwise.
     *
     * @return Returns <b>true</b> if the vertex coordinates are clockwise; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool GetClockWise() const;

    /**
     * @brief Sets a polygon after rectangle transformation.
     * @param polygon Indicates the polygon to set.
     * @since 1.0
     * @version 1.0
     */
    void SetPolygon(const Polygon& polygon)
    {
        polygon_ = polygon;
    }

    /**
     * @brief Obtains the polygon after rectangle transformation.
     * @return Returns the polygon.
     * @since 1.0
     * @version 1.0
     */
    Polygon GetPolygon() const
    {
        return polygon_;
    }

    /**
     * @brief Obtains the pivot for the rotation or scaling operation.
     * @return Returns the pivot.
     * @since 1.0
     * @version 1.0
     */
    const Vector3<float>& GetPivot() const
    {
        return scalePivot_;
    }

    /**
     * @brief Obtains the pivot for the rotation or scaling operation.
     * @return Returns the pivot.
     * @since 1.0
     * @version 1.0
     */
    const Vector3<float>& GetRotatePivot() const
    {
        return rotatePivotStart_;
    }

    void SetTransMapRect(const Rect& rect);

    const Rect& GetTransMapRect() const
    {
        return rect_;
    }

    void SetInvalid(bool state)
    {
        isInvalid_ = state;
    }

    /**
     * @brief Checks whether the <b>TransformMap</b> instance is invalid. When the vertices are all 0, the
     *        <b>TransformMap</b> is invalid.
     * @return Returns <b>true</b> if <b>TransformMap</b> is invalid; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool IsInvalid() const;

    /**
     * @brief Obtains the minimum rectangle that can contain a polygon. All vertices of the polygon are inside this
     *        rectangle.
     * @return Returns the minimum rectangle that can contain the polygon.
     * @since 1.0
     * @version 1.0
     */
    Rect GetBoxRect() const
    {
        return polygon_.MakeAABB();
    }

    /**
     * @brief Obtains a three-dimensional homogeneous transformation matrix.
     * @return Returns the three-dimensional homogeneous transformation matrix.
     * @since 1.0
     * @version 1.0
     */
    const Matrix4<float>& GetTransformMatrix() const
    {
        return matrix_;
    }

    const Matrix4<float>& GetOrigTransformMatrix() const
    {
        return matrixOrig_;
    }

    const Matrix4<float>& GetRotateMatrix() const
    {
        return rotate_;
    }

    const Matrix4<float>& GetScaleMatrix() const
    {
        return scale_;
    }

    const Matrix4<float>& GetTranslateMatrix() const
    {
        return translate_;
    }

    const Matrix4<float>& GetShearMatrix() const
    {
        return shear_;
    }

    const Matrix4<float>& GetPerspectiveMatrix() const
    {
        return perspectiveMatrix_;
    }

    int16_t GetRotateAngle() const
    {
        return angle_;
    }

    void SetRotateAngle(int16_t angle)
    {
        this->angle_=angle;
    }

    /**
     * @brief Rotates the rectangle.
     * @param angle Indicates the angle to rotate.
     * @param pivot Indicates the rotation pivot.
     * @since 1.0
     * @version 1.0
     */
    void Rotate(int16_t angle, const Vector2<float>& pivot);

    void Rotate(int16_t angle, const Vector3<float>& rotatePivotStart, const Vector3<float>& rotatePivotEnd);

    /**
     * @brief Scales the rectangle.
     *
     * @param scale Indicates the scaling factors of the x-axis and y-axis.
     * @param pivot Indicates the pivot for scaling.
     * @since 1.0
     * @version 1.0
     */
    void Scale(const Vector2<float>& scale, const Vector2<float>& pivot);

    void Scale(const Vector3<float>& scale, const Vector3<float>& pivot);

    void Translate(const Vector2<int16_t>& trans);

    void Translate(const Vector3<int16_t>& trans);

    void Shear(const Vector2<float>& shearX, const Vector2<float>& shearY, const Vector2<float>& shearZ);

    bool operator==(const TransformMap& other) const;

    void SetMatrix(const Matrix4<float>& matrix, bool isInternalMatrix = false);

    void SetCameraDistance(int16_t distance);

    void SetCameraPosition(const Vector2<float>& position);

    Matrix3<float> invMatrix_;

    bool Is3DTransform() const;

private:
    void UpdateMap();
    void AddOp(uint8_t op);

    enum : uint8_t {
        ROTATE = 0,
        SCALE,
        SHEAR,
        TRANSLATE,
        TRANS_NUM,
    };
    Rect rect_ = {0, 0, 0, 0};       /* orig rect */
    Polygon polygon_ = Polygon(Rect(0, 0, 0, 0)); /* transformed from rect and 'rotate_' 'translate_' 'scale_' */
    int16_t angle_ = 0;
    int16_t cameraDistance_ = 1000;  // 1000 : default distance
    bool isInvalid_ = false;
    bool isIdentity_ = false;
    bool is3d_ = false;
    bool isInternalMatrix_ = true;
    Vector2<float> cameraPosition_ = {0, 0};
    Vector3<float> scaleCoeff_ = {1.0f, 1.0f, 1.0f};
    Vector3<float> scalePivot_ = {0, 0, 0};
    Vector3<float> rotatePivotStart_ = {0, 0, 0};
    Vector3<float> rotatePivotEnd_ = {0, 0, 0};
    Vector2<float> shearX_ = {0, 0};
    Vector2<float> shearY_ = {0, 0};
    Vector2<float> shearZ_ = {0, 0};

    Matrix4<float> scale_;
    Matrix4<float> rotate_;
    Matrix4<float> shear_;
    Matrix4<float> translate_;
    Matrix4<float>* trans_[TRANS_NUM];
    uint8_t opOrder_[TRANS_NUM];
    Matrix4<float> matrix_;
    Matrix4<float> perspectiveMatrix_;
    Matrix4<float> matrixOrig_;
};

/**
 * @brief Rotates a point around the pivot by a certain angle.
 * @param point Indicates the point to rotate.
 * @param angle Indicates the angle to rotate.
 * @param pivot Indicates the rotation pivot.
 * @param out Indicates the point generated after rotation.
 * @since 1.0
 * @version 1.0
 */
void Rotate(const Vector2<int16_t>& point, int16_t angle, const Vector2<int16_t>& pivot, Vector2<int16_t>& out);

/**
 * @brief Rotates a line around the pivot by a certain angle.
 * @param origLine Indicates the line segment to rotate.
 * @param angle Indicates the angle to rotate.
 * @param pivot Indicates the rotation pivot.
 * @param out Indicates the line generated after rotation.
 * @since 1.0
 * @version 1.0
 */
void Rotate(const Line& origLine, int16_t angle, const Vector2<int16_t>& pivot, Line& out);

/**
 * @brief Rotates a rectangle around the pivot by a certain angle.
 * @param origRect Indicates the rectangle to rotate.
 * @param angle Indicates the angle to rotate.
 * @param pivot Indicates the rotation pivot.
 * @param out Indicates the polygon generated after the rectangle is rotated.
 * @since 1.0
 * @version 1.0
 */
void Rotate(const Rect& origRect, int16_t angle, const Vector2<int16_t>& pivot, Polygon& out);
} // namespace OHOS
#endif // GRAPHIC_LITE_TRANSFORM_H
