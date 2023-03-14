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

#ifndef FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_CORE_COMPONENTS_COMMON_PROPERTIES_TEXT_STYLE_PARSER_H
#define FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_CORE_COMPONENTS_COMMON_PROPERTIES_TEXT_STYLE_PARSER_H

namespace OHOS::Ace {

bool ParseFontVariantCaps(const std::string& fontVariant, std::map<std::string, int32_t>& fontFeatures);
bool ParseFontVariantNumeric(const std::string& fontVariant, std::map<std::string, int32_t>& fontFeatures);
bool ParseFontVariantAlternates(const std::string& fontVariant, std::map<std::string, int32_t>& fontFeatures);
bool ParseFontVariantLigatures(const std::string& fontVariant, std::map<std::string, int32_t>& fontFeatures);
bool ParseFontVariantEastAsian(const std::string& fontVariant, std::map<std::string, int32_t>& fontFeatures);
void ParseFontVariant(const std::string& fontVariant, std::map<std::string, int32_t>& fontFeatures);
std::unordered_map<std::string, int32_t> ParseFontVariants(const std::string& fontVariants);

void ParseFontFeatureSetting(const std::string& fontFeatureSetting, std::map<std::string, int32_t>& fontFeatures);
std::unordered_map<std::string, int32_t> ParseFontFeatureSettings(const std::string& fontFeatureSettings);

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_CORE_COMPONENTS_COMMON_PROPERTIES_TEXT_STYLE_PARSER_H
