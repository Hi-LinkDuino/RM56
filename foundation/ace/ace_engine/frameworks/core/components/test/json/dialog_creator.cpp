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

#include "core/components/test/json/dialog_creator.h"

#include "base/log/log.h"
#include "core/components/dialog/dialog_component.h"
#include "core/components/test/json/color_creator.h"
#include "core/components/test/json/edge_creator.h"
#include "core/components/test/json/text_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> DialogCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson dialog");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != DIALOG_NAME) {
        LOGE("Create DialogComponent err: not a dialog json.");
        return nullptr;
    }

    auto dialogComponent = AceType::MakeRefPtr<DialogComponent>();
    // create title.
    auto titleJson = componentJson.GetObject(DIALOG_TITLE);
    auto title = TextCreator::CreateFromJson(*titleJson, factory);
    dialogComponent->SetTitle(AceType::DynamicCast<TextComponent>(title));

    // create titlePadding.
    auto titlePaddingJson = componentJson.GetObject(DIALOG_TITLE_PADDING);
    auto titlePadding = EdgeCreator::CreateFromJson(*titlePaddingJson);
    dialogComponent->SetTitlePadding(titlePadding);

    // create content.
    auto contentJson = componentJson.GetObject(DIALOG_CONTENT);
    auto content = TextCreator::CreateFromJson(*contentJson, factory);
    dialogComponent->SetContent(AceType::DynamicCast<TextComponent>(content));

    // create contentPadding.
    auto contentPaddingJson = componentJson.GetObject(DIALOG_CONTENT_PADDING);
    auto contentPadding = EdgeCreator::CreateFromJson(*contentPaddingJson);
    dialogComponent->SetContentPadding(contentPadding);

    // create backgroundColor.
    auto backgroundColorJson = componentJson.GetObject(DIALOG_BACKGROUND_COLOR);
    auto backgroundColor = ColorCreator::CreateFromJson(*backgroundColorJson);
    dialogComponent->SetBackgroundColor(backgroundColor);

    // create autoCancel.
    dialogComponent->SetAutoCancel(componentJson.GetBool(DIALOG_AUTO_CANCEL, true));

    // create actions.
    if (componentJson.Contains(DIALOG_ACTIONS) && componentJson.GetValue(DIALOG_ACTIONS)->IsArray()) {
        auto actionsJson = componentJson.GetValue(DIALOG_ACTIONS);
        auto components = factory.CreateComponents(*actionsJson);
        std::list<RefPtr<ButtonComponent>> actions;
        for (const auto& component : components) {
            auto button = AceType::DynamicCast<ButtonComponent>(component);
            if (button) {
                actions.emplace_back(button);
            } else {
                LOGW("Create ButtonComponent err: not a button json.");
            }
        }
        dialogComponent->SetActions(actions);
    }
    return dialogComponent;
}

} // namespace OHOS::Ace