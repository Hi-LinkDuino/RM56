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

#include "frameworks/bridge/common/dom/dom_picker_base.h"

#include "base/utils/linear_map.h"
#include "base/utils/string_utils.h"
#include "base/utils/utils.h"
#include "frameworks/bridge/common/dom/dom_reflect_map.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

const PickerDate DEFAULT_PICKER_DATE(1970, 1, 1);
const PickerDate DEFAULT_END_PICKER_DATE(2100, 12, 31);
constexpr char PICKER_DATE_SPLITER = '-';
const PickerTime DEFAULT_PICKER_TIME(0, 0, 0);
constexpr char PICKER_TIME_SPLITER = ':';

} // namespace

DOMPickerBase::DOMPickerBase(NodeId nodeId, const std::string& nodeName, bool hasValue)
    : DOMNode(nodeId, nodeName), hasValue_(hasValue)
{
    if (!hasValue_) {
        return;
    }
    valuePickerChild_ = AceType::MakeRefPtr<PickerValueComponent>([weak = WeakClaim(this)]() {
        auto refPtr = weak.Upgrade();
        if (refPtr) {
            refPtr->HandleClickCallback();
        }
    });
    pickerId_ = nodeId;
}

void DOMPickerBase::InitializeStyle()
{
    ResetInitializedStyle();
}

void DOMPickerBase::ResetInitializedStyle()
{
    auto theme = GetTheme<PickerTheme>();
    if (!theme) {
        return;
    }
    theme_ = theme->clone();
    if (!theme_ || !valuePickerChild_) {
        return;
    }
    valuePickerChild_->SetTheme(theme_);
}

RefPtr<Component> DOMPickerBase::GetSpecializedComponent()
{
    if (!hasValue_) {
        return basePickerChild_;
    }
    return valuePickerChild_;
}

void DOMPickerBase::HandleClickCallback() const
{
    if (!hasValue_) {
        return;
    }
    ShowDialog();
}

void DOMPickerBase::ShowDialog() const
{
    if (!basePickerChild_) {
        LOGW("not implementation now.");
        return;
    }
    auto context = GetPipelineContext().Upgrade();
    if (!context) {
        LOGE("Pipeline context not found error!");
        return;
    }
    auto pageStack = context->GetLastStack();
    if (!pageStack) {
        LOGE("Page stack not found error!");
        return;
    }
    basePickerChild_->ShowDialog(pageStack);
}

bool DOMPickerBase::GetPickerDate(const std::string& strDate, PickerDate& outDate) const
{
    std::string strValue;
    std::stringstream streamDate(strDate);
    if (!std::getline(streamDate, strValue, PICKER_DATE_SPLITER)) {
        return false;
    }
    outDate.SetYear(StringUtils::StringToInt(strValue));
    if (!std::getline(streamDate, strValue, PICKER_DATE_SPLITER)) {
        return false;
    }
    outDate.SetMonth(StringUtils::StringToInt(strValue));
    if (!std::getline(streamDate, strValue, PICKER_DATE_SPLITER)) {
        return false;
    }
    outDate.SetDay(StringUtils::StringToInt(strValue));
    return true;
}

bool DOMPickerBase::GetPickerTime(const std::string& strDate, PickerTime& outTime) const
{
    std::string strValue;
    std::stringstream streamDate(strDate);
    if (!std::getline(streamDate, strValue, PICKER_TIME_SPLITER)) {
        return false;
    }
    outTime.SetHour(StringUtils::StringToInt(strValue));
    if (!std::getline(streamDate, strValue, PICKER_TIME_SPLITER)) {
        return false;
    }
    outTime.SetMinute(StringUtils::StringToInt(strValue));
    // the format time hh:mm is supported, so the time should be set
    if (std::getline(streamDate, strValue)) {
        outTime.SetSecond(StringUtils::StringToInt(strValue));
    }
    return true;
}

bool DOMPickerBase::GetPickerDateTime(const std::string& strDate, PickerDateTime& outDateTime) const
{
    std::vector<std::string> strValues;
    StringUtils::StringSpliter(strDate, PICKER_DATE_SPLITER, strValues);
    PickerDate outDate = outDateTime.GetDate();
    PickerTime outTime = outDateTime.GetTime();
    if (strValues.size() == 4) { // MM-dd-hh-mm
        outDate.SetMonth(StringUtils::StringToInt(strValues[0]));
        outDate.SetDay(StringUtils::StringToInt(strValues[1]));
        outTime.SetHour(StringUtils::StringToInt(strValues[2]));
        outTime.SetMinute(StringUtils::StringToInt(strValues[3]));
    } else if (strValues.size() == 5) { // yyyy-MM-dd-hh-mm
        outDate.SetYear(StringUtils::StringToInt(strValues[0]));
        outDate.SetMonth(StringUtils::StringToInt(strValues[1]));
        outDate.SetDay(StringUtils::StringToInt(strValues[2]));
        outTime.SetHour(StringUtils::StringToInt(strValues[3]));
        outTime.SetMinute(StringUtils::StringToInt(strValues[4]));
    } else {
        LOGE("input date time is invalidate.");
        return false;
    }
    outDateTime.SetDate(outDate);
    outDateTime.SetTime(outTime);
    return true;
}

bool DOMPickerBase::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    if (attr.first == DOM_VALUE) {
        if (!hasValue_) {
            LOGE("picker view has no attribute of value.");
            return false;
        }
        std::string strValue = attr.second;
        strValue.erase(
            std::remove_if(strValue.begin(), strValue.end(), [](char c) { return c == DOM_PICKER_SPLIT_ARRAY; }),
            strValue.end());
        valuePickerChild_->SetText(strValue);
        return true;
    }

    if (attr.first == DOM_TYPE) {
        type_ = attr.second;
        return true;
    }

    static const LinearMapNode<void (*)(DOMPickerBase&, const std::string&)> pickerAttrOperators[] = {
        { DOM_PICKER_COLUMN_COUNT, [](DOMPickerBase& picker, const std::string& val) { picker.SetColumnCount(val); } },
        { DOM_PICKER_CONTAIN_SECOND, [](DOMPickerBase& picker, const std::string& val) { picker.SetHasSecond(val); } },
        { DOM_END, [](DOMPickerBase& picker, const std::string& val) { picker.SetEnd(val); } },
        { DOM_PICKER_HOUR24, [](DOMPickerBase& picker, const std::string& val) { picker.SetHour24(val); } },
        { DOM_PICKER_PREFIX, [](DOMPickerBase& picker, const std::string& val) { picker.SetPrefix(val); } },
        { DOM_PICKER_SUFFIX, [](DOMPickerBase& picker, const std::string& val) { picker.SetSuffix(val); } },
        { DOM_PICKER_SHOW_LUNAR, [](DOMPickerBase& picker, const std::string& val) { picker.SetShowLunar(val); } },
        { DOM_PICKER_LUNAR, [](DOMPickerBase& picker, const std::string& val) { picker.SetLunar(val); } },
        { DOM_PICKER_RANGE, [](DOMPickerBase& picker, const std::string& val) { picker.SetRange(val); } },
        { DOM_SELECTED, [](DOMPickerBase& picker, const std::string& val) { picker.SetSelected(val); } },
        { DOM_START, [](DOMPickerBase& picker, const std::string& val) { picker.SetStart(val); } },
        { DOM_PICKER_VIBRATE, [](DOMPickerBase& picker, const std::string& val) { picker.SetVibrate(val); } },
    };
    auto it = BinarySearchFindIndex(pickerAttrOperators, ArraySize(pickerAttrOperators), attr.first.c_str());
    if (it != -1) {
        if (!basePickerChild_) {
            storeAttributes_.emplace_back(attr);
            return true;
        }
        pickerAttrOperators[it].value(*this, attr.second);
        return true;
    }
    return false;
}

bool DOMPickerBase::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    static const LinearMapNode<void (*)(RefPtr<PickerBaseComponent>&, const EventMarker&)> pickerEventOperators[] = {
        { DOM_CANCEL,
            [](RefPtr<PickerBaseComponent>& picker, const EventMarker& event) { picker->SetOnCancel(event); } },
        { DOM_CHANGE,
            [](RefPtr<PickerBaseComponent>& picker, const EventMarker& event) { picker->SetOnChange(event); } },
        { DOM_PICKER_COLUMN_CHANGE,
            [](RefPtr<PickerBaseComponent>& picker, const EventMarker& event) { picker->SetOnColumnChange(event); } },
    };
    auto it = BinarySearchFindIndex(pickerEventOperators, ArraySize(pickerEventOperators), event.c_str());
    if (it != -1) {
        if (!basePickerChild_) {
            storeEvents_.emplace_back(std::make_pair(pageId, event));
            return true;
        }
        pickerEventOperators[it].value(basePickerChild_, EventMarker(GetNodeIdForEvent(), event, pageId));
        return true;
    }
    return false;
}

bool DOMPickerBase::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    if (!hasValue_) {
        return SetOptionStyleOperators(style);
    }
    return SetTextStyleOperators(style);
}

bool DOMPickerBase::SetOptionStyleOperators(const std::pair<std::string, std::string>& style)
{
    static const LinearMapNode<void (*)(const DOMPickerBase&,
        const std::string&, TextStyle&, TextStyle&, TextStyle&, TextStyle&)>
        optionStyleOperators[] = {
            { DOM_TEXT_COLOR, [](const DOMPickerBase& node, const std::string& val, TextStyle& normal, TextStyle&,
                              TextStyle&, TextStyle&) { normal.SetTextColor(node.ParseColor(val)); } },
            { DOM_PICKER_DISAPPEAR_COLOR, [](const DOMPickerBase& node, const std::string& val,
                TextStyle&, TextStyle&, TextStyle&, TextStyle& style) { style.SetTextColor(node.ParseColor(val)); } },
            { DOM_PICKER_DISAPPEAR_FONT_SIZE, [](const DOMPickerBase& node, const std::string& val,
                TextStyle&, TextStyle&, TextStyle&, TextStyle& style) {
                style.SetFontSize(node.ParseDimension(val));
                style.SetAdaptMaxFontSize(node.ParseDimension(val)); } },
            { DOM_PICKER_FOCUS_COLOR, [](const DOMPickerBase& node, const std::string& val, TextStyle&, TextStyle&,
                                      TextStyle& focus, TextStyle&) { focus.SetTextColor(node.ParseColor(val)); } },
            { DOM_PICKER_FOCUS_SIZE,
                [](const DOMPickerBase& node, const std::string& val,
                    TextStyle&, TextStyle&, TextStyle& focus, TextStyle&) {
                    focus.SetFontSize(node.ParseDimension(val));
                    focus.SetAdaptMaxFontSize(node.ParseDimension(val));
                } },
            { DOM_TEXT_FONT_FAMILY,
                [](const DOMPickerBase& node, const std::string& val, TextStyle& normal, TextStyle& select,
                    TextStyle& focus, TextStyle& disappear) {
                    normal.SetFontFamilies(node.ParseFontFamilies(val));
                    select.SetFontFamilies(node.ParseFontFamilies(val));
                    focus.SetFontFamilies(node.ParseFontFamilies(val));
                    disappear.SetFontFamilies(node.ParseFontFamilies(val));
                } },
            { DOM_TEXT_FONT_SIZE,
                [](const DOMPickerBase& node, const std::string& val,
                    TextStyle& normal, TextStyle&, TextStyle&, TextStyle&) {
                    normal.SetFontSize(node.ParseDimension(val));
                    normal.SetAdaptMaxFontSize(node.ParseDimension(val));
                } },
            { DOM_PICKER_SELECT_COLOR, [](const DOMPickerBase& node, const std::string& val,
                                       TextStyle&, TextStyle& select,
                                       TextStyle&, TextStyle&) { select.SetTextColor(node.ParseColor(val)); } },
            { DOM_PICKER_SELECT_SIZE,
                [](const DOMPickerBase& node, const std::string& val,
                    TextStyle&, TextStyle& select, TextStyle&, TextStyle&) {
                    select.SetFontSize(node.ParseDimension(val));
                    select.SetAdaptMaxFontSize(node.ParseDimension(val));
                } },
        };
    auto styleIter = BinarySearchFindIndex(optionStyleOperators, ArraySize(optionStyleOperators), style.first.c_str());
    if (styleIter != -1) {
        if (!basePickerChild_) {
            storeStyles_.emplace_back(style);
            return true;
        }
        auto theme = basePickerChild_->GetTheme();
        auto normalStyle = theme->GetOptionStyle(false, false);
        auto selectStyle = theme->GetOptionStyle(true, false);
        auto focusStyle = theme->GetOptionStyle(true, true);
        auto disappearStyle = theme->GetDisappearOptionStyle();
        optionStyleOperators[styleIter].value(*this, style.second,
            normalStyle, selectStyle, focusStyle, disappearStyle);
        theme->SetOptionStyle(false, false, normalStyle);
        theme->SetOptionStyle(true, false, selectStyle);
        theme->SetOptionStyle(true, true, focusStyle);
        theme->SetDisappearOptionStyle(disappearStyle);
        return true;
    }
    return false;
}

bool DOMPickerBase::SetTextBackgroundColor(const std::pair<std::string, std::string>& style)
{
    if (style.first != DOM_BACKGROUND_COLOR) {
        return false;
    }
    auto decoration = theme_->GetOptionDecoration(false);
    if (!decoration) {
        return false;
    }
    decoration->SetBackgroundColor(ParseColor(style.second));
    return true;
}

bool DOMPickerBase::SetTextStyleOperators(const std::pair<std::string, std::string>& style)
{
    static const LinearMapNode<void (*)(const DOMPickerBase&, const std::string&, TextStyle&, TextStyle&)>
        textStyleOperators[] = {
            { DOM_TEXT_ALLOW_SCALE,
                [](const DOMPickerBase& node, const std::string& val, TextStyle& normal, TextStyle& focus) {
                    normal.SetAllowScale(StringToBool(val));
                    focus.SetAllowScale(StringToBool(val));
                } },
            { DOM_TEXT_FONT_FAMILY,
                [](const DOMPickerBase& node, const std::string& val, TextStyle& normal, TextStyle& focus) {
                    normal.SetFontFamilies(node.ParseFontFamilies(val));
                    focus.SetFontFamilies(node.ParseFontFamilies(val));
                } },
            { DOM_TEXT_FONT_SIZE,
                [](const DOMPickerBase& node, const std::string& val, TextStyle& normal, TextStyle& focus) {
                    normal.SetFontSize(node.ParseDimension(val));
                    normal.SetAdaptMaxFontSize(node.ParseDimension(val));
                    focus.SetFontSize(node.ParseDimension(val));
                    focus.SetAdaptMaxFontSize(node.ParseDimension(val));
                } },
            { DOM_TEXT_FONT_STYLE,
                [](const DOMPickerBase& node, const std::string& val, TextStyle& normal, TextStyle& focus) {
                    normal.SetFontStyle(ConvertStrToFontStyle(val));
                    focus.SetFontStyle(ConvertStrToFontStyle(val));
                } },
            { DOM_TEXT_FONT_WEIGHT,
                [](const DOMPickerBase& node, const std::string& val, TextStyle& normal, TextStyle& focus) {
                    normal.SetFontWeight(ConvertStrToFontWeight(val));
                    focus.SetFontWeight(ConvertStrToFontWeight(val));
                } },
            { DOM_TEXT_LETTER_SPACING,
                [](const DOMPickerBase& node, const std::string& val, TextStyle& normal, TextStyle& focus) {
                    normal.SetLetterSpacing(node.ParseDimension(val));
                    focus.SetLetterSpacing(node.ParseDimension(val));
                } },
            { DOM_TEXT_LINE_HEIGHT,
                [](const DOMPickerBase& node, const std::string& val, TextStyle& normal, TextStyle& focus) {
                    normal.SetLineHeight(node.ParseLineHeight(val));
                    focus.SetLineHeight(node.ParseLineHeight(val));
                } },
            { DOM_PICKER_TEXT_COLOR,
                [](const DOMPickerBase& node, const std::string& val, TextStyle& normal, TextStyle& focus) {
                    normal.SetTextColor(node.ParseColor(val));
                    focus.SetTextColor(node.ParseColor(val));
                } },
            { DOM_TEXT_DECORATION,
                [](const DOMPickerBase& node, const std::string& val, TextStyle& normal, TextStyle& focus) {
                    normal.SetTextDecoration(ConvertStrToTextDecoration(val));
                    focus.SetTextDecoration(ConvertStrToTextDecoration(val));
                } },
        };
    auto operatorIter = BinarySearchFindIndex(textStyleOperators, ArraySize(textStyleOperators), style.first.c_str());
    if (operatorIter != -1) {
        auto normalStyle = theme_->GetOptionStyle(false, false);
        auto focusStyle = theme_->GetOptionStyle(true, true);
        textStyleOperators[operatorIter].value(*this, style.second, normalStyle, focusStyle);
        theme_->SetOptionStyle(false, false, normalStyle);
        theme_->SetOptionStyle(true, false, normalStyle);
        theme_->SetOptionStyle(true, true, focusStyle);
        return true;
    }
    if (SetColumnHeight(style)) {
        return true;
    }
    if (SetTextBackgroundColor(style)) {
        return true;
    }
    return false;
}

void DOMPickerBase::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    if (!basePickerChild_) {
        LOGW("not implementation now.");
        return;
    }
    if (method == DOM_ROTATION) {
        auto controller = basePickerChild_->GetRotationController();
        if (controller) {
            LOGD("Rotation focus picker view");
            controller->RequestRotation(true);
        }
    }
    if (!hasValue_) {
        return;
    }
    if (method == DOM_SHOW) {
        ShowDialog();
    }
}

void DOMPickerBase::PrepareSpecializedComponent()
{
    if (valuePickerChild_) {
        valuePickerChild_->SetTextDirection((IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR));
    }
    if (CreatePicker()) {
        for (const auto& attribute : storeAttributes_) {
            SetSpecializedAttr(attribute);
        }
        for (const auto& oneEvent : storeEvents_) {
            AddSpecializedEvent(oneEvent.first, oneEvent.second);
        }
        for (const auto& oneStyle : storeStyles_) {
            SetSpecializedStyle(oneStyle);
        }
    }
    storeAttributes_.clear();
    storeEvents_.clear();
    storeStyles_.clear();
}

void DOMPickerBase::CompositeComponents()
{
    if (basePickerChild_ && type_ == DOM_PICKER_TYPE_DATE &&
        AceType::InstanceOf<PickerDateComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerDateComponent>(basePickerChild_);
        // check invalidate of start and end.
        const auto& start = picker->GetStartDate();
        const auto& end = picker->GetEndDate();
        if (start.GetYear() > end.GetYear() ||
            (start.GetYear() == end.GetYear() && start.GetMonth() > end.GetMonth()) ||
            (start.GetYear() == end.GetYear() && start.GetMonth() == end.GetMonth() && start.GetDay() > end.GetDay())) {
            // invalidate => use default start date and end date.
            picker->SetStartDate(DEFAULT_PICKER_DATE); // default start date is 1970-1-1 from FA document.
            picker->SetEndDate(DEFAULT_END_PICKER_DATE); // default end date is 2100-12-31 from FA document.
        }
    }
    if (IsRightToLeft()) {
        SetAlignment(Alignment::CENTER_RIGHT);
    }
    DOMNode::CompositeComponents();
}

bool DOMPickerBase::CreatePicker()
{
    // The type cannot be dynamically changed.
    if (basePickerChild_) {
        return true;
    }

    // Operator map for type.
    static const std::unordered_map<std::string, void (*)(RefPtr<PickerBaseComponent>&)> pickerOperators = {
        { DOM_PICKER_TYPE_TIME,
            [](RefPtr<PickerBaseComponent>& pickerBase) {
                pickerBase = AceType::MakeRefPtr<PickerTimeComponent>(); } },
        { DOM_PICKER_TYPE_DATE,
            [](RefPtr<PickerBaseComponent>& pickerBase) {
                pickerBase = AceType::MakeRefPtr<PickerDateComponent>(); } },
        { DOM_PICKER_TYPE_TEXT,
            [](RefPtr<PickerBaseComponent>& pickerBase) {
                pickerBase = AceType::MakeRefPtr<PickerTextComponent>(); } },
        { DOM_PICKER_TYPE_MULTITEXT,
            [](RefPtr<PickerBaseComponent>& pickerBase) {
                pickerBase = AceType::MakeRefPtr<PickerMultiTextComponent>();
            } },
        { DOM_PICKER_TYPE_DATETIME,
            [](RefPtr<PickerBaseComponent>& pickerBase) {
                pickerBase = AceType::MakeRefPtr<PickerDateTimeComponent>();
            } },
    };

    auto operatorIter = pickerOperators.find(type_);
    if (operatorIter != pickerOperators.end()) {
        operatorIter->second(basePickerChild_);
    } else {
        basePickerChild_ = AceType::MakeRefPtr<PickerTextComponent>();
        type_ = DOM_PICKER_TYPE_TEXT;
    }

    if (basePickerChild_) {
        auto theme = GetTheme<PickerTheme>();
        basePickerChild_->SetTheme(theme->clone());
        basePickerChild_->SetTextDirection((IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR));
        basePickerChild_->SetIsDialog(hasValue_);
        basePickerChild_->SetNodeId(GetNodeId());
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
        basePickerChild_->SetPickerBaseId(pickerId_);
#endif
        return true;
    }

    LOGE("unsupported type: %{private}s.", type_.c_str());
    return false;
}

bool DOMPickerBase::SetColumnHeight(const std::pair<std::string, std::string>& style)
{
    if (style.first != DOM_PICKER_COLUMN_HEIGHT) {
        return false;
    }

    if (!hasValue_) {
        return false;
    }

    if (!basePickerChild_) {
        storeStyles_.emplace_back(style);
        return false;
    }

    basePickerChild_->SetColumnHeight(StringToDimension(style.second));
    return true;
}

bool DOMPickerBase::SetStart(const std::string& value)
{
    if (basePickerChild_ && type_ == DOM_PICKER_TYPE_DATE &&
        AceType::InstanceOf<PickerDateComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerDateComponent>(basePickerChild_);
        auto date = DEFAULT_PICKER_DATE;
        if (GetPickerDate(value, date)) {
            picker->SetStartDate(date);
        } else {
            LOGE("the value of picker date is invalid.");
        }

        return true;
    }

    return false;
}

bool DOMPickerBase::SetEnd(const std::string& value)
{
    if (basePickerChild_ && type_ == DOM_PICKER_TYPE_DATE &&
        AceType::InstanceOf<PickerDateComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerDateComponent>(basePickerChild_);
        auto date = DEFAULT_PICKER_DATE;
        if (GetPickerDate(value, date)) {
            picker->SetEndDate(date);
        } else {
            LOGE("input value of picker date is invalid.");
        }

        return true;
    }

    return false;
}

bool DOMPickerBase::SetSelected(const std::string& value)
{
    return (SetTextSelected(value) || SetDateTimeSelected(value));
}

bool DOMPickerBase::SetTextSelected(const std::string& value)
{
    if (basePickerChild_ && type_ == DOM_PICKER_TYPE_TEXT &&
        AceType::InstanceOf<PickerTextComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerTextComponent>(basePickerChild_);
        picker->SetSelected(StringUtils::StringToInt(value));
        return true;
    }

    if (basePickerChild_ && type_ == DOM_PICKER_TYPE_MULTITEXT &&
        AceType::InstanceOf<PickerMultiTextComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerMultiTextComponent>(basePickerChild_);
        std::vector<std::string> out;
        StringUtils::StringSpliter(value, DOM_PICKER_SPLIT_ARRAY, out);
        std::vector<uint32_t> selectedIndexes;
        for (uint32_t index = 0; index < out.size(); ++index) {
            selectedIndexes.emplace_back(StringUtils::StringToInt(out[index]));
        }
        picker->SetSelected(selectedIndexes);
        return true;
    }

    return false;
}

bool DOMPickerBase::SetDateTimeSelected(const std::string& value)
{
    if (basePickerChild_ && type_ == DOM_PICKER_TYPE_DATE &&
        AceType::InstanceOf<PickerDateComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerDateComponent>(basePickerChild_);
        auto date = DEFAULT_PICKER_DATE;
        if (GetPickerDate(value, date)) {
            picker->SetSelectedDate(date);
        } else {
            LOGE("input value of picker date is invalid.");
        }

        return true;
    }

    if (basePickerChild_ && type_ == DOM_PICKER_TYPE_TIME &&
        AceType::InstanceOf<PickerTimeComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerTimeComponent>(basePickerChild_);
        auto time = DEFAULT_PICKER_TIME;
        if (GetPickerTime(value, time)) {
            picker->SetSelectedTime(time);
        } else {
            LOGE("input value of picker time is invalid.");
        }
        return true;
    }

    if (basePickerChild_ && type_ == DOM_PICKER_TYPE_DATETIME &&
        AceType::InstanceOf<PickerDateTimeComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerDateTimeComponent>(basePickerChild_);
        auto dateTime = PickerDateTime::Current();
        if (GetPickerDateTime(value, dateTime)) {
            picker->SetSelectedDateTime(dateTime);
        } else {
            LOGE("input value of picker date time is invalid.");
        }

        return true;
    }

    return false;
}

bool DOMPickerBase::SetHasSecond(const std::string& value)
{
    if (basePickerChild_ && type_ == DOM_PICKER_TYPE_TIME &&
        AceType::InstanceOf<PickerTimeComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerTimeComponent>(basePickerChild_);
        picker->SetHasSecond((value == "true")); // bool attribute's value is "true" and "false".
        return true;
    }

    return false;
}

bool DOMPickerBase::SetRange(const std::string& value)
{
    if (basePickerChild_ && type_ == DOM_PICKER_TYPE_TEXT &&
        AceType::InstanceOf<PickerTextComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerTextComponent>(basePickerChild_);
        std::vector<std::string> out;
        StringUtils::StringSpliter(value, DOM_PICKER_SPLIT_ARRAY, out);
        picker->SetRange(out);
        return true;
    }

    if (basePickerChild_ && type_ == DOM_PICKER_TYPE_MULTITEXT &&
        AceType::InstanceOf<PickerMultiTextComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerMultiTextComponent>(basePickerChild_);
        std::vector<std::string> out;
        StringUtils::StringSpliter(value, DOM_PICKER_SPLIT_ARRAY, out);
        std::vector<std::vector<std::string>> arrayRange;
        for (uint32_t index = 0; index < out.size(); ++index) {
            std::vector<std::string> textRange;
            StringUtils::StringSpliter(out[index], DOM_PICKER_SPLIT_ITEM, textRange);
            arrayRange.emplace_back(textRange);
        }
        picker->SetRange(arrayRange);
        return true;
    }

    return false;
}

bool DOMPickerBase::SetColumnCount(const std::string& value)
{
    if (basePickerChild_ && type_ == DOM_PICKER_TYPE_MULTITEXT &&
        AceType::InstanceOf<PickerMultiTextComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerMultiTextComponent>(basePickerChild_);
        picker->SetColumnCount(StringUtils::StringToInt(value));
        return true;
    }

    return false;
}

bool DOMPickerBase::SetHour24(const std::string& value)
{
    if (basePickerChild_ && (type_ == DOM_PICKER_TYPE_DATETIME || type_ == DOM_PICKER_TYPE_TIME) &&
        AceType::InstanceOf<PickerTimeComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerTimeComponent>(basePickerChild_);
        if (value == "24") {
            picker->SetHour24(true);
        } else if (value == "12") {
            picker->SetHour24(false);
        } else {
            LOGE("value of hour24 is invalid.");
        }
        return true;
    }

    return false;
}

bool DOMPickerBase::SetShowLunar(const std::string& value)
{
    static const char* FALSE = "false";
    static const char* TRUE = "true";

    if (basePickerChild_ && type_ == DOM_PICKER_TYPE_DATETIME &&
        AceType::InstanceOf<PickerDateTimeComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerDateTimeComponent>(basePickerChild_);
        if (value == TRUE) {
            picker->SetShowLunar(true);
        } else if (value == FALSE) {
            picker->SetShowLunar(false);
        } else {
            LOGE("value of show lunar is invalid.");
        }
        return true;
    }

    if (basePickerChild_ && type_ == DOM_PICKER_TYPE_DATE &&
        AceType::InstanceOf<PickerDateComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerDateComponent>(basePickerChild_);
        if (value == TRUE) {
            picker->SetShowLunar(true);
        } else if (value == FALSE) {
            picker->SetShowLunar(false);
        } else {
            LOGE("value of show lunar is invalid.");
        }
        return true;
    }

    return false;
}

bool DOMPickerBase::SetLunar(const std::string& value)
{
    static const char* FALSE = "false";
    static const char* TRUE = "true";

    if (basePickerChild_ && type_ == DOM_PICKER_TYPE_DATETIME &&
        AceType::InstanceOf<PickerDateTimeComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerDateTimeComponent>(basePickerChild_);
        if (value == TRUE) {
            picker->SetHasLunar(true);
        } else if (value == FALSE) {
            picker->SetHasLunar(false);
        } else {
            LOGE("value of lunar is invalid.");
        }
        return true;
    }

    if (basePickerChild_ && type_ == DOM_PICKER_TYPE_DATE &&
        AceType::InstanceOf<PickerDateComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerDateComponent>(basePickerChild_);
        if (value == TRUE) {
            picker->SetHasLunar(true);
        } else if (value == FALSE) {
            picker->SetHasLunar(false);
        } else {
            LOGE("value of lunar is invalid.");
        }
        return true;
    }

    return false;
}

bool DOMPickerBase::SetPrefix(const std::string& value)
{
    if (basePickerChild_ && type_ == DOM_PICKER_TYPE_TEXT &&
        AceType::InstanceOf<PickerTextComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerTextComponent>(basePickerChild_);
        picker->SetPrefix(value);
        return true;
    }
    return false;
}

bool DOMPickerBase::SetSuffix(const std::string& value)
{
    if (basePickerChild_ && type_ == DOM_PICKER_TYPE_TEXT &&
        AceType::InstanceOf<PickerTextComponent>(basePickerChild_)) {
        auto picker = AceType::DynamicCast<PickerTextComponent>(basePickerChild_);
        picker->SetSuffix(value);
        return true;
    }
    return false;
}

bool DOMPickerBase::SetVibrate(const std::string& value)
{
    static const char* FALSE = "false";
    static const char* TRUE = "true";

    if (basePickerChild_) {
        if (value == TRUE) {
            basePickerChild_->SetNeedVibrate(true);
        } else if (value == FALSE) {
            basePickerChild_->SetNeedVibrate(false);
        } else {
            LOGE("value of lunar is invalid.");
        }
        return true;
    }
    return false;
}

} // namespace OHOS::Ace::Framework
