/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "pbap_pse_service.h"
#include "../obex/obex_utils.h"
#include "../obex/obex_mp_server.h"
#include "adapter_config.h"
#include "adapter_manager.h"
#include "base_observer_list.h"
#include "btm.h"
#include "class_creator.h"
#include "log.h"
#include "log_util.h"
#include "pbap_pse_def.h"
#include "pbap_pse_gap.h"
#include "pbap_pse_header_msg.h"
#include "pbap_pse_obex_server.h"
#include "pbap_pse_sdp.h"
#include "pbap_pse_state_machine.h"
#include "profile_config.h"
#include "rfcomm.h"

namespace OHOS {
namespace bluetooth {
struct PbapPseService::impl {
    // state machine
    std::map<std::string, std::unique_ptr<PbapPseStateMachine>> machineMap_ {};

    // observer
    BaseObserverList<IPbapPseObserver> observerMgrList_ {};
    // gap
    std::unique_ptr<PbapPseGap> pbapPseGap_ = nullptr;
    // sdp
    std::unique_ptr<PbapPseSdp> pbapPseSdp_ = nullptr;

    // obex server
    std::unique_ptr<PbapPseObexServer> pseObexServer_ = nullptr;
    // is disable?
    bool disableFlag_ = false;
    // rfcomm channel
    uint8_t rfcommScn_ = 0x00;
    // connection id mutex
    std::mutex connectIdMutex_ {};
    // machineMap mutex
    std::recursive_mutex machineMapMutex_ {};
    // connection id
    uint32_t connectIdSeq_ = 0;
    struct AdapterConfig {
        bool srmEnable_ = true;
        int rfcommMtu_ = 0;
        int l2capMtu_ = 0;
    } adapterConfig_ {};
};

PbapPseService::PbapPseService() : utility::Context(PROFILE_NAME_PBAP_PSE, "1.2.3"), pimpl(std::make_unique<impl>())
{
    PBAP_PSE_LOG_INFO("ProfileService:%{public}s Create", Name().c_str());
}

PbapPseService::~PbapPseService()
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
}

utility::Context *PbapPseService::GetContext()
{
    return this;
}

void PbapPseService::EnableService()
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    // load config
    LoadAdapterConfig();

    pimpl->disableFlag_ = false;
    pimpl->rfcommScn_ = RFCOMM_AssignServerNum();
    pimpl->pbapPseSdp_ = std::make_unique<PbapPseSdp>(pimpl->rfcommScn_, PBAP_PSE_GOEP_L2CAP_PSM);
    int retVal = pimpl->pbapPseSdp_->Register();
    if (retVal != RET_NO_ERROR) {
        GetContext()->OnEnable(PROFILE_NAME_PBAP_PSE, false);
        PBAP_PSE_LOG_ERROR("Enable PbapPse Service failure!");
        return;
    }
    BTM_AddLocalL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_PBAP, PBAP_PSE_GOEP_L2CAP_PSM);
    BTM_AddLocalRfcommScnForLogging(BTM_HCI_LOG_FILTER_MODULE_PBAP, pimpl->rfcommScn_);

    pimpl->pbapPseGap_ = std::make_unique<PbapPseGap>(pimpl->rfcommScn_, PBAP_PSE_GOEP_L2CAP_PSM);
    retVal = pimpl->pbapPseGap_->Register();
    if (retVal != RET_NO_ERROR) {
        GetContext()->OnEnable(PROFILE_NAME_PBAP_PSE, false);
        PBAP_PSE_LOG_ERROR("Enable PbapPse Service failure!");
        return;
    }

    ObexServerConfig config;
#ifdef PBAP_PSE_L2CAP_ENABLE
    config.useL2cap_ = true;
#else
    config.useL2cap_ = false;
#endif
    config.l2capPsm_ = PBAP_PSE_GOEP_L2CAP_PSM;
    config.l2capMtu_ = pimpl->adapterConfig_.l2capMtu_;
    config.useRfcomm_ = true;
    config.rfcommScn_ = pimpl->rfcommScn_;
    config.rfcommMtu_ = pimpl->adapterConfig_.rfcommMtu_;
    config.isSupportSrm_ = true;
    config.isSupportReliableSession_ = false;

    // call obex server startup
    pimpl->pseObexServer_ = std::make_unique<PbapPseObexServer>(PBAP_PSE_SERVICE_NAME, config, *this);
    bool isStarted = pimpl->pseObexServer_->StartUp() == 0;
    GetContext()->OnEnable(PROFILE_NAME_PBAP_PSE, isStarted);
    if (!isStarted) {
        PBAP_PSE_LOG_ERROR("Enable PbapPse Service failure!");
    }
}

void PbapPseService::Enable()
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    GetDispatcher()->PostTask(std::bind(&PbapPseService::EnableService, this));
}

void PbapPseService::DisableService()
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s start", __PRETTY_FUNCTION__);
    pimpl->disableFlag_ = true;
    if (pimpl->pbapPseSdp_) {
        pimpl->pbapPseSdp_->Deregister();
        BTM_RemoveLocalL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_PBAP, PBAP_PSE_GOEP_L2CAP_PSM);
        BTM_RemoveLocalRfcommScnChannelForLogging(BTM_HCI_LOG_FILTER_MODULE_PBAP, pimpl->rfcommScn_);
    }
    for (auto &stm : pimpl->machineMap_) {
        if (stm.second->GetConnectState() == BTConnectState::DISCONNECTED) {
            continue;
        }
        if (!stm.second->IsConnected() && !stm.second->IsAccepted()) {
            utility::Message msg(PSE_INCOMING_REJECT);
            stm.second->ProcessMessage(msg);
        } else if (!stm.second->IsConnected() && stm.second->IsAccepted()) {
            ProcessWaitToDisconnect(stm.second->GetDevice());
        } else if (stm.second->IsConnected()) {
            utility::Message msg(PSE_API_DISCONNECT);
            stm.second->ProcessMessage(msg);
        }
    }
    if (IsAllDisconnected()) {
        pimpl->pseObexServer_->ShutDown();
    }
    PBAP_PSE_LOG_INFO("Call %{public}s end", __PRETTY_FUNCTION__);
}

void PbapPseService::Disable()
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    GetDispatcher()->PostTask(std::bind(&PbapPseService::DisableService, this));
}

int PbapPseService::Connect(const RawAddress &device)
{
    return RET_NO_SUPPORT;
}

int PbapPseService::Disconnect(const RawAddress &device)
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    auto target = pimpl->machineMap_.find(device.GetAddress());
    if (target == pimpl->machineMap_.end()) {
        HILOGI("Disconnect: %{public}s doesn't exist.", GET_ENCRYPT_ADDR(device));
        return RET_BAD_STATUS;
    }
    BTConnectState state = target->second->GetConnectState();
    if (state != BTConnectState::CONNECTED) {
        HILOGI("Disconnect: %{public}s isn't connected.", GET_ENCRYPT_ADDR(device));
        return RET_BAD_STATUS;
    }
    GetDispatcher()->PostTask(std::bind(&PbapPseService::DisConnectInternal, this, device));
    return RET_NO_ERROR;
}

void PbapPseService::DisConnectInternal(RawAddress device)
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    auto target = pimpl->machineMap_.find(device.GetAddress());
    if (target != pimpl->machineMap_.end()) {
        if (target->second->GetConnectState() == BTConnectState::DISCONNECTED) {
            return;
        }
        if (target->second->IsConnected()) {
            utility::Message msg(PSE_API_DISCONNECT);
            target->second->ProcessMessage(msg);
        } else if (!target->second->IsConnected() && !target->second->IsAccepted()) {
            ProcessGrantPermission(device, false, false);
        } else if (!target->second->IsConnected() && target->second->IsAccepted()) {
            ProcessWaitToDisconnect(device);
        }
    }
}

std::list<RawAddress> PbapPseService::GetConnectDevices()
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    std::list<RawAddress> devList;
    for (auto &stm : pimpl->machineMap_) {
        BTConnectState state = stm.second->GetConnectState();
        if (state == BTConnectState::CONNECTED) {
            devList.push_front(stm.second->GetDevice());
        }
    }
    return devList;
}

int PbapPseService::GetConnectState()
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    uint8_t result = 0x00;
    for (auto &stm : pimpl->machineMap_) {
        BTConnectState state = stm.second->GetConnectState();
        if (state == BTConnectState::DISCONNECTED) {
            result |= PROFILE_STATE_DISCONNECTED;
        } else if (state == BTConnectState::CONNECTING) {
            result |= PROFILE_STATE_CONNECTING;
        } else if (state == BTConnectState::CONNECTED) {
            result |= PROFILE_STATE_CONNECTED;
        } else if (state == BTConnectState::DISCONNECTING) {
            result |= PROFILE_STATE_DISCONNECTING;
        }
    }
    return result;
}

int PbapPseService::GetMaxConnectNum()
{
    return RET_NO_SUPPORT;
}

int PbapPseService::SetConnectionStrategy(const RawAddress &device, int strategy)
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    switch (strategy) {
        case static_cast<int>(BTStrategyType::CONNECTION_ALLOWED):
        case static_cast<int>(BTStrategyType::CONNECTION_FORBIDDEN):
            if (!SaveConnectPolicy(device.GetAddress(), strategy)) {
                return RET_NO_SUPPORT;
            }
            break;
        default:
            return RET_BAD_PARAM;
    }
    if (strategy == static_cast<int>(BTStrategyType::CONNECTION_FORBIDDEN)) {
        GetDispatcher()->PostTask(std::bind(&PbapPseService::DisConnectInternal, this, device));
    }
    return RET_NO_ERROR;
}

int PbapPseService::GetConnectionStrategy(const RawAddress &device) const
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    int strategy = 0;
    if (!LoadConnectPolicy(device.GetAddress(), strategy)) {
        return static_cast<int>(BTStrategyType::CONNECTION_UNKNOWN);
    }
    if (strategy == static_cast<int>(BTStrategyType::CONNECTION_ALLOWED) ||
        strategy == static_cast<int>(BTStrategyType::CONNECTION_FORBIDDEN)) {
        return strategy;
    }
    return static_cast<int>(BTStrategyType::CONNECTION_UNKNOWN);
}

void PbapPseService::GrantPermission(const RawAddress &device, bool allow, bool save)
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    GetDispatcher()->PostTask(std::bind(&PbapPseService::ProcessGrantPermission, this, device, allow, save));
}

int PbapPseService::SetDevicePassword(const RawAddress &device, const std::string &password, std::string userId)
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    if (password.empty() || userId.size() > ObexDigestResponse::MAX_USER_ID_LEN) {
        PBAP_PSE_LOG_ERROR("SetDevicePassword: password or userId is invalid!");
        return RET_BAD_PARAM;
    }
    auto target = pimpl->machineMap_.find(device.GetAddress());
    if (target == pimpl->machineMap_.end()) {
        HILOGI("SetDevicePassword: %{public}s doesn't exist!", GET_ENCRYPT_ADDR(device));
        return RET_BAD_STATUS;
    }
    if (target->second->GetConnectState() != BTConnectState::CONNECTING) {
        HILOGI("SetDevicePassword: %{public}s isn't connecting, now state is %{public}d.",
            GET_ENCRYPT_ADDR(device), static_cast<int>(target->second->GetConnectState()));
        return RET_BAD_STATUS;
    }
    GetDispatcher()->PostTask(std::bind(&PbapPseService::ProcessSetDevicePassword, this, device, password, userId));
    return RET_NO_ERROR;
}

int PbapPseService::GetDeviceState(const RawAddress &device) const
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    BTConnectState result = BTConnectState::DISCONNECTED;
    auto target = pimpl->machineMap_.find(device.GetAddress());
    if (target != pimpl->machineMap_.end()) {
        result = target->second->GetConnectState();
    }
    return static_cast<int>(result);
}

std::vector<RawAddress> PbapPseService::GetDevicesByStates(const std::vector<int> &states) const
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    std::vector<RawAddress> devices;
    for (auto &state : states) {
        for (auto &stm : pimpl->machineMap_) {
            if (static_cast<int>(stm.second->GetConnectState()) == state) {
                devices.push_back(stm.second->GetDevice());
            }
        }
    }
    return devices;
}

void PbapPseService::RegisterObserver(IPbapPseObserver &observer)
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    pimpl->observerMgrList_.Register(observer);
}

void PbapPseService::DeregisterObserver(IPbapPseObserver &observer)
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    pimpl->observerMgrList_.Deregister(observer);
}

uint32_t PbapPseService::GetNextConnectId() const
{
    std::lock_guard<std::mutex> lock(pimpl->connectIdMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    return ++pimpl->connectIdSeq_;
}

void PbapPseService::PostMessage(utility::Message msg)
{
    switch (msg.what_) {
        case PSE_SHUTDOWN_COMPLETED:
            GetDispatcher()->PostTask(std::bind(&PbapPseService::ProcessShutDownComplete, this, msg));
            break;
        default:
            break;
    }
}

void PbapPseService::ProcessObexRequest(int what, const RawAddress &device) const
{
    switch (what) {
        case PSE_DEVICE_DISCONNECTED:
            ProcessDeviceDisconnected(device, utility::Message(what));
            break;
        case PSE_DEVICE_TRANSPORT_ERROR:
            // is need ？
            break;
        default:
            break;
    }
}

void PbapPseService::ProcessObexRequest(int what, const RawAddress &device, ObexIncomingConnect &incomingConnect)
{
    switch (what) {
        case PSE_DEVICE_CONNECT_INCOMING:
            ProcessConnectIncoming(device, utility::Message(what, 0, static_cast<void *>(&incomingConnect)));
            break;
        default:
            break;
    }
}

void PbapPseService::ProcessObexRequest(
    int what, const RawAddress &device, ObexServerSession &session, const ObexHeader &req)
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    auto target = pimpl->machineMap_.find(device.GetAddress());
    if (target == pimpl->machineMap_.end()) {
        HILOGI("ProcessObexRequest: %{public}s doesn't exist! ", GET_ENCRYPT_ADDR(device));
        return;
    }
    auto obexMsg = std::make_unique<PbapPseObexMessage>(session, req);
    utility::Message msg(what, 0, static_cast<void *>(obexMsg.get()));
    switch (what) {
        case PSE_REQ_OBEX_CONNECT:
            target->second->ProcessMessage(msg);
            break;
        case PSE_REQ_OBEX_DISCONNECT:
            target->second->ProcessMessage(msg);
            break;
        case PSE_REQ_OBEX_GET:
            target->second->ProcessMessage(msg);
            break;
        case PSE_REQ_OBEX_SETPATH:
            target->second->ProcessMessage(msg);
            break;
        default:
            break;
    }
}

void PbapPseService::ProcessObexBusy(const RawAddress &device, bool isBusy) const
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    auto target = pimpl->machineMap_.find(device.GetAddress());
    if (target == pimpl->machineMap_.end()) {
        return;
    }
    target->second->SetBusy(isBusy);
}

void PbapPseService::ProcessConnectIncoming(const RawAddress &device, const utility::Message &msg)
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    auto target = pimpl->machineMap_.find(device.GetAddress());
    if (target != pimpl->machineMap_.end() && target->second->GetConnectState() != BTConnectState::DISCONNECTED) {
        HILOGI("ProcessConnectIncoming: %{public}s already exists! , state is %{public}d .",
            GET_ENCRYPT_ADDR(device), static_cast<int>(target->second->GetConnectState()));
        auto incomeConnect = static_cast<ObexIncomingConnect *>(msg.arg2_);
        incomeConnect->RejectConnection();
        return;
    }
    auto stm = std::make_unique<PbapPseStateMachine>(
        device, pimpl->observerMgrList_, std::bind(&PbapPseService::GetNextConnectId, this));
    stm->Init();
    pimpl->machineMap_[device.GetAddress()] = std::move(stm);
    pimpl->machineMap_[device.GetAddress()]->ProcessMessage(msg);
    if (pimpl->disableFlag_) {
        HILOGI("PBAP DISABLING AUTO FORBIDDEN FOR DEVICE:%{public}s", GET_ENCRYPT_ADDR(device));
        ProcessGrantPermission(device, false, false);
        return;
    }
    BTPermissionType permission = AdapterManager::GetInstance()->GetPhonebookPermission(device.GetAddress());
    if (BTPermissionType::ACCESS_ALLOWED == permission) {
        HILOGI("PBAP ACCESS AUTO ALLOWED FOR DEVICE:%{public}s", GET_ENCRYPT_ADDR(device));
        SetConnectionStrategy(device, static_cast<int>(BTStrategyType::CONNECTION_ALLOWED));
        ProcessGrantPermission(device, true, false);
    } else if (BTPermissionType::ACCESS_FORBIDDEN == permission) {
        HILOGI("PBAP ACCESS AUTO FORBIDDEN FOR DEVICE:%{public}s", GET_ENCRYPT_ADDR(device));
        ProcessGrantPermission(device, false, false);
    } else {
        pimpl->machineMap_[device.GetAddress()]->StartTimer(
            std::bind(&PbapPseService::IncomingTimeout, this, device), PBAP_PSE_INCOMING_TIME_OUT);
        HILOGI("PBAP ACCESS UNKNOWN FOR DEVICE:%{public}s", GET_ENCRYPT_ADDR(device));
        pimpl->observerMgrList_.ForEach([device](IPbapPseObserver &observer) {
            HILOGI("OnServicePermission(%{public}s) is called.", GET_ENCRYPT_ADDR(device));
            observer.OnServicePermission(device);
        });
    }
}

void PbapPseService::ProcessGrantPermission(const RawAddress &device, bool allow, bool save) const
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    auto target = pimpl->machineMap_.find(device.GetAddress());
    if (target == pimpl->machineMap_.end()) {
        HILOGI("ProcessGrantPermission: %{public}s doesn't exist.", GET_ENCRYPT_ADDR(device));
        return;
    }
    if (target->second->GetConnectState() != BTConnectState::CONNECTING) {
        HILOGI("ProcessGrantPermission: %{public}s isn't connecting, now state is %{public}d.",
            GET_ENCRYPT_ADDR(device), static_cast<int>(target->second->GetConnectState()));
        return;
    }
    target->second->StopTimer();
    if (save) {
        auto adapterManager = AdapterManager::GetInstance();
        // Save Permission
        BTPermissionType permission = adapterManager->GetPhonebookPermission(device.GetAddress());
        if (allow && BTPermissionType::ACCESS_ALLOWED != permission) {
            HILOGI("Pbap permission changed to ALLOWED for device: %{public}s", GET_ENCRYPT_ADDR(device));
            adapterManager->SetPhonebookPermission(device.GetAddress(), BTPermissionType::ACCESS_ALLOWED);
        } else if (!allow && BTPermissionType::ACCESS_FORBIDDEN != permission) {
            HILOGI("Pbap permission changed to FORBIDDEN for device: %{public}s", GET_ENCRYPT_ADDR(device));
            adapterManager->SetPhonebookPermission(device.GetAddress(), BTPermissionType::ACCESS_FORBIDDEN);
        }
    }
    utility::Message msg(allow ? PSE_INCOMING_ACCEPT : PSE_INCOMING_REJECT, static_cast<int>(save));
    target->second->ProcessMessage(msg);
}

void PbapPseService::ProcessSetDevicePassword(const RawAddress &device, std::string password, std::string userId) const
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    auto target = pimpl->machineMap_.find(device.GetAddress());
    if (target == pimpl->machineMap_.end()) {
        HILOGI("ProcessSetDevicePassword: %{public}s doesn't exist! ", GET_ENCRYPT_ADDR(device));
        return;
    }
    if (target->second->GetConnectState() != BTConnectState::CONNECTING) {
        HILOGI("ProcessSetDevicePassword: %{public}s isn't connecting, now state is %{public}d.",
            GET_ENCRYPT_ADDR(device), static_cast<int>(target->second->GetConnectState()));
        return;
    }
    auto msgData = std::make_unique<PbapPsePasswordInputMsg>(password, userId);
    utility::Message msg(PSE_PASSWORD_INPUT, 0, static_cast<void *>(msgData.get()));
    target->second->ProcessMessage(msg);
}

void PbapPseService::ProcessShutDownComplete(const utility::Message &msg)
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    if (pimpl->pbapPseGap_) {
        pimpl->pbapPseGap_->Deregister();
    }
    int retVal = RFCOMM_FreeServerNum(pimpl->rfcommScn_);
    if (retVal != RET_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Pbap free server num 0x%02X error, retVal=%{public}d", pimpl->rfcommScn_, retVal);
    }
    GetContext()->OnDisable(PROFILE_NAME_PBAP_PSE, true);
}

void PbapPseService::ProcessDeviceDisconnected(const RawAddress &device, const utility::Message &msg) const
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    auto target = pimpl->machineMap_.find(device.GetAddress());
    if (target != pimpl->machineMap_.end()) {
        target->second->ProcessMessage(msg);
    }
    if (pimpl->disableFlag_ && IsAllDisconnected()) {
        pimpl->pseObexServer_->ShutDown();
    }
}

void PbapPseService::IncomingTimeout(const RawAddress &device)
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    GetDispatcher()->PostTask(std::bind(&PbapPseService::ProcessIncomingTimeout, this, device));
}

void PbapPseService::ProcessIncomingTimeout(const RawAddress &device) const
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    auto target = pimpl->machineMap_.find(device.GetAddress());
    if (target != pimpl->machineMap_.end()) {
        utility::Message msg(PSE_INCOMING_REJECT);
        target->second->ProcessMessage(msg);
    }
}

bool PbapPseService::SaveConnectPolicy(const std::string &addr, int strategy) const
{
    IProfileConfig *config = ProfileConfig::GetInstance();
    if (!config->SetValue(addr, SECTION_CONNECTION_POLICIES, PROPERTY_PBAP_CONNECTION_POLICY, strategy)) {
        HILOGE("SaveConnectPolicy: %{public}s %{public}s save fail.",
            GetEncryptAddr(addr).c_str(), PROPERTY_PBAP_CONNECTION_POLICY.c_str());
        return false;
    }
    return true;
}

bool PbapPseService::LoadConnectPolicy(const std::string &addr, int &strategy) const
{
    IProfileConfig *config = ProfileConfig::GetInstance();
    if (!config->GetValue(addr, SECTION_CONNECTION_POLICIES, PROPERTY_PBAP_CONNECTION_POLICY, strategy)) {
        HILOGE("LoadConnectPolicy: %{public}s %{public}s load fail.",
            GetEncryptAddr(addr).c_str(), PROPERTY_PBAP_CONNECTION_POLICY.c_str());
        return false;
    }
    return true;
}

void PbapPseService::ProcessWaitToDisconnect(const RawAddress &device) const
{
    std::lock_guard<std::recursive_mutex> lock(pimpl->machineMapMutex_);
    HILOGI("device is accepted, wait it connected: %{public}s ", GET_ENCRYPT_ADDR(device));
    auto target = pimpl->machineMap_.find(device.GetAddress());
    if (target != pimpl->machineMap_.end()) {
        utility::Message msg(PSE_WAITING_CONNECTED_TO_DISCONNECT);
        target->second->ProcessMessage(msg);
    }
}

bool PbapPseService::IsAllDisconnected() const
{
    bool result = true;
    for (auto &stm : pimpl->machineMap_) {
        if (stm.second->GetConnectState() != BTConnectState::DISCONNECTED) {
            result = false;
            break;
        }
    }
    return result;
}

void PbapPseService::LoadAdapterConfig() const
{
    IAdapterConfig *adpterConfig = AdapterConfig::GetInstance();
    auto &config = pimpl->adapterConfig_;
    if (!adpterConfig->GetValue(SECTION_PBAP_PSE_SERVICE, PROPERTY_SRM_ENABLE, config.srmEnable_)) {
        PBAP_PSE_LOG_ERROR("Load config %{public}s failure", PROPERTY_SRM_ENABLE.c_str());
        config.srmEnable_ = true;
    }
    if (!adpterConfig->GetValue(SECTION_PBAP_PSE_SERVICE, PROPERTY_RFCOMM_MTU, config.rfcommMtu_)) {
        PBAP_PSE_LOG_ERROR("Load config %{public}s failure", PROPERTY_RFCOMM_MTU.c_str());
        config.rfcommMtu_ = OBEX_DEFAULT_MTU;
    }
    if (!adpterConfig->GetValue(SECTION_PBAP_PSE_SERVICE, PROPERTY_L2CAP_MTU, config.l2capMtu_)) {
        PBAP_PSE_LOG_ERROR("Load config %{public}s failure", PROPERTY_L2CAP_MTU.c_str());
        config.l2capMtu_ = OBEX_DEFAULT_MTU;
    }
    PBAP_PSE_LOG_INFO(
        "srmEnable = %{public}d, rfcommMtu = 0x%X, l2capMtu_ = 0x%X", config.srmEnable_, config.rfcommMtu_, config.l2capMtu_);
}

REGISTER_CLASS_CREATOR(PbapPseService);
}  // namespace bluetooth
}  // namespace OHOS
