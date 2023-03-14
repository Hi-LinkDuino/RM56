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

#include "pbap_pce_service.h"
#include "adapter_config.h"
#include "class_creator.h"
#include "interface_adapter.h"
#include "log.h"
#include "log_util.h"
#include "pbap_pce_base_state.h"
#include "pbap_pce_gap.h"
#include "pbap_pce_sdp.h"
#include "pbap_pce_state_machine.h"
#include "profile_config.h"

namespace OHOS {
namespace bluetooth {
PbapPceService::PbapPceService() : utility::Context("PbapPceService", "1.2.3")
{
    PBAP_PCE_LOG_INFO("ProfileService:%{public}s Create", Name().c_str());
    pbapPceSdp_ = std::make_unique<PbapPceSdp>(*this);
}

PbapPceService::~PbapPceService()
{}

utility::Context *PbapPceService::GetContext()
{
    return this;
}

void PbapPceService::RegisterObserver(IPbapPceObserver &observer)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    observerMgrList_.Register(observer);
}

void PbapPceService::DeregisterObserver(IPbapPceObserver &observer)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    observerMgrList_.Deregister(observer);
}

void PbapPceService::ForwardMsgToStm(const RawAddress &device, const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
    std::lock_guard<std::recursive_mutex> lock(machineMapMutex_);
    if (machineMap_.find(device.GetAddress()) != machineMap_.end()) {
        machineMap_[device.GetAddress()]->ProcessMessage(msg);
    }
    PBAP_PCE_LOG_INFO("%{public}s end, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
}

int PbapPceService::SetConnectionStrategy(const RawAddress &device, int strategy)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    switch (strategy) {
        case static_cast<int>(BTStrategyType::CONNECTION_ALLOWED):
        case static_cast<int>(BTStrategyType::CONNECTION_FORBIDDEN):
            if (!SaveConnectPolicy(device.GetAddress(), strategy)) {
                PBAP_PCE_LOG_ERROR("%{public}s end, save strategy value failed.", __PRETTY_FUNCTION__);
                return RET_NO_SUPPORT;
            }
            break;
        default:
            PBAP_PCE_LOG_ERROR("%{public}s end, save strategy invalid value.", __PRETTY_FUNCTION__);
            return RET_BAD_PARAM;
    }

    // Adjoint action
    if (strategy == static_cast<int>(BTStrategyType::CONNECTION_FORBIDDEN)) {
        if (IsConnected(device)) {
            Disconnect(device);  // Immediately
        } else if (GetDeviceState(device) == static_cast<int>(BTConnectState::CONNECTING)) {
            utility::Message msg(PCE_REQ_SET_TARGET_STATE, PCE_TARGET_STATE_DISCONNECTED);
            ForwardMsgToStm(device, msg);
        }
    } else if (strategy == static_cast<int>(BTStrategyType::CONNECTION_ALLOWED)) {
        if (GetDeviceState(device) == static_cast<int>(BTConnectState::DISCONNECTED)) {
            Connect(device);  // Immediately
        } else if (GetDeviceState(device) == static_cast<int>(BTConnectState::DISCONNECTING)) {
            utility::Message msg(PCE_REQ_SET_TARGET_STATE, PCE_TARGET_STATE_CONNECTED);
            ForwardMsgToStm(device, msg);
        }
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

int PbapPceService::GetConnectionStrategy(const RawAddress &device)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    int strategy = static_cast<int>(BTStrategyType::CONNECTION_UNKNOWN);
    if (!LoadConnectPolicy(device.GetAddress(), strategy)) {
        PBAP_PCE_LOG_ERROR("%{public}s end, load strategy value failed.", __PRETTY_FUNCTION__);
        return strategy;
    }
    if (strategy == static_cast<int>(BTStrategyType::CONNECTION_ALLOWED) ||
        strategy == static_cast<int>(BTStrategyType::CONNECTION_FORBIDDEN)) {
        PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
        return strategy;
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return strategy;
}

void PbapPceService::EnableService()
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    serviceState_ = PBAP_PCE_STATE_STARTUPING;
    LoadPceConfig();
    int retVal = pbapPceSdp_->Register();
    if (retVal != RET_NO_ERROR) {
        PBAP_PCE_LOG_ERROR("end, Call pbapPceSdp_->Register() Error");
        serviceState_ = PBAP_PCE_STATE_STARTUP;
        GetContext()->OnEnable(PROFILE_NAME_PBAP_PCE, false);
        return;
    }
#ifdef PBAP_PCE_ENABLE_L2CAP_PSM
    ObexMpClient::RegisterL2capLPsm(PBAP_PCE_LOCAL_GOEP_L2CAP_PSM);
#endif
    serviceState_ = PBAP_PCE_STATE_STARTUP;
    GetContext()->OnEnable(PROFILE_NAME_PBAP_PCE, true);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceService::Enable()
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);

    if (GetDispatcher() == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s end, GetDispatcher() == nullptr", __PRETTY_FUNCTION__);
        return;
    }
    GetDispatcher()->PostTask(std::bind(&PbapPceService::EnableService, this));

    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

bool PbapPceService::TryShutDown(bool ret)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (serviceState_ == PBAP_PCE_STATE_SHUTDOWN) {
        PBAP_PCE_LOG_INFO("%{public}s end, already shutdown status", __PRETTY_FUNCTION__);
        return true;
    }

    if (serviceState_ != PBAP_PCE_STATE_SHUTDOWNING) {
        PBAP_PCE_LOG_INFO("%{public}s end, not shutdowning status", __PRETTY_FUNCTION__);
        return false;
    }

    std::vector<int> states {
        static_cast<int>(BTConnectState::CONNECTING),
        static_cast<int>(BTConnectState::CONNECTED),
        static_cast<int>(BTConnectState::DISCONNECTING)
    };
    if (GetDevicesByStates(states).size() == 0) {
        std::lock_guard<std::recursive_mutex> lock(machineMapMutex_);
        serviceState_ = PBAP_PCE_STATE_SHUTDOWN;
#ifdef PBAP_PCE_ENABLE_L2CAP_PSM
        ObexMpClient::DeregisterL2capLPsm(PBAP_PCE_LOCAL_GOEP_L2CAP_PSM);
#endif
        GetContext()->OnDisable(PROFILE_NAME_PBAP_PCE, ret);
        PBAP_PCE_LOG_INFO("%{public}s end, Shutdown OK", __PRETTY_FUNCTION__);
        return true;
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return false;
}

bool PbapPceService::IsBusy(const RawAddress &device)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);

    bool ret = false;
    std::string strAddr = device.GetAddress();
    std::lock_guard<std::recursive_mutex> lock(machineMapMutex_);
    if (machineMap_.find(strAddr) != machineMap_.end()) {
        auto stm = static_cast<PbapPceStateMachine *>(machineMap_[strAddr].get());
        ret = stm->IsBusy();
    }

    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

bool PbapPceService::SetBusy(const RawAddress &device, bool busy)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    bool ret = false;
    std::string strAddr = device.GetAddress();
    std::lock_guard<std::recursive_mutex> lock(machineMapMutex_);
    if (machineMap_.find(strAddr) != machineMap_.end()) {
        auto stm = static_cast<PbapPceStateMachine *>(machineMap_[strAddr].get());
        ret = stm->SetBusy(busy);
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

void PbapPceService::SetPowerStatusBusy(const RawAddress &device, bool busy)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    std::string strAddr = device.GetAddress();
    std::lock_guard<std::recursive_mutex> lock(machineMapMutex_);
    if (machineMap_.find(strAddr) != machineMap_.end()) {
        auto stm = static_cast<PbapPceStateMachine *>(machineMap_[strAddr].get());
        stm->SetPowerStatusBusy(busy);
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return;
}

bool PbapPceService::CheckAndSetDeviceBusy(const RawAddress &device)
{
    if (IsBusy(device)) {
        PBAP_PCE_LOG_ERROR("%{public}s Busy!", __PRETTY_FUNCTION__);
        return false;
    }

    if (!SetBusy(device, true)) {
        PBAP_PCE_LOG_ERROR("%{public}s Set Busy error!", __PRETTY_FUNCTION__);
        return false;
    }

    return true;
}

int PbapPceService::CheckDeviceStatusForPbapAction(const RawAddress &device)
{
    if (GetDispatcher() == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s GetDispatcher() == nullptr", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }

    if (!IsConnected(device)) {
        PBAP_PCE_LOG_ERROR("%{public}s NOT connected!", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }

    if (!CheckAndSetDeviceBusy(device)) {
        PBAP_PCE_LOG_ERROR("%{public}s CheckAndSetDeviceBusy failed!", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }

    return RET_NO_ERROR;
}

void PbapPceService::DisableService()
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    serviceState_ = PBAP_PCE_STATE_SHUTDOWNING;

    int retVal = 0;
    if (pbapPceSdp_) {
        retVal = pbapPceSdp_->Deregister();
        if (retVal != RET_NO_ERROR) {
            PBAP_PCE_LOG_ERROR("pbapPceSdp_->Deregister() error");
        }
    }

    if (!TryShutDown(retVal == RET_NO_ERROR)) {
        retVal = DisconnectAllDevices();
        if (retVal != RET_NO_ERROR) {
            PBAP_PCE_LOG_ERROR("DisconnectAllDevices error");
        }
        TryShutDown(true);
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

int PbapPceService::DisconnectAllDevices()
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    int retVal2 = RET_NO_ERROR;
    std::lock_guard<std::recursive_mutex> lock(machineMapMutex_);
    for (auto &stm : machineMap_) {
        int retVal1 = AbortDownloadingInternal(RawAddress(stm.first));
        retVal2 = DisconnectInternal(RawAddress(stm.first));
        PBAP_PCE_LOG_INFO("%{public}s [%{public}s] retVal1=%{public}d, retVal2=%{public}d", __PRETTY_FUNCTION__, stm.first.c_str(), retVal1, retVal2);
    }

    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return retVal2;
}

void PbapPceService::Disable()
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (GetDispatcher() == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s GetDispatcher() == nullptr", __PRETTY_FUNCTION__);
        return;
    }
    GetDispatcher()->PostTask(std::bind(&PbapPceService::DisableService, this));
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

int PbapPceService::Connect(const RawAddress &device)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    RawAddress rDevice(device.GetAddress());
    BTStrategyType strate = BTStrategyType(GetConnectionStrategy(rDevice));
    if (strate == BTStrategyType::CONNECTION_FORBIDDEN) {
        PBAP_PCE_LOG_ERROR("%{public}s end, connect forbidden.", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }

    int state = GetDeviceState(device);
    if ((state == static_cast<int>(BTConnectState::CONNECTING)) ||
        (state == static_cast<int>(BTConnectState::CONNECTED)) ||
        (state == static_cast<int>(BTConnectState::DISCONNECTING))) {
        PBAP_PCE_LOG_ERROR("%{public}s end, bad status.", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }

    if (GetConnectedDevices().size() == (std::size_t)GetMaxConnectNum()) {
        PBAP_PCE_LOG_ERROR("%{public}s end, maximum devices is %{public}d.", __PRETTY_FUNCTION__, GetMaxConnectNum());
        return RET_NO_SUPPORT;
    }

    if (GetDispatcher() == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s end, GetDispatcher() == nullptr", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }

    if (IsBusy(device)) {
        PBAP_PCE_LOG_ERROR("%{public}s end, Is busy!", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }

    GetDispatcher()->PostTask(std::bind(&PbapPceService::ConnectInternal, this, device));
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

int PbapPceService::ConnectInternal(const RawAddress &device)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (serviceState_ != PBAP_PCE_STATE_STARTUP) {
        PBAP_PCE_LOG_ERROR("%{public}s end, pbap pce service isn't started.", __PRETTY_FUNCTION__);
        return RET_NO_SUPPORT;
    }

    if (GetConnectedDevices().size() == (std::size_t)GetMaxConnectNum()) {
        PBAP_PCE_LOG_ERROR("%{public}s end, maximum devices is %{public}d.",  __PRETTY_FUNCTION__, GetMaxConnectNum());
        return RET_NO_SUPPORT;
    }

    std::string addr = device.GetAddress();
    HILOGI("Connect to rawaddr: %{public}s", GetEncryptAddr(addr).c_str());
    std::lock_guard<std::recursive_mutex> lock(machineMapMutex_);
    if (machineMap_.find(addr) == machineMap_.end()) {
        machineMap_[addr] = std::make_unique<PbapPceStateMachine>(device, *this, *pbapPceSdp_, observerMgrList_);
    } else {
        int state = GetDeviceState(device);
        if ((state == static_cast<int>(BTConnectState::DISCONNECTING)) ||
            (state == static_cast<int>(BTConnectState::DISCONNECTED))) {
            PBAP_PCE_LOG_INFO("start connect for disconnected device.");
            machineMap_[addr]->InitState(PCE_CONNECTING_STATE);
        } else {
            PBAP_PCE_LOG_ERROR("%{public}s end, already connecting or  for connected device.", __PRETTY_FUNCTION__);
            return RET_BAD_STATUS;
        }
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

int PbapPceService::Disconnect(const RawAddress &device)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    int state = GetDeviceState(device);
    if ((state == static_cast<int>(BTConnectState::CONNECTING)) ||
        (state == static_cast<int>(BTConnectState::DISCONNECTING)) ||
        (state == static_cast<int>(BTConnectState::DISCONNECTED))) {
        PBAP_PCE_LOG_ERROR("%{public}s end, bad status.", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }

    if (GetDispatcher() == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s end, GetDispatcher() == nullptr", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }

    GetDispatcher()->PostTask(std::bind(&PbapPceService::DisconnectInternal, this, device));
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

bool PbapPceService::IsConnected(const RawAddress &device)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (GetDeviceState(device) == static_cast<int>(BTConnectState::CONNECTED)) {
        return true;
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return false;
}

int PbapPceService::DisconnectInternal(const RawAddress &device)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (IsBusy(device)) {
        PBAP_PCE_LOG_ERROR("%{public}s end, Is busy!", __PRETTY_FUNCTION__);
        utility::Message msg(PCE_REQ_SET_TARGET_STATE, PCE_TARGET_STATE_DISCONNECTED);
        ForwardMsgToStm(device, msg);
    } else {
        utility::Message msg(PCE_REQ_DISCONNECTED);
        ForwardMsgToStm(device, msg);
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

// for override IProfilePbapPce
std::list<RawAddress> PbapPceService::GetConnectDevices()
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(machineMapMutex_);
    std::list<RawAddress> devList;

    for (auto &it : machineMap_) {
        std::string name = it.second->GetState()->Name();
        if (name == PCE_CONNECTED_STATE) {
            auto stm = static_cast<PbapPceStateMachine *>(it.second.get());
            devList.push_front(stm->GetDevice());
        }
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return devList;
}

int PbapPceService::GetConnectState()
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    uint8_t result = 0;
    std::lock_guard<std::recursive_mutex> lock(machineMapMutex_);
    for (auto &stm : machineMap_) {
        std::string name = stm.second->GetState()->Name();
        if (name == PCE_DISCONNECTED_STATE) {
            result |= PROFILE_STATE_DISCONNECTED;
        } else if (name == PCE_CONNECTING_STATE) {
            result |= PROFILE_STATE_CONNECTING;
        } else if (name == PCE_CONNECTED_STATE) {
            result |= PROFILE_STATE_CONNECTED;
        } else if (name == PCE_DISCONNECTING_STATE) {
            result |= PROFILE_STATE_DISCONNECTING;
        }
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return result;
}

int PbapPceService::GetMaxConnectNum()
{
    return GetPceConfig().pceMaxDevices_;
}

void PbapPceService::ProcessObexRespMessage(const RawAddress &device, const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
    SetPowerStatusBusy(device, false);
    switch (msg.what_) {
        case PCE_OBEX_CONNECTED:
            ProcessConnected(device, msg);
            break;
        case PCE_OBEX_DISCONNECTED:
            ProcessDisconnected(device, msg);
            break;
        case PCE_OBEX_CONNECT_FAILED:
            ProcessConnectFailed(device, msg);
            break;
        case PCE_TRANSPORT_FAILED:
            ProcessTransportFailed(device, msg);
            break;
        case PCE_PULLPHONEBOOKSIZE_COMPLETED:
        case PCE_PULLPHONEBOOK_COMPLETED:
        case PCE_SETPHONEBOOK_COMPLETED:
        case PCE_PULLVCARDLISTINGSIZE_COMPLETED:
        case PCE_PULLVCARDLISTING_COMPLETED:
        case PCE_PULLVCARDENTRY_COMPLETED:
        case PCE_ABORTDOWNLOADING_COMPLETED:
            ProcessPhoneBookActionCompleted(device, msg);
            break;
        default:
            break;
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceService::PostMessage(const RawAddress &device, const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
    if (GetDispatcher() == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s end, GetDispatcher() == nullptr", __PRETTY_FUNCTION__);
        return;
    }
    switch (msg.what_) {
        case PCE_SDP_FAILED:
        case PCE_GAP_FAILED:
            GetDispatcher()->PostTask(std::bind(&PbapPceService::ProcessSdpGapFailed, this, device, msg));
            break;
        case PCE_SDP_FINISH:
        case PCE_GAP_FINISH:
            GetDispatcher()->PostTask(std::bind(&PbapPceService::ProcessSdpGapFinish, this, device, msg));
            break;
        case PCE_PASSWORD_INPUT:
            GetDispatcher()->PostTask(std::bind(&PbapPceService::ProcessUserInput, this, device, msg));
            break;
        default:
            break;
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceService::ProcessSdpGapFailed(const RawAddress &device, const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
    ForwardMsgToStm(device, msg);
    PBAP_PCE_LOG_INFO("%{public}s end, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
}

void PbapPceService::ProcessSdpGapFinish(const RawAddress &device, const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
    ForwardMsgToStm(device, msg);
    PBAP_PCE_LOG_INFO("%{public}s end, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
}

void PbapPceService::ProcessConnectFailed(const RawAddress &device, const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
    ForwardMsgToStm(device, msg);
    TryShutDown(true);
    PBAP_PCE_LOG_INFO("%{public}s end, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
}

void PbapPceService::ProcessConnected(const RawAddress &device, const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
    ForwardMsgToStm(device, msg);
    PBAP_PCE_LOG_INFO("%{public}s end, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
}

void PbapPceService::ProcessDisconnected(const RawAddress &device, const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
    ForwardMsgToStm(device, msg);
    TryShutDown(true);
    PBAP_PCE_LOG_INFO("%{public}s end, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
}

void PbapPceService::ProcessTransportFailed(const RawAddress &device, const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
    ForwardMsgToStm(device, msg);
    TryShutDown(true);
    PBAP_PCE_LOG_INFO("%{public}s end, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
}

void PbapPceService::ProcessUserInput(const RawAddress &device, const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
    ForwardMsgToStm(device, msg);
    PBAP_PCE_LOG_INFO("%{public}s end, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
}

void PbapPceService::ProcessPhoneBookActionCompleted(const RawAddress &device, const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
    ForwardMsgToStm(device, msg);
    utility::Message msg2(PCE_REQ_TRANSIT_TARGET_STATE);
    ForwardMsgToStm(device, msg2);
    TryShutDown(true);
    PBAP_PCE_LOG_INFO("%{public}s end, msg.what=%{public}d", __PRETTY_FUNCTION__, msg.what_);
}

int PbapPceService::GetDeviceState(const RawAddress &device)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    BTConnectState result = BTConnectState::DISCONNECTED;
    std::lock_guard<std::recursive_mutex> lock(machineMapMutex_);
    auto target = machineMap_.find(device.GetAddress());
    if (target != machineMap_.end()) {
        std::string name = target->second->GetState()->Name();
        if (name == PCE_DISCONNECTED_STATE) {
            result = BTConnectState::DISCONNECTED;
        } else if (name == PCE_CONNECTING_STATE) {
            result = BTConnectState::CONNECTING;
        } else if (name == PCE_CONNECTED_STATE) {
            result = BTConnectState::CONNECTED;
        } else if (name == PCE_DISCONNECTING_STATE) {
            result = BTConnectState::DISCONNECTING;
        }
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return static_cast<int>(result);
}

std::vector<RawAddress> PbapPceService::GetDevicesByStates(const std::vector<int> &states)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(machineMapMutex_);
    std::vector<RawAddress> devices;
    for (auto &state : states) {
        for (auto &it : machineMap_) {
            if (static_cast<int>(GetDeviceState(RawAddress(it.first))) == state) {
                auto stm = static_cast<PbapPceStateMachine *>(it.second.get());
                devices.push_back(stm->GetDevice());
            }
        }
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return devices;
}

std::string PbapPceService::GetDownloadFileName(const RawAddress &device)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(machineMapMutex_);
    if (machineMap_.find(device.GetAddress()) != machineMap_.end()) {
        auto stm = static_cast<PbapPceStateMachine *>(machineMap_[device.GetAddress()].get());
        return stm->GetDownloadFileName();
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return "";
}

const PbapPceConfig &PbapPceService::GetPceConfig() const
{
    return pbapPceConfig_;
}

std::vector<RawAddress> PbapPceService::GetConnectedDevices()
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(machineMapMutex_);
    std::vector<RawAddress> devices;
    std::vector<int> states {static_cast<int>(BTConnectState::CONNECTED)};
    auto allDevices = GetDevicesByStates(states);
    std::copy(allDevices.begin(), allDevices.end(), devices.begin());
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return devices;
}

int PbapPceService::SetDevicePassword(const RawAddress &device, const std::string &password, const std::string &userId)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (password.empty() || userId.size() > ObexDigestResponse::MAX_USER_ID_LEN) {
        return RET_BAD_PARAM;
    }
    std::lock_guard<std::recursive_mutex> lock(machineMapMutex_);
    auto target = machineMap_.find(device.GetAddress());
    if (target == machineMap_.end()) {
        return RET_NO_SUPPORT;
    }
    auto msgData = std::make_unique<PbapPcePasswordInputMsg>(password, userId);
    utility::Message msg(PCE_PASSWORD_INPUT, 0, static_cast<void *>(msgData.release()));
    PostMessage(device, msg);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

int PbapPceService::PullPhoneBook(const RawAddress &device, const IPbapPullPhoneBookParam &param)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    // validate param
    if (!param.Validate()) {
        PBAP_PCE_LOG_ERROR("%{public}s end, RET_BAD_PARAM", __PRETTY_FUNCTION__);
        return RET_BAD_PARAM;
    }
    int ret = CheckDeviceStatusForPbapAction(device);
    if (ret != RET_NO_ERROR) {
        PBAP_PCE_LOG_ERROR("%{public}s end, RET_BAD_STATUS", __PRETTY_FUNCTION__);
        return ret;
    }
    GetDispatcher()->PostTask(std::bind(&PbapPceService::PullPhoneBookInternal, this, device, param));
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

int PbapPceService::SetPhoneBook(const RawAddress &device, const std::u16string &name, int flag)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if ((flag != PBAP_FLAG_GO_TO_ROOT) && (flag != PBAP_FLAG_GO_DOWN) && (flag != PBAP_FLAG_GO_UP)) {
        PBAP_PCE_LOG_ERROR("%{public}s end, param flag=%{public}d is invalid.", __PRETTY_FUNCTION__, flag);
        return RET_BAD_PARAM;
    }

    int ret = CheckDeviceStatusForPbapAction(device);
    if (ret != RET_NO_ERROR) {
        PBAP_PCE_LOG_ERROR("%{public}s end, RET_BAD_STATUS", __PRETTY_FUNCTION__);
        return ret;
    }

    GetDispatcher()->PostTask(std::bind(&PbapPceService::SetPhoneBookInternal, this, device, name, flag));
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

int PbapPceService::PullvCardListing(const RawAddress &device, const IPbapPullvCardListingParam &param)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    // validate param
    if (!param.Validate()) {
        PBAP_PCE_LOG_ERROR("%{public}s end, RET_BAD_PARAM", __PRETTY_FUNCTION__);
        return RET_BAD_PARAM;
    }

    int ret = CheckDeviceStatusForPbapAction(device);
    if (ret != RET_NO_ERROR) {
        PBAP_PCE_LOG_INFO("%{public}s end, RET_BAD_STATUS ", __PRETTY_FUNCTION__);
        return ret;
    }

    GetDispatcher()->PostTask(std::bind(&PbapPceService::PullvCardListingInternal, this, device, param));
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

int PbapPceService::PullvCardEntry(const RawAddress &device, const IPbapPullvCardEntryParam &param)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (!param.Validate()) {
        PBAP_PCE_LOG_ERROR("%{public}s end, RET_BAD_PARAM", __PRETTY_FUNCTION__);
        return RET_BAD_PARAM;
    }

    int ret = CheckDeviceStatusForPbapAction(device);
    if (ret != RET_NO_ERROR) {
        PBAP_PCE_LOG_ERROR("%{public}s end, RET_BAD_STATUS", __PRETTY_FUNCTION__);
        return ret;
    }

    GetDispatcher()->PostTask(std::bind(&PbapPceService::PullvCardEntryInternal, this, device, param));
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

bool PbapPceService::IsDownloading(const RawAddress &device)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    bool ret = IsBusy(device);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int PbapPceService::AbortDownloading(const RawAddress &device)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (!IsConnected(device)) {
        PBAP_PCE_LOG_ERROR("%{public}s end, NOT connected!", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }

    if (GetDispatcher() == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s end, GetDispatcher() == nullptr", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }
    GetDispatcher()->PostTask(std::bind(&PbapPceService::AbortDownloadingInternal, this, device));

    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

int PbapPceService::PullPhoneBookInternal(const RawAddress &device, const IPbapPullPhoneBookParam &param)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    PbapPcePullPhoneBookMsg pbapMsg;
    pbapMsg.SetName(param.GetName());
    pbapMsg.SetPropertySelector(param.GetPropertySelector());
    pbapMsg.SetFormat(param.GetFormat());
    pbapMsg.SetMaxListCount(param.GetMaxListCount());
    pbapMsg.SetListStartOffset(param.GetListStartOffset());
    pbapMsg.SetResetNewMissedCalls(param.GetResetNewMissedCalls());
    pbapMsg.SetvCardSelector(param.GetvCardSelector());
    pbapMsg.SetvCardSelectorOp(param.GetvCardSelectorOp());
    pbapMsg.SetSpecifiedBitset(param.GetSpecifiedBitset());

    utility::Message msg(PCE_REQ_PULLPHONEBOOK, 0, (void *)&pbapMsg);
    std::string strAddr = device.GetAddress();
    std::lock_guard<std::recursive_mutex> lock(machineMapMutex_);
    if (machineMap_.find(strAddr) != machineMap_.end()) {
        auto stm = static_cast<PbapPceStateMachine *>(machineMap_[strAddr].get());
        stm->ProcessMessage(msg);  // go to PceConnectedState::Dispatch
    }

    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

int PbapPceService::SetPhoneBookInternal(const RawAddress &device, const std::u16string &name, int flag)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    PbapPceSetPhoneBookMsg pbapMsg(name, flag);
    utility::Message msg(PCE_REQ_SETPHONEBOOK, 0, (void *)&pbapMsg);
    std::string strAddr = device.GetAddress();
    std::lock_guard<std::recursive_mutex> lock(machineMapMutex_);
    if (machineMap_.find(strAddr) != machineMap_.end()) {
        auto stm = static_cast<PbapPceStateMachine *>(machineMap_[strAddr].get());
        stm->ProcessMessage(msg);  // go to PceConnectedState::Dispatch
    }

    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

int PbapPceService::PullvCardListingInternal(const RawAddress &device, const IPbapPullvCardListingParam &param)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    PbapPcePullvCardListingMsg pbapMsg;
    pbapMsg.SetName(param.GetName());
    pbapMsg.SetOrder(param.GetOrder());
    pbapMsg.SetSearchValue(param.GetSearchValue());
    pbapMsg.SetSearchProperty(param.GetSearchProperty());
    pbapMsg.SetMaxListCount(param.GetMaxListCount());
    pbapMsg.SetListStartOffset(param.GetListStartOffset());
    pbapMsg.SetResetNewMissedCalls(param.GetResetNewMissedCalls());
    pbapMsg.SetvCardSelector(param.GetvCardSelector());
    pbapMsg.SetvCardSelectorOp(param.GetvCardSelectorOp());
    pbapMsg.SetSpecifiedBitset(param.GetSpecifiedBitset());
    utility::Message msg(PCE_REQ_PULLVCARDLISTING, 0, (void *)&pbapMsg);
    ForwardMsgToStm(device, msg);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

int PbapPceService::PullvCardEntryInternal(const RawAddress &device, const IPbapPullvCardEntryParam &param)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    PbapPcePullvCardEntryMsg pbapMsg(
        param.GetName(), param.GetPropertySelector(), param.GetFormat(), param.GetSpecifiedBitset());
    utility::Message msg(PCE_REQ_PULLVCARDENTRY, 0, (void *)&pbapMsg);
    ForwardMsgToStm(device, msg);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

int PbapPceService::AbortDownloadingInternal(const RawAddress &device)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    utility::Message msg(PCE_REQ_ABORTDOWNLOADING);
    ForwardMsgToStm(device, msg);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

void PbapPceService::LoadPceConfig()
{
    IAdapterConfig *adpterConfig = AdapterConfig::GetInstance();
    auto &config = reinterpret_cast<PbapPceConfig &>(const_cast<PbapPceConfig &>(GetPceConfig()));
    if (!adpterConfig->GetValue(SECTION_PBAP_PCE_SERVICE, PROPERTY_SRM_ENABLE, config.srmEnable_)) {
        PBAP_PCE_LOG_ERROR("Load config %{public}s failure", PROPERTY_SRM_ENABLE.c_str());
    }
    if (!adpterConfig->GetValue(SECTION_PBAP_PCE_SERVICE, PROPERTY_RFCOMM_MTU, config.rfcommMtu_)) {
        PBAP_PCE_LOG_ERROR("Load config %{public}s failure", PROPERTY_RFCOMM_MTU.c_str());
    }
    if (!adpterConfig->GetValue(SECTION_PBAP_PCE_SERVICE, PROPERTY_L2CAP_MTU, config.l2capMtu_)) {
        PBAP_PCE_LOG_ERROR("Load config %{public}s failure", PROPERTY_L2CAP_MTU.c_str());
    }
    if (!adpterConfig->GetValue(SECTION_PBAP_PCE_SERVICE, PROPERTY_MAX_CONNECTED_DEVICES, config.pceMaxDevices_)) {
        PBAP_PCE_LOG_ERROR("Load config %{public}s failure", PROPERTY_MAX_CONNECTED_DEVICES.c_str());
    }
    PBAP_PCE_LOG_INFO("srmEnable = %{public}d, rfcommMtu=0x%X, l2capMtu_=0x%X, maxDevices=%{public}d",
        config.srmEnable_,
        config.rfcommMtu_,
        config.l2capMtu_,
        config.pceMaxDevices_);
}

bool PbapPceService::SaveConnectPolicy(const std::string &addr, int strategy)
{
    IProfileConfig *config = ProfileConfig::GetInstance();
    if (!config->SetValue(addr, SECTION_CONNECTION_POLICIES, PROPERTY_PBAP_CLIENT_CONNECTION_POLICY, strategy)) {
        HILOGE("addr: %{public}s %{public}s save fail.",
            GetEncryptAddr(addr).c_str(), PROPERTY_PBAP_CLIENT_CONNECTION_POLICY.c_str());
        return false;
    }
    return true;
}

bool PbapPceService::LoadConnectPolicy(const std::string &addr, int &strategy)
{
    IProfileConfig *config = ProfileConfig::GetInstance();
    if (!config->GetValue(addr, SECTION_CONNECTION_POLICIES, PROPERTY_PBAP_CLIENT_CONNECTION_POLICY, strategy)) {
        HILOGE("addr: %{public}s %{public}s load fail.",
            GetEncryptAddr(addr).c_str(), PROPERTY_PBAP_CLIENT_CONNECTION_POLICY.c_str());
        return false;
    }
    return true;
}

REGISTER_CLASS_CREATOR(PbapPceService);
}  // namespace bluetooth
}  // namespace OHOS
