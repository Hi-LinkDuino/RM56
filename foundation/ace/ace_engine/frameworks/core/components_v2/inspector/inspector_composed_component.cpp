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

#include "core/components_v2/inspector/inspector_composed_component.h"

#include <unordered_map>

#include "base/utils/string_utils.h"
#include "core/common/container.h"
#include "core/components_v2/inspector/actionsheetdialog_composed_element.h"
#include "core/components_v2/inspector/alertdialog_composed_element.h"
#include "core/components_v2/inspector/badge_composed_element.h"
#include "core/components_v2/inspector/blank_composed_element.h"
#include "core/components_v2/inspector/button_composed_element.h"
#include "core/components_v2/inspector/calendar_composed_element.h"
#include "core/components_v2/inspector/checkbox_composed_element.h"
#include "core/components_v2/inspector/checkboxGroup_composed_element.h"
#include "core/components_v2/inspector/column_composed_element.h"
#include "core/components_v2/inspector/column_split_composed_element.h"
#include "core/components_v2/inspector/counter_composed_element.h"
#include "core/components_v2/inspector/customdialog_composed_element.h"
#include "core/components_v2/inspector/data_panel_composed_element.h"
#include "core/components_v2/inspector/date_picker_composed_element.h"
#include "core/components_v2/inspector/date_picker_dialog_composed_element.h"
#include "core/components_v2/inspector/divider_composed_element.h"
#include "core/components_v2/inspector/flex_composed_element.h"
#include "core/components_v2/inspector/gauge_composed_element.h"
#include "core/components_v2/inspector/grid_composed_element.h"
#include "core/components_v2/inspector/grid_container_composed_element.h"
#include "core/components_v2/inspector/grid_item_composed_element.h"
#include "core/components_v2/inspector/hyperlink_composed_element.h"
#include "core/components_v2/inspector/image_animator_composed_element.h"
#include "core/components_v2/inspector/image_composed_element.h"
#include "core/components_v2/inspector/indexer_composed_element.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/components_v2/inspector/list_composed_element.h"
#include "core/components_v2/inspector/list_item_composed_element.h"
#include "core/components_v2/inspector/loading_progress_composed_element.h"
#include "core/components_v2/inspector/marquee_composed_element.h"
#include "core/components_v2/inspector/menu_composed_element.h"
#include "core/components_v2/inspector/navigation_composed_element.h"
#include "core/components_v2/inspector/navigator_composed_element.h"
#include "core/components_v2/inspector/panel_composed_element.h"
#include "core/components_v2/inspector/picker_text_dialog_composed_element.h"
#include "core/components_v2/inspector/progress_composed_element.h"
#include "core/components_v2/inspector/qrcode_composed_element.h"
#include "core/components_v2/inspector/radio_composed_element.h"
#include "core/components_v2/inspector/rating_composed_element.h"
#include "core/components_v2/inspector/refresh_composed_element.h"
#include "core/components_v2/inspector/row_composed_element.h"
#include "core/components_v2/inspector/row_split_composed_element.h"
#include "core/components_v2/inspector/scroll_bar_composed_element.h"
#include "core/components_v2/inspector/scroll_composed_element.h"
#include "core/components_v2/inspector/search_composed_element.h"
#include "core/components_v2/inspector/select_composed_element.h"
#include "core/components_v2/inspector/shape_composed_element.h"
#include "core/components_v2/inspector/shape_container_composed_element.h"
#include "core/components_v2/inspector/sheet_composed_element.h"
#include "core/components_v2/inspector/side_bar_composed_element.h"
#include "core/components_v2/inspector/slider_composed_element.h"
#include "core/components_v2/inspector/span_composed_element.h"
#include "core/components_v2/inspector/stack_composed_element.h"
#include "core/components_v2/inspector/stepper_composed_element.h"
#include "core/components_v2/inspector/stepper_item_composed_element.h"
#include "core/components_v2/inspector/swiper_composed_element.h"
#include "core/components_v2/inspector/switch_composed_element.h"
#include "core/components_v2/inspector/tab_content_composed_element.h"
#include "core/components_v2/inspector/tabs_composed_element.h"
#include "core/components_v2/inspector/text_clock_composed_element.h"
#include "core/components_v2/inspector/text_composed_element.h"
#include "core/components_v2/inspector/text_picker_composed_element.h"
#include "core/components_v2/inspector/textarea_composed_element.h"
#include "core/components_v2/inspector/textinput_composed_element.h"
#include "core/components_v2/inspector/texttimer_composed_element.h"
#include "core/components_v2/inspector/time_picker_composed_element.h"
#include "core/components_v2/inspector/time_picker_dialog_composed_element.h"
#include "core/components_v2/inspector/toggle_composed_element.h"
#include "core/components_v2/inspector/wrap_composed_element.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace::V2 {

namespace {

using CreateElementFunc = std::function<RefPtr<InspectorComposedElement>(const std::string& id)>;

const std::unordered_map<std::string, CreateElementFunc> CREATE_ELEMENT_MAP {
    { COLUMN_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::ColumnComposedElement>(id); } },
    { COLUMN_SPLIT_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::ColumnSplitComposedElement>(id); } },
    { COUNTER_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::CounterComposedElement>(id); } },
    { NAVIGATION_VIEW_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::NavigationComposedElement>(id); } },
    { ROW_SPLIT_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::RowSplitComposedElement>(id); } },
    { STACK_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::StackComposedElement>(id); } },
    { SWIPER_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::SwiperComposedElement>(id); } },
    { TAB_CONTENT_ITEM_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::TabContentComposedElement>(id); } },
    { TABS_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::TabsComposedElement>(id); } },
    { TEXT_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::TextComposedElement>(id); } },
    { FLEX_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::FlexComposedElement>(id); } },
    { WRAP_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::WrapComposedElement>(id); } },
    { GRID_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::GridComposedElement>(id); } },
    { GRID_ITEM_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::GridItemComposedElement>(id); } },
    { LIST_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::ListComposedElement>(id); } },
    { LIST_ITEM_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::ListItemComposedElement>(id); } },
    { NAVIGATOR_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::NavigatorComposedElement>(id); } },
    { PANEL_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::PanelComposedElement>(id); } },
    { ROW_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::RowComposedElement>(id); } },
    { SHAPE_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::ShapeComposedElement>(id); } },
    { SHAPE_CONTAINER_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::ShapeContainerComposedElement>(id); } },
    { IMAGE_ANIMATOR_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::ImageAnimatorComposedElement>(id); } },
    { IMAGE_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::ImageComposedElement>(id); } },
    { QRCODE_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::QrcodeComposedElement>(id); } },
    { SPAN_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::SpanComposedElement>(id); } },
    { BOX_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::BlankComposedElement>(id); } },
    { BUTTON_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::ButtonComposedElement>(id); } },
    { DIVIDER_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::DividerComposedElement>(id); } },
    { CHECKBOX_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::CheckboxComposedElement>(id); } },
    { CHECK_BOX_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::CheckboxComposedElement>(id); } },
    { SWITCH_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::SwitchComposedElement>(id); } },
    { TOGGLE_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::ToggleComposedElement>(id); } },
    { SCROLL_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::ScrollComposedElement>(id); } },
    { CALENDAR_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::CalendarComposedElement>(id); } },
    { BADGE_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::BadgeComposedElement>(id); } },
    { SEARCH_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::SearchComposedElement>(id); } },
    { FORM_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::InspectorComposedElement>(id); } },
    { VIDEO_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::InspectorComposedElement>(id); } },
    { INDEXER_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::IndexerComposedElement>(id); } },
    { SLIDER_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::SliderComposedElement>(id); } },
    { RATING_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::RatingComposedElement>(id); } },
    { PROGRESS_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::ProgressComposedElement>(id); } },
    { DATA_PANEL_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::DataPanelComposedElement>(id); } },
    { SHEET_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::SheetComposedElement>(id); } },
    { HYPERLINK_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::HyperlinkComposedElement>(id); } },
    { STEPPER_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::StepperComposedElement>(id); } },
    { STEPPER_ITEM_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::StepperItemComposedElement>(id); } },
    { SCROLL_BAR_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::ScrollBarComposedElement>(id); } },
    { REFRESH_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::RefreshComposedElement>(id); } },
    { DATE_PICKER_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::DatePickerComposedElement>(id); } },
    { TIME_PICKER_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::TimePickerComposedElement>(id); } },
    { RADIO_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::RadioComposedElement>(id); } },
    { GAUGE_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::GaugeComposedElement>(id); } },
    { GRIDCONTAINER_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::GridContainerComposedElement>(id); } },
    { MENU_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::MenuComposedElement>(id); } },
    { TEXTAREA_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::TextareaComposedElement>(id); } },
    { TEXTINPUT_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::TextInputComposedElement>(id); } },
    { TEXTTIMER_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::TextTimerComposedElement>(id); } },
    { SELECT_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::SelectComposedElement>(id); } },
    { MARQUEE_COMPONENT_TAG,
        [](const std::string& id) { return AceType::MakeRefPtr<V2::MarqueeComposedElement>(id); } },
    { TEXTCLOCK_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::TextClockComposedElement>(id); } },
    { TEXT_PICKER_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::TextPickerComposedElement>(id); } },
    { PICKER_TEXT_DIALOG_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::PickerTextDialogComposedElement>(id); } },
    { CANVAS_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::InspectorComposedElement>(id); } },
    { ACTIONSHEETDIALOG_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::ActionSheetDialogComposedElement>(id); } },
    { ALERTDIALOG_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::AlertDialogComposedElement>(id); } },
    { CUSTOMDIALOG_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::CustomDialogComposedElement>(id); } },
    { DATE_PICKER_DIALOG_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::DatePickerDialogComposedElement>(id); } },
    { SIDE_BAR_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::SideBarComposedElement>(id); } },
    { LOADING_PROGRESS_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::LoadingProgressComposedElement>(id); } },
    { CHECKBOXGROUP_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::CheckboxGroupComposedElement>(id); } },
    { TIME_PICKER_DIALOG_COMPONENT_TAG,
        [](const std::string& id) {return AceType::MakeRefPtr<V2::TimePickerDialogComposedElement>(id); } }
};

} // namespace

const std::unordered_map<std::string, std::string> COMPONENT_TAG_TO_ETS_TAG_MAP {
    { COLUMN_COMPONENT_TAG, COLUMN_ETS_TAG },
    { TEXT_COMPONENT_TAG, TEXT_ETS_TAG },
    { COLUMN_SPLIT_COMPONENT_TAG, COLUMN_SPLIT_ETS_TAG },
    { COUNTER_COMPONENT_TAG, COUNTER_ETS_TAG },
    { NAVIGATION_VIEW_COMPONENT_TAG, NAVIGATION_VIEW_ETS_TAG },
    { ROW_SPLIT_COMPONENT_TAG, ROW_SPLIT_ETS_TAG },
    { STACK_COMPONENT_TAG, STACK_ETS_TAG },
    { SWIPER_COMPONENT_TAG, SWIPER_ETS_TAG },
    { TAB_CONTENT_ITEM_COMPONENT_TAG, TAB_CONTENT_ITEM_ETS_TAG },
    { TABS_COMPONENT_TAG, TABS_ETS_TAG },
    { FLEX_COMPONENT_TAG, FLEX_ETS_TAG },
    { WRAP_COMPONENT_TAG, WRAP_ETS_TAG },
    { GRID_COMPONENT_TAG, GRID_ETS_TAG },
    { GRID_ITEM_COMPONENT_TAG, GRID_ITEM_ETS_TAG },
    { LIST_COMPONENT_TAG, LIST_ETS_TAG },
    { LIST_ITEM_COMPONENT_TAG, LIST_ITEM_ETS_TAG },
    { NAVIGATOR_COMPONENT_TAG, NAVIGATOR_ETS_TAG },
    { PANEL_COMPONENT_TAG, PANEL_ETS_TAG },
    { ROW_COMPONENT_TAG, ROW_ETS_TAG },
    { IMAGE_ANIMATOR_COMPONENT_TAG, IMAGE_ANIMATOR_ETS_TAG },
    { SHAPE_CONTAINER_COMPONENT_TAG, SHAPE_CONTAINER_ETS_TAG },
    { SHAPE_COMPONENT_TAG, SHAPE_ETS_TAG },
    { IMAGE_COMPONENT_TAG, IMAGE_ETS_TAG },
    { QRCODE_COMPONENT_TAG, QRCODE_ETS_TAG },
    { SPAN_COMPONENT_TAG, SPAN_ETS_TAG },
    { BOX_COMPONENT_TAG, BLANK_ETS_TAG },
    { BUTTON_COMPONENT_TAG, BUTTON_ETS_TAG },
    { DIVIDER_COMPONENT_TAG, DIVIDER_ETS_TAG },
    { CHECKBOX_COMPONENT_TAG, CHECKBOX_ETS_TAG },
    { CHECK_BOX_COMPONENT_TAG, CHECK_BOX_ETS_TAG },
    { SWITCH_COMPONENT_TAG, SWITCH_ETS_TAG },
    { TOGGLE_COMPONENT_TAG, TOGGLE_ETS_TAG },
    { SCROLL_COMPONENT_TAG, SCROLL_ETS_TAG },
    { CALENDAR_COMPONENT_TAG, CALENDAR_ETS_TAG },
    { BADGE_COMPONENT_TAG, BADGE_ETS_TAG },
    { SEARCH_COMPONENT_TAG, SEARCH_ETS_TAG },
    { FORM_COMPONENT_TAG, FORM_ETS_TAG },
    { VIDEO_COMPONENT_TAG, VIDEO_ETS_TAG },
    { INDEXER_COMPONENT_TAG, INDEXER_ETS_TAG },
    { SLIDER_COMPONENT_TAG, SLIDER_ETS_TAG },
    { RATING_COMPONENT_TAG, RATING_ETS_TAG },
    { PROGRESS_COMPONENT_TAG, PROGRESS_ETS_TAG },
    { DATA_PANEL_COMPONENT_TAG, DATA_PANEL_ETS_TAG },
    { SHEET_COMPONENT_TAG, SHEET_ETS_TAG },
    { HYPERLINK_COMPONENT_TAG, HYPERLINK_ETS_TAG },
    { STEPPER_COMPONENT_TAG, STEPPER_ETS_TAG },
    { STEPPER_ITEM_COMPONENT_TAG, STEPPER_ITEM_ETS_TAG },
    { SCROLL_BAR_COMPONENT_TAG, SCROLL_BAR_ETS_TAG },
    { REFRESH_COMPONENT_TAG, REFRESH_ETS_TAG },
    { DATE_PICKER_COMPONENT_TAG, DATE_PICKER_ETS_TAG },
    { TIME_PICKER_COMPONENT_TAG, TIME_PICKER_ETS_TAG },
    { RADIO_COMPONENT_TAG, RADIO_ETS_TAG },
    { GRIDCONTAINER_COMPONENT_TAG, GRIDCONTAINER_ETS_TAG },
    { INDEXER_COMPONENT_TAG, INDEXER_ETS_TAG },
    { MENU_COMPONENT_TAG, MENU_ETS_TAG },
    { MENU_TAG, MENU_ETS_TAG },
    { TEXTAREA_COMPONENT_TAG, TEXTAREA_ETS_TAG },
    { TEXTINPUT_COMPONENT_TAG, TEXTINPUT_ETS_TAG },
    { MARQUEE_COMPONENT_TAG, MARQUEE_ETS_TAG },
    { SELECT_COMPONENT_TAG, SELECT_ETS_TAG },
    { TEXTCLOCK_COMPONENT_TAG, TEXTCLOCK_ETS_TAG },
    { TEXTTIMER_COMPONENT_TAG, TEXTTIMER_ETS_TAG },
    { TEXT_PICKER_COMPONENT_TAG, TEXT_PICKER_ETS_TAG },
    { PICKER_TEXT_DIALOG_COMPONENT_TAG, PICKER_TEXT_DIALOG_ETS_TAG },
    { CANVAS_COMPONENT_TAG, CANVAS_ETS_TAG },
    { ACTIONSHEETDIALOG_COMPONENT_TAG, ACTIONSHEETDIALOG_ETS_TAG },
    { ALERTDIALOG_COMPONENT_TAG, ALERTDIALOG_ETS_TAG },
    { CUSTOMDIALOG_COMPONENT_TAG, CUSTOMDIALOG_ETS_TAG },
    { DATE_PICKER_DIALOG_COMPONENT_TAG, DATE_PICKER_DIALOG_ETS_TAG },
    { SIDE_BAR_COMPONENT_TAG, SIDE_BAR_ETS_TAG },
    { LOADING_PROGRESS_COMPONENT_TAG, LOADING_PROGRESS_ETS_TAG },
    { CHECKBOXGROUP_COMPONENT_TAG, CHECKBOXGROUP_ETS_TAG },
    { TIME_PICKER_DIALOG_COMPONENT_TAG, TIME_PICKER_DIALOG_ETS_TAG}
};

RefPtr<Element> InspectorComposedComponent::CreateElement()
{
    auto generateFunc = CREATE_ELEMENT_MAP.find(GetName());
    if (generateFunc != CREATE_ELEMENT_MAP.end()) {
        auto composedElement = generateFunc->second(id_);
        composedElement->SetInspectorTag(GetName());
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
        composedElement->SetDebugLine(GetDebugLine());
#endif
        auto inspectorElement = AceType::DynamicCast<InspectorComposedElement>(composedElement);
        inspectorElement->SetKey(GetInspectorKey());
        return composedElement;
    }
    return nullptr;
}

RefPtr<AccessibilityManager> InspectorComposedComponent::GetAccessibilityManager()
{
    auto container = OHOS::Ace::Container::Current();
    if (!container) {
        LOGE("container is nullptr");
        return nullptr;
    }
    auto front = container->GetFrontend();
    if (!front) {
        LOGE("front is nullptr");
        return nullptr;
    }
    auto accessibilityManager = front->GetAccessibilityManager();
    return accessibilityManager;
}

bool InspectorComposedComponent::HasInspectorFinished(std::string tag)
{
    auto generateFunc = CREATE_ELEMENT_MAP.find(tag);
    if (generateFunc != CREATE_ELEMENT_MAP.end()) {
        return true;
    }
    return false;
}

RefPtr<AccessibilityNode> InspectorComposedComponent::CreateAccessibilityNode(
    const std::string& tag, int32_t nodeId, int32_t parentNodeId, int32_t itemIndex)
{
    auto accessibilityManager = GetAccessibilityManager();
    if (!accessibilityManager) {
        LOGE("get AccessibilityManager failed");
        return nullptr;
    }

    auto node = accessibilityManager->CreateAccessibilityNode(
        InspectorComposedComponent::GetEtsTag(tag), nodeId, parentNodeId, itemIndex);
    return node;
}

std::string InspectorComposedComponent::GetEtsTag(const std::string& tag)
{
    auto iter = COMPONENT_TAG_TO_ETS_TAG_MAP.find(tag);
    if (iter == COMPONENT_TAG_TO_ETS_TAG_MAP.end()) {
        return tag;
    }
    return iter->second;
}

} // namespace OHOS::Ace::V2
