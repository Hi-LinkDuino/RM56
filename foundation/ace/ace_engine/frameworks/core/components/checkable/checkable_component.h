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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_CHECKABLE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_CHECKABLE_COMPONENT_H

#include "base/geometry/dimension.h"
#include "base/utils/label_target.h"
#include "core/components/checkable/checkable_theme.h"
#include "core/components/declaration/common/attribute.h"
#include "core/components_v2/common/common_def.h"
#include "core/pipeline/base/render_component.h"
#include "core/pipeline/base/render_node.h"
#ifndef WEARABLE_PRODUCT
#include "core/event/multimodal/multimodal_properties.h"
#endif

namespace OHOS::Ace {

enum class CheckableType {
    RADIO,
    CHECKBOX,
    SWITCH,
    UNKNOWN,
};

enum class CheckableStatus {
    ALL,
    PART,
    NONE,
};

template<class T>
class CheckableValue {
public:
    CheckableValue() = default;
    explicit CheckableValue(T value) : value_(value) {}
    virtual ~CheckableValue() = default;

    T GetValue() const
    {
        return value_;
    }

    void SetValue(T value)
    {
        value_ = value;
    }

private:
    T value_;
};

class ACE_EXPORT CheckableComponent : public RenderComponent {
    DECLARE_ACE_TYPE(CheckableComponent, RenderComponent);

public:
    CheckableComponent(CheckableType type, const RefPtr<CheckableTheme>& theme);
    ~CheckableComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    void ApplyTheme(const RefPtr<CheckableTheme>& theme);

    EventMarker& GetChangeEvent()
    {
        return changeEvent_;
    }

    void SetChangeEvent(const EventMarker& changeEvent)
    {
        changeEvent_ = changeEvent;
    }

    const EventMarker& GetClickEvent() const
    {
        return clickEvent_;
    }

    void SetClickEvent(const EventMarker& clickEvent)
    {
        clickEvent_ = clickEvent;
    }

    const EventMarker& GetDomChangeEvent() const
    {
        return domChangeEvent_;
    }

    void SetDomChangeEvent(const EventMarker& domChangeEvent)
    {
        domChangeEvent_ = domChangeEvent;
    }

    const Dimension& GetWidth() const
    {
        return width_;
    }

    const Dimension& GetHeight() const
    {
        return height_;
    }

    void SetWidth(const Dimension& width)
    {
        width_ = width;
    }

    void SetHeight(const Dimension& height)
    {
        height_ = height;
    }

    void SetHorizontalPadding(const Dimension& hotZoneHorizontalPadding)
    {
        hotZoneHorizontalPadding_ = hotZoneHorizontalPadding;
    }

    const Dimension& GetHotZoneHorizontalPadding() const
    {
        return hotZoneHorizontalPadding_;
    }

    void SetHotZoneVerticalPadding(const Dimension& hotZoneVerticalPadding)
    {
        hotZoneVerticalPadding_ = hotZoneVerticalPadding;
    }

    const Dimension& GetHotZoneVerticalPadding() const
    {
        return hotZoneVerticalPadding_;
    }

    void SetAspectRatio(double aspectRatio)
    {
        aspectRatio_ = aspectRatio;
    }

    double GetAspectRatio() const
    {
        return aspectRatio_;
    }

    double GetRadioInnerSizeRatio() const
    {
        return radioInnerSizeRatio_;
    }

    bool IsBackgroundSolid() const
    {
        return backgroundSolid_;
    }

    void SetBackgroundSolid(bool backgroundSolid)
    {
        backgroundSolid_ = backgroundSolid;
    }

    bool IsDisabled() const
    {
        return disabled_;
    }

    void SetDisabled(bool disabled)
    {
        disabled_ = disabled;
    }

    const Color& GetPointColor() const
    {
        return pointColor_;
    }

    void SetPointColor(const Color& pointColor)
    {
        pointColor_ = pointColor;
    }

    const Color& GetActiveColor() const
    {
        return activeColor_;
    }

    void SetActiveColor(const Color& activeColor)
    {
        activeColor_ = activeColor;
    }

    const Color& GetInactiveColor() const
    {
        return inactiveColor_;
    }

    void SetInactiveColor(const Color& inactiveColor)
    {
        inactiveColor_ = inactiveColor;
    }

    const Color& GetFocusColor() const
    {
        return focusColor_;
    }

    void SetFocusColor(const Color& focusColor)
    {
        focusColor_ = focusColor;
    }

    void SetDefaultWidth(const Dimension& defaultWidth)
    {
        defaultWidth_ = defaultWidth;
    }

    const Dimension& GetDefaultWidth() const
    {
        return defaultWidth_;
    }

    void SetDefaultHeight(const Dimension& defaultHeight)
    {
        defaultHeight_ = defaultHeight;
    }

    const Dimension& GetDefaultHeight() const
    {
        return defaultHeight_;
    }

    void SetNeedFocus(bool needFocus)
    {
        needFocus_ = needFocus;
    }

    bool GetNeedFocus() const
    {
        return needFocus_;
    }

    void SetHoverColor(const Color& hoverColor)
    {
        hoverColor_ = hoverColor;
    }

    const Color& GetHoverColor() const
    {
        return hoverColor_;
    }

    const Color& GetInactivePointColor() const
    {
        return inactivePointColor_;
    }

    const Color& GetShadowColor() const
    {
        return shadowColor_;
    }

    const Dimension& GetShadowWidth() const
    {
        return shadowWidth_;
    }

    const Dimension& GetHoverRadius() const
    {
        return hoverRadius_;
    }

    const Dimension& GetBorderWidth() const
    {
        return borderWidth_;
    }

    ACE_DEFINE_COMPONENT_EVENT(OnChange, void(bool));
    ACE_DEFINE_COMPONENT_EVENT(OnClick, void());

protected:
    CheckableType checkableType_ = CheckableType::UNKNOWN;
    Dimension width_;
    Dimension height_;
    Dimension hotZoneHorizontalPadding_;
    Dimension hotZoneVerticalPadding_;
    Dimension defaultWidth_;
    Dimension defaultHeight_;
    Dimension shadowWidth_;
    Dimension hoverRadius_;
    Dimension borderWidth_;
    bool backgroundSolid_ = true;
    bool disabled_ = false;
    bool needFocus_ = true;
    Color pointColor_;
    Color activeColor_;
    Color inactiveColor_;
    Color inactivePointColor_;
    Color focusColor_;
    Color hoverColor_;
    Color shadowColor_;
    EventMarker changeEvent_;
    EventMarker clickEvent_;
    EventMarker domChangeEvent_;
    double aspectRatio_ = 1.0;
    double radioInnerSizeRatio_ = 0.5;
};

class ACE_EXPORT CheckboxGroupResult : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(CheckboxGroupResult, BaseEventInfo);

public:
    CheckboxGroupResult(const std::vector<std::string>& vec, int32_t status) :
        BaseEventInfo("CheckboxGroupResult"), status_(status) {
        for (int i = 0; i < static_cast<int32_t>(vec.size()); ++i) {
            nameVector_.push_back(vec[i]);
        }
    }

    ~CheckboxGroupResult() = default;

    const std::vector<std::string>& GetNameList() const
    {
        return nameVector_;
    }

    int32_t GetStatus() const
    {
        return status_;
    }

private:
    std::vector<std::string> nameVector_;
    int32_t status_ = 0;
};

class ACE_EXPORT CheckboxComponent : public CheckableComponent, public CheckableValue<bool>, public LabelTarget {
    DECLARE_ACE_TYPE(CheckboxComponent, CheckableComponent, LabelTarget);

public:
    explicit CheckboxComponent(const RefPtr<CheckboxTheme>& theme);
    ~CheckboxComponent() override = default;
    void SetGroupName(const std::string& groupName)
    {
        groupName_ = groupName;
    }

    const std::string& GetGroupName() const
    {
        return groupName_;
    }

    void SetGroupValue(const CheckableStatus value)
    {
        groupValue_ = value;
    }

    CheckableStatus GetGroupValue() const
    {
        return groupValue_;
    }

    void SetGroup(const RefPtr<CheckboxComponent>& groupComponent)
    {
        group_ = groupComponent;
    }

    RefPtr<CheckboxComponent> GetGroup() const
    {
        return group_;
    }

    void SetCheckboxName(const std::string& checkboxName)
    {
        checkboxName_ = checkboxName;
    }

    const std::string& GetCheckboxName() const
    {
        return checkboxName_;
    }

    void SetBelongGroup(const std::string& group)
    {
        belongGroup_ = group;
    }

    const std::string& GetBelongGroup() const
    {
        return belongGroup_;
    }

    void SetGroupValueUpdateHandler(const std::function<void(CheckableStatus)>& value)
    {
        groupValueUpdateHandler_ = value;
    }

    void UpdateRenderChecked(CheckableStatus checked)
    {
        if (groupValueUpdateHandler_) {
            groupValueUpdateHandler_(checked);
        }
    }

    void AddCheckbox(RefPtr<CheckboxComponent> checkboxComponent)
    {
        checkboxList_.push_back(checkboxComponent);
    }

    void SetItemValueUpdateHandler(const std::function<void(bool)>& value)
    {
        itemValueUpdateHandler_ = value;
    }

    void UpdateItemChecked(bool checked)
    {
        if (itemValueUpdateHandler_) {
            itemValueUpdateHandler_(checked);
        }
    }

    void SetMember(bool isAllSelect)
    {
        for (auto& item : checkboxList_) {
            if (item->GetValue() == isAllSelect) {
                continue;
            }
            item->SetValue(isAllSelect);
            item->UpdateItemChecked(isAllSelect);
        }
    }

    void SetGroupStatus()
    {
        size_t count = 0;
        for (auto& item : checkboxList_) {
            if (item->GetValue()) {
                ++count;
            }
        }

        if (count == checkboxList_.size()) {
            UpdateRenderChecked(CheckableStatus::ALL);
        } else if (count ==0) {
            UpdateRenderChecked(CheckableStatus::NONE);
        } else {
            UpdateRenderChecked(CheckableStatus::PART);
        }
    }

    std::list<RefPtr<CheckboxComponent>> GetCheckboxList()
    {
        return checkboxList_;
    }

    void SetOnGroupChange(const EventMarker& OnGroupChange)
    {
        OnGroupChange_ = OnGroupChange;
    }

    const EventMarker& GetOnGroupChange() const
    {
        return OnGroupChange_;
    }

    void GetSelectedCheckBoxName(std::vector<std::string>& result)
    {
        for (auto& item : checkboxList_) {
            if (item->GetValue()) {
                result.push_back(item->GetCheckboxName());
            }
        }
    }

private:
    CheckableStatus groupValue_ = CheckableStatus::NONE;
    std::list<RefPtr<CheckboxComponent>> checkboxList_;
    RefPtr<CheckboxComponent> group_;
    std::string groupName_ = "";
    std::string checkboxName_ = "";
    std::string belongGroup_ = "";
    std::function<void(bool)> itemValueUpdateHandler_;
    std::function<void(CheckableStatus)> groupValueUpdateHandler_;
    EventMarker OnGroupChange_;
};

class ACE_EXPORT SwitchComponent : public CheckableComponent, public CheckableValue<bool> {
    DECLARE_ACE_TYPE(SwitchComponent, CheckableComponent);

public:
    explicit SwitchComponent(const RefPtr<SwitchTheme>& theme);
    ~SwitchComponent() override = default;

    void SetTextStyle(const TextStyle& textStyle)
    {
        textStyle_ = textStyle;
    }

    const TextStyle& GetTextStyle() const
    {
        return textStyle_;
    }

    const std::string& GetTextOn() const
    {
        return textOn_;
    }

    void SetTextOn(const std::string& textOn)
    {
        textOn_ = textOn;
    }

    const std::string& GetTextOff() const
    {
        return textOff_;
    }

    void SetTextOff(const std::string& textOff)
    {
        textOff_ = textOff;
    }

    bool GetShowText() const
    {
        return showText_;
    }

    void SetShowText(bool showText)
    {
        showText_ = showText;
    }

    const Color& GetTextColorOn() const
    {
        return textColorOn_;
    }

    void SetTextColorOn(const Color& textColorOn)
    {
        textColorOn_ = textColorOn;
    }

    const Color& GetTextColorOff() const
    {
        return textColorOff_;
    }

    void SetTextColorOff(const Color& textColorOff)
    {
        textColorOff_ = textColorOff;
    }

    const Dimension& GetTextPadding() const
    {
        return textPadding_;
    }

    void SetTextPadding(const Dimension& textPadding)
    {
        textPadding_ = textPadding;
    }

#ifndef WEARABLE_PRODUCT
    const CommonMultimodalAttribute& GetMultimodalProperties() const
    {
        return multimodalProperties_;
    }

    void SetMultimodalProperties(const CommonMultimodalAttribute& multimodalProperties)
    {
        multimodalProperties_ = multimodalProperties;
    }
#endif

private:
    std::string textOn_ = "On";
    std::string textOff_ = "Off";
    Color textColorOn_ = Color::BLACK;
    Color textColorOff_ = Color::BLACK;
    bool showText_ = false;
    TextStyle textStyle_;
#ifndef WEARABLE_PRODUCT
    CommonMultimodalAttribute multimodalProperties_;
#endif
    Dimension textPadding_ { 0, DimensionUnit::PX };
};

template<class VALUE_TYPE>
class ACE_EXPORT RadioComponent : public CheckableComponent, public CheckableValue<VALUE_TYPE>, public LabelTarget {
    DECLARE_ACE_TYPE(RadioComponent<VALUE_TYPE>, CheckableComponent, LabelTarget);

public:
    explicit RadioComponent(const RefPtr<RadioTheme>& theme) : CheckableComponent(CheckableType::RADIO, theme) {}
    ~RadioComponent() override = default;

    VALUE_TYPE GetGroupValue() const
    {
        return groupValue_;
    }

    void SetGroupValue(VALUE_TYPE groupValue)
    {
        groupValue_ = groupValue;
    }

    bool GetOriginChecked()
    {
        return originChecked_;
    }

    void SetOriginChecked(bool checked)
    {
        originChecked_ = checked;
    }

    const std::function<void(VALUE_TYPE)>& GetGroupValueChangedListener() const
    {
        return groupValueChangedListener_;
    }

    void SetGroupValueChangedListener(const std::function<void(VALUE_TYPE)>& groupValueChangedListener)
    {
        groupValueChangedListener_ = groupValueChangedListener;
    }

    void SetGroupValueUpdateHandler(const std::function<void(VALUE_TYPE)>& groupValueUpdateHandler)
    {
        groupValueUpdateHandler_ = groupValueUpdateHandler;
    }

    void UpdateGroupValue(VALUE_TYPE groupValue)
    {
        if (groupValueUpdateHandler_) {
            groupValueUpdateHandler_(groupValue);
        }
    }

    const std::string& GetGroupName() const
    {
        return groupName_;
    }
    void SetGroupName(const std::string& groupName)
    {
        groupName_ = groupName;
    }

private:
    VALUE_TYPE groupValue_;
    std::string groupName_;
    std::function<void(VALUE_TYPE)> groupValueChangedListener_;
    std::function<void(VALUE_TYPE)> groupValueUpdateHandler_;
    bool originChecked_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_CHECKABLE_COMPONENT_H
