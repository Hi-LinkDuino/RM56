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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_NAVIGATION_BAR_COMPONENT_BASE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_NAVIGATION_BAR_COMPONENT_BASE_H

#include "core/components/button/button_component.h"
#include "core/components/image/image_component.h"
#include "core/components/navigation_bar/navigation_bar_theme.h"
#include "core/components/text/text_component.h"
#include "core/pipeline/base/component.h"

namespace OHOS::Ace {

class PaddingComponent;
class ComposedComponent;
class NavigationBarTheme;
class EventMarker;

class ACE_EXPORT NavigationTitleModeChangeEvent : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(NavigationTitleModeChangeEvent, BaseEventInfo);

public:
    explicit NavigationTitleModeChangeEvent(bool isMiniBar)
        : BaseEventInfo("NavigationTitleModeChangeEvent"), isMiniBar_(isMiniBar)
    {}
    ~NavigationTitleModeChangeEvent() = default;

    bool IsMiniBar() const
    {
        return isMiniBar_;
    }

private:
    bool isMiniBar_ = 0;
};

class NavigationBarComponentBase {
public:
    NavigationBarComponentBase() = default;
    virtual ~NavigationBarComponentBase() = default;
protected:
    struct IconImage {
        IconImage(InternalResource::ResourceId resourceId, const Dimension& width, const Dimension& height)
            : resourceId(resourceId), width(width), height(height),
              image(AceType::MakeRefPtr<ImageComponent>(resourceId))
        {
            image->SetWidth(width);
            image->SetHeight(height);
        };
        IconImage(const std::string& src, const Dimension& width, const Dimension& height,
            std::optional<Color> imageFill = std::nullopt)
            : src(src), width(width), height(height), image(AceType::MakeRefPtr<ImageComponent>(src))
        {
            image->SetWidth(width);
            image->SetHeight(height);
            image->SetImageFill(imageFill);
        };
        ~IconImage() = default;

        InternalResource::ResourceId resourceId = InternalResource::ResourceId::NO_ID;
        std::string src;
        Dimension width;
        Dimension height;
        RefPtr<ImageComponent> image;
    };

    RefPtr<ButtonComponent> BuildIconButton(const RefPtr<NavigationBarTheme>& theme, const Dimension& width,
        const Dimension& height, const IconImage& icon, const EventMarker& clickMarker = EventMarker());
    RefPtr<TextComponent> BuildTitleText(
        const std::string& value, const Color& color, const Dimension& fontSize, const FontWeight& fontWeight);
    RefPtr<PaddingComponent> BuildPadding(double size, bool isVertical = false);
    RefPtr<ComposedComponent> GenerateAccessibilityComposed(
        int32_t parentId, const std::string& name, const RefPtr<Component>& child);

    WeakPtr<PipelineContext> context_;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_NAVIGATION_BAR_COMPONENT_BASE_H
