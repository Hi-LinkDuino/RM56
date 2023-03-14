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

#include "profile_service_manager.h"

#include <algorithm>

#include "bt_def.h"
#include "log.h"

#include "adapter_config.h"
#include "adapter_manager.h"
#include "class_creator.h"
#include "profile_info.h"
#include "profile_list.h"

namespace OHOS {
namespace bluetooth {
class ProfileServicesContextCallback : public utility::IContextCallback {
public:
    explicit ProfileServicesContextCallback(ProfileServiceManager &psm) : psm_(psm){};
    ~ProfileServicesContextCallback() = default;

    void OnEnable(const std::string &name, bool ret)
    {
        LOG_DEBUG("%{public}s, name=%{public}s, ret=%{public}d\n", __PRETTY_FUNCTION__, name.c_str(), ret);
        psm_.OnEnable(name, ret);
    }

    void OnDisable(const std::string &name, bool ret)
    {
        LOG_DEBUG("%{public}s, name=%{public}s, ret=%{public}d\n", __PRETTY_FUNCTION__, name.c_str(), ret);
        psm_.OnDisable(name, ret);
    }

private:
    ProfileServiceManager &psm_;
};

// static function
static std::unique_ptr<ProfileServiceManager> g_profileManager = nullptr;
IProfileManager *IProfileManager::GetInstance()
{
    return ProfileServiceManager::GetInstance();
}

ProfileServiceManager *ProfileServiceManager::GetInstance()
{
    if (g_profileManager != nullptr) {
        return g_profileManager.get();
    }
    return nullptr;
}

void ProfileServiceManager::Initialize(utility::Dispatcher &dispatch)
{
    if (g_profileManager == nullptr) {
        g_profileManager = std::make_unique<ProfileServiceManager>(dispatch);
    }
    g_profileManager->Start();
}

void ProfileServiceManager::Uninitialize()
{
    if (g_profileManager != nullptr) {
        g_profileManager->Stop();
    }
}

// ProfileServiceManager
enum ServiceStateID {
    TURNING_ON = BTStateID::STATE_TURNING_ON,
    TURN_ON = BTStateID::STATE_TURN_ON,
    TURNING_OFF = BTStateID::STATE_TURNING_OFF,
    TURN_OFF = BTStateID::STATE_TURN_OFF,
    WAIT_TURN_ON,
};

struct ProfileServiceManager::impl {
    explicit impl(utility::Dispatcher &dispatch) : dispatcher_(dispatch)
    {}
    utility::Dispatcher &dispatcher_;
    ProfilesList<IProfile *> startedProfiles_ = {};
    ProfilesList<ServiceStateID> profilesState_ = {};
    std::unique_ptr<ProfileServicesContextCallback> contextCallback_ = nullptr;

    BT_DISALLOW_COPY_AND_ASSIGN(impl);
};

ProfileServiceManager::ProfileServiceManager(utility::Dispatcher &dispatch)
    : pimpl(std::make_unique<ProfileServiceManager::impl>(dispatch))
{
    // context callback create
    pimpl->contextCallback_ = std::make_unique<ProfileServicesContextCallback>(*this);
}

ProfileServiceManager::~ProfileServiceManager()
{}

void ProfileServiceManager::Start() const
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);

    if (AdapterManager::GetInstance()->GetAdapter(BTTransport::ADAPTER_BREDR)) {
        CreateClassicProfileServices();
    }

    if (AdapterManager::GetInstance()->GetAdapter(BTTransport::ADAPTER_BLE)) {
        CreateBleProfileServices();
    }
}

void ProfileServiceManager::CreateClassicProfileServices() const
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);

    for (auto &sp : GET_CONFIG_PROFILES(BTTransport::ADAPTER_BREDR)) {
        IProfile *profile = ClassCreator<IProfile>::NewInstance(sp.name_);

        if (profile != nullptr) {
            LOG_DEBUG("%{public}s %{public}s", __PRETTY_FUNCTION__, sp.name_.c_str());
            profile->GetContext()->Initialize();
            profile->GetContext()->RegisterCallback(*(pimpl->contextCallback_));
            pimpl->startedProfiles_.SetProfile(BTTransport::ADAPTER_BREDR, sp.name_, profile);
            pimpl->profilesState_.SetProfile(BTTransport::ADAPTER_BREDR, sp.name_, ServiceStateID::TURN_OFF);
        } else {
            LOG_ERROR("%{public}s is not registered!!!", sp.name_.c_str());
        }
    }
}

void ProfileServiceManager::CreateBleProfileServices() const
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);

    for (auto &sp : GET_CONFIG_PROFILES(BTTransport::ADAPTER_BLE)) {
        // BREDR started the profile
        IProfile *bredrProfile = nullptr;
        if (pimpl->startedProfiles_.Find(BTTransport::ADAPTER_BREDR, sp.name_, bredrProfile)) {
            // BREDR create the profile
            LOG_DEBUG("%{public}s classic %{public}s", __PRETTY_FUNCTION__, sp.name_.c_str());
            pimpl->startedProfiles_.SetProfile(BTTransport::ADAPTER_BLE, sp.name_, bredrProfile);
            // init profilesState_
            pimpl->profilesState_.SetProfile(BTTransport::ADAPTER_BLE, sp.name_, ServiceStateID::TURN_OFF);
        } else {
            // the profile is not create
            IProfile *newProfile = ClassCreator<IProfile>::NewInstance(sp.name_);
            if (newProfile != nullptr) {
                LOG_DEBUG("%{public}s %{public}s", __PRETTY_FUNCTION__, sp.name_.c_str());
                newProfile->GetContext()->Initialize();
                newProfile->GetContext()->RegisterCallback(*(pimpl->contextCallback_));
                pimpl->startedProfiles_.SetProfile(BTTransport::ADAPTER_BLE, sp.name_, newProfile);
                pimpl->profilesState_.SetProfile(BTTransport::ADAPTER_BLE, sp.name_, ServiceStateID::TURN_OFF);
            } else {
                LOG_ERROR("%{public}s is not registered!!!", sp.name_.c_str());
            }
        }
    }
}

void ProfileServiceManager::Stop() const
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);

    for (auto &sp : GET_SUPPORT_PROFILES()) {
        IProfile *profile = nullptr;
        if (pimpl->startedProfiles_.Find(BTTransport::ADAPTER_BREDR, sp.name_, profile)) {
            profile->GetContext()->Uninitialize();
            pimpl->startedProfiles_.SetProfile(BTTransport::ADAPTER_BREDR, sp.name_, nullptr);
            delete profile;
        } else if (pimpl->startedProfiles_.Find(BTTransport::ADAPTER_BLE, sp.name_, profile)) {
            profile->GetContext()->Uninitialize();
            pimpl->startedProfiles_.SetProfile(BTTransport::ADAPTER_BLE, sp.name_, nullptr);
            delete profile;
        } else {
            // Nothing to do
        }
    }

    pimpl->startedProfiles_.Clear();
    pimpl->profilesState_.Clear();
}

IProfile *ProfileServiceManager::GetProfileService(const std::string &name) const
{
    IProfile *profile = nullptr;

    if (pimpl->startedProfiles_.Find(name, profile)) {
        return profile;
    } else {
        return nullptr;
    }
}

bool ProfileServiceManager::Enable(const BTTransport transport) const
{
    LOG_DEBUG("%{public}s transport is %{public}d", __PRETTY_FUNCTION__, transport);

    if (IsAllEnabled(transport)) {
        LOG_DEBUG("%{public}s OK", __PRETTY_FUNCTION__);
        AdapterManager::GetInstance()->OnProfileServicesEnableComplete(transport, true);
    } else {
        EnableProfiles(transport);
        if (IsAllEnabled(transport)) {
            LOG_DEBUG("%{public}s OK", __PRETTY_FUNCTION__);
            AdapterManager::GetInstance()->OnProfileServicesEnableComplete(transport, true);
        }
    }
    return true;
}

void ProfileServiceManager::OnAllEnabled(const BTTransport transport) const
{
    LOG_DEBUG("%{public}s %{public}d", __PRETTY_FUNCTION__, transport);

    if (pimpl->startedProfiles_.IsEmpty(transport)) {
        LOG_DEBUG("%{public}s empty %{public}d", __PRETTY_FUNCTION__, transport);
        return;
    }

    FOR_EACH_LIST(it, pimpl->profilesState_, transport)
    {
        pimpl->profilesState_.SetProfile(transport, it.first, ServiceStateID::TURN_ON);
    }
}

bool ProfileServiceManager::IsAllEnabled(const BTTransport transport) const
{
    if (pimpl->startedProfiles_.IsEmpty(transport)) {
        LOG_DEBUG("%{public}s empty %{public}d", __PRETTY_FUNCTION__, transport);
        return true;
    }

    const bool any = std::any_of(pimpl->profilesState_.GetProfiles(transport)->begin(),
        pimpl->profilesState_.GetProfiles(transport)->end(),
        [](const auto &temp) -> bool { return temp.second != ServiceStateID::TURN_ON; });

    if (any) {
        LOG_DEBUG("%{public}s false %{public}d", __PRETTY_FUNCTION__, transport);
        return false;
    }
    LOG_DEBUG("%{public}s true %{public}d", __PRETTY_FUNCTION__, transport);
    return true;
}

void ProfileServiceManager::EnableProfiles(const BTTransport transport) const
{
    FOR_EACH_LIST(it, pimpl->profilesState_, transport)
    {
        std::string name = it.first;
        BTTransport otherTransport =
            (transport == BTTransport::ADAPTER_BREDR) ? BTTransport::ADAPTER_BLE : BTTransport::ADAPTER_BREDR;
        ServiceStateID otherTransportState = ServiceStateID::TURN_OFF;

        if (pimpl->profilesState_.Find(otherTransport, name, otherTransportState)) {
            switch (otherTransportState) {
                case ServiceStateID::TURN_ON:
                    LOG_DEBUG("%{public}s TURN_ON otherTransport %{public}d %{public}s", __PRETTY_FUNCTION__, otherTransport, name.c_str());
                    pimpl->profilesState_.SetProfile(transport, name, ServiceStateID::TURN_ON);
                    break;
                case ServiceStateID::TURNING_OFF:
                    LOG_DEBUG("%{public}s TURNING_OFF otherTransport %{public}d %{public}s", __PRETTY_FUNCTION__, otherTransport, name.c_str());
                    pimpl->profilesState_.SetProfile(transport, name, ServiceStateID::WAIT_TURN_ON);
                    break;
                case ServiceStateID::TURNING_ON:
                    LOG_DEBUG("%{public}s TURNING_ON otherTransport %{public}d %{public}s", __PRETTY_FUNCTION__, otherTransport, name.c_str());
                    pimpl->profilesState_.SetProfile(transport, name, ServiceStateID::TURNING_ON);
                    break;
                default:
                    break;
            }
        }

        if (pimpl->profilesState_.Get(transport, name) == ServiceStateID::TURN_OFF) {
            pimpl->profilesState_.SetProfile(transport, name, ServiceStateID::TURNING_ON);
            LOG_DEBUG("%{public}s transport %{public}d %{public}s enable", __PRETTY_FUNCTION__, transport, name.c_str());
            IProfile *profile = nullptr;
            if (pimpl->startedProfiles_.Find(transport, name, profile)) {
                profile->GetContext()->Enable();
            } else {
                LOG_DEBUG("%{public}s startedProfiles_ is not find", __PRETTY_FUNCTION__);
            }
        }
    }
}

void ProfileServiceManager::OnEnable(const std::string &name, bool ret) const
{
    LOG_DEBUG("%{public}s, name=%{public}s, ret=%{public}d\n", __PRETTY_FUNCTION__, name.c_str(), ret);
    pimpl->dispatcher_.PostTask(std::bind(&ProfileServiceManager::EnableCompleteProcess, this, name, ret));
}
void ProfileServiceManager::EnableCompleteProcess(const std::string &name, bool ret) const
{
    ServiceStateID newState = ret ? ServiceStateID::TURN_ON : ServiceStateID::TURN_OFF;
    std::string profileName = name;

    ServiceStateID state = ServiceStateID::TURN_OFF;
    if ((pimpl->profilesState_.Find(BTTransport::ADAPTER_BREDR, profileName, state)) &&
        (state == ServiceStateID::TURNING_ON)) {
        LOG_DEBUG("%{public}s BREDR %{public}s complete ret %{public}d", __PRETTY_FUNCTION__, profileName.c_str(), ret);
        pimpl->profilesState_.SetProfile(BTTransport::ADAPTER_BREDR, profileName, newState);
        if (!IsProfilesTurning(BTTransport::ADAPTER_BREDR)) {
            EnableCompleteNotify(BTTransport::ADAPTER_BREDR);
        }
    }

    if ((pimpl->profilesState_.Find(BTTransport::ADAPTER_BLE, profileName, state)) &&
        (state == ServiceStateID::TURNING_ON)) {
        LOG_DEBUG("%{public}s BLE %{public}s complete ret %{public}d", __PRETTY_FUNCTION__, profileName.c_str(), ret);
        pimpl->profilesState_.SetProfile(BTTransport::ADAPTER_BLE, profileName, newState);
        if (!IsProfilesTurning(BTTransport::ADAPTER_BLE)) {
            EnableCompleteNotify(BTTransport::ADAPTER_BLE);
        }
    }
}

bool ProfileServiceManager::IsProfilesTurning(const BTTransport transport) const
{
    if (pimpl->startedProfiles_.IsEmpty(transport)) {
        LOG_DEBUG("%{public}s empty %{public}d", __PRETTY_FUNCTION__, transport);
        return false;
    }
    const bool any = std::any_of(pimpl->profilesState_.GetProfiles(transport)->begin(),
        pimpl->profilesState_.GetProfiles(transport)->end(),
        [](const auto &temp) -> bool {
            return temp.second != ServiceStateID::TURN_ON && temp.second != ServiceStateID::TURN_OFF;
        });
    if (any) {
        LOG_DEBUG("%{public}s true %{public}d", __PRETTY_FUNCTION__, transport);
        return true;
    }
    LOG_DEBUG("%{public}s false %{public}d", __PRETTY_FUNCTION__, transport);
    return false;
}

void ProfileServiceManager::EnableCompleteNotify(const BTTransport transport) const
{
    int turnOnProfileCount = std::count_if(pimpl->profilesState_.GetProfiles(transport)->begin(),
        pimpl->profilesState_.GetProfiles(transport)->end(),
        [](const auto &temp) -> bool { return temp.second == ServiceStateID::TURN_ON; });

    if (turnOnProfileCount == pimpl->profilesState_.Size(transport)) {
        LOG_DEBUG("%{public}s OK transport %{public}d turnOnProfileCount %{public}d", __PRETTY_FUNCTION__, transport, turnOnProfileCount);
        AdapterManager::GetInstance()->OnProfileServicesEnableComplete(transport, true);
    } else {
        LOG_DEBUG("%{public}s NG transport %{public}d turnOnProfileCount %{public}d", __PRETTY_FUNCTION__, transport, turnOnProfileCount);
        AdapterManager::GetInstance()->OnProfileServicesEnableComplete(transport, false);
    }
}

bool ProfileServiceManager::Disable(const BTTransport transport) const
{
    LOG_DEBUG("%{public}s transport is %{public}d", __PRETTY_FUNCTION__, transport);

    if (IsAllDisabled(transport)) {
        LOG_DEBUG("%{public}s OK", __PRETTY_FUNCTION__);
        AdapterManager::GetInstance()->OnProfileServicesDisableComplete(transport, true);
    } else {
        DisableProfiles(transport);
        if (IsAllDisabled(transport)) {
            LOG_DEBUG("%{public}s OK", __PRETTY_FUNCTION__);
            AdapterManager::GetInstance()->OnProfileServicesDisableComplete(transport, true);
        }
    }

    return true;
}

void ProfileServiceManager::OnAllDisabled(const BTTransport transport) const
{
    LOG_DEBUG("%{public}s %{public}d", __PRETTY_FUNCTION__, transport);

    if (pimpl->startedProfiles_.IsEmpty(transport)) {
        LOG_DEBUG("%{public}s empty %{public}d", __PRETTY_FUNCTION__, transport);
        return;
    }

    FOR_EACH_LIST(it, pimpl->profilesState_, transport)
    {
        pimpl->profilesState_.SetProfile(transport, it.first, ServiceStateID::TURN_OFF);
    }
}

bool ProfileServiceManager::IsAllDisabled(const BTTransport transport) const
{
    if (pimpl->startedProfiles_.IsEmpty(transport)) {
        LOG_DEBUG("%{public}s empty %{public}d", __PRETTY_FUNCTION__, transport);
        return true;
    }

    const bool any = std::any_of(pimpl->profilesState_.GetProfiles(transport)->begin(),
        pimpl->profilesState_.GetProfiles(transport)->end(),
        [](const auto &temp) -> bool { return temp.second != ServiceStateID::TURN_OFF; });

    if (any) {
        LOG_DEBUG("%{public}s false %{public}d", __PRETTY_FUNCTION__, transport);
        return false;
    }
    LOG_DEBUG("%{public}s true %{public}d", __PRETTY_FUNCTION__, transport);
    return true;
}

void ProfileServiceManager::DisableProfiles(const BTTransport transport) const
{
    FOR_EACH_LIST(it, pimpl->profilesState_, transport)
    {
        ServiceStateID otherTransportState = ServiceStateID::TURN_OFF;
        BTTransport otherTransport =
            (transport == BTTransport::ADAPTER_BREDR) ? BTTransport::ADAPTER_BLE : BTTransport::ADAPTER_BREDR;
        std::string name = it.first;

        if (pimpl->profilesState_.Find(otherTransport, name, otherTransportState)) {
            switch (otherTransportState) {
                case ServiceStateID::TURN_ON:
                case ServiceStateID::TURNING_ON:
                    LOG_DEBUG("%{public}s %{public}d otherTransport %{public}d %{public}s",
                        __PRETTY_FUNCTION__,
                        otherTransportState,
                        otherTransport,
                        name.c_str());
                    pimpl->profilesState_.SetProfile(transport, name, ServiceStateID::TURN_OFF);
                    break;
                default:
                    break;
            }
        }

        if (pimpl->profilesState_.Get(transport, name) == ServiceStateID::TURN_ON) {
            pimpl->profilesState_.SetProfile(transport, name, ServiceStateID::TURNING_OFF);
            LOG_DEBUG("%{public}s transport %{public}d %{public}s disable", __PRETTY_FUNCTION__, transport, name.c_str());
            IProfile *profile = nullptr;
            if (pimpl->startedProfiles_.Find(transport, name, profile)) {
                profile->GetContext()->Disable();
            }
        }
    }
}

void ProfileServiceManager::OnDisable(const std::string &name, bool ret) const
{
    LOG_DEBUG("%{public}s, name=%{public}s, ret=%{public}d\n", __PRETTY_FUNCTION__, name.c_str(), ret);
    pimpl->dispatcher_.PostTask(std::bind(&ProfileServiceManager::DisableCompleteProcess, this, name, ret));
}

void ProfileServiceManager::DisableCompleteProcess(const std::string &name, bool ret) const
{
    std::string profileName = name;

    ServiceStateID state = ServiceStateID::TURN_OFF;
    if ((pimpl->profilesState_.Find(BTTransport::ADAPTER_BREDR, profileName, state)) &&
        (state == ServiceStateID::TURNING_OFF)) {
        LOG_DEBUG("%{public}s BREDR %{public}s complete ret %{public}d", __PRETTY_FUNCTION__, profileName.c_str(), ret);
        pimpl->profilesState_.SetProfile(BTTransport::ADAPTER_BREDR, profileName, ServiceStateID::TURN_OFF);
        if (!IsProfilesTurning(BTTransport::ADAPTER_BREDR)) {
            DisableCompleteNotify(BTTransport::ADAPTER_BREDR);
        }
        CheckWaitEnableProfiles(profileName, BTTransport::ADAPTER_BLE);
    }
    if ((pimpl->profilesState_.Find(BTTransport::ADAPTER_BLE, profileName, state)) &&
        (state == ServiceStateID::TURNING_OFF)) {
        LOG_DEBUG("%{public}s BLE %{public}s complete ret %{public}d", __PRETTY_FUNCTION__, profileName.c_str(), ret);
        pimpl->profilesState_.SetProfile(BTTransport::ADAPTER_BLE, profileName, ServiceStateID::TURN_OFF);
        if (!IsProfilesTurning(BTTransport::ADAPTER_BLE)) {
            DisableCompleteNotify(BTTransport::ADAPTER_BLE);
        }
        CheckWaitEnableProfiles(profileName, BTTransport::ADAPTER_BREDR);
    }
}

void ProfileServiceManager::DisableCompleteNotify(const BTTransport transport) const
{
    int turnOffProfileCount = std::count_if(pimpl->profilesState_.GetProfiles(transport)->begin(),
        pimpl->profilesState_.GetProfiles(transport)->end(),
        [](const auto &temp) -> bool { return temp.second == ServiceStateID::TURN_OFF; });

    if (turnOffProfileCount == pimpl->profilesState_.Size(transport)) {
        LOG_DEBUG(
            "%{public}s OK transport %{public}d turnOffProfileCount %{public}d", __PRETTY_FUNCTION__, (int)transport, turnOffProfileCount);
        AdapterManager::GetInstance()->OnProfileServicesDisableComplete(transport, true);
    } else {
        LOG_DEBUG(
            "%{public}s NG transport %{public}d turnOffProfileCount %{public}d", __PRETTY_FUNCTION__, (int)transport, turnOffProfileCount);
        AdapterManager::GetInstance()->OnProfileServicesDisableComplete(transport, false);
    }
}

void ProfileServiceManager::CheckWaitEnableProfiles(const std::string &name, const BTTransport transport) const
{
    ServiceStateID state = ServiceStateID::TURN_OFF;
    if ((pimpl->profilesState_.Find(transport, name, state)) && (state == ServiceStateID::WAIT_TURN_ON)) {
        LOG_DEBUG("%{public}s %{public}s ::WAIT_TURN_ON", __PRETTY_FUNCTION__, name.c_str());
        IProfile *profile = nullptr;
        if (pimpl->startedProfiles_.Find(transport, name, profile)) {
            pimpl->profilesState_.SetProfile(transport, name, ServiceStateID::TURNING_ON);
            profile->GetContext()->Enable();
        }
    }
}

void ProfileServiceManager::GetProfileServicesSupportedUuids(std::vector<std::string> &uuids) const
{
    for (auto &sp : GET_SUPPORT_PROFILES()) {
        ServiceStateID state = ServiceStateID::TURN_OFF;
        if (pimpl->profilesState_.Find(BTTransport::ADAPTER_BREDR, sp.name_, state) &&
            (state == ServiceStateID::TURN_ON) && (sp.uuid_ != "") &&
            (std::find(uuids.begin(), uuids.end(), sp.uuid_) == uuids.end())) {
            uuids.push_back(sp.uuid_);
        }
        if (pimpl->profilesState_.Find(BTTransport::ADAPTER_BLE, sp.name_, state) &&
            (state == ServiceStateID::TURN_ON) && (sp.uuid_ != "") &&
            (std::find(uuids.begin(), uuids.end(), sp.uuid_) == uuids.end())) {
            uuids.push_back(sp.uuid_);
        }
    }
}

std::vector<uint32_t> ProfileServiceManager::GetProfileServicesList() const
{
    std::vector<uint32_t> profileServicesList;
    for (auto &sp : GET_SUPPORT_PROFILES()) {
        if (pimpl->startedProfiles_.Contains(sp.name_)) {
            profileServicesList.push_back(sp.id_);
        }
    }
    return profileServicesList;
}

BTConnectState ProfileServiceManager::GetProfileServiceConnectState(const uint32_t profileID) const
{
    std::string profileName = SupportProfilesInfo::IdToName(profileID);
    IProfile *profile = nullptr;
    if (!pimpl->startedProfiles_.Find(profileName, profile)) {
        return BTConnectState::DISCONNECTED;
    }

    unsigned int profileStateMask = (unsigned int)profile->GetConnectState();
    LOG_DEBUG("%{public}s profileStateMask is %{public}d", __PRETTY_FUNCTION__, profileStateMask);
    if (profileStateMask & PROFILE_STATE_CONNECTED) {
        return BTConnectState::CONNECTED;
    } else if (profileStateMask & PROFILE_STATE_CONNECTING) {
        return BTConnectState::CONNECTING;
    } else if (profileStateMask & PROFILE_STATE_DISCONNECTING) {
        return BTConnectState::DISCONNECTING;
    } else {
        return BTConnectState::DISCONNECTED;
    }
}

BTConnectState ProfileServiceManager::GetProfileServicesConnectState() const
{
    unsigned int stateMask = 0;

    for (auto &sp : GET_SUPPORT_PROFILES()) {
        IProfile *profile = nullptr;
        if (pimpl->startedProfiles_.Find(sp.name_, profile)) {
            if (sp.name_ != PROFILE_NAME_GATT_CLIENT && sp.name_ != PROFILE_NAME_GATT_SERVER &&
                sp.name_ != PROFILE_NAME_SPP) {
                stateMask |= (unsigned int)profile->GetConnectState();
            }
        }
    }

    LOG_DEBUG("%{public}s profileServicesStateMask is %{public}d", __PRETTY_FUNCTION__, stateMask);
    if (stateMask & PROFILE_STATE_CONNECTED) {
        return BTConnectState::CONNECTED;
    } else if (stateMask & PROFILE_STATE_CONNECTING) {
        return BTConnectState::CONNECTING;
    } else if (stateMask & PROFILE_STATE_DISCONNECTING) {
        return BTConnectState::DISCONNECTING;
    } else {
        return BTConnectState::DISCONNECTED;
    }
}
}  // namespace bluetooth
}  // namespace OHOS