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

#include "avrcp_tg_state_machine.h"
#include "avrcp_tg_pass_through.h"
#include "avrcp_tg_sub_unit_info.h"
#include "avrcp_tg_unit_info.h"
#include "avrcp_tg_vendor.h"
#include "avrcp_tg_vendor_continuation.h"
#include "avrcp_tg_vendor_player_application_settings.h"
#include "log_util.h"

namespace OHOS {
namespace bluetooth {
/// The name of state "CONNECTING".
const std::string AVRC_TG_SM_STATE_CONNECTING = "CONNECTING";
/// The name of state "CONNECTED".
const std::string AVRC_TG_SM_STATE_CONNECTED = "CONNECTED";
/// The name of state "DISCONNECTING".
const std::string AVRC_TG_SM_STATE_DISCONNECTING = "DISCONNECTING";
/// The name of state "PENDING".
const std::string AVRC_TG_SM_STATE_PENDING = "PENDING";
/// The name of state "BUSY".
const std::string AVRC_TG_SM_STATE_BUSY = "BUSY";
/// The name of state "CONTINUATION".
const std::string AVRC_TG_SM_STATE_CONTINUATION = "CONTINUATION";
/// The name of state "DISABLE".
const std::string AVRC_TG_SM_STATE_DISABLE = "DISABLE";

AvrcTgStateMachineManager *AvrcTgStateMachineManager::GetInstance(void)
{
    HILOGI("enter");

    static AvrcTgStateMachineManager instance;

    return &instance;
}

AvrcTgStateMachineManager::~AvrcTgStateMachineManager()
{
    HILOGI("enter");

    stateMachines_.clear();
}

void AvrcTgStateMachineManager::ShutDown(void)
{
    HILOGI("enter");
    std::lock_guard<std::mutex> lock(mutex_);
    stateMachines_.clear();
}

int AvrcTgStateMachineManager::AddControlStateMachine(const RawAddress &rawAddr)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    std::lock_guard<std::mutex> lock(mutex_);

    int result = RET_NO_ERROR;

    if (GetPairOfStateMachine(rawAddr.GetAddress()) == nullptr) {
        std::unique_ptr<StateMachine> statemachine =
            std::make_unique<StateMachine>(StateMachine::Type::AVRC_TG_SM_TYPE_CONTROL, rawAddr);
        statemachine->AddStates();
        statemachine->InitState(AVRC_TG_SM_STATE_CONNECTING);
        stateMachines_.insert(std::make_pair(rawAddr.GetAddress(), std::make_pair(std::move(statemachine), nullptr)));
    } else {
        result = RET_BAD_PARAM;
        HILOGI("exists! Address: %{public}s", GetEncryptAddr(rawAddr.GetAddress()).c_str());
    }

    return result;
}

int AvrcTgStateMachineManager::AddBrowseStateMachine(const RawAddress &rawAddr)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    std::lock_guard<std::mutex> lock(mutex_);

    int result = RET_NO_ERROR;

    StateMachinePair *pair = GetPairOfStateMachine(rawAddr.GetAddress());
    if (pair == nullptr) {
        result = RET_BAD_PARAM;
        HILOGI("doest not exists! Address: %{public}s", GetEncryptAddr(rawAddr.GetAddress()).c_str());
    } else if (pair->second == nullptr) {
        pair->second = std::make_unique<StateMachine>(StateMachine::Type::AVRC_TG_SM_TYPE_BROWSE, rawAddr);
        pair->second->AddStates();
        pair->second->InitState(AVRC_TG_SM_STATE_CONNECTING);
    } else {
        result = RET_BAD_PARAM;
        HILOGI("exists! Address: %{public}s", GetEncryptAddr(rawAddr.GetAddress()).c_str());
    }

    return result;
}

void AvrcTgStateMachineManager::DeletePairOfStateMachine(const RawAddress &rawAddr)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    std::lock_guard<std::mutex> lock(mutex_);

    StateMachinePair *pair = GetPairOfStateMachine(rawAddr.GetAddress());
    if (pair != nullptr) {
        stateMachines_.erase(rawAddr.GetAddress());
    } else {
        HILOGI("does not exist! Address: %{public}s", GetEncryptAddr(rawAddr.GetAddress()).c_str());
    }
}

void AvrcTgStateMachineManager::DeleteBrowseStateMachine(const RawAddress &rawAddr)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    std::lock_guard<std::mutex> lock(mutex_);

    StateMachinePair *pair = GetPairOfStateMachine(rawAddr.GetAddress());
    if (pair != nullptr && pair->second != nullptr) {
        pair->second = nullptr;
    } else {
        HILOGI("does not exist! Address: %{public}s", GetEncryptAddr(rawAddr.GetAddress()).c_str());
    }
}

bool AvrcTgStateMachineManager::SendMessageToControlStateMachine(const RawAddress &rawAddr, const utility::Message &msg)
{
    HILOGI("addr:%{public}s, msg: %{public}x", GET_ENCRYPT_AVRCP_ADDR(rawAddr), msg.what_);
    std::lock_guard<std::mutex> lock(mutex_);
    bool result = false;

    StateMachinePair *pair = GetPairOfStateMachine(rawAddr.GetAddress());
    if (pair != nullptr && pair->first != nullptr) {
        result = pair->first->ProcessMessage(msg);
        if (!result) {
            HILOGI("Unknown message: %{public}d, Address: %{public}s",
                msg.what_, GetEncryptAddr(rawAddr.GetAddress()).c_str());
        }
    } else {
        HILOGI("does not exist! Address: %{public}s", GetEncryptAddr(rawAddr.GetAddress()).c_str());
    }

    return result;
}

void AvrcTgStateMachineManager::SendMessageToAllControlStateMachine(const utility::Message &msg)
{
    HILOGI("enter, msg: %{public}x", msg.what_);
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto &pairSm : stateMachines_) {
        if (pairSm.second.first != nullptr) {
            pairSm.second.first->ProcessMessage(msg);
        }
    }
}

bool AvrcTgStateMachineManager::SendMessageToBrowseStateMachine(const RawAddress &rawAddr, const utility::Message &msg)
{
    HILOGI("addr:%{public}s, msg: %{public}x", GET_ENCRYPT_AVRCP_ADDR(rawAddr), msg.what_);
    std::lock_guard<std::mutex> lock(mutex_);
    bool result = false;

    StateMachinePair *pair = GetPairOfStateMachine(rawAddr.GetAddress());
    if (pair != nullptr && pair->second != nullptr) {
        result = pair->second->ProcessMessage(msg);
        if (!result) {
            HILOGI("Unknown message: %{public}d, Address:%{public}s",
                msg.what_, GetEncryptAddr(rawAddr.GetAddress()).c_str());
        }
    } else {
        HILOGI("does not exist! Address: %{public}s", GetEncryptAddr(rawAddr.GetAddress()).c_str());
    }

    return result;
}

void AvrcTgStateMachineManager::SendMessageToAllBrowseStateMachine(const utility::Message &msg)
{
    HILOGI("enter, msg: %{public}x", msg.what_);
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto &pairSm : stateMachines_) {
        if (pairSm.second.second != nullptr) {
            pairSm.second.second->ProcessMessage(msg);
        }
    }
}

bool AvrcTgStateMachineManager::IsControlConnectingState(const RawAddress &rawAddr)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    return IsControlSpecifiedState(rawAddr.GetAddress(), AVRC_TG_SM_STATE_CONNECTING);
}

bool AvrcTgStateMachineManager::IsControlConnectedState(const RawAddress &rawAddr)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    return IsControlSpecifiedState(rawAddr.GetAddress(), AVRC_TG_SM_STATE_CONNECTED);
}

bool AvrcTgStateMachineManager::IsControlDisconnectingState(const RawAddress &rawAddr)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    return IsControlSpecifiedState(rawAddr.GetAddress(), AVRC_TG_SM_STATE_DISCONNECTING);
}

bool AvrcTgStateMachineManager::IsControlContinuationState(const RawAddress &rawAddr)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    return IsControlSpecifiedState(rawAddr.GetAddress(), AVRC_TG_SM_STATE_CONTINUATION);
}

bool AvrcTgStateMachineManager::IsControlDisableState(const RawAddress &rawAddr)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    return IsControlSpecifiedState(rawAddr.GetAddress(), AVRC_TG_SM_STATE_DISABLE);
}

AvrcTgStateMachineManager::AvrcTgStateMachineManager()
{
    HILOGI("enter");
}

AvrcTgStateMachineManager::StateMachinePair *AvrcTgStateMachineManager::GetPairOfStateMachine(const std::string &addr)
{
    HILOGI("addr:%{public}s", GetEncryptAddr(addr).c_str());

    StateMachinePair *pair = nullptr;

    auto iter = stateMachines_.find(addr);
    if (iter != stateMachines_.end()) {
        pair = &iter->second;
    }

    return pair;
}

bool AvrcTgStateMachineManager::IsControlSpecifiedState(const std::string &addr, std::string stateName)
{
    HILOGI("addr:%{public}s, stateName:%{public}s", GetEncryptAddr(addr).c_str(), stateName.c_str());

    std::lock_guard<std::mutex> lock(mutex_);

    int result = false;

    StateMachinePair *pair = GetPairOfStateMachine(addr);
    if (pair != nullptr && pair->first != nullptr) {
        if (pair->first->GetState()->Name().compare(stateName) == 0) {
            result = true;
        }
    } else {
        HILOGI("The pair of state machines does not exist!");
    }

    return result;
}

AvrcTgStateMachineManager::StateMachine::StateMachine(Type type, const RawAddress &rawAddr)
    : type_(type), rawAddr_(rawAddr)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
}

void AvrcTgStateMachineManager::StateMachine::AddStates(void)
{
    HILOGI("enter");

    if (type_ == Type::AVRC_TG_SM_TYPE_CONTROL) {
        /// CONNECTING state.
        std::unique_ptr<utility::StateMachine::State> ctConnectingPtr =
            std::make_unique<CtConnecting>(AVRC_TG_SM_STATE_CONNECTING, *this, rawAddr_);
        /// CONNECTED state.
        std::unique_ptr<utility::StateMachine::State> ctConnectedPtr =
            std::make_unique<CtConnected>(AVRC_TG_SM_STATE_CONNECTED, *this, rawAddr_, *ctConnectingPtr.get());
        /// DISCONNECTING state.
        std::unique_ptr<utility::StateMachine::State> ctDisconnectingPtr =
            std::make_unique<CtDisconnecting>(AVRC_TG_SM_STATE_DISCONNECTING, *this, rawAddr_, *ctConnectedPtr.get());
        /// CONTINUATION state.
        std::unique_ptr<utility::StateMachine::State> ctContinuationPtr =
            std::make_unique<CtContinuation>(AVRC_TG_SM_STATE_CONTINUATION, *this, rawAddr_, *ctConnectedPtr.get());
        /// DISABLE state.
        std::unique_ptr<utility::StateMachine::State> ctDisablePtr =
            std::make_unique<CtDisable>(AVRC_TG_SM_STATE_DISABLE, *this, rawAddr_, *ctConnectedPtr.get());
        Move(ctDisconnectingPtr);
        Move(ctContinuationPtr);
        Move(ctDisablePtr);
        Move(ctConnectedPtr);
        Move(ctConnectingPtr);
    } else if (type_ == Type::AVRC_TG_SM_TYPE_BROWSE) {
        /// CONNECTING state.
        std::unique_ptr<utility::StateMachine::State> brConnectingPtr =
            std::make_unique<BrConnecting>(AVRC_TG_SM_STATE_CONNECTING, *this, rawAddr_);
        /// CONNECTED state.
        std::unique_ptr<utility::StateMachine::State> brConnectedPtr =
            std::make_unique<BrConnected>(AVRC_TG_SM_STATE_CONNECTED, *this, rawAddr_, *brConnectingPtr.get());
        /// DISCONNECTING state.
        std::unique_ptr<utility::StateMachine::State> brDisconnectingPtr =
            std::make_unique<BrDisconnecting>(AVRC_TG_SM_STATE_DISCONNECTING, *this, rawAddr_, *brConnectedPtr.get());
        Move(brDisconnectingPtr);
        Move(brConnectedPtr);
        Move(brConnectingPtr);
    } else {
        HILOGI("The type of the state machine is wrong!");
    }
}

/******************************************************************
 * AvrcTgStateMachineManager::StateMachine::State                 *
 ******************************************************************/

AvrcTgStateMachineManager::StateMachine::State::State(const std::string &name, utility::StateMachine &stateMachine,
    const RawAddress &rawAddr, utility::StateMachine::State &parent)
    : utility::StateMachine::State(name, stateMachine, parent), rawAddr_(rawAddr)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
}

AvrcTgStateMachineManager::StateMachine::State::State(
    const std::string &name, utility::StateMachine &stateMachine, const RawAddress &rawAddr)
    : utility::StateMachine::State(name, stateMachine), rawAddr_(rawAddr)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
}

AvrcTgStateMachineManager::StateMachine::State::~State()
{
    HILOGI("enter");
}

/******************************************************************
 * AvrcTgStateMachineManager::StateMachine::CtConnecting          *
 ******************************************************************/

AvrcTgStateMachineManager::StateMachine::CtConnecting::CtConnecting(
    const std::string &name, utility::StateMachine &stateMachine, const RawAddress &rawAddr)
    : State(name, stateMachine, rawAddr)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
}

void AvrcTgStateMachineManager::StateMachine::CtConnecting::Entry(void)
{
    HILOGI("enter");

    const AvrcTgConnectInfo *info = AvrcTgConnectManager::GetInstance()->GetConnectInfo(rawAddr_);

    AVCT_Register(info->controlMtu_, info->browseMtu_, AVCT_CT);

    uint8_t connectId = 0x00;
    AvctConnectParam param = {info->eventCallback_, info->msgCallback_, AVRC_TG_AV_REMOTE_CONTROL,
        info->role_, nullptr};
    BtAddr btAddr = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00};
    rawAddr_.ConvertToUint8(btAddr.addr);

    if (AVCT_ConnectReq(&connectId, &param, &btAddr) != AVCT_SUCCESS) {
        info->eventCallback_(connectId, AVCT_CONNECT_CFM_EVT, AVCT_FAILED, &btAddr, nullptr);
        HILOGI("Call AVCT_ConnectReq Failed! Address: %{public}s",
            GetEncryptAddr(rawAddr_.GetAddress()).c_str());
    }
}

void AvrcTgStateMachineManager::StateMachine::CtConnecting::Exit(void)
{
    HILOGI("enter");
}

bool AvrcTgStateMachineManager::StateMachine::CtConnecting::Dispatch(const utility::Message &msg)
{
    HILOGI("enter, msg: %{public}x", msg.what_);

    bool result = true;

    switch (msg.what_) {
        case AVRC_TG_SM_EVENT_TO_DISABLE_STATE:
            Transition(AVRC_TG_SM_STATE_DISABLE);
            break;
        case AVRC_TG_SM_EVENT_TO_CONNECTED_STATE:
            Transition(AVRC_TG_SM_STATE_CONNECTED);
            break;
        default:
            result = false;
            break;
    }

    return result;
}

/******************************************************************
 * AvrcTgStateMachineManager::StateMachine::CtConnected           *
 ******************************************************************/
AvrcTgStateMachineManager::StateMachine::CtConnected::CtConnected(const std::string &name,
    utility::StateMachine &stateMachine, const RawAddress &rawAddr, utility::StateMachine::State &parent)
    : State(name, stateMachine, rawAddr, parent)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
}

void AvrcTgStateMachineManager::StateMachine::CtConnected::Entry(void)
{
    HILOGI("enter");
}

void AvrcTgStateMachineManager::StateMachine::CtConnected::Exit(void)
{
    HILOGI("enter");
}

bool AvrcTgStateMachineManager::StateMachine::CtConnected::Dispatch(const utility::Message &msg)
{
    HILOGI("msg: %{public}x", msg.what_);

    bool result = true;
    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    if (cnManager->GetActiveDevice().compare(rawAddr_.GetAddress()) == 0x00) {
        switch (msg.what_) {
            case AVRC_TG_SM_EVENT_TO_DISCONNECTING_STATE:
                Transition(AVRC_TG_SM_STATE_DISCONNECTING);
                break;
            case AVRC_TG_SM_EVENT_PASS_THROUGH: {
                ToPassThroughEvent();
                break;
            }
            case AVRC_TG_SM_EVENT_UNIT_INFO:
            case AVRC_TG_SM_EVENT_SUB_UNIT_INFO: {
                ToUnitInfoEvent();
                break;
            }
            case AVRC_TG_PDU_ID_GENERAL_REJECT:
            case AVRC_TG_SM_EVENT_REGISTER_NOTIFICATION:
            case AVRC_TG_SM_EVENT_GET_CAPABILITIES:
            case AVRC_TG_SM_EVENT_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES:
            case AVRC_TG_SM_EVENT_LIST_PLAYER_APPLICATION_SETTING_VALUES:
            case AVRC_TG_SM_EVENT_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE:
            case AVRC_TG_SM_EVENT_SET_PLAYER_APPLICATION_SETTING_VALUE:
            case AVRC_TG_SM_EVENT_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT:
            case AVRC_TG_SM_EVENT_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT:
            case AVRC_TG_SM_EVENT_GET_ELEMENT_ATTRIBTUES:
            case AVRC_TG_SM_EVENT_GET_PLAY_STATUS:
            case AVRC_TG_SM_EVENT_SET_ADDRESSED_PLAYER:
            case AVRC_TG_SM_EVENT_PLAY_ITEM:
            case AVRC_TG_SM_EVENT_ADD_TO_NOW_PLAYING:
            case AVRC_TG_SM_EVENT_SET_ABSOLUTE_VOLUME: {
                ToCommonEvent();
                break;
            }
            case AVRC_TG_SM_EVENT_ABORT_CONTINUING_RESPONSE: {
                ToAbortContinuingEvent();
                break;
            }
            default:
                result = false;
                break;
        }
    }

    return result;
}

void AvrcTgStateMachineManager::StateMachine::CtConnected::ToPassThroughEvent()
{
    HILOGI("enter");

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();

    if (cnManager->GetActiveDevice().compare(rawAddr_.GetAddress()) == 0x00) {
        std::shared_ptr<AvrcTgPassPacket> packet = cnManager->GetPassPacket(rawAddr_);
        AVCT_SendMsgReq(cnManager->GetConnectId(rawAddr_), packet->GetLabel(), AVCT_RESPONSE, packet->AssemblePacket());
    }
}
void AvrcTgStateMachineManager::StateMachine::CtConnected::ToUnitInfoEvent()
{
    HILOGI("enter");

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();

    std::shared_ptr<AvrcTgUnitPacket> packet = cnManager->GetUnitPacket(rawAddr_);
    AVCT_SendMsgReq(cnManager->GetConnectId(rawAddr_), packet->GetLabel(), AVCT_RESPONSE, packet->AssemblePacket());
    Transition(AVRC_TG_SM_STATE_CONNECTED);
}
void AvrcTgStateMachineManager::StateMachine::CtConnected::ToCommonEvent()
{
    HILOGI("enter");

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();

    std::shared_ptr<AvrcTgVendorPacket> packet = cnManager->GetVendorPacket(rawAddr_);
    Packet *pkt = packet->PopAssembledPacket();

    AVCT_SendMsgReq(cnManager->GetConnectId(rawAddr_), packet->GetLabel(), AVCT_RESPONSE, pkt);
    PacketFree(pkt);

    if (packet->IsPacketExist()) {
        Transition(AVRC_TG_SM_STATE_CONTINUATION);
    } else {
        Transition(AVRC_TG_SM_STATE_CONNECTED);
    }
}
void AvrcTgStateMachineManager::StateMachine::CtConnected::ToAbortContinuingEvent()
{
    HILOGI("enter");

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();

    AvrcTgAcrPacket *packet = static_cast<AvrcTgAcrPacket *>(cnManager->GetVendorPacket(rawAddr_).get());
    Packet *pkt = packet->PopAssembledPacket();

    AVCT_SendMsgReq(cnManager->GetConnectId(rawAddr_), packet->GetLabel(), AVCT_RESPONSE, pkt);
    PacketFree(pkt);

    Transition(AVRC_TG_SM_STATE_CONNECTED);
}

/******************************************************************
 * AvrcTgStateMachineManager::StateMachine::CtDisconnecting       *
 ******************************************************************/

AvrcTgStateMachineManager::StateMachine::CtDisconnecting::CtDisconnecting(const std::string &name,
    utility::StateMachine &stateMachine, const RawAddress &rawAddr, utility::StateMachine::State &parent)
    : State(name, stateMachine, rawAddr, parent)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
}

void AvrcTgStateMachineManager::StateMachine::CtDisconnecting::Entry(void)
{
    HILOGI("enter");

    const AvrcTgConnectInfo *info = AvrcTgConnectManager::GetInstance()->GetConnectInfo(rawAddr_);

    if (AVCT_DisconnectReq(info->connectId_) != AVCT_SUCCESS) {
        BtAddr btAddr = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00};
        rawAddr_.ConvertToUint8(btAddr.addr);
        info->eventCallback_(info->connectId_, AVCT_DISCONNECT_CFM_EVT, AVCT_FAILED, &btAddr, nullptr);
        HILOGI("Call AVCT_DisconnectReq Failed! Address: %{public}s",
            GetEncryptAddr(rawAddr_.GetAddress()).c_str());
    }
}

void AvrcTgStateMachineManager::StateMachine::CtDisconnecting::Exit(void)
{
    HILOGI("enter");
}

bool AvrcTgStateMachineManager::StateMachine::CtDisconnecting::Dispatch(const utility::Message &msg)
{
    HILOGI("msg: %{public}x", msg.what_);

    bool result = true;

    switch (msg.what_) {
        default:
            result = false;
            break;
    }

    return result;
}

/******************************************************************
 * AvrcTgStateMachineManager::StateMachine::CtContinuation        *
 ******************************************************************/

AvrcTgStateMachineManager::StateMachine::CtContinuation::CtContinuation(const std::string &name,
    utility::StateMachine &stateMachine, const RawAddress &rawAddr, utility::StateMachine::State &parent)
    : State(name, stateMachine, rawAddr, parent)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
}

void AvrcTgStateMachineManager::StateMachine::CtContinuation::Entry(void)
{
    HILOGI("enter");
}

void AvrcTgStateMachineManager::StateMachine::CtContinuation::Exit(void)
{
    HILOGI("enter");
}

bool AvrcTgStateMachineManager::StateMachine::CtContinuation::Dispatch(const utility::Message &msg)
{
    HILOGI("msg: %{public}x", msg.what_);

    bool result = true;
    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();

    if (cnManager->GetActiveDevice().compare(rawAddr_.GetAddress()) == 0x00) {
        switch (msg.what_) {
            case AVRC_TG_SM_EVENT_TO_CONNECTED_STATE:
                Transition(AVRC_TG_SM_STATE_CONNECTED);
                break;
            case AVRC_TG_SM_EVENT_REQUEST_CONTINUING_RESPONSE: {
                std::shared_ptr<AvrcTgVendorPacket> packet = cnManager->GetVendorPacket(rawAddr_);
                Packet *pkt = packet->PopAssembledPacket();

                AVCT_SendMsgReq(cnManager->GetConnectId(rawAddr_), packet->GetLabel(), AVCT_RESPONSE, pkt);
                PacketFree(pkt);

                if (!packet->IsPacketExist()) {
                    Transition(AVRC_TG_SM_STATE_CONNECTED);
                }
                break;
            }
            case AVRC_TG_SM_EVENT_ABORT_CONTINUING_RESPONSE:
            /// FALL THROUGH
            default:
                result = false;
                break;
        }
    } else {
        /// Drops the message which is sent to the inactive device !
    }

    return result;
}

/******************************************************************
 * AvrcTgStateMachineManager::StateMachine::CtDisable             *
 ******************************************************************/

AvrcTgStateMachineManager::StateMachine::CtDisable::CtDisable(const std::string &name,
    utility::StateMachine &stateMachine, const RawAddress &rawAddr, utility::StateMachine::State &parent)
    : State(name, stateMachine, rawAddr, parent)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
}

void AvrcTgStateMachineManager::StateMachine::CtDisable::Entry(void)
{
    HILOGI("enter");

    AVCT_Deregister();

    const AvrcTgConnectInfo *info = AvrcTgConnectManager::GetInstance()->GetConnectInfo(rawAddr_);
    if (info != nullptr) {
        if (AVCT_DisconnectReq(info->connectId_) != AVCT_SUCCESS) {
            BtAddr btAddr = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00};
            rawAddr_.ConvertToUint8(btAddr.addr);
            info->eventCallback_(info->connectId_, AVCT_DISCONNECT_CFM_EVT, AVCT_FAILED, &btAddr, nullptr);
            HILOGI("Call AVCT_DisconnectReq Failed! Address: %{public}s",
                GetEncryptAddr(rawAddr_.GetAddress()).c_str());
        }
    }
}

void AvrcTgStateMachineManager::StateMachine::CtDisable::Exit(void)
{
    HILOGI("enter");
}

bool AvrcTgStateMachineManager::StateMachine::CtDisable::Dispatch(const utility::Message &msg)
{
    HILOGI("msg: %{public}x", msg.what_);

    bool result = true;

    switch (msg.what_) {
        default:
            result = false;
            break;
    }

    return result;
}

/******************************************************************
 * AvrcTgStateMachineManager::StateMachine::BrConnecting          *
 ******************************************************************/

AvrcTgStateMachineManager::StateMachine::BrConnecting::BrConnecting(
    const std::string &name, utility::StateMachine &stateMachine, const RawAddress &rawAddr)
    : State(name, stateMachine, rawAddr)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
}

void AvrcTgStateMachineManager::StateMachine::BrConnecting::Entry(void)
{
    HILOGI("enter");

    const AvrcTgConnectInfo *info = AvrcTgConnectManager::GetInstance()->GetConnectInfo(rawAddr_);

    if (AVCT_BrConnectReq(info->connectId_, info->role_) != AVCT_SUCCESS) {
        BtAddr btAddr = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00};
        rawAddr_.ConvertToUint8(btAddr.addr);
        info->eventCallback_(info->connectId_, AVCT_BR_CONNECT_CFM_EVT, AVCT_FAILED, &btAddr, nullptr);
        HILOGI("Call AVCT_BrConnectReq Failed! Address: %{public}s",
            GetEncryptAddr(rawAddr_.GetAddress()).c_str());
    }
}

void AvrcTgStateMachineManager::StateMachine::BrConnecting::Exit(void)
{
    HILOGI("enter");
}

bool AvrcTgStateMachineManager::StateMachine::BrConnecting::Dispatch(const utility::Message &msg)
{
    HILOGI("msg: %{public}x", msg.what_);

    bool result = true;

    switch (msg.what_) {
        case AVRC_TG_SM_EVENT_TO_DISABLE_STATE:
            Transition(AVRC_TG_SM_STATE_DISABLE);
            break;
        case AVRC_TG_SM_EVENT_TO_CONNECTED_STATE:
            Transition(AVRC_TG_SM_STATE_CONNECTED);
            break;
        default:
            result = false;
            break;
    }

    return result;
}

/******************************************************************
 * AvrcTgStateMachineManager::StateMachine::BrConnected           *
 ******************************************************************/

AvrcTgStateMachineManager::StateMachine::BrConnected::BrConnected(const std::string &name,
    utility::StateMachine &stateMachine, const RawAddress &rawAddr, utility::StateMachine::State &parent)
    : State(name, stateMachine, rawAddr, parent)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
}

void AvrcTgStateMachineManager::StateMachine::BrConnected::Entry(void)
{
    HILOGI("enter");
}

void AvrcTgStateMachineManager::StateMachine::BrConnected::Exit(void)
{
    HILOGI("enter");
}

bool AvrcTgStateMachineManager::StateMachine::BrConnected::Dispatch(const utility::Message &msg)
{
    HILOGI("msg: %{public}x", msg.what_);

    bool result = true;
    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();

    if (cnManager->GetActiveDevice().compare(rawAddr_.GetAddress()) == 0x00) {
        switch (msg.what_) {
            case AVRC_TG_SM_EVENT_TO_DISCONNECTING_STATE:
                Transition(AVRC_TG_SM_STATE_DISCONNECTING);
                break;
            case AVRC_TG_SM_EVENT_GENERAL_REJECT:
            case AVRC_TG_SM_EVENT_SET_BROWSED_PLAYER:
            case AVRC_TG_SM_EVENT_CHANGE_PATH:
            case AVRC_TG_SM_EVENT_GET_FOLDER_ITEMS:
            case AVRC_TG_SM_EVENT_GET_ITEM_ATTRIBUTES:
            case AVRC_TG_SM_EVENT_GET_TOTAL_NUMBER_OF_ITEMS: {
                std::shared_ptr<AvrcTgBrowsePacket> packet = cnManager->GetBrowsePacket(rawAddr_);

                AVCT_BrSendMsgReq(
                    cnManager->GetConnectId(rawAddr_), packet->GetLabel(), AVCT_RESPONSE, packet->AssemblePacket());

                Transition(AVRC_TG_SM_STATE_CONNECTED);
                break;
            }
            default:
                result = false;
                break;
        }
    } else {
        /// Drops the message which is sent to the inactive device !
    }

    return result;
}

/******************************************************************
 * AvrcTgStateMachineManager::StateMachine::BrDisconnecting       *
 ******************************************************************/

AvrcTgStateMachineManager::StateMachine::BrDisconnecting::BrDisconnecting(const std::string &name,
    utility::StateMachine &stateMachine, const RawAddress &rawAddr, utility::StateMachine::State &parent)
    : State(name, stateMachine, rawAddr, parent)
{
    HILOGI("addr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
}

void AvrcTgStateMachineManager::StateMachine::BrDisconnecting::Entry(void)
{
    HILOGI("enter");

    const AvrcTgConnectInfo *info = AvrcTgConnectManager::GetInstance()->GetConnectInfo(rawAddr_);

    if (AVCT_BrDisconnectReq(info->connectId_) != AVCT_SUCCESS) {
        BtAddr btAddr = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00};
        rawAddr_.ConvertToUint8(btAddr.addr);
        info->eventCallback_(info->connectId_, AVCT_BR_DISCONNECT_CFM_EVT, AVCT_FAILED, &btAddr, nullptr);
        HILOGI("Call AVCT_BrDisconnectReq Failed! Address: %{public}s",
            GetEncryptAddr(rawAddr_.GetAddress()).c_str());
    }
}

void AvrcTgStateMachineManager::StateMachine::BrDisconnecting::Exit(void)
{
    HILOGI("enter");
}

bool AvrcTgStateMachineManager::StateMachine::BrDisconnecting::Dispatch(const utility::Message &msg)
{
    HILOGI("msg: %{public}x", msg.what_);

    bool result = true;
    switch (msg.what_) {
        default:
            result = false;
            break;
    }

    return result;
}
}  // namespace bluetooth
}  // namespace OHOS
