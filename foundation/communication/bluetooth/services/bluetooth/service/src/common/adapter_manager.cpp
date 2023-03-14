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

#include "adapter_manager.h"

#include <array>
#include <functional>

#include "btm.h"
#include "btstack.h"
#include "log.h"

#include "adapter_config.h"
#include "adapter_device_config.h"
#include "adapter_device_info.h"
#include "adapter_state_machine.h"
#include "base_def.h"
#include "base_observer_list.h"
#include "class_creator.h"
#include "permission_utils.h"
#include "power_manager.h"
#include "profile_config.h"
#include "profile_service_manager.h"
#include "sys_state_machine.h"

namespace OHOS {
namespace bluetooth {
// data define
const int TRANSPORT_MAX = 2;

struct AdapterInfo {
    AdapterInfo(std::unique_ptr<IAdapter> instance, std::unique_ptr<AdapterStateMachine> stateMachine)
        : instance_(std::move(instance)), stateMachine_(std::move(stateMachine))
    {}
    ~AdapterInfo()
    {}

    BTStateID state_ = BTStateID::STATE_TURN_OFF;
    std::unique_ptr<IAdapter> instance_ = nullptr;
    std::unique_ptr<AdapterStateMachine> stateMachine_ = nullptr;
};

// static function
IAdapterManager *IAdapterManager::GetInstance()
{
    return AdapterManager::GetInstance();
}
AdapterManager *AdapterManager::GetInstance()
{
    static AdapterManager instance;
    return &instance;
}

// impl class
struct AdapterManager::impl {
    impl();
    ~impl();

    std::recursive_mutex syncMutex_ = {};
    std::promise<void> stopPromise_ = {};
    std::promise<void> resetPromise_ = {};
    std::unique_ptr<utility::Dispatcher> dispatcher_ = nullptr;
    std::array<std::unique_ptr<AdapterInfo>, TRANSPORT_MAX> adapters_ = {};
    SysStateMachine sysStateMachine_ = {};
    std::string sysState_ = SYS_STATE_STOPPED;
    BtmCallbacks hciFailureCallbacks = {};
    BaseObserverList<IAdapterStateObserver> adapterObservers_ = {};
    BaseObserverList<ISystemStateObserver> systemObservers_ = {};

    class AdaptersContextCallback;
    std::unique_ptr<AdaptersContextCallback> contextCallback_ = nullptr;

    void OnEnable(const std::string &name, bool ret);
    void OnDisable(const std::string &name, bool ret);
    void ProcessMessage(const BTTransport transport, const utility::Message &msg);

    BT_DISALLOW_COPY_AND_ASSIGN(impl);
};

class AdapterManager::impl::AdaptersContextCallback : public utility::IContextCallback {
public:
    explicit AdaptersContextCallback(AdapterManager::impl &impl) : impl_(impl){};
    ~AdaptersContextCallback() = default;

    void OnEnable(const std::string &name, bool ret)
    {
        LOG_DEBUG("%{public}s, name=%{public}s, ret=%{public}d\n", __PRETTY_FUNCTION__, name.c_str(), ret);
        impl_.OnEnable(name, ret);
    }
    void OnDisable(const std::string &name, bool ret)
    {
        LOG_DEBUG("%{public}s, name=%{public}s, ret=%{public}d\n", __PRETTY_FUNCTION__, name.c_str(), ret);
        impl_.OnDisable(name, ret);
    }

private:
    AdapterManager::impl &impl_;
};

AdapterManager::impl::impl()
{
    dispatcher_ = std::make_unique<utility::Dispatcher>("AdapterManager");
    dispatcher_->Initialize();

    // context callback create
    contextCallback_ = std::make_unique<AdaptersContextCallback>(*this);
}

AdapterManager::impl::~impl()
{
    if (dispatcher_ != nullptr) {
        dispatcher_->Uninitialize();
    }
}

void AdapterManager::impl::OnEnable(const std::string &name, bool ret)
{
    LOG_DEBUG("%{public}s, name=%{public}s, ret=%{public}d\n", __PRETTY_FUNCTION__, name.c_str(), ret);
    BTTransport transport = BTTransport::ADAPTER_BREDR;

    if (name.c_str() == ADAPTER_NAME_CLASSIC) {
        transport = BTTransport::ADAPTER_BREDR;
    } else if (name.c_str() == ADAPTER_NAME_BLE) {
        transport = BTTransport::ADAPTER_BLE;
    } else {
        LOG_ERROR("%{public}s, name=%{public}s is warning transport\n", __PRETTY_FUNCTION__, name.c_str());
    }

    utility::Message msg(AdapterStateMachine::MSG_ADAPTER_ENABLE_CMP, ret ? true : false);
    dispatcher_->PostTask(std::bind(&AdapterManager::impl::ProcessMessage, this, transport, msg));
}

void AdapterManager::impl::OnDisable(const std::string &name, bool ret)
{
    LOG_DEBUG("%{public}s, name=%{public}s, ret=%{public}d\n", __PRETTY_FUNCTION__, name.c_str(), ret);
    BTTransport transport = BTTransport::ADAPTER_BREDR;

    if (name.c_str() == ADAPTER_NAME_CLASSIC) {
        transport = BTTransport::ADAPTER_BREDR;
    } else if (name.c_str() == ADAPTER_NAME_BLE) {
        transport = BTTransport::ADAPTER_BLE;
    } else {
        LOG_ERROR("%{public}s, name=%{public}s is warning transport\n", __PRETTY_FUNCTION__, name.c_str());
    }

    utility::Message msg(AdapterStateMachine::MSG_ADAPTER_DISABLE_CMP, ret ? true : false);
    dispatcher_->PostTask(std::bind(&AdapterManager::impl::ProcessMessage, this, transport, msg));
}

void AdapterManager::impl::ProcessMessage(const BTTransport transport, const utility::Message &msg)
{
    std::lock_guard<std::recursive_mutex> lock(syncMutex_);

    if (adapters_[transport] == nullptr) {
        LOG_DEBUG("%{public}s adapter is nullptr", __PRETTY_FUNCTION__);
        return;
    }

    if (adapters_[transport]->stateMachine_ == nullptr) {
        LOG_DEBUG("%{public}s stateMachine_ is nullptr", __PRETTY_FUNCTION__);
        return;
    }

    adapters_[transport]->stateMachine_->ProcessMessage(msg);
}

// AdapterManager class
AdapterManager::AdapterManager() : pimpl(std::make_unique<AdapterManager::impl>())
{
    // sys state Machine create
    pimpl->sysStateMachine_.Init(*this);
}

AdapterManager::~AdapterManager()
{}

bool AdapterManager::Start()
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);

    if (GetSysState() == SYS_STATE_STARTED) {
        LOG_ERROR("Bluetooth has been started!!");
        return false;
    }

    if (!AdapterConfig::GetInstance()->Load()) {
        LOG_ERROR("Load Config File Failed!!");
        return false;
    }

    if (!ProfileConfig::GetInstance()->Load()) {
        LOG_ERROR("Load Profile Config File Failed!!");
        return false;
    }

    if (BTM_Initialize() != BT_NO_ERROR) {
        LOG_ERROR("Bluetooth Stack Initialize Failed!!");
        return false;
    }

    if (!OutputSetting()) {
        LOG_ERROR("Bluetooth output set Failed!!");
        return false;
    }

    CreateAdapters();

    ProfileServiceManager::Initialize(*pimpl->dispatcher_);

    IPowerManager::Initialize(*pimpl->dispatcher_);

    RegisterHciResetCallback();

    OnSysStateChange(SYS_STATE_STARTED);

    utility::Message msg(SysStateMachine::MSG_SYS_START_CMP);
    pimpl->dispatcher_->PostTask(std::bind(&utility::StateMachine::ProcessMessage, &(pimpl->sysStateMachine_), msg));

    return true;
}

bool AdapterManager::OutputSetting() const
{
    bool outputValue = false;
    bool desensitization = false;

    AdapterConfig::GetInstance()->GetValue(SECTION_OUTPUT_SETTING, PROPERTY_DESENSITIZATION, desensitization);
    if (AdapterConfig::GetInstance()->GetValue(SECTION_OUTPUT_SETTING, PROPERTY_BTSNOOP_OUTPUT, outputValue) &&
        outputValue) {
        std::string outputPath = "./snoop.log";
        AdapterConfig::GetInstance()->GetValue(SECTION_OUTPUT_SETTING, PROPERTY_BTSNOOP_OUTPUT_PATH, outputPath);

        if (BTM_SetSnoopFilePath(outputPath.c_str(), outputPath.length()) != BT_NO_ERROR) {
            LOG_ERROR("Set snoop file path Failed!!");
            return false;
        }

        if (BTM_EnableSnoopFileOutput(desensitization) != BT_NO_ERROR) {
            LOG_ERROR("Enable snoop file output Failed!!");
            return false;
        }
    } else {
        if (BTM_DisableSnoopFileOutput() != BT_NO_ERROR) {
            LOG_ERROR("Disable snoop file output Failed!!");
            return false;
        }
    }

    outputValue = false;
    if (AdapterConfig::GetInstance()->GetValue(SECTION_OUTPUT_SETTING, PROPERTY_HCILOG_OUTPUT, outputValue) &&
        outputValue) {
        if (BTM_EnableHciLogOutput(desensitization) != BT_NO_ERROR) {
            LOG_ERROR("Enable HciLog output Failed!!");
            return false;
        }
    } else {
        if (BTM_DisableHciLogOutput() != BT_NO_ERROR) {
            LOG_ERROR("Disable HciLog output Failed!!");
            return false;
        }
    }

    return true;
}

void AdapterManager::Stop() const
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);

    if (GetSysState() == SYS_STATE_STOPPED) {
        LOG_DEBUG("AdapterManager is stoped");
    } else if (GetSysState() == SYS_STATE_STOPPING) {
        LOG_DEBUG("AdapterManager is stoping...");
    } else {
        std::promise<void> stopPromise;
        std::future<void> stopFuture = stopPromise.get_future();

        {
            std::lock_guard<std::recursive_mutex> lock(pimpl->syncMutex_);
            pimpl->stopPromise_ = std::move(stopPromise);
        }

        utility::Message msg(SysStateMachine::MSG_SYS_STOP_REQ);
        pimpl->dispatcher_->PostTask(
            std::bind(&utility::StateMachine::ProcessMessage, &(pimpl->sysStateMachine_), msg));
        stopFuture.wait();
    }
}

bool AdapterManager::AdapterStop() const
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);
    ProfileServiceManager::Uninitialize();
    IPowerManager::Uninitialize();

    DeregisterHciResetCallback();

    for (int i = 0; i < TRANSPORT_MAX; i++) {
        if (pimpl->adapters_[i]) {
            pimpl->adapters_[i]->instance_->GetContext()->Uninitialize();
            pimpl->adapters_[i] = nullptr;
        }
    }

    BTM_Close();

    utility::Message msg(SysStateMachine::MSG_SYS_STOP_CMP);
    pimpl->dispatcher_->PostTask(std::bind(&utility::StateMachine::ProcessMessage, &(pimpl->sysStateMachine_), msg));

    return true;
}

bool AdapterManager::Enable(const BTTransport transport) const
{
    LOG_DEBUG("%{public}s start transport is %{public}d", __PRETTY_FUNCTION__, transport);
    std::lock_guard<std::recursive_mutex> lock(pimpl->syncMutex_);

    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        LOG_ERROR("Enable() false, check permission failed");
        return false;
    }

    if (GetSysState() != SYS_STATE_STARTED) {
        LOG_ERROR("AdapterManager system is stoped");
        return false;
    }

    if (pimpl->adapters_[transport] == nullptr) {
        LOG_INFO("%{public}s BTTransport not register", __PRETTY_FUNCTION__);
        return false;
    }

    if (GetState(transport) == BTStateID::STATE_TURN_OFF) {
        utility::Message msg(AdapterStateMachine::MSG_USER_ENABLE_REQ);
        pimpl->dispatcher_->PostTask(std::bind(&AdapterManager::impl::ProcessMessage, pimpl.get(), transport, msg));
        return true;
    } else if (GetState(transport) == BTStateID::STATE_TURN_ON) {
        LOG_INFO("%{public}s is turn on", __PRETTY_FUNCTION__);
        return false;
    } else {
        LOG_INFO("%{public}s is turning state %{public}d", __PRETTY_FUNCTION__, GetState(transport));
        return false;
    }
}

bool AdapterManager::Disable(const BTTransport transport) const
{
    LOG_DEBUG("%{public}s start transport is %{public}d", __PRETTY_FUNCTION__, transport);
    std::lock_guard<std::recursive_mutex> lock(pimpl->syncMutex_);

    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        LOG_ERROR("Disable() false, check permission failed");
        return false;
    }

    if (pimpl->adapters_[transport] == nullptr) {
        LOG_INFO("%{public}s BTTransport not register", __PRETTY_FUNCTION__);
        return false;
    }

    if (GetState(transport) == BTStateID::STATE_TURN_ON) {
        utility::Message msg(AdapterStateMachine::MSG_USER_DISABLE_REQ);
        pimpl->dispatcher_->PostTask(std::bind(&AdapterManager::impl::ProcessMessage, pimpl.get(), transport, msg));
        return true;
    } else if (GetState(transport) == BTStateID::STATE_TURN_OFF) {
        LOG_INFO("%{public}s is turn off", __PRETTY_FUNCTION__);
        return false;
    } else {
        LOG_INFO("%{public}s is turning state %{public}d", __PRETTY_FUNCTION__, GetState(transport));
        return false;
    }
}

bool AdapterManager::FactoryReset() const
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);

    if (GetSysState() == SYS_STATE_STARTED) {
        std::promise<void> resetPromise;
        std::future<void> resetFuture = resetPromise.get_future();

        {
            std::lock_guard<std::recursive_mutex> lock(pimpl->syncMutex_);
            pimpl->resetPromise_ = std::move(resetPromise);
        }

        utility::Message msg(SysStateMachine::MSG_SYS_FACTORY_RESET_REQ);
        pimpl->dispatcher_->PostTask(
            std::bind(&utility::StateMachine::ProcessMessage, &(pimpl->sysStateMachine_), msg));
        resetFuture.wait();
        return true;
    } else {
        LOG_INFO("System state is not started");
        return false;
    }
}

void AdapterManager::HciFailedReset(void *context)
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);
    (static_cast<AdapterManager *>(context))->Reset();
}

void AdapterManager::RegisterHciResetCallback()
{
    pimpl->hciFailureCallbacks.hciFailure = HciFailedReset;
    BTM_RegisterCallbacks(&(pimpl->hciFailureCallbacks), this);
}

void AdapterManager::DeregisterHciResetCallback() const
{
    if (pimpl->hciFailureCallbacks.hciFailure != nullptr) {
        BTM_DeregisterCallbacks(&(pimpl->hciFailureCallbacks));
        pimpl->hciFailureCallbacks.hciFailure = nullptr;
    }
}

void AdapterManager::Reset() const
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);

    utility::Message msg(SysStateMachine::MSG_SYS_RESET_REQ);
    pimpl->dispatcher_->PostTask(std::bind(&utility::StateMachine::ProcessMessage, &(pimpl->sysStateMachine_), msg));
}

bool AdapterManager::ClearAllStorage() const
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);

    if (!AdapterConfig::GetInstance()->Reload()) {
        return false;
    }
    if (!ProfileConfig::GetInstance()->Reload()) {
        return false;
    }
    if (!AdapterDeviceConfig::GetInstance()->Reload()) {
        return false;
    }
    if (!AdapterDeviceInfo::GetInstance()->Reload()) {
        return false;
    }

    utility::Message msg(SysStateMachine::MSG_SYS_CLEAR_ALL_STORAGE_CMP);
    pimpl->dispatcher_->PostTask(std::bind(&utility::StateMachine::ProcessMessage, &(pimpl->sysStateMachine_), msg));
    return true;
}

BTStateID AdapterManager::GetState(const BTTransport transport) const
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->syncMutex_);

    if (pimpl->adapters_[transport] == nullptr) {
        LOG_INFO("%{public}s BTTransport not register", __PRETTY_FUNCTION__);
        return BTStateID::STATE_TURN_OFF;
    }

    return pimpl->adapters_[transport]->state_;
}

bool AdapterManager::RegisterStateObserver(IAdapterStateObserver &observer) const
{
    return pimpl->adapterObservers_.Register(observer);
}

bool AdapterManager::DeregisterStateObserver(IAdapterStateObserver &observer) const
{
    return pimpl->adapterObservers_.Deregister(observer);
}

bool AdapterManager::RegisterSystemStateObserver(ISystemStateObserver &observer) const
{
    return pimpl->systemObservers_.Register(observer);
}

bool AdapterManager::DeregisterSystemStateObserver(ISystemStateObserver &observer) const
{
    return pimpl->systemObservers_.Deregister(observer);
}

BTConnectState AdapterManager::GetAdapterConnectState() const
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);
    return ProfileServiceManager::GetInstance()->GetProfileServicesConnectState();
}

IAdapter *AdapterManager::GetAdapter(const BTTransport transport) const
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->syncMutex_);

    if (pimpl->adapters_[transport] != nullptr) {
        return pimpl->adapters_[transport]->instance_.get();
    } else {
        return nullptr;
    }
}

void AdapterManager::OnSysStateChange(const std::string &state) const
{
    LOG_DEBUG("%{public}s state is %{public}s", __PRETTY_FUNCTION__, state.c_str());

    std::string oldSysState;
    std::string newSysState = state;

    {  // lock start,update systerm state
        std::lock_guard<std::recursive_mutex> lock(pimpl->syncMutex_);
        oldSysState = pimpl->sysState_;
        pimpl->sysState_ = state;
    }  // lock end

    // notify systerm state update
    BTSystemState notifySysState = (newSysState == SYS_STATE_STARTED) ? BTSystemState::ON : BTSystemState::OFF;
    if ((newSysState != oldSysState) && ((newSysState == SYS_STATE_STARTED) || (oldSysState == SYS_STATE_STARTED))) {
        LOG_DEBUG(
            "%{public}s oldSysState is %{public}s, newSysState is %{public}s", __PRETTY_FUNCTION__, oldSysState.c_str(), newSysState.c_str());
        pimpl->systemObservers_.ForEach(
            [notifySysState](ISystemStateObserver &observer) { observer.OnSystemStateChange(notifySysState); });
    }
}

std::string AdapterManager::GetSysState() const
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->syncMutex_);
    return pimpl->sysState_;
}

void AdapterManager::OnSysStateExit(const std::string &state) const
{
    LOG_DEBUG("%{public}s state is %{public}s", __PRETTY_FUNCTION__, state.c_str());

    if (state == SYS_STATE_FRESETTING) {
        pimpl->resetPromise_.set_value();
    } else if (state == SYS_STATE_STOPPING) {
        pimpl->stopPromise_.set_value();
    } else {
        // Nothing to do.
    }
}

void AdapterManager::OnAdapterStateChange(const BTTransport transport, const BTStateID state) const
{
    LOG_DEBUG("%{public}s transport is %{public}d state is %{public}d", __PRETTY_FUNCTION__, transport, state);
    std::lock_guard<std::recursive_mutex> lock(pimpl->syncMutex_);

    if (pimpl->adapters_[transport] == nullptr) {
        return;
    }

    // notify observers state update
    if (pimpl->adapters_[transport]->state_ != state) {
        pimpl->adapters_[transport]->state_ = state;
        if (GetSysState() != SYS_STATE_RESETTING) {
            pimpl->adapterObservers_.ForEach(
                [transport, state](IAdapterStateObserver &observer) { observer.OnStateChange(transport, state); });
        }
    }

    // notify sys state machine
    int classicState = pimpl->adapters_[BTTransport::ADAPTER_BREDR]
                           ? pimpl->adapters_[BTTransport::ADAPTER_BREDR]->state_
                           : BTStateID::STATE_TURN_OFF;
    int bleState = pimpl->adapters_[BTTransport::ADAPTER_BLE] ? pimpl->adapters_[BTTransport::ADAPTER_BLE]->state_
                                                              : BTStateID::STATE_TURN_OFF;

    utility::Message msg(SysStateMachine::MSG_SYS_ADAPTER_STATE_CHANGE_REQ);
    msg.arg1_ = ((unsigned int)classicState << CLASSIC_ENABLE_STATE_BIT) + bleState;
    pimpl->dispatcher_->PostTask(std::bind(&utility::StateMachine::ProcessMessage, &(pimpl->sysStateMachine_), msg));
}

void AdapterManager::OnProfileServicesEnableComplete(const BTTransport transport, const bool ret) const
{
    LOG_DEBUG("%{public}s transport is %{public}d, ret is %{public}d", __PRETTY_FUNCTION__, transport, ret);

    utility::Message msg(AdapterStateMachine::MSG_PROFILE_ENABLE_CMP, ret ? true : false);
    pimpl->dispatcher_->PostTask(std::bind(&AdapterManager::impl::ProcessMessage, pimpl.get(), transport, msg));
}

void AdapterManager::OnProfileServicesDisableComplete(const BTTransport transport, const bool ret) const
{
    LOG_DEBUG("%{public}s transport is %{public}d, ret is %{public}d", __PRETTY_FUNCTION__, transport, ret);

    utility::Message msg(AdapterStateMachine::MSG_PROFILE_DISABLE_CMP, ret ? true : false);
    pimpl->dispatcher_->PostTask(std::bind(&AdapterManager::impl::ProcessMessage, pimpl.get(), transport, msg));
}

void AdapterManager::OnPairDevicesRemoved(const BTTransport transport, const std::vector<RawAddress> &devices) const
{
    pimpl->dispatcher_->PostTask(std::bind(&AdapterManager::RemoveDeviceProfileConfig, this, transport, devices));
}

void AdapterManager::RemoveDeviceProfileConfig(
    const BTTransport transport, const std::vector<RawAddress> &devices) const
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);

    BTTransport otherTransport =
        (transport == BTTransport::ADAPTER_BREDR) ? BTTransport::ADAPTER_BLE : BTTransport::ADAPTER_BREDR;

    std::vector<RawAddress> otherDevices;
    otherDevices.clear();

    if (pimpl->adapters_[otherTransport] != nullptr) {
        otherDevices = GetAdapter(otherTransport)->GetPairedDevices();
    }

    for (auto &device : devices) {
        if (std::find(otherDevices.begin(), otherDevices.end(), device) == otherDevices.end()) {
            ProfileConfig::GetInstance()->RemoveAddr(device.GetAddress());
        }
    }
}

void AdapterManager::CreateAdapters() const
{
    static const std::array<std::pair<std::string, std::string>, TRANSPORT_MAX> adapterConfigTbl = {
        std::make_pair(SECTION_CLASSIC_ADAPTER, ADAPTER_NAME_CLASSIC),
        std::make_pair(SECTION_BLE_ADAPTER, ADAPTER_NAME_BLE),
    };

    for (int i = 0; i < TRANSPORT_MAX; i++) {
        bool value = false;
        if (AdapterConfig::GetInstance()->GetValue(adapterConfigTbl[i].first, PROPERTY_IS_VALID, value) && value) {
            std::unique_ptr<IAdapter> adapter(ClassCreator<IAdapter>::NewInstance(adapterConfigTbl[i].second));
            std::unique_ptr<AdapterStateMachine> stateMachine(
                std::make_unique<AdapterStateMachine>(*pimpl->dispatcher_));

            if (adapter == nullptr || stateMachine == nullptr) {
                adapter = nullptr;
                stateMachine = nullptr;
                LOG_ERROR("Create %{public}s Failed!!", adapterConfigTbl[i].second.c_str());
            } else {
                adapter->GetContext()->Initialize();
                adapter->GetContext()->RegisterCallback(*(pimpl->contextCallback_));
                stateMachine->Init(*adapter);
                pimpl->adapters_[i].reset(
                    std::make_unique<AdapterInfo>(std::move(adapter), std::move(stateMachine)).release());
            }
        }
    }
}

int AdapterManager::GetMaxNumConnectedAudioDevices() const
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(pimpl->syncMutex_);

    int value = 0;
    IAdapterConfig *config = AdapterConfig::GetInstance();

    if (!config->GetValue(SECTION_A2DP_SRC_SERVICE, PROPERTY_MAX_CONNECTED_DEVICES, value)) {
        LOG_ERROR("%{public}s %{public}s not found", SECTION_A2DP_SRC_SERVICE.c_str(), PROPERTY_MAX_CONNECTED_DEVICES.c_str());
    }
    return value;
}

bool AdapterManager::SetPhonebookPermission(const std::string &address, BTPermissionType permission) const
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(pimpl->syncMutex_);

    IProfileConfig *config = ProfileConfig::GetInstance();
    bool tmp = false;
    switch (permission) {
        case BTPermissionType::ACCESS_UNKNOWN:
            return config->RemoveProperty(address, SECTION_PERMISSION, PROPERTY_PHONEBOOK_PERMISSION);
        case BTPermissionType::ACCESS_ALLOWED:
            tmp = true;
            break;
        case BTPermissionType::ACCESS_FORBIDDEN:
            break;
        default:
            return false;
    }
    return config->SetValue(address, SECTION_PERMISSION, PROPERTY_PHONEBOOK_PERMISSION, tmp);
}

BTPermissionType AdapterManager::GetPhonebookPermission(const std::string &address) const
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(pimpl->syncMutex_);

    IProfileConfig *config = ProfileConfig::GetInstance();
    bool value = false;

    if (!config->GetValue(address, SECTION_PERMISSION, PROPERTY_PHONEBOOK_PERMISSION, value)) {
        LOG_DEBUG("%{public}s %{public}s not found", address.c_str(), PROPERTY_PHONEBOOK_PERMISSION.c_str());
        return BTPermissionType::ACCESS_UNKNOWN;
    }

    if (value) {
        return BTPermissionType::ACCESS_ALLOWED;
    } else {
        return BTPermissionType::ACCESS_FORBIDDEN;
    }
}

bool AdapterManager::SetMessagePermission(const std::string &address, BTPermissionType permission) const
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(pimpl->syncMutex_);

    IProfileConfig *config = ProfileConfig::GetInstance();
    bool tmp = false;

    switch (permission) {
        case BTPermissionType::ACCESS_UNKNOWN:
            return config->RemoveProperty(address, SECTION_PERMISSION, PROPERTY_MESSAGE_PERMISSION);
        case BTPermissionType::ACCESS_ALLOWED:
            tmp = true;
            break;
        case BTPermissionType::ACCESS_FORBIDDEN:
            break;
        default:
            return false;
    }
    return config->SetValue(address, SECTION_PERMISSION, PROPERTY_MESSAGE_PERMISSION, tmp);
}

BTPermissionType AdapterManager::GetMessagePermission(const std::string &address) const
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(pimpl->syncMutex_);

    IProfileConfig *config = ProfileConfig::GetInstance();
    bool value = false;

    if (!config->GetValue(address, SECTION_PERMISSION, PROPERTY_MESSAGE_PERMISSION, value)) {
        LOG_DEBUG("%{public}s %{public}s not found", address.c_str(), PROPERTY_MESSAGE_PERMISSION.c_str());
        return BTPermissionType::ACCESS_UNKNOWN;
    }

    if (value) {
        return BTPermissionType::ACCESS_ALLOWED;
    } else {
        return BTPermissionType::ACCESS_FORBIDDEN;
    }
}

int AdapterManager::GetPowerMode(const std::string &address) const
{
    LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(pimpl->syncMutex_);

    RawAddress addr = RawAddress(address);
    return static_cast<int>(IPowerManager::GetInstance().GetPowerMode(addr));
}
}  // namespace bluetooth
}  // namespace OHOS