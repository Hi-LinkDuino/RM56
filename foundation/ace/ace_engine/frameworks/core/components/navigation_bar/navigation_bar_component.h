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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_NAVIGATION_BAR_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_NAVIGATION_BAR_COMPONENT_H

#include "core/components/button/button_component.h"
#include "core/components/flex/flex_component.h"
#include "core/components/navigation_bar/navigation_bar_component_base.h"
#include "core/components/navigation_bar/navigation_bar_theme.h"
#include "core/components/text/text_component.h"
#ifndef WEARABLE_PRODUCT
#include "core/components/menu/menu_component.h"
#include "core/components/navigation_bar/render_collapsing_navigation_bar.h"
#include "core/components/select/select_component.h"
#include "core/components/tab_bar/tab_bar_component.h"
#endif

namespace OHOS::Ace {

using NavigationBarShowFunc = std::function<void()>;
using NavigationBarHideFunc = std::function<void()>;

class NavigationBarController : public virtual AceType {
    DECLARE_ACE_TYPE(NavigationBarController, AceType);

public:
    void Show() const
    {
        if (showFunc_) {
            showFunc_();
        }
    }

    void Hide() const
    {
        if (hideFunc_) {
            hideFunc_();
        }
    }

    void SetFunction(const NavigationBarShowFunc& showFunc, const NavigationBarHideFunc& hideFunc)
    {
        showFunc_ = showFunc;
        hideFunc_ = hideFunc;
    }

private:
    NavigationBarShowFunc showFunc_;
    NavigationBarHideFunc hideFunc_;
};

enum class NavigationBarType {
    NORMAL = 0,
    EMPHASIZE,
};

class TitleBarBuilder;

struct NavigationBarData : public virtual AceType {
    DECLARE_ACE_TYPE(NavigationBarData, AceType);

public:
    NavigationBarData() = default;
    ~NavigationBarData() = default;

    std::string header;
    std::string title;
    std::string subTitle;
    bool backEnabled = false;
    EventMarker backClickMarker;
    std::string logo;
    std::string startIcon;
    EventMarker startClickMarker;
    std::string endIcon;
    EventMarker endClickMarker;
    Color titleColor;
    Color subTitleColor;
    std::optional<Color> imageFill;
    NavigationBarType type = NavigationBarType::NORMAL;

#ifndef WEARABLE_PRODUCT
    RefPtr<TabBarComponent> tabBar;
    RefPtr<SelectComponent> selectPopup;
    RefPtr<MenuComponent> menu;
    std::list<RefPtr<OptionComponent>> allMenuItems;
#endif

    RefPtr<NavigationBarController> controller;
    RefPtr<NavigationBarTheme> theme;
};

class NavigationBarComponent : public ComposedComponent {
    DECLARE_ACE_TYPE(NavigationBarComponent, ComposedComponent);

public:
    struct MenuItemInBar {
        std::string value;
        EventMarker clickEvent;
        RefPtr<ButtonComponent> button;
    };

    NavigationBarComponent(const ComposeId& id, const std::string& name);
    ~NavigationBarComponent() override = default;

    void InitStyle(const RefPtr<NavigationBarTheme>& theme);
    RefPtr<Element> CreateElement() override;
    RefPtr<Component> Build(const WeakPtr<PipelineContext>& context);

#ifndef WEARABLE_PRODUCT
    RefPtr<MenuComponent> GetMenu() const
    {
        return data_->menu;
    }

    const std::map<std::string, NavigationBarComponent::MenuItemInBar>& GetMenuItemsInBar() const
    {
        return menuItemsInBar_;
    }

    const RefPtr<ButtonComponent>& GetMoreButton() const
    {
        return moreButton_;
    }
#endif
    const RefPtr<NavigationBarData>& GetData() const
    {
        return data_;
    }

private:
    WeakPtr<PipelineContext> context_;
    RefPtr<NavigationBarData> data_;
#ifndef WEARABLE_PRODUCT
    std::map<std::string, NavigationBarComponent::MenuItemInBar> menuItemsInBar_;
    RefPtr<ButtonComponent> moreButton_;
#endif
};

class CommonBuilder : public NavigationBarComponentBase {
public:
    CommonBuilder() = delete;
    virtual ~CommonBuilder() = default;
    explicit CommonBuilder(const RefPtr<NavigationBarTheme>& theme)
        : theme_(theme), menuZoneSize_(theme_->GetMenuZoneSize()), menuIconSize_(theme_->GetMenuIconSize())
    {}
    void SetContext(const WeakPtr<PipelineContext>& context)
    {
        context_ = context;
    }
    void SetId(const ComposeId& id)
    {
        id_ = id;
    }
    void SetTextDirection(const TextDirection& direction)
    {
        direction_ = direction;
    }

protected:
    RefPtr<NavigationBarTheme> theme_;
    ComposeId id_;
    Dimension menuZoneSize_;
    Dimension menuIconSize_;
    TextDirection direction_ = TextDirection::LTR;
};

class TitleBarBuilder : public virtual AceType, public virtual CommonBuilder {
    DECLARE_ACE_TYPE(TitleBarBuilder, AceType);

public:
    explicit TitleBarBuilder(const RefPtr<NavigationBarData>& data)
        : CommonBuilder(data->theme), height_(data->theme->GetHeight()), title_(data->title),
          titleColor_(data->titleColor), subTitle_(data->subTitle), subTitleColor_(data->subTitleColor),
          subTitleFontSize_(data->theme->GetSubTitleFontSize())
    {}
    virtual ~TitleBarBuilder() = default;
    virtual RefPtr<Component> Build() = 0;

protected:
#ifndef WEARABLE_PRODUCT
    RefPtr<BoxComponent> BuildContainer(const RefPtr<Component>& child)
    {
        auto boxContainer = AceType::MakeRefPtr<BoxComponent>();
        boxContainer->SetHeight(height_.Value(), height_.Unit());
        boxContainer->SetChild(child);
        boxContainer->SetPadding(padding_);
        return boxContainer;
    }
#endif

    Edge padding_;
    Dimension height_;

    std::string title_;
    Color titleColor_;
    Dimension titleFontSize_;

    std::string subTitle_;
    Color subTitleColor_;
    Dimension subTitleFontSize_;
};

class WatchTitleBarBuilder : public TitleBarBuilder {
    DECLARE_ACE_TYPE(WatchTitleBarBuilder, TitleBarBuilder);

public:
    explicit WatchTitleBarBuilder(const RefPtr<NavigationBarData>& data)
        : CommonBuilder(data->theme), TitleBarBuilder(data), logoSrc_(data->logo)
    {}
    ~WatchTitleBarBuilder() = default;
    RefPtr<Component> Build() override;

private:
    void BuildTitle(const RefPtr<ComponentGroup>& parent);
    void BuildSubTitle(const RefPtr<ComponentGroup>& parent);
    void BuildLogo(const RefPtr<ComponentGroup>& parent);

    std::string logoSrc_;
};

#ifndef WEARABLE_PRODUCT
class TabBarBuilder : public virtual CommonBuilder {
public:
    explicit TabBarBuilder(const RefPtr<NavigationBarData>& data) : CommonBuilder(data->theme), tabBar_(data->tabBar) {}
    ~TabBarBuilder() = default;

    void BuildTabBar(const RefPtr<ComponentGroup>& parent);

protected:
    RefPtr<TabBarComponent> tabBar_;
};

class TitleBarMenuBuilder : public virtual AceType, public virtual CommonBuilder {
    DECLARE_ACE_TYPE(TitleBarMenuBuilder, AceType);

public:
    explicit TitleBarMenuBuilder(const RefPtr<NavigationBarData>& data)
        : CommonBuilder(data->theme), menu_(data->menu), imageFill_(data->imageFill),
          allMenuItems_(data->allMenuItems) {}
    ~TitleBarMenuBuilder() = default;

    const std::map<std::string, NavigationBarComponent::MenuItemInBar>& GetMenuItemsInBar() const
    {
        return menuItemsInBar_;
    }
    const RefPtr<ButtonComponent>& GetMoreButton() const
    {
        return moreButton_;
    }
    const RefPtr<MenuComponent>& GetMenu() const
    {
        return menu_;
    }

protected:
    bool AddMenu(const RefPtr<ComponentGroup>& container, bool canCollapse = true);

    RefPtr<MenuComponent> menu_;
    std::optional<Color> imageFill_;

private:
    void MoveMenuItemsToBar(const RefPtr<ComponentGroup>& container);
    void AddCollapseMenu(const RefPtr<ComponentGroup>& container);

    std::map<std::string, NavigationBarComponent::MenuItemInBar> menuItemsInBar_;
    std::list<RefPtr<OptionComponent>> allMenuItems_;
    RefPtr<ButtonComponent> moreButton_;
};

class TVTitleBarBuilder : public TitleBarBuilder, public TitleBarMenuBuilder {
    DECLARE_ACE_TYPE(TVTitleBarBuilder, TitleBarBuilder, TitleBarMenuBuilder);

public:
    explicit TVTitleBarBuilder(const RefPtr<NavigationBarData>& data)
        : CommonBuilder(data->theme), TitleBarBuilder(data), TitleBarMenuBuilder(data)
    {}
    ~TVTitleBarBuilder() = default;

    RefPtr<Component> Build() override;

private:
    void BuildTitle(const RefPtr<ComponentGroup>& parent);
    void BuildSubTitle(const RefPtr<ComponentGroup>& parent, const RefPtr<ComponentGroup>& leftContainer);
};

class PhoneTitleBarBuilder : public TitleBarBuilder, public TitleBarMenuBuilder, public TabBarBuilder {
    DECLARE_ACE_TYPE(PhoneTitleBarBuilder, TitleBarBuilder, TitleBarMenuBuilder);

public:
    explicit PhoneTitleBarBuilder(const RefPtr<NavigationBarData>& data)
        : CommonBuilder(data->theme), TitleBarBuilder(data), TitleBarMenuBuilder(data), TabBarBuilder(data),
          selectPopup_(data->selectPopup)
    {
        titleFontSize_ = data->theme->GetTitleFontSize();
    }
    ~PhoneTitleBarBuilder() = default;

    RefPtr<Component> Build() override = 0;
    const RefPtr<ComposedComponent>& GetTitleComposed() const
    {
        return titleComposed_;
    }
    const RefPtr<ComposedComponent>& GetSubtitleComposed() const
    {
        return subTitleComposed_;
    }

protected:
    RefPtr<Component> BuildTitle(double padding = 0.0);
    void BuildSelectPopup(const RefPtr<ComponentGroup>& parent);
    void BuildTitleZone(const RefPtr<ComponentGroup>& parent, double padding = 0.0);

    RefPtr<SelectComponent> selectPopup_;
    std::string header_;
    RefPtr<ComposedComponent> titleComposed_;
    RefPtr<ComposedComponent> subTitleComposed_;
};

class EmphasizeTitleBarBuilder : public PhoneTitleBarBuilder {
    DECLARE_ACE_TYPE(EmphasizeTitleBarBuilder, PhoneTitleBarBuilder);

public:
    explicit EmphasizeTitleBarBuilder(const RefPtr<NavigationBarData>& data)
        : CommonBuilder(data->theme), PhoneTitleBarBuilder(data)
    {
        height_ = Dimension(112, DimensionUnit::VP);
        padding_.SetLeft(theme_->GetMaxPaddingStart());
        padding_.SetRight(theme_->GetDefaultPaddingEnd());
        titleFontSize_ = theme_->GetTitleFontSizeBig();
        header_ = data->header;
    }
    ~EmphasizeTitleBarBuilder() = default;

    RefPtr<Component> Build() override;
};

class NormalTitleBarBuilder : public PhoneTitleBarBuilder {
    DECLARE_ACE_TYPE(NormalTitleBarBuilder, PhoneTitleBarBuilder);

public:
    explicit NormalTitleBarBuilder(const RefPtr<NavigationBarData>& data)
        : CommonBuilder(data->theme), PhoneTitleBarBuilder(data), backEnabled_(data->backEnabled),
          backClickMarker_(data->backClickMarker), logoSrc_(data->logo), startIconSrc_(data->startIcon),
          startClickMarker_(data->startClickMarker), endIconSrc_(data->endIcon), endClickMarker_(data->endClickMarker),
          imageFill_(data->imageFill)
    {}
    ~NormalTitleBarBuilder() = default;

    RefPtr<Component> Build() override;
    void BindDefaultBackEvent(const WeakPtr<PipelineContext>& context);

private:
    void BuildStartZone(const RefPtr<RowComponent>& parent);
    void BuildLogo(const RefPtr<RowComponent>& parent);
    void BuildEndZone(const RefPtr<RowComponent>& parent);

    bool backEnabled_ = false;
    EventMarker backClickMarker_;
    std::string logoSrc_;
    std::string startIconSrc_;
    EventMarker startClickMarker_;
    std::string endIconSrc_;
    EventMarker endClickMarker_;
    std::optional<Color> imageFill_;
};

class CollapsingNavigationBarComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(CollapsingNavigationBarComponent, ComponentGroup);

public:
    CollapsingNavigationBarComponent() = default;
    CollapsingNavigationBarComponent(const RefPtr<ComposedComponent>& title, const RefPtr<ComposedComponent>& subTitle,
        const EventMarker& changeEvent)
        : titleComposed_(title), subTitleComposed_(subTitle), titleModeChangedEvent_(changeEvent)
    {}
    explicit CollapsingNavigationBarComponent(const RefPtr<EmphasizeTitleBarBuilder>& builder)
    {
        titleComposed_ = builder->GetTitleComposed();
        subTitleComposed_ = builder->GetSubtitleComposed();
    }
    ~CollapsingNavigationBarComponent() override = default;
    RefPtr<Element> CreateElement() override
    {
        return AceType::MakeRefPtr<ComponentGroupElement>();
    }

    RefPtr<RenderNode> CreateRenderNode() override
    {
        return AceType::MakeRefPtr<RenderCollapsingNavigationBar>();
    }
    const RefPtr<ComposedComponent>& GetTitleComposed() const
    {
        return titleComposed_;
    }
    const RefPtr<ComposedComponent>& GetSubtitleComposed() const
    {
        return subTitleComposed_;
    }
    const Dimension& GetMinHeight() const
    {
        return minHeight_;
    }
    void SetMinHeight(const Dimension& minHeight)
    {
        minHeight_ = minHeight;
    }
    const EventMarker& GetTitleModeChangedEvent() const
    {
        return titleModeChangedEvent_;
    }

private:
    RefPtr<TextComponent> titleComponent_;
    RefPtr<ComposedComponent> titleComposed_;
    RefPtr<ComposedComponent> subTitleComposed_;
    Dimension minHeight_;
    EventMarker titleModeChangedEvent_;
};

#endif

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_NAVIGATION_BAR_COMPONENT_H
