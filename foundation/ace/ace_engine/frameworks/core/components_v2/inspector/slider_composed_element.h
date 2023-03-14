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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_SLIDER_COMPOSED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_SLIDER_COMPOSED_ELEMENT_H

#include "core/components/slider/render_slider.h"
#include "core/components_v2/inspector/inspector_composed_element.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT SliderComposedElement : public InspectorComposedElement {
    DECLARE_ACE_TYPE(SliderComposedElement, InspectorComposedElement)

public:
    explicit SliderComposedElement(const ComposeId& id) : InspectorComposedElement(id) {}
    ~SliderComposedElement() override = default;

    void Dump() override;
    std::unique_ptr<JsonValue> ToJsonObject() const override;
    std::unique_ptr<JsonValue> GetConstructor() const;

    std::string GetValue() const;
    std::string GetMax() const;
    std::string GetMin() const;
    std::string GetStep() const;
    std::string GetStyle() const;
    std::string GetReverse() const;
    std::string GetThickness() const;
    std::string GetBlockColor() const;
    std::string GetTrackColor() const;
    std::string GetSelectedColor() const;
    std::string GetShowSteps() const;
    std::string GetShowTips() const;
    std::string GetSliderDirection() const;


private:
    RefPtr<RenderSlider> GetRenderSlider() const;
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_SLIDER_COMPOSED_ELEMENT_H