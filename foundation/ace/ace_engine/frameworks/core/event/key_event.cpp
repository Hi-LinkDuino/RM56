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

#include "core/event/key_event.h"

namespace OHOS::Ace {

namespace {

// key : define aosp key event code, value : define ace key string
constexpr int32_t KEYCODE_SIZE = 289;
const char* g_aospKeycode2String[KEYCODE_SIZE] = {
    "Unknown", "SoftLeft", "SoftRight", "Home", "Back", "Call", "EndCall", "Digit0", "Digit1", "Digit2", "Digit3",
    "Digit4", "Digit5", "Digit6", "Digit7", "Digit8", "Digit9", "Star", "Pound", "DpadUp", "DpadDown", "DpadLeft",
    "DpadRight", "DpadCenter", "VolumeUp", "VolumeDown", "Power", "Camera", "Clear", "KeyA", "KeyB", "KeyC",
    "KeyD", "KeyE", "KeyF", "KeyG", "KeyH", "KeyI", "KeyJ", "KeyK", "KeyL", "KeyM", "KeyN", "KeyO", "KeyP",
    "KeyQ", "KeyR", "KeyS", "KeyT", "KeyU", "KeyV", "KeyW", "KeyX", "KeyY", "KeyZ", "Comma", "Period", "AltLeft",
    "AltRight", "ShiftLeft", "ShiftRight", "Tab", "Space", "Sym", "Explorer", "Envelope", "Enter", "Del", "Grave",
    "Minus", "Equals", "LeftBracket", "RightBracket", "BackSlash", "Semicolon", "Apostrophe", "Slash", "At",
    "Num", "HeadsetHook", "Focus", "Plus", "Menu", "Notification", "Search", "MeidaPlayPause",
    "MeidaStop", "MeidaNext", "MeidaPrevious", "MeidaRewind", "MeidaFastForward", "Mute", "PageUp", "PageDown",
    "PictSymbols", "SwitchCharset", "ButtonA", "ButtonB", "ButtonC", "ButtonX", "ButtonY", "ButtonZ", "ButtonL1",
    "ButtonR1", "ButtonL2", "ButtonR2", "ButtonThumbl", "ButtonThumbr", "ButtonStart", "ButtonSelect", "ButtonMode",
    "Escape", "ForwardDel", "CtrlLeft", "CtrlRight", "CapsLock", "ScrollLock", "MetaLeft", "MetaRight", "Function",
    "SysRq", "Break", "MoveHome", "MoveEnd", "Insert", "Forward", "MediaPlay", "MediaPause", "MediaClose",
    "MediaReject", "MediaRecord", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "NumLock",
    "Numpad0", "Numpad1", "Numpad2", "Numpad3", "Numpad4", "Numpad5", "Numpad6", "Numpad7", "Numpad8", "NumPad9",
    "NumpadDivide", "NumpadMultiply", "NumpadSubtract", "NumpadAdd", "NumpadDot", "NumpadComma", "NumpadEnter",
    "NumpadEquals", "NumpadLeftParen", "NumpadRightParen", "VolumeMute", "Info", "ChannelUp", "ChannelDown", "ZoomIn",
    "ZoomOut", "Tv", "Window", "Guide", "Dvr", "BookMark", "Captions", "Settings", "TvPower", "TvInput", "StbPower",
    "StbInput", "AvrPower", "AvrInput", "ProgRed", "ProgGreen", "ProgYellow", "ProgBlue", "AppSwitch", "Button1",
    "Button2", "Button3", "Button4", "Button5", "Button6", "Button7", "Button8", "Button9", "Button10", "Button11",
    "Button12", "Button13", "Button14", "Button15", "Button16", "LanguageSwitch", "MannerMode", "3DMode", "Contacts",
    "Calendar", "Music", "Calculator", "ZenkakuHankaku", "Eisu", "Muhenkan", "Henkan", "KatakanaHiragana", "Yen", "Ro",
    "Kana", "Assist", "BrightnessDown", "BrightnessUp", "MediaAudioTrack", "Sleep", "WakeUp", "Pairing", "MediaTopMenu",
    "Digit11", "Digit12", "LastChannel", "TvDataService", "VoiceAssist", "TvRadioService", "TvTeletext",
    "TvNumberEntry", "TvTerrestrialAnalog", "TvTerrestrialDigital", "TvSatellite", "TvSatelliteBS", "TvSatelliteCS",
    "TvSatelliteService", "TvNetWork", "TvAntennaCable", "TvInputHdmi1", "TvInputHdmi2", "TvInputHdmi3", "TvInputHdmi4",
    "TvInputComposite1", "TvInputComposite2", "TvInputComponent1", "TvInputComponent2", "TvInputVGA1",
    "TvAudioDescription", "TvAudioDescriptionMixUp", "TvAudioDescriptionMixDown", "TvZoomMode", "TvContentsMenu",
    "TvMediaContextMenu", "TvTimerProgramming", "Help", "NavigatePrevious", "NavigateNext", "NavigateIn", "NavigateOut",
    "StemPrimary", "Stem1", "Stem2", "Stem3", "DpadUpLeft", "DpadDownLeft", "DpadUpRight", "DpadDownRight",
    "MediaSkipForward", "MediaSkipBackward", "MediaStepForward", "MediaStepBackward", "SoftSleep", "Cut", "Copy",
    "Paste", "SystemNavigationUp", "SystemNavigationDown", "SystemNavigationLeft", "SystemNavigationRight", "AllApps",
    "Refresh", "ThumbsUp", "ThumbsDown", "ProfileSwitch"
};

} // namespace

const char* KeyToString(int32_t code)
{
    if (code >= 0 && code < KEYCODE_SIZE) {
        return g_aospKeycode2String[code];
    } else {
        return g_aospKeycode2String[0]; // "UnKnown"
    }
}

} // namespace OHOS::Ace