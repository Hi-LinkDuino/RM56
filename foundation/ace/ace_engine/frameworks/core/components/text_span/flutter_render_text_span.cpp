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

#include "core/components/text_span/flutter_render_text_span.h"

#include "base/utils/string_utils.h"
#include "core/components/calendar/flutter_render_calendar.h"
#include "core/components/font/constants_converter.h"

namespace OHOS::Ace {

void FlutterRenderTextSpan::UpdateText(
    txt::ParagraphBuilder& builder, std::map<int32_t, std::map<GestureType, EventMarker>>& touchRegions)
{
    if (!spanComponent_) {
        return;
    }

    if (!spanComponent_->IsShow()) {
        LOGD("the span is not show");
        return;
    }
    if (spanComponent_->HasNewStyle()) {
        LOGD("test span has new style");
        txt::TextStyle style;
        Constants::ConvertTxtStyle(spanStyle_, context_, style);
        builder.PushStyle(style);
    }
    UpdateTouchRegions(touchRegions);
    auto displayText = spanComponent_->GetSpanData();
    StringUtils::TransfromStrCase(displayText, (int32_t)spanStyle_.GetTextCase());
    builder.AddText(StringUtils::Str8ToStr16(displayText));
    for (const auto& child : GetChildren()) {
        auto flutterRenderTextSpan = AceType::DynamicCast<FlutterRenderTextSpan>(child);
        if (flutterRenderTextSpan) {
            flutterRenderTextSpan->UpdateText(builder, touchRegions);
        }
    }
    if (spanComponent_->HasNewStyle()) {
        builder.Pop();
    }
}

void FlutterRenderTextSpan::UpdateTouchRegions(std::map<int32_t, std::map<GestureType, EventMarker>>& touchRegions)
{
    auto spanData = StringUtils::Str8ToStr16(spanComponent_->GetSpanData());
    if (spanData.empty()) {
        LOGD("FlutterRenderTextSpan::UpdateTouchRegions spanData is empty");
        return;
    }

    std::map<GestureType, EventMarker> markersMap;
    auto declaration = spanComponent_->GetDeclaration();
    if (declaration) {
        auto& gestureEvent = static_cast<CommonGestureEvent&>(declaration->GetEvent(EventTag::COMMON_GESTURE_EVENT));
        if (gestureEvent.IsValid()) {
            if (!gestureEvent.click.eventMarker.IsEmpty()) {
                markersMap.try_emplace(GestureType::CLICK, gestureEvent.click.eventMarker);
            }
            if (!gestureEvent.longPress.eventMarker.IsEmpty()) {
                markersMap.try_emplace(GestureType::LONG_PRESS, gestureEvent.longPress.eventMarker);
            }
        }

        auto& remoteMessageEvent = static_cast<CommonGestureEvent&>(declaration->GetEvent(
            EventTag::COMMON_REMOTE_MESSAGE_GRESURE_EVENT));
        if (remoteMessageEvent.IsValid()) {
            if (!remoteMessageEvent.click.eventMarker.IsEmpty()) {
                markersMap.try_emplace(GestureType::REMOTE_MESSAGE, remoteMessageEvent.click.eventMarker);
            }
        }

        auto& rawEvent = static_cast<CommonRawEvent&>(declaration->GetEvent(EventTag::COMMON_RAW_EVENT));
        if (rawEvent.IsValid()) {
            if (!rawEvent.touchStart.eventMarker.IsEmpty()) {
                markersMap.try_emplace(GestureType::TOUCH_START, rawEvent.touchStart.eventMarker);
            }
            if (!rawEvent.touchMove.eventMarker.IsEmpty()) {
                markersMap.try_emplace(GestureType::TOUCH_MOVE, rawEvent.touchMove.eventMarker);
            }
            if (!rawEvent.touchCancel.eventMarker.IsEmpty()) {
                markersMap.try_emplace(GestureType::TOUCH_CANCEL, rawEvent.touchCancel.eventMarker);
            }
            if (!rawEvent.touchEnd.eventMarker.IsEmpty()) {
                markersMap.try_emplace(GestureType::TOUCH_END, rawEvent.touchEnd.eventMarker);
            }
        }
    }

    if (touchRegions.empty()) {
        touchRegions.try_emplace(int32_t(spanData.length()), markersMap);
    } else {
        touchRegions.try_emplace(int32_t(touchRegions.rbegin()->first + spanData.length()), markersMap);
    }
}

} // namespace OHOS::Ace