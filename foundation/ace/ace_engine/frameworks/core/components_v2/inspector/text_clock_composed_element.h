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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_TEXTCLOCK_COMPOSED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_TEXTCLOCK_COMPOSED_ELEMENT_H

#include "core/components/text_clock/render_text_clock.h"
#include "core/components/text/render_text.h"
#include "core/components/text_clock/text_clock_element.h"
#include "core/components_v2/inspector/inspector_composed_element.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT TextClockComposedElement : public InspectorComposedElement {
    DECLARE_ACE_TYPE(TextClockComposedElement, InspectorComposedElement)

public:
    explicit TextClockComposedElement(const ComposeId& id) : InspectorComposedElement(id) {}
    ~TextClockComposedElement() override = default;

    void Dump() override;
    std::unique_ptr<JsonValue> ToJsonObject() const override;

    std::string GetTimeZoneOffset() const;
    std::string GeFormat() const;

    std::string GetTextFontColor() const;
    std::string GetTextFontStyle() const;
    std::string GetTextFontWeight() const;
    std::string GetTextFontFamily() const;
    std::string GetTextFontSize() const;

    AceType::IdType GetTargetTypeId() const override
    {
        return TextClockElement::TypeId();
    }

private:
    RefPtr<RenderTextClock> GetRenderTextClock() const;
    RefPtr<RenderText> GetRenderText() const;
    std::string ConvertFontFamily(const std::vector<std::string>& fontFamily) const;
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_TEXTCLOCK_COMPOSED_ELEMENT_H