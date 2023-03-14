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

#include "core/components/test/json/background_image_creator.h"

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/test/json/align_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {
namespace {

const BackgroundImageSizeType IMAGE_SIZE_TYPES[] = { BackgroundImageSizeType::CONTAIN, BackgroundImageSizeType::COVER,
    BackgroundImageSizeType::AUTO, BackgroundImageSizeType::LENGTH, BackgroundImageSizeType::PERCENT };
const BackgroundImagePositionType IMAGE_POSITION_TYPE[] = { BackgroundImagePositionType::PERCENT,
    BackgroundImagePositionType::PX };

} // namespace

RefPtr<BackgroundImage> BackgroundImageCreator::CreateFromJson(const JsonValue& json)
{
    LOGD("BackgroundImage creator");
    std::string classType = json.GetValue(CLASS_NAME)->GetString();
    if (classType != BACKGROUND_IMAGE_NAME) {
        LOGE("Create BackgroundImage err: not a BackgroundImage json.");
        return nullptr;
    }

    if (!json.Contains(BACKGROUND_SRC) || !json.GetValue(BACKGROUND_SRC)->IsString()) {
        LOGE("Missing src for image.");
        return nullptr;
    }
    std::string src = json.GetValue(BACKGROUND_SRC)->GetString();
    RefPtr<BackgroundImage> backgroundImage = AceType::MakeRefPtr<BackgroundImage>();
    backgroundImage->SetSrc(src, nullptr);
    if (json.Contains(IMAGE_REPEAT) && json.GetValue(IMAGE_REPEAT)->IsNumber()) {
        backgroundImage->SetImageRepeat(
            ConvertIntToEnum(json.GetValue(IMAGE_REPEAT)->GetInt(), IMAGE_REPEATS, ImageRepeat::NOREPEAT));
    }
    if (json.Contains(IMAGE_SIZE) && json.GetValue(IMAGE_SIZE)->IsObject()) {
        auto imageSizeJson = json.GetValue(IMAGE_SIZE);
        SetImageSizeFromJson(backgroundImage, *imageSizeJson);
    }
    if (json.Contains(IMAGE_POSITION) && json.GetValue(IMAGE_POSITION)->IsObject()) {
        auto imagePositionJson = json.GetValue(IMAGE_POSITION);
        SetImagePositionFromJson(backgroundImage, *imagePositionJson);
    }
    return backgroundImage;
}

void BackgroundImageCreator::SetImageSizeFromJson(RefPtr<BackgroundImage>& backgroundImage, const JsonValue& json)
{
    double value = json.GetDouble(IMAGE_SIZE_VALUE, FULL_IMG_SIZE);
    BackgroundImageSizeType type =
        ConvertIntToEnum(json.GetValue(IMAGE_SIZE_TYPE)->GetInt(), IMAGE_SIZE_TYPES, BackgroundImageSizeType::PERCENT);
    backgroundImage->SetImageSize(type, value);
}

void BackgroundImageCreator::SetImagePositionFromJson(RefPtr<BackgroundImage>& backgroundImage, const JsonValue& json)
{
    if (json.Contains(IMAGE_POSITION) && json.GetValue(IMAGE_POSITION)->IsObject()) {
        BackgroundImagePositionType typeX = ConvertIntToEnum(
            json.GetValue(IMAGE_POSITION_SIZE_X)->GetInt(), IMAGE_POSITION_TYPE, BackgroundImagePositionType::PERCENT);
        double valueX = json.GetDouble(IMAGE_POSITION_VALUE_X, CENTER_OFFSET);
        BackgroundImagePositionType typeY = ConvertIntToEnum(
            json.GetValue(IMAGE_POSITION_SIZE_Y)->GetInt(), IMAGE_POSITION_TYPE, BackgroundImagePositionType::PERCENT);
        double valueY = json.GetDouble(IMAGE_POSITION_VALUE_Y, CENTER_OFFSET);
        backgroundImage->SetImagePosition(typeX, valueX, typeY, valueY);
    }
}

} // namespace OHOS::Ace
