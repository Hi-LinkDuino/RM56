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

#include "core/components/picker/picker_column_component.h"

#include "base/utils/system_properties.h"
#include "core/components/clip/clip_component.h"
#include "core/components/display/display_component.h"
#include "core/components/flex/flex_component.h"
#include "core/components/picker/picker_column_element.h"
#include "core/components/picker/picker_option_component.h"
#include "core/components/picker/render_picker_column.h"
#include "core/components/scroll/scroll_component.h"

namespace OHOS::Ace {
namespace {

const uint32_t OPTION_COUNT_PHONE_LANDSCAPE = 3;

} // namespace

RefPtr<RenderNode> PickerColumnComponent::CreateRenderNode()
{
    return RenderPickerColumn::Create();
}

RefPtr<Element> PickerColumnComponent::CreateElement()
{
    return AceType::MakeRefPtr<PickerColumnElement>();
}

void PickerColumnComponent::Initialize()
{
    if (!theme_) {
        LOGE("Theme is null.");
        return;
    }

    if (GetCurrentIndex() >= GetOptionCount()) {
        LOGE("current index[%{private}u] is out of range[0~%{private}u).", GetCurrentIndex(), GetOptionCount());
        return;
    }

    std::list<RefPtr<Component>> showOptions;
    uint32_t showCount = theme_->GetShowOptionCount();
    if (SystemProperties::GetDeviceType() == DeviceType::PHONE &&
        SystemProperties::GetDevcieOrientation() == DeviceOrientation::LANDSCAPE) {
        showCount = OPTION_COUNT_PHONE_LANDSCAPE;
    }
    uint32_t selectedIndex = showCount / 2; // the middle option is selected that is current.
    for (uint32_t index = 0; index < showCount; index++) {
        RefPtr<PickerOptionComponent> option = AceType::MakeRefPtr<PickerOptionComponent>();
        option->SetSelected((index == selectedIndex));
        // Has selected item and normal item and disappear item which means at lease 5 items
        // The first and last item is disappear item.
        option->SetDisappear((SystemProperties::GetDeviceType() == DeviceType::PHONE
            && showCount >= 5 && (index == 0 || index == showCount - 1)));
        option->SetAlignTop(false);
        option->SetAlignBottom(false);
        option->SetTheme(theme_->clone());
        option->SetFixHeight(fixHeight_);
        showOptions.emplace_back(option);
    }

    RefPtr<ColumnComponent> column = AceType::MakeRefPtr<ColumnComponent>(
        FlexAlign::CENTER, FlexAlign::CENTER, showOptions);
    column->SetMainAxisSize(MainAxisSize::MIN);
    RefPtr<DisplayComponent> display = AceType::MakeRefPtr<DisplayComponent>(column);
    RefPtr<ClipComponent> columnClip = AceType::MakeRefPtr<ClipComponent>(display);
    columnClip->SetFollowChild(true);
    ClearChildren();
    AppendChild(columnClip);
    if (!GetHasSplitter()) {
        return;
    }
    RefPtr<TextComponent> splitter = AceType::MakeRefPtr<TextComponent>(":"); // use char ':' to split
    auto splitterStyle = theme_->GetOptionStyle(true, false);
    splitterStyle.SetFontWeight(FontWeight::BOLDER);
    splitter->SetTextStyle(splitterStyle);
    splitter->SetFocusColor(splitterStyle.GetTextColor());
    AppendChild(splitter);
}

} // namespace OHOS::Ace
