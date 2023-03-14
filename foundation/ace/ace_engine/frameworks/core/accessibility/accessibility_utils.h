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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ACCESSIBILITY_ACCESSIBILITY_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ACCESSIBILITY_ACCESSIBILITY_UTILS_H

#include <cstddef>
#include <cstdint>

namespace OHOS::Ace {

// define accessibility node tags
extern const char ACCESSIBILITY_TAG_DIV[];
extern const char ACCESSIBILITY_TAG_CALENDAR[];
extern const char ACCESSIBILITY_TAG_TEXT[];
extern const char ACCESSIBILITY_TAG_PICKER[];
extern const char ACCESSIBILITY_TAG_OPTION[];
extern const char ACCESSIBILITY_TAG_POPUP[];
extern const char ACCESSIBILITY_TAG_PROGRESS[];
extern const char ACCESSIBILITY_TAG_SELECT[];
extern const char ACCESSIBILITY_TAG_MENU[];
extern const char ACCESSIBILITY_TAG_SLIDER[];
extern const char ACCESSIBILITY_TAG_SPAN[];
extern const char ACCESSIBILITY_TAG_STACK[];
extern const char ACCESSIBILITY_TAG_SWIPER[];
extern const char ACCESSIBILITY_TAG_SWITCH[];
extern const char ACCESSIBILITY_TAG_TABS[];
extern const char ACCESSIBILITY_TAG_TAB_BAR[];
extern const char ACCESSIBILITY_TAG_TAB_CONTENT[];
extern const char ACCESSIBILITY_TAG_REFRESH[];
extern const char ACCESSIBILITY_TAG_IMAGE[];
extern const char ACCESSIBILITY_TAG_LIST[];
extern const char ACCESSIBILITY_TAG_LIST_ITEM[];
extern const char ACCESSIBILITY_TAG_LIST_ITEM_GROUP[];
extern const char ACCESSIBILITY_TAG_VIDEO[];
extern const char ACCESSIBILITY_TAG_RATING[];
extern const char ACCESSIBILITY_TAG_MARQUEE[];
extern const char ACCESSIBILITY_TAG_NAVIGATION_BAR[];
extern const char ACCESSIBILITY_TAG_NAVIGATION_MENU[];
extern const char ACCESSIBILITY_TAG_TEXTAREA[];
extern const char ACCESSIBILITY_TAG_INPUT[];
extern const char ACCESSIBILITY_TAG_LABEL[];
extern const char ACCESSIBILITY_TAG_DIVIDER[];
extern const char ACCESSIBILITY_TAG_CANVAS[];
extern const char ACCESSIBILITY_TAG_BUTTON[];
extern const char ACCESSIBILITY_TAG_CHART[];
extern const char ACCESSIBILITY_TAG_CLOCK[];
extern const char ACCESSIBILITY_TAG_DIALOG[];
extern const char ACCESSIBILITY_TAG_SEARCH[];

enum class AccessibilityEventType: size_t {
    CLICK = 0x00000001,
    LONG_PRESS = 0x00000002,
    SELECTED = 0x00000004,
    BLUR = 0x00000007,
    FOCUS = 0x00000008,
    TEXT_CHANGE = 0x00000010,
    EJECT_DISMISS = 0x00000020,
    MOUSE = 149,
    KEYBOARD_BACK = 150,
    KEYBOARD_UP = 151,
    KEYBOARD_DOWN = 152,
    KEYBOARD_LEFT = 153,
    KEYBOARD_RIGHT = 154,
    KEYBOARD_CENTER = 155,
    HANDLE_A = 156,
    HANDLE_SELECT = 157,
    KEYBOARD_TAB = 158,
    KEYBOARD_SPACE = 159,
    KEYBOARD_ENTER = 160,
    KEYBOARD_ESCAPE = 161,
    KEYBOARD_NUMBER_ENTER = 162,
    TV_CONTROL_MEDIA_PLAY = 163,
    PAGE_CHANGE = 200,
    TOUCH_START = 0x00000200,
    TOUCH_MOVE = 0x00000201,
    TOUCH_CANCEL = 0x00000202,
    TOUCH_END = 0x00000400,
    CHANGE = 0x00000800,
    SCROLL_END = 0x00001000,
    SCROLL_START = 0x01000000,
    UNKNOWN,
};

struct OperableInfo {
    bool checkable = false;
    bool clickable = false;
    bool scrollable = false;
    bool longClickable = false;
    bool focusable = false;
};

enum class AceAction : uint32_t {
    ACTION_NONE = 0,
    GLOBAL_ACTION_BACK,
    CUSTOM_ACTION,
    ACTION_CLICK = 10,
    ACTION_LONG_CLICK,
    ACTION_SCROLL_FORWARD,
    ACTION_SCROLL_BACKWARD,
    ACTION_FOCUS,
    ACTION_ACCESSIBILITY_FOCUS,
    ACTION_CLEAR_ACCESSIBILITY_FOCUS,
    ACTION_NEXT_AT_MOVEMENT_GRANULARITY,
    ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY
};

struct AccessibilityValue {
    double min = 0.0;
    double max = 0.0;
    double current = 0.0;
};

enum class AceTextCategory {
    INPUT_TYPE_DEFAULT = 0,
    INPUT_TYPE_TEXT,
    INPUT_TYPE_EMAIL,
    INPUT_TYPE_DATE,
    INPUT_TYPE_TIME,
    INPUT_TYPE_NUMBER,
    INPUT_TYPE_PASSWORD
};

struct AceCollectionInfo {
    int32_t rows = 0;
    int32_t columns = 0;
};

struct AceCollectionItemInfo {
    int32_t row = 0;
    int32_t column = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ACCESSIBILITY_ACCESSIBILITY_UTILS_H
