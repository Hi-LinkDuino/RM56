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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_PROPERTY_ANIMATABLE_HELPER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_PROPERTY_ANIMATABLE_HELPER_H

#include <functional>

#include "base/geometry/dimension.h"
#include "core/components/common/properties/border.h"
#include "core/components/common/properties/edge.h"
#include "core/components/common/properties/radius.h"

namespace OHOS::Ace {

class BorderEdgeHelper {
public:
    using Setter = void (Border::*)(const BorderEdge&);
    using Getter = const BorderEdge& (Border::*)() const;

    BorderEdgeHelper(Setter setter, Getter getter) : setter_(setter), getter_(getter) {}
    ~BorderEdgeHelper() = default;

    const BorderEdge& Get(const Border& border) const
    {
        return std::bind(getter_, &border)();
    }

    bool Set(double value, Border* border) const
    {
        if (LessNotEqual(value, 0.0)) {
            return false;
        }
        BorderEdge edge = Get(*border);
        Dimension width = edge.GetWidth();
        if (NearEqual(value, width.Value())) {
            return false;
        }
        width.SetValue(value);
        edge.SetWidth(width);
        std::bind(setter_, border, edge)();
        return true;
    }

    bool Set(Color value, Border* border) const
    {
        BorderEdge edge = Get(*border);
        if (edge.GetColor() == value) {
            return false;
        }
        edge.SetColor(value);
        std::bind(setter_, border, edge)();
        return true;
    }

    bool Set(BorderStyle value, Border* border) const
    {
        BorderEdge edge = Get(*border);
        if (edge.GetBorderStyle() == value) {
            return false;
        }
        edge.SetStyle(value);
        std::bind(setter_, border, edge)();
        return true;
    }

private:
    Setter setter_ = nullptr;
    Getter getter_ = nullptr;
};

class BorderRadiusHelper {
public:
    using Setter = void (Border::*)(const Radius&);
    using Getter = const Radius& (Border::*)() const;

    BorderRadiusHelper(Setter setter, Getter getter) : setter_(setter), getter_(getter) {}
    ~BorderRadiusHelper() = default;

    float Get(const Border& border) const
    {
        return std::bind(getter_, &border)().GetX().Value();
    }

    bool Set(double value, Border* border) const
    {
        if (LessNotEqual(value, 0.0)) {
            return false;
        }
        float oldValue = Get(*border);
        if (NearEqual(oldValue, value)) {
            return false;
        }
        std::bind(setter_, border, Radius(value))();
        return true;
    }

private:
    Setter setter_ = nullptr;
    Getter getter_ = nullptr;
};

class DimensionHelper {
public:
    using Getter = const AnimatableDimension& (Edge::*)() const;
    using Setter = void (Edge::*)(const AnimatableDimension&);

    DimensionHelper(Setter setter, Getter getter) : setter_(setter), getter_(getter) {}
    ~DimensionHelper() = default;

    const AnimatableDimension& Get(const Edge& edge) const
    {
        return std::bind(getter_, &edge)();
    }

    bool Set(const AnimatableDimension& value, Edge* edge) const
    {
        if (LessNotEqual(value.Value(), 0.0)) {
            return false;
        }
        AnimatableDimension old = Get(*edge);
        if (old == value) {
            return false;
        }
        std::bind(setter_, edge, value)();
        return true;
    }

private:
    Setter setter_ = nullptr;
    Getter getter_ = nullptr;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_PROPERTY_ANIMATABLE_HELPER_H
