/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "opp_service.h"

#include "../obex/obex_types.h"
#include "adapter_config.h"
#include "class_creator.h"
#include "log.h"
#include "profile_service_manager.h"
#include "rfcomm.h"

namespace OHOS {
namespace bluetooth {
OppService::OppService() : utility::Context(PROFILE_NAME_OPP, "1.2.1")
{
    HILOGI("[OPP Service]%{public}s:%{public}s Create", PROFILE_NAME_OPP.c_str(), Name().c_str());
}

OppService::~OppService()
{
    HILOGI("[OPP Service]%{public}s:%{public}s Destroy", PROFILE_NAME_OPP.c_str(), Name().c_str());
}

utility::Context *OppService::GetContext()
{
    return this;
}

OppService *OppService::GetService()
{
    auto servManager = IProfileManager::GetInstance();
    return static_cast<OppService *>(servManager->GetProfileService(PROFILE_NAME_OPP));
}

void OppService::RegisterObserver(IOppObserver &oppObserver)
{
    HILOGI("[OPP Service] Enter");

    oppObservers_.Register(oppObserver);
}

void OppService::DeregisterObserver(IOppObserver &oppObserver)
{
    HILOGI("[OPP Service] Enter");

    oppObservers_.Deregister(oppObserver);
}

void OppService::NotifyStateChanged(const RawAddress &device, int state)
{
    HILOGI("[OPP Service] Enter newState=%{public}d",
        stateMap_.at(state));
}

void OppService::NotifyReceiveIncomingFile(IOppTransferInformation info)
{
    HILOGI("[OPP Service] Enter");

    oppObservers_.ForEach([info](IOppObserver &observer) {
        observer.OnReceiveIncomingFile(info);
    });
}

void OppService::NotifyTransferStateChanged(IOppTransferInformation info)
{
    HILOGI("[OPP Service] Enter");

    oppObservers_.ForEach([info](IOppObserver &observer) {
        observer.OnTransferStateChange(info);
    });
}

void OppService::Enable(void)
{
    HILOGI("[OPP Service] Enter");

    OppMessage event(OPP_SERVICE_STARTUP_EVT);
    PostEvent(event);
}

void OppService::Disable(void)
{
    HILOGI("[OPP Service] Enter");

    OppMessage event(OPP_SERVICE_SHUTDOWN_EVT);
    PostEvent(event);
}

void OppService::StartUp()
{
    HILOGI("[OPP Service]:==========<start>==========");
    if (isStarted_) {
        GetContext()->OnEnable(PROFILE_NAME_OPP, true);
        HILOGW("[OPP Service]:OppService has already been started before.");
        return;
    }
    LoadOppConfig();
    uint8_t rfcommScn = RFCOMM_AssignServerNum();

    oppSdpServer_ = std::make_unique<OppSdpServer>();
    oppGapServer_ = std::make_unique<OppGapServer>(rfcommScn, OPP_GOEP_L2CAP_PSM);
    ObexServerConfig config;
    config.useL2cap_ = true;
    config.l2capPsm_ = OPP_GOEP_L2CAP_PSM;
    config.l2capMtu_ = oppConfig_.l2capMtu;
    config.useRfcomm_ = true;
    config.rfcommScn_ = rfcommScn;
    config.rfcommMtu_ = oppConfig_.rfcommMtu;
    config.isSupportSrm_ = true;
    config.isSupportReliableSession_ = false;
    oppObexServer_ = std::make_unique<OppObexServer>(config, *GetDispatcher());
    if ((oppSdpServer_->Register(rfcommScn, OPP_GOEP_L2CAP_PSM) == BT_NO_ERROR) &&
        (oppGapServer_->Register() == BT_NO_ERROR) &&
        (oppObexServer_->StartUp() == BT_NO_ERROR)) {
        GetContext()->OnEnable(PROFILE_NAME_OPP, true);
        isStarted_ = true;
        HILOGI("[OPP Service]:PanService started");
    } else {
        oppSdpServer_ = nullptr;
        oppObexServer_ = nullptr;
        GetContext()->OnEnable(PROFILE_NAME_OPP, false);
        HILOGE("[OPP Service]:Sdp register or obex start failed!");
    }
}

void OppService::ShutDown()
{
    HILOGI("[OPP Service]:==========<start>==========");
    if (!isStarted_) {
        GetContext()->OnDisable(PROFILE_NAME_OPP, true);
        HILOGW("[OPP Service]:OppService has already been shutdown before.");
        return;
    }

    while (!oppTransferList_.empty()) {
        oppTransferList_.front()->OnTransferStateChange(OPP_TRANSFER_STATUS_FAILD, OPP_TRANSFER_FAILED_PROTOCOL);
        oppTransferList_.pop_front();
    }

    isShuttingDown_ = true;
    bool isDisconnected = false;
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if ((it->second != nullptr) && (it->second->GetDeviceStateInt() > OPP_STATE_DISCONNECTED)) {
            Disconnect(RawAddress(it->first));
            isDisconnected = true;
        }
    }

    if (!isDisconnected) {
        ShutDownDone(true);
    }
}

void OppService::ShutDownDone(bool isAllDisconnected)
{
    HILOGI("[OPP Service]:==========<start>==========");
    if (!isAllDisconnected) {
        for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
            if ((it->second != nullptr) && (it->second->GetDeviceStateInt() > OPP_STATE_DISCONNECTED)) {
                return;
            }
        }
    }

    stateMachines_.clear();
    if (oppSdpServer_) {
        oppSdpServer_->Deregister();
    }
    oppSdpServer_ = nullptr;
    if (oppGapServer_) {
        oppGapServer_->Deregister();
    }
    oppGapServer_ = nullptr;
    oppObexServer_->ShutDown();
    oppObexServer_ = nullptr;

    GetContext()->OnDisable(PROFILE_NAME_OPP, true);
    isStarted_ = false;
    HILOGI("[OPP Service]:OppService shutdown");
    isShuttingDown_ = false;
}

int OppService::SendFile(const RawAddress &device, const std::vector<std::string> filePaths,
    const std::vector<std::string> mimeTypes)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    HILOGI("[OPP Service] Enter");
    std::string address = device.GetAddress();

    std::shared_ptr<OppTransfer> oppTransfer = std::make_shared<OppTransfer>(
        address, filePaths, mimeTypes, OPP_TRANSFER_DIRECTION_OUTBOND);
    if (oppTransfer->GetFileNumber() <= 0) {
        HILOGE("[OPP Service]:Send file size is 0");
        return RET_BAD_STATUS;
    }
    oppTransferList_.push_back(oppTransfer);

    if (oppTransferList_.size() == 1) {
        OppMessage event(OPP_CONNECT_REQ_EVT);
        event.dev_ = address;
        PostEvent(event);
    }
    return RET_NO_ERROR;
}

int OppService::SetIncomingFileConfirmation(const bool accept)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    HILOGI("[OPP Service] Enter");

    if (!oppTransferList_.empty()) {
        return oppTransferList_.front()->SetIncomingFileConfirmation(accept);
    }
    return RET_BAD_STATUS;
}

IOppTransferInformation OppService::GetCurrentTransferInformation()
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    HILOGI("[OPP Service] Enter");

    if (!oppTransferList_.empty()) {
        return oppTransferList_.front()->GetCurrentTransferInformation();
    }
    IOppTransferInformation ret;
    HILOGE("[OPP Service]:Current transfer is null");
    return ret;
}

int OppService::CancelTransfer()
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    HILOGI("[OPP Service] Enter");

    if (!oppTransferList_.empty()) {
        return oppTransferList_.front()->CancelTransfer();
    }
    return RET_BAD_STATUS;
}

int OppService::CancelTransfer(const std::string &device)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    HILOGI("[OPP Service] Enter");

    if (!oppTransferList_.empty()) {
        if (oppTransferList_.front()->GetDeviceAddress() == device) {
            return oppTransferList_.front()->CancelTransfer();
        } else {
            HILOGE("[OPP Service]:not current device");
        }
    }
    return RET_BAD_STATUS;
}

void OppService::ConnectObex(const std::string &device, const ObexClientConfig &config)
{
    if (!oppTransferList_.empty()) {
        if (oppTransferList_.front()->GetDeviceAddress() == device) {
            oppTransferList_.front()->ConnectObex(config, *GetDispatcher());
        } else {
            HILOGE("[OPP Service]:not current device");
        }
    } else {
        HILOGE("[OPP Service]:not find transfer");
    }
}

void OppService::DisconnectObex(const std::string &device)
{
    if (!oppTransferList_.empty()) {
        if (oppTransferList_.front()->GetDeviceAddress() == device) {
            oppTransferList_.front()->DisconnectObex();
        } else {
            HILOGE("[OPP Service]:not current device");
        }
    } else {
        HILOGE("[OPP Service]:not find transfer");
    }
}

void OppService::OnReceiveIncomingConnect(ObexServerSession &session, uint32_t connectId)
{
    HILOGI("[OPP Service] is inbond,creat a transfer");
    for (std::list<std::shared_ptr<OppTransfer>>::iterator iter = oppTransferList_.begin();
        iter != oppTransferList_.end(); iter++) {
        if ((*iter)->GetDirection() == OPP_TRANSFER_DIRECTION_INBOND) {
            HILOGE("[OPP Service]:Had a inbond connect,reject incoming connect");
            auto header = ObexHeader::CreateResponse(ObexRspCode::SERVICE_UNAVAILABLE, true);
            session.SendResponse(*header);
            session.Disconnect();
            return;
        }
    }
    std::vector<std::string> filePaths;
    std::vector<std::string> mimeTypes;
    std::shared_ptr<OppTransfer> oppTransfer = std::make_shared<OppTransfer>(
        session.GetRemoteAddr().GetAddress(), filePaths, mimeTypes, OPP_TRANSFER_DIRECTION_INBOND);
    oppTransfer->OnReceiveIncomingConnect(session, connectId);
    oppTransferList_.push_back(oppTransfer);

    if (oppTransferList_.size() == 1) {
        oppTransferList_.front()->AcceptConnect();
    }
}

void OppService::OnReceiveIncomingFile(IOppTransferInformation info)
{
    if (!oppTransferList_.empty() && (oppTransferList_.front()->GetDeviceAddress() == info.GetDeviceAddress())) {
        oppTransferList_.front()->OnReceiveIncomingFile(info);
    } else {
        HILOGE("[OPP Service]:not current device");
    }
}

void OppService::OnObexConnected(const std::string &device)
{
    int ret = RET_NO_ERROR;
    if (!oppTransferList_.empty() && (oppTransferList_.front()->GetDeviceAddress() == device)) {
        ret = oppTransferList_.front()->StartTransfer();
        if (ret != RET_NO_ERROR) {
            oppTransferList_.front()->OnTransferStateChange(
                OPP_TRANSFER_STATUS_FAILD, OPP_TRANSFER_FAILED_PROTOCOL);
        }
    } else {
        HILOGE("[OPP Service]:not current device");
    }
}

void OppService::OnObexDisconnected(const std::string &device)
{
    if (oppTransferList_.empty()) {
        HILOGE("[OPP Service]:Transfer list_ is empty");
        return;
    }
    std::list<std::shared_ptr<OppTransfer>>::iterator iter = oppTransferList_.begin();
    for (; iter != oppTransferList_.end(); iter++) {
        if ((*iter)->GetDeviceAddress() == device) {
            break;
        }
    }
    if (iter == oppTransferList_.end()) {
        HILOGE("[OPP Service]:Not find transfer");
        return;
    }
    if (iter == oppTransferList_.begin()) {
        GetDispatcher()->PostTask(std::bind(&OppService::StartNextTransfer, this));
    }
    (*iter)->OnObexDisconnected();
    oppTransferList_.erase(iter);
}

void OppService::OnTransferStateChange(const std::string &device, int state, int reason)
{
    if (oppTransferList_.empty()) {
        HILOGE("[OPP Service]:Transfer list_ is empty");
        return;
    }
    std::list<std::shared_ptr<OppTransfer>>::iterator iter = oppTransferList_.begin();
    for (; iter != oppTransferList_.end(); iter++) {
        if ((*iter)->GetDeviceAddress() == device) {
            break;
        }
    }
    if (iter == oppTransferList_.end()) {
        HILOGE("[OPP Service]:Not find transfer");
        return;
    }
    (*iter)->OnTransferStateChange(state, reason);
}

void OppService::OnTransferPositionChange(const std::string &device, size_t position)
{
    if (oppTransferList_.empty()) {
        HILOGE("[OPP Service]:Transfer list_ is empty");
        return;
    }
    std::list<std::shared_ptr<OppTransfer>>::iterator iter = oppTransferList_.begin();
    for (; iter != oppTransferList_.end(); iter++) {
        if ((*iter)->GetDeviceAddress() == device) {
            break;
        }
    }
    if (iter == oppTransferList_.end()) {
        HILOGE("[OPP Service]:Not find transfer");
        return;
    }
    (*iter)->OnTransferPositionChange(position);
}

void OppService::StartNextTransfer()
{
    if (oppTransferList_.size() > 0) {
        if (oppTransferList_.front()->GetDirection() == OPP_TRANSFER_DIRECTION_OUTBOND) {
            OppMessage event(OPP_CONNECT_REQ_EVT);
            event.dev_ = oppTransferList_.front()->GetDeviceAddress();
            PostEvent(event);
        } else {
            oppTransferList_.front()->AcceptConnect();
        }
    } else {
        HILOGI("[OPP Service] No more file to transfer");
    }
}

void OppService::RemoveStateMachine(const std::string &device)
{
    OppMessage event(OPP_REMOVE_STATE_MACHINE_EVT);
    event.dev_ = device;
    PostEvent(event);
}

void OppService::PostEvent(const OppMessage &event)
{
    GetDispatcher()->PostTask(std::bind(&OppService::ProcessEvent, this, event));
}

void OppService::ProcessEvent(const OppMessage &event)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = event.dev_;
    HILOGI("[OPP Service]:address[%{public}s] event_no[%{public}d]",
        address.c_str(), event.what_);
    switch (event.what_) {
        case OPP_SERVICE_STARTUP_EVT:
            StartUp();
            break;
        case OPP_SERVICE_SHUTDOWN_EVT:
            ShutDown();
            break;
        case OPP_CONNECT_REQ_EVT:
        case OPP_CONNECTED_EVT:
            ProcessConnectEvent(event);
            break;
        case OPP_REMOVE_STATE_MACHINE_EVT:
            ProcessRemoveStateMachine(event.dev_);
            break;
        default:
            ProcessDefaultEvent(event);
            break;
    }
}

void OppService::ProcessConnectEvent(const OppMessage &event)
{
    auto it = stateMachines_.find(event.dev_);
    if (it != stateMachines_.end() && it->second != nullptr && it->second->IsRemoving()) {
        // peer device may send connect request before we remove statemachine for last connection.
        // so post this connect request, process it after we remove statemachine completely.
        PostEvent(event);
    } else if (it == stateMachines_.end() || it->second == nullptr) {
        stateMachines_[event.dev_] = std::make_unique<OppStateMachine>(event.dev_);
        stateMachines_[event.dev_]->Init();
        stateMachines_[event.dev_]->ProcessMessage(event);
    } else {
        it->second->ProcessMessage(event);
    }
}

void OppService::ProcessRemoveStateMachine(const std::string &address)
{
    stateMachines_.erase(address);
    if (isShuttingDown_) {
        ShutDownDone(false);
    }
}

void OppService::ProcessDefaultEvent(const OppMessage &event) const
{
    auto it = stateMachines_.find(event.dev_);
    if ((it != stateMachines_.end()) && (it->second != nullptr)) {
        it->second->ProcessMessage(event);
    } else {
        HILOGE("[OPP Service]:invalid address[%{public}s]", event.dev_.c_str());
    }
}

void OppService::LoadOppConfig()
{
    IAdapterConfig *adpterConfig = AdapterConfig::GetInstance();
    if (!adpterConfig->GetValue(SECTION_OPP_SERVICE, PROPERTY_RFCOMM_MTU, oppConfig_.rfcommMtu)) {
        LOG_ERROR("[OPP Service]%{public}s():Load config %{public}s failure",
            __FUNCTION__, PROPERTY_RFCOMM_MTU.c_str());
        oppConfig_.rfcommMtu = OBEX_DEFAULT_MTU;
    }
    if (!adpterConfig->GetValue(SECTION_OPP_SERVICE, PROPERTY_L2CAP_MTU, oppConfig_.l2capMtu)) {
        LOG_ERROR("[OPP Service]%{public}s():Load config %{public}s failure",
            __FUNCTION__, PROPERTY_L2CAP_MTU.c_str());
        oppConfig_.l2capMtu = OBEX_DEFAULT_MTU;
    }
    LOG_INFO("[OPP Service]%{public}s():rfcommMtu = 0x%X, l2capMtu = 0x%X",
        __FUNCTION__, oppConfig_.rfcommMtu, oppConfig_.l2capMtu);
}

OppConfig &OppService::GetOppConfig()
{
    return oppConfig_;
}

int OppService::Connect(const RawAddress &device)
{
    HILOGI("[OPP Service] Enter");
    // DO NOTHING
    return RET_NO_ERROR;
}

int OppService::Disconnect(const RawAddress &device)
{
    HILOGI("[OPP Service] Enter");
    // DO NOTHING
    return RET_NO_ERROR;
}

std::list<RawAddress> OppService::GetConnectDevices(void)
{
    HILOGI("[OPP Service] Enter");
    std::list<RawAddress> devList;
    return devList;
}

std::vector<RawAddress> OppService::GetDevicesByStates(std::vector<int> states)
{
    HILOGI("[OPP Service] Enter");
    std::vector<RawAddress> devList;
    return devList;
}

int OppService::GetDeviceState(const RawAddress &device)
{
    HILOGI("[OPP Service] Enter");
    return PROFILE_STATE_DISCONNECTED;
}

int OppService::GetConnectState(void)
{
    HILOGI("[OPP Service] Enter");
    return PROFILE_STATE_DISCONNECTED;
}

int OppService::GetMaxConnectNum(void)
{
    HILOGI("[OPP Service] Enter");
    return 0;
}

REGISTER_CLASS_CREATOR(OppService);
}  // namespace bluetooth
}  // namespace OHOS