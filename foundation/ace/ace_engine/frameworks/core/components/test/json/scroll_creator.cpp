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

#include "core/components/test/json/scroll_creator.h"

#include "base/geometry/axis.h"
#include "base/log/log.h"
#include "core/components/scroll/scroll_component.h"
#include "core/components/test/json/edge_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> ScrollCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson Scroll");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != SCROLL_NAME) {
        LOGE("Create scroll err: not a scroll json.");
        return nullptr;
    }

    RefPtr<Component> child;
    if (componentJson.Contains(CHILD) && componentJson.GetValue(CHILD)->IsObject()) {
        auto childJson = componentJson.GetValue(CHILD);
        child = factory.CreateComponent(*childJson);
    }

    auto scroll = AceType::MakeRefPtr<ScrollComponent>(child);
    if (componentJson.Contains(SCROLL_DIRECTION) && componentJson.GetValue(SCROLL_DIRECTION)->IsNumber()) {
        int32_t direction = componentJson.GetValue(SCROLL_DIRECTION)->GetInt();
        Axis axis = Axis::HORIZONTAL;
        if (direction == 0) {
            axis = Axis::VERTICAL;
        }
        scroll->SetAxisDirection(axis);
    }

    if (componentJson.Contains(SCROLL_PADDING) && componentJson.GetValue(SCROLL_PADDING)->IsObject()) {
        auto childJson = componentJson.GetValue(SCROLL_PADDING);
        auto padding = EdgeCreator::CreateFromJson(*childJson);
        scroll->SetPadding(padding);
    }

    if (componentJson.Contains(SCROLL_PAGE) && componentJson.GetValue(SCROLL_PAGE)->IsBool()) {
        bool scrollPage = componentJson.GetValue(SCROLL_PAGE)->GetBool();
        scroll->SetScrollPage(scrollPage);
    }

    RefPtr<ScrollPositionController> controller = AceType::MakeRefPtr<ScrollPositionController>();
    if (componentJson.Contains(SCROLL_POSITION) && componentJson.GetValue(SCROLL_POSITION)->IsNumber()) {
        double scrollPosition = componentJson.GetValue(SCROLL_POSITION)->GetDouble();
        controller->SetInitialOffset(scrollPosition);
    }
    scroll->SetScrollPositionController(controller);

    return scroll;
}

} // namespace OHOS::Ace