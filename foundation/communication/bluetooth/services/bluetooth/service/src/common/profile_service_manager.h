/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines profile services manager for bluetooth system.
 *
 * @since 6
 */

/**
 * @file profile_service_manager.h
 *
 * @brief system state machine interface.
 *
 * @since 6
 */

#ifndef PROFILE_SERVICE_MANAGER_H
#define PROFILE_SERVICE_MANAGER_H

#include <map>
#include <vector>

#include "interface_profile.h"
#include "interface_profile_manager.h"
#include "util/dispatcher.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Represents profile service manager.
 *
 * @since 6
 */
class ProfileServiceManager : public IProfileManager {
public:
    /**
     * @brief Get profile service manager singleton instance pointer.
     *
     * @return Returns the singleton instance pointer.
     * @since 6
     */
    static ProfileServiceManager *GetInstance();

    /**
     * @brief Initialize profile service manager.
     *
     * @param dispatch The dispatch used in profile service manager.
     * @since 6
     */
    static void Initialize(utility::Dispatcher &dispatch);

    /**
     * @brief Uninitialize profile service manager.
     *
     * @since 6
     */
    static void Uninitialize();

    // framework function
    /**
     * @brief Get profile service pointer.
     *
     * @param name Profile service name.
     * @return Returns the profile service pointer.
     * @since 6
     */
    IProfile *GetProfileService(const std::string &name) const override;

    /**
     * @brief Create profile services according to config.xml.
     *
     * @since 6
     */
    void Start() const;

    /**
     * @brief Delete all profile services when Start() create.
     *
     * @since 6
     */
    void Stop() const;

    /**
     * @brief Enable profile services.
     *
     * @param transport Adapter transport.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool Enable(const BTTransport transport) const;

    /**
     * @brief Set all profile services states as turn on.
     *
     * @param transport Adapter transport.
     * @since 6
     */
    void OnAllEnabled(const BTTransport transport) const;

    /**
     * @brief Disable profile services.
     *
     * @param transport Adapter transport.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool Disable(const BTTransport transport) const;

    /**
     * @brief Set all profile services states as turn off.
     *
     * @param transport Adapter transport.
     * @since 6
     */
    void OnAllDisabled(const BTTransport transport) const;

    /**
     * @brief Get profile service ID list.
     *
     * @return Returns vector of enabled profile services ID.
     * @since 6
     */
    std::vector<uint32_t> GetProfileServicesList() const override;

    /**
     * @brief Get profile service connect state.
     *
     * @param profileID Profile service ID.
     * @return Returns connect state for designated profile service.
     * @since 6
     */
    BTConnectState GetProfileServiceConnectState(const uint32_t profileID) const override;

    /**
     * @brief Get local device supported uuids.
     *
     * @param[out] Vector which use to return support uuids.
     * @since 6
     */
    void GetProfileServicesSupportedUuids(std::vector<std::string> &uuids) const override;
    /**
     * @brief Get all profile services connect state.
     *
     * @return Returns profile services connect state.
     * @since 6
     */
    BTConnectState GetProfileServicesConnectState() const;

    /**
     * @brief Profile service enable complete notify.
     *
     * @param profileID Profile service ID.
     * @param ret Profile service enable operation result.
     * @since 6
     */
    void OnEnable(const std::string &name, bool ret) const;

    /**
     * @brief Profile service disable complete notify.
     *
     * @param profileID Profile service ID.
     * @param ret Profile service disable operation result.
     * @since 6
     */
    void OnDisable(const std::string &name, bool ret) const;

    /**
     * @brief A constructor used to create an <b>ProfileServiceManager</b> instance.
     *
     * @param dispatch The dispatch used in profile service manager.
     * @since 6
     */
    explicit ProfileServiceManager(utility::Dispatcher &dispatch);

    /**
     * @brief A destructor used to delete the <b>ProfileServiceManager</b> instance.
     *
     * @since 6
     */
    ~ProfileServiceManager();

private:
    ProfileServiceManager() = delete;

    void CreateConfigSupportProfiles() const;
    void CreateClassicProfileServices() const;
    void CreateBleProfileServices() const;

    void EnableProfiles(const BTTransport transport) const;
    void DisableProfiles(const BTTransport transport) const;
    void EnableCompleteProcess(const std::string &name, bool ret) const;
    void DisableCompleteProcess(const std::string &name, bool ret) const;
    void EnableCompleteNotify(const BTTransport transport) const;
    void DisableCompleteNotify(const BTTransport transport) const;
    bool IsAllEnabled(const BTTransport transport) const;
    bool IsProfilesTurning(const BTTransport transport) const;
    bool IsAllDisabled(const BTTransport transport) const;
    void CheckWaitEnableProfiles(const std::string &name, const BTTransport transport) const;

    BT_DISALLOW_COPY_AND_ASSIGN(ProfileServiceManager);
    DECLARE_IMPL();
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // PROFILE_SERVICE_MANAGER_H