/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_UTILS_H

#include <algorithm>
#include <cctype>
#include <climits>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "base/geometry/axis.h"
#include "base/json/json_util.h"
#include "base/log/log.h"
#include "base/resource/asset_manager.h"
#include "base/utils/linear_map.h"
#include "base/utils/string_utils.h"
#include "base/utils/utils.h"
#include "core/animation/animation_pub.h"
#include "core/animation/curve.h"
#include "core/animation/curves.h"
#include "core/animation/spring_curve.h"
#include "core/common/ime/text_input_action.h"
#include "core/common/ime/text_input_type.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/clip_path.h"
#include "core/components/common/properties/decoration.h"
#include "core/components/common/properties/text_style.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace::Framework {

constexpr int32_t OFFSET_VALUE_NUMBER = 2;
constexpr uint8_t UTF8_CHARATER_HEAD = 0xc0;
constexpr uint8_t UTF8_CHARATER_BODY = 0x80;
constexpr int32_t UNICODE_LENGTH = 4;
constexpr int32_t STRTOL_BASE = 10;
constexpr int32_t INVALID_PARAM = -1;
constexpr int32_t PLACE_HOLDER_LENGTH = 3;

constexpr char INPUT_ACTION_NEXT[] = "next";
constexpr char INPUT_ACTION_GO[] = "go";
constexpr char INPUT_ACTION_DONE[] = "done";
constexpr char INPUT_ACTION_SEND[] = "send";
constexpr char INPUT_ACTION_SEARCH[] = "search";
constexpr char PLURAL_COUNT_POS[] = "{count}";
constexpr char DEFAULT_PLURAL_CHOICE[] = "other";

const char DOUBLE_QUOTATION = '"';
const char BACKSLASH = '\\';
const char ESCAPE_CHARATER_START = '\x00';
const char ESCAPE_CHARATER_END = '\x1f';
const char UNICODE_BODY = '0';
const char UNICODE_HEAD[] = "\\u";
const char LEFT_CURLY_BRACES = '{';
const char RIGHT_CURLY_BRACES = '}';

template<class T>
bool GetAssetContentImpl(const RefPtr<AssetManager>& assetManager, const std::string& url, T& content)
{
    if (!assetManager) {
        LOGE("AssetManager is null");
        return false;
    }
    auto jsAsset = assetManager->GetAsset(url);
    if (jsAsset == nullptr) {
        LOGW("uri:%{public}s Asset is null", url.c_str());
        return false;
    }
    auto bufLen = jsAsset->GetSize();
    auto buffer = jsAsset->GetData();
    if ((buffer == nullptr) || (bufLen <= 0)) {
        LOGE("uri:%{private}s buffer is null", url.c_str());
        return false;
    }
    content.assign(buffer, buffer + bufLen);
    return true;
}

template<class T>
bool GetAssetContentAllowEmpty(const RefPtr<AssetManager>& assetManager, const std::string& url, T& content)
{
    if (!assetManager) {
        LOGE("AssetManager is null");
        return false;
    }
    auto jsAsset = assetManager->GetAsset(url);
    if (jsAsset == nullptr) {
        LOGW("uri:%{public}s Asset is null", url.c_str());
        return false;
    }
    auto bufLen = jsAsset->GetSize();
    auto buffer = jsAsset->GetData();
    content.assign(buffer, buffer + bufLen);
    return true;
}

inline std::string GetAssetPathImpl(const RefPtr<AssetManager>& assetManager, const std::string& url)
{
    if (!assetManager) {
        LOGE("AssetManager is null");
        return "";
    }
    return assetManager->GetAssetPath(url);
}

inline std::unique_ptr<JsonValue> ParseFileData(const std::string& data)
{
    const char* endMsg = nullptr;
    auto fileData = JsonUtil::ParseJsonString(data, &endMsg);
    if (!fileData) {
        LOGE("parse i18n data failed, error: %{private}s", endMsg);
        return nullptr;
    }
    return fileData;
}

inline double StringToDouble(const std::string& value)
{
    return StringUtils::StringToDouble(value);
}

inline Dimension StringToDimension(const std::string& value)
{
    return StringUtils::StringToDimension(value);
}

inline Dimension StringToDimensionWithUnit(const std::string& value, DimensionUnit defaultUnit = DimensionUnit::PX)
{
    return StringUtils::StringToDimensionWithUnit(value, defaultUnit);
}

inline int32_t StringToInt(const std::string& value)
{
    return StringUtils::StringToInt(value);
}

inline bool StringToBool(const std::string& value)
{
    return value == "true";
}

inline BorderStyle ConvertStrToBorderStyle(const std::string& style)
{
    static const LinearMapNode<BorderStyle> borderStyleTable[] = {
        { "dashed", BorderStyle::DASHED },
        { "dotted", BorderStyle::DOTTED },
        { "solid", BorderStyle::SOLID },
    };

    auto index = BinarySearchFindIndex(borderStyleTable, ArraySize(borderStyleTable), style.c_str());
    return index < 0 ? BorderStyle::NONE : borderStyleTable[index].value;
}

inline BorderImageRepeat ConvertStrToBorderImageRepeat(const std::string& repeat)
{
    static const LinearMapNode<BorderImageRepeat> borderImageRepeatTable[] = {
        { "repeat", BorderImageRepeat::REPEAT },
        { "round", BorderImageRepeat::ROUND },
        { "space", BorderImageRepeat::SPACE },
        { "stretch", BorderImageRepeat::STRETCH },
    };

    auto index = BinarySearchFindIndex(borderImageRepeatTable, ArraySize(borderImageRepeatTable), repeat.c_str());
    return index < 0 ? BorderImageRepeat::STRETCH : borderImageRepeatTable[index].value;
}

inline BadgePosition ConvertStrToBadgePosition(const std::string& badgePosition)
{
    static const LinearMapNode<BadgePosition> badgePositionTable[] = {
        { "left", BadgePosition::LEFT },
        { "right", BadgePosition::RIGHT },
        { "rightTop", BadgePosition::RIGHT_TOP },
    };
    auto index = BinarySearchFindIndex(badgePositionTable, ArraySize(badgePositionTable), badgePosition.c_str());
    return index < 0 ? BadgePosition::RIGHT_TOP : badgePositionTable[index].value;
}

inline BoxSizing ConvertStrToBoxSizing(const std::string& value)
{
    static const LinearMapNode<BoxSizing> boxSizingTable[] = {
        { "border-box", BoxSizing::BORDER_BOX },
        { "content-box", BoxSizing::CONTENT_BOX },
    };
    auto index = BinarySearchFindIndex(boxSizingTable, ArraySize(boxSizingTable), value.c_str());
    return index < 0 ? BoxSizing::BORDER_BOX : boxSizingTable[index].value;
}

inline ImageRepeat ConvertStrToImageRepeat(const std::string& repeat)
{
    static const LinearMapNode<ImageRepeat> imageRepeatTable[] = {
        { "no-repeat", ImageRepeat::NOREPEAT },
        { "repeat", ImageRepeat::REPEAT },
        { "repeat-x", ImageRepeat::REPEATX },
        { "repeat-y", ImageRepeat::REPEATY },
    };

    auto index = BinarySearchFindIndex(imageRepeatTable, ArraySize(imageRepeatTable), repeat.c_str());
    return index < 0 ? ImageRepeat::NOREPEAT : imageRepeatTable[index].value;
}

inline FontWeight ConvertStrToFontWeight(const std::string& weight)
{
    return StringUtils::StringToFontWeight(weight);
}

inline TextDecoration ConvertStrToTextDecoration(const std::string& textDecoration)
{
    // this map should be sorted bu key.
    static const LinearMapNode<TextDecoration> textDecorationTable[] = {
        { DOM_TEXT_DECORATION_INHERIT, TextDecoration::INHERIT },
        { DOM_TEXT_DECORATION_LINETHROUGH, TextDecoration::LINE_THROUGH },
        { DOM_TEXT_DECORATION_NONE, TextDecoration::NONE },
        { DOM_TEXT_DECORATION_OVERLINE, TextDecoration::OVERLINE },
        { DOM_TEXT_DECORATION_UNDERLINE, TextDecoration::UNDERLINE },
    };

    auto index = BinarySearchFindIndex(textDecorationTable, ArraySize(textDecorationTable), textDecoration.c_str());
    return index < 0 ? TextDecoration::NONE : textDecorationTable[index].value;
}

inline WhiteSpace ConvertStrToWhiteSpace(const std::string& whiteSpace)
{
    // this map should be sorted bu key.
    static const LinearMapNode<WhiteSpace> whiteSpaceTable[] = {
        { DOM_WHITE_SPACE_INHERIT, WhiteSpace::INHERIT },
        { DOM_WHITE_SPACE_NORMAL, WhiteSpace::NORMAL },
        { DOM_WHITE_SPACE_NOWRAP, WhiteSpace::NOWRAP },
        { DOM_WHITE_SPACE_PRE, WhiteSpace::PRE },
        { DOM_WHITE_SPACE_PRELINE, WhiteSpace::PRE_LINE },
        { DOM_WHITE_SPACE_PREWRAP, WhiteSpace::PRE_WRAP },
    };

    auto index = BinarySearchFindIndex(whiteSpaceTable, ArraySize(whiteSpaceTable), whiteSpace.c_str());
    return index < 0 ? WhiteSpace::NORMAL : whiteSpaceTable[index].value;
}

inline VerticalAlign ConvertStrToTextVerticalAlign(const std::string& align)
{
    static const LinearMapNode<VerticalAlign> textVerticalAlignTable[] = {
        {DOM_BOTTOM, VerticalAlign::BOTTOM},
        {DOM_MIDDLE, VerticalAlign::CENTER},
        {DOM_TOP, VerticalAlign::TOP},
    };
    auto index = BinarySearchFindIndex(textVerticalAlignTable, ArraySize(textVerticalAlignTable), align.c_str());
    return index < 0 ? VerticalAlign::NONE : textVerticalAlignTable[index].value;
}

inline FontStyle ConvertStrToFontStyle(const std::string& fontStyle)
{
    return fontStyle == DOM_TEXT_FONT_STYLE_ITALIC ? FontStyle::ITALIC : FontStyle::NORMAL;
}

inline TextAlign ConvertStrToTextAlign(const std::string& align)
{
    static const LinearMapNode<TextAlign> textAlignTable[] = {
        { DOM_CENTER, TextAlign::CENTER },
        { DOM_END, TextAlign::END },
        { DOM_LEFT, TextAlign::LEFT },
        { DOM_RIGHT, TextAlign::RIGHT },
        { DOM_START, TextAlign::START },
    };

    auto index = BinarySearchFindIndex(textAlignTable, ArraySize(textAlignTable), align.c_str());
    return index < 0 ? TextAlign::CENTER : textAlignTable[index].value;
}

inline TextOverflow ConvertStrToTextOverflow(const std::string& overflow)
{
    return overflow == DOM_ELLIPSIS ? TextOverflow::ELLIPSIS : TextOverflow::CLIP;
}

inline Overflow ConvertStrToOverflow(const std::string& val)
{
    const LinearMapNode<Overflow> overflowTable[] = {
        { "auto", Overflow::SCROLL },
        { "hidden", Overflow::FORCE_CLIP },
        { "scroll", Overflow::SCROLL },
        { "visible", Overflow::OBSERVABLE },
    };
    auto index = BinarySearchFindIndex(overflowTable, ArraySize(overflowTable), val.c_str());
    return index < 0 ? Overflow::CLIP : overflowTable[index].value;
}

inline TextDirection ConvertStrToTextDirection(const std::string& val)
{
    const LinearMapNode<TextDirection> textDirectionTable[] = {
        { "ltr", TextDirection::LTR },
        { "rtl", TextDirection::RTL },
        { "inherit", TextDirection::INHERIT },
    };
    auto index = BinarySearchFindIndex(textDirectionTable, ArraySize(textDirectionTable), val.c_str());
    return index < 0 ? TextDirection::LTR : textDirectionTable[index].value;
}
inline std::vector<std::string> ConvertStrToFontFamilies(const std::string& family)
{
    std::vector<std::string> fontFamilies;
    std::stringstream stream(family);
    std::string fontFamily;
    while (getline(stream, fontFamily, ',')) {
        fontFamilies.emplace_back(fontFamily);
    }
    return fontFamilies;
}

inline FlexDirection ConvertStrToFlexDirection(const std::string& flexKey)
{
    return flexKey == DOM_FLEX_COLUMN ? FlexDirection::COLUMN : FlexDirection::ROW;
}

inline FlexAlign ConvertStrToFlexAlign(const std::string& flexKey)
{
    static const LinearMapNode<FlexAlign> flexMap[] = {
        { DOM_ALIGN_ITEMS_BASELINE, FlexAlign::BASELINE },
        { DOM_JUSTIFY_CONTENT_CENTER, FlexAlign::CENTER },
        { DOM_JUSTIFY_CONTENT_END, FlexAlign::FLEX_END },
        { DOM_JUSTIFY_CONTENT_START, FlexAlign::FLEX_START },
        { DOM_JUSTIFY_CONTENT_AROUND, FlexAlign::SPACE_AROUND },
        { DOM_JUSTIFY_CONTENT_BETWEEN, FlexAlign::SPACE_BETWEEN },
        { DOM_JUSTIFY_CONTENT_EVENLY, FlexAlign::SPACE_EVENLY },
        { DOM_ALIGN_ITEMS_STRETCH, FlexAlign::STRETCH },
    };
    auto index = BinarySearchFindIndex(flexMap, ArraySize(flexMap), flexKey.c_str());
    return index < 0 ? FlexAlign::FLEX_START : flexMap[index].value;
}

inline Offset ConvertStrToOffset(const std::string& value)
{
    Offset offset;
    std::vector<std::string> offsetValues;
    std::stringstream stream(value);
    std::string offsetValue;
    while (getline(stream, offsetValue, ' ')) {
        offsetValues.emplace_back(offsetValue);
    }
    // To avoid illegal input, such as "100px ".
    offsetValues.resize(OFFSET_VALUE_NUMBER);
    offset.SetX(StringToDouble(offsetValues[0]));
    offset.SetY(StringToDouble(offsetValues[1]));
    return offset;
}

inline QrcodeType ConvertStrToQrcodeType(const std::string& value)
{
    return value == "circle" ? QrcodeType::CIRCLE : QrcodeType::RECT;
}

inline AnimationCurve ConvertStrToAnimationCurve(const std::string& value)
{
    return value == "standard" ? AnimationCurve::STANDARD : AnimationCurve::FRICTION;
}

inline TextInputAction ConvertStrToTextInputAction(const std::string& action)
{
    TextInputAction inputAction;
    if (action == INPUT_ACTION_NEXT) {
        inputAction = TextInputAction::NEXT;
    } else if (action == INPUT_ACTION_GO) {
        inputAction = TextInputAction::GO;
    } else if (action == INPUT_ACTION_DONE) {
        inputAction = TextInputAction::DONE;
    } else if (action == INPUT_ACTION_SEND) {
        inputAction = TextInputAction::SEND;
    } else if (action == INPUT_ACTION_SEARCH) {
        inputAction = TextInputAction::SEARCH;
    } else {
        inputAction = TextInputAction::UNSPECIFIED;
    }
    return inputAction;
}

inline TextInputType ConvertStrToTextInputType(const std::string& type)
{
    TextInputType inputType;
    if (type == DOM_INPUT_TYPE_NUMBER) {
        inputType = TextInputType::NUMBER;
    } else if (type == DOM_INPUT_TYPE_DATE || type == DOM_INPUT_TYPE_TIME) {
        inputType = TextInputType::DATETIME;
    } else if (type == DOM_INPUT_TYPE_EMAIL) {
        inputType = TextInputType::EMAIL_ADDRESS;
    } else if (type == DOM_INPUT_TYPE_PASSWORD) {
        inputType = TextInputType::VISIBLE_PASSWORD;
    } else {
        inputType = TextInputType::TEXT;
    }
    return inputType;
}

inline TabBarMode ConvertStrToTabBarMode(const std::string& value)
{
    std::string temp = value;
    transform(temp.begin(), temp.end(), temp.begin(), tolower);
    return temp == "fixed" ? TabBarMode::FIXED : TabBarMode::SCROLLABEL;
}

RefPtr<Curve> CreateBuiltinCurve(const std::string& aniTimFunc);

RefPtr<Curve> CreateCustomCurve(const std::string& aniTimFunc);

ACE_EXPORT RefPtr<Curve> CreateCurve(const std::string& aniTimFunc);

ACE_EXPORT TransitionType ParseTransitionType(const std::string& transitionType);

ACE_EXPORT RefPtr<ClipPath> CreateClipPath(const std::string& value);

inline FillMode StringToFillMode(const std::string& fillMode)
{
    if (fillMode == DOM_ANIMATION_FILL_MODE_FORWARDS) {
        return FillMode::FORWARDS;
    } else if (fillMode == DOM_ANIMATION_FILL_MODE_BACKWARDS) {
        return FillMode::BACKWARDS;
    } else if (fillMode == DOM_ANIMATION_FILL_MODE_BOTH) {
        return FillMode::BOTH;
    } else {
        return FillMode::NONE;
    }
}

inline AnimationDirection StringToAnimationDirection(const std::string& direction)
{
    if (direction == DOM_ANIMATION_DIRECTION_ALTERNATE) {
        return AnimationDirection::ALTERNATE;
    } else if (direction == DOM_ANIMATION_DIRECTION_REVERSE) {
        return AnimationDirection::REVERSE;
    } else if (direction == DOM_ANIMATION_DIRECTION_ALTERNATE_REVERSE) {
        return AnimationDirection::ALTERNATE_REVERSE;
    } else {
        return AnimationDirection::NORMAL;
    }
}

inline AnimationOperation StringToAnimationOperation(const std::string& direction)
{
    if (direction == DOM_ANIMATION_PLAY_STATE_IDLE) {
        return AnimationOperation::CANCEL;
    } else if (direction == DOM_ANIMATION_PLAY_STATE_RUNNING) {
        return AnimationOperation::RUNNING;
    } else if (direction == DOM_ANIMATION_PLAY_STATE_PAUSED) {
        return AnimationOperation::PAUSE;
    } else if (direction == DOM_ANIMATION_PLAY_STATE_FINISHED) {
        return AnimationOperation::FINISH;
    } else {
        return AnimationOperation::NONE;
    }
}

inline void RemoveHeadTailSpace(std::string& value)
{
    if (!value.empty()) {
        auto start = value.find_first_not_of(' ');
        if (start == std::string::npos) {
            value.clear();
        } else {
            value = value.substr(start, value.find_last_not_of(' ') - start + 1);
        }
    }
}

inline GradientDirection StrToGradientDirection(const std::string& direction)
{
    static const LinearMapNode<GradientDirection> gradientDirectionTable[] = {
        { DOM_GRADIENT_DIRECTION_LEFT, GradientDirection::LEFT },
        { DOM_GRADIENT_DIRECTION_RIGHT, GradientDirection::RIGHT },
        { DOM_GRADIENT_DIRECTION_TOP, GradientDirection::TOP },
        { DOM_GRADIENT_DIRECTION_BOTTOM, GradientDirection::BOTTOM },
    };

    auto index = BinarySearchFindIndex(gradientDirectionTable, ArraySize(gradientDirectionTable), direction.c_str());
    return index < 0 ? GradientDirection::BOTTOM : gradientDirectionTable[index].value;
}

bool ParseBackgroundImagePosition(const std::string& value, BackgroundImagePosition& backgroundImagePosition);

bool ParseBackgroundImageSize(const std::string& value, BackgroundImageSize& backgroundImageSize);

ImageObjectPosition ParseImageObjectPosition(const std::string& value);

std::optional<RadialSizeType> ParseRadialGradientSize(const std::string& value);

inline bool StartWith(const std::string& dst, const std::string& prefix)
{
    return dst.compare(0, prefix.size(), prefix) == 0;
}

inline bool EndWith(const std::string& dst, const std::string& suffix)
{
    return dst.size() >= suffix.size() && dst.compare(dst.size() - suffix.size(), suffix.size(), suffix) == 0;
}

inline double ConvertTimeStr(const std::string& str)
{
    std::string time(str);
    StringUtils::TrimStr(time);
    double result = 0.0;
    if (EndWith(time, "ms")) {
        result = StringToDouble(std::string(time.begin(), time.end() - 2.0));
    } else if (EndWith(time, "s")) {
        result = StringToDouble(std::string(time.begin(), time.end() - 1.0)) * 1000.0;
    } else if (EndWith(time, "m")) {
        result = StringToDouble(std::string(time.begin(), time.end() - 1.0)) * 60.0 * 1000.0;
    } else {
        result = StringToDouble(str);
    }
    return result;
}

inline WordBreak ConvertStrToWordBreak(const std::string& wordBreak)
{
    return StringUtils::StringToWordBreak(wordBreak);
}

inline bool CheckTransformEnum(const std::string& str)
{
    const static std::unordered_set<std::string> offsetKeywords = { "left", "right", "center", "top", "bottom" };

    return offsetKeywords.find(str) != offsetKeywords.end();
}

inline std::pair<bool, Dimension> ConvertStrToTransformOrigin(const std::string& str, Axis axis)
{
    const static std::unordered_map<std::string, Dimension> xOffsetKeywords = {
        { "left", 0.0_pct },
        { "right", 1.0_pct },
        { "center", 0.5_pct },
    };
    const static std::unordered_map<std::string, Dimension> yOffsetKeywords = {
        { "top", 0.0_pct },
        { "bottom", 1.0_pct },
        { "center", 0.5_pct },
    };

    if (axis == Axis::HORIZONTAL) {
        auto pos = xOffsetKeywords.find(str);
        if (pos != xOffsetKeywords.end()) {
            return std::make_pair(true, pos->second);
        }
    } else if (axis == Axis::VERTICAL) {
        auto pos = yOffsetKeywords.find(str);
        if (pos != yOffsetKeywords.end()) {
            return std::make_pair(true, pos->second);
        }
    }

    return std::make_pair(false, Dimension {});
}

inline int32_t ParseUtf8TextLength(std::string& text)
{
    int32_t trueLength = 0;
    int32_t stringLength = 0;
    while (stringLength < static_cast<int32_t>(text.length())) {
        uint8_t stringChar = static_cast<uint8_t>(text[stringLength++]);
        if ((stringChar & UTF8_CHARATER_HEAD) != UTF8_CHARATER_BODY) {
            trueLength++;
        }
    }
    return trueLength;
}

inline int32_t ParseUtf8TextSubstrStartPos(std::string& text, int32_t targetPos)
{
    int32_t truePos = 0;
    int32_t stringPos = 0;
    while ((stringPos < static_cast<int32_t>(text.length())) && (truePos < targetPos)) {
        uint8_t stringChar = static_cast<uint8_t>(text[stringPos++]);
        if ((stringChar & UTF8_CHARATER_HEAD) != UTF8_CHARATER_BODY) {
            truePos++;
        }
    }

    return stringPos;
}

inline int32_t ParseUtf8TextSubstrEndPos(std::string& text, int32_t targetPos)
{
    auto stringPos = ParseUtf8TextSubstrStartPos(text, targetPos);
    while (stringPos < static_cast<int32_t>(text.length())) {
        uint8_t stringChar = static_cast<uint8_t>(text[stringPos]);
        if ((stringChar & UTF8_CHARATER_HEAD) != UTF8_CHARATER_BODY) {
            break;
        }
        stringPos++;
    }

    return stringPos;
}

inline void HandleEscapeCharaterInUtf8TextForJson(std::string& text)
{
    for (size_t pos = 0; pos < text.size(); pos++) {
        if ((text.at(pos) == DOUBLE_QUOTATION) || (text.at(pos) == BACKSLASH) ||
            ((text.at(pos) >= ESCAPE_CHARATER_START) && (text.at(pos) <= ESCAPE_CHARATER_END))) {
            std::ostringstream is;
            is << UNICODE_HEAD << std::hex << std::setw(UNICODE_LENGTH) << std::setfill(UNICODE_BODY)
                << int(text.at(pos));
            text.replace(pos, 1, is.str());
        }
    }
}

inline int32_t ParseResourceInputNumberParam(const std::string& param)
{
    if (!StringUtils::IsNumber(param) || param.empty()) {
        return INVALID_PARAM;
    } else {
        errno = 0;
        char* pEnd = nullptr;
        int64_t result = std::strtol(param.c_str(), &pEnd, STRTOL_BASE);
        if ((result < INT_MIN || result > INT_MAX) || errno == ERANGE) {
            return INT_MAX;
        } else {
            return static_cast<int32_t>(result);
        }
    }
}

inline void ReplacePlaceHolderArray(std::string& resultStr, const std::vector<std::string>& arrayResult)
{
    auto size = arrayResult.size();
    size_t startPos = 0;
    for (size_t i = 0; i < size; i++) {
        std::string placeHolder;
        placeHolder += LEFT_CURLY_BRACES;
        placeHolder += (i + '0');
        placeHolder += RIGHT_CURLY_BRACES;
        if (startPos < resultStr.length()) {
            auto pos = resultStr.find(placeHolder, startPos);
            if (pos != std::string::npos) {
                resultStr.replace(pos, PLACE_HOLDER_LENGTH, arrayResult[i]);
                startPos = pos + arrayResult[i].length();
            }
        }
    }
}

inline void ReplacePlaceHolder(std::string& resultStr, const std::unique_ptr<JsonValue>& argsPtr)
{
    auto placeHolderKey = argsPtr->GetChild()->GetKey();
    std::string placeHolder;
    placeHolder += LEFT_CURLY_BRACES;
    placeHolder += placeHolderKey;
    placeHolder += RIGHT_CURLY_BRACES;
    auto pos = resultStr.find(placeHolder);
    if (pos != std::string::npos) {
        resultStr.replace(pos, placeHolder.length(), argsPtr->GetChild()->GetString());
    }
}

inline std::string ParserPluralResource(const std::unique_ptr<JsonValue>& argsPtr, const std::string& choice,
    const std::string& count)
{
    std::string valueStr;
    std::string defaultPluralStr(DEFAULT_PLURAL_CHOICE);
    if (argsPtr->Contains(choice)) {
        valueStr = argsPtr->GetValue(choice)->GetString();
    } else if (argsPtr->Contains(defaultPluralStr)) {
        valueStr = argsPtr->GetValue(defaultPluralStr)->GetString();
    } else {
        return std::string();
    }

    std::string pluralStr(PLURAL_COUNT_POS);
    auto pos = valueStr.find(pluralStr);
    if (pos != std::string::npos) {
        valueStr.replace(pos, pluralStr.length(), count);
    }
    return valueStr;
}

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_UTILS_H
