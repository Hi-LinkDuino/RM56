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

#include "frameworks/bridge/common/media_query/media_queryer.h"

#include <list>
#include <regex>

#include "base/log/log.h"
#include "frameworks/bridge/common/media_query/media_query_info.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr double NOT_FOUND = -1.0;
enum class MediaError {
    NONE,
    SYNTAX,
};
using ConditionParser =
    std::function<bool(const std::smatch& matchResults, const MediaFeature& mediaFeature, MediaError& failReason)>;

class MediaQueryerRule {
public:
    MediaQueryerRule(const std::regex& regex, const ConditionParser& parser, uint32_t matchResultSize)
        : regex_(regex), parser_(parser), matchResultSize_(matchResultSize)
    {}
    explicit MediaQueryerRule(const std::regex& regex) : regex_(regex) {}
    ~MediaQueryerRule() = default;

    bool ParseCondition(std::smatch& matchResults, const MediaFeature& mediaFeature, MediaError& failReason) const
    {
        if (!parser_) {
            return false;
        }
        if (matchResults.size() != matchResultSize_) {
            failReason = MediaError::SYNTAX;
            return false;
        }
        return parser_(matchResults, mediaFeature, failReason);
    }
    bool Match(const std::string& condition, std::smatch& matchResults) const
    {
        return std::regex_match(condition, matchResults, regex_);
    }
    bool Match(const std::string& condition) const
    {
        return std::regex_match(condition, regex_);
    }

private:
    const std::regex regex_;
    const ConditionParser parser_;
    const uint32_t matchResultSize_ = 0;
};

namespace RelationShip {
const std::string GREAT_OR_EQUAL(">=");
const std::string GREAT_NOT_EQUAL = ">";
const std::string LESS_OR_EQUAL = "<=";
const std::string LESS_NOT_EQUAL = "<";
}; // namespace RelationShip

/**
 * transfer unit the same with condition value unit
 * @param value: device value should be transfer unit the same with condition value
 * @param unit: condition value unit, such as: dpi/dpcm/dppx
 */
double TransferValue(double value, const std::string& unit)
{
    double transfer;
    if (unit == "dpi") {
        transfer = 96.0; // 1px = 96 dpi
    } else if (unit == "dpcm") {
        transfer = 36.0; // 1px = 36 dpcm
    } else {
        transfer = 1.0; // default same with device unit: px
    }
    return value * transfer;
}

bool CalculateExpression(double lvalue, const std::string& relationship, double rvalue, MediaError& failReason)
{
    if (relationship == RelationShip::GREAT_OR_EQUAL) {
        return GreatOrEqual(lvalue, rvalue);
    } else if (relationship == RelationShip::GREAT_NOT_EQUAL) {
        return GreatNotEqual(lvalue, rvalue);
    } else if (relationship == RelationShip::LESS_OR_EQUAL) {
        return LessOrEqual(lvalue, rvalue);
    } else if (relationship == RelationShip::LESS_NOT_EQUAL) {
        return LessNotEqual(lvalue, rvalue);
    } else {
        failReason = MediaError::SYNTAX;
    }
    return false;
}

const MediaQueryerRule CONDITION_WITH_SCREEN(
    std::regex("(((only|not)screen)|screen)((and|or|,)\\([\\w\\.:><=-]+\\))*"));
const MediaQueryerRule CONDITION_WITHOUT_SCREEN(std::regex("\\([\\w\\.:><=-]+\\)((and|or|,)\\([\\w\\.:><=-]+\\))*"));
const MediaQueryerRule CONDITION_WITH_AND(std::regex("(\\([\\.a-z0-9:>=<-]+\\))(and\\([\\.a-z0-9:>=<-]+\\))+"));

// condition such as: (100 < width < 1000)
const MediaQueryerRule CSS_LEVEL4_MULTI(
    std::regex("\\(([\\d\\.]+)(dpi|dppx|dpcm|px)?(>|<|>=|<=)([a-z0-9:-]+)(>|<|>=|<=)([\\d\\.]+)(dpi|dppx|dpcm|px)?\\)"),
    [](const std::smatch& matchResults, const MediaFeature& mediaFeature, MediaError& failReason) {
        static constexpr int32_t LEFT_CONDITION_VALUE = 6;
        static constexpr int32_t LEFT_UNIT = 7;
        static constexpr int32_t LEFT_RELATIONSHIP = 5;
        static constexpr int32_t MEDIA_FEATURE = 4;
        static constexpr int32_t RIGHT_CONDITION_VALUE = 1;
        static constexpr int32_t RIGHT_UNIT = 2;
        static constexpr int32_t RIGHT_RELATIONSHIP = 3;

        auto mediaFeatureValue = mediaFeature->GetDouble(matchResults[MEDIA_FEATURE], NOT_FOUND);
        return CalculateExpression(TransferValue(mediaFeatureValue, matchResults[LEFT_UNIT]),
                   matchResults[LEFT_RELATIONSHIP], StringToDouble(matchResults[LEFT_CONDITION_VALUE]), failReason) &&
               CalculateExpression(StringToDouble(matchResults[RIGHT_CONDITION_VALUE]),
                   matchResults[RIGHT_RELATIONSHIP], TransferValue(mediaFeatureValue, matchResults[RIGHT_UNIT]),
                   failReason);
    },
    8);

// condition such as: width < 1000
const MediaQueryerRule CSS_LEVEL4_LEFT(
    std::regex("\\(([^m][a-z-]+)(>|<|>=|<=)([\\d\\.]+)(dpi|dppx|dpcm|px)?\\)"),
    [](const std::smatch& matchResults, const MediaFeature& mediaFeature, MediaError& failReason) {
        static constexpr int32_t CONDITION_VALUE = 3;
        static constexpr int32_t UNIT = 4;
        static constexpr int32_t RELATIONSHIP = 2;
        static constexpr int32_t MEDIA_FEATURE = 1;

        return CalculateExpression(
            TransferValue(mediaFeature->GetDouble(matchResults[MEDIA_FEATURE], NOT_FOUND), matchResults[UNIT]),
            matchResults[RELATIONSHIP], StringToDouble(matchResults[CONDITION_VALUE]), failReason);
    },
    5);

// condition such as: 1000 < width
const MediaQueryerRule CSS_LEVEL4_RIGHT(
    std::regex("\\(([\\d\\.]+)(dpi|dppx|dpcm|px)?(>|<|>=|<=)([^m][a-z-]+)\\)"),
    [](const std::smatch& matchResults, const MediaFeature& mediaFeature, MediaError& failReason) {
        static constexpr int32_t CONDITION_VALUE = 1;
        static constexpr int32_t UNIT = 2;
        static constexpr int32_t RELATIONSHIP = 3;
        static constexpr int32_t MEDIA_FEATURE = 4;
        return CalculateExpression(StringToDouble(matchResults[CONDITION_VALUE]), matchResults[RELATIONSHIP],
            TransferValue(mediaFeature->GetDouble(matchResults[MEDIA_FEATURE], NOT_FOUND), matchResults[UNIT]),
            failReason);
    },
    5);

// condition such as: min-width: 1000
const MediaQueryerRule CSS_LEVEL3_RULE(
    std::regex("\\((min|max)-([a-z-]+):([\\d\\.]+)(dpi|dppx|dpcm)?\\)"),
    [](const std::smatch& matchResults, const MediaFeature& mediaFeature, MediaError& failReason) {
        static constexpr int32_t RELATIONSHIP = 1;
        static constexpr int32_t MEDIA_FEATURE = 2;
        static constexpr int32_t CONDITION_VALUE = 3;
        static constexpr int32_t UNIT = 4;
        std::string relationship;
        if (matchResults[RELATIONSHIP] == "max") {
            relationship = RelationShip::LESS_OR_EQUAL;
        } else if (matchResults[RELATIONSHIP] == "min") {
            relationship = RelationShip::GREAT_OR_EQUAL;
        } else {
            return false;
        }

        return CalculateExpression(
            TransferValue(mediaFeature->GetDouble(matchResults[MEDIA_FEATURE], NOT_FOUND), matchResults[UNIT]),
            relationship, StringToDouble(matchResults[CONDITION_VALUE]), failReason);
    },
    5);

const MediaQueryerRule SCREEN_SHAPE_RULE(
    std::regex("\\(round-screen:([a-z]+)\\)"),
    [](const std::smatch& matchResults, const MediaFeature& mediaFeature, MediaError& failReason) {
        static constexpr int32_t CONDITION_VALUE = 1;
        return StringToBool(matchResults[CONDITION_VALUE]) == mediaFeature->GetBool("round-screen", false);
    },
    2);

const MediaQueryerRule ORIENTATION_RULE(
    std::regex("\\(orientation:([a-z]+)\\)"),
    [](const std::smatch& matchResults, const MediaFeature& mediaFeature, MediaError& failReason) {
        static constexpr int32_t CONDITION_VALUE = 1;
        return matchResults[CONDITION_VALUE] == mediaFeature->GetString("orientation", "");
    },
    2);

const MediaQueryerRule DEVICE_TYPE_RULE(
    std::regex("\\(device-type:([a-z]+)\\)"),
    [](const std::smatch& matchResults, const MediaFeature& mediaFeature, MediaError& failReason) {
        static constexpr int32_t CONDITION_VALUE = 1;
        return matchResults[CONDITION_VALUE] == mediaFeature->GetString("device-type", "");
    },
    2);

const MediaQueryerRule DEVICE_BRAND_RULE(
    std::regex("\\(device-brand:([A-Z]+)\\)"),
    [](const std::smatch& matchResults, const MediaFeature& mediaFeature, MediaError& failReason) {
        static constexpr int32_t CONDITION_VALUE = 1;
        auto value = matchResults[CONDITION_VALUE] == mediaFeature->GetString("device-brand", "");
        return value;
    },
    2);

const MediaQueryerRule DARK_MODE_RULE(
    std::regex("\\(dark-mode:([a-z]+)\\)"),
    [](const std::smatch& matchResults, const MediaFeature& mediaFeature, MediaError& failReason) {
        static constexpr int32_t CONDITION_VALUE = 1;
        return StringToBool(matchResults[CONDITION_VALUE]) == mediaFeature->GetBool("dark-mode", false);
    },
    2);

const std::list<MediaQueryerRule> SINGLE_CONDITION_RULES = {
    CSS_LEVEL4_MULTI,
    CSS_LEVEL4_LEFT,
    CSS_LEVEL4_RIGHT,
    CSS_LEVEL3_RULE,
    ORIENTATION_RULE,
    DEVICE_TYPE_RULE,
    DEVICE_BRAND_RULE,
    SCREEN_SHAPE_RULE,
    DARK_MODE_RULE,

};

bool ParseSingleCondition(const std::string& condition, const MediaFeature& mediaFeature, MediaError& failReason)
{
    for (const auto& rule : SINGLE_CONDITION_RULES) {
        std::smatch matchResults;
        if (rule.Match(condition, matchResults)) {
            return rule.ParseCondition(matchResults, mediaFeature, failReason);
        }
    }
    LOGE("illegal condition");
    failReason = MediaError::SYNTAX;
    return false;
}

bool ParseAndCondition(const std::string& condition, const MediaFeature& mediaFeature, MediaError& failReason)
{
    auto noAnd = std::regex_replace(condition, std::regex("and[^a-z]"), ",(");
    std::vector<std::string> conditionArr;
    StringUtils::SplitStr(noAnd, ",", conditionArr);
    if (conditionArr.empty()) {
        failReason = MediaError::SYNTAX;
        return false;
    }

    for (const auto& item : conditionArr) {
        if (!ParseSingleCondition(item, mediaFeature, failReason)) {
            return false;
        }
    }
    return true;
}

bool DoMatchCondition(const std::string& condition, const MediaFeature& mediaFeature)
{
    // remove space from condition string
    std::string noSpace = std::regex_replace(condition, std::regex("\\s"), "");
    bool inverse = false;
    std::string noScreen;
    if (CONDITION_WITH_SCREEN.Match(noSpace)) {
        if (noSpace.find("notscreen") != std::string::npos) {
            inverse = true;
        }
        MediaQueryerRule screenPatten(std::regex("screen[^and:]"));
        if (screenPatten.Match(noSpace)) {
            return !inverse;
        }
        noScreen = std::regex_replace(noSpace, std::regex("^(only|not)?screen(and)?"), "");
    } else if (CONDITION_WITHOUT_SCREEN.Match(noSpace)) {
        noScreen = noSpace;
    } else {
        LOGE("illegal condition");
        return false;
    }
    MediaError failReason = MediaError::NONE;
    // replace 'or' with comma ','
    auto commaCondition = std::regex_replace(noScreen, std::regex("or[(]"), ",(");
    // remove screen and modifier
    std::vector<std::string> conditionArr;
    StringUtils::SplitStr(commaCondition, ",", conditionArr);
    int32_t len = static_cast<int32_t>(conditionArr.size());
    for (int32_t i = 0; i < len; i++) {
        if (CONDITION_WITH_AND.Match(conditionArr[i])) {
            bool result = ParseAndCondition(conditionArr[i], mediaFeature, failReason);
            if (failReason == MediaError::SYNTAX) {
                return false;
            }
            if (i + 1 == len) {
                return (inverse && !result) || (!inverse && result);
            }
        } else {
            if (ParseSingleCondition(conditionArr[i], mediaFeature, failReason)) {
                return !inverse;
            }
            if (failReason == MediaError::SYNTAX) {
                return false;
            }
        }
    }
    return inverse;
}

} // namespace

bool MediaQueryer::MatchCondition(const std::string& condition, const MediaFeature& mediaFeature)
{
    if (condition.empty()) {
        return false;
    }

    // If width and height are not initialized, and the query condition includes "width" or "height",
    // return false directly.
    if (mediaFeature->GetInt("width", 0) == 0 &&
        (condition.find("width") != std::string::npos || condition.find("height") != std::string::npos)) {
        return false;
    }

    auto iter = queryHistories.find(condition);
    if (iter != queryHistories.end()) {
        auto queryHistory = iter->second;
        if (queryHistory.mediaFeatureValue == mediaFeature->ToString()) {
            return queryHistory.result;
        }
    }

    auto result = DoMatchCondition(condition, mediaFeature);
    queryHistories[condition] = { mediaFeature->ToString(), result };
    return result;
}

/* card info */
std::unique_ptr<JsonValue> MediaQueryer::GetMediaFeature() const
{
    auto json = MediaQueryInfo::GetMediaQueryJsonInfo();

    /* cover the following aspects with card specified values */
    double aspectRatio = (height_ != 0) ? (static_cast<double>(width_) / height_) : 1.0;
    json->Replace("width", width_);
    json->Replace("height", height_);
    json->Replace("aspect-ratio", aspectRatio);
    json->Replace("dark-mode", colorMode_ == ColorMode::DARK);
    json->Put("device-brand", SystemProperties::GetBrand().c_str());
    LOGD("current media info %{public}s", json->ToString().c_str());
    return json;
}

} // namespace OHOS::Ace::Framework
