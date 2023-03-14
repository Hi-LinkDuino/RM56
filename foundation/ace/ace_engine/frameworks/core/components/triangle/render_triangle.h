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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRIANGLE_RENDER_TRIANGLE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRIANGLE_RENDER_TRIANGLE_H

#include "base/geometry/arc.h"
#include "core/gestures/click_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class TriangleRotationCalculator {

public:
    void SetPointOne(double x, double y)
    {
        x1_ = x;
        y1_ = y;
    }

    void SetPointTwo(double x, double y)
    {
        x2_ = x;
        y2_ = y;
    }

    void SetPointThree(double x, double y)
    {
        x3_ = x;
        y3_ = y;
    }

    void SetAngle(double value)
    {
        angle_ = value;
    }

    void SetRadius(double value)
    {
        radius_ = value;
    }

    bool Calculate(double xOffset, double yOffset);

    bool IsDown() const;

    const Arc& GetOutArc1()
    {
        return outArc1_;
    }

    const Arc& GetOutArc2()
    {
        return outArc2_;
    }

    const Arc& GetOutArc3()
    {
        return outArc3_;
    }

private:
    // point one
    double x1_ = 0.0;
    double y1_ = 0.0;
    // point two
    double x2_ = 0.0;
    double y2_ = 0.0;
    // point three
    double x3_ = 0.0;
    double y3_ = 0.0;
    // rotate angle
    double angle_ = 0.0;
    // radius value
    double radius_ = 0.0;
    // out arc 1
    Arc outArc1_;
    // out arc 2
    Arc outArc2_;
    // out arc 3
    Arc outArc3_;
};

class RenderTriangle : public RenderNode {
    DECLARE_ACE_TYPE(RenderTriangle, RenderNode);

public:
    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

    const std::function<void(bool)>& GetOnClick() const
    {
        return onClick_;
    }
    void SetOnClick(const std::function<void(bool)>& value)
    {
        onClick_ = value;
    }

    void UpdateAngle(double value)
    {
        data_.SetAngle(value);
        MarkNeedRender();
    }

    void OnTouchTestHit(const Offset&, const TouchRestrict&, TouchTestResult& result) override;

protected:
    void CalculateTrianglePoints();
    void HandleClick();

    Dimension width_;
    Dimension height_;
    Dimension padding_;
    Color color_;

    TriangleRotationCalculator data_;

private:
    RefPtr<ClickRecognizer> click_;
    std::function<void(bool)> onClick_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRIANGLE_RENDER_TRIANGLE_H
