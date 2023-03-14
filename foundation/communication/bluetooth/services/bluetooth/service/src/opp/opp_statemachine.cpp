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

#include "opp_statemachine.h"

#include "log.h"
#include "opp_service.h"

namespace OHOS {
namespace bluetooth {
OppStateMachine::OppStateMachine(const std::string &address)
    : address_(address)
{}

void OppStateMachine::Init()
{
    connTimer_ = std::make_unique<utility::Timer>(
        std::bind(&bluetooth::OppStateMachine::ConnectionTimeout, this));
    disconnTimer_ = std::make_unique<utility::Timer>(
        std::bind(&bluetooth::OppStateMachine::DisonnectionTimeout, this));

    std::unique_ptr<utility::StateMachine::State> disconnectedState =
        std::make_unique<OppDisconnectedState>(DISCONNECTED, *this);
    std::unique_ptr<utility::StateMachine::State> connectingState =
        std::make_unique<OppConnectingState>(CONNECTING, *this);
    std::unique_ptr<utility::StateMachine::State> disconnectingState =
        std::make_unique<OppDisconnectingState>(DISCONNECTING, *this);
    std::unique_ptr<utility::StateMachine::State> connectedState =
        std::make_unique<OppConnectedState>(CONNECTED, *this);

    Move(disconnectedState);
    Move(connectingState);
    Move(disconnectingState);
    Move(connectedState);

    InitState(DISCONNECTED);
}

bool OppStateMachine::IsRemoving() const
{
    return isRemoving_;
}

void OppStateMachine::SetRemoving(bool isRemoving)
{
    isRemoving_ = isRemoving;
}

std::string OppStateMachine::GetDeviceAdress()
{
    return address_;
}

void OppDisconnectedState::Entry()
{
    if (isReentry_) {
        stateMachine_.SetRemoving(true);
        OppService *service = OppService::GetService();
        if (service != nullptr) {
            service->RemoveStateMachine(stateMachine_.GetDeviceAdress());
        } else {
            HILOGE("[OPP Machine]:OppService is nullptr!");
        }
        stateMachine_.NotifyStateTransitions();
    }
}

void OppDisconnectedState::Exit()
{
    isReentry_ = true;
}

bool OppDisconnectedState::Dispatch(const utility::Message &msg)
{
    OppMessage &event = (OppMessage &)msg;
    HILOGI("[OPP Machine]:[Disconnected][%{public}s]",
        OppStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case OPP_CONNECT_REQ_EVT:
            stateMachine_.ProcessConnectReqEvent(event);
            Transition(OppStateMachine::CONNECTING);
            break;
        case OPP_CONNECTED_EVT:
            stateMachine_.ProcessConnectedEvent(event);
            Transition(OppStateMachine::CONNECTED);
            break;
        default:
            break;
    }
    return true;
}

void OppConnectingState::Entry()
{
    stateMachine_.NotifyStateTransitions();
    stateMachine_.StartConnectionTimer();
}
void OppConnectingState::Exit()
{
    stateMachine_.StopConnectionTimer();
}

bool OppConnectingState::Dispatch(const utility::Message &msg)
{
    OppMessage &event = (OppMessage &)msg;
    HILOGI("[OPP Machine]:[Connecting][%{public}s]",
        OppStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case OPP_DISCONNECT_REQ_EVT:
            stateMachine_.ProcessDisconnectReqEvent(event);
            Transition(OppStateMachine::DISCONNECTING);
            break;
        case OPP_CONNECTED_EVT:
            stateMachine_.ProcessConnectedEvent(event);
            Transition(OppStateMachine::CONNECTED);
            break;
        case OPP_DISCONNECTED_EVT:
            stateMachine_.ProcessDisconnectedEvent(event);
            Transition(OppStateMachine::DISCONNECTED);
            break;
        case OPP_SDP_CMPL_EVT:
            stateMachine_.ProcessSdpCompleteEvent(event);
            break;
        case OPP_GAP_CMPL_EVT:
            stateMachine_.ProcessGapCompleteEvent(event);
            break;
        case OPP_CONNECTION_TIMEOUT_EVT:
            stateMachine_.ProcessDisconnectedEvent(event);
            Transition(OppStateMachine::DISCONNECTED);
            break;
        default:
            break;
    }
    return true;
}

void OppDisconnectingState::Entry()
{
    stateMachine_.NotifyStateTransitions();
    stateMachine_.StartDisconnectionTimer();
}

void OppDisconnectingState::Exit()
{
    stateMachine_.StopDisconnectionTimer();
}

bool OppDisconnectingState::Dispatch(const utility::Message &msg)
{
    OppMessage &event = (OppMessage &)msg;
    HILOGI("[OPP Machine]:[Disconnecting][%{public}s]",
        OppStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case OPP_CONNECTED_EVT:
            stateMachine_.ProcessConnectedEvent(event);
            Transition(OppStateMachine::CONNECTED);
            break;
        case OPP_DISCONNECTED_EVT:
            stateMachine_.ProcessDisconnectedEvent(event);
            Transition(OppStateMachine::DISCONNECTED);
            break;
        case OPP_DISCONNECTION_TIMEOUT_EVT:
            stateMachine_.ProcessDisconnectedEvent(event);
            Transition(OppStateMachine::DISCONNECTED);
            break;
        default:
            break;
    }
    return true;
}

void OppConnectedState::Entry()
{
    stateMachine_.NotifyStateTransitions();
}

void OppConnectedState::Exit()
{
}

bool OppConnectedState::Dispatch(const utility::Message &msg)
{
    OppMessage &event = (OppMessage &)msg;
    HILOGI("[OPP Machine]:[Connected][%{public}s]",
        OppStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case OPP_DISCONNECT_REQ_EVT:
            stateMachine_.ProcessDisconnectReqEvent(event);
            Transition(OppStateMachine::DISCONNECTING);
            break;
        case OPP_DISCONNECTED_EVT:
            stateMachine_.ProcessDisconnectedEvent(event);
            Transition(OppStateMachine::DISCONNECTED);
            break;
        default:
            break;
    }
    return true;
}

void OppStateMachine::ProcessConnectReqEvent(const OppMessage &msg)
{
    if (sdpClient_ == nullptr) {
        sdpClient_ = std::make_unique<OppSdpClient>(address_);
    }
    sdpClient_->SdpSearch();
}

void OppStateMachine::ProcessDisconnectReqEvent(const OppMessage &msg)
{
    OppService::GetService()->DisconnectObex(address_);
}

void OppStateMachine::ProcessSdpCompleteEvent(const OppMessage &msg)
{
    if (msg.arg1_ == OPP_SDP_SUCCESS) {
        GapSecChannel gapSecChannel;
        RawAddress(address_).ConvertToUint8(obexConfig_.addr_.addr);
        obexConfig_.addr_.type = BT_PUBLIC_DEVICE_ADDRESS;
        obexConfig_.lpsm_ = OPP_GOEP_L2CAP_PSM;
        obexConfig_.isSupportReliableSession_ = false;
        if (msg.sdpInfo_.psm > 0) {
            obexConfig_.isGoepL2capPSM_ = true;
            obexConfig_.scn_ = msg.sdpInfo_.psm;
            obexConfig_.isSupportSrm_ = true;
            obexConfig_.mtu_ = OppService::GetService()->GetOppConfig().l2capMtu;
            gapSecChannel.l2capPsm = obexConfig_.scn_;
        } else {
            obexConfig_.scn_ = msg.sdpInfo_.rfcommNo;
            obexConfig_.mtu_ = OppService::GetService()->GetOppConfig().rfcommMtu;
            gapSecChannel.rfcommChannel = obexConfig_.scn_;
        }
        gapClient_ = std::make_unique<OppGapClient>(address_, gapSecChannel, obexConfig_.isGoepL2capPSM_);
        if ((gapClient_->Register() != BT_NO_ERROR) || (gapClient_->RequestSecurity() != BT_NO_ERROR)) {
            OppMessage event(OPP_DISCONNECTED_EVT);
            event.dev_ = address_;
            OppService::GetService()->PostEvent(event);
        }
    } else {
        OppMessage event(OPP_DISCONNECTED_EVT);
        event.dev_ = address_;
        OppService::GetService()->PostEvent(event);
    }
}

void OppStateMachine::ProcessGapCompleteEvent(const OppMessage &msg)
{
    if (msg.arg1_ == OPP_GAP_SUCCESS) {
        OppService::GetService()->ConnectObex(address_, obexConfig_);
    } else {
        OppMessage event(OPP_DISCONNECTED_EVT);
        event.dev_ = address_;
        OppService::GetService()->PostEvent(event);
    }
}

void OppStateMachine::ProcessConnectedEvent(const OppMessage &msg)
{
    OppService::GetService()->OnObexConnected(address_);
}

void OppStateMachine::ProcessDisconnectedEvent(const OppMessage &msg)
{
    OppService::GetService()->OnObexDisconnected(address_);
}

int OppStateMachine::GetDeviceStateInt() const
{
    return static_cast<const OppState*>(GetState())->GetStateInt();
}

void OppStateMachine::StartConnectionTimer() const
{
    connTimer_->Start(connectionTimeoutMs);
    HILOGI("[OPP Machine]:Start connection timer!");
}

void OppStateMachine::StopConnectionTimer() const
{
    connTimer_->Stop();
    HILOGI("[OPP Machine]:Stop connection timer!");
}

void OppStateMachine::ConnectionTimeout() const
{
    OppMessage event(OPP_CONNECTION_TIMEOUT_EVT);
    event.dev_ = address_;
    OppService::GetService()->PostEvent(event);
}

void OppStateMachine::StartDisconnectionTimer() const
{
    disconnTimer_->Start(disconnectionTimeoutMs);
    HILOGI("[OPP Machine]:Start disconnection timer!");
}

void OppStateMachine::StopDisconnectionTimer() const
{
    disconnTimer_->Stop();
    HILOGI("[OPP Machine]:Stop disconnection timer!");
}

void OppStateMachine::DisonnectionTimeout() const
{
    OppMessage event(OPP_DISCONNECTION_TIMEOUT_EVT);
    event.dev_ = address_;
    OppService::GetService()->PostEvent(event);
}

std::string OppStateMachine::GetEventName(int what)
{
    switch (what) {
        case OPP_CONNECT_REQ_EVT:
            return "OPP_CONNECT_REQ_EVT";
        case OPP_DISCONNECT_REQ_EVT:
            return "OPP_DISCONNECT_REQ_EVT";
        case OPP_CONNECTED_EVT:
            return "OPP_CONNECTED_EVT";
        case OPP_DISCONNECTED_EVT:
            return "OPP_DISCONNECTED_EVT";
        case OPP_SDP_CMPL_EVT:
            return "OPP_SDP_CMPL_EVT";
        case OPP_GAP_CMPL_EVT:
            return "OPP_GAP_CMPL_EVT";
        case OPP_CONNECTION_TIMEOUT_EVT:
            return "OPP_CONNECTION_TIMEOUT_EVT";
        case OPP_DISCONNECTION_TIMEOUT_EVT:
            return "OPP_DISCONNECTION_TIMEOUT_EVT";
        default:
            return "Unknown";
    }
}

void OppStateMachine::NotifyStateTransitions()
{
    OppService *service = OppService::GetService();
    int toState = GetDeviceStateInt();
    if (service != nullptr) {
        RawAddress device(address_);
        if ((preState_ != toState) && (preState_ <= OPP_STATE_CONNECTED) &&
            (toState <= OPP_STATE_CONNECTED)) {
            service->NotifyStateChanged(device, toState);
        }
    }

    preState_ = toState;
}
}  // namespace bluetooth
}  // namespace OHOS
