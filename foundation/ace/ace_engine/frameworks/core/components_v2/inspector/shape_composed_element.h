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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_SHAPE_COMPOSED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_SHAPE_COMPOSED_ELEMENT_H

#include "core/components/shape/render_shape.h"
#include "core/components/shape/shape_element.h"
#include "core/components_v2/inspector/inspector_composed_element.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT ShapeComposedElement : public InspectorComposedElement {
    DECLARE_ACE_TYPE(ShapeComposedElement, InspectorComposedElement)
public:
    explicit ShapeComposedElement(const ComposeId& id) : InspectorComposedElement(id) {}
    ~ShapeComposedElement() override = default;
    void Dump() override;
    std::unique_ptr<JsonValue> ToJsonObject() const override;
    std::string GetWidth() const override;
    std::string GetHeight() const override;

    std::string GetShapeType() const;
    std::string GetCommands() const;
    std::string GetAntiAlias() const;
    std::string GetFill() const;
    std::string GetFillOpacity() const;
    std::string GetStroke() const;
    std::string GetStrokeDashOffset() const;
    std::string GetStrokeLineCap() const;
    std::string GetStrokeLineJoin() const;
    std::string GetStrokeMiterLimit() const;
    std::string GetStrokeOpacity() const;
    std::string GetStrokeWidth() const;
    std::unique_ptr<JsonValue> GetRadiusArray() const;
    std::unique_ptr<JsonValue> GetRadiusHeightArray() const;
    std::unique_ptr<JsonValue> GetRadiusWidthArray() const;
    std::unique_ptr<JsonValue> GetStrokeDashArray() const;
    std::unique_ptr<JsonValue> GetStartPointArray() const;
    std::unique_ptr<JsonValue> GetEndPointArray() const;
    std::unique_ptr<JsonValue> GetPointsArray() const;

    AceType::IdType GetTargetTypeId() const override
    {
        return ShapeElement::TypeId();
    }
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_SHAPE_COMPOSED_ELEMENT_H