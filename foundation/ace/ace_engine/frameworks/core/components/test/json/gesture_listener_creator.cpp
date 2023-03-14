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

#include "gesture_listener_creator.h"

#include "core/components/gesture_listener/gesture_listener_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> GestureListenerCreator::CreateFromJson(const JsonValue& json, const JsonComponentFactory& factory)
{
    auto children = factory.CreateChildComponent(json);
    auto child = children.empty() ? nullptr : children.front();
    auto gestureListener = AceType::MakeRefPtr<GestureListenerComponent>(child);
    gestureListener->SetOnClickId(EventMarker(json.GetString(ON_CLICK_EVENT)));
    gestureListener->SetOnLongPressId(EventMarker(json.GetString(ON_LONG_PRESS_EVENT)));

    gestureListener->SetOnHorizontalDragStartId(EventMarker(json.GetString(ON_HORIZONTAL_DRAG_START_EVENT)));
    gestureListener->SetOnHorizontalDragUpdateId(EventMarker(json.GetString(ON_HORIZONTAL_DRAG_UPDATE_EVENT)));
    gestureListener->SetOnHorizontalDragEndId(EventMarker(json.GetString(ON_HORIZONTAL_DRAG_END_EVENT)));
    gestureListener->SetOnHorizontalDragCancelId(EventMarker(json.GetString(ON_HORIZONTAL_DRAG_CANCEL_EVENT)));

    gestureListener->SetOnVerticalDragStartId(EventMarker(json.GetString(ON_VERTICAL_DRAG_START_EVENT)));
    gestureListener->SetOnVerticalDragUpdateId(EventMarker(json.GetString(ON_VERTICAL_DRAG_UPDATE_EVENT)));
    gestureListener->SetOnVerticalDragEndId(EventMarker(json.GetString(ON_VERTICAL_DRAG_END_EVENT)));
    gestureListener->SetOnVerticalDragCancelId(EventMarker(json.GetString(ON_VERTICAL_DRAG_CANCEL_EVENT)));

    gestureListener->SetOnFreeDragStartId(EventMarker(json.GetString(ON_FREE_DRAG_START_EVENT)));
    gestureListener->SetOnFreeDragUpdateId(EventMarker(json.GetString(ON_FREE_DRAG_UPDATE_EVENT)));
    gestureListener->SetOnFreeDragEndId(EventMarker(json.GetString(ON_FREE_DRAG_END_EVENT)));
    gestureListener->SetOnFreeDragCancelId(EventMarker(json.GetString(ON_FREE_DRAG_CANCEL_EVENT)));

    return gestureListener;
}

} // namespace OHOS::Ace