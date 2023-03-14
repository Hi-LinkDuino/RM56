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

#include "core/components/test/json/box_creator.h"

#include "base/log/log.h"
#include "core/components/box/box_component.h"
#include "core/components/test/json/align_creator.h"
#include "core/components/test/json/color_creator.h"
#include "core/components/test/json/decoration_creator.h"
#include "core/components/test/json/edge_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> BoxCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson box");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != BOX_NAME) {
        LOGE("Create BoxComponent err: not a box json.");
        return nullptr;
    }

    auto boxComponent = AceType::MakeRefPtr<BoxComponent>();
    if (componentJson.Contains(CHILD) && componentJson.GetValue(CHILD)->IsObject()) {
        auto childJson = componentJson.GetValue(CHILD);
        auto child = factory.CreateComponent(*childJson);
        boxComponent->SetChild(child);
    }
    if (componentJson.Contains(ALIGN) && componentJson.GetValue(ALIGN)->IsObject()) {
        auto alignmentJson = componentJson.GetValue(ALIGN);
        auto alignment = AlignCreator::GetAlignmentFromJson(*alignmentJson);
        boxComponent->SetAlignment(alignment);
    }
    if (componentJson.Contains(BOX_BACK_DECORATION) && componentJson.GetValue(BOX_BACK_DECORATION)->IsObject()) {
        auto backDecorationJson = componentJson.GetValue(BOX_BACK_DECORATION);
        auto decoration = DecorationCreator::CreateFromJson(*backDecorationJson);
        boxComponent->SetBackDecoration(decoration);
    }
    if (componentJson.Contains(BOX_FRONT_DECORATION) && componentJson.GetValue(BOX_FRONT_DECORATION)->IsObject()) {
        auto frondDecorationJson = componentJson.GetValue(BOX_FRONT_DECORATION);
        auto decoration = DecorationCreator::CreateFromJson(*frondDecorationJson);
        boxComponent->SetFrontDecoration(decoration);
    }
    if (componentJson.Contains(MARGIN) && componentJson.GetValue(MARGIN)->IsObject()) {
        auto marginJson = componentJson.GetValue(MARGIN);
        auto margin = EdgeCreator::CreateFromJson(*marginJson);
        boxComponent->SetMargin(margin);
    }
    if (componentJson.Contains(PADDING) && componentJson.GetValue(PADDING)->IsObject()) {
        auto paddingJson = componentJson.GetValue(PADDING);
        auto padding = EdgeCreator::CreateFromJson(*paddingJson);
        boxComponent->SetPadding(padding);
    }
    if (componentJson.Contains(WIDTH) && componentJson.GetValue(WIDTH)->IsNumber()) {
        double width = componentJson.GetValue(WIDTH)->GetDouble();
        boxComponent->SetWidth(width);
    }
    if (componentJson.Contains(HEIGHT) && componentJson.GetValue(HEIGHT)->IsNumber()) {
        double height = componentJson.GetValue(HEIGHT)->GetDouble();
        boxComponent->SetHeight(height);
    }
    if (componentJson.Contains(COLOR) && componentJson.GetValue(COLOR)->IsObject()) {
        auto colorJson = componentJson.GetValue(COLOR);
        auto color = ColorCreator::CreateFromJson(*colorJson);
        boxComponent->SetColor(color);
    }

    return boxComponent;
}

} // namespace OHOS::Ace
