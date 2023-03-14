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

#include "core/components_v2/inspector/inspector_constants.h"
#include "core/components/shape/shape_component.h"

namespace OHOS::Ace::V2 {
// common attrs
// dimension
const char ATTRS_COMMON_WIDTH[] = "width";
const char ATTRS_COMMON_HEIGHT[] = "height";
const char ATTRS_COMMON_PADDING[] = "padding";
const char ATTRS_COMMON_MARGIN[] = "margin";
const char ATTRS_COMMON_LAYOUT_PRIORITY[] = "layoutPriority";
const char ATTRS_COMMON_LAYOUT_WEIGHT[] = "layoutWeight";
// position
const char ATTRS_COMMON_ALIGN[] = "align";
const char ATTRS_COMMON_DIRECTION[] = "direction";
const char ATTRS_COMMON_POSITION[] = "position";
const char ATTRS_COMMON_OFFSET[] = "margin";
const char ATTRS_COMMON_USE_ALIGN[] = "useAlign";
// layout
const char ATTRS_COMMON_RELATE_PARENT[] = "relateParent";
const char ATTRS_COMMON_ASPECT_RATIO[] = "aspectRatio";
const char ATTRS_COMMON_DISPLAY_PRIORITY[] = "displayPriority";
// border
const char ATTRS_COMMON_BORDER[] = "border";
// background
const char ATTRS_COMMON_BACKGROUND_COLOR[] = "backgroundColor";
// opacity
const char ATTRS_COMMON_OPACITY[] = "opacity";
// visibility
const char ATTRS_COMMON_VISIBILITY[] = "visibility";
// enable
const char ATTRS_COMMON_ENABLE[] = "enable";
// zindex
const char ATTRS_COMMON_ZINDEX[] = "zIndex";

// column
const char COLUMN_COMPONENT_TAG[] = "ColumnComponent";
const char COLUMN_ETS_TAG[] = "Column";
const char COLUMN_ATTRS_ALIGN_ITEMS[] = "alignItems";

// text
const char TEXT_COMPONENT_TAG[] = "TextComponentV2";
const char TEXT_ETS_TAG[] = "Text";
const char TEXT_ATTRS_OVER_FLOW[] = "textOverflow";
const char TEXT_ATTRS_MAX_LINES[] = "maxLines";

// stack
const char STACK_COMPONENT_TAG[] = "StackComponent";
const char STACK_ETS_TAG[] = "Stack";

// swiper
const char SWIPER_COMPONENT_TAG[] = "SwiperComponent";
const char SWIPER_ETS_TAG[] = "Swiper";

// tabs
const char TABS_COMPONENT_TAG[] = "TabsComponent";
const char TABS_ETS_TAG[] = "Tabs";

// tab content item
const char TAB_CONTENT_ITEM_COMPONENT_TAG[] = "TabContentItemComponent";
const char TAB_CONTENT_ITEM_ETS_TAG[] = "TabContent";

// navigation view
const char NAVIGATION_VIEW_COMPONENT_TAG[] = "NavigationContainerComponent";
const char NAVIGATION_VIEW_ETS_TAG[] = "Navigation";

// row split
const char ROW_SPLIT_COMPONENT_TAG[] = "RowSplitComponent";
const char ROW_SPLIT_ETS_TAG[] = "RowSplit";

// column split
const char COLUMN_SPLIT_COMPONENT_TAG[] = "ColumnSplitComponent";
const char COLUMN_SPLIT_ETS_TAG[] = "ColumnSplit";

// counter
const char COUNTER_COMPONENT_TAG[] = "CounterComponent";
const char COUNTER_ETS_TAG[] = "Counter";

// flex
const char FLEX_COMPONENT_TAG[] = "FlexComponentV2";
const char FLEX_ETS_TAG[] = "Flex";
const char WRAP_COMPONENT_TAG[] = "WrapComponent";
const char WRAP_ETS_TAG[] = "Flex";

// grid
const char GRID_COMPONENT_TAG[] = "GridLayoutComponent";
const char GRID_ETS_TAG[] = "Grid";

// grid-item
const char GRID_ITEM_COMPONENT_TAG[] = "GridLayoutItemComponent";
const char GRID_ITEM_ETS_TAG[] = "GridItem";

// list
const char LIST_COMPONENT_TAG[] = "V2::ListComponent";
const char LIST_ETS_TAG[] = "List";

// list-item
const char LIST_ITEM_COMPONENT_TAG[] = "V2::ListItemComponent";
const char LIST_ITEM_ETS_TAG[] = "ListItem";

// navigator
const char NAVIGATOR_COMPONENT_TAG[] = "NavigatorComponent";
const char NAVIGATOR_ETS_TAG[] = "Navigator";

// panel
const char PANEL_COMPONENT_TAG[] = "SlidingPanelComponentV2";
const char PANEL_ETS_TAG[] = "Panel";

// row
const char ROW_COMPONENT_TAG[] = "RowComponent";
const char ROW_ETS_TAG[] = "Row";

// shape
const char SHAPE_COMPONENT_TAG[] = "ShapeComponent";
const char SHAPE_ETS_TAG[] = "Shape";

// shape container
const char SHAPE_CONTAINER_COMPONENT_TAG[] = "ShapeContainerComponent";
const char SHAPE_CONTAINER_ETS_TAG[] = "Shape";

// imageAnimator
const char IMAGE_ANIMATOR_COMPONENT_TAG[] = "ImageAnimator";
const char IMAGE_ANIMATOR_ETS_TAG[] = "ImageAnimator";

// image
const char IMAGE_COMPONENT_TAG[] = "ImageComponent";
const char IMAGE_ETS_TAG[] = "Image";

// qrcode
const char QRCODE_COMPONENT_TAG[] = "QrcodeComponent";
const char QRCODE_ETS_TAG[] = "QRCode";

// span
const char SPAN_COMPONENT_TAG[] = "TextSpanComponent";
const char SPAN_ETS_TAG[] = "Span";

// blank
const char BOX_COMPONENT_TAG[] = "BoxComponent";
const char BLANK_ETS_TAG[] = "Blank";

// button
ACE_EXPORT extern const char BUTTON_COMPONENT_TAG[] = "ButtonComponent";
ACE_EXPORT extern const char BUTTON_ETS_TAG[] = "Button";

// divider
ACE_EXPORT extern const char DIVIDER_COMPONENT_TAG[] = "DividerComponent";
ACE_EXPORT extern const char DIVIDER_ETS_TAG[] = "Divider";

// checkbox
const char CHECKBOX_COMPONENT_TAG[] = "CheckboxComponent";
const char CHECKBOX_ETS_TAG[] = "Toggle";
ACE_EXPORT extern const char CHECK_BOX_COMPONENT_TAG[] = "Checkbox";
ACE_EXPORT extern const char CHECK_BOX_ETS_TAG[] = "Checkbox";

// switch
const char SWITCH_COMPONENT_TAG[] = "SwitchComponent";
const char SWITCH_ETS_TAG[] = "Toggle";

// toggle
const char TOGGLE_COMPONENT_TAG[] = "ToggleComponent";
const char TOGGLE_ETS_TAG[] = "Toggle";

// scroll
const char SCROLL_COMPONENT_TAG[] = "ScrollComponent";
const char SCROLL_ETS_TAG[] = "Scroll";

// calendar
ACE_EXPORT extern const char CALENDAR_COMPONENT_TAG[] = "calendar";
ACE_EXPORT extern const char CALENDAR_ETS_TAG[] = "Calendar";

// badge
const char BADGE_COMPONENT_TAG[] = "BadgeComponent";
const char BADGE_ETS_TAG[] = "Badge";

// search
const char SEARCH_COMPONENT_TAG[] = "SearchComponent";
const char SEARCH_ETS_TAG[] = "Search";

// formComponent
const char FORM_COMPONENT_TAG[] = "FormComponent";
const char FORM_ETS_TAG[] = "FormComponent";

// video
const char VIDEO_COMPONENT_TAG[] = "VideoComponentV2";
const char VIDEO_ETS_TAG[] = "Video";

// AlphabetIndexer
const char INDEXER_COMPONENT_TAG[] = "IndexerComponent";
const char INDEXER_ETS_TAG[] = "AlphabetIndexer";

// slider
ACE_EXPORT extern const char SLIDER_COMPONENT_TAG[] = "SliderComponent";
ACE_EXPORT extern const char SLIDER_ETS_TAG[] = "Slider";

// rating
ACE_EXPORT extern const char RATING_COMPONENT_TAG[] = "RatingComponent";
ACE_EXPORT extern const char RATING_ETS_TAG[] = "Rating";

// progress
ACE_EXPORT extern const char PROGRESS_COMPONENT_TAG[] = "ProgressComponent";
ACE_EXPORT extern const char PROGRESS_ETS_TAG[] = "Progress";

// datapanel
ACE_EXPORT extern const char DATA_PANEL_COMPONENT_TAG[] = "PercentageDataPanelComponent";
ACE_EXPORT extern const char DATA_PANEL_ETS_TAG[] = "DataPanel";

// sheet
ACE_EXPORT extern const char SHEET_COMPONENT_TAG[] = "SheetComponent";
ACE_EXPORT extern const char SHEET_ETS_TAG[] = "Sheet";

// hyperlink
ACE_EXPORT extern const char HYPERLINK_COMPONENT_TAG[] = "HyperlinkComponent";
ACE_EXPORT extern const char HYPERLINK_ETS_TAG[] = "Hyperlink";

// stepper
ACE_EXPORT extern const char STEPPER_COMPONENT_TAG[] = "StepperComponent";
ACE_EXPORT extern const char STEPPER_ETS_TAG[] = "Stepper";

// stepperItem
ACE_EXPORT extern const char STEPPER_ITEM_COMPONENT_TAG[] = "StepperItemComponentV2";
ACE_EXPORT extern const char STEPPER_ITEM_ETS_TAG[] = "StepperItem";

// scrollbar
ACE_EXPORT extern const char SCROLL_BAR_COMPONENT_TAG[] = "ScrollBarComponent";
ACE_EXPORT extern const char SCROLL_BAR_ETS_TAG[] = "ScrollBar";

// refresh
ACE_EXPORT extern const char REFRESH_COMPONENT_TAG[] = "RefreshComponent";
ACE_EXPORT extern const char REFRESH_ETS_TAG[] = "Refresh";

// datePicker
ACE_EXPORT extern const char DATE_PICKER_COMPONENT_TAG[] = "PickerDateComponent";
ACE_EXPORT extern const char DATE_PICKER_ETS_TAG[] = "DatePicker";
ACE_EXPORT extern const char TIME_PICKER_COMPONENT_TAG[] = "PickerTimeComponent";
ACE_EXPORT extern const char TIME_PICKER_ETS_TAG[] = "TimePicker";

// Radio
ACE_EXPORT extern const char RADIO_COMPONENT_TAG[] = "RadioComponent<VALUE_TYPE>";
ACE_EXPORT extern const char RADIO_ETS_TAG[] = "Radio";

// gauge
ACE_EXPORT extern const char GAUGE_COMPONENT_TAG[] = "Gauge";
ACE_EXPORT extern const char GAUGE_ETS_TAG[] = "Gauge";

// gridContainer
ACE_EXPORT extern const char GRIDCONTAINER_COMPONENT_TAG[] = "GridContainer";
ACE_EXPORT extern const char GRIDCONTAINER_ETS_TAG[] = "GridContainer";

// menu
ACE_EXPORT extern const char MENU_COMPONENT_TAG[] = "MenuComponent";
ACE_EXPORT extern const char MENU_TAG[] = "menu";
ACE_EXPORT extern const char MENU_ETS_TAG[] = "Menu";

// textarea
ACE_EXPORT extern const char TEXTAREA_COMPONENT_TAG[] = "TextFieldComponent";
ACE_EXPORT extern const char TEXTAREA_ETS_TAG[] = "TextArea";

// textinput
ACE_EXPORT extern const char TEXTINPUT_COMPONENT_TAG[] = "TextInput";
ACE_EXPORT extern const char TEXTINPUT_ETS_TAG[] = "TextInput";

// texttimer
ACE_EXPORT extern const char TEXTTIMER_COMPONENT_TAG[] = "TextTimerComponent";
ACE_EXPORT extern const char TEXTTIMER_ETS_TAG[] = "TextTimer";

// select
ACE_EXPORT extern const char SELECT_COMPONENT_TAG[] = "SelectComponent";
ACE_EXPORT extern const char SELECT_ETS_TAG[] = "Select";

// marquee
ACE_EXPORT extern const char MARQUEE_COMPONENT_TAG[] = "MarqueeComponent";
ACE_EXPORT extern const char MARQUEE_ETS_TAG[] = "Marquee";

// textclock
ACE_EXPORT extern const char TEXTCLOCK_COMPONENT_TAG[] = "TextClockComponent";
ACE_EXPORT extern const char TEXTCLOCK_ETS_TAG[] = "TextClock";

// textpicker
ACE_EXPORT extern const char TEXT_PICKER_COMPONENT_TAG[] = "PickerTextComponent";
ACE_EXPORT extern const char TEXT_PICKER_ETS_TAG[] = "TextPicker";

// pickertextdialog
ACE_EXPORT extern const char PICKER_TEXT_DIALOG_COMPONENT_TAG[] = "pickerTextDialog";
ACE_EXPORT extern const char PICKER_TEXT_DIALOG_ETS_TAG[] = "pickerTextDialog";

// canvas
ACE_EXPORT extern const char CANVAS_COMPONENT_TAG[] = "CustomPaintComponent";
ACE_EXPORT extern const char CANVAS_ETS_TAG[] = "Canvas";

// ActionSheet
ACE_EXPORT extern const char ACTIONSHEETDIALOG_COMPONENT_TAG[] = "ActionSheet";
ACE_EXPORT extern const char ACTIONSHEETDIALOG_ETS_TAG[] = "ActionSheet";

// AlertDialog
ACE_EXPORT extern const char ALERTDIALOG_COMPONENT_TAG[] = "AlertDialog";
ACE_EXPORT extern const char ALERTDIALOG_ETS_TAG[] = "AlertDialog";

// customdialog
ACE_EXPORT extern const char CUSTOMDIALOG_COMPONENT_TAG[] = "CustomDialog";
ACE_EXPORT extern const char CUSTOMDIALOG_ETS_TAG[] = "CustomDialog";

// datePickerdialog
ACE_EXPORT extern const char DATE_PICKER_DIALOG_COMPONENT_TAG[] = "DatePickerDialog";
ACE_EXPORT extern const char DATE_PICKER_DIALOG_ETS_TAG[] = "DatePickerDialog";

// sideBar
ACE_EXPORT extern const char SIDE_BAR_COMPONENT_TAG[] = "SideBarContainerComponent";
ACE_EXPORT extern const char SIDE_BAR_ETS_TAG[] = "SideBarContainer";

// loadingProgress
ACE_EXPORT extern const char LOADING_PROGRESS_COMPONENT_TAG[] = "LoadingProgressComponent";
ACE_EXPORT extern const char LOADING_PROGRESS_ETS_TAG[] = "LoadingProgress";

// checkboxGroup
const char CHECKBOXGROUP_COMPONENT_TAG[] = "CheckboxGroupComponent";
const char CHECKBOXGROUP_ETS_TAG[] = "CheckboxGroup";

// timePickerdialog
ACE_EXPORT extern const char TIME_PICKER_DIALOG_COMPONENT_TAG[] = "TimePickerDialog";
ACE_EXPORT extern const char TIME_PICKER_DIALOG_ETS_TAG[] = "TimePickerDialog";

} // namespace OHOS::Ace::V2
