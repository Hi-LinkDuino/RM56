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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_BASE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_BASE_COMPONENT_H

#include "base/utils/system_properties.h"
#include "core/accessibility/accessibility_manager.h"
#include "core/common/container.h"
#include "core/components/checkable/checkable_component.h"
#include "core/components/common/rotation/rotation_controller.h"
#include "core/components/dialog/dialog_component.h"
#include "core/components/picker/picker_animation_controller.h"
#include "core/components/picker/picker_column_component.h"
#include "core/components/stack/stack_element.h"
#include "core/components/text/text_component.h"
#include "core/event/back_end_event_manager.h"
#include "core/pipeline/base/sole_child_component.h"
#include "core/components_v2/common/common_def.h"

namespace OHOS::Ace {

enum DatePickerType {
    TIME = 0,
    DATE,
};

class ACE_EXPORT PickerDate final {
public:
    PickerDate() = default;
    PickerDate(uint32_t year, uint32_t month, uint32_t day) : year_(year), month_(month), day_(day) {}

    ~PickerDate() = default;

    static PickerDate Current();

    static uint32_t GetMaxDay(uint32_t year, uint32_t month);

    static bool IsLeapYear(uint32_t year);

    uint32_t GetYear() const
    {
        return year_;
    }
    void SetYear(uint32_t value)
    {
        year_ = value;
    }

    uint32_t GetMonth() const
    {
        return month_;
    }
    void SetMonth(uint32_t value)
    {
        month_ = value;
    }

    uint32_t GetDay() const
    {
        return day_;
    }
    void SetDay(uint32_t value)
    {
        day_ = value;
    }

    uint32_t GetWeek() const
    {
        return week_;
    }
    void SetWeek(uint32_t value)
    {
        week_ = value;
    }

    std::string ToString(bool jsonFormat, int32_t status = -1) const;

    uint32_t ToDays() const;
    void FromDays(uint32_t days);

private:
    uint32_t year_ = 0;
    uint32_t month_ = 0;
    uint32_t day_ = 0;
    uint32_t week_ = 0;
};

class PickerTime final {
public:
    PickerTime() = default;
    PickerTime(uint32_t hour, uint32_t minute, uint32_t second) : hour_(hour), minute_(minute), second_(second) {}

    ~PickerTime() = default;

    static PickerTime Current();

    uint32_t GetHour() const
    {
        return hour_;
    }
    void SetHour(uint32_t value)
    {
        hour_ = value;
    }

    uint32_t GetMinute() const
    {
        return minute_;
    }
    void SetMinute(uint32_t value)
    {
        minute_ = value;
    }

    uint32_t GetSecond() const
    {
        return second_;
    }
    void SetSecond(uint32_t value)
    {
        second_ = value;
    }

    std::string ToString(bool jsonFormat, bool hasSecond, int32_t status = -1) const;

private:
    uint32_t hour_ = 0;
    uint32_t minute_ = 0;
    uint32_t second_ = 0;
};

class PickerDateTime final {
public:
    PickerDateTime() = default;
    PickerDateTime(const PickerDate& date, const PickerTime& time) : date_(date), time_(time) {}

    ~PickerDateTime() = default;

    static PickerDateTime Current();

    const PickerDate& GetDate() const
    {
        return date_;
    }
    void SetDate(const PickerDate& value)
    {
        date_ = value;
    }

    const PickerTime& GetTime() const
    {
        return time_;
    }
    void SetTime(const PickerTime& value)
    {
        time_ = value;
    }

    std::string ToString(bool jsonFormat, int32_t status = -1) const;

private:
    PickerDate date_;
    PickerTime time_;
};

class ACE_EXPORT DatePickerChangeEvent : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(DatePickerChangeEvent, BaseEventInfo);

public:
    explicit DatePickerChangeEvent(const std::string& str) : BaseEventInfo
        ("DatePickerChangeEvent"), selectedStr_(str) {}
    ~DatePickerChangeEvent() = default;

    const std::string& GetSelectedStr() const
    {
        return selectedStr_;
    }

private:
    std::string selectedStr_;
};

class ACE_EXPORT PickerBaseComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(PickerBaseComponent, SoleChildComponent);

public:
    PickerBaseComponent() : rotationController_(AceType::MakeRefPtr<RotationController>()) {}
    ~PickerBaseComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    void Initialize(const RefPtr<AccessibilityManager>& accessibilityManager, const RefPtr<ThemeManager>& themeManager);

    void SetDialogName(const std::string& dialogName)
    {
        dialogInsprctorTag_ = dialogName;
    }

    const std::string& GetDialogName()
    {
        return dialogInsprctorTag_;
    }

    bool GetHasTitle() const
    {
        return hasTitle_;
    }
    void SetHasTitle(bool value)
    {
        hasTitle_ = (!subsidiary_ && value);
    }

    bool GetHasLunar() const
    {
        return hasLunar_;
    }
    void SetHasLunar(bool value)
    {
        hasLunar_ = SystemProperties::GetDeviceType() == DeviceType::PHONE && value;
    }

    bool GetHasButtons() const
    {
        return hasButtons_;
    }
    void SetHasButtons(bool value)
    {
        hasButtons_ = (!subsidiary_ && value);
    }

    bool GetHasTriangle() const
    {
        return hasTriangle_;
    }
    void SetHasTriangle(bool value)
    {
        hasTriangle_ = value;
    }

    bool GetIsDialog() const
    {
        return isDialog_;
    }
    void SetIsDialog(bool value)
    {
        isDialog_ = value;
    }

    bool GetIsCreateDialogComponent() const
    {
        return isCreateDialogComponent_;
    }
    void SetIsCreateDialogComponent(bool value)
    {
        isCreateDialogComponent_ = value;
    }

    bool GetSubsidiary() const
    {
        return subsidiary_;
    }
    void SetSubsidiary(bool value)
    {
        subsidiary_ = value;
    }

    bool GetMasterHasLunar() const
    {
        return masterHasLunar_;
    }
    void SetMasterHasLunar(bool value)
    {
        masterHasLunar_ = value;
    }

    const EventMarker& GetOnChange() const
    {
        return onChange_;
    }
    void SetOnChange(const EventMarker& value)
    {
        onChange_ = value;
    }

    const EventMarker& GetOnColumnChange() const
    {
        return onColumnChange_;
    }
    void SetOnColumnChange(const EventMarker& value)
    {
        onColumnChange_ = value;
    }

    const EventMarker& GetOnCancel() const
    {
        return onCancel_;
    }
    void SetOnCancel(const EventMarker& value)
    {
        onCancel_ = value;
    }

    const EventMarker& GetDialogAcceptEvent() const
    {
        return OnDialogAccept_;
    }
    void SetDialogAcceptEvent(const EventMarker& value)
    {
        OnDialogAccept_ = value;
    }

    const EventMarker& GetDialogCancelEvent() const
    {
        return OnDialogCancel_;
    }
    void SetDialogCancelEvent(const EventMarker& value)
    {
        OnDialogCancel_ = value;
    }

    const EventMarker& GetDialogChangeEvent() const
    {
        return OnDialogChange_;
    }
    void SetDialogChangeEvent(const EventMarker& value)
    {
        OnDialogChange_ = value;
    }

    void ClearColumns();

    void AppendColumn(const RefPtr<PickerColumnComponent>& column);

    RefPtr<PickerColumnComponent> GetColumn(const std::string& tag) const;

    virtual uint32_t GetColumnCount() const
    {
        return columns_.size();
    }

    void RemoveColumn(const std::string& tag);

    void SetFinishCallback(const ColumnFinishCallback& value);

    void SetChangeCallback(const ColumnChangeCallback& value);

    const RefPtr<PickerTheme>& GetTheme() const
    {
        return theme_;
    }
    void SetTheme(const RefPtr<PickerTheme>& value)
    {
        theme_ = value;
    }


    const RefPtr<PickerAnimationController>& GetAnimationController() const
    {
        return animationController_;
    }

    void SetAnimationController(const RefPtr<PickerAnimationController>& value)
    {
        animationController_ = value;
    }

    const RefPtr<TextComponent>& GetTitle() const
    {
        return title_;
    }

    const RefPtr<CheckboxComponent>& GetLunar() const
    {
        return lunar_;
    }

    int32_t GetNodeId() const
    {
        return nodeId_;
    }

    void SetNodeId(int32_t value)
    {
        if (!GetIsDialog()) {
            nodeId_ = value;
        }
    }

    const Dimension& GetColumnHeight() const
    {
        return columnHeight_;
    }
    void SetColumnHeight(const Dimension& value)
    {
        columnHeight_ = value;
    }

    const RefPtr<StackElement>& GetStack()
    {
        return stack_;
    }

    void ShowDialog(const RefPtr<StackElement>& stack, bool disableTouchEvent = true);
    bool HideDialog();
    void OpenDialog(DialogProperties& properties);
    void CloseDialog();

    bool IsDialogShowed()
    {
        return dialogShowed_;
    }

    const RefPtr<AccessibilityNode>& GetLunarAccessibility() const
    {
        return lunarAccessibility_;
    }

    const RefPtr<AccessibilityNode>& GetSwitchAccessibility() const
    {
        return switchAccessibility_;
    }

    const RefPtr<AccessibilityNode>& GetTitleAccessibility() const
    {
        return titleAccessibility_;
    }

    const RefPtr<AccessibilityNode>& GetCancelAccessibility() const
    {
        return cancelAccessibility_;
    }

    const RefPtr<AccessibilityNode>& GetOkAccessibility() const
    {
        return okAccessibility_;
    }

    // used for inspector node in PC preview
    const RefPtr<AccessibilityNode>& GetPickerDialogAccessibility() const
    {
        return rootAccessibility_;
    }

    virtual bool SubsidiaryShowed() const
    {
        return false;
    }

    virtual bool NeedRtlColumnOrder() const
    {
        return true;
    }

    virtual bool IsShowLunar() const
    {
        return false;
    }

    virtual void OnTitleBuilding();

    virtual void OnColumnsCreating() {}

    virtual void OnColumnsBuilding() {}

    virtual std::string GetSelectedObject(bool isColumnChange,
        const std::string& changeColumnTag, int32_t status = -1) const
    {
        return "{}";
    }

    virtual void OnSelectedSaving() {}

    virtual void OnDataLinking(const std::string& tag, bool isAdd, uint32_t index, std::vector<std::string>& resultTags)
    {}

    virtual void OnLunarCallback(bool checked, std::vector<std::string>& resultTags) {}
    virtual void OnTriangleCallback(bool value) {}
    virtual void OnAnimationPlaying() {}

    const RefPtr<RotationController>& GetRotationController() const
    {
        return rotationController_;
    }

    const EventMarker& GetOnCancelClickId() const
    {
        return onCancelClickId_;
    }

    const EventMarker& GetOnOkClickId() const
    {
        return onOkClickId_;
    }

    // used for inspector node in PC preview
    void SetPickerBaseId(const int32_t nodeId)
    {
        pickerId_ = nodeId;
    }

    // used for inspector node in PC preview
    int32_t GetPickerBaseId() const
    {
        return pickerId_;
    }

    ACE_DEFINE_COMPONENT_EVENT(OnTextCancel, void(void));
    ACE_DEFINE_COMPONENT_EVENT(OnTextAccept, void(const std::string&, double));
    ACE_DEFINE_COMPONENT_EVENT(OnTextChange, void(const std::string&, double));

    void SetNeedVibrate(bool needVibrate);

    bool GetNeedVibrate()
    {
        return needVibrate_;
    }

    static const char PICKER_YEAR_COLUMN[];
    static const char PICKER_MONTH_COLUMN[];
    static const char PICKER_DAY_COLUMN[];
    static const char PICKER_HOUR_COLUMN[];
    static const char PICKER_MINUTE_COLUMN[];
    static const char PICKER_SECOND_COLUMN[];
    static const char PICKER_TEXT_COLUMN[];
    static const char PICKER_MONTHDAY_COLUMN[];
    static const char PICKER_AMPM_COLUMN[];

private:
    void InitializeTitle(std::list<RefPtr<Component>>& outChildren);

    void InitializeColumns(
        std::list<RefPtr<Component>>& outChildren, const RefPtr<AccessibilityManager>& accessibilityManager);

    void InitializeChildAccessibility(const RefPtr<AccessibilityManager>& accessibilityManager);

    void ClearAccessibilityNodes();

    RefPtr<Component> GenerateAccessibilityComposed(
        const std::string& name, const RefPtr<Component>& child, RefPtr<AccessibilityNode>& node);

    void InitializeLunar(std::list<RefPtr<Component>>& outChildren, const RefPtr<ThemeManager>& themeManager);

    void InitializeButtons(std::list<RefPtr<Component>>& outChildren, const RefPtr<ThemeManager>& themeManager);

    void InitializeContainer(const std::list<RefPtr<Component>>& outChildren);

    bool dialogShowed_ = false;
    bool hasTitle_ = false;
    bool hasLunar_ = false;
    bool hasButtons_ = false;
    bool hasTriangle_ = false;
    bool isDialog_ = true;
    bool subsidiary_ = false;
    bool masterHasLunar_ = false;
    bool needVibrate_ = true;
    bool isCreateDialogComponent_ = false;
    int32_t nodeId_ = -1; // default of dialog's node id.
    // used for inspector node in PC preview
    int32_t pickerId_ = -1;

    RefPtr<PickerTheme> theme_;
    RefPtr<StackElement> stack_;

    RefPtr<TextComponent> title_ = AceType::MakeRefPtr<TextComponent>("");
    RefPtr<CheckboxComponent> lunar_;
    std::vector<RefPtr<PickerColumnComponent>> columns_;
    WeakPtr<AccessibilityManager> accessibilityManager_;
    int32_t rootAccessibilityId_ = -1;
    RefPtr<AccessibilityNode> rootAccessibility_;
    RefPtr<AccessibilityNode> titleAccessibility_;
    RefPtr<AccessibilityNode> lunarAccessibility_;
    RefPtr<AccessibilityNode> switchAccessibility_;
    RefPtr<AccessibilityNode> cancelAccessibility_;
    RefPtr<AccessibilityNode> okAccessibility_;

    EventMarker onChange_;
    EventMarker onColumnChange_;
    EventMarker onCancel_;
    EventMarker onOkClickId_ = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
    EventMarker onCancelClickId_ = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
    EventMarker OnDialogAccept_;
    EventMarker OnDialogCancel_;
    EventMarker OnDialogChange_;

    Dimension columnHeight_;

    RefPtr<RotationController> rotationController_;

    RefPtr<PickerAnimationController> animationController_;
    RefPtr<DialogComponent> dialogComponent_;
    std::string dialogInsprctorTag_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_BASE_COMPONENT_H
