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

#ifndef FOUNDATION_ACE_INTERFACE_INNERKITS_ACE_VIEWPORT_CONFIG_H
#define FOUNDATION_ACE_INTERFACE_INNERKITS_ACE_VIEWPORT_CONFIG_H

#include <string>

namespace OHOS::Ace {

class ViewportConfig {
public:
    ViewportConfig() = default;
    ViewportConfig(int32_t width, int32_t height, float density)
        : width_(width), height_(height), density_(density)
    {}
    ~ViewportConfig() = default;

    void SetSize(int32_t width, int32_t height)
    {
        width_ = width;
        height_ = height;
    }

    void SetPosition(int32_t x, int32_t y)
    {
        posX_ = x;
        posY_ = y;
    }

    int32_t Width() const
    {
        return width_;
    }

    int32_t Height() const
    {
        return height_;
    }

    int32_t Left() const
    {
        return posX_;
    }

    int32_t Top() const
    {
        return posY_;
    }

    void SetDensity(float density)
    {
        density_ = density;
    }

    float Density() const
    {
        return density_;
    }

    void SetOrientation(int32_t orientation)
    {
        orientation_ = orientation;
    }

    int32_t Orientation() const
    {
        return orientation_;
    }

    std::string ToString() const
    {
        std::string config = "Viewport config: ";
        config.append(" size:(" + std::to_string(width_) + ", " + std::to_string(height_) + ")");
        config.append(" orintentation:" + std::to_string(orientation_));
        config.append(" density:" + std::to_string(density_));
        config.append(" position:(" + std::to_string(posX_) + ", " + std::to_string(posY_) + ")");
        return config;
    }

private:
    int32_t width_ = 0;
    int32_t height_ = 0;
    int32_t posX_ = 0;
    int32_t posY_ = 0;
    int32_t orientation_ = 0;
    float density_ = 1.0f;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_INTERFACE_INNERKITS_ACE_VIEWPORT_CONFIG_H
