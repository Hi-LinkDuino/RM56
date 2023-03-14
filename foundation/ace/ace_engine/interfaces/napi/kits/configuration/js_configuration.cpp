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

#include <string>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "frameworks/core/common/ace_application_info.h"

namespace OHOS::Ace::Napi {
namespace {

const char LOCALE_TEXT_DIR_LTR[] = "ltr";
const char LOCALE_TEXT_DIR_RTL[] = "rtl";

} // namespace

static napi_value JSConfigurationGetLocale(napi_env env, napi_callback_info info)
{
    std::string language = OHOS::Ace::AceApplicationInfo::GetInstance().GetLanguage();
    std::string countryOrRegion = OHOS::Ace::AceApplicationInfo::GetInstance().GetCountryOrRegion();
    std::string dir =
        OHOS::Ace::AceApplicationInfo::GetInstance().IsRightToLeft() ? LOCALE_TEXT_DIR_RTL : LOCALE_TEXT_DIR_LTR;
    std::string unicodeSetting = OHOS::Ace::AceApplicationInfo::GetInstance().GetUnicodeSetting();
    size_t languageLen = language.length();
    size_t countryOrRegionLen = countryOrRegion.length();
    size_t dirLen = dir.length();
    size_t unicodeSettingLen = unicodeSetting.length();
    if (languageLen == 0 && countryOrRegionLen == 0 && dirLen == 0 && unicodeSettingLen == 0) {
        return nullptr;
    }

    napi_value resultArray[4] = { 0 };
    napi_create_string_utf8(env, language.c_str(), languageLen, &resultArray[0]);
    napi_create_string_utf8(env, countryOrRegion.c_str(), countryOrRegionLen, &resultArray[1]);
    napi_create_string_utf8(env, dir.c_str(), dirLen, &resultArray[2]);
    napi_create_string_utf8(env, unicodeSetting.c_str(), unicodeSettingLen, &resultArray[3]);

    napi_value result = nullptr;
    napi_create_object(env, &result);
    napi_set_named_property(env, result, "language", resultArray[0]);
    napi_set_named_property(env, result, "countryOrRegion", resultArray[1]);
    napi_set_named_property(env, result, "dir", resultArray[2]);
    napi_set_named_property(env, result, "unicodeSetting", resultArray[3]);

    return result;
}

static napi_value ConfigurationExport(napi_env env, napi_value exports)
{
    static napi_property_descriptor configurationDesc[] = {
        DECLARE_NAPI_FUNCTION("getLocale", JSConfigurationGetLocale),
    };
    NAPI_CALL(env, napi_define_properties(
        env, exports, sizeof(configurationDesc) / sizeof(configurationDesc[0]), configurationDesc));
    return exports;
}

static napi_module configurationModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = ConfigurationExport,
    .nm_modname = "configuration",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void ConfigurationRegister()
{
    napi_module_register(&configurationModule);
}

} // namespace OHOS::Ace::Napi