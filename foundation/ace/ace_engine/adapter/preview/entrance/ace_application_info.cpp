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

#include "adapter/preview/entrance/ace_application_info.h"

#include "unicode/locid.h"

#ifdef WINDOWS_PLATFORM
#include <windows.h>
#else
#include <dirent.h>
#include <sys/types.h>
#endif

#include "base/i18n/localization.h"
#include "base/log/ace_trace.h"
#include "base/log/log.h"
#include "base/resource/ace_res_config.h"
#include "base/resource/ace_res_data_struct.h"
#include "core/common/ace_engine.h"

namespace OHOS::Ace::Platform {

void AceApplicationInfoImpl::ChangeLocale(const std::string& language, const std::string& countryOrRegion) 
{
    std::string languageLower;
    std::transform(language.begin(), language.end(), languageLower.begin(), ::tolower);

    std::string countryOrRegionUpper;
    std::transform(countryOrRegion.begin(), countryOrRegion.end(), countryOrRegionUpper.begin(), ::tolower);

    std::unique_ptr<Global::Resource::ResConfig> resConfig(Global::Resource::CreateResConfig());
    resourceManager_->GetResConfig(*resConfig);

    auto localeInfo = resConfig->GetLocaleInfo();
    if (localeInfo == nullptr) {
        LOGE("get local info failed");
        return;
    }

    auto script = Localization::ComputeScript(language, countryOrRegion);
    resConfig->SetLocaleInfo(languageLower.c_str(), script.c_str(), countryOrRegionUpper.c_str());
    resourceManager_->UpdateResConfig(*resConfig);

    SetLocale(languageLower, countryOrRegionUpper, (script.empty()) ? "" : script, "");
}

void AceApplicationInfoImpl::SetLocale(const std::string& language, const std::string& countryOrRegion,
    const std::string& script, const std::string& keywordsAndValues)
{
    language_ = language;
    countryOrRegion_ = countryOrRegion;
    script_ = script;
    keywordsAndValues_ = keywordsAndValues;

    localeTag_ = language;
    if (!script_.empty()) {
        localeTag_.append("-" + script_);
    }
    if (!countryOrRegion_.empty()) {
        localeTag_.append("-" + countryOrRegion_);
    }

    icu::Locale locale(language_.c_str(), countryOrRegion.c_str());
    isRightToLeft_ = locale.isRightToLeft();

    auto languageList = Localization::GetLanguageList(language_);
    Localization::SetLocale(
        language_, countryOrRegion_, script, languageList.front(), keywordsAndValues_);
}

bool AceApplicationInfoImpl::GetBundleInfo(const std::string& packageName, AceBundleInfo& bundleInfo)
{
    return false;
}

double AceApplicationInfoImpl::GetLifeTime() const
{
    return 0;
}

std::string AceApplicationInfoImpl::GetJsEngineParam(const std::string& key) const
{
    return "";
}

AceApplicationInfoImpl& AceApplicationInfoImpl::GetInstance()
{
    static AceApplicationInfoImpl instance;
    return instance;
}

void AceApplicationInfoImpl::SetDebug(bool isDebugVersion, bool needDebugBreakpoint)
{
}

} // namespace OHOS::Ace::Platform

namespace OHOS::Ace {

AceApplicationInfo& AceApplicationInfo::GetInstance()
{
    return Platform::AceApplicationInfoImpl::GetInstance();
}

} // namespace OHOS::Ace
