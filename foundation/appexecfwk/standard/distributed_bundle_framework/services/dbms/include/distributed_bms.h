/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_D_BUNDLEMGR_INCLUDE_DISTRIBUTED_BMS_H
#define FOUNDATION_APPEXECFWK_SERVICES_D_BUNDLEMGR_INCLUDE_DISTRIBUTED_BMS_H

#include <memory>

#include "bundle_info.h"
#include "bundle_mgr_interface.h"
#include "distributed_bms_host.h"
#include "if_system_ability_manager.h"
#include "iremote_object.h"
#include "image_buffer.h"
#include "resource_manager.h"
#include "system_ability.h"

namespace OHOS {
namespace AppExecFwk {
class DistributedBms : public SystemAbility, public DistributedBmsHost {
    DECLARE_SYSTEM_ABILITY(DistributedBms);

public:
    DistributedBms(int32_t saId, bool runOnCreate);
    ~DistributedBms();

    /**
     * @brief get remote ability info
     * @param elementName Indicates the elementName.
     * @param remoteAbilityInfo Indicates the remote ability info.
     * @return Returns true when get remote ability info success; returns false otherwise.
     */
    int32_t GetRemoteAbilityInfo(
        const OHOS::AppExecFwk::ElementName &elementName, RemoteAbilityInfo &remoteAbilityInfo) override;

    /**
     * @brief get remote ability info
     * @param elementName Indicates the elementName.
     * @param localeInfo Indicates the localeInfo.
     * @param remoteAbilityInfo Indicates the remote ability info.
     * @return Returns true when get remote ability info success; returns false otherwise.
     */
    int32_t GetRemoteAbilityInfo(const OHOS::AppExecFwk::ElementName &elementName, const std::string &localeInfo,
        RemoteAbilityInfo &remoteAbilityInfo) override;
    /**
     * @brief get remote ability infos
     * @param elementNames Indicates the elementNames.
     * @param remoteAbilityInfos Indicates the remote ability infos.
     * @return Returns true when get remote ability info success; returns false otherwise.
     */
    int32_t GetRemoteAbilityInfos(
        const std::vector<ElementName> &elementNames, std::vector<RemoteAbilityInfo> &remoteAbilityInfos) override;

    /**
     * @brief get remote ability infos
     * @param elementNames Indicates the elementNames.
     * @param localeInfo Indicates the localeInfo.
     * @param remoteAbilityInfos Indicates the remote ability infos.
     * @return Returns true when get remote ability info success; returns false otherwise.
     */
    int32_t GetRemoteAbilityInfos(const std::vector<ElementName> &elementNames, const std::string &localeInfo,
        std::vector<RemoteAbilityInfo> &remoteAbilityInfos) override;

    /**
     * @brief get ability info
     * @param elementName Indicates the elementName.
     * @param remoteAbilityInfo Indicates the remote ability info.
     * @return Returns true when get remote ability info success; returns false otherwise.
     */
    int32_t GetAbilityInfo(
        const OHOS::AppExecFwk::ElementName &elementName, RemoteAbilityInfo &remoteAbilityInfo) override;

    /**
     * @brief get ability info
     * @param elementName Indicates the elementName.
     * @param localeInfo Indicates the localeInfo.
     * @param remoteAbilityInfo Indicates the remote ability info.
     * @return Returns true when get remote ability info success; returns false otherwise.
     */
    int32_t GetAbilityInfo(const OHOS::AppExecFwk::ElementName &elementName, const std::string &localeInfo,
        RemoteAbilityInfo &remoteAbilityInfo) override;
    /**
     * @brief get ability infos
     * @param elementNames Indicates the elementNames.
     * @param remoteAbilityInfos Indicates the remote ability infos.
     * @return Returns true when get remote ability info success; returns false otherwise.
     */
    int32_t GetAbilityInfos(
        const std::vector<ElementName> &elementNames, std::vector<RemoteAbilityInfo> &remoteAbilityInfos) override;

    /**
     * @brief get ability infos
     * @param elementNames Indicates the elementNames.
     * @param localeInfo Indicates the localeInfo.
     * @param remoteAbilityInfos Indicates the remote ability infos.
     * @return Returns true when get remote ability info success; returns false otherwise.
     */
    int32_t GetAbilityInfos(const std::vector<ElementName> &elementNames, const std::string &localeInfo,
        std::vector<RemoteAbilityInfo> &remoteAbilityInfos) override;

    /**
     * @brief Start the bundle manager service.
     * @return
     */
    virtual void OnStart() override;
    /**
     * @brief Stop the bundle manager service.
     * @return
     */
    virtual void OnStop() override;
private:
    std::shared_ptr<Global::Resource::ResourceManager> GetResourceManager(
        const AppExecFwk::BundleInfo &bundleInfo, const std::string &localeInfo);
    bool GetMediaBase64(std::string &path, std::string &value);
    bool GetMediaBae64FromImageBuffer(std::shared_ptr<ImageBuffer>& imageBuffer, std::string& value);
    std::unique_ptr<unsigned char[]> LoadResourceFile(std::string &path, int &len);
    std::unique_ptr<char[]> EncodeBase64(std::unique_ptr<unsigned char[]> &data, int srcLen);
    bool GetCurrentUserId(int &userId);

    static OHOS::sptr<OHOS::AppExecFwk::IBundleMgr> bundleMgr_;
    static std::mutex bundleMgrMutex_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_D_BUNDLEMGR_INCLUDE_DISTRIBUTED_BMS_H