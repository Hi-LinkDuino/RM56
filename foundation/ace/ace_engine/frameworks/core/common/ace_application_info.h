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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_ACE_APPLICATION_INFO_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_ACE_APPLICATION_INFO_H

#include <chrono>
#include <set>
#include <string>
#include <vector>

#include "base/json/json_util.h"
#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"
#include "base/utils/string_utils.h"

namespace OHOS::Ace {

struct AceBundleInfo {
    uint32_t versionCode = 0;
    std::string versionName;
};

class ACE_FORCE_EXPORT_WITH_PREVIEW AceApplicationInfo : public NonCopyable {
public:
    ACE_EXPORT static AceApplicationInfo& GetInstance();

    virtual void SetLocale(const std::string& language, const std::string& countryOrRegion, const std::string& script,
        const std::string& keywordsAndValues) = 0;
    virtual void ChangeLocale(const std::string& language, const std::string& countryOrRegion) = 0;
    virtual void SetDebug(bool isDebugVersion, bool needDebugBreakpoint) = 0;

    void SetUserId(int userId)
    {
        userId_ = userId;
    }

    int GetUserId() const
    {
        return userId_;
    }

    void SetPackageName(const std::string& packageName);
    const std::string& GetPackageName() const;

    void SetUid(int32_t uid);
    int32_t GetUid() const;

    void SetProcessName(const std::string& processName);
    const std::string& GetProcessName() const;


    void SetDataFileDirPath(const std::string& dataDirFilePath)
    {
        dataDirFilePath_ = dataDirFilePath;
    }
    const std::string& GetDataFileDirPath() const
    {
        return dataDirFilePath_;
    }

    virtual bool GetBundleInfo(const std::string& packageName, AceBundleInfo& bundleInfo) = 0;
    virtual double GetLifeTime() const = 0;

    virtual std::string GetJsEngineParam(const std::string& key) const = 0;

    const std::string& GetCountryOrRegion() const
    {
        return countryOrRegion_;
    }

    const std::string& GetLanguage() const
    {
        return language_;
    }

    const std::string& GetScript() const
    {
        return script_;
    }

    const std::string& GetLocaleTag() const
    {
        return localeTag_;
    }

    std::string GetUnicodeSetting() const;

    bool IsRightToLeft() const
    {
        return isRightToLeft_;
    }

    bool IsDebugVersion() const
    {
        return isDebugVersion_;
    }

    bool IsNeedDebugBreakPoint() const
    {
        return needDebugBreakpoint_;
    }

    void SetNeedDebugBreakPoint(const bool needDebugBreakpoint)
    {
        needDebugBreakpoint_ = needDebugBreakpoint;
    }

    void SetCardType()
    {
        isCardType_ = true;
    }

    bool GetIsCardType() const
    {
        return isCardType_;
    }

    void SetAccessibilityEnabled(bool isEnabled)
    {
        isAccessibilityEnabled_ = isEnabled;
    }
    bool IsAccessibilityEnabled() const
    {
        return isAccessibilityEnabled_;
    }

protected:
    std::string countryOrRegion_;
    std::string language_;
    std::string script_;
    std::string localeTag_;
    std::string keywordsAndValues_;

    std::string packageName_;
    std::string processName_;
    std::string dataDirFilePath_;
    int32_t uid_;

    bool isRightToLeft_ = false;
    bool isDebugVersion_ = false;
    bool needDebugBreakpoint_ = false;

    bool isCardType_ = false;

    int userId_ = 0;
    bool isAccessibilityEnabled_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_ACE_APPLICATION_INFO_H
