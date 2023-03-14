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

#include "core/components/search/render_search.h"
#include "core/components_v2/inspector/inspector_composed_element.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT SearchComposedElement : public InspectorComposedElement {
    DECLARE_ACE_TYPE(SearchComposedElement, InspectorComposedElement)

public:
    explicit SearchComposedElement(const ComposeId& id) : InspectorComposedElement(id) {}
    ~SearchComposedElement() override = default;

    void Dump(void) override;
    std::unique_ptr<OHOS::Ace::JsonValue> ToJsonObject() const override;
    std::string GetIcon(void) const;
    std::string GetSearchButton(void) const;

private:
    OHOS::Ace::RefPtr<OHOS::Ace::RenderSearch> GetRenderSearch() const;
};

}