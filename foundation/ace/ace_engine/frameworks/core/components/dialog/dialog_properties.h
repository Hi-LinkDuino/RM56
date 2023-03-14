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

#ifndef FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_CORE_COMPONENTS_COMMON_PROPERTIES_DIALOG_PROPERTIES_H
#define FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_CORE_COMPONENTS_COMMON_PROPERTIES_DIALOG_PROPERTIES_H

#include "base/geometry/dimension_offset.h"
#include "core/event/ace_event_handler.h"
#include "core/gestures/gesture_info.h"

namespace OHOS::Ace {

enum class DialogType {
    COMMON = 0,
    ALERT_DIALOG,
    ACTION_SHEET,
};

// Alignment of dialog in vertical.
enum class DialogAlignment {
    TOP = 0,
    CENTER,
    BOTTOM,
    DEFAULT,
    TOP_START,
    TOP_END,
    CENTER_START,
    CENTER_END,
    BOTTOM_START,
    BOTTOM_END,
};

// Information of ActionSheet
struct ActionSheetInfo {
    std::string title;       // title of ActionSheet, necessary.
    std::string icon;        // icon of ActionSheet, not necessary.
    EventMarker callbackId;  // called when ActionSheet is clicked.
    RefPtr<Gesture> gesture; // called when ActionSheet is clicked.

    // Whether sheetInfo is valid, valid if title if not empty.
    bool IsValid() const
    {
        return !title.empty();
    }
};

// Information of Button.
struct ButtonInfo {
    std::string text;      // text of button.
    std::string textColor; // style of text in button.
    bool isBgColorSetted = false;
    Color bgColor;   // background color of button.

    // Whether button info is valid, valid if text is not empty.
    bool IsValid() const
    {
        return !text.empty();
    }
};

struct DialogProperties {
    DialogType type = DialogType::COMMON; // type of dialog, current support common dialog and alert dialog.
    std::string title;                    // title of dialog.
    std::string content;                  // message of dialog.
    bool autoCancel = true;               // pop dialog when click mask if autoCancel is true.
    bool customStyle = false;             // whether use custom style.
    bool isMenu = false;
    std::vector<ButtonInfo> buttons;
    std::unordered_map<std::string, EventMarker> callbacks; // <callback type(success, cancel, complete), eventId>
    DialogAlignment alignment = DialogAlignment::DEFAULT;   // Alignment of dialog.
    DimensionOffset offset;                                 // Offset which base on alignment of Dialog.
    int32_t gridCount = 0;

    // These ids is used for AlertDialog of declarative.
    EventMarker primaryId;   // first button's callback.
    EventMarker secondaryId; // second button's callback.

    // These attributes is used for CustomDialog.
    RefPtr<Component> customComponent;         // Used for CustomDialog in declarative.
    std::function<void(bool)> onStatusChanged; // Called when dialog appear or disappear.

    // These attributes is used for ActionSheet.
    std::vector<ActionSheetInfo> sheetsInfo;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_CORE_COMPONENTS_COMMON_PROPERTIES_DIALOG_PROPERTIES_H
