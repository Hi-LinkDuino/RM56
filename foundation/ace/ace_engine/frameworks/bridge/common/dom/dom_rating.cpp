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

#include "frameworks/bridge/common/dom/dom_rating.h"

#include "core/components/rating/rating_theme.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMRating::DOMRating(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    ratingChild_ = AceType::MakeRefPtr<RatingComponent>();
}

void DOMRating::ResetInitializedStyle()
{
    SetThemeAttrs();
}

bool DOMRating::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    if (attr.first == DOM_RATING_INDICATOR) {
        ratingChild_->SetIndicator(StringToBool(attr.second));
        return true;
    }
    // DOMRating attributes map
    static const std::unordered_map<std::string, void (*)(const std::string&, DOMRating&)> ratingAttrOperators = {
        { DOM_STAR_NUM,
            [](const std::string& val, DOMRating& rating) {
                rating.starNum_.first = StringToInt(val);
                rating.starNum_.second = true;
            } },
        { DOM_RATING_SCORE,
            [](const std::string& val, DOMRating& rating) {
                rating.ratingScore_.first = StringToDouble(val);
                rating.ratingScore_.second = true;
            } },
        { DOM_RATING_STEP,
            [](const std::string& val, DOMRating& rating) {
                rating.stepSize_.first = StringToDouble(val);
                rating.stepSize_.second = true;
            } },
    };
    auto operatorIter = ratingAttrOperators.find(attr.first);
    if (operatorIter != ratingAttrOperators.end()) {
        operatorIter->second(attr.second, *this);
        return true;
    }

    return false;
}

bool DOMRating::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    if (style.first == DOM_RTL_FLIP) {
        ratingChild_->SetRtlFlip(StringToBool(style.second));
        return true;
    }
    // DOMRating styles map
    static const std::unordered_map<std::string, void (*)(const std::string&, DOMRating&)> ratingStyleOperators = {
        { DOM_BACKGROUND_SRC,
            [](const std::string& val, DOMRating& rating) {
                rating.backgroundSrc_.first = val;
                rating.backgroundSrc_.second = true;
            } },
        { DOM_FOREGROUND_SRC,
            [](const std::string& val, DOMRating& rating) {
                rating.foregroundSrc_.first = val;
                rating.foregroundSrc_.second = true;
            } },
        { DOM_SECONDARY_SRC,
            [](const std::string& val, DOMRating& rating) {
                rating.secondarySrc_.first = val;
                rating.secondarySrc_.second = true;
            } },
    };
    auto operatorIter = ratingStyleOperators.find(style.first);
    if (operatorIter != ratingStyleOperators.end()) {
        operatorIter->second(style.second, *this);
        return true;
    }

    return false;
}

bool DOMRating::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    LOGD("DOMRating AddEvent");
    if (event == DOM_CHANGE) {
        changeEventId_ = EventMarker(GetNodeIdForEvent(), event, pageId);
        ratingChild_->SetChangeEventId(changeEventId_);
        return true;
    }
    return false;
}

void DOMRating::SetThemeAttrs()
{
    auto theme = GetTheme<RatingTheme>();
    if (!theme) {
        return;
    }
    if (ratingChild_->GetIndicator()) {
        // If box's size not defined, use theme to initialize.
        if (boxComponent_->GetHeightDimension().Value() < 0.0) {
            SetHeight(theme->GetRatingMiniHeight());
            boxComponent_->SetHeight(theme->GetRatingMiniHeight().Value(), theme->GetRatingMiniHeight().Unit());
        }
        if (boxComponent_->GetWidthDimension().Value() < 0.0) {
            SetWidth(theme->GetRatingMiniWidth());
            boxComponent_->SetWidth(theme->GetRatingMiniWidth().Value(), theme->GetRatingMiniWidth().Unit());
        }
        ratingChild_->SetPaddingVertical(Dimension());
        ratingChild_->SetRatingScore(theme->GetRatingMiniScore());
        ratingChild_->SetMiniResIdFromTheme(theme);
    } else {
        // If box's size not defined, use theme to initialize.
        if (boxComponent_->GetHeightDimension().Value() < 0.0) {
            SetHeight(theme->GetRatingHeight());
            boxComponent_->SetHeight(theme->GetRatingHeight().Value(), theme->GetRatingHeight().Unit());
        }
        if (boxComponent_->GetWidthDimension().Value() < 0.0) {
            SetWidth(theme->GetRatingWidth());
            boxComponent_->SetWidth(theme->GetRatingWidth().Value(), theme->GetRatingWidth().Unit());
        }
        ratingChild_->SetRatingScore(theme->GetRatingScore());
        ratingChild_->SetPaddingVertical(theme->GetPaddingVertical());
        ratingChild_->SetResIdFromTheme(theme);
    }
    ratingChild_->SetStarNum(theme->GetStarNum());
    ratingChild_->SetStepSize(theme->GetStepSize());
    ratingChild_->SetDefaultHeight(theme->GetRatingHeight());
    ratingChild_->SetDesignedStarAspectRatio(theme->GetDesignedStarAspectRatio());
    ratingChild_->SetFocusBorderWidth(theme->GetFocusBorderWidth());
    ratingChild_->SetFocusBorderRadius(theme->GetFocusBorderRadius());
    ratingChild_->SetHoverColor(theme->GetHoverColor());
    ratingChild_->SetStarColorActive(theme->GetStarColorActive());
    ratingChild_->SetStarColorInactive(theme->GetStarColorInactive());
}

void DOMRating::PrepareSpecializedComponent()
{
    SetThemeAttrs();
    ratingChild_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    if (starNum_.second) {
        ratingChild_->SetStarNum(starNum_.first);
    }
    if (ratingScore_.second) {
        ratingChild_->SetRatingScore(ratingScore_.first);
    }
    if (stepSize_.second) {
        ratingChild_->SetStepSize(stepSize_.first);
    }
    if (backgroundSrc_.second) {
        ratingChild_->SetBackgroundSrc(backgroundSrc_.first);
    }
    if (foregroundSrc_.second) {
        ratingChild_->SetForegroundSrc(foregroundSrc_.first);
    }
    if (secondarySrc_.second) {
        ratingChild_->SetSecondarySrc(secondarySrc_.first);
    }
}

} // namespace OHOS::Ace::Framework
