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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_IMAGE_ANIMATOR_COMPOSED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_IMAGE_ANIMATOR_COMPOSED_ELEMENT_H

#include "core/components/image/image_animator_component.h"
#include "core/components/image/image_animator_element.h"
#include "core/components_v2/inspector/inspector_composed_element.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT ImageAnimatorComposedElement : public InspectorComposedElement {
    DECLARE_ACE_TYPE(ImageAnimatorComposedElement, InspectorComposedElement)
public:
    explicit ImageAnimatorComposedElement(const ComposeId& id) : InspectorComposedElement(id) {}
    ~ImageAnimatorComposedElement() override = default;

    void Dump() override;
    std::unique_ptr<JsonValue> ToJsonObject() const override;
    std::string GetStatus() const;
    std::string GetDuration() const;
    std::string GetReverse() const;
    std::string GetFixedSize() const;
    std::string GetPreDecode() const;
    std::string GetFillMode() const;
    std::string GetIteration() const;
    std::string GetImages() const;

    AceType::IdType GetTargetTypeId() const override
    {
        return ImageAnimatorElement::TypeId();
    }
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_IMAGE_ANIMATOR_COMPOSED_ELEMENT_H