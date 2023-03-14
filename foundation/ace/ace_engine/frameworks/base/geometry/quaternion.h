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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_QUATERNION_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_QUATERNION_H

namespace OHOS::Ace {

class Quaternion {
public:
    Quaternion() = default;
    Quaternion(double x, double y, double z, double w) : x_(x), y_(y), z_(z), w_(w) {}
    ~Quaternion() = default;

    double GetX() const
    {
        return x_;
    }
    double GetY() const
    {
        return y_;
    }
    double GetZ() const
    {
        return z_;
    }
    double GetW() const
    {
        return w_;
    }
    void SetX(double x)
    {
        x_ = x;
    }
    void SetY(double y)
    {
        y_ = y;
    }
    void SetZ(double z)
    {
        z_ = z;
    }
    void SetW(double w)
    {
        w_ = w;
    }

    Quaternion operator+(const Quaternion& q) const
    {
        auto x = this->x_ + q.x_;
        auto y = this->y_ + q.y_;
        auto z = this->z_ + q.z_;
        auto w = this->w_ + q.w_;
        return Quaternion(x, y, z, w);
    }

    Quaternion operator*(const Quaternion& q) const
    {
        auto x = w_ * q.x_ + x_ * q.w_ + y_ * q.z_ - z_ * q.y_;
        auto y = w_ * q.y_ - x_ * q.z_ + y_ * q.w_ + z_ * q.x_;
        auto z = w_ * q.z_ + x_ * q.y_ - y_ * q.x_ + z_ * q.w_;
        auto w = w_ * q.w_ - x_ * q.x_ - y_ * q.y_ - z_ * q.z_;
        return Quaternion(x, y, z, w);
    }

    Quaternion inverse() const
    {
        return { -x_, -y_, -z_, w_ };
    }

    Quaternion flip() const
    {
        return { -x_, -y_, -z_, -w_ };
    }

    // Blends with the given quaternion, |q|, via spherical linear interpolation.
    // Values of |t| in the range [0, 1] will interpolate between |this| and |q|,
    // and values outside that range will extrapolate beyond in either direction.
    Quaternion Slerp(const Quaternion& q, double t) const;

private:
    double x_ = 0.0;
    double y_ = 0.0;
    double z_ = 0.0;
    double w_ = 0.0;
};

// |s| is an arbitrary, real constant.
inline Quaternion operator*(const Quaternion& q, double s)
{
    return Quaternion(q.GetX() * s, q.GetY() * s, q.GetZ() * s, q.GetW() * s);
}

// |s| is an arbitrary, real constant.
inline Quaternion operator*(double s, const Quaternion& q)
{
    return Quaternion(q.GetX() * s, q.GetY() * s, q.GetZ() * s, q.GetW() * s);
}

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_QUATERNION_H
