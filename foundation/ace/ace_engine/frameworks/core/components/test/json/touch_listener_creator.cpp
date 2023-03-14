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

#include "core/components/test/json/touch_listener_creator.h"

#include "core/components/touch_listener/touch_listener_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> TouchListenerCreator::CreateFromJson(const JsonValue& json, const JsonComponentFactory& factory)
{
    auto children = factory.CreateChildComponent(json);
    auto child = children.empty() ? nullptr : children.front();
    auto touchListener = AceType::MakeRefPtr<TouchListenerComponent>(child);
    touchListener->SetOnTouchDownId(EventMarker(json.GetString(ON_TOUCH_DOWN_EVENT)));
    touchListener->SetOnTouchMoveId(EventMarker(json.GetString(ON_TOUCH_MOVE_EVENT)));
    touchListener->SetOnTouchUpId(EventMarker(json.GetString(ON_TOUCH_UP_EVENT)));
    touchListener->SetOnTouchCancel(EventMarker(json.GetString(ON_TOUCH_CANCEL_EVENT)));
    return touchListener;
}

} // namespace OHOS::Ace