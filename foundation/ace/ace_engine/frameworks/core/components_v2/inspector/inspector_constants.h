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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_INSPECTOR_CONSTANTS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_INSPECTOR_CONSTANTS_H

#include <cstdint>
#include <unordered_map>
#include <string>

#include "base/utils/macros.h"

namespace OHOS::Ace::V2 {

ACE_EXPORT extern const char ATTRS_COMMON_WIDTH[];
ACE_EXPORT extern const char ATTRS_COMMON_HEIGHT[];
ACE_EXPORT extern const char ATTRS_COMMON_PADDING[];
ACE_EXPORT extern const char ATTRS_COMMON_MARGIN[];
ACE_EXPORT extern const char ATTRS_COMMON_LAYOUT_PRIORITY[];
ACE_EXPORT extern const char ATTRS_COMMON_LAYOUT_WEIGHT[];
// position
ACE_EXPORT extern const char ATTRS_COMMON_ALIGN[];
ACE_EXPORT extern const char ATTRS_COMMON_DIRECTION[];
ACE_EXPORT extern const char ATTRS_COMMON_POSITION[];
ACE_EXPORT extern const char ATTRS_COMMON_OFFSET[];
ACE_EXPORT extern const char ATTRS_COMMON_USE_ALIGN[];
// layout
ACE_EXPORT extern const char ATTRS_COMMON_RELATE_PARENT[];
ACE_EXPORT extern const char ATTRS_COMMON_ASPECT_RATIO[];
ACE_EXPORT extern const char ATTRS_COMMON_DISPLAY_PRIORITY[];
// border
ACE_EXPORT extern const char ATTRS_COMMON_BORDER[];
// background
ACE_EXPORT extern const char ATTRS_COMMON_BACKGROUND_COLOR[];
// opacity
ACE_EXPORT extern const char ATTRS_COMMON_OPACITY[];
// visibility
ACE_EXPORT extern const char ATTRS_COMMON_VISIBILITY[];
// enable
ACE_EXPORT extern const char ATTRS_COMMON_ENABLE[];
// zindex
ACE_EXPORT extern const char ATTRS_COMMON_ZINDEX[];

// column
ACE_EXPORT extern const char COLUMN_COMPONENT_TAG[];
ACE_EXPORT extern const char COLUMN_ETS_TAG[];
ACE_EXPORT extern const char COLUMN_ATTRS_ALIGN_ITEMS[];

// text
ACE_EXPORT extern const char TEXT_COMPONENT_TAG[];
ACE_EXPORT extern const char TEXT_ETS_TAG[];
ACE_EXPORT extern const char TEXT_ATTRS_OVER_FLOW[];
ACE_EXPORT extern const char TEXT_ATTRS_MAX_LINES[];

// stack
ACE_EXPORT extern const char STACK_COMPONENT_TAG[];
ACE_EXPORT extern const char STACK_ETS_TAG[];

// swiper
ACE_EXPORT extern const char SWIPER_COMPONENT_TAG[];
ACE_EXPORT extern const char SWIPER_ETS_TAG[];

// tabs
ACE_EXPORT extern const char TABS_COMPONENT_TAG[];
ACE_EXPORT extern const char TABS_ETS_TAG[];

// tab content item
ACE_EXPORT extern const char TAB_CONTENT_ITEM_COMPONENT_TAG[];
ACE_EXPORT extern const char TAB_CONTENT_ITEM_ETS_TAG[];

// navigation view
ACE_EXPORT extern const char NAVIGATION_VIEW_COMPONENT_TAG[];
ACE_EXPORT extern const char NAVIGATION_VIEW_ETS_TAG[];

// row split
ACE_EXPORT extern const char ROW_SPLIT_COMPONENT_TAG[];
ACE_EXPORT extern const char ROW_SPLIT_ETS_TAG[];

// column split
ACE_EXPORT extern const char COLUMN_SPLIT_COMPONENT_TAG[];
ACE_EXPORT extern const char COLUMN_SPLIT_ETS_TAG[];

// counter
ACE_EXPORT extern const char COUNTER_COMPONENT_TAG[];
ACE_EXPORT extern const char COUNTER_ETS_TAG[];

// flex
ACE_EXPORT extern const char FLEX_COMPONENT_TAG[];
ACE_EXPORT extern const char FLEX_ETS_TAG[];
ACE_EXPORT extern const char WRAP_COMPONENT_TAG[];
ACE_EXPORT extern const char WRAP_ETS_TAG[];

// grid
ACE_EXPORT extern const char GRID_COMPONENT_TAG[];
ACE_EXPORT extern const char GRID_ETS_TAG[];

// grid-item
ACE_EXPORT extern const char GRID_ITEM_COMPONENT_TAG[];
ACE_EXPORT extern const char GRID_ITEM_ETS_TAG[];

// list
ACE_EXPORT extern const char LIST_COMPONENT_TAG[];
ACE_EXPORT extern const char LIST_ETS_TAG[];

// list-item
ACE_EXPORT extern const char LIST_ITEM_COMPONENT_TAG[];
ACE_EXPORT extern const char LIST_ITEM_ETS_TAG[];

// navigator
ACE_EXPORT extern const char NAVIGATOR_COMPONENT_TAG[];
ACE_EXPORT extern const char NAVIGATOR_ETS_TAG[];

// panel
ACE_EXPORT extern const char PANEL_COMPONENT_TAG[];
ACE_EXPORT extern const char PANEL_ETS_TAG[];

// row
ACE_EXPORT extern const char ROW_COMPONENT_TAG[];
ACE_EXPORT extern const char ROW_ETS_TAG[];

// shape
ACE_EXPORT extern const char SHAPE_COMPONENT_TAG[];
ACE_EXPORT extern const char SHAPE_ETS_TAG[];

// shape container
ACE_EXPORT extern const char SHAPE_CONTAINER_COMPONENT_TAG[];
ACE_EXPORT extern const char SHAPE_CONTAINER_ETS_TAG[];

// imageAnimator
ACE_EXPORT extern const char IMAGE_ANIMATOR_COMPONENT_TAG[];
ACE_EXPORT extern const char IMAGE_ANIMATOR_ETS_TAG[];

// image
ACE_EXPORT extern const char IMAGE_COMPONENT_TAG[];
ACE_EXPORT extern const char IMAGE_ETS_TAG[];

// qrcode
ACE_EXPORT extern const char QRCODE_COMPONENT_TAG[];
ACE_EXPORT extern const char QRCODE_ETS_TAG[];

// span
ACE_EXPORT extern const char SPAN_COMPONENT_TAG[];
ACE_EXPORT extern const char SPAN_ETS_TAG[];

// text
ACE_EXPORT extern const char TEXT_COMPONENT_TAG[];
ACE_EXPORT extern const char TEXT_ETS_TAG[];

// blank
ACE_EXPORT extern const char BOX_COMPONENT_TAG[];
ACE_EXPORT extern const char BLANK_ETS_TAG[];

// button
ACE_EXPORT extern const char BUTTON_COMPONENT_TAG[];
ACE_EXPORT extern const char BUTTON_ETS_TAG[];

// divider
ACE_EXPORT extern const char DIVIDER_COMPONENT_TAG[];
ACE_EXPORT extern const char DIVIDER_ETS_TAG[];

// checkbox
ACE_EXPORT extern const char CHECKBOX_COMPONENT_TAG[];
ACE_EXPORT extern const char CHECKBOX_ETS_TAG[];
ACE_EXPORT extern const char CHECK_BOX_COMPONENT_TAG[];
ACE_EXPORT extern const char CHECK_BOX_ETS_TAG[];

// switch
ACE_EXPORT extern const char SWITCH_COMPONENT_TAG[];
ACE_EXPORT extern const char SWITCH_ETS_TAG[];

// toggle
ACE_EXPORT extern const char TOGGLE_COMPONENT_TAG[];
ACE_EXPORT extern const char TOGGLE_ETS_TAG[];

// scroll
ACE_EXPORT extern const char SCROLL_COMPONENT_TAG[];
ACE_EXPORT extern const char SCROLL_ETS_TAG[];


// calendar
ACE_EXPORT extern const char CALENDAR_COMPONENT_TAG[];
ACE_EXPORT extern const char CALENDAR_ETS_TAG[];

// badge
ACE_EXPORT extern const char BADGE_COMPONENT_TAG[];
ACE_EXPORT extern const char BADGE_ETS_TAG[];

// search
ACE_EXPORT extern const char SEARCH_COMPONENT_TAG[];
ACE_EXPORT extern const char SEARCH_ETS_TAG[];

// formComponent
ACE_EXPORT extern const char FORM_COMPONENT_TAG[];
ACE_EXPORT extern const char FORM_ETS_TAG[];

// video
ACE_EXPORT extern const char VIDEO_COMPONENT_TAG[];
ACE_EXPORT extern const char VIDEO_ETS_TAG[];

// AlphabetIndexer
ACE_EXPORT extern const char INDEXER_COMPONENT_TAG[];
ACE_EXPORT extern const char INDEXER_ETS_TAG[];

// slider
ACE_EXPORT extern const char SLIDER_COMPONENT_TAG[];
ACE_EXPORT extern const char SLIDER_ETS_TAG[];

// rating
ACE_EXPORT extern const char RATING_COMPONENT_TAG[];
ACE_EXPORT extern const char RATING_ETS_TAG[];

// progress
ACE_EXPORT extern const char PROGRESS_COMPONENT_TAG[];
ACE_EXPORT extern const char PROGRESS_ETS_TAG[];

// datapanel
ACE_EXPORT extern const char DATA_PANEL_COMPONENT_TAG[];
ACE_EXPORT extern const char DATA_PANEL_ETS_TAG[];

// sheet
ACE_EXPORT extern const char SHEET_COMPONENT_TAG[];
ACE_EXPORT extern const char SHEET_ETS_TAG[];

// hyperlink
ACE_EXPORT extern const char HYPERLINK_COMPONENT_TAG[];
ACE_EXPORT extern const char HYPERLINK_ETS_TAG[];

// stepper
ACE_EXPORT extern const char STEPPER_COMPONENT_TAG[];
ACE_EXPORT extern const char STEPPER_ETS_TAG[];

// stepperItem
ACE_EXPORT extern const char STEPPER_ITEM_COMPONENT_TAG[];
ACE_EXPORT extern const char STEPPER_ITEM_ETS_TAG[];

// scrollBar
ACE_EXPORT extern const char SCROLL_BAR_COMPONENT_TAG[];
ACE_EXPORT extern const char SCROLL_BAR_ETS_TAG[];

ACE_EXPORT extern const std::unordered_map<std::string, std::string> COMPONENT_TAG_TO_ETS_TAG_MAP;
// refresh
ACE_EXPORT extern const char REFRESH_COMPONENT_TAG[];
ACE_EXPORT extern const char REFRESH_ETS_TAG[];

// datePicker
ACE_EXPORT extern const char DATE_PICKER_COMPONENT_TAG[];
ACE_EXPORT extern const char DATE_PICKER_ETS_TAG[];
ACE_EXPORT extern const char TIME_PICKER_COMPONENT_TAG[];
ACE_EXPORT extern const char TIME_PICKER_ETS_TAG[];

// Radio
ACE_EXPORT extern const char RADIO_COMPONENT_TAG[];
ACE_EXPORT extern const char RADIO_ETS_TAG[];

// gauge
ACE_EXPORT extern const char GAUGE_COMPONENT_TAG[];
ACE_EXPORT extern const char GAUGE_ETS_TAG[];

// gridContainer
ACE_EXPORT extern const char GRIDCONTAINER_COMPONENT_TAG[];
ACE_EXPORT extern const char GRIDCONTAINER_ETS_TAG[];

// menu
ACE_EXPORT extern const char MENU_COMPONENT_TAG[];
ACE_EXPORT extern const char MENU_TAG[];
ACE_EXPORT extern const char MENU_ETS_TAG[];

// textarea
ACE_EXPORT extern const char TEXTAREA_COMPONENT_TAG[];
ACE_EXPORT extern const char TEXTAREA_ETS_TAG[];

// textinput
ACE_EXPORT extern const char TEXTINPUT_COMPONENT_TAG[];
ACE_EXPORT extern const char TEXTINPUT_ETS_TAG[];

// texttimer
ACE_EXPORT extern const char TEXTTIMER_COMPONENT_TAG[];
ACE_EXPORT extern const char TEXTTIMER_ETS_TAG[];

// select
ACE_EXPORT extern const char SELECT_COMPONENT_TAG[];
ACE_EXPORT extern const char SELECT_ETS_TAG[];

// marquee
ACE_EXPORT extern const char MARQUEE_COMPONENT_TAG[];
ACE_EXPORT extern const char MARQUEE_ETS_TAG[];

// textclock
ACE_EXPORT extern const char TEXTCLOCK_COMPONENT_TAG[];
ACE_EXPORT extern const char TEXTCLOCK_ETS_TAG[];

// textpicker
ACE_EXPORT extern const char TEXT_PICKER_COMPONENT_TAG[];
ACE_EXPORT extern const char TEXT_PICKER_ETS_TAG[];

// pickertextdialog
ACE_EXPORT extern const char PICKER_TEXT_DIALOG_COMPONENT_TAG[];
ACE_EXPORT extern const char PICKER_TEXT_DIALOG_ETS_TAG[];

// canvas
ACE_EXPORT extern const char CANVAS_COMPONENT_TAG[];
ACE_EXPORT extern const char CANVAS_ETS_TAG[];

// ActionSheet
ACE_EXPORT extern const char ACTIONSHEETDIALOG_COMPONENT_TAG[];
ACE_EXPORT extern const char ACTIONSHEETDIALOG_ETS_TAG[];

// AlertDialog
ACE_EXPORT extern const char ALERTDIALOG_COMPONENT_TAG[];
ACE_EXPORT extern const char ALERTDIALOG_ETS_TAG[];

// CustomDialog
ACE_EXPORT extern const char CUSTOMDIALOG_COMPONENT_TAG[];
ACE_EXPORT extern const char CUSTOMDIALOG_ETS_TAG[];

// datePickerdialog
ACE_EXPORT extern const char DATE_PICKER_DIALOG_COMPONENT_TAG[];
ACE_EXPORT extern const char DATE_PICKER_DIALOG_ETS_TAG[];

// sideBar
ACE_EXPORT extern const char SIDE_BAR_COMPONENT_TAG[];
ACE_EXPORT extern const char SIDE_BAR_ETS_TAG[];

// loadingProgress
ACE_EXPORT extern const char LOADING_PROGRESS_COMPONENT_TAG[];
ACE_EXPORT extern const char LOADING_PROGRESS_ETS_TAG[];

// checkboxGroup
ACE_EXPORT extern const char CHECKBOXGROUP_COMPONENT_TAG[];
ACE_EXPORT extern const char CHECKBOXGROUP_ETS_TAG[];

// timePickerdialog
ACE_EXPORT extern const char TIME_PICKER_DIALOG_COMPONENT_TAG[];
ACE_EXPORT extern const char TIME_PICKER_DIALOG_ETS_TAG[];

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_INSPECTOR_CONSTANTS_H
