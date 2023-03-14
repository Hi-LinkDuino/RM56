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

#include <regex>
#include <unordered_map>

#include "core/components/common/properties/text_style.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t FONT_FEATURE_MAX_SIZE = 2;
constexpr int32_t FONT_FEATURE_KEY_LENGTH = 6;
constexpr int32_t FONT_FEATURE_PARENTHESES_LENGTH = 2; // length of ()
const char FONT_FEATURE_NONE[] = "none";
const char FONT_FEATURE_NORMAL[] = "normal";
const char FONT_FEATURE_ON[] = "on";
const char FONT_FEATURE_OFF[] = "off";

} // namespace

bool ParseFontVariantCaps(const std::string& fontVariant, std::unordered_map<std::string, int32_t>& fontFeatures)
{
    if (fontVariant == FONT_FEATURE_NORMAL || fontVariant.empty()) {
        return true;
    }

    const std::unordered_map<std::string, void (*)(std::unordered_map<std::string, int32_t>&)> operators = {
        { "small-caps",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("smcp", 1); } },
        { "all-small-caps",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) {
                fontFeatures.try_emplace("c2sc", 1);
                fontFeatures.try_emplace("smcp", 1);
            } },
        { "petite-caps",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("pcap", 1); } },
        { "all-petite-caps",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) {
                fontFeatures.try_emplace("c2pc", 1);
                fontFeatures.try_emplace("pcap", 1);
            } },
        { "unicase",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("unic", 1); } },
        { "titling-caps",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("titl", 1); } },
    };
    auto iter = operators.find(fontVariant);
    if (iter != operators.end()) {
        iter->second(fontFeatures);
        return true;
    }
    return false;
}

bool ParseFontVariantNumeric(const std::string& fontVariant, std::unordered_map<std::string, int32_t>& fontFeatures)
{
    if (fontVariant == FONT_FEATURE_NORMAL || fontVariant.empty()) {
        return true;
    }

    const std::unordered_map<std::string, void (*)(std::unordered_map<std::string, int32_t>&)> operators = {
        { "ordinal",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("ordn", 1); } },
        { "slashed-zero",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("zero", 1); } },
        { "lining-nums",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("lnum", 1); } },
        { "oldstyle-nums",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("onum", 1); } },
        { "proportional-nums",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("pnum", 1); } },
        { "tabular-nums",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("tnum", 1); } },
        { "diagonal-fractions",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("frac", 1); } },
        { "stacked-fractions",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("afrc", 1); } },
    };
    auto iter = operators.find(fontVariant);
    if (iter != operators.end()) {
        iter->second(fontFeatures);
        return true;
    }
    return false;
}

bool ParseFontVariantAlternates(const std::string& fontVariant, std::unordered_map<std::string, int32_t>& fontFeatures)
{
    // format of font-variant-alternates is key(value) | normal | historical-forms.
    if (fontVariant == FONT_FEATURE_NORMAL || fontVariant.empty()) {
        return true;
    }
    if (fontVariant != "historical-forms" && !regex_match(fontVariant, std::regex(".+(.+)"))) {
        return false;
    }

    std::string key = fontVariant;
    int32_t value = 1;
    auto valueIndex = fontVariant.find_first_of("(");
    if (valueIndex != std::string::npos && fontVariant.size() > valueIndex) {
        key = fontVariant.substr(0, valueIndex);
        value = StringUtils::StringToInt(
            fontVariant.substr(valueIndex + 1, fontVariant.size() - key.size() - FONT_FEATURE_PARENTHESES_LENGTH));
    }
    const std::unordered_map<std::string,
        void (*)(const std::string&, int32_t, std::unordered_map<std::string, int32_t>&)>
        operators = {
            { "historical-forms",
                [](const std::string& key, int32_t value, std::unordered_map<std::string, int32_t>& fontFeatures) {
                    fontFeatures.try_emplace("hist", 1);
                } },
            { "stylistic",
                [](const std::string& key, int32_t value, std::unordered_map<std::string, int32_t>& fontFeatures) {
                    fontFeatures.try_emplace("salt", value);
                } },
            { "styleset",
                [](const std::string& key, int32_t value, std::unordered_map<std::string, int32_t>& fontFeatures) {
                    // Convert value to ssxx, like 1 to ss01.
                    std::string temp = "0" + std::to_string(value);
                    fontFeatures.try_emplace("ss" + temp.substr(temp.size() - 2, 2), 1);
                } },
            { "character-variant",
                [](const std::string& key, int32_t value, std::unordered_map<std::string, int32_t>& fontFeatures) {
                    // Convert value to cvxx, like 1 to cv01.
                    std::string temp = "0" + std::to_string(value);
                    fontFeatures.try_emplace("cv" + temp.substr(temp.size() - 2, 2), 1);
                } },
            { "swash",
                [](const std::string& key, int32_t value, std::unordered_map<std::string, int32_t>& fontFeatures) {
                    fontFeatures.try_emplace("swsh", value);
                    fontFeatures.try_emplace("cswh", value);
                } },
            { "ornaments",
                [](const std::string& key, int32_t value, std::unordered_map<std::string, int32_t>& fontFeatures) {
                    fontFeatures.try_emplace("ornm", value);
                } },
            { "annotation",
                [](const std::string& key, int32_t value, std::unordered_map<std::string, int32_t>& fontFeatures) {
                    fontFeatures.try_emplace("nalt", value);
                } },
        };
    auto iter = operators.find(key);
    if (iter != operators.end()) {
        iter->second(key, value, fontFeatures);
        return true;
    }
    return false;
}

bool ParseFontVariantLigatures(const std::string& fontVariant, std::unordered_map<std::string, int32_t>& fontFeatures)
{
    if (fontVariant == FONT_FEATURE_NORMAL || fontVariant.empty()) {
        return true;
    }

    const std::unordered_map<std::string, void (*)(std::unordered_map<std::string, int32_t>&)> operators = {
        { "none",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) {
                fontFeatures.try_emplace("liga ", 0);
                fontFeatures.try_emplace("clig ", 0);
                fontFeatures.try_emplace("dlig ", 0);
                fontFeatures.try_emplace("hlig ", 0);
                fontFeatures.try_emplace("calt ", 0);
            } },
        { "common-ligatures",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) {
                fontFeatures.try_emplace("liga ", 1);
                fontFeatures.try_emplace("clig ", 1);
            } },
        { "no-common-ligatures",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) {
                fontFeatures.try_emplace("liga", 0);
                fontFeatures.try_emplace("clig", 0);
            } },
        { "discretionary-ligatures",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("dlig", 1); } },
        { "no-discretionary-ligatures",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("dlig", 0); } },
        { "historical-ligatures",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("hlig", 1); } },
        { "no-historical-ligatures",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("hlig", 0); } },
        { "contextual",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("calt", 1); } },
        { "no-contextual",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("calt", 0); } },
    };
    auto iter = operators.find(fontVariant);
    if (iter != operators.end()) {
        iter->second(fontFeatures);
        return true;
    }
    return false;
}

bool ParseFontVariantEastAsian(const std::string& fontVariant, std::unordered_map<std::string, int32_t>& fontFeatures)
{
    if (fontVariant == FONT_FEATURE_NORMAL || fontVariant.empty()) {
        return true;
    }

    const std::unordered_map<std::string, void (*)(std::unordered_map<std::string, int32_t>&)> operators = {
        { "ruby", [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("ruby", 1); } },
        { "jis78",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("jp78", 1); } },
        { "jis83",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("jp83", 1); } },
        { "jis90",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("jp90", 1); } },
        { "jis04",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("jp04", 1); } },
        { "simplified",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("smpl", 1); } },
        { "traditional",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("trad", 1); } },
        { "proportional-width",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("pwid", 1); } },
        { "full-width",
            [](std::unordered_map<std::string, int32_t>& fontFeatures) { fontFeatures.try_emplace("fwid", 1); } },
    };
    auto iter = operators.find(fontVariant);
    if (iter != operators.end()) {
        iter->second(fontFeatures);
        return true;
    }
    return false;
}

void ParseFontVariant(const std::string& fontVariant, std::unordered_map<std::string, int32_t>& fontFeatures)
{
    if (fontVariant.empty()) {
        return;
    }
    auto tempFontVariant = fontVariant;
    StringUtils::TrimStrLeadingAndTrailing(tempFontVariant);
    if (ParseFontVariantCaps(tempFontVariant, fontFeatures)) {
        return;
    }
    if (ParseFontVariantNumeric(tempFontVariant, fontFeatures)) {
        return;
    }
    if (ParseFontVariantAlternates(tempFontVariant, fontFeatures)) {
        return;
    }
    if (ParseFontVariantLigatures(tempFontVariant, fontFeatures)) {
        return;
    }
    if (ParseFontVariantEastAsian(tempFontVariant, fontFeatures)) {
        return;
    }
}

/*
 * Format of font-feature-settings:
 * normal | none | <string>
 * number of <string> can be single or multiple, and separated by space ' '
 */
std::unordered_map<std::string, int32_t> ParseFontVariants(const std::string& fontVariants)
{
    std::unordered_map<std::string, int32_t> fontFeatures;
    if (fontVariants == FONT_FEATURE_NORMAL || fontVariants.empty()) {
        return fontFeatures;
    }

    if (fontVariants == FONT_FEATURE_NONE) {
        fontFeatures.try_emplace("liga ", 0);
        fontFeatures.try_emplace("clig ", 0);
        fontFeatures.try_emplace("dlig ", 0);
        fontFeatures.try_emplace("hlig ", 0);
        fontFeatures.try_emplace("calt ", 0);
        return fontFeatures;
    }

    std::stringstream stream(fontVariants);
    std::string fontVariant;
    while (getline(stream, fontVariant, ' ')) {
        ParseFontVariant(fontVariant, fontFeatures);
    }
    return fontFeatures;
}

void ParseFontFeatureSetting(
    const std::string& fontFeatureSetting, std::unordered_map<std::string, int32_t>& fontFeatures)
{
    if (fontFeatureSetting.empty()) {
        LOGW("ParseFontFeatureSetting fontFeatureSetting is empty");
        return;
    }

    auto temp = fontFeatureSetting;
    StringUtils::TrimStrLeadingAndTrailing(temp);
    if (temp.empty()) {
        LOGW("ParseFontFeatureSetting fontFeatureSetting is empty");
        return;
    }

    std::vector<std::string> value;
    StringUtils::StringSpliter(temp, ' ', value);
    if (value.empty() || value.size() > FONT_FEATURE_MAX_SIZE || value[0].size() != FONT_FEATURE_KEY_LENGTH) {
        LOGW("ParseFontFeatureSetting param is invalid");
        return;
    }

    switch (value.size()) {
        case 1:
            fontFeatures.try_emplace(value[0], 1);
            break;
        case 2:
            fontFeatures.try_emplace(value[0],
                value[1] == FONT_FEATURE_ON ? 1
                                            : (value[1] == FONT_FEATURE_OFF ? 0 : StringUtils::StringToInt(value[1])));
            break;
        default:
            LOGW("ParseFontFeatureSetting format of font-feature-settings is invalid");
            break;
    }
}

/*
 * Format of font-feature-settings:
 * normal | <feature-tag-value>, where <feature-tag-value> = <string> [ <integer> | on | off ], like: "liga" 0
 * number of <feature-tag-value> can be single or multiple, and separated by comma ','
 */
std::unordered_map<std::string, int32_t> ParseFontFeatureSettings(const std::string& fontFeatureSettings)
{
    std::unordered_map<std::string, int32_t> fontFeatures;
    if (fontFeatureSettings == FONT_FEATURE_NORMAL || fontFeatureSettings.empty()) {
        return fontFeatures;
    }

    std::stringstream stream(fontFeatureSettings);
    std::string fontFeatureSetting;
    while (getline(stream, fontFeatureSetting, ',')) {
        ParseFontFeatureSetting(fontFeatureSetting, fontFeatures);
    }
    return fontFeatures;
}

} // namespace OHOS::Ace