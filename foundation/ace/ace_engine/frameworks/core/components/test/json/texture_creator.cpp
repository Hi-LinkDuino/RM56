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

#include "core/components/test/json/texture_creator.h"

#include "base/log/log.h"
#include "core/components/video/texture_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> TextureCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson texture");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != TEXTURE_NAME) {
        LOGE("Create texture err: not a texture json.");
        return nullptr;
    }

    auto texture = AceType::MakeRefPtr<TextureComponent>();
    if (componentJson.Contains(TEXTURE_SRC_ID) && componentJson.GetValue(TEXTURE_SRC_ID)->IsNumber()) {
        texture->SetTextureId(componentJson.GetValue(TEXTURE_SRC_ID)->GetInt());
    }

    if (componentJson.Contains(TEXTURE_SRC_WIDTH) && componentJson.GetValue(TEXTURE_SRC_WIDTH)->IsNumber()) {
        texture->SetSrcWidth(componentJson.GetValue(TEXTURE_SRC_WIDTH)->GetInt());
    }

    if (componentJson.Contains(TEXTURE_SRC_HEIGHT) && componentJson.GetValue(TEXTURE_SRC_HEIGHT)->IsNumber()) {
        texture->SetSrcHeight(componentJson.GetValue(TEXTURE_SRC_HEIGHT)->GetInt());
    }

    if (componentJson.Contains(TEXTURE_FIT) && componentJson.GetValue(TEXTURE_FIT)->IsString()) {
        auto fit = componentJson.GetValue(TEXTURE_FIT)->GetString();
        texture->SetFit(ConvertStrToFit(fit));
    }

    if (componentJson.Contains(CHILD) && componentJson.GetValue(CHILD)->IsObject()) {
        auto childJson = componentJson.GetValue(CHILD);
        auto child = factory.CreateComponent(*childJson);
        texture->SetChild(child);
    }

    return texture;
}

ImageFit TextureCreator::ConvertStrToFit(const std::string& fit)
{
    ImageFit imageFit;
    if (fit == "fill") {
        imageFit = ImageFit::FILL;
    } else if (fit == "contain") {
        imageFit = ImageFit::CONTAIN;
    } else if (fit == "cover") {
        imageFit = ImageFit::COVER;
    } else if (fit == "scaledown") {
        imageFit = ImageFit::SCALEDOWN;
    } else {
        imageFit = ImageFit::NONE;
    }
    return imageFit;
}

} // namespace OHOS::Ace
