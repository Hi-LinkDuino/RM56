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

#include "core/components/test/json/image_creator.h"

#include "base/log/log.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/common/properties/color.h"
#include "core/components/image/image_component.h"
#include "core/components/test/json/align_creator.h"
#include "core/components/test/json/color_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> ImageCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory&)
{
    LOGD("CreateFromJson Image");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != IMAGE_NAME) {
        LOGE("Create Image err: not a image json.");
        return nullptr;
    }
    std::string src = componentJson.GetString(IMAGE_SRC);
    auto image = AceType::MakeRefPtr<ImageComponent>(src);
    image->SetAlt(componentJson.GetString(IMAGE_ALT));
    if (componentJson.Contains(IMAGE_WIDTH) && componentJson.GetValue(IMAGE_WIDTH)->IsNumber()) {
        double width = componentJson.GetValue(IMAGE_WIDTH)->GetDouble();
        image->SetWidth(Dimension(width));
    }
    if (componentJson.Contains(IMAGE_HEIGHT) && componentJson.GetValue(IMAGE_HEIGHT)->IsNumber()) {
        double height = componentJson.GetValue(IMAGE_HEIGHT)->GetDouble();
        image->SetHeight(Dimension(height));
    }
    if (componentJson.Contains(IMAGE_ALIGNMENT) && componentJson.GetValue(IMAGE_ALIGNMENT)->IsObject()) {
        JsonValue alignmentJson = *(componentJson.GetValue(IMAGE_ALIGNMENT));
        Alignment alignment = AlignCreator::GetAlignmentFromJson(alignmentJson);
        image->SetAlignment(alignment);
    }
    if (componentJson.Contains(IMAGE_REPEAT) && componentJson.GetValue(IMAGE_REPEAT)->IsNumber()) {
        image->SetImageRepeat(
            ConvertIntToEnum(componentJson.GetValue(IMAGE_REPEAT)->GetInt(), IMAGE_REPEATS, ImageRepeat::NOREPEAT));
    }
    image->SetImageFit(ConvertStrToFit(componentJson.GetString(IMAGE_FIT)));
    if (componentJson.Contains(IMAGE_COLOR) && componentJson.GetValue(IMAGE_COLOR)->IsObject()) {
        auto colorJson = componentJson.GetValue(COLOR);
        auto color = ColorCreator::CreateFromJson(*colorJson);
        image->SetColor(color);
    }
    auto loadSuccessEvent = componentJson.GetString(IMAGE_ON_LOAD_SUCCESS);
    image->SetLoadSuccessEvent(EventMarker(loadSuccessEvent));
    auto loadFailEvent = componentJson.GetString(IMAGE_ON_LOAD_FAIL);
    image->SetLoadFailEvent(EventMarker(loadFailEvent));
    return image;
}

ImageFit ImageCreator::ConvertStrToFit(const std::string& fit)
{
    ImageFit imageFit;
    if (fit == "fill") {
        imageFit = ImageFit::FILL;
    } else if (fit == "none") {
        imageFit = ImageFit::NONE;
    } else if (fit == "cover") {
        imageFit = ImageFit::COVER;
    } else if (fit == "fitwidth") {
        imageFit = ImageFit::FITWIDTH;
    } else if (fit == "fitheight") {
        imageFit = ImageFit::FITHEIGHT;
    } else if (fit == "scaledown") {
        imageFit = ImageFit::SCALEDOWN;
    } else {
        imageFit = ImageFit::CONTAIN;
    }
    return imageFit;
}

} // namespace OHOS::Ace
