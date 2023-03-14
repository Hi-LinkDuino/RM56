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

#include "pan_statemachine.h"
#include "pan_service.h"

namespace OHOS {
namespace bluetooth {
PanStateMachine::PanStateMachine(const std::string &address)
    : address_(address), panBnep_(address)
{}

void PanStateMachine::Init()
{
    connTimer_ = std::make_unique<utility::Timer>(
        std::bind(&bluetooth::PanStateMachine::ConnectionTimeout, this));
    disconnTimer_ = std::make_unique<utility::Timer>(
        std::bind(&bluetooth::PanStateMachine::DisonnectionTimeout, this));

    std::unique_ptr<utility::StateMachine::State> disconnectedState =
        std::make_unique<PanDisconnectedState>(DISCONNECTED, *this);
    std::unique_ptr<utility::StateMachine::State> connectingState =
        std::make_unique<PanConnectingState>(CONNECTING, *this);
    std::unique_ptr<utility::StateMachine::State> disconnectingState =
        std::make_unique<PanDisconnectingState>(DISCONNECTING, *this);
    std::unique_ptr<utility::StateMachine::State> connectedState =
        std::make_unique<PanConnectedState>(CONNECTED, *this);

    Move(disconnectedState);
    Move(connectingState);
    Move(disconnectingState);
    Move(connectedState);

    InitState(DISCONNECTED);
}

bool PanStateMachine::IsRemoving() const
{
    return isRemoving_;
}

void PanStateMachine::SetRemoving(bool isRemoving)
{
    isRemoving_ = isRemoving;
}

uint16_t PanStateMachine::GetDeviceLcid()
{
    return panBnep_.GetLcid();
}

void PanStateMachine::ProcessL2capConnectionEvent(
    const PanMessage &event)
{
    LOG_DEBUG("[PAN Machine]%{public}s():event[%{public}s]", __FUNCTION__,
        PanStateMachine::GetEventName(event.what_).c_str());
    panBnep_.ProcessEvent(event);
}

std::string PanStateMachine::GetDeviceAdress()
{
    return address_;
}

void PanDisconnectedState::Entry()
{
    stateMachine_.ProcessDeferredMessage();

    if (isReentry_) {
        stateMachine_.SetRemoving(true);
        PanService *service = PanService::GetService();
        if (service != nullptr) {
            service->RemoveStateMachine(stateMachine_.GetDeviceAdress());
        } else {
            LOG_ERROR("[PAN Machine]%{public}s():PanService is nullptr!", __FUNCTION__);
        }
        stateMachine_.NotifyStateTransitions();
    }
}

void PanDisconnectedState::Exit()
{
    isReentry_ = true;
}

bool PanDisconnectedState::Dispatch(const utility::Message &msg)
{
    PanMessage &event = (PanMessage &)msg;
    LOG_DEBUG("[PAN Machine]%{public}s():[Disconnected][%{public}s]",
        __FUNCTION__, PanStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case PAN_INT_OPEN_EVT:
            stateMachine_.ProcessOpenEvent(event);
            Transition(PanStateMachine::CONNECTING);
            break;
        case PAN_OPEN_CMPL_EVT:
            Transition(PanStateMachine::CONNECTED);
            break;
        default:
            break;
    }
    return true;
}


void PanConnectingState::Entry()
{
    stateMachine_.NotifyStateTransitions();
    stateMachine_.StartConnectionTimer();
}
void PanConnectingState::Exit()
{
    stateMachine_.StopConnectionTimer();
}

bool PanConnectingState::Dispatch(const utility::Message &msg)
{
    PanMessage &event = (PanMessage &)msg;
    LOG_DEBUG("[PAN Machine]%{public}s():[Connecting][%{public}s]", __FUNCTION__,
        PanStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case PAN_API_CLOSE_EVT:
            stateMachine_.AddDeferredMessage(event);
            break;
        case PAN_INT_OPEN_EVT:
            stateMachine_.ProcessOpenEvent(event);
            Transition(PanStateMachine::CONNECTING);
            break;
        case PAN_INT_CLOSE_EVT:
            stateMachine_.ProcessCloseEvent(event);
            Transition(PanStateMachine::DISCONNECTED);
            break;
        case PAN_OPEN_CMPL_EVT:
            stateMachine_.ProcessOpenComplete(event);
            Transition(PanStateMachine::CONNECTED);
            break;
        default:
            break;
    }
    return true;
}

void PanDisconnectingState::Entry()
{
    stateMachine_.NotifyStateTransitions();
    stateMachine_.StartDisconnectionTimer();
}

void PanDisconnectingState::Exit()
{
    stateMachine_.StopDisconnectionTimer();
}

bool PanDisconnectingState::Dispatch(const utility::Message &msg)
{
    PanMessage &event = (PanMessage &)msg;
    LOG_DEBUG("[PAN Machine]%{public}s():[Disconnecting][%{public}s]", __FUNCTION__,
        PanStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case PAN_INT_OPEN_EVT:
            stateMachine_.ProcessOpenEvent(event);
            Transition(PanStateMachine::CONNECTING);
            break;
        case PAN_INT_CLOSE_EVT:
            stateMachine_.ProcessCloseEvent(event);
            Transition(PanStateMachine::DISCONNECTED);
            break;
        case PAN_OPEN_CMPL_EVT:
            Transition(PanStateMachine::CONNECTED);
            break;
        case PAN_DISCONNECTION_TIMEOUT_EVT:
            Transition(PanStateMachine::CONNECTED);
            break;
        default:
            break;
    }
    return true;
}

void PanConnectedState::Entry()
{
    stateMachine_.ProcessDeferredMessage();
    stateMachine_.NotifyStateTransitions();
}

void PanConnectedState::Exit()
{
}

bool PanConnectedState::Dispatch(const utility::Message &msg)
{
    PanMessage &event = (PanMessage &)msg;
    LOG_DEBUG("[PAN Machine]%{public}s():[Connected][%{public}s]", __FUNCTION__,
        PanStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case PAN_API_CLOSE_EVT:
            stateMachine_.ProcessCloseReqEvent(event);
            Transition(PanStateMachine::DISCONNECTING);
            break;
        case PAN_INT_CLOSE_EVT:
            stateMachine_.ProcessCloseEvent(event);
            Transition(PanStateMachine::DISCONNECTED);
            break;
        case PAN_API_WRITE_DATA_EVT:
            stateMachine_.ProcessSendData(event);
            break;
        case PAN_INT_DATA_EVT:
            stateMachine_.ProcessReciveData(event);
            break;
        default:
            break;
    }
    return true;
}

void PanStateMachine::ProcessOpenEvent(const PanMessage &msg)
{
    PanMessage event(PAN_OPEN_CMPL_EVT);
    event.dev_ = address_;
    PanService::GetService()->PostEvent(event);
}

void PanStateMachine::ProcessCloseReqEvent(const PanMessage &msg)
{
    panBnep_.Disconnect();
}

void PanStateMachine::ProcessCloseEvent(const PanMessage &msg)
{
    PanService::GetService()->CloseNetwork(address_);
}

void PanStateMachine::ProcessReciveData(const PanMessage &msg)
{
    uint8_t* data = nullptr;
    int dataLength = 0;
    if (msg.data_ != nullptr) {
        data = msg.data_->get();
        dataLength = msg.dataLength_;
    }
    PanService::GetService()->WriteNetworkData(address_, msg.ethernetHeader_, data, dataLength);
}

void PanStateMachine::ProcessSendData(const PanMessage &msg)
{
    uint8_t* data = nullptr;
    int dataLength = 0;
    if (msg.data_ != nullptr) {
        data = msg.data_->get();
        dataLength = msg.dataLength_;
    }
    panBnep_.SendData(msg.ethernetHeader_, data, dataLength);
}

void PanStateMachine::ProcessOpenComplete(const PanMessage &msg)
{
    if (!PanService::GetService()->IsTetheringOn()) {
        PanMessage event(PAN_API_CLOSE_EVT);
        event.dev_ = address_;
        PanService::GetService()->PostEvent(event);
        return;
    }
    PanService::GetService()->OpenNetwork();
}

int PanStateMachine::GetDeviceStateInt() const
{
    return static_cast<const PanState*>(GetState())->GetStateInt();
}

void PanStateMachine::StartConnectionTimer() const
{
    connTimer_->Start(CONNECTION_TIMEOUT_MS);
    LOG_INFO("[PAN Machine]%{public}s():Start connection timer!", __FUNCTION__);
}

void PanStateMachine::StopConnectionTimer() const
{
    connTimer_->Stop();
    LOG_INFO("[PAN Machine]%{public}s():Stop connection timer!", __FUNCTION__);
}

void PanStateMachine::ConnectionTimeout() const
{
    PanMessage event(PAN_CONNECTION_TIMEOUT_EVT);
    event.dev_ = address_;
    PanService::GetService()->PostEvent(event);
}

void PanStateMachine::StartDisconnectionTimer() const
{
    disconnTimer_->Start(DISCONNECTION_TIMEOUT_MS);
    LOG_INFO("[PAN Machine]%{public}s():Start disconnection timer!", __FUNCTION__);
}

void PanStateMachine::StopDisconnectionTimer() const
{
    disconnTimer_->Stop();
    LOG_INFO("[PAN Machine]%{public}s():Stop disconnection timer!", __FUNCTION__);
}

void PanStateMachine::DisonnectionTimeout() const
{
    PanMessage event(PAN_DISCONNECTION_TIMEOUT_EVT);
    event.dev_ = address_;
    PanService::GetService()->PostEvent(event);
}

std::string PanStateMachine::GetEventName(int what)
{
    switch (what) {
        case PAN_API_CLOSE_EVT:
            return "PAN_API_CLOSE_EVENT";
        case PAN_INT_OPEN_EVT:
            return "PAN_INT_OPEN_EVENT";
        case PAN_INT_CLOSE_EVT:
            return "PAN_INT_CLOSE_EVENT";
        case PAN_OPEN_CMPL_EVT:
            return "PAN_OPEN_CMPL_EVENT";
        case PAN_API_WRITE_DATA_EVT:
            return "PAN_API_WRITE_DATA_EVENT";
        case PAN_INT_DATA_EVT:
            return "PAN_INT_DATA_EVENT";
        case PAN_DISCONNECTION_TIMEOUT_EVT:
            return "PAN_DISCONNECTION_TIMEOUT_EVT";
        default:
            return "Unknown";
    }
}

void PanStateMachine::NotifyStateTransitions()
{
    PanService *service = PanService::GetService();
    int toState = GetDeviceStateInt();
    if (service != nullptr) {
        RawAddress device(address_);
        if ((preState_ != toState) && (preState_ <= PAN_STATE_CONNECTED)
            && (toState <= PAN_STATE_CONNECTED)) {
            service->NotifyStateChanged(device, toState);
        }
    }

    preState_ = toState;
}

void PanStateMachine::AddDeferredMessage(const PanMessage &msg)
{
    deferMsgs_.push_back(msg);
}

void PanStateMachine::ProcessDeferredMessage()
{
    auto size = deferMsgs_.size();
    while (size-- > 0 && !deferMsgs_.empty()) {
        PanMessage event = deferMsgs_.front();
        deferMsgs_.pop_front();
        PanService::GetService()->PostEvent(event);
    }
}
}  // namespace bluetooth
}  // namespace OHOS
