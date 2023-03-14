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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_SELECT_COMPOSED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_SELECT_COMPOSED_ELEMENT_H

#include "core/components/select/render_select.h"
#include "core/components_v2/inspector/inspector_composed_element.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT SelectComposedElement : public InspectorComposedElement {
    DECLARE_ACE_TYPE(SelectComposedElement, InspectorComposedElement)

public:
    explicit SelectComposedElement(const ComposeId& id) : InspectorComposedElement(id) {}
    ~SelectComposedElement() override = default;

    void Dump(void) override;
    std::unique_ptr<OHOS::Ace::JsonValue> ToJsonObject() const override;

    std::string GetOptions(void) const;
    std::string GetSelected(void) const;
    std::string GetSelectValue(void) const;
    std::string GetFont(void) const;
    std::string GetFontColor(void) const;
    std::string GetSelectBgColor(void) const;
    std::string GetSelectFont(void) const;
    std::string GetSelectFontColor(void) const;
    std::string GetOptionBgColor(void) const;
    std::string GetOptionFont(void) const;
    std::string GetOptionFontColor(void) const;

private:
    OHOS::Ace::RefPtr<OHOS::Ace::RenderSelect> GetRenderSelect() const;
    std::string ConvertFontFamily(const std::vector<std::string>& fontFamily) const;
    std::string GetTextStyle(TextStyle textStyle) const;
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_SELECT_COMPOSED_ELEMENT_H