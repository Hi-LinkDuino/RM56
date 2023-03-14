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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_RATING_COMPOSED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_RATING_COMPOSED_ELEMENT_H

#include "core/components/rating/render_rating.h"
#include "core/components_v2/inspector/inspector_composed_element.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT RatingComposedElement : public InspectorComposedElement {
    DECLARE_ACE_TYPE(RatingComposedElement, InspectorComposedElement)

public:
    explicit RatingComposedElement(const ComposeId& id) : InspectorComposedElement(id) {}
    ~RatingComposedElement() override = default;

    void Dump() override;
    std::unique_ptr<JsonValue> ToJsonObject() const override;

    std::string GetRating() const;
    std::string GetIndicator() const;
    std::string GetStars() const;
    std::string GetStepSize() const;
    std::string GetStarStyle() const;

private:
    RefPtr<RenderRating> GetRenderRating() const;
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_Rating_COMPOSED_ELEMENT_H