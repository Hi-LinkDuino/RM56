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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_VELOCITY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_VELOCITY_H

#include "base/geometry/offset.h"

namespace OHOS::Ace {

class Velocity final {
public:
    Velocity() = default;
    explicit Velocity(const Offset& offsetPerSecond) : offsetPerSecond_(offsetPerSecond) {}
    ~Velocity() = default;

    void Reset()
    {
        offsetPerSecond_.Reset();
    }

    void SetOffsetPerSecond(const Offset& offsetPerSecond)
    {
        offsetPerSecond_ = offsetPerSecond;
    }

    Velocity operator+(const Velocity& velocity) const
    {
        return Velocity(offsetPerSecond_ + velocity.offsetPerSecond_);
    }

    Velocity operator-(const Velocity& velocity) const
    {
        return Velocity(offsetPerSecond_ - velocity.offsetPerSecond_);
    }

    Velocity operator*(double value) const
    {
        return Velocity(offsetPerSecond_ * value);
    }

    Velocity& operator+=(const Velocity& velocity)
    {
        offsetPerSecond_ += velocity.offsetPerSecond_;
        return *this;
    }

    bool operator==(const Velocity& velocity) const
    {
        return offsetPerSecond_ == velocity.offsetPerSecond_;
    }

    bool operator!=(const Velocity& velocity) const
    {
        return !operator==(velocity);
    }

    const Offset& GetOffsetPerSecond() const
    {
        return offsetPerSecond_;
    }

    double GetVelocityX() const
    {
        return offsetPerSecond_.GetX();
    }

    double GetVelocityY() const
    {
        return offsetPerSecond_.GetY();
    }

    double GetVelocityValue() const
    {
        return offsetPerSecond_.GetDistance();
    }

private:
    Offset offsetPerSecond_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_VELOCITY_H
