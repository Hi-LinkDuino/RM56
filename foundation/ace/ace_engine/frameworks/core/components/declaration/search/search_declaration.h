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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SEARCH_SEARCH_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SEARCH_SEARCH_DECLARATION_H

#include "core/common/ime/text_edit_controller.h"
#include "core/components/common/properties/input_option.h"
#include "core/components/declaration/common/declaration.h"
#include "core/components/declaration/textfield/textfield_declaration.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace {

struct SearchAttribute : Attribute {
    Dimension closeIconSize;
    Dimension closeIconHotZoneHorizontal;
    std::string searchText;
    std::string closeIconSrc;
    Dimension searchHeight;
    Dimension searchWidth;
};

struct SearchStyle : Style {
    RefPtr<Decoration> decoration;
    Color hoverColor;
    Color pressColor;
    TextStyle editingStyle;
    TextStyle placeHoldStyle;
};

struct SearchEvent : Event {
    std::function<void(const std::string&)> submitEvent;
    EventMarker changeEventId;
    EventMarker submitEventId;
};

struct SearchMethod : Method {};

class SearchDeclaration : public Declaration {
    DECLARE_ACE_TYPE(SearchDeclaration, Declaration);

public:
    SearchDeclaration();
    ~SearchDeclaration() override = default;

    void InitializeStyle() override;
    void OnRequestFocus(bool shouldFocus) override;

    void SetCloseIconSize(const Dimension& closeIconSize)
    {
        auto& attribute = MaybeResetAttribute<SearchAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.closeIconSize = closeIconSize;
    }

    const Dimension& GetCloseIconSize() const
    {
        auto& attribute = static_cast<SearchAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.closeIconSize;
    }

    void SetCloseIconHotZoneHorizontal(const Dimension& closeIconHotZoneHorizontal)
    {
        auto& attribute = MaybeResetAttribute<SearchAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.closeIconHotZoneHorizontal = closeIconHotZoneHorizontal;
    }

    const Dimension& GetCloseIconHotZoneHorizontal() const
    {
        auto& attribute = static_cast<SearchAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.closeIconHotZoneHorizontal;
    }

    const std::string& GetSearchText() const
    {
        auto& attribute = static_cast<SearchAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.searchText;
    }

    void SetSearchText(const std::string& searchText)
    {
        auto& attribute = MaybeResetAttribute<SearchAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.searchText = searchText;
    }

    const std::string& GetCloseIconSrc() const
    {
        auto& attribute = static_cast<SearchAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.closeIconSrc;
    }

    void SetCloseIconSrc(const std::string& closeIconSrc)
    {
        auto& attribute = MaybeResetAttribute<SearchAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.closeIconSrc = closeIconSrc;
    }

    void SetChangeEventId(const EventMarker& changeEventId)
    {
        auto& event = MaybeResetEvent<SearchEvent>(EventTag::SPECIALIZED_EVENT);
        event.changeEventId = changeEventId;
    }

    const EventMarker& GetChangeEventId() const
    {
        auto& event = static_cast<SearchEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.changeEventId;
    }

    void SetSubmitEventId(const EventMarker& submitEventId)
    {
        auto& event = MaybeResetEvent<SearchEvent>(EventTag::SPECIALIZED_EVENT);
        event.submitEventId = submitEventId;
    }

    const EventMarker& GetSubmitEventId() const
    {
        auto& event = static_cast<SearchEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.submitEventId;
    }

    void SetSubmitEvent(const std::function<void(const std::string&)>& submitEvent)
    {
        auto& event = MaybeResetEvent<SearchEvent>(EventTag::SPECIALIZED_EVENT);
        event.submitEvent = submitEvent;
    }

    const std::function<void(const std::string&)>& GetSubmitEvent() const
    {
        auto& event = static_cast<SearchEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.submitEvent;
    }

    RefPtr<Decoration> GetDecoration() const
    {
        auto& style = static_cast<SearchStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.decoration;
    }

    void SetDecoration(const RefPtr<Decoration>& decoration)
    {
        auto& style = MaybeResetStyle<SearchStyle>(StyleTag::SPECIALIZED_STYLE);
        style.decoration = decoration;
    }

    const Color& GetHoverColor() const
    {
        auto& style = static_cast<SearchStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.hoverColor;
    }

    void SetHoverColor(const Color& hoverColor)
    {
        auto& style = MaybeResetStyle<SearchStyle>(StyleTag::SPECIALIZED_STYLE);
        style.hoverColor = hoverColor;
    }

    const Color& GetPressColor() const
    {
        auto& style = static_cast<SearchStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.pressColor;
    }

    void SetPressColor(const Color& pressColor)
    {
        auto& style = MaybeResetStyle<SearchStyle>(StyleTag::SPECIALIZED_STYLE);
        style.pressColor = pressColor;
    }

    const RefPtr<TextFieldDeclaration>& GetTextFieldDeclaration() const
    {
        return textFieldDeclaration_;
    }

    void SetTextFieldDeclaration(const RefPtr<TextFieldDeclaration>& textFieldDeclaration)
    {
        textFieldDeclaration_ = textFieldDeclaration;
    }

    const RefPtr<TextEditController>& GetTextEditController() const
    {
        return textEditController_;
    }

    void SetTextEditController(const RefPtr<TextEditController>& controller)
    {
        textEditController_ = controller;
    }

    void SetPlaceHoldStyle(const TextStyle& textstyle)
    {
        auto& style = MaybeResetStyle<SearchStyle>(StyleTag::SPECIALIZED_STYLE);
        style.placeHoldStyle = textstyle;
    }

    void SetEditingStyle(const TextStyle& textstyle)
    {
        auto& style = MaybeResetStyle<SearchStyle>(StyleTag::SPECIALIZED_STYLE);
        style.editingStyle = textstyle;
    }

    const TextStyle& GetPlaceHoldStyle() const
    {
        auto& style = static_cast<SearchStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.placeHoldStyle;
    }

    const TextStyle& GetEditingStyle() const
    {
        auto& style = static_cast<SearchStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.editingStyle;
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event) override;
    void CallSpecializedMethod(const std::string& method, const std::string& args) override;

private:
    bool hasBoxRadius_ = false;
    bool isPaddingChanged_ = true;
    RefPtr<TextEditController> textEditController_;
    RefPtr<TextFieldDeclaration> textFieldDeclaration_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SEARCH_SEARCH_DECLARATION_H
