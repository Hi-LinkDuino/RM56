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

#include "core/components/test/json/rating_creator.h"

#include "base/log/log.h"
#include "core/components/image/image_component.h"
#include "core/components/rating/rating_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> RatingCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory&)
{
    LOGI("CreateFromJson Rating");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != RATING_NAME) {
        LOGE("Create Rating err: not a Rating json.");
        return nullptr;
    }
    auto rating = AceType::MakeRefPtr<RatingComponent>();
    int32_t starNum = componentJson.GetInt(STAR_NUM, DEFAULT_RATING_STAR_NUM);
    rating->SetStarNum(starNum);
    double ratingScore = componentJson.GetDouble(RATING_SCORE, DEFAULT_RATING_SCORE);
    rating->SetRatingScore(ratingScore);
    double stepSize = componentJson.GetDouble(STEP_SIZE, DEFAULT_RATING_STEP_SIZE);
    rating->SetStepSize(stepSize);

    double width = componentJson.GetDouble(RATING_WIDTH, DEFAULT_RATING_WIDTH);
    rating->SetWidth(Dimension(width, DimensionUnit::PX));
    double height = componentJson.GetDouble(RATING_HEIGHT, DEFAULT_RATING_HEIGHT);
    rating->SetHeight(Dimension(height, DimensionUnit::PX));
    double paddingHorizontal = componentJson.GetDouble(RATING_HORIZONTAL_PADDING, DEFAULT_RATING_HORIZONTAL_PADDING);
    rating->SetPaddingHorizontal(Dimension(paddingHorizontal, DimensionUnit::PX));
    double paddingVertical = componentJson.GetDouble(RATING_VERTICAL_PADDING, DEFAULT_RATING_VERTICAL_PADDING);
    rating->SetPaddingVertical(Dimension(paddingVertical, DimensionUnit::PX));

    std::string foregroundSrc = componentJson.GetString(RATING_FOREGROUND_SRC);
    rating->SetForegroundSrc(foregroundSrc);
    rating->GetForegroundImage()->SetSrc(foregroundSrc);

    std::string secondarySrc = componentJson.GetString(RATING_SECONDARY_SRC);
    rating->SetSecondarySrc(secondarySrc);
    rating->GetSecondaryImage()->SetSrc(secondarySrc);

    std::string backgroundSrc = componentJson.GetString(RATING_BACKGROUND_SRC);
    rating->SetBackgroundSrc(backgroundSrc);
    rating->GetBackgroundImage()->SetSrc(backgroundSrc);

    int32_t indicator = componentJson.GetInt(RATING_INDICATOR, DEFAULT_INDICATOR_VALUE);
    rating->SetIndicator((indicator == DEFAULT_NO_INDICATOR_VALUE) ? false : true);

    auto changeEventID = componentJson.GetString(RATING_ON_CHANGE);
    rating->SetChangeEventId(EventMarker(changeEventID));

    return rating;
}

} // namespace OHOS::Ace
