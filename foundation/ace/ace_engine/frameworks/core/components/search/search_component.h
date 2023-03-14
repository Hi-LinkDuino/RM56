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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEARCH_SEARCH_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEARCH_SEARCH_COMPONENT_H

#include "base/resource/internal_resource.h"
#include "base/utils/utils.h"
#include "core/components/image/image_component.h"
#include "core/components/text_field/text_field_component.h"
#include "core/pipeline/base/constants.h"
#include "core/pipeline/base/element.h"
#include "core/pipeline/base/sole_child_component.h"
#include "core/components/declaration/search/search_declaration.h"
#include "core/components/text_field/textfield_theme.h"

namespace OHOS::Ace {

class ACE_EXPORT SearchComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(SearchComponent, SoleChildComponent);

    ACE_DEFINE_COMPONENT_EVENT(OnSubmit, void(const std::string&));
    ACE_DEFINE_COMPONENT_EVENT(OnChange, void(const std::string&));

public:
    SearchComponent();
    ~SearchComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    void SetCloseIconSize(const Dimension& closeIconSize);
    const Dimension& GetCloseIconSize() const;

    void SetCloseIconHotZoneHorizontal(const Dimension& closeIconHotZoneHorizontal);
    const Dimension& GetCloseIconHotZoneHorizontal() const;

    const std::string& GetSearchText() const;
    void SetSearchText(const std::string& searchText);

    const std::string& GetCloseIconSrc() const;
    void SetCloseIconSrc(const std::string& closeIconSrc);

    void SetChangeEventId(const EventMarker& changeEventId);
    const EventMarker& GetChangeEventId() const;

    void SetSubmitEventId(const EventMarker& submitEventId);
    const EventMarker& GetSubmitEventId() const;

    RefPtr<TextEditController> GetTextEditController() const;
    void SetTextEditController(const RefPtr<TextEditController>& controller);

    void SetSubmitEvent(const std::function<void(const std::string&)>& event);
    const std::function<void(const std::string&)>& GetSubmitEvent() const;

    RefPtr<Decoration> GetDecoration() const;
    void SetDecoration(const RefPtr<Decoration>& decoration);

    const Color& GetHoverColor() const;
    void SetHoverColor(const Color& hoverColor);

    const Color& GetPressColor() const;
    void SetPressColor(const Color& pressColor);

    void SetPlaceHoldStyle(const TextStyle& style);
    const TextStyle& GetPlaceHoldStyle() const;

    void SetEditingStyle(const TextStyle& style);
    const TextStyle& GetEditingStyle() const;

    void SetDeclaration(const RefPtr<SearchDeclaration>& declaration);

private:
    RefPtr<SearchDeclaration> declaration_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEARCH_SEARCH_COMPONENT_H
