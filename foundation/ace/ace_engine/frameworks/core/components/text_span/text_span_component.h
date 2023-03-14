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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_SPAN_TEXT_SPAN_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_SPAN_TEXT_SPAN_COMPONENT_H

#include "core/components/common/properties/text_style.h"
#include "core/components/text_span/text_span_element.h"
#include "core/pipeline/base/component_group.h"
#include "core/components/declaration/span/span_declaration.h"

namespace OHOS::Ace {

class ACE_EXPORT TextSpanComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(TextSpanComponent, ComponentGroup);

public:
    explicit TextSpanComponent(const std::string& spanData);
    ~TextSpanComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    const std::string& GetSpanData() const;
    void SetSpanData(const std::string& data);

    const TextStyle& GetTextStyle() const;
    void SetTextStyle(const TextStyle& spanStyle);

    bool IsShow() const;
    void SetIsShow(bool isShow);

    void SetOnClick(const EventMarker& onClick);
    void SetRemoteMessageEventId(const EventMarker& eventId);

    bool HasNewStyle() const
    {
        return hasNewStyle_;
    }

    const RefPtr<SpanDeclaration>& GetDeclaration() const;
    void SetDeclaration(const RefPtr<SpanDeclaration>& declaration);

private:
    bool hasNewStyle_ = false;
    RefPtr<SpanDeclaration> declaration_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_SPAN_TEXT_SPAN_COMPONENT_H
