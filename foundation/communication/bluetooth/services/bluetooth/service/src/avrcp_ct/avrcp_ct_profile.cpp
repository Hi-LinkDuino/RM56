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

#include "avrcp_ct_profile.h"
#include "avrcp_ct_browse.h"
#include "avrcp_ct_connection.h"
#include "avrcp_ct_notification.h"
#include "avrcp_ct_pass_through.h"
#include "avrcp_ct_sub_unit_info.h"
#include "avrcp_ct_unit_info.h"
#include "avrcp_ct_vendor.h"
#include "avrcp_ct_vendor_continuation.h"
#include "avrcp_ct_vendor_player_application_settings.h"
#include "compat.h"
#include "power_manager.h"

namespace OHOS {
namespace bluetooth {
bool AvrcCtProfile::g_isEnabled = false;

AvrcCtProfile::AvrcCtProfile(uint16_t features, uint32_t companyId, uint16_t controlMtu, uint16_t browseMtu,
    uint8_t maxFragments, utility::Dispatcher *dispatcher, AvctChannelEventCallback eventCallback,
    AvctMsgCallback msgCallback)
    : features_(features),
      companyId_(companyId),
      controlMtu_(controlMtu),
      browseMtu_(browseMtu),
      maxFragments_(maxFragments),
      dispatcher_(dispatcher),
      connectId_(0xFF),
      eventCallback_(eventCallback),
      msgCallback_(msgCallback)
{
    HILOGI("features: %{public}d, companyId: %{public}u, controlMtu: %{public}d, browseMtu: %{public}d, "
        "maxFragments: %{public}d", features, companyId, controlMtu, browseMtu, maxFragments);

    SetEnableFlag(false);
}

bool AvrcCtProfile::IsEnabled(void)
{
    HILOGI("enter");

    return g_isEnabled;
}

AvrcCtProfile::~AvrcCtProfile()
{
    HILOGI("enter");

    SetEnableFlag(false);
}

/******************************************************************
 * REGISTER / UNREGISTER OBSERVER                                 *
 ******************************************************************/

void AvrcCtProfile::RegisterObserver(AvrcCtProfile::Observer *observer)
{
    HILOGI("enter");

    myObserver_ = observer;
}

void AvrcCtProfile::UnregisterObserver(void)
{
    HILOGI("enter");

    myObserver_ = nullptr;
}

/******************************************************************
 * ENABLE / DISABLE                                               *
 ******************************************************************/

int AvrcCtProfile::Enable(bool isTgEnabled)
{
    HILOGI("isTgEnabled: %{public}d", isTgEnabled);

    int result = RET_NO_ERROR;
    SetEnableFlag(true);

    if (!isTgEnabled) {
        /// If the following statement in the debt_config.xml. That means that TG allows to enable. CT do not
        /// allow the passive connection. Because if CT and TG are registered passive connections, it will case that the
        /// inability to distinguish between the end device
        /// "<T1 property=" AvrcpTgService ">true</T1>"
        AVCT_Register(controlMtu_, browseMtu_, AVCT_CT);

        AvctConnectParam param = {eventCallback_, msgCallback_, AVRC_CT_AV_REMOTE_CONTROL, AVCT_ACPT, nullptr};
        BtAddr btAddr = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00};
        if (AVCT_ConnectReq(&connectId_, &param, &btAddr) != AVCT_SUCCESS) {
            result = RET_BAD_STATUS;
        }
    } else {
        /// CT is only allowed the active connection.
    }

    return result;
}

int AvrcCtProfile::Disable(void)
{
    HILOGI("enter");

    do {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        SetEnableFlag(false);
    } while (false);

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    AvrcCtStateMachineManager *smManager = AvrcCtStateMachineManager::GetInstance();

    if (!cnManager->IsConnectInfoEmpty()) {
        utility::Message msg(AVRC_CT_SM_EVENT_TO_DISABLE_STATE);
        smManager->SendMessageToAllBrowseStateMachine(msg);
        smManager->SendMessageToAllControlStateMachine(msg);
    } else {
        AvrcpCtSafeDelete(cnManager);
        smManager->ShutDown();
        myObserver_->onDisabled();
    }

    return RET_NO_ERROR;
}

void AvrcCtProfile::SetEnableFlag(bool isEnabled)
{
    HILOGI("isEnabled: %{public}d", isEnabled);

    g_isEnabled = isEnabled;
}

/******************************************************************
 * CONNECTION                                                     *
 ******************************************************************/

std::vector<bluetooth::RawAddress> AvrcCtProfile::GetConnectedDevices(void)
{
    HILOGI("enter");
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    std::vector<bluetooth::RawAddress> result;

    std::list<std::string> addrs = AvrcCtConnectManager::GetInstance()->GetDeviceAddresses();
    for (auto &addr : addrs) {
        RawAddress rawAddr(addr);
        if (GetDeviceState(rawAddr) == static_cast<int>(BTConnectState::CONNECTED)) {
            result.push_back(rawAddr);
        }
    }

    return result;
}

std::vector<RawAddress> AvrcCtProfile::GetDevicesByStates(const std::vector<int> &states)
{
    HILOGI("enter");
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();

    std::vector<RawAddress> rawAddrs;
    std::list<std::string> addrs;

    for (auto &state : states) {
        if (state == static_cast<int>(BTConnectState::DISCONNECTED)) {
            addrs = cnManager->GetAllDisconnectedDevices();
        } else {
            addrs = cnManager->GetDeviceAddresses();
        }
        for (auto &addr : addrs) {
            RawAddress rawAddr(addr);
            if (GetDeviceState(rawAddr) == state) {
                rawAddrs.push_back(rawAddr);
            }
        }
    }

    return rawAddrs;
}

int AvrcCtProfile::GetDeviceState(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    AvrcCtStateMachineManager *smManager = AvrcCtStateMachineManager::GetInstance();
    int result;

    if (smManager->IsControlConnectingState(rawAddr)) {
        result = static_cast<int>(BTConnectState::CONNECTING);
    } else if (smManager->IsControlConnectedState(rawAddr) || smManager->IsControlPendingState(rawAddr) ||
               smManager->IsControlContinuationState(rawAddr)) {
        result = static_cast<int>(BTConnectState::CONNECTED);
    } else if (smManager->IsControlDisconnectingState(rawAddr)) {
        result = static_cast<int>(BTConnectState::DISCONNECTING);
    } else if (smManager->IsControlDisableState(rawAddr)) {
        result = static_cast<int>(BTConnectState::DISCONNECTING);
    } else {
        result = static_cast<int>(BTConnectState::DISCONNECTED);
    }

    return result;
}

int AvrcCtProfile::GetMaxConnectNum(void)
{
    HILOGI("enter");
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    int result = 0x00;

    std::list<std::string> addrs = AvrcCtConnectManager::GetInstance()->GetDeviceAddresses();

    for (auto &addr : addrs) {
        RawAddress rawAddr(addr);
        if (GetDeviceState(rawAddr) == static_cast<int>(BTConnectState::CONNECTED)) {
            ++result;
        }
    }

    return result;
}

int AvrcCtProfile::Connect(const RawAddress &rawAddr) const
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    int result = RET_NO_ERROR;

    bool absVolume = Compat::CompatCheck(bluetooth::CompatType::COMPAT_REJECT_ABSOLUTE_VOLUME, rawAddr.GetAddress());
    result |= AvrcCtConnectManager::GetInstance()->Add(
        rawAddr, 0x00, AVCT_INIT, controlMtu_, browseMtu_, companyId_, 0x0000, absVolume, eventCallback_, msgCallback_);
    result |= AvrcCtStateMachineManager::GetInstance()->AddControlStateMachine(rawAddr);
    if (result == RET_NO_ERROR) {
        myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::CONNECTING));
    }

    return result;
}

int AvrcCtProfile::Disconnect(const RawAddress &rawAddr) const
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtStateMachineManager *smManager = AvrcCtStateMachineManager::GetInstance();
    utility::Message msg(AVRC_CT_SM_EVENT_TO_DISCONNECTING_STATE);
    bool result = true;

    result &= smManager->SendMessageToBrowseStateMachine(rawAddr, msg);
    result &= smManager->SendMessageToControlStateMachine(rawAddr, msg);
    if (result) {
        myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::DISCONNECTING));
    }

    return RET_NO_ERROR;
}

int AvrcCtProfile::ConnectBr(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtStateMachineManager *smManager = AvrcCtStateMachineManager::GetInstance();

    return smManager->AddBrowseStateMachine(rawAddr);
}

int AvrcCtProfile::DisconnectBr(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    utility::Message msg(AVRC_CT_SM_EVENT_TO_DISCONNECTING_STATE);
    AvrcCtStateMachineManager::GetInstance()->SendMessageToBrowseStateMachine(rawAddr, msg);

    return RET_NO_ERROR;
}

int AvrcCtProfile::GetConnectState(void)
{
    HILOGI("enter");
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    int result = PROFILE_STATE_DISCONNECTED;

    std::list<std::string> addrs = AvrcCtConnectManager::GetInstance()->GetDeviceAddresses();
    for (auto &addr : addrs) {
        RawAddress rawAddr(addr);
        switch (static_cast<BTConnectState>(GetDeviceState(rawAddr))) {
            case BTConnectState::CONNECTING:
                result |= PROFILE_STATE_CONNECTING;
                break;
            case BTConnectState::CONNECTED:
                result |= PROFILE_STATE_CONNECTED;
                break;
            case BTConnectState::DISCONNECTING:
                result |= PROFILE_STATE_DISCONNECTING;
                break;
            case BTConnectState::DISCONNECTED:
                /// FALL THROUGH
            default:
                result |= PROFILE_STATE_DISCONNECTED;
                break;
        }
    }

    return result;
}

/******************************************************************
 * PASS THROUGH COMMAND                                           *
 ******************************************************************/

void AvrcCtProfile::SendPressButtonCmd(const RawAddress &rawAddr, uint8_t button)
{
    HILOGI("address: %{public}s, button: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), button);

    std::shared_ptr<AvrcCtPassPacket> packet = std::make_shared<AvrcCtPassPacket>(button, AVRC_KEY_STATE_PRESS);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendPassCmd(rawAddr, packet);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::SendReleaseButtonCmd(const RawAddress &rawAddr, uint8_t button)
{
    HILOGI("address: %{public}s, button: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), button);

    std::shared_ptr<AvrcCtPassPacket> packet = std::make_shared<AvrcCtPassPacket>(button, AVRC_KEY_STATE_RELEASE);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendPassCmd(rawAddr, packet);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

bool AvrcCtProfile::IsPassQueueFull(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    return (AvrcCtConnectManager::GetInstance()->GetSizeOfPassQueue(rawAddr) == AVRC_CT_DEFAULT_SIZE_OF_QUEUE);
}

void AvrcCtProfile::SendPassCmd(const RawAddress &rawAddr, const std::shared_ptr<AvrcCtPassPacket> &pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();

    if (cnManager->IsPassPacketEmpty(rawAddr)) {
        /// Unprocessed pass through command.

        /// Sets the information which is used in the "CONNECTED" state of the control state machine.
        cnManager->SetPassPacket(rawAddr, pkt);

        utility::Message msg(AVRC_CT_SM_EVENT_PASS_THROUGH);
        AvrcCtStateMachineManager::GetInstance()->SendMessageToControlStateMachine(rawAddr, msg);

        /// Sets the information which is used in the timeout.
        auto func =
            std::bind(&AvrcCtProfile::PassTimeoutCallback, this, rawAddr, pkt->GetKeyOperation(), pkt->GetKeyState());
        cnManager->SetPassTimer(rawAddr, func, AVRC_CT_TIMER_PASS_THROUGH);
    } else {
        /// There is a command in processing,
        if (cnManager->GetSizeOfPassQueue(rawAddr) >= AVRC_CT_DEFAULT_SIZE_OF_QUEUE) {
            InformPassRsp(rawAddr, pkt->GetKeyOperation(), pkt->GetKeyState(), RET_BAD_STATUS);
            HILOGI("The pass through command is full! - Address: %{public}s - sizeOf: %{public}d",
                GET_ENCRYPT_AVRCP_ADDR(rawAddr), AVRC_CT_DEFAULT_SIZE_OF_QUEUE);
        } else {
            cnManager->PushPassQueue(rawAddr, pkt);
            HILOGI("Waiting for the response! - Address[%{public}s]", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
        }
    }
}

void AvrcCtProfile::SendNextPassCmd(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    cnManager->ClearPassInfo(rawAddr);

    if (cnManager->GetSizeOfPassQueue(rawAddr) != 0x00) {
        std::shared_ptr<AvrcCtPassPacket> packet = cnManager->PopPassQueue(rawAddr);
        SendPassCmd(rawAddr, packet);
    }
}

void AvrcCtProfile::ReceivePassRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    SendNextPassCmd(rawAddr);

    AvrcCtPassPacket packet(pkt);
    HILOGI("Address: %{public}s - key: %{public}x - state: %{public}x", GET_ENCRYPT_AVRCP_ADDR(rawAddr),
        packet.GetKeyOperation(), packet.GetKeyState());

    InformPassRsp(
        rawAddr, packet.GetKeyOperation(), packet.GetKeyState(), ExpainPassCrCodeToResult(packet.GetCrCode()));
}

void AvrcCtProfile::InformPassRsp(const RawAddress &rawAddr, uint8_t button, uint8_t state, int result) const
{
    HILOGI("address: %{public}s, button: %{public}d, state: %{public}d, result: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), button, state, result);

    switch (state) {
        case AVRC_KEY_STATE_PRESS:
            myObserver_->onButtonPressed(rawAddr, button, result);
            break;
        case AVRC_KEY_STATE_RELEASE:
            myObserver_->onButtonReleased(rawAddr, button, result);
            break;
        default:
            HILOGI("The button state is incorrect!");
            break;
    }
}

void AvrcCtProfile::ProcessPassTimeout(RawAddress rawAddr, uint8_t key, uint8_t state)
{
    HILOGI("address: %{public}s, key: %{public}x, state: %{public}x", GET_ENCRYPT_AVRCP_ADDR(rawAddr), key, state);

    if (IsEnabled()) {
        SendNextPassCmd(rawAddr);
        InformPassRsp(rawAddr, key, state, RET_BAD_STATUS);
    }
}

void AvrcCtProfile::PassTimeoutCallback(const RawAddress &rawAddr, uint8_t key, uint8_t state)
{
    HILOGI("address: %{public}s, key: %{public}x - state: %{public}x", GET_ENCRYPT_AVRCP_ADDR(rawAddr), key, state);

    if (IsEnabled()) {
        dispatcher_->PostTask(std::bind(&AvrcCtProfile::ProcessPassTimeout, this, rawAddr, key, state));
    }
}

/******************************************************************
 * UNIT INFO / SUB UNIT INFO COMMAND                              *
 ******************************************************************/

void AvrcCtProfile::SendUnitCmd(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    AvrcCtStateMachineManager *smManager = AvrcCtStateMachineManager::GetInstance();

    std::shared_ptr<AvrcCtUnitPacket> pkt = std::make_shared<AvrcCtUnitPacket>();
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    if (!smManager->IsControlPendingState(rawAddr) && !smManager->IsControlContinuationState(rawAddr)) {
        /// Unprocessed vendor dependent command.

        /// Sets the information which is used in the "CONNECTED" state of the control state machine.
        cnManager->SetUnitPacket(rawAddr, pkt);

        utility::Message msg(AVRC_CT_SM_EVENT_UNIT_INFO);
        AvrcCtStateMachineManager::GetInstance()->SendMessageToControlStateMachine(rawAddr, msg);

        /// Sets the information which is used in the timeout.
        auto func = std::bind(&AvrcCtProfile::UnitTimeoutCallback, this, rawAddr);
        cnManager->SetUnitTimer(rawAddr, func, AVRC_CT_TIMER_T_MTP);
    } else {
        /// There is a command in processing,

        if (cnManager->GetSizeOfUnitQueue(rawAddr) > AVRC_CT_DEFAULT_SIZE_OF_QUEUE) {
            HILOGI("The vendor queue is oversize: %{public}d", cnManager->GetSizeOfVendorQueue(rawAddr));
        } else {
            cnManager->PushUnitQueue(rawAddr, pkt);
            HILOGI("Waiting for the response!");
        }
    }
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::SendSubUnitCmd(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    AvrcCtStateMachineManager *smManager = AvrcCtStateMachineManager::GetInstance();

    std::shared_ptr<AvrcCtUnitPacket> pkt = std::make_shared<AvrcCtSubUnitPacket>();
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    if (!smManager->IsControlPendingState(rawAddr) && !smManager->IsControlContinuationState(rawAddr)) {
        /// Unprocessed vendor dependent command.

        /// Sets the information which is used in the "CONNECTED" state of the control state machine.
        cnManager->SetUnitPacket(rawAddr, pkt);

        utility::Message msg(AVRC_CT_SM_EVENT_SUB_UNIT_INFO);
        AvrcCtStateMachineManager::GetInstance()->SendMessageToControlStateMachine(rawAddr, msg);

        /// Sets the information which is used in the timeout.
        auto func = std::bind(&AvrcCtProfile::UnitTimeoutCallback, this, rawAddr);
        cnManager->SetUnitTimer(rawAddr, func, AVRC_CT_TIMER_T_MTP);
    } else {
        /// There is a command in processing,

        if (cnManager->GetSizeOfUnitQueue(rawAddr) > AVRC_CT_DEFAULT_SIZE_OF_QUEUE) {
            HILOGI("The vendor queue is oversize: %{public}d", cnManager->GetSizeOfVendorQueue(rawAddr));
        } else {
            cnManager->PushUnitQueue(rawAddr, pkt);
            HILOGI("Waiting for the response!");
        }
    }
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

bool AvrcCtProfile::IsUnitQueueFull(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    return (AvrcCtConnectManager::GetInstance()->GetSizeOfUnitQueue(rawAddr) == AVRC_CT_DEFAULT_SIZE_OF_QUEUE);
}

void AvrcCtProfile::SendNextUnitCmd(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    AvrcCtStateMachineManager *smManager = AvrcCtStateMachineManager::GetInstance();
    utility::Message msg(AVRC_CT_SM_EVENT_INVALID);

    cnManager->ClearUnitInfo(rawAddr);
    msg.what_ = AVRC_CT_SM_EVENT_TO_CONNECTED_STATE;
    smManager->SendMessageToControlStateMachine(rawAddr, msg);

    if (cnManager->GetSizeOfUnitQueue(rawAddr) != 0x00) {
        std::shared_ptr<AvrcCtUnitPacket> packet = cnManager->PopUnitQueue(rawAddr);

        switch (packet->GetOpCode()) {
            case AVRC_CT_OP_CODE_UNIT_INFO:
                msg.what_ = AVRC_CT_SM_EVENT_UNIT_INFO;
                break;
            case AVRC_CT_OP_CODE_SUB_UNIT_INFO:
                msg.what_ = AVRC_CT_SM_EVENT_SUB_UNIT_INFO;
                break;
            default:
                HILOGI("Opcode is wrong! - opCode: %{public}x", packet->GetOpCode());
                break;
        }

        cnManager->SetUnitPacket(rawAddr, packet);
        smManager->SendMessageToControlStateMachine(rawAddr, msg);
    } else if (cnManager->GetSizeOfVendorQueue(rawAddr) != 0x00) {
        SendNextVendorCmd(rawAddr);
    }
}

void AvrcCtProfile::ReceiveUnitRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    SendNextUnitCmd(rawAddr);

    AvrcCtUnitPacket packet(pkt);
    HILOGI("response: %{public}x", packet.GetCrCode());
}

void AvrcCtProfile::ReceiveSubUnitRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    SendNextUnitCmd(rawAddr);

    AvrcCtSubUnitPacket packet(pkt);
    HILOGI("response: %{public}x", packet.GetCrCode());
}

void AvrcCtProfile::ProcessUnitTimeout(RawAddress rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    if (IsEnabled()) {
        std::shared_ptr<AvrcCtUnitPacket> packet = AvrcCtConnectManager::GetInstance()->GetUnitPacket(rawAddr);
        if (packet != nullptr) {
            switch (packet->GetOpCode()) {
                case AVRC_CT_OP_CODE_UNIT_INFO:
                    HILOGI("opCode[UNIT_INFO]");
                    break;
                case AVRC_CT_OP_CODE_SUB_UNIT_INFO:
                    HILOGI("opCode[SUB_UNIT_INFO]");
                    break;
                default:
                    HILOGI("Opcode is wrong! - opCode: %{public}x", packet->GetOpCode());
                    break;
            }
        } else {
            HILOGI("The saved packet is nullptr!");
        }
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
        SendNextUnitCmd(rawAddr);
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
    }
}

void AvrcCtProfile::UnitTimeoutCallback(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    if (IsEnabled()) {
        dispatcher_->PostTask(std::bind(&AvrcCtProfile::ProcessUnitTimeout, this, rawAddr));
    }
}

/******************************************************************
 * VENDOR COMMAND                                                 *
 ******************************************************************/

void AvrcCtProfile::SendVendorCmd(
    const RawAddress &rawAddr, const std::shared_ptr<AvrcCtVendorPacket> &pkt, AvrcCtSmEvent event)
{
    HILOGI("address: %{public}s, event: %{public}x", GET_ENCRYPT_AVRCP_ADDR(rawAddr), event);

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    AvrcCtStateMachineManager *smManager = AvrcCtStateMachineManager::GetInstance();

    if (!smManager->IsControlPendingState(rawAddr) && !smManager->IsControlContinuationState(rawAddr)) {
        /// Unprocessed vendor dependent command.
        /// Sets the information which is used in the "CONNECTED" state of the control state machine.
        cnManager->ClearVendorInfo(rawAddr);
        cnManager->SetVendorPacket(rawAddr, pkt);

        utility::Message msg(event);
        AvrcCtStateMachineManager::GetInstance()->SendMessageToControlStateMachine(rawAddr, msg);

        /// Sets the information which is used in the timeout.
        auto func = std::bind(&AvrcCtProfile::VendorTimeoutCallback, this, rawAddr);
        cnManager->SetVendorTimer(rawAddr, func, AVRC_CT_TIMER_T_MTP);
    } else {
        /// There is a command in processing,

        if (cnManager->GetSizeOfVendorQueue(rawAddr) > AVRC_CT_DEFAULT_SIZE_OF_QUEUE) {
            HILOGI("The vendor queue is oversize: %{public}d", cnManager->GetSizeOfVendorQueue(rawAddr));
        } else {
            cnManager->PushVendorQueue(rawAddr, pkt);
            HILOGI("Waiting for the response!");
        }
    }
}

void AvrcCtProfile::SendNextVendorCmd(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    cnManager->ClearVendorInfo(rawAddr);

    utility::Message msg(AVRC_CT_SM_EVENT_TO_CONNECTED_STATE);
    AvrcCtStateMachineManager::GetInstance()->SendMessageToControlStateMachine(rawAddr, msg);

    if (cnManager->GetSizeOfVendorQueue(rawAddr) != 0x00) {
        std::shared_ptr<AvrcCtVendorPacket> packet = cnManager->PopVendorQueue(rawAddr);
        SendVendorCmd(rawAddr, packet, static_cast<AvrcCtSmEvent>(packet->GetPduId()));
    } else if (cnManager->GetSizeOfUnitQueue(rawAddr) != 0x00) {
        SendNextUnitCmd(rawAddr);
    }
}

void AvrcCtProfile::SendVendorContinueCmd(const RawAddress &rawAddr, uint8_t pduId)
{
    HILOGI("address: %{public}s, pduId: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), pduId);

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    cnManager->ClearVendorPacket(rawAddr);
    cnManager->ClearVendorTimer(rawAddr);

    std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtRcrPacket>(pduId);
    cnManager->SetVendorPacket(rawAddr, packet);

    utility::Message msg(AVRC_CT_SM_EVENT_REQUEST_CONTINUING_RESPONSE);
    AvrcCtStateMachineManager::GetInstance()->SendMessageToControlStateMachine(rawAddr, msg);

    /// Sets the information which is used in the timeout.
    auto func = std::bind(&AvrcCtProfile::VendorTimeoutCallback, this, rawAddr);
    cnManager->SetVendorTimer(rawAddr, func, AVRC_CT_TIMER_T_MTP);
}

void AvrcCtProfile::SendVendorAbortCmd(const RawAddress &rawAddr, uint8_t pduId) const
{
    HILOGI("address: %{public}s, pduId: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), pduId);

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    cnManager->ClearVendorPacket(rawAddr);
    cnManager->ClearVendorTimer(rawAddr);

    std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtAcrPacket>(pduId);
    cnManager->SetVendorPacket(rawAddr, packet);

    utility::Message msg(AVRC_CT_SM_EVENT_ABORT_CONTINUING_RESPONSE);
    AvrcCtStateMachineManager::GetInstance()->SendMessageToControlStateMachine(rawAddr, msg);
}

bool AvrcCtProfile::IsVendorQueueFull(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    return (AvrcCtConnectManager::GetInstance()->GetSizeOfVendorQueue(rawAddr) == AVRC_CT_DEFAULT_SIZE_OF_QUEUE);
}

void AvrcCtProfile::SendGetCapabilitiesCmd(const RawAddress &rawAddr, uint8_t capabilityId)
{
    HILOGI("address: %{public}s, capabilityId: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), capabilityId);

    std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtGcPacket>(capabilityId);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendVendorCmd(rawAddr, packet, AVRC_CT_SM_EVENT_GET_CAPABILITIES);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceiveGetCapabilitiesRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    SendNextVendorCmd(rawAddr);

    AvrcCtGcPacket packet(pkt);
    if (packet.GetEvents().size() != 0x00) {
        AvrcCtConnectManager::GetInstance()->DisableExcludeEvents(rawAddr, packet.GetEvents());
    }
    myObserver_->onGetCapabilities(
        rawAddr, packet.GetCompanies(), packet.GetEvents(), ExplainCrCodeToResult(packet.GetCrCode()));
}

void AvrcCtProfile::SendListPlayerApplicationSettingAttributesCmd(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtLpasaPacket>();
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendVendorCmd(rawAddr, packet, AVRC_CT_SM_EVENT_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceiveListPlayerApplicationSettingAttributesRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    SendNextVendorCmd(rawAddr);

    AvrcCtLpasaPacket packet(pkt);
    myObserver_->onListPlayerApplicationSettingAttributes(
        rawAddr, packet.GetAttributes(), ExplainCrCodeToResult(packet.GetCrCode()));
}

void AvrcCtProfile::SendListPlayerApplicationSettingValuesCmd(const RawAddress &rawAddr, uint8_t attribute)
{
    HILOGI("address: %{public}s, attribute: %{public}x", GET_ENCRYPT_AVRCP_ADDR(rawAddr), attribute);

    std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtLpasvPacket>(attribute);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendVendorCmd(rawAddr, packet, AVRC_CT_SM_EVENT_LIST_PLAYER_APPLICATION_SETTING_VALUES);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceiveListPlayerApplicationSettingValuesRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();

    std::shared_ptr<AvrcCtVendorPacket> packet = cnManager->GetVendorContinuePacket(rawAddr);
    AvrcCtLpasvPacket *lpasvPkt = nullptr;

    if (packet == nullptr) {
        packet = cnManager->GetVendorPacket(rawAddr);
        lpasvPkt = static_cast<AvrcCtLpasvPacket *>(cnManager->GetVendorPacket(rawAddr).get());
        lpasvPkt->DisassemblePacket(pkt);
    } else {
        lpasvPkt = static_cast<AvrcCtLpasvPacket *>(packet.get());
        lpasvPkt->DisassemblePacket(pkt);
    }

    HILOGI("attribute: %{public}x - values.size: %{public}zu - result: %{public}d", lpasvPkt->GetAttribute(),
        lpasvPkt->GetValues().size(), ExplainCrCodeToResult(lpasvPkt->GetCrCode()));

    switch (lpasvPkt->GetPacketType()) {
        case AVRC_CT_VENDOR_PACKET_TYPE:
        /// FALL THROUGH
        case AVRC_CT_VENDOR_PACKET_TYPE_END: {
            SendNextVendorCmd(rawAddr);

            myObserver_->onListPlayerApplicationSettingValues(
                rawAddr, lpasvPkt->GetAttribute(), lpasvPkt->GetValues(), ExplainCrCodeToResult(lpasvPkt->GetCrCode()));
            break;
        }
        case AVRC_CT_VENDOR_PACKET_TYPE_START:
        /// FALL THROUGH
        case AVRC_CT_VENDOR_PACKET_TYPE_CONTINUE: {
            if (packet->GetReceivedFragments() >= maxFragments_) {
                SendVendorAbortCmd(rawAddr, lpasvPkt->GetPduId());
                myObserver_->onListPlayerApplicationSettingValues(rawAddr,
                    lpasvPkt->GetAttribute(),
                    lpasvPkt->GetValues(),
                    ExplainCrCodeToResult(lpasvPkt->GetCrCode()));
            } else {
                cnManager->SetVendorContinuePacket(rawAddr, packet);
                SendVendorContinueCmd(rawAddr, lpasvPkt->GetPduId());
            }
            break;
        }
        default:
            HILOGI("Packet Type is wrong! - packetType: %{public}d", lpasvPkt->GetPacketType());
            break;
    }
}

void AvrcCtProfile::SendGetCurrentPlayerApplicationSettingValueCmd(
    const RawAddress &rawAddr, const std::vector<uint8_t> &attributes)
{
    HILOGI("address: %{public}s, attributes.size: %{public}zu", GET_ENCRYPT_AVRCP_ADDR(rawAddr), attributes.size());

    std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtGcpasvPacket>(attributes);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendVendorCmd(rawAddr, packet, AVRC_CT_SM_EVENT_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceiveGetCurrentPlayerApplicationSettingValueRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();

    std::shared_ptr<AvrcCtVendorPacket> packet = cnManager->GetVendorContinuePacket(rawAddr);
    AvrcCtGcpasvPacket *gcpasvPkt = nullptr;

    if (packet == nullptr) {
        packet = cnManager->GetVendorPacket(rawAddr);
        gcpasvPkt = static_cast<AvrcCtGcpasvPacket *>(packet.get());
        gcpasvPkt->ClearAttributes();
        gcpasvPkt->DisassemblePacket(pkt);
    } else {
        gcpasvPkt = static_cast<AvrcCtGcpasvPacket *>(packet.get());
        gcpasvPkt->DisassemblePacket(pkt);
    }

    HILOGI("attributes.size: %{public}zu - values.size: %{public}zu - result: %{public}d",
        gcpasvPkt->GetAttributes().size(), gcpasvPkt->GetValues().size(),
        ExplainCrCodeToResult(gcpasvPkt->GetCrCode()));

    switch (gcpasvPkt->GetPacketType()) {
        case AVRC_CT_VENDOR_PACKET_TYPE:
        /// FALL THROUGH
        case AVRC_CT_VENDOR_PACKET_TYPE_END: {
            SendNextVendorCmd(rawAddr);

            myObserver_->onGetCurrentPlayerApplicationSettingValue(rawAddr,
                gcpasvPkt->GetAttributes(),
                gcpasvPkt->GetValues(),
                ExplainCrCodeToResult(gcpasvPkt->GetCrCode()));
            break;
        }
        case AVRC_CT_VENDOR_PACKET_TYPE_START:
        /// FALL THROUGH
        case AVRC_CT_VENDOR_PACKET_TYPE_CONTINUE: {
            if (packet->GetReceivedFragments() >= maxFragments_) {
                SendVendorAbortCmd(rawAddr, gcpasvPkt->GetPduId());
                myObserver_->onGetCurrentPlayerApplicationSettingValue(rawAddr,
                    gcpasvPkt->GetAttributes(),
                    gcpasvPkt->GetValues(),
                    ExplainCrCodeToResult(gcpasvPkt->GetCrCode()));
            } else {
                cnManager->SetVendorContinuePacket(rawAddr, packet);
                SendVendorContinueCmd(rawAddr, gcpasvPkt->GetPduId());
            }
            break;
        }
        default:
            HILOGI("Packet Type is wrong! - packetType: %{public}d", gcpasvPkt->GetPacketType());
            break;
    }
}

void AvrcCtProfile::SendSetPlayerApplicationSettingValueCmd(
    const RawAddress &rawAddr, const std::vector<uint8_t> &attributes, const std::vector<uint8_t> &values)
{
    HILOGI("address: %{public}s, attributes.size: %{public}zu, values.size: %{public}zu",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), attributes.size(), values.size());

    std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtSpasvPacket>(attributes, values);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendVendorCmd(rawAddr, packet, AVRC_CT_SM_EVENT_SET_PLAYER_APPLICATION_SETTING_VALUE);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceiveSetPlayerApplicationSettingValueRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    SendNextVendorCmd(rawAddr);

    AvrcCtSpasvPacket packet(pkt);
    myObserver_->onSetPlayerApplicationSettingValue(rawAddr, ExplainCrCodeToResult(packet.GetCrCode()));
}

void AvrcCtProfile::SendGetPlayerApplicationSettingAttributeTextCmd(
    const RawAddress &rawAddr, const std::vector<uint8_t> &attributes)
{
    HILOGI("address: %{public}s, attributes.size: %{public}zu", GET_ENCRYPT_AVRCP_ADDR(rawAddr), attributes.size());

    std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtGpasatPacket>(attributes);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendVendorCmd(rawAddr, packet, AVRC_CT_SM_EVENT_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceiveGetPlayerApplicationSettingAttributeTextRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();

    std::shared_ptr<AvrcCtVendorPacket> packet = cnManager->GetVendorContinuePacket(rawAddr);
    AvrcCtGpasatPacket *gpasatPkt = nullptr;

    if (packet == nullptr) {
        packet = cnManager->GetVendorPacket(rawAddr);
        gpasatPkt = static_cast<AvrcCtGpasatPacket *>(packet.get());
        gpasatPkt->ClearAttributes();
        gpasatPkt->ClearValueName();
        gpasatPkt->DisassemblePacket(pkt);
    } else {
        gpasatPkt = static_cast<AvrcCtGpasatPacket *>(packet.get());
        gpasatPkt->DisassemblePacket(pkt);
    }
    HILOGI("attributes.size: %{public}zu, attributeStringLength.size: %{public}zu, result: %{public}d",
        gpasatPkt->GetAttributes().size(), gpasatPkt->GetAttributeName().size(),
        ExplainCrCodeToResult(gpasatPkt->GetCrCode()));

    switch (gpasatPkt->GetPacketType()) {
        case AVRC_CT_VENDOR_PACKET_TYPE:
        /// FALL THROUGH
        case AVRC_CT_VENDOR_PACKET_TYPE_END: {
            SendNextVendorCmd(rawAddr);

            myObserver_->onGetPlayerApplicationSettingAttributeText(rawAddr,
                gpasatPkt->GetAttributes(),
                gpasatPkt->GetAttributeName(),
                ExplainCrCodeToResult(gpasatPkt->GetCrCode()));
            break;
        }
        case AVRC_CT_VENDOR_PACKET_TYPE_START:
        /// FALL THROUGH
        case AVRC_CT_VENDOR_PACKET_TYPE_CONTINUE: {
            if (packet->GetReceivedFragments() >= maxFragments_) {
                SendVendorAbortCmd(rawAddr, gpasatPkt->GetPduId());
                myObserver_->onGetPlayerApplicationSettingAttributeText(rawAddr,
                    gpasatPkt->GetAttributes(),
                    gpasatPkt->GetAttributeName(),
                    ExplainCrCodeToResult(gpasatPkt->GetCrCode()));
            } else {
                cnManager->SetVendorContinuePacket(rawAddr, packet);
                SendVendorContinueCmd(rawAddr, gpasatPkt->GetPduId());
            }
            break;
        }
        default:
            HILOGI("Packet Type is wrong! - packetType: %{public}d", gpasatPkt->GetPacketType());
            break;
    }
}

void AvrcCtProfile::SendGetPlayerApplicationSettingValueTextCmd(
    const RawAddress &rawAddr, uint8_t attributeId, const std::vector<uint8_t> &values)
{
    HILOGI("address: %{public}s, attributeId: %{public}d, values.size[%{public}zu]",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), attributeId, values.size());

    std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtGpasvtPacket>(attributeId, values);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendVendorCmd(rawAddr, packet, AVRC_CT_SM_EVENT_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceiveGetPlayerApplicationSettingValueTextRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();

    std::shared_ptr<AvrcCtVendorPacket> packet = cnManager->GetVendorContinuePacket(rawAddr);
    AvrcCtGpasvtPacket *gpasvtPkt = nullptr;

    if (packet == nullptr) {
        packet = cnManager->GetVendorPacket(rawAddr);
        gpasvtPkt = static_cast<AvrcCtGpasvtPacket *>(packet.get());
        gpasvtPkt->ClearValues();
        gpasvtPkt->ClearValueName();
        gpasvtPkt->DisassemblePacket(pkt);
    } else {
        gpasvtPkt = static_cast<AvrcCtGpasvtPacket *>(packet.get());
        gpasvtPkt->DisassemblePacket(pkt);
    }
    HILOGI("values.size: %{public}zu, valuestr.size: %{public}zu, result: %{public}d",
        gpasvtPkt->GetValues().size(), gpasvtPkt->GetValueName().size(), ExplainCrCodeToResult(gpasvtPkt->GetCrCode()));

    switch (gpasvtPkt->GetPacketType()) {
        case AVRC_CT_VENDOR_PACKET_TYPE:
        /// FALL THROUGH
        case AVRC_CT_VENDOR_PACKET_TYPE_END: {
            SendNextVendorCmd(rawAddr);

            myObserver_->onGetPlayerApplicationSettingValueText(rawAddr,
                gpasvtPkt->GetValues(),
                gpasvtPkt->GetValueName(),
                ExplainCrCodeToResult(gpasvtPkt->GetCrCode()));
            break;
        }
        case AVRC_CT_VENDOR_PACKET_TYPE_START:
        /// FALL THROUGH
        case AVRC_CT_VENDOR_PACKET_TYPE_CONTINUE: {
            if (packet->GetReceivedFragments() >= maxFragments_) {
                SendVendorAbortCmd(rawAddr, gpasvtPkt->GetPduId());
                myObserver_->onGetPlayerApplicationSettingValueText(rawAddr,
                    gpasvtPkt->GetValues(),
                    gpasvtPkt->GetValueName(),
                    ExplainCrCodeToResult(gpasvtPkt->GetCrCode()));
            } else {
                cnManager->SetVendorContinuePacket(rawAddr, packet);
                SendVendorContinueCmd(rawAddr, gpasvtPkt->GetPduId());
            }
            break;
        }
        default:
            HILOGI("Packet Type is wrong! packetType: %{public}d", gpasvtPkt->GetPacketType());
            break;
    }
}

void AvrcCtProfile::SendGetElementAttributesCmd(
    const RawAddress &rawAddr, uint64_t identifier, const std::vector<uint32_t> &attributes)
{
    HILOGI("address: %{public}s, identifier: %{public}ju, attributes.size: %{public}zu",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), identifier, attributes.size());

    std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtGeaPacket>(identifier, attributes);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendVendorCmd(rawAddr, packet, AVRC_CT_SM_EVENT_GET_ELEMENT_ATTRIBUTES);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceiveGetElementAttributesRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();

    std::shared_ptr<AvrcCtVendorPacket> packet = cnManager->GetVendorContinuePacket(rawAddr);
    AvrcCtGeaPacket *geaPkt = nullptr;
    if (packet == nullptr) {
        packet = cnManager->GetVendorPacket(rawAddr);
        geaPkt = static_cast<AvrcCtGeaPacket *>(packet.get());
        geaPkt->ClearAttributes();
        geaPkt->ClearValues();
        geaPkt->DisassemblePacket(pkt);
    } else {
        geaPkt = static_cast<AvrcCtGeaPacket *>(packet.get());
        geaPkt->DisassemblePacket(pkt);
    }

    HILOGI("attributes.size: %{public}zu, values.size: %{public}zu, result: %{public}d",
        geaPkt->GetAttributes().size(), geaPkt->GetValues().size(), ExplainCrCodeToResult(geaPkt->GetCrCode()));

    switch (geaPkt->GetPacketType()) {
        case AVRC_CT_VENDOR_PACKET_TYPE:
        /// FALL THROUGH
        case AVRC_CT_VENDOR_PACKET_TYPE_END: {
            SendNextVendorCmd(rawAddr);

            myObserver_->onGetElementAttributes(
                rawAddr, geaPkt->GetAttributes(), geaPkt->GetValues(), ExplainCrCodeToResult(geaPkt->GetCrCode()));
            break;
        }
        case AVRC_CT_VENDOR_PACKET_TYPE_START:
        /// FALL THROUGH
        case AVRC_CT_VENDOR_PACKET_TYPE_CONTINUE: {
            if (packet->GetReceivedFragments() >= maxFragments_) {
                SendVendorAbortCmd(rawAddr, geaPkt->GetPduId());
                myObserver_->onGetElementAttributes(
                    rawAddr, geaPkt->GetAttributes(), geaPkt->GetValues(), ExplainCrCodeToResult(geaPkt->GetCrCode()));
            } else {
                cnManager->SetVendorContinuePacket(rawAddr, packet);
                SendVendorContinueCmd(rawAddr, geaPkt->GetPduId());
            }
            break;
        }
        default:
            HILOGI("Packet Type is wrong! packetType: %{public}d", geaPkt->GetPacketType());
            break;
    }
}

void AvrcCtProfile::SendGetPlayStatusCmd(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtGpsPacket>();
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendVendorCmd(rawAddr, packet, AVRC_CT_SM_EVENT_GET_PLAY_STATUS);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceiveGetPlayStatusRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    SendNextVendorCmd(rawAddr);

    AvrcCtGpsPacket packet(pkt);
    myObserver_->onGetPlayStatus(rawAddr,
        packet.GetSongLength(),
        packet.GetSongPosition(),
        packet.GetPlayStatus(),
        ExplainCrCodeToResult(packet.GetCrCode()));
}

void AvrcCtProfile::SendRequestContinuingResponseCmd(const RawAddress &rawAddr, uint8_t pduId)
{
    HILOGI("address: %{public}s, pduId: %{public}x", GET_ENCRYPT_AVRCP_ADDR(rawAddr), pduId);

    std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtRcrPacket>(pduId);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendVendorCmd(rawAddr, packet, AVRC_CT_SM_EVENT_REQUEST_CONTINUING_RESPONSE);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::SendAbortContinuingResponseCmd(const RawAddress &rawAddr, uint8_t pduId)
{
    HILOGI("address: %{public}s, pduId: %{public}x", GET_ENCRYPT_AVRCP_ADDR(rawAddr), pduId);

    std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtAcrPacket>(pduId);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendVendorCmd(rawAddr, packet, AVRC_CT_SM_EVENT_ABORT_CONTINUING_RESPONSE);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceiveAbortContinuingResponseRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtAcrPacket acrPkt(pkt);

    std::shared_ptr<AvrcCtVendorPacket> packet = AvrcCtConnectManager::GetInstance()->GetVendorContinuePacket(rawAddr);

    if (packet != nullptr) {
        if (acrPkt.GetPduId() != packet->GetPduId()) {
            HILOGI("PDU ID is wrong! Address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
        }
    } else {
        HILOGI("The saved continue packet is nullptr!");
    }

    SendNextVendorCmd(rawAddr);
}

void AvrcCtProfile::SendSetAddressedPlayerCmd(const RawAddress &rawAddr, uint16_t playerId)
{
    HILOGI("address: %{public}s, playerId: %{public}x", GET_ENCRYPT_AVRCP_ADDR(rawAddr), playerId);

    std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtSapPacket>(playerId);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendVendorCmd(rawAddr, packet, AVRC_CT_SM_EVENT_SET_ADDRESSED_PLAYER);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceiveSetAddressedPlayerRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    SendNextVendorCmd(rawAddr);

    AvrcCtSapPacket packet(pkt);

    int result = RET_BAD_STATUS;
    if (packet.GetStatus() == AVRC_ES_CODE_NO_ERROR) {
        result = RET_NO_ERROR;
    }

    myObserver_->onSetAddressedPlayer(rawAddr, result, packet.GetStatus());
}

void AvrcCtProfile::SendPlayItemCmd(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter)
{
    HILOGI("address: %{public}s, scope: %{public}x, uid: %{public}jx, uidCounter: %{public}hd",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope, uid, uidCounter);

    std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtPiPacket>(scope, uid, uidCounter);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendVendorCmd(rawAddr, packet, AVRC_CT_SM_EVENT_PLAY_ITEM);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceivePlayItemRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    SendNextVendorCmd(rawAddr);

    AvrcCtPiPacket packet(pkt);
    int result;
    if (packet.GetStatus() == AVRC_ES_CODE_NO_ERROR) {
        result = RET_NO_ERROR;
    } else {
        result = RET_BAD_STATUS;
    }
    myObserver_->onPlayItem(rawAddr, result, packet.GetStatus());
}

void AvrcCtProfile::SendAddToNowPlayingCmd(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter)
{
    HILOGI("address: %{public}s, scope: %{public}d, uid: %{public}ju, uidCounter: %{public}hu",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope, uid, uidCounter);

    std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtAtnpPacket>(scope, uid, uidCounter);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendVendorCmd(rawAddr, packet, AVRC_CT_SM_EVENT_ADD_TO_NOW_PLAYING);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceiveAddToNowPlayingRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    SendNextVendorCmd(rawAddr);

    AvrcCtAtnpPacket packet(pkt);

    int result = RET_BAD_STATUS;
    if (packet.GetStatus() == AVRC_ES_CODE_NO_ERROR) {
        result = RET_NO_ERROR;
    }

    myObserver_->onAddToNowPlaying(rawAddr, result, packet.GetStatus());
}

void AvrcCtProfile::SendSetAbsoluteVolumeCmd(const RawAddress &rawAddr, uint8_t volume)
{
    HILOGI("address: %{public}s, volume: %{public}x", GET_ENCRYPT_AVRCP_ADDR(rawAddr), volume);

    std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtSavPacket>(volume);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendVendorCmd(rawAddr, packet, AVRC_CT_SM_EVENT_SET_ABSOLUTE_VOLUME);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceiveSetAbsoluteVolumeRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    SendNextVendorCmd(rawAddr);

    AvrcCtSavPacket packet(pkt);
    myObserver_->onSetAbsoluteVolume(rawAddr, packet.GetAbsoluteVolume(), ExplainCrCodeToResult(packet.GetCrCode()));
}

void AvrcCtProfile::EnableNotification(const RawAddress &rawAddr, const std::vector<uint8_t> &events, uint32_t interval)
{
    HILOGI("address: %{public}s, events.size: %{public}zu", GET_ENCRYPT_AVRCP_ADDR(rawAddr), events.size());

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    for (auto event : events) {
        cnManager->EnableNotifyState(rawAddr, event);

        std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtNotifyPacket>(event);
        SendVendorCmd(rawAddr, packet, AVRC_CT_SM_EVENT_REGISTER_NOTIFICATION);
    }
}

void AvrcCtProfile::DisableNotification(const RawAddress &rawAddr, const std::vector<uint8_t> &events)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    for (auto event : events) {
        cnManager->DisableNotifyState(rawAddr, event);
    }
}

void AvrcCtProfile::ReceiveRegisterNotificationRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    std::shared_ptr<AvrcCtNotifyPacket> notifyPkt = std::make_shared<AvrcCtNotifyPacket>(pkt);
    uint8_t crCode = notifyPkt->GetCrCode();
    bool supported = false;
    if (cnManager->IsNotifyStateEnabled(rawAddr, notifyPkt->GetEventId()) &&
        crCode != AVRC_CT_RSP_CODE_NOT_IMPLEMENTED) {
        supported = true;
    }

    int result = AVRC_ES_CODE_NO_ERROR;
    if (crCode == AVRC_CT_RSP_CODE_CHANGED) {
        result = AVRC_ES_CODE_NOTIFICATION_CHANGED;
    }
    if (crCode == AVRC_CT_RSP_CODE_REJECTED) {
        result = AVRC_ES_CODE_INTERNAL_ERROR;
    }

    if ((notifyPkt->GetPacketType() == AVRC_CT_VENDOR_PACKET_TYPE ||
        notifyPkt->GetPacketType() == AVRC_CT_VENDOR_PACKET_TYPE_END)) {
        SendNextVendorCmd(rawAddr);
    }
    if (supported) {
        InformNotificationChanged(rawAddr, notifyPkt, result);
    }

    if (supported && crCode == AVRC_CT_RSP_CODE_CHANGED) {
        std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtNotifyPacket>(notifyPkt->GetEventId());
        SendVendorCmd(rawAddr, packet, AVRC_CT_SM_EVENT_REGISTER_NOTIFICATION);
    }
}

void AvrcCtProfile::InformNotificationChanged(
    const RawAddress &rawAddr, const std::shared_ptr<AvrcCtNotifyPacket> &notifyPkt, int result)
{
    HILOGI("address: %{public}s, result: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), result);

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();

    switch (notifyPkt->GetEventId()) {
        case AVRC_CT_EVENT_ID_PLAYBACK_STATUS_CHANGED:
            myObserver_->onPlaybackStatusChanged(rawAddr, notifyPkt->GetPlayStatus(), result);
            break;
        case AVRC_CT_EVENT_ID_TRACK_CHANGED:
            myObserver_->onTrackChanged(rawAddr, notifyPkt->GetUid(), result);
            break;
        case AVRC_CT_EVENT_ID_TRACK_REACHED_END:
            myObserver_->onTrackReachedEnd(rawAddr, result);
            break;
        case AVRC_CT_EVENT_ID_TRACK_REACHED_START:
            myObserver_->onTrackReachedStart(rawAddr, result);
            break;
        case AVRC_CT_EVENT_ID_PLAYBACK_POS_CHANGED:
            myObserver_->onPlaybackPosChanged(rawAddr, notifyPkt->GetPlaybackPosition(), result);
            break;
        case AVRC_CT_EVENT_ID_NOW_PLAYING_CONTENT_CHANGED:
            myObserver_->onNowPlayingContentChanged(rawAddr, result);
            break;
        case AVRC_CT_EVENT_ID_AVAILABLE_PLAYERS_CHANGED:
            myObserver_->onAvailablePlayersChanged(rawAddr, result);
            break;
        case AVRC_CT_EVENT_ID_ADDRESSED_PLAYER_CHANGED:
            cnManager->SetUidCounter(rawAddr, notifyPkt->GetUidCounter());
            myObserver_->onAddressedPlayerChanged(
                rawAddr, notifyPkt->GetPlayerId(), notifyPkt->GetUidCounter(), result);
            break;
        case AVRC_CT_EVENT_ID_UIDS_CHANGED:
            cnManager->SetUidCounter(rawAddr, notifyPkt->GetUidCounter());
            myObserver_->onUidChanged(rawAddr, notifyPkt->GetUidCounter(), result);
            break;
        case AVRC_CT_EVENT_ID_VOLUME_CHANGED:
            myObserver_->onVolumeChanged(rawAddr, notifyPkt->GetAbsoluteVolume(), result);
            break;
        case AVRC_CT_EVENT_ID_PLAYER_APPLICATION_SETTING_CHANGED:
            InformPlayerApplicationSettingChanged(rawAddr, notifyPkt, result);
            break;
        default:
            HILOGI("The event id is wrong! event: %{public}d", notifyPkt->GetEventId());
            break;
    }
}

void AvrcCtProfile::InformPlayerApplicationSettingChanged(
    const RawAddress &rawAddr, const std::shared_ptr<AvrcCtNotifyPacket> &notifyPkt, int result)
{
    HILOGI("address: %{public}s, result: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), result);

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();

    if (notifyPkt->GetPacketType() == AVRC_CT_VENDOR_PACKET_TYPE_START ||
        notifyPkt->GetPacketType() == AVRC_CT_VENDOR_PACKET_TYPE_CONTINUE) {
        std::shared_ptr<AvrcCtVendorPacket> packet = cnManager->GetVendorContinuePacket(rawAddr);
        if (packet == nullptr) {
            packet = notifyPkt;
        }

        if (packet->GetReceivedFragments() >= maxFragments_) {
            SendVendorAbortCmd(rawAddr, notifyPkt->GetPduId());
            myObserver_->onVolumeChanged(rawAddr, notifyPkt->GetAbsoluteVolume(), result);
        } else {
            cnManager->SetVendorContinuePacket(rawAddr, packet);
            SendVendorContinueCmd(rawAddr, notifyPkt->GetPduId());
        }
    } else {
        myObserver_->onPlayerApplicationSettingChanged(
            rawAddr, notifyPkt->GetAttributes(), notifyPkt->GetValues(), result);
    }
}

void AvrcCtProfile::ReceiveVendorRspAvcStatus(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    switch (AvrcCtPacket::GetVendorPdu(pkt)) {
        case AVRC_CT_PDU_ID_GET_CAPABILITIES:
            ReceiveGetCapabilitiesRsp(rawAddr, pkt);
            break;
        case AVRC_CT_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES:
            ReceiveListPlayerApplicationSettingAttributesRsp(rawAddr, pkt);
            break;
        case AVRC_CT_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_VALUES:
            ReceiveListPlayerApplicationSettingValuesRsp(rawAddr, pkt);
            break;
        case AVRC_CT_PDU_ID_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE:
            ReceiveGetCurrentPlayerApplicationSettingValueRsp(rawAddr, pkt);
            break;
        case AVRC_CT_PDU_ID_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT:
            ReceiveGetPlayerApplicationSettingAttributeTextRsp(rawAddr, pkt);
            break;
        case AVRC_CT_PDU_ID_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT:
            ReceiveGetPlayerApplicationSettingValueTextRsp(rawAddr, pkt);
            break;
        case AVRC_CT_PDU_ID_GET_ELEMENT_ATTRIBUTES:
            ReceiveGetElementAttributesRsp(rawAddr, pkt);
            break;
        case AVRC_CT_PDU_ID_GET_PLAY_STATUS:
            ReceiveGetPlayStatusRsp(rawAddr, pkt);
            break;
        default:
            break;
    }
}

void AvrcCtProfile::ReceiveVendorRspAvcControl(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    switch (AvrcCtPacket::GetVendorPdu(pkt)) {
        case AVRC_CT_PDU_ID_SET_PLAYER_APPLICATION_SETTING_VALUE:
            ReceiveSetPlayerApplicationSettingValueRsp(rawAddr, pkt);
            break;
        case AVRC_CT_PDU_ID_ABORT_CONTINUING_RESPONSE:
            ReceiveAbortContinuingResponseRsp(rawAddr, pkt);
            break;
        case AVRC_CT_PDU_ID_SET_ADDRESSED_PLAYER:
            ReceiveSetAddressedPlayerRsp(rawAddr, pkt);
            break;
        case AVRC_CT_PDU_ID_PLAY_ITEM:
            ReceivePlayItemRsp(rawAddr, pkt);
            break;
        case AVRC_CT_PDU_ID_ADD_TO_NOW_PLAYING:
            ReceiveAddToNowPlayingRsp(rawAddr, pkt);
            break;
        case AVRC_CT_PDU_ID_SET_ABSOLUTE_VOLUME:
            ReceiveSetAbsoluteVolumeRsp(rawAddr, pkt);
            break;
        default:
            break;
    }
}

void AvrcCtProfile::ReceiveVendorRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    switch (AvrcCtPacket::GetVendorPdu(pkt)) {
        case AVRC_CT_PDU_ID_GET_CAPABILITIES:
        case AVRC_CT_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES:
        case AVRC_CT_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_VALUES:
        case AVRC_CT_PDU_ID_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE:
        case AVRC_CT_PDU_ID_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT:
        case AVRC_CT_PDU_ID_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT:
        case AVRC_CT_PDU_ID_GET_ELEMENT_ATTRIBUTES:
        case AVRC_CT_PDU_ID_GET_PLAY_STATUS:
            ReceiveVendorRspAvcStatus(rawAddr, pkt);
            break;
        case AVRC_CT_PDU_ID_SET_PLAYER_APPLICATION_SETTING_VALUE:
        case AVRC_CT_PDU_ID_ABORT_CONTINUING_RESPONSE:
        case AVRC_CT_PDU_ID_SET_ADDRESSED_PLAYER:
        case AVRC_CT_PDU_ID_PLAY_ITEM:
        case AVRC_CT_PDU_ID_ADD_TO_NOW_PLAYING:
        case AVRC_CT_PDU_ID_SET_ABSOLUTE_VOLUME:
            ReceiveVendorRspAvcControl(rawAddr, pkt);
            break;
        case AVRC_CT_PDU_ID_REGISTER_NOTIFICATION:
            ReceiveRegisterNotificationRsp(rawAddr, pkt);
            break;
        default:
            HILOGI("The PDU ID is wrong! Address: %{public}s, pduId: %{public}x",
                GET_ENCRYPT_AVRCP_ADDR(rawAddr), AvrcCtPacket::GetVendorPdu(pkt));
            break;
    }
}

void AvrcCtProfile::ProcessVendorNotificationTimeout(RawAddress rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    AvrcCtNotifyPacket *pkt = static_cast<AvrcCtNotifyPacket *>(cnManager->GetVendorPacket(rawAddr).get());
    uint8_t event = pkt->GetEventId();
    if (cnManager->IsNotifyStateEnabled(rawAddr, event)) {
        std::shared_ptr<AvrcCtVendorPacket> packet = std::make_shared<AvrcCtNotifyPacket>(event);
        SendVendorCmd(rawAddr, packet, AVRC_CT_SM_EVENT_REGISTER_NOTIFICATION);
    }
}

void AvrcCtProfile::ProcessVendorAvcControlTimeout(RawAddress rawAddr, const std::shared_ptr<AvrcCtVendorPacket> &packet)
{
    switch (packet->GetPduId()) {
        case AVRC_CT_PDU_ID_SET_PLAYER_APPLICATION_SETTING_VALUE: {
            myObserver_->onSetPlayerApplicationSettingValue(rawAddr, RET_BAD_STATUS);
            break;
        }
        case AVRC_CT_PDU_ID_REQUEST_CONTINUING_RESPONSE:
        /// FALL THROUGH
        case AVRC_CT_PDU_ID_ABORT_CONTINUING_RESPONSE: {
            AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
            std::shared_ptr<AvrcCtVendorPacket> pkt = cnManager->GetVendorContinuePacket(rawAddr);
            cnManager->SetVendorPacket(rawAddr, pkt);
            ProcessVendorTimeout(rawAddr);
            break;
        }
        case AVRC_CT_PDU_ID_SET_ADDRESSED_PLAYER: {
            myObserver_->onSetAddressedPlayer(rawAddr, RET_BAD_STATUS, AVRC_ES_CODE_INTERNAL_ERROR);
            break;
        }
        case AVRC_CT_PDU_ID_PLAY_ITEM: {
            AvrcCtPiPacket *pkt = static_cast<AvrcCtPiPacket *>(packet.get());
            myObserver_->onPlayItem(rawAddr, pkt->GetStatus(), RET_BAD_STATUS);
            break;
        }
        case AVRC_CT_PDU_ID_SET_ABSOLUTE_VOLUME: {
            AvrcCtSavPacket *pkt = static_cast<AvrcCtSavPacket *>(packet.get());
            myObserver_->onSetAbsoluteVolume(rawAddr, pkt->GetAbsoluteVolume(), RET_BAD_STATUS);
            break;
        }
        case AVRC_CT_PDU_ID_ADD_TO_NOW_PLAYING: {
            myObserver_->onAddToNowPlaying(rawAddr, RET_BAD_STATUS, AVRC_ES_CODE_INTERNAL_ERROR);
            break;
        }
        default:
            break;
    }
}

void AvrcCtProfile::ProcessVendorAvcStatus1Timeout(RawAddress rawAddr, const std::shared_ptr<AvrcCtVendorPacket> &packet)
{
    switch (packet->GetPduId()) {
        case AVRC_CT_PDU_ID_GET_CAPABILITIES: {
            AvrcCtGcPacket *gcPkt = static_cast<AvrcCtGcPacket *>(packet.get());
            myObserver_->onGetCapabilities(rawAddr, gcPkt->GetCompanies(), gcPkt->GetEvents(), RET_BAD_STATUS);
            break;
        }
        case AVRC_CT_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES: {
            AvrcCtLpasaPacket *pkt = static_cast<AvrcCtLpasaPacket *>(packet.get());
            myObserver_->onListPlayerApplicationSettingAttributes(rawAddr, pkt->GetAttributes(), RET_BAD_STATUS);
            break;
        }
        case AVRC_CT_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_VALUES: {
            AvrcCtLpasvPacket *pkt = static_cast<AvrcCtLpasvPacket *>(packet.get());
            myObserver_->onListPlayerApplicationSettingValues(
                rawAddr, pkt->GetAttribute(), pkt->GetValues(), RET_BAD_STATUS);
            break;
        }
        case AVRC_CT_PDU_ID_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE: {
            AvrcCtGcpasvPacket *pkt = static_cast<AvrcCtGcpasvPacket *>(packet.get());
            myObserver_->onGetCurrentPlayerApplicationSettingValue(
                rawAddr, pkt->GetAttributes(), pkt->GetValues(), RET_BAD_STATUS);
            break;
        }

        default:
            break;
    }
}

void AvrcCtProfile::ProcessVendorAvcStatus2Timeout(RawAddress rawAddr, const std::shared_ptr<AvrcCtVendorPacket> &packet)
{
    switch (packet->GetPduId()) {
        case AVRC_CT_PDU_ID_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT: {
            AvrcCtGpasatPacket *gpasatPkt = static_cast<AvrcCtGpasatPacket *>(packet.get());
            myObserver_->onGetPlayerApplicationSettingAttributeText(
                rawAddr, gpasatPkt->GetAttributes(), gpasatPkt->GetAttributeName(), RET_BAD_STATUS);
            break;
        }
        case AVRC_CT_PDU_ID_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT: {
            AvrcCtGpasvtPacket *gpasvtPkt = static_cast<AvrcCtGpasvtPacket *>(packet.get());
            myObserver_->onGetPlayerApplicationSettingAttributeText(
                rawAddr, gpasvtPkt->GetValues(), gpasvtPkt->GetValueName(), RET_BAD_STATUS);
            break;
        }
        case AVRC_CT_PDU_ID_GET_ELEMENT_ATTRIBUTES: {
            AvrcCtGeaPacket *geaPkt = static_cast<AvrcCtGeaPacket *>(packet.get());
            myObserver_->onGetElementAttributes(rawAddr, geaPkt->GetAttributes(), geaPkt->GetValues(), RET_BAD_STATUS);
            break;
        }
        case AVRC_CT_PDU_ID_GET_PLAY_STATUS: {
            AvrcCtGpsPacket *pkt = static_cast<AvrcCtGpsPacket *>(packet.get());
            myObserver_->onGetPlayStatus(
                rawAddr, pkt->GetSongLength(), pkt->GetSongPosition(), pkt->GetPlayStatus(), RET_BAD_STATUS);
            break;
        }
        default:
            break;
    }
}

void AvrcCtProfile::ProcessVendorTimeout(RawAddress rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();

    if (IsEnabled()) {
        std::shared_ptr<AvrcCtVendorPacket> packet = cnManager->GetVendorPacket(rawAddr);
        if (packet != nullptr) {
            switch (packet->GetPduId()) {
                case AVRC_CT_PDU_ID_GET_CAPABILITIES:
                case AVRC_CT_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES:
                case AVRC_CT_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_VALUES:
                case AVRC_CT_PDU_ID_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE:
                    ProcessVendorAvcStatus1Timeout(rawAddr, packet);
                    break;
                case AVRC_CT_PDU_ID_SET_PLAYER_APPLICATION_SETTING_VALUE:
                case AVRC_CT_PDU_ID_REQUEST_CONTINUING_RESPONSE:
                /// FALL THROUGH
                case AVRC_CT_PDU_ID_ABORT_CONTINUING_RESPONSE:
                case AVRC_CT_PDU_ID_SET_ADDRESSED_PLAYER:
                case AVRC_CT_PDU_ID_PLAY_ITEM:
                case AVRC_CT_PDU_ID_SET_ABSOLUTE_VOLUME:
                case AVRC_CT_PDU_ID_ADD_TO_NOW_PLAYING:
                    ProcessVendorAvcControlTimeout(rawAddr, packet);
                    break;
                case AVRC_CT_PDU_ID_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT:
                case AVRC_CT_PDU_ID_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT:
                case AVRC_CT_PDU_ID_GET_ELEMENT_ATTRIBUTES:
                case AVRC_CT_PDU_ID_GET_PLAY_STATUS:
                    ProcessVendorAvcStatus2Timeout(rawAddr, packet);
                    break;
                case AVRC_CT_PDU_ID_REGISTER_NOTIFICATION: {
                    ProcessVendorNotificationTimeout(rawAddr);
                    break;
                }
                default:
                    HILOGI("The PDU ID is wrong! Address: %{public}s, pduId: %{public}x",
                        GET_ENCRYPT_AVRCP_ADDR(rawAddr), packet->GetPduId());
                    break;
            }
        } else {
            HILOGI("The saved packet is nullptr! Address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
        }

        SendNextVendorCmd(rawAddr);
    }
}

void AvrcCtProfile::VendorTimeoutCallback(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    if (IsEnabled()) {
        dispatcher_->PostTask(std::bind(&AvrcCtProfile::ProcessVendorTimeout, this, rawAddr));
    }
}

/******************************************************************
 * BROWSING COMMAND                                               *
 ******************************************************************/

void AvrcCtProfile::SendBrowseCmd(
    const RawAddress &rawAddr, const std::shared_ptr<AvrcCtBrowsePacket> &pkt, AvrcCtSmEvent event)
{
    HILOGI("address: %{public}s, event%{public}x", GET_ENCRYPT_AVRCP_ADDR(rawAddr), event);

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    AvrcCtStateMachineManager *smManager = AvrcCtStateMachineManager::GetInstance();

    if (!smManager->IsBrowsePendingState(rawAddr)) {
        cnManager->ClearBrowseInfo(rawAddr);
        cnManager->SetBrowsePacket(rawAddr, pkt);

        utility::Message msg(event);
        AvrcCtStateMachineManager::GetInstance()->SendMessageToBrowseStateMachine(rawAddr, msg);

        auto func = std::bind(&AvrcCtProfile::BrowseTimeoutCallback, this, rawAddr);
        cnManager->SetBrowseTimer(rawAddr, func, AVRC_CT_TIMER_T_MTP);
    } else {
        if (cnManager->GetSizeOfBrowseQueue(rawAddr) > AVRC_CT_DEFAULT_SIZE_OF_QUEUE) {
            HILOGI("The queue is oversize: %{public}d", cnManager->GetSizeOfBrowseQueue(rawAddr));
        } else {
            cnManager->PushBrowseQueue(rawAddr, pkt);
            HILOGI("Waiting for the response! Address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
        }
    }
}

void AvrcCtProfile::SendNextBrowseCmd(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    cnManager->ClearBrowseInfo(rawAddr);

    utility::Message msg(AVRC_CT_SM_EVENT_TO_CONNECTED_STATE);
    AvrcCtStateMachineManager::GetInstance()->SendMessageToBrowseStateMachine(rawAddr, msg);

    if (cnManager->GetSizeOfBrowseQueue(rawAddr) != 0x00) {
        std::shared_ptr<AvrcCtBrowsePacket> packet = cnManager->PopBrowseQueue(rawAddr);
        AvrcCtSmEvent event = static_cast<AvrcCtSmEvent>(packet->GetPduId());

        SendBrowseCmd(rawAddr, packet, event);
    }
}

bool AvrcCtProfile::IsBrowseQueueFull(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    return (AvrcCtConnectManager::GetInstance()->GetSizeOfBrowseQueue(rawAddr) == AVRC_CT_DEFAULT_SIZE_OF_QUEUE);
}

bool AvrcCtProfile::IsDisableAbsoluteVolume(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    return AvrcCtConnectManager::GetInstance()->IsDisableAbsoluteVolume(rawAddr);
}

bool AvrcCtProfile::IsBrowsingConnected(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    return AvrcCtConnectManager::GetInstance()->IsBrowsingConnected(rawAddr);
}

void AvrcCtProfile::SendSetBrowsedPlayerCmd(const RawAddress &rawAddr, uint16_t playerId)
{
    HILOGI("address: %{public}s, playerId: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), playerId);

    std::shared_ptr<AvrcCtBrowsePacket> packet = std::make_shared<AvrcCtSbpPacket>(playerId);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendBrowseCmd(rawAddr, packet, AVRC_CT_SM_EVENT_SET_BROWSED_PLAYER);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceiveSetBrowsedPlayerRsp(const RawAddress &rawAddr, Packet *pkt) const
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtSbpPacket packet(pkt);

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    cnManager->SetUidCounter(rawAddr, packet.GetUidCounter());

    int result = RET_BAD_STATUS;
    if (packet.GetStatus() == AVRC_ES_CODE_NO_ERROR) {
        result = RET_NO_ERROR;
    }

    myObserver_->onSetBrowsedPlayer(
        rawAddr, packet.GetUidCounter(), packet.GetNumOfItems(), packet.GetFolderNames(), result, packet.GetStatus());
}

void AvrcCtProfile::SendChangePathCmd(
    const RawAddress &rawAddr, uint16_t uidCounter, uint8_t direction, uint64_t folderUid)
{
    HILOGI("address: %{public}s, uidCounter: %{public}d, direction: %{public}x, folderUid: %{public}jx",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, direction, folderUid);

    std::shared_ptr<AvrcCtBrowsePacket> packet = std::make_shared<AvrcCtCpPacket>(uidCounter, direction, folderUid);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendBrowseCmd(rawAddr, packet, AVRC_CT_SM_EVENT_CHANGE_PATH);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceiveChangePathRsp(const RawAddress &rawAddr, Packet *pkt) const
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtCpPacket packet(pkt);

    int result = RET_BAD_STATUS;
    if (packet.GetStatus() == AVRC_ES_CODE_NO_ERROR) {
        result = RET_NO_ERROR;
    }

    myObserver_->onChangePath(rawAddr, packet.GetNumOfItems(), result, packet.GetStatus());
}

void AvrcCtProfile::SendGetFolderItemsCmd(const RawAddress &rawAddr, uint8_t scope, uint32_t startItem,
    uint32_t endItem, const std::vector<uint32_t> &attributes)
{
    HILOGI("addr:%{public}s, scope:%{public}d, startItem:%{public}d, endItem:%{public}d, attributes.size:%{public}zu",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope, startItem, endItem, attributes.size());

    std::shared_ptr<AvrcCtBrowsePacket> packet =
        std::make_shared<AvrcCtGfiPacket>(scope, startItem, endItem, attributes);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendBrowseCmd(rawAddr, packet, AVRC_CT_SM_EVENT_GET_ITEM_ATTRIBUTES);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceiveGetFolderItemsRsp(const RawAddress &rawAddr, Packet *pkt) const
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtGfiPacket packet(pkt);

    int result = RET_BAD_STATUS;
    if (packet.GetStatus() == AVRC_ES_CODE_NO_ERROR) {
        result = RET_NO_ERROR;
    }

    myObserver_->onGetFolderItems(rawAddr,
        packet.GetScope(),
        packet.GetUidCounter(),
        packet.GetMediaPlayers(),
        packet.GetMediaItems(),
        result,
        packet.GetStatus());
}

void AvrcCtProfile::SendGetItemAttributesCmd(const RawAddress &rawAddr, uint8_t scope, uint64_t uid,
    uint16_t uidCounter, const std::vector<uint32_t> &attributes)
{
    HILOGI("addr: %{public}s, scope: %{public}x, uid: %{public}jx, uidCounter: %{public}hu",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope, uid, uidCounter);

    std::shared_ptr<AvrcCtBrowsePacket> packet = std::make_shared<AvrcCtGiaPacket>(scope, uid, uidCounter, attributes);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendBrowseCmd(rawAddr, packet, AVRC_CT_SM_EVENT_GET_ITEM_ATTRIBUTES);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceiveGetItemAttributesRsp(const RawAddress &rawAddr, Packet *pkt) const
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtGiaPacket packet(pkt);

    int result = RET_BAD_STATUS;
    if (packet.GetStatus() == AVRC_ES_CODE_NO_ERROR) {
        result = RET_NO_ERROR;
    }

    myObserver_->onGetItemAttributes(rawAddr, packet.GetAttributes(), packet.GetValues(), result, packet.GetStatus());
}

void AvrcCtProfile::SendGetTotalNumberOfItemsCmd(const RawAddress &rawAddr, uint8_t scope)
{
    HILOGI("address: %{public}s, scope: %{public}x", GET_ENCRYPT_AVRCP_ADDR(rawAddr), scope);

    std::shared_ptr<AvrcCtBrowsePacket> packet = std::make_shared<AvrcCtGtnoiPacket>(scope);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);
    SendBrowseCmd(rawAddr, packet, AVRC_CT_SM_EVENT_GET_TOTAL_NUMBER_OF_ITEMS);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ReceiveGetTotalNumberOfItemsRsp(const RawAddress &rawAddr, Packet *pkt) const
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtGtnoiPacket packet(pkt);

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    cnManager->SetUidCounter(rawAddr, packet.GetUidCounter());

    int result = RET_BAD_STATUS;
    if (packet.GetStatus() == AVRC_ES_CODE_NO_ERROR) {
        result = RET_NO_ERROR;
    }

    myObserver_->onGetTotalNumberOfItems(
        rawAddr, packet.GetUidCounter(), packet.GetNumOfItems(), result, packet.GetStatus());
}

void AvrcCtProfile::ReceiveBrowseRsp(const RawAddress &rawAddr, Packet *pkt)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    SendNextBrowseCmd(rawAddr);

    switch (AvrcCtPacket::GetBrowsePdu(pkt)) {
        case AVRC_CT_PDU_ID_SET_BROWSED_PLAYER:
            ReceiveSetBrowsedPlayerRsp(rawAddr, pkt);
            break;
        case AVRC_CT_PDU_ID_CHANGE_PATH:
            ReceiveChangePathRsp(rawAddr, pkt);
            break;
        case AVRC_CT_PDU_ID_GET_FOLDER_ITEMS:
            ReceiveGetFolderItemsRsp(rawAddr, pkt);
            break;
        case AVRC_CT_PDU_ID_GET_ITEM_ATTRIBUTES:
            ReceiveGetItemAttributesRsp(rawAddr, pkt);
            break;
        case AVRC_CT_PDU_ID_GET_TOTAL_NUMBER_OF_ITEMS:
            ReceiveGetTotalNumberOfItemsRsp(rawAddr, pkt);
            break;
        default:
            HILOGI("The PDU ID is wrong! Address: %{public}s, pduId: %{public}x",
                GET_ENCRYPT_AVRCP_ADDR(rawAddr), AvrcCtPacket::GetBrowsePdu(pkt));
            break;
    }
}

void AvrcCtProfile::ProcessBrowseTimeout(RawAddress rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    if (IsEnabled()) {
        std::shared_ptr<AvrcCtBrowsePacket> packet = AvrcCtConnectManager::GetInstance()->GetBrowsePacket(rawAddr);
        if (packet != nullptr) {
            switch (packet->GetPduId()) {
                case AVRC_CT_PDU_ID_SET_BROWSED_PLAYER: {
                    AvrcCtSbpPacket *pkt = static_cast<AvrcCtSbpPacket *>(packet.get());
                    myObserver_->onSetBrowsedPlayer(rawAddr,
                        pkt->GetUidCounter(),
                        0x00,
                        pkt->GetFolderNames(),
                        RET_BAD_STATUS,
                        AVRC_ES_CODE_INTERNAL_ERROR);
                    break;
                }
                case AVRC_CT_PDU_ID_CHANGE_PATH:
                    myObserver_->onChangePath(rawAddr, 0x00, RET_BAD_STATUS, AVRC_ES_CODE_INTERNAL_ERROR);
                    break;
                case AVRC_CT_PDU_ID_GET_FOLDER_ITEMS: {
                    AvrcCtGfiPacket *pkt = static_cast<AvrcCtGfiPacket *>(packet.get());
                    myObserver_->onGetFolderItems(rawAddr,
                        pkt->GetScope(),
                        pkt->GetUidCounter(),
                        pkt->GetMediaPlayers(),
                        pkt->GetMediaItems(),
                        RET_BAD_STATUS,
                        AVRC_ES_CODE_INTERNAL_ERROR);
                    break;
                }
                case AVRC_CT_PDU_ID_GET_ITEM_ATTRIBUTES: {
                    AvrcCtGiaPacket *pkt = static_cast<AvrcCtGiaPacket *>(packet.get());
                    myObserver_->onGetItemAttributes(
                        rawAddr, pkt->GetAttributes(), pkt->GetValues(), RET_BAD_STATUS, AVRC_ES_CODE_INTERNAL_ERROR);
                    break;
                }
                case AVRC_CT_PDU_ID_GET_TOTAL_NUMBER_OF_ITEMS:
                    myObserver_->onGetTotalNumberOfItems(
                        rawAddr, 0x00, 0x00, RET_BAD_STATUS, AVRC_ES_CODE_INTERNAL_ERROR);
                    break;
                default:
                    HILOGI("The PDU ID is wrong! Address: %{public}s, pduId: %{public}x",
                        GET_ENCRYPT_AVRCP_ADDR(rawAddr), packet->GetPduId());
                    break;
            }
        }
        SendNextBrowseCmd(rawAddr);
    }
}

void AvrcCtProfile::BrowseTimeoutCallback(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    if (IsEnabled()) {
        dispatcher_->PostTask(std::bind(&AvrcCtProfile::ProcessBrowseTimeout, this, rawAddr));
    }
}

void AvrcCtProfile::ProcessChannelEvent(
    const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context) const
{
    HILOGI("Receive: %{public}s, connectId: %{public}d, Result: %{public}x, Address: %{public}s",
        GetEventName(event).c_str(), connectId, result, GET_ENCRYPT_AVRCP_ADDR(rawAddr));
    switch (event) {
        case AVCT_CONNECT_IND_EVT:
            ProcessChannelEventConnectIndEvt(rawAddr, connectId, event, result, context);
            break;
        case AVCT_CONNECT_CFM_EVT:
            ProcessChannelEventConnectCfmEvt(rawAddr, connectId, event, result, context);
            break;
        case AVCT_DISCONNECT_IND_EVT:
            ProcessChannelEventDisconnectIndEvt(rawAddr, connectId, event, result, context);
            break;
        case AVCT_DISCONNECT_CFM_EVT:
            ProcessChannelEventDisconnectCfmEvt(rawAddr, connectId, event, result, context);
            break;
        case AVCT_BR_CONNECT_IND_EVT:
            ProcessChannelEventBrConnectIndEvt(rawAddr, connectId, event, result, context);
            break;
        case AVCT_BR_CONNECT_CFM_EVT:
            ProcessChannelEventBrConnectCfmEvt(rawAddr, connectId, event, result, context);
            break;
        case AVCT_BR_DISCONNECT_IND_EVT:
            DeleteBrowseStateMachine(rawAddr);
            break;
        case AVCT_BR_DISCONNECT_CFM_EVT:
            DeleteBrowseStateMachine(rawAddr);
            break;
        default:
            break;
    };
}

void AvrcCtProfile::ProcessChannelEventConnectIndEvt(
    const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context) const
{
    HILOGI("Receive: %{public}s, connectId: %{public}d, Result: %{public}x, Address: %{public}s",
        GetEventName(event).c_str(), connectId, result, GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    result = ExpainAvctResult(result);

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    AvrcCtStateMachineManager *smManager = AvrcCtStateMachineManager::GetInstance();
    utility::Message msg(AVRC_CT_SM_EVENT_INVALID);

    if (result == RET_NO_ERROR) {
        bool absVolume =
            Compat::CompatCheck(bluetooth::CompatType::COMPAT_REJECT_ABSOLUTE_VOLUME, rawAddr.GetAddress());
        result |= cnManager->Add(rawAddr,
            connectId,
            AVCT_ACPT,
            controlMtu_,
            browseMtu_,
            companyId_,
            0x0000,
            absVolume,
            eventCallback_,
            msgCallback_);
        result |= smManager->AddControlStateMachine(rawAddr);
        if (result == RET_NO_ERROR) {
            msg.what_ = AVRC_CT_SM_EVENT_TO_CONNECTED_STATE;
            AvrcCtStateMachineManager::GetInstance()->SendMessageToControlStateMachine(rawAddr, msg);

            myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::CONNECTED));
            IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_ON, PROFILE_NAME_AVRCP_CT, rawAddr);
        }
    }
}

void AvrcCtProfile::ProcessChannelEventConnectCfmEvt(
    const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context) const
{
    HILOGI("Receive: %{public}s, connectId: %{public}d, Result: %{public}x, Address: %{public}s",
        GetEventName(event).c_str(), connectId, result, GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    AvrcCtStateMachineManager *smManager = AvrcCtStateMachineManager::GetInstance();
    utility::Message msg(AVRC_CT_SM_EVENT_INVALID);

    if (result == RET_NO_ERROR) {
        cnManager->SetConnectId(rawAddr, connectId);

        msg.what_ = AVRC_CT_SM_EVENT_TO_CONNECTED_STATE;
        smManager->SendMessageToControlStateMachine(rawAddr, msg);

        if (IsSupportedBrowsing()) {
            ConnectBr(rawAddr);
        }
        cnManager->DeleteDisconnectedDevice(rawAddr.GetAddress());
        if (!IsSupportedBrowsing()) {
            myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::CONNECTED));
            IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_ON, PROFILE_NAME_AVRCP_CT, rawAddr);
        }
    } else {
        DeleteResource(rawAddr);
        myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::DISCONNECTED));
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_OFF, PROFILE_NAME_AVRCP_CT, rawAddr);
    }
}

void AvrcCtProfile::ProcessChannelEventDisconnectIndEvt(
    const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context) const
{
    HILOGI("Receive: %{public}s, connectId: %{public}d, Result: %{public}x, Address: %{public}s",
        GetEventName(event).c_str(), connectId, result, GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();

    myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::DISCONNECTING));
    DeleteResource(rawAddr);
    cnManager->AddDisconnectedDevice(rawAddr.GetAddress());
    myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::DISCONNECTED));
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_OFF, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::ProcessChannelEventDisconnectCfmEvt(
    const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context) const
{
    HILOGI("Receive: %{public}s, connectId: %{public}d, Result: %{public}x, Address: %{public}s",
        GetEventName(event).c_str(), connectId, result, GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    AvrcCtStateMachineManager *smManager = AvrcCtStateMachineManager::GetInstance();
    utility::Message msg(AVRC_CT_SM_EVENT_INVALID);

    if (smManager->IsControlDisableState(rawAddr)) {
        DeleteResource(rawAddr);
        if (cnManager->IsConnectInfoEmpty()) {
            AvrcpCtSafeDelete(cnManager);
            smManager->ShutDown();
            myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::DISCONNECTED));
            IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_OFF, PROFILE_NAME_AVRCP_CT, rawAddr);
            myObserver_->onDisabled();
        }
    } else {
        DeleteResource(rawAddr);
        cnManager->AddDisconnectedDevice(rawAddr.GetAddress());
        myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::DISCONNECTED));
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_OFF, PROFILE_NAME_AVRCP_CT, rawAddr);
    }
}

void AvrcCtProfile::ProcessChannelEventBrConnectIndEvt(
    const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context) const
{
    HILOGI("Receive: %{public}s, connectId: %{public}d, Result: %{public}x, Address: %{public}s",
        GetEventName(event).c_str(), connectId, result, GET_ENCRYPT_AVRCP_ADDR(rawAddr));
    result = ExpainAvctResult(result);

    AvrcCtStateMachineManager *smManager = AvrcCtStateMachineManager::GetInstance();
    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    utility::Message msg(AVRC_CT_SM_EVENT_INVALID);

    if (result == RET_NO_ERROR) {
        result |= smManager->AddBrowseStateMachine(rawAddr);
        if (result == RET_NO_ERROR) {
            msg.what_ = AVRC_CT_SM_EVENT_TO_CONNECTED_STATE;
            smManager->SendMessageToBrowseStateMachine(rawAddr, msg);
            cnManager->SetBrowsingState(rawAddr, true);
        }
    }
    if (IsSupportedBrowsing()) {
        myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::CONNECTED));
    }
}

void AvrcCtProfile::ProcessChannelEventBrConnectCfmEvt(
    const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context) const
{
    HILOGI("Receive: %{public}s, connectId: %{public}d, Result: %{public}x, Address: %{public}s",
        GetEventName(event).c_str(), connectId, result, GET_ENCRYPT_AVRCP_ADDR(rawAddr));
    AvrcCtStateMachineManager *smManager = AvrcCtStateMachineManager::GetInstance();
    AvrcCtConnectManager *cnManager = AvrcCtConnectManager::GetInstance();
    utility::Message msg(AVRC_CT_SM_EVENT_INVALID);

    if (result == RET_NO_ERROR) {
        msg.what_ = AVRC_CT_SM_EVENT_TO_CONNECTED_STATE;
        smManager->SendMessageToBrowseStateMachine(rawAddr, msg);
        cnManager->SetBrowsingState(rawAddr, true);
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_ON, PROFILE_NAME_AVRCP_CT, rawAddr);
    } else {
        DeleteBrowseStateMachine(rawAddr);
    }
    if (IsSupportedBrowsing()) {
        myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::CONNECTED));
    }
}

void AvrcCtProfile::ProcessChannelMessage(
    uint8_t connectId, uint8_t label, uint8_t crType, uint8_t chType, Packet *pkt, void *context)
{
    HILOGI("connectId: %{public}d, label: %{public}d, crType: %{public}d, chType: %{public}d",
        connectId, label, crType, chType);

    RawAddress rawAddr = AvrcCtConnectManager::GetInstance()->GetRawAddress(connectId);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_CT, rawAddr);

    if (chType == AVCT_DATA_CTRL) {
        uint8_t opCode = AvrcCtPacket::GetOpCode(pkt);
        HILOGI("opCode: %{public}x", opCode);
        switch (opCode) {
            case AVRC_CT_OP_CODE_PASS_THROUGH:
                ReceivePassRsp(rawAddr, pkt);
                break;
            case AVRC_CT_OP_CODE_UNIT_INFO:
                ReceiveUnitRsp(rawAddr, pkt);
                break;
            case AVRC_CT_OP_CODE_SUB_UNIT_INFO:
                ReceiveSubUnitRsp(rawAddr, pkt);
                break;
            case AVRC_CT_OP_CODE_VENDOR:
                ReceiveVendorRsp(rawAddr, pkt);
                break;
            default:
                HILOGI("opCode: %{public}x is wrong! ConnectId: %{public}x", opCode, connectId);
                break;
        }
    } else if (chType == AVCT_DATA_BR) {
        ReceiveBrowseRsp(rawAddr, pkt);
    } else {
        HILOGI("chType: %{public}x is wrong! ConnectId: %{public}x", chType, connectId);
    }

    PacketFree(pkt);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_CT, rawAddr);
}

void AvrcCtProfile::DeleteResource(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtConnectManager::GetInstance()->Delete(rawAddr);
    AvrcCtStateMachineManager::GetInstance()->DeletePairOfStateMachine(rawAddr);
}

void AvrcCtProfile::DeleteBrowseStateMachine(const RawAddress &rawAddr)
{
    HILOGI("address: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcCtStateMachineManager::GetInstance()->DeleteBrowseStateMachine(rawAddr);
}

int AvrcCtProfile::ExpainAvctResult(uint16_t avctRet)
{
    HILOGI("avctRet: %{public}d", avctRet);
    int result = RET_BAD_STATUS;

    switch (avctRet) {
        case AVCT_SUCCESS:
            result = RET_NO_ERROR;
            break;
        case AVCT_FAILED:
            /// FALL THROUGH
        default:
            /// Do nothing!
            break;
    }

    return result;
}

int AvrcCtProfile::ExpainPassCrCodeToResult(uint8_t code)
{
    HILOGI("code: %{public}d", code);
    int result = RET_BAD_STATUS;

    switch (code) {
        case AVRC_CT_RSP_CODE_ACCEPTED:
        /// FALL THROUGH
        case AVRC_CT_RSP_CODE_INTERIM:
            result = RET_NO_ERROR;
            break;
        case AVRC_CT_RSP_CODE_NOT_IMPLEMENTED:
            result = RET_NO_SUPPORT;
            break;
        case AVRC_CT_RSP_CODE_REJECTED:
        /// FALL THROUGH
        default:
            /// Do nothing!
            break;
    }

    return result;
}

int AvrcCtProfile::ExplainCrCodeToResult(uint8_t crCode)
{
    HILOGI("crCode: %{public}d", crCode);
    int result = RET_BAD_STATUS;

    switch (crCode) {
        case AVRC_CT_RSP_CODE_ACCEPTED:
        case AVRC_CT_RSP_CODE_STABLE:
        case AVRC_CT_RSP_CODE_CHANGED:
            /// FALL THROUGH
        case AVRC_CT_RSP_CODE_INTERIM:
            result = RET_NO_ERROR;
            break;
        case AVRC_CT_RSP_CODE_NOT_IMPLEMENTED:
            result = RET_NO_SUPPORT;
            break;
        case AVRC_CT_RSP_CODE_REJECTED:
            /// FALL THROUGH
        default:
            /// Do nothing!
            break;
    }

    return result;
}

std::string AvrcCtProfile::GetEventName(uint8_t event)
{
    switch (event) {
        case AVCT_CONNECT_IND_EVT:
            return "AVCT_CONNECT_IND_EVT";
        case AVCT_CONNECT_CFM_EVT:
            return "AVCT_CONNECT_CFM_EVT";
        case AVCT_DISCONNECT_IND_EVT:
            return "AVCT_DISCONNECT_IND_EVT";
        case AVCT_DISCONNECT_CFM_EVT:
            return "AVCT_DISCONNECT_CFM_EVT";
        case AVCT_BR_CONNECT_IND_EVT:
            return "AVCT_BR_CONNECT_IND_EVT";
        case AVCT_BR_CONNECT_CFM_EVT:
            return "AVCT_BR_CONNECT_CFM_EVT";
        case AVCT_BR_DISCONNECT_IND_EVT:
            return "AVCT_BR_DISCONNECT_IND_EVT";
        case AVCT_BR_DISCONNECT_CFM_EVT:
            return "AVCT_BR_DISCONNECT_CFM_EVT";
        default:
            return "Unknown";
    }
}
}  // namespace bluetooth
}  // namespace OHOS
