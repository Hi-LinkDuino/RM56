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

#include "decoration_creator.h"

#include "base/log/log.h"
#include "core/components/test/json/background_image_creator.h"
#include "core/components/test/json/border_creator.h"
#include "core/components/test/json/color_creator.h"
#include "core/components/test/json/edge_creator.h"
#include "core/components/test/json/gradient_creator.h"
#include "core/components/test/json/shadow_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Decoration> DecorationCreator::CreateFromJson(const JsonValue& json)
{
    LOGD("CreateFromJson Decoration");
    std::string classType = json.GetValue(CLASS_NAME)->GetString();
    if (classType != DECORATION_NAME) {
        LOGE("Create decoration err: not a decoration json.");
        return nullptr;
    }
    RefPtr<Decoration> decoration = AceType::MakeRefPtr<Decoration>();

    // set padding
    if (json.Contains(DECORATION_PADDING) && json.GetValue(DECORATION_PADDING)->IsObject()) {
        auto paddingJson = json.GetValue(DECORATION_PADDING);
        decoration->SetPadding(EdgeCreator::CreateFromJson(*paddingJson));
    }

    // set border
    if (json.Contains(DECORATION_BORDER) && json.GetValue(DECORATION_BORDER)->IsObject()) {
        auto borderJson = json.GetValue(DECORATION_BORDER);
        decoration->SetBorder(*(BorderCreator::CreateFromJson(*borderJson)));
    }

    // set shadows
    if (json.Contains(SHADOW_LIST) && json.GetValue(SHADOW_LIST)->IsArray()) {
        auto shadowsJson = json.GetValue(SHADOW_LIST);
        int32_t size = shadowsJson->GetArraySize();
        for (int32_t index = 0; index < size; index++) {
            auto shadowItem = ShadowCreator::CreateFromJson(*(shadowsJson->GetArrayItem(index)));
            decoration->AddShadow(shadowItem);
        }
    }

    // set background color/gradient/image
    if (json.Contains(BACKGROUND_COLOR) && json.GetValue(BACKGROUND_COLOR)->IsObject()) {
        auto colorJson = json.GetValue(BACKGROUND_COLOR);
        decoration->SetBackgroundColor(ColorCreator::CreateFromJson(*colorJson));
    }
    if (json.Contains(GRADIENT_NAME) && json.GetValue(GRADIENT_NAME)->IsObject()) {
        auto gradientJson = json.GetValue(GRADIENT_NAME);
        decoration->SetGradient(GradientCreator::CreateFromJson(*gradientJson));
    }
    if (json.Contains(BACKGROUND_IMAGE) && json.GetValue(BACKGROUND_IMAGE)->IsObject()) {
        auto backgroundJson = json.GetValue(BACKGROUND_IMAGE);
        decoration->SetImage(BackgroundImageCreator::CreateFromJson(*backgroundJson));
    }
    return decoration;
}

} // namespace OHOS::Ace