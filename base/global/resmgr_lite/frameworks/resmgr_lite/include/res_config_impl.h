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
#ifndef OHOS_RESOURCE_MANAGER_RESCONFIG_IMPL_H
#define OHOS_RESOURCE_MANAGER_RESCONFIG_IMPL_H

#include <stdint.h>
#include "locale_info.h"
#include "res_locale.h"
#include "res_common.h"
#include "res_config.h"

using OHOS::I18N::LocaleInfo;
namespace OHOS {
namespace Global {
namespace Resource {
class ResConfigImpl : public ResConfig {
public:
    ResConfigImpl();

    /**
     * Whether this resConfig more match request resConfig
     * @param other the other resConfig
     * @param request  the request resConfig
     * @return true if this resConfig more match request resConfig than other resConfig, else false
     */
    bool IsMoreSuitable(const ResConfigImpl *other, const ResConfigImpl *request) const;

    /**
     * Set locale information
     * @param language the locale language
     * @param script  the locale script
     * @param region  the locale region
     * @return SUCCESS if set locale information success, else false
     */
    RState SetLocaleInfo(const char *language, const char *script, const char *region);
    
    /**
     * Set locale information
     * @param localeInfo the localeInfo
     * @return SUCCESS if set locale information success, else false
     */
    RState SetLocaleInfo(LocaleInfo &localeInfo);

    /**
     * Set resConfig device type
     * @param deviceType the device type
     */
    void SetDeviceType(DeviceType deviceType);

    /**
     * Set resConfig direction
     * @param direction the resConfig direction
     */
    void SetDirection(Direction direction);

    /**
     * Set resConfig screenDensity
     * @param screenDensity the resConfig screenDensity
     */
    void SetScreenDensity(ScreenDensity screenDensity);

    const LocaleInfo *GetLocaleInfo() const;

    const ResLocale *GetResLocale() const;

    Direction GetDirection() const;

    ScreenDensity GetScreenDensity() const;

    DeviceType GetDeviceType() const;

    /**
     * Whether this resConfig match other resConfig
     * @param other the other resConfig
     * @return true if this resConfig match other resConfig, else false
     */
    bool Match(const ResConfigImpl *other) const;

    /**
     * Copy other resConfig to this resConfig
     * @param other the other resConfig
     * @return true if copy other resConfig to this resConfig success, else false
     */
    bool Copy(ResConfig &other);

    /**
     * Complete the local script
     */
    void CompleteScript();

    /**
     * Whether this resLocal script completed
     * @return true if resLocal script completed, else false
     */
    bool IsCompletedScript() const;

    virtual ~ResConfigImpl();

private:
    bool IsMoreSpecificThan(const ResConfigImpl *other) const;

    bool CopyLocale(ResConfig &other);

private:
    ResLocale *resLocale_;
    Direction direction_;
    ScreenDensity screenDensity_;
    DeviceType deviceType_;
    bool isCompletedScript_;
    LocaleInfo *localeInfo_;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif