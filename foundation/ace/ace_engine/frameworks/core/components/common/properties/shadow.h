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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_SHADOW_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_SHADOW_H

#include "base/geometry/offset.h"
#include "core/components/common/properties/color.h"

namespace OHOS::Ace {

constexpr float LIGHT_HEIGHT = 600.0f; // System recommended value.
constexpr float LIGHT_RADIUS = 800.0f; // System recommended value.

// A style class indicates the way to render shadow effect
class Shadow final {
public:
    static Shadow Blend(const Shadow& to, const Shadow& from, float progress);

    Shadow() = default;
    ~Shadow() = default;

    // create shadow for hardware rending.
    Shadow(float elevation, Offset offset, Color spotColor) : offset_(offset), color_(spotColor)
    {
        SetElevation(elevation);
    };

    // create shadow for software rending.
    Shadow(double blurRadius, double spreadRadius, Offset offset, Color spotColor)
        : spreadRadius_(spreadRadius), offset_(offset), color_(spotColor)
    {
        SetBlurRadius(blurRadius);
    };

    bool operator==(const Shadow& rhs) const
    {
        return color_ == rhs.color_ && NearEqual(blurRadius_, rhs.blurRadius_) && offset_ == rhs.offset_ &&
               NearEqual(spreadRadius_, rhs.spreadRadius_) && NearEqual(elevation_, rhs.elevation_);
    }

    bool operator!=(const Shadow& rhs) const
    {
        return !(rhs == *this);
    }

    void SetColor(const Color& newColor)
    {
        color_ = newColor;
    }

    const Color& GetColor() const
    {
        return color_;
    }

    void SetBlurRadius(double blurRadius)
    {
        if (blurRadius >= 0.0) {
            blurRadius_ = blurRadius;
            isHardwareAcceleration_ = false;
            return;
        }
        blurRadius_ = 0.0;
    }

    double GetBlurRadius() const
    {
        return blurRadius_;
    }

    void SetOffset(const Offset& offset)
    {
        offset_ = offset;
    }

    const Offset& GetOffset() const
    {
        return offset_;
    }

    void SetOffsetX(double x)
    {
        offset_.SetX(x);
    }

    void SetOffsetY(double y)
    {
        offset_.SetY(y);
    }

    void SetSpreadRadius(double spreadRadius)
    {
        spreadRadius_ = spreadRadius;
        isHardwareAcceleration_ = false;
    }

    double GetSpreadRadius() const
    {
        return spreadRadius_;
    }

    void SetElevation(float elevation)
    {
        if (elevation >= 0.0f && elevation < LIGHT_HEIGHT) {
            elevation_ = elevation;
            isHardwareAcceleration_ = true;
            return;
        }
        elevation_ = 0.0f;
    }

    float GetElevation() const
    {
        return elevation_;
    }

    void SetHardwareAcceleration(bool acceleration)
    {
        isHardwareAcceleration_ = acceleration;
    }

    bool GetHardwareAcceleration() const
    {
        return isHardwareAcceleration_;
    }

    void SetLightHeight(float lightHeight)
    {
        if (lightHeight > 0.0f) {
            lightHeight_ = lightHeight;
        }
    }

    float GetLightHeight() const
    {
        return lightHeight_;
    }

    void SetLightRadius(float lightRadius)
    {
        if (lightRadius > 0.0f) {
            lightRadius_ = lightRadius;
        }
    }

    float GetLightRadius() const
    {
        return lightRadius_;
    }

    bool IsValid() const
    {
        if (isHardwareAcceleration_) {
            return elevation_ > 0.0f && elevation_ < LIGHT_HEIGHT;
        } else {
            return blurRadius_ > 0.0 || spreadRadius_ > 0.0 || offset_ != Offset::Zero();
        }
    }

private:
    float lightHeight_ = LIGHT_HEIGHT;
    float lightRadius_ = LIGHT_RADIUS;
    float elevation_ = 0.0f; // The default scope:0.0 ~ LIGHT_HEIGHT.
    double blurRadius_ = 0.0;
    double spreadRadius_ = 0.0;
    Offset offset_;
    Color color_ = Color::BLACK;
    bool isHardwareAcceleration_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_SHADOW_H
