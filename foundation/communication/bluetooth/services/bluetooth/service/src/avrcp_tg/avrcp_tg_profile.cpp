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

#include "avrcp_tg_profile.h"
#include "avrcp_tg_browse.h"
#include "avrcp_tg_connection.h"
#include "avrcp_tg_notification.h"
#include "avrcp_tg_pass_through.h"
#include "avrcp_tg_sub_unit_info.h"
#include "avrcp_tg_unit_info.h"
#include "avrcp_tg_vendor.h"
#include "avrcp_tg_vendor_continuation.h"
#include "avrcp_tg_vendor_player_application_settings.h"
#include "power_manager.h"
#include "log_util.h"

namespace OHOS {
namespace bluetooth {
bool AvrcTgProfile::g_isEnabled = false;

AvrcTgProfile::AvrcTgProfile(uint16_t features, uint32_t companyId, uint16_t controlMtu, uint16_t browseMtu,
    utility::Dispatcher *dispatcher, AvctChannelEventCallback eventCallback, AvctMsgCallback msgCallback)
    : features_(features),
      companyId_(companyId),
      controlMtu_(controlMtu),
      browseMtu_(browseMtu),
      dispatcher_(dispatcher),
      connectId_(0xFF),
      eventCallback_(eventCallback),
      msgCallback_(msgCallback)
{
    HILOGI("features: %{public}d, companyId: %{public}u, controlMtu: %{public}d, browseMtu: %{public}d",
        features, companyId, controlMtu, browseMtu);

    SetEnableFlag(false);
    playStatusChanged = std::make_pair(false, 0);
    trackChanged = std::make_pair(false, 0);
}

AvrcTgProfile::~AvrcTgProfile()
{
    HILOGI("enter");

    SetEnableFlag(false);
    playStatusChanged = std::make_pair(false, 0);
    trackChanged = std::make_pair(false, 0);
}

/******************************************************************
 * REGISTER / UNREGISTER OBSERVER                                 *
 ******************************************************************/

void AvrcTgProfile::RegisterObserver(AvrcTgProfile::Observer *observer)
{
    HILOGI("enter");

    myObserver_ = observer;
}

void AvrcTgProfile::UnregisterObserver(void)
{
    HILOGI("enter");

    myObserver_ = nullptr;
}

/******************************************************************
 * ENABLE / DISABLE                                               *
 ******************************************************************/

int AvrcTgProfile::Enable(void)
{
    HILOGI("enter");

    int result = RET_NO_ERROR;

    AVCT_Register(controlMtu_, browseMtu_, AVCT_TG);

    AvctConnectParam param = {eventCallback_, msgCallback_, AVRC_TG_AV_REMOTE_CONTROL, AVCT_ACPT, nullptr};
    BtAddr btAddr = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00};
    if (AVCT_ConnectReq(&connectId_, &param, &btAddr) != AVCT_SUCCESS) {
        result = RET_BAD_STATUS;
    } else {
        SetEnableFlag(true);
    }

    return result;
}

int AvrcTgProfile::Disable(void)
{
    HILOGI("enter");

    do {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        SetEnableFlag(false);
    } while (false);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    AvrcTgStateMachineManager *smManager = AvrcTgStateMachineManager::GetInstance();

    if (!cnManager->IsConnectInfoEmpty()) {
        utility::Message msg(AVRC_TG_SM_EVENT_TO_DISABLE_STATE);
        smManager->SendMessageToAllBrowseStateMachine(msg);
        smManager->SendMessageToAllControlStateMachine(msg);
    } else {
        AvrcpTgSafeDelete(cnManager);
        smManager->ShutDown();
        myObserver_->onDisabled();
    }

    return RET_NO_ERROR;
}

void AvrcTgProfile::SetEnableFlag(bool isEnabled)
{
    HILOGI("isEnabled: %{public}d", isEnabled);

    g_isEnabled = isEnabled;
}

/******************************************************************
 * CONNECTION                                                     *
 ******************************************************************/

void AvrcTgProfile::SetActiveDevice(const RawAddress &rawAddr)
{
    HILOGI("rawAddr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();

    cnManager->SetActiveDevice(rawAddr.GetAddress());
}

std::vector<bluetooth::RawAddress> AvrcTgProfile::GetConnectedDevices(void)
{
    HILOGI("enter");
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    std::vector<bluetooth::RawAddress> result;

    std::list<std::string> addrs = AvrcTgConnectManager::GetInstance()->GetDeviceAddresses();
    for (auto &addr : addrs) {
        RawAddress rawAddr(addr);
        if (GetDeviceState(rawAddr) == static_cast<int>(BTConnectState::CONNECTED)) {
            result.push_back(rawAddr);
        }
    }

    return result;
}

std::vector<RawAddress> AvrcTgProfile::GetDevicesByStates(const std::vector<int> &states)
{
    HILOGI("enter");
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();

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

int AvrcTgProfile::GetDeviceState(const RawAddress &rawAddr)
{
    HILOGI("rawAddr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    AvrcTgStateMachineManager *smManager = AvrcTgStateMachineManager::GetInstance();
    int result;

    if (smManager->IsControlConnectingState(rawAddr)) {
        result = static_cast<int>(BTConnectState::CONNECTING);
    } else if (smManager->IsControlConnectedState(rawAddr) || smManager->IsControlContinuationState(rawAddr)) {
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

int AvrcTgProfile::GetMaxConnectNum(void)
{
    HILOGI("enter");
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    int result = 0x00;

    std::list<std::string> addrs = AvrcTgConnectManager::GetInstance()->GetDeviceAddresses();

    for (auto &addr : addrs) {
        RawAddress rawAddr(addr);
        if (GetDeviceState(rawAddr) == static_cast<int>(BTConnectState::CONNECTED)) {
            ++result;
        }
    }

    return result;
}

int AvrcTgProfile::Connect(const RawAddress &rawAddr) const
{
    HILOGI("rawAddr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    uint8_t result = RET_NO_ERROR;

    result |= AvrcTgConnectManager::GetInstance()->Add(
        rawAddr, 0x00, AVCT_INIT, controlMtu_, browseMtu_, companyId_, 0x0000, eventCallback_, msgCallback_);
    result |= AvrcTgStateMachineManager::GetInstance()->AddControlStateMachine(rawAddr);
    if (result == RET_NO_ERROR) {
        myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::CONNECTING));
    }

    return result;
}

int AvrcTgProfile::ConnectBr(const RawAddress &rawAddr)
{
    HILOGI("rawAddr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcTgStateMachineManager *smManager = AvrcTgStateMachineManager::GetInstance();

    return smManager->AddBrowseStateMachine(rawAddr);
}

int AvrcTgProfile::Disconnect(const RawAddress &rawAddr) const
{
    HILOGI("rawAddr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcTgStateMachineManager *smManager = AvrcTgStateMachineManager::GetInstance();
    utility::Message msg(AVRC_TG_SM_EVENT_TO_DISCONNECTING_STATE);
    bool result = true;
    result &= smManager->SendMessageToBrowseStateMachine(rawAddr, msg);
    result &= smManager->SendMessageToControlStateMachine(rawAddr, msg);
    if (result) {
        myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::DISCONNECTING));
    }

    return RET_NO_ERROR;
}

int AvrcTgProfile::DisconnectBr(const RawAddress &rawAddr)
{
    HILOGI("rawAddr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    utility::Message msg(AVRC_TG_SM_EVENT_TO_DISCONNECTING_STATE);
    AvrcTgStateMachineManager::GetInstance()->SendMessageToBrowseStateMachine(rawAddr, msg);

    return RET_NO_ERROR;
}

int AvrcTgProfile::GetConnectState(void)
{
    HILOGI("enter");
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    int result = PROFILE_STATE_DISCONNECTED;

    std::list<std::string> addrs = AvrcTgConnectManager::GetInstance()->GetDeviceAddresses();
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

void AvrcTgProfile::AcceptPassiveConnect(const RawAddress &rawAddr)
{
    HILOGI("rawAddr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    utility::Message msg(AVRC_TG_SM_EVENT_TO_CONNECTED_STATE);
    AvrcTgStateMachineManager::GetInstance()->SendMessageToControlStateMachine(rawAddr, msg);
}

void AvrcTgProfile::RejectPassiveConnect(const RawAddress &rawAddr)
{
    HILOGI("rawAddr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AVCT_DisconnectReq(connectId_);
    DeleteResource(rawAddr);
}

/******************************************************************
 * PASS THROUGH COMMAND                                           *
 ******************************************************************/

void AvrcTgProfile::SendPressButtonRsp(const RawAddress &rawAddr, uint8_t button, uint8_t label, int result)
{
    HILOGI("rawAddr: %{public}s, button: %{public}d, label: %{public}d, result: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), button, label, result);

    std::shared_ptr<AvrcTgPassPacket> packet = std::make_shared<AvrcTgPassPacket>(button, AVRC_KEY_STATE_PRESS, label);
    packet->SetCrCode(ExplainResultToPassCrCode(result));
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendPassRsp(rawAddr, packet);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::SendReleaseButtonRsp(const RawAddress &rawAddr, uint8_t button, uint8_t label, int result)
{
    HILOGI("rawAddr: %{public}s, button: %{public}d, label: %{public}d, result: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), button, label, result);
    std::shared_ptr<AvrcTgPassPacket> packet =
        std::make_shared<AvrcTgPassPacket>(button, AVRC_KEY_STATE_RELEASE, label);
    packet->SetCrCode(ExplainResultToPassCrCode(result));
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendPassRsp(rawAddr, packet);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

bool AvrcTgProfile::IsEnabled(void)
{
    HILOGI("enter");

    return g_isEnabled;
}

bool AvrcTgProfile::IsSamePassPressCommand(const RawAddress &rawAddr, uint8_t button)
{
    HILOGI("rawAddr: %{public}s, button: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), button);

    bool result = false;

    std::shared_ptr<AvrcTgPassPacket> packet = AvrcTgConnectManager::GetInstance()->GetPassPacket(rawAddr);
    do {
        if (packet == nullptr) {
            break;
        }

        if (packet->GetKeyOperation() != button) {
            break;
        }

        if (packet->GetKeyState() != AVRC_KEY_STATE_PRESS) {
            break;
        }

        result = true;
    } while (false);

    return result;
}

bool AvrcTgProfile::IsIgnorePassReleaseCommand(const RawAddress &rawAddr, uint8_t button)
{
    HILOGI("rawAddr: %{public}s, button: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), button);

    bool result = true;

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    std::shared_ptr<AvrcTgPassPacket> packet = cnManager->GetPassPacket(rawAddr);

    do {
        if (packet == nullptr) {
            break;
        }

        if (packet->GetKeyOperation() != button) {
            break;
        }

        if (packet->GetKeyState() != AVRC_KEY_STATE_PRESS) {
            break;
        }

        if (cnManager->IsPassTimerEmpty(rawAddr)) {
            break;
        }

        result = false;
    } while (false);

    return result;
}

void AvrcTgProfile::SendPassRsp(const RawAddress &rawAddr, std::shared_ptr<AvrcTgPassPacket> &pkt)
{
    HILOGI("rawAddr: %{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();

    cnManager->ClearPassInfo(rawAddr);
    cnManager->SetPassPacket(rawAddr, pkt);

    utility::Message msg(AVRC_TG_SM_EVENT_PASS_THROUGH);
    AvrcTgStateMachineManager::GetInstance()->SendMessageToControlStateMachine(rawAddr, msg);

    /// If the target has not received the pressed command within two seconds or the target receives the
    /// pressed command with another operation id, then the target regards that the released command was sent but
    /// missed to receive.
    /// @see AV/C Panel Subunit Specification 1.23 Section 9.4 PASS THROUGH control command.
    if (pkt->GetCrCode() == AVRC_TG_RSP_CODE_ACCEPTED && pkt->GetKeyState() == AVRC_KEY_STATE_PRESS) {
        /// Sets the information which is used in the timeout.
        auto func = std::bind(&AvrcTgProfile::PassTimeoutCallback,
            this,
            rawAddr,
            pkt->GetKeyOperation(),
            pkt->GetKeyState(),
            pkt->GetLabel());
        cnManager->SetPassTimer(rawAddr, func, AVRC_TG_TIMER_PASS_THROUGH);
    }
}

void AvrcTgProfile::ReceivePassCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr: %{public}s, label: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    std::shared_ptr<AvrcTgPassPacket> packet = std::make_shared<AvrcTgPassPacket>(pkt, label);
    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();

    if (!packet->IsValid()) {
        /// The packet is invalid, so send the reject frame directly.
        SendPassRsp(rawAddr, packet);
        return;
    }

    uint8_t key = packet->GetKeyOperation();
    uint8_t state = packet->GetKeyState();

    HILOGI("key: %{public}x, state: %{public}x, label: %{public}u", key, state, label);

    switch (state) {
        case AVRC_KEY_STATE_PRESS:
            if (cnManager->IsPassTimerEmpty(rawAddr)) {
                myObserver_->onButtonPressed(rawAddr, key, label);
                break;
            }

            /// If the target receives the pressed command with another operation id, then the target regards
            /// that the released command was sent but missed to receive.
            if (!IsSamePassPressCommand(rawAddr, key)) {
                /// Informs the previous button is released.
                myObserver_->onButtonReleased(rawAddr, cnManager->GetPassPacket(rawAddr)->GetKeyOperation(), label);
                /// Informs the current button is pressed.
                myObserver_->onButtonPressed(rawAddr, key, label);
                break;
            }

            /// A command with the pressed value is valid for two seconds from the time when a target sends
            /// back a response of the command. The controller shall continue sending pressed value with
            /// identical operation id value in the operation_id field while the command is wished to stay
            /// valid.
            myObserver_->onButtonHeld(rawAddr, key, label);
            break;
        case AVRC_KEY_STATE_RELEASE:
            /// If the target has not received the pressed command within two seconds or the target receives
            /// the pressed command with another operation id, then the target regards that the released command was
            /// sent but missed to receive. In these cases, the target will ignore the released command when the
            /// target receives this released command after the time out or reception of the new pressed command.
            /// @see AV/C Panel Subunit Specification 1.23 Section 9.4 PASS THROUGH control command.
            if (!IsIgnorePassReleaseCommand(rawAddr, key)) {
                myObserver_->onButtonReleased(rawAddr, key, label);
                break;
            }

            packet->SetCrCode(AVRC_TG_RSP_CODE_REJECTED);
            SendPassRsp(rawAddr, packet);
            HILOGI("The PASS THROUGH command is rejected! key: %{public}x state: %{public}x", state, label);
            break;
        default:
            HILOGI("The key state is incorrect!");
            break;
    }
}

void AvrcTgProfile::ProcessPassTimeout(RawAddress rawAddr, uint8_t button, uint8_t state, uint8_t label)
{
    HILOGI("rawAddr: %{public}s, button: %{public}d, state: %{public}d, label: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), button, state, label);

    ASSERT(state == AVRC_KEY_STATE_PRESS);

    if (IsEnabled()) {
        AvrcTgConnectManager::GetInstance()->ClearPassInfo(rawAddr);
        myObserver_->onButtonReleased(rawAddr, button, label);
    }
}

void AvrcTgProfile::PassTimeoutCallback(const RawAddress &rawAddr, uint8_t button, uint8_t state, uint8_t label)
{
    HILOGI("rawAddr: %{public}s, button: %{public}d, state: %{public}d, label: %{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), button, state, label);

    if (IsEnabled()) {
        dispatcher_->PostTask(std::bind(&AvrcTgProfile::ProcessPassTimeout, this, rawAddr, button, state, label));
    }
}

/******************************************************************
 * UNIT INFO / SUB UNIT INFO COMMAND                              *
 ******************************************************************/

void AvrcTgProfile::SendUnitRsp(const RawAddress &rawAddr, std::shared_ptr<AvrcTgUnitPacket> &pkt, AvrcTgSmEvent event)
{
    HILOGI("rawAddr: %{public}s, event: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), event);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    cnManager->ClearUnitInfo(rawAddr);
    cnManager->SetUnitPacket(rawAddr, pkt);

    utility::Message msg(event);
    AvrcTgStateMachineManager::GetInstance()->SendMessageToControlStateMachine(rawAddr, msg);
}

void AvrcTgProfile::ReceiveUnitCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr: %{public}s, label: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    std::shared_ptr<AvrcTgUnitPacket> packet = std::make_shared<AvrcTgUnitPacket>(pkt, label);

    SendUnitRsp(rawAddr, packet, AVRC_TG_SM_EVENT_UNIT_INFO);
}

void AvrcTgProfile::ReceiveSubUnitCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr: %{public}s, label: %{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    std::shared_ptr<AvrcTgUnitPacket> packet = std::make_shared<AvrcTgSubUnitPacket>(pkt, label);

    SendUnitRsp(rawAddr, packet, AVRC_TG_SM_EVENT_SUB_UNIT_INFO);
}

/******************************************************************
 * VENDOR COMMAND                                                 *
 ******************************************************************/

void AvrcTgProfile::SendVendorRsp(
    const RawAddress &rawAddr, std::shared_ptr<AvrcTgVendorPacket> &pkt, AvrcTgSmEvent event)
{
    HILOGI("rawAddr: %{public}s, event: %{public}x", GET_ENCRYPT_AVRCP_ADDR(rawAddr), event);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    cnManager->ClearVendorInfo(rawAddr);
    cnManager->SetVendorPacket(rawAddr, pkt);

    utility::Message msg(event);
    AvrcTgStateMachineManager::GetInstance()->SendMessageToControlStateMachine(rawAddr, msg);
}

void AvrcTgProfile::SendGetCapabilitiesRsp(
    const RawAddress &rawAddr, const std::vector<uint32_t> &companies, uint8_t label, int result)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d, result:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, result);

    std::shared_ptr<AvrcTgVendorPacket> packet =
        std::make_shared<AvrcTgGcPacket>(ExplainResultToStatusCrCode(result), companies, label);
    SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_CAPABILITIES);
}

void AvrcTgProfile::SendGetCapabilitiesRsp(
    const RawAddress &rawAddr, const std::vector<uint8_t> &events, uint8_t label, int result)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d, result:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, result);

    std::shared_ptr<AvrcTgVendorPacket> packet =
        std::make_shared<AvrcTgGcPacket>(ExplainResultToStatusCrCode(result), events, label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_CAPABILITIES);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::ReceiveGetCapabilitiesCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    std::shared_ptr<AvrcTgGcPacket> gcPkt = std::make_shared<AvrcTgGcPacket>(pkt, label);

    if (gcPkt->IsValid()) {
        if (gcPkt->GetCapabilityId() == AVRC_CAPABILITY_COMPANYID) {
            AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
            std::vector<uint32_t> companies;
            companies.push_back(cnManager->GetCompanyId(rawAddr));

            SendGetCapabilitiesRsp(rawAddr, companies, label, RET_NO_ERROR);
        } else {
            myObserver_->getCapabilities(rawAddr, label);
        }
    } else {
        std::shared_ptr<AvrcTgVendorPacket> packet = gcPkt;
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_CAPABILITIES);
    }
}

void AvrcTgProfile::SendListPlayerApplicationSettingAttributesRsp(
    const RawAddress &rawAddr, std::deque<uint8_t> attributes, uint8_t label, int result)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d, result:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, result);

    std::shared_ptr<AvrcTgVendorPacket> packet =
        std::make_shared<AvrcTgLpasaPacket>(ExplainResultToStatusCrCode(result), attributes, label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::ReceiveListPlayerApplicationSettingAttributesCmd(
    const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    std::shared_ptr<AvrcTgLpasaPacket> lpasaPkt = std::make_shared<AvrcTgLpasaPacket>(pkt, label);

    if (lpasaPkt->IsValid()) {
        myObserver_->listPlayerApplicationSettingAttributes(rawAddr, label);
    } else {
        std::shared_ptr<AvrcTgVendorPacket> packet = lpasaPkt;
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_LIST_PLAYER_APPLICATION_SETTING_VALUES);
    }
}

void AvrcTgProfile::SendListPlayerApplicationSettingValuesRsp(
    const RawAddress &rawAddr, const std::deque<uint8_t> &values, uint8_t label, int result)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d, result:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, result);

    std::shared_ptr<AvrcTgVendorPacket> packet =
        std::make_shared<AvrcTgLpasvPacket>(ExplainResultToStatusCrCode(result), values, label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_LIST_PLAYER_APPLICATION_SETTING_VALUES);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::ReceiveListPlayerApplicationSettingValuesCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    std::shared_ptr<AvrcTgLpasvPacket> lpasvPkt = std::make_shared<AvrcTgLpasvPacket>(pkt, label);

    if (lpasvPkt->IsValid()) {
        myObserver_->listPlayerApplicationSettingValues(rawAddr, lpasvPkt->GetAttribute(), label);
    } else {
        std::shared_ptr<AvrcTgVendorPacket> packet = lpasvPkt;
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_LIST_PLAYER_APPLICATION_SETTING_VALUES);
    }
}

void AvrcTgProfile::SendGetCurrentPlayerApplicationSettingValueRsp(const RawAddress &rawAddr,
    const std::deque<uint8_t> &attributes, const std::deque<uint8_t> &values, uint8_t label, int result)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d, result:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, result);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    std::shared_ptr<AvrcTgVendorPacket> packet = std::make_shared<AvrcTgGcpasvPacket>(
        cnManager->GetControlMtu(rawAddr), ExplainResultToStatusCrCode(result), attributes, values, label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::ReceiveGetCurrentPlayerApplicationSettingValueCmd(
    const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    std::shared_ptr<AvrcTgGcpasvPacket> gcpasvPkt = std::make_shared<AvrcTgGcpasvPacket>(pkt, label);

    if (gcpasvPkt->IsValid()) {
        myObserver_->getCurrentPlayerApplicationSettingValue(
            rawAddr, gcpasvPkt->GetAttributes(), label, AVRC_ACTION_TYPE_GET_PLAYER_APP_SETTING_CURRENT_VALUE);
    } else {
        std::shared_ptr<AvrcTgVendorPacket> packet = gcpasvPkt;
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE);
    }
}

void AvrcTgProfile::SendSetPlayerApplicationSettingValueRsp(const RawAddress &rawAddr, uint8_t label, int result)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d, result:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, result);

    std::shared_ptr<AvrcTgVendorPacket> packet =
        std::make_shared<AvrcTgSpasvPacket>(ExplainResultToControlCrCode(result), label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_SET_PLAYER_APPLICATION_SETTING_VALUE);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::ReceiveSetPlayerApplicationSettingValueCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    std::shared_ptr<AvrcTgSpasvPacket> spasvPkt = std::make_shared<AvrcTgSpasvPacket>(pkt, label);
    if (spasvPkt != nullptr) {
        if (spasvPkt->IsValid()) {
            myObserver_->setPlayerApplicationSettingValue(
                rawAddr, spasvPkt->GetAttributes(), spasvPkt->GetValues(), label);
        } else {
            std::shared_ptr<AvrcTgVendorPacket> packet = spasvPkt;
            SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_SET_PLAYER_APPLICATION_SETTING_VALUE);
        }
    }
}

void AvrcTgProfile::SendGetPlayerApplicationSettingAttributeTextRsp(const RawAddress &rawAddr,
    const std::vector<uint8_t> &attributes, const std::vector<std::string> &attrStr, uint8_t label, int result)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d, result:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, result);

    int crCode = ExplainResultToControlCrCode(result);
    if (crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
        crCode = AVRC_TG_RSP_CODE_STABLE;
    }

    std::shared_ptr<AvrcTgVendorPacket> packet =
        std::make_shared<AvrcTgGpasatPacket>(crCode, attributes, attrStr, label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::ReceiveGetPlayerApplicationSettingAttributeTextCmd(
    const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    std::shared_ptr<AvrcTgGpasatPacket> gpasatPkt = std::make_shared<AvrcTgGpasatPacket>(pkt, label);
    if (gpasatPkt != nullptr) {
        if (gpasatPkt->IsValid()) {
            myObserver_->getPlayerApplicationSettingAttributeText(rawAddr, gpasatPkt->GetAttributes(), label);
        } else {
            std::shared_ptr<AvrcTgVendorPacket> packet = gpasatPkt;
            IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
            SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT);
            IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
        }
    }
}

void AvrcTgProfile::SendGetPlayerApplicationSettingValueTextRsp(const RawAddress &rawAddr,
    const std::vector<uint8_t> &values, const std::vector<std::string> &valueStr, uint8_t label, int result)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d, result:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, result);

    int crCode = ExplainResultToControlCrCode(result);
    if (crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
        crCode = AVRC_TG_RSP_CODE_STABLE;
    }

    std::shared_ptr<AvrcTgVendorPacket> packet = std::make_shared<AvrcTgGpasvtPacket>(crCode, values, valueStr, label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::ReceiveGetPlayerApplicationSettingValueTextCmd(
    const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    std::shared_ptr<AvrcTgGpasvtPacket> gpasvtPkt = std::make_shared<AvrcTgGpasvtPacket>(pkt, label);

    if (gpasvtPkt->IsValid()) {
        myObserver_->getPlayerApplicationSettingValueText(
            rawAddr, gpasvtPkt->GetAttributeId(), gpasvtPkt->GetValues(), label);
    } else {
        std::shared_ptr<AvrcTgVendorPacket> packet = gpasvtPkt;
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT);
    }
}

void AvrcTgProfile::SendGetElementAttributesRsp(const RawAddress &rawAddr, const std::vector<uint32_t> &attribtues,
    const std::vector<std::string> &values, uint8_t label, int result)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d, result:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, result);
    int crCode = ExplainResultToControlCrCode(result);
    if (crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
        crCode = AVRC_TG_RSP_CODE_STABLE;
    }
    std::shared_ptr<AvrcTgVendorPacket> packet = std::make_shared<AvrcTgGeaPacket>(crCode, attribtues, values, label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_ELEMENT_ATTRIBTUES);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::ReceiveGetElementAttributesCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    std::shared_ptr<AvrcTgGeaPacket> geaPkt = std::make_shared<AvrcTgGeaPacket>(pkt, label);
    if (geaPkt != nullptr) {
        if (geaPkt->IsValid()) {
            myObserver_->getElementAttributes(rawAddr, geaPkt->GetIdentifier(), geaPkt->GetAttributes(), label);
        } else {
            std::shared_ptr<AvrcTgVendorPacket> packet = geaPkt;
            SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_ELEMENT_ATTRIBTUES);
        }
    }
}

void AvrcTgProfile::SendGetPlayStatusRsp(const RawAddress &rawAddr, uint32_t songLength, uint32_t songPosition,
    uint8_t playStatus, uint8_t label, int result)
{
    HILOGI("rawAddr:%{public}s, songLength:%{public}u, songPosition:%{public}u, playStatus:%{public}d, "
        "label:%{public}d, result:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), songLength, songPosition, playStatus,
        label, result);

    std::shared_ptr<AvrcTgVendorPacket> packet = std::make_shared<AvrcTgGpsPacket>(
        ExplainResultToStatusCrCode(result), songLength, songPosition, playStatus, label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_PLAY_STATUS);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::ReceiveGetPlayStatusCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    std::shared_ptr<AvrcTgGpsPacket> gpsPkt = std::make_shared<AvrcTgGpsPacket>(pkt, label);
    if (gpsPkt != nullptr) {
        if (gpsPkt->IsValid()) {
            myObserver_->getPlayStatus(rawAddr, label, AVRC_ACTION_TYPE_GET_PLAY_STATUS);
        } else {
            std::shared_ptr<AvrcTgVendorPacket> packet = gpsPkt;
            SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_PLAY_STATUS);
        }
    }
}

void AvrcTgProfile::ReceiveRequestContinuingResponseCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    std::shared_ptr<AvrcTgRcrPacket> rcrPkt = std::make_shared<AvrcTgRcrPacket>(pkt, label);

    std::shared_ptr<AvrcTgVendorPacket> packet = cnManager->GetVendorPacket(rawAddr);
    uint16_t event = AVRC_TG_SM_EVENT_INVALID;

    do {
        if (packet == nullptr) {
            cnManager->ClearVendorInfo(rawAddr);
            event = AVRC_TG_SM_EVENT_TO_CONNECTED_STATE;
            HILOGI("Receive the RequestContinuingResponse command, but the vendor packet is nullptr!");
            break;
        }

        if (!AvrcTgStateMachineManager::GetInstance()->IsControlContinuationState(rawAddr)) {
            cnManager->ClearVendorInfo(rawAddr);
            event = AVRC_TG_SM_EVENT_TO_CONNECTED_STATE;
            HILOGI("Receive the RequestContinuingResponse command, but current is not CONTINUATION!");
            break;
        }

        if (packet->GetPduId() != rcrPkt->GetRequestContinuingPduId()) {
            cnManager->ClearVendorInfo(rawAddr);
            event = AVRC_TG_SM_EVENT_TO_CONNECTED_STATE;
            HILOGI("Receive the AbortContinuingResponse command, but pdu id is incorrect! saved pduId: %{public}x, "
                "received pduId: %{public}x", packet->GetPduId(), rcrPkt->GetRequestContinuingPduId());
            break;
        }

        event = AVRC_TG_SM_EVENT_REQUEST_CONTINUING_RESPONSE;
    } while (false);

    utility::Message msg(event);
    AvrcTgStateMachineManager::GetInstance()->SendMessageToControlStateMachine(rawAddr, msg);
}

void AvrcTgProfile::ReceiveAbortContinuingResponseCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    std::shared_ptr<AvrcTgAcrPacket> acrPkt = std::make_shared<AvrcTgAcrPacket>(pkt, label);
    std::shared_ptr<AvrcTgVendorPacket> packet = AvrcTgConnectManager::GetInstance()->GetVendorPacket(rawAddr);

    do {
        if (packet == nullptr) {
            acrPkt->SetCrCode(AVRC_TG_RSP_CODE_REJECTED);
            HILOGI("Receive the AbortContinuingResponse command, but the vendor packet is nullptr!");
            break;
        }

        if (!AvrcTgStateMachineManager::GetInstance()->IsControlContinuationState(rawAddr)) {
            acrPkt->SetCrCode(AVRC_TG_RSP_CODE_REJECTED);
            HILOGI("Receive the AbortContinuingResponse command, but current is not CONTINUATION!");
            break;
        }

        if (packet->GetPduId() != acrPkt->GetRequestContinuingPduId()) {
            acrPkt->SetCrCode(AVRC_TG_RSP_CODE_REJECTED);
            HILOGI("Receive the AbortContinuingResponse command, but pdu id is incorrect! saved pduId: %{public}x, "
                "received pduId: %{public}x", packet->GetPduId(), acrPkt->GetRequestContinuingPduId());
            break;
        }
    } while (false);

    packet = std::make_shared<AvrcTgAcrPacket>(acrPkt->GetRequestContinuingPduId(), label);

    SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_ABORT_CONTINUING_RESPONSE);
}

void AvrcTgProfile::SendSetAddressedPlayerRsp(const RawAddress &rawAddr, int status, uint8_t label, int result)
{
    HILOGI("rawAddr:%{public}s, status:%{public}d, label:%{public}d, result:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), status, label, result);

    uint8_t crCode = ExplainResultToControlCrCode(result);
    if (status != AVRC_ES_CODE_NO_ERROR) {
        crCode = AVRC_TG_RSP_CODE_REJECTED;
    }

    std::shared_ptr<AvrcTgVendorPacket> packet = std::make_shared<AvrcTgSapPacket>(crCode, status, label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_SET_ADDRESSED_PLAYER);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::ReceiveSetAddressedPlayerCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();

    std::shared_ptr<AvrcTgSapPacket> sapPkt = std::make_shared<AvrcTgSapPacket>(pkt, label);
    if (sapPkt->IsValid()) {
        myObserver_->setAddressedPlayer(rawAddr, sapPkt->GetPlayerId(), cnManager->GetUidCounter(rawAddr), label);
    } else {
        std::shared_ptr<AvrcTgVendorPacket> packet = sapPkt;
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_SET_ADDRESSED_PLAYER);
    }
}

void AvrcTgProfile::SendPlayItemRsp(const RawAddress &rawAddr, int status, uint8_t label, int result)
{
    HILOGI("rawAddr:%{public}s, status:%{public}d, label:%{public}d, result:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), status, label, result);

    uint8_t crCode = ExplainResultToControlCrCode(result);
    if (status != AVRC_ES_CODE_NO_ERROR) {
        crCode = AVRC_TG_RSP_CODE_REJECTED;
    }

    std::shared_ptr<AvrcTgVendorPacket> packet = std::make_shared<AvrcTgPiPacket>(crCode, status, label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_PLAY_ITEM);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::ReceivePlayItemCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    std::shared_ptr<AvrcTgPiPacket> piPkt = std::make_shared<AvrcTgPiPacket>(pkt, label);
    if (piPkt->IsValid()) {
        myObserver_->playItem(rawAddr, piPkt->GetScope(), piPkt->GetUid(), piPkt->GetUidCounter(), label);
    } else {
        std::shared_ptr<AvrcTgVendorPacket> packet = piPkt;
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_PLAY_ITEM);
    }
}

void AvrcTgProfile::SendAddToNowPlayingRsp(const RawAddress &rawAddr, int status, uint8_t label, int result)
{
    HILOGI("rawAddr:%{public}s, status:%{public}d, label:%{public}d, result:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), status, label, result);

    uint8_t crCode = ExplainResultToControlCrCode(result);
    if (status != AVRC_ES_CODE_NO_ERROR) {
        crCode = AVRC_TG_RSP_CODE_REJECTED;
    }

    std::shared_ptr<AvrcTgVendorPacket> packet = std::make_shared<AvrcTgAtnpPacket>(crCode, status, label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_ADD_TO_NOW_PLAYING);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::ReceiveAddToNowPlayingCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    std::shared_ptr<AvrcTgAtnpPacket> atnpPkt = std::make_shared<AvrcTgAtnpPacket>(pkt, label);
    if (atnpPkt->IsValid()) {
        myObserver_->addToNowPlaying(rawAddr, atnpPkt->GetScope(), atnpPkt->GetUid(), atnpPkt->GetUidCounter(), label);
    } else {
        std::shared_ptr<AvrcTgVendorPacket> packet = atnpPkt;
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_ADD_TO_NOW_PLAYING);
    }
}

void AvrcTgProfile::SendSetAbsoluteVolumeRsp(const RawAddress &rawAddr, uint8_t volume, uint8_t label, int result)
{
    HILOGI("rawAddr:%{public}s, volume:%{public}d, label:%{public}d, result:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), volume, label, result);

    std::shared_ptr<AvrcTgVendorPacket> packet =
        std::make_shared<AvrcTgSavPacket>(ExplainResultToControlCrCode(result), volume, label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_SET_ABSOLUTE_VOLUME);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::ReceiveSetAbsoluteVolumeCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    std::shared_ptr<AvrcTgSavPacket> savPkt = std::make_shared<AvrcTgSavPacket>(pkt, label);
    if (savPkt->IsValid()) {
        myObserver_->setAbsoluteVolume(rawAddr, savPkt->GetAbsoluteVolume(), label);
    } else {
        std::shared_ptr<AvrcTgVendorPacket> packet = savPkt;
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_SET_ABSOLUTE_VOLUME);
    }
}

void AvrcTgProfile::SendPlaybackStatusChangedRsp(bool isInterim, uint8_t playStatus, uint8_t label, int result)
{
    HILOGI("isInterim:%{public}d, playStatus:%{public}d, label:%{public}d, result:%{public}d",
        isInterim, playStatus, label, result);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    RawAddress rawAddr(cnManager->GetActiveDevice());

    if (cnManager->IsNotifyStateEnabled(rawAddr, AVRC_TG_EVENT_ID_PLAYBACK_STATUS_CHANGED)) {
        uint8_t crCode = ExplainResultToControlCrCode(result);
        if (isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_INTERIM;
        } else if (!isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_CHANGED;
        }

        std::shared_ptr<AvrcTgNotifyPacket> notifyPkt =
            std::make_shared<AvrcTgNotifyPacket>(AVRC_TG_EVENT_ID_PLAYBACK_STATUS_CHANGED, crCode, label);
        notifyPkt->InitPlaybackStatusChanged(playStatus);
        notifyPkt->AssemblePackets();
        std::shared_ptr<AvrcTgVendorPacket> packet = notifyPkt;
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_REGISTER_NOTIFICATION);
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);

        if (!isInterim) {
            playStatusChanged = std::make_pair(false, 0);
        }
    }

    if (!isInterim) {
        cnManager->DisableNotifyState(rawAddr, AVRC_TG_EVENT_ID_PLAYBACK_STATUS_CHANGED);
    }
}

void AvrcTgProfile::SendTrackChangedRsp(bool isInterim, uint64_t identifier, uint8_t label, int result)
{
    HILOGI("isInterim:%{public}d, identifier:%{public}llu, label:%{public}d, result:%{public}d",
        isInterim, static_cast<unsigned long long>(identifier), label, result);
    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    RawAddress rawAddr(cnManager->GetActiveDevice());

    if (cnManager->IsNotifyStateEnabled(rawAddr, AVRC_TG_EVENT_ID_TRACK_CHANGED)) {
        uint8_t crCode = ExplainResultToControlCrCode(result);
        if (isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_INTERIM;
        } else if (!isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_CHANGED;
        }

        std::shared_ptr<AvrcTgNotifyPacket> notifyPkt =
            std::make_shared<AvrcTgNotifyPacket>(AVRC_TG_EVENT_ID_TRACK_CHANGED, crCode, label);
        notifyPkt->InitTrackChanged(identifier);
        notifyPkt->AssemblePackets();
        std::shared_ptr<AvrcTgVendorPacket> packet = notifyPkt;
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_REGISTER_NOTIFICATION);
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
    }

    if (!isInterim) {
        cnManager->DisableNotifyState(rawAddr, AVRC_TG_EVENT_ID_TRACK_CHANGED);
    }
}

void AvrcTgProfile::SendTrackReachedEndRsp(bool isInterim, uint8_t label, int result)
{
    HILOGI("isInterim:%{public}d, label:%{public}d, result:%{public}d", isInterim, label, result);
    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    RawAddress rawAddr(cnManager->GetActiveDevice());

    if (cnManager->IsNotifyStateEnabled(rawAddr, AVRC_TG_EVENT_ID_TRACK_REACHED_END)) {
        uint8_t crCode = ExplainResultToControlCrCode(result);
        if (isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_INTERIM;
        } else if (!isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_CHANGED;
        }

        std::shared_ptr<AvrcTgNotifyPacket> notifyPkt =
            std::make_shared<AvrcTgNotifyPacket>(AVRC_TG_EVENT_ID_TRACK_REACHED_END, crCode, label);
        notifyPkt->AssemblePackets();
        std::shared_ptr<AvrcTgVendorPacket> packet = notifyPkt;
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_REGISTER_NOTIFICATION);
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
    }

    if (!isInterim) {
        cnManager->DisableNotifyState(rawAddr, AVRC_TG_EVENT_ID_TRACK_REACHED_END);
    }
}

void AvrcTgProfile::SendTrackReachedStartRsp(bool isInterim, uint8_t label, int result)
{
    HILOGI("isInterim:%{public}d, label:%{public}d, result:%{public}d", isInterim, label, result);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    RawAddress rawAddr(cnManager->GetActiveDevice());

    if (cnManager->IsNotifyStateEnabled(rawAddr, AVRC_TG_EVENT_ID_TRACK_REACHED_START)) {
        uint8_t crCode = ExplainResultToControlCrCode(result);
        if (isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_INTERIM;
        } else if (!isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_CHANGED;
        }

        std::shared_ptr<AvrcTgNotifyPacket> notifyPkt =
            std::make_shared<AvrcTgNotifyPacket>(AVRC_TG_EVENT_ID_TRACK_REACHED_START, crCode, label);
        notifyPkt->AssemblePackets();
        std::shared_ptr<AvrcTgVendorPacket> packet = notifyPkt;
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_REGISTER_NOTIFICATION);
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
    }

    if (!isInterim) {
        cnManager->DisableNotifyState(rawAddr, AVRC_TG_EVENT_ID_TRACK_REACHED_START);
    }
}

void AvrcTgProfile::SendPlaybackPosChangedRsp(bool isInterim, uint32_t playbackPos, uint8_t label, int result)
{
    HILOGI("isInterim:%{public}d, playbackPos:%{public}u, label:%{public}d, result:%{public}d",
        isInterim, playbackPos, label, result);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    RawAddress rawAddr(cnManager->GetActiveDevice());

    if (cnManager->IsNotifyStateEnabled(rawAddr, AVRC_TG_EVENT_ID_PLAYBACK_POS_CHANGED)) {
        uint8_t crCode = ExplainResultToControlCrCode(result);
        if (isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_INTERIM;
        } else if (!isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_CHANGED;
        }

        std::shared_ptr<AvrcTgNotifyPacket> notifyPkt =
            std::make_shared<AvrcTgNotifyPacket>(AVRC_TG_EVENT_ID_PLAYBACK_POS_CHANGED, crCode, label);
        notifyPkt->InitPlaybackPosChanged(playbackPos);
        notifyPkt->AssemblePackets();
        std::shared_ptr<AvrcTgVendorPacket> packet = notifyPkt;
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_REGISTER_NOTIFICATION);
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
    }

    if (!isInterim) {
        cnManager->DisableNotifyState(rawAddr, AVRC_TG_EVENT_ID_PLAYBACK_POS_CHANGED);
    }
}

void AvrcTgProfile::SendPlayerApplicationSettingChangedRsp(
    bool isInterim, const std::deque<uint8_t> &attributes, const std::deque<uint8_t> &values, uint8_t label, int result)
{
    HILOGI("isInterim:%{public}d, label:%{public}d, result:%{public}d", isInterim, label, result);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    RawAddress rawAddr(cnManager->GetActiveDevice());

    if (cnManager->IsNotifyStateEnabled(rawAddr, AVRC_TG_EVENT_ID_PLAYER_APPLICATION_SETTING_CHANGED)) {
        uint8_t crCode = ExplainResultToControlCrCode(result);
        if (isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_INTERIM;
        } else if (!isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_CHANGED;
        }

        std::shared_ptr<AvrcTgNotifyPacket> notifyPkt =
            std::make_shared<AvrcTgNotifyPacket>(AVRC_TG_EVENT_ID_PLAYER_APPLICATION_SETTING_CHANGED, crCode, label);
        notifyPkt->InitPlayerApplicationSettingChanged(attributes, values);
        notifyPkt->AssemblePackets();
        std::shared_ptr<AvrcTgVendorPacket> packet = notifyPkt;
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_REGISTER_NOTIFICATION);
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
    }

    if (!isInterim) {
        cnManager->DisableNotifyState(rawAddr, AVRC_TG_EVENT_ID_PLAYER_APPLICATION_SETTING_CHANGED);
    }
}

void AvrcTgProfile::SendNowPlayingContentChangedRsp(bool isInterim, uint8_t label, int result)
{
    HILOGI("isInterim:%{public}d, label:%{public}d, result:%{public}d", isInterim, label, result);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    RawAddress rawAddr(cnManager->GetActiveDevice());

    if (cnManager->IsNotifyStateEnabled(rawAddr, AVRC_TG_EVENT_ID_NOW_PLAYING_CONTENT_CHANGED)) {
        uint8_t crCode = ExplainResultToControlCrCode(result);
        if (isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_INTERIM;
        } else if (!isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_CHANGED;
        }

        std::shared_ptr<AvrcTgNotifyPacket> notifyPkt =
            std::make_shared<AvrcTgNotifyPacket>(AVRC_TG_EVENT_ID_NOW_PLAYING_CONTENT_CHANGED, crCode, label);
        notifyPkt->AssemblePackets();
        std::shared_ptr<AvrcTgVendorPacket> packet = notifyPkt;
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_REGISTER_NOTIFICATION);
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
    }

    if (!isInterim) {
        cnManager->DisableNotifyState(rawAddr, AVRC_TG_EVENT_ID_NOW_PLAYING_CONTENT_CHANGED);
    }
}

void AvrcTgProfile::SendAvailablePlayersChangedRsp(bool isInterim, uint8_t label, int result)
{
    HILOGI("isInterim:%{public}d, label:%{public}d, result:%{public}d", isInterim, label, result);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    RawAddress rawAddr(cnManager->GetActiveDevice());

    if (cnManager->IsNotifyStateEnabled(rawAddr, AVRC_TG_EVENT_ID_AVAILABLE_PLAYERS_CHANGED)) {
        uint8_t crCode = ExplainResultToControlCrCode(result);
        if (isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_INTERIM;
        } else if (!isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_CHANGED;
        }

        std::shared_ptr<AvrcTgNotifyPacket> notifyPkt =
            std::make_shared<AvrcTgNotifyPacket>(AVRC_TG_EVENT_ID_AVAILABLE_PLAYERS_CHANGED, crCode, label);
        notifyPkt->AssemblePackets();
        std::shared_ptr<AvrcTgVendorPacket> packet = notifyPkt;
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_REGISTER_NOTIFICATION);
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
    }

    if (!isInterim) {
        cnManager->DisableNotifyState(rawAddr, AVRC_TG_EVENT_ID_AVAILABLE_PLAYERS_CHANGED);
    }
}

void AvrcTgProfile::SendAddressedPlayerChangedRsp(
    bool isInterim, uint16_t playerId, uint16_t uidCounter, uint8_t label, int result)
{
    HILOGI("isInterim:%{public}d, playerId:%{public}d, uidCounter:%{public}d, label:%{public}d, result:%{public}d",
        isInterim, playerId, uidCounter, label, result);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    RawAddress rawAddr(cnManager->GetActiveDevice());

    if (cnManager->IsNotifyStateEnabled(rawAddr, AVRC_TG_EVENT_ID_ADDRESSED_PLAYER_CHANGED)) {
        uint8_t crCode = ExplainResultToControlCrCode(result);
        if (isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_INTERIM;
        } else if (!isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_CHANGED;
        }

        std::shared_ptr<AvrcTgNotifyPacket> notifyPkt =
            std::make_shared<AvrcTgNotifyPacket>(AVRC_TG_EVENT_ID_ADDRESSED_PLAYER_CHANGED, crCode, label);
        notifyPkt->InitAddressedPlayerChanged(playerId, uidCounter);
        notifyPkt->AssemblePackets();
        std::shared_ptr<AvrcTgVendorPacket> packet = notifyPkt;
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_REGISTER_NOTIFICATION);
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
    }

    if (!isInterim) {
        cnManager->DisableNotifyState(rawAddr, AVRC_TG_EVENT_ID_ADDRESSED_PLAYER_CHANGED);
    }
}

void AvrcTgProfile::SendUidsChangedRsp(bool isInterim, uint16_t uidCounter, uint8_t label, int result)
{
    HILOGI("isInterim:%{public}d, uidCounter:%{public}d, label:%{public}d, result:%{public}d",
        isInterim, uidCounter, label, result);
    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    RawAddress rawAddr(cnManager->GetActiveDevice());

    if (cnManager->IsNotifyStateEnabled(rawAddr, AVRC_TG_EVENT_ID_UIDS_CHANGED)) {
        uint8_t crCode = ExplainResultToControlCrCode(result);
        if (isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_INTERIM;
        } else if (!isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_CHANGED;
        }

        std::shared_ptr<AvrcTgNotifyPacket> notifyPkt =
            std::make_shared<AvrcTgNotifyPacket>(AVRC_TG_EVENT_ID_UIDS_CHANGED, crCode, label);
        notifyPkt->InitUidsChanged(uidCounter);
        notifyPkt->AssemblePackets();
        std::shared_ptr<AvrcTgVendorPacket> packet = notifyPkt;
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_REGISTER_NOTIFICATION);
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
    }

    if (!isInterim) {
        cnManager->DisableNotifyState(rawAddr, AVRC_TG_EVENT_ID_UIDS_CHANGED);
    }
}

void AvrcTgProfile::SendVolumeChangedRsp(bool isInterim, uint8_t volume, uint8_t label, int result)
{
    HILOGI("isInterim:%{public}d, volume:%{public}d, label:%{public}d, result:%{public}d",
        isInterim, volume, label, result);
    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    RawAddress rawAddr(cnManager->GetActiveDevice());

    if (cnManager->IsNotifyStateEnabled(rawAddr, AVRC_TG_EVENT_ID_VOLUME_CHANGED)) {
        uint8_t crCode = ExplainResultToControlCrCode(result);
        if (isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_INTERIM;
        } else if (!isInterim && crCode == AVRC_TG_RSP_CODE_ACCEPTED) {
            crCode = AVRC_TG_RSP_CODE_CHANGED;
        }

        std::shared_ptr<AvrcTgNotifyPacket> notifyPkt =
            std::make_shared<AvrcTgNotifyPacket>(AVRC_TG_EVENT_ID_VOLUME_CHANGED, crCode, label);
        notifyPkt->InitVolumeChanged(volume);
        notifyPkt->AssemblePackets();
        std::shared_ptr<AvrcTgVendorPacket> packet = notifyPkt;
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
        SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_REGISTER_NOTIFICATION);
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
    }

    if (!isInterim) {
        cnManager->DisableNotifyState(rawAddr, AVRC_TG_EVENT_ID_VOLUME_CHANGED);
    }
}

void AvrcTgProfile::ReceiveRegisterNotificationCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);
    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();

    std::shared_ptr<AvrcTgNotifyPacket> notifyPkt = std::make_shared<AvrcTgNotifyPacket>(pkt, label);
    uint8_t event = notifyPkt->GetEventId();
    cnManager->EnableNotifyState(rawAddr, event);
    HILOGI("event:%{public}d", event);
    switch (event) {
        case AVRC_TG_EVENT_ID_PLAYBACK_STATUS_CHANGED:
            if (notifyPkt->IsValid()) {
                SetNotificationLabel(AVRC_TG_EVENT_ID_PLAYBACK_STATUS_CHANGED, label);
                myObserver_->getPlayStatus(rawAddr, label, AVRC_ACTION_TYPE_NOTIFY_PLAYBACK_STATUS_CHANGED);
            }
            break;
        case AVRC_TG_EVENT_ID_TRACK_CHANGED:
            if (notifyPkt->IsValid()) {
                SetNotificationLabel(AVRC_TG_EVENT_ID_TRACK_CHANGED, label);
                myObserver_->getSelectedTrack(rawAddr, label);
            }
            break;
        case AVRC_TG_EVENT_ID_TRACK_REACHED_END: {
            SetNotificationLabel(AVRC_TG_EVENT_ID_TRACK_REACHED_END, label);
            SendTrackReachedEndRsp(true, label, RET_NO_ERROR);
            break;
        }
        case AVRC_TG_EVENT_ID_TRACK_REACHED_START:
            SetNotificationLabel(AVRC_TG_EVENT_ID_TRACK_REACHED_START, label);
            SendTrackReachedStartRsp(true, label, RET_NO_ERROR);
            break;
        case AVRC_TG_EVENT_ID_PLAYBACK_POS_CHANGED:
            if (notifyPkt->IsValid()) {
                SetNotificationLabel(AVRC_TG_EVENT_ID_PLAYBACK_POS_CHANGED, label);
                myObserver_->setPlaybackInterval(rawAddr, notifyPkt->GetPlaybackInterval());
                myObserver_->getPlayStatus(rawAddr, label, AVRC_ACTION_TYPE_NOTIFY_PLAYBACK_POS_CHANGED);
            }
            break;
        case AVRC_TG_EVENT_ID_PLAYER_APPLICATION_SETTING_CHANGED:
            if (notifyPkt->IsValid()) {
                std::deque<uint8_t> attributes = {
                AVRC_PLAYER_ATTRIBUTE_EQUALIZER,
                AVRC_PLAYER_ATTRIBUTE_REPEAT,
                AVRC_PLAYER_ATTRIBUTE_SHUFFLE,
                AVRC_PLAYER_ATTRIBUTE_SCAN
                };
                SetNotificationLabel(AVRC_TG_EVENT_ID_PLAYER_APPLICATION_SETTING_CHANGED, label);
                myObserver_->getCurrentPlayerApplicationSettingValue(
                    rawAddr, attributes, label, AVRC_ACTION_TYPE_NOTIFY_PLAYER_APPLICATION_SETTING_CHANGED);
            }
            break;
        case AVRC_TG_EVENT_ID_NOW_PLAYING_CONTENT_CHANGED:
            SetNotificationLabel(AVRC_TG_EVENT_ID_NOW_PLAYING_CONTENT_CHANGED, label);
            SendNowPlayingContentChangedRsp(true, label, RET_NO_ERROR);
            break;
        case AVRC_TG_EVENT_ID_AVAILABLE_PLAYERS_CHANGED:
            SetNotificationLabel(AVRC_TG_EVENT_ID_AVAILABLE_PLAYERS_CHANGED, label);
            SendAvailablePlayersChangedRsp(true, label, RET_NO_ERROR);
            break;
        case AVRC_TG_EVENT_ID_ADDRESSED_PLAYER_CHANGED:
            if (notifyPkt->IsValid()) {
                SetNotificationLabel(AVRC_TG_EVENT_ID_ADDRESSED_PLAYER_CHANGED, label);
                myObserver_->getAddressedPlayer(rawAddr, label);
            }
            break;
        case AVRC_TG_EVENT_ID_UIDS_CHANGED:
            if (notifyPkt->IsValid()) {
                SetNotificationLabel(AVRC_TG_EVENT_ID_UIDS_CHANGED, label);
                myObserver_->getUidCounter(rawAddr, label);
            }
            break;
        case AVRC_TG_EVENT_ID_VOLUME_CHANGED:
            if (notifyPkt->IsValid()) {
                SetNotificationLabel(AVRC_TG_EVENT_ID_VOLUME_CHANGED, label);
                myObserver_->getCurrentAbsoluteVolume(rawAddr, label);
            }
            break;
        default:
            std::shared_ptr<AvrcTgVendorPacket> packet = notifyPkt;
            SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_REGISTER_NOTIFICATION);
            break;
    }
}

void AvrcTgProfile::ReceiveVendorCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);
    uint8_t pduId = AvrcTgPacket::GetVendorPdu(pkt);
    HILOGI("enter, pduId:%{public}d", pduId);
    switch (pduId) {
        case AVRC_TG_PDU_ID_GET_CAPABILITIES:
            ReceiveGetCapabilitiesCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES:
            ReceiveListPlayerApplicationSettingAttributesCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_VALUES:
            ReceiveListPlayerApplicationSettingValuesCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_SM_EVENT_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE:
            ReceiveGetCurrentPlayerApplicationSettingValueCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_SM_EVENT_SET_PLAYER_APPLICATION_SETTING_VALUE:
            ReceiveSetPlayerApplicationSettingValueCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_SM_EVENT_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT:
            ReceiveGetPlayerApplicationSettingAttributeTextCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_SM_EVENT_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT:
            ReceiveGetPlayerApplicationSettingValueTextCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_PDU_ID_GET_ELEMENT_ATTRIBUTES:
            ReceiveGetElementAttributesCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_PDU_ID_SET_ADDRESSED_PLAYER:
            ReceiveSetAddressedPlayerCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_PDU_ID_GET_PLAY_STATUS:
            ReceiveGetPlayStatusCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_PDU_ID_REQUEST_CONTINUING_RESPONSE:
            ReceiveRequestContinuingResponseCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_PDU_ID_ABORT_CONTINUING_RESPONSE:
            ReceiveAbortContinuingResponseCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_PDU_ID_PLAY_ITEM:
            ReceivePlayItemCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_PDU_ID_ADD_TO_NOW_PLAYING:
            ReceiveAddToNowPlayingCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_PDU_ID_SET_ABSOLUTE_VOLUME:
            ReceiveSetAbsoluteVolumeCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_PDU_ID_REGISTER_NOTIFICATION:
            ReceiveRegisterNotificationCmd(rawAddr, label, pkt);
            break;
        default:
            std::shared_ptr<AvrcTgVendorPacket> packet =
                std::make_shared<AvrcTgVendorPacket>(pduId, AVRC_ES_CODE_INVALID_COMMAND, label);
            SendVendorRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GENERAL_REJECT);
            HILOGI("The PDU ID is wrong! Address: %{public}s, pduId: %{public}x",
                GET_ENCRYPT_AVRCP_ADDR(rawAddr), pduId);
            break;
    }
}

/******************************************************************
 * BROWSING COMMAND                                               *
 ******************************************************************/

void AvrcTgProfile::SendBrowseRsp(
    const RawAddress &rawAddr, std::shared_ptr<AvrcTgBrowsePacket> &pkt, AvrcTgSmEvent event)
{
    HILOGI("rawAddr:%{public}s, event:%{public}x", GET_ENCRYPT_AVRCP_ADDR(rawAddr), event);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    cnManager->ClearBrowseInfo(rawAddr);
    cnManager->SetBrowsePacket(rawAddr, pkt);

    utility::Message msg(event);
    AvrcTgStateMachineManager::GetInstance()->SendMessageToBrowseStateMachine(rawAddr, msg);
}

void AvrcTgProfile::SendSetBrowsedPlayerRsp(const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems,
    const std::vector<std::string> &folderNames, uint8_t label, int status)
{
    HILOGI("rawAddr:%{public}s, uidCounter:%{public}d, numOfItems:%{public}u, label:%{public}d, status:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, numOfItems, label, status);
    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    if (status == AVRC_ES_CODE_NO_ERROR) {
        cnManager->SetUidCounter(rawAddr, uidCounter);
    }

    std::shared_ptr<AvrcTgBrowsePacket> packet = std::make_shared<AvrcTgSbpPacket>(
        AVCT_BrGetPeerMtu(cnManager->GetConnectId(rawAddr)), status, uidCounter, numOfItems, folderNames, label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendBrowseRsp(rawAddr, packet, AVRC_TG_SM_EVENT_SET_BROWSED_PLAYER);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::ReceiveSetBrowsedPlayerCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    uint16_t uidCounter = cnManager->GetUidCounter(rawAddr);
    std::shared_ptr<AvrcTgBrowsePacket> packet = std::make_shared<AvrcTgSbpPacket>(pkt, label);
    if (packet->IsValid()) {
        AvrcTgSbpPacket *myPkt = static_cast<AvrcTgSbpPacket *>(packet.get());
        myObserver_->setBrowsedPlayer(rawAddr, myPkt->GetPlayerId(), uidCounter, label);
    } else {
        SendBrowseRsp(rawAddr, packet, AVRC_TG_SM_EVENT_SET_BROWSED_PLAYER);
    }
}

void AvrcTgProfile::SendChangePathRsp(const RawAddress &rawAddr, uint32_t numOfItems, uint8_t label, int status)
{
    HILOGI("rawAddr:%{public}s, numOfItems:%{public}u, label:%{public}d, status:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), numOfItems, label, status);

    std::shared_ptr<AvrcTgBrowsePacket> packet = std::make_shared<AvrcTgCpPacket>(status, numOfItems, label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendBrowseRsp(rawAddr, packet, AVRC_TG_SM_EVENT_CHANGE_PATH);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::ReceiveChangePathCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    uint16_t uidCounter = cnManager->GetUidCounter(rawAddr);
    std::shared_ptr<AvrcTgBrowsePacket> packet = std::make_shared<AvrcTgCpPacket>(pkt, uidCounter, label);

    if (packet->IsValid()) {
        AvrcTgCpPacket *myPkt = static_cast<AvrcTgCpPacket *>(packet.get());
        myObserver_->changePath(rawAddr, myPkt->GetUidCounter(), myPkt->GetDirection(), myPkt->GetFolderUid(), label);
    } else {
        SendBrowseRsp(rawAddr, packet, AVRC_TG_SM_EVENT_CHANGE_PATH);
    }
}

void AvrcTgProfile::SendGetFolderItemsRsp(
    const RawAddress &rawAddr, uint16_t uidCounter, const std::vector<AvrcMpItem> &items, uint8_t label, int status)
{
    HILOGI("rawAddr:%{public}s, uidCounter:%{public}d, label:%{public}d, status:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, label, status);
    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    if (status == AVRC_ES_CODE_NO_ERROR) {
        cnManager->SetUidCounter(rawAddr, uidCounter);
    }

    std::shared_ptr<AvrcTgBrowsePacket> packet = std::make_shared<AvrcTgGfiPacket>(
        AVCT_BrGetPeerMtu(cnManager->GetConnectId(rawAddr)), status, uidCounter, items, label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendBrowseRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_FOLDER_ITEMS);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::SendGetFolderItemsRsp(
    const RawAddress &rawAddr, uint16_t uidCounter, const std::vector<AvrcMeItem> &items, uint8_t label, int status)
{
    HILOGI("rawAddr:%{public}s, uidCounter:%{public}d, label:%{public}d, status:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, label, status);
    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    if (status == AVRC_ES_CODE_NO_ERROR) {
        cnManager->SetUidCounter(rawAddr, uidCounter);
    }

    std::shared_ptr<AvrcTgBrowsePacket> packet = std::make_shared<AvrcTgGfiPacket>(
        AVCT_BrGetPeerMtu(cnManager->GetConnectId(rawAddr)), status, uidCounter, items, label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendBrowseRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_FOLDER_ITEMS);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::ReceiveGetFolderItemsCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    std::shared_ptr<AvrcTgBrowsePacket> packet = std::make_shared<AvrcTgGfiPacket>(pkt, label);

    if (packet->IsValid()) {
        AvrcTgGfiPacket *myPkt = static_cast<AvrcTgGfiPacket *>(packet.get());
        myObserver_->getFolderItems(
            rawAddr, myPkt->GetScope(), myPkt->GetStartItem(), myPkt->GetEndItem(), myPkt->GetAttributes(), label);
    } else {
        SendBrowseRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_FOLDER_ITEMS);
    }
}

void AvrcTgProfile::SendGetItemAttributesRsp(const RawAddress &rawAddr, const std::vector<uint32_t> &attributes,
    const std::vector<std::string> &values, uint8_t label, int status)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d, status:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label, status);
    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();

    std::shared_ptr<AvrcTgBrowsePacket> packet = std::make_shared<AvrcTgGiaPacket>(
        AVCT_BrGetPeerMtu(cnManager->GetConnectId(rawAddr)), status, attributes, values, label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendBrowseRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_ITEM_ATTRIBUTES);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::ReceiveGetItemAttributesCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    uint16_t uidCounter = cnManager->GetUidCounter(rawAddr);
    HILOGI("The saved uidCounter: %{public}u", uidCounter);

    std::shared_ptr<AvrcTgBrowsePacket> packet = std::make_shared<AvrcTgGiaPacket>(pkt, uidCounter, label);
    if (packet->IsValid()) {
        AvrcTgGiaPacket *myPkt = static_cast<AvrcTgGiaPacket *>(packet.get());
        myObserver_->getGetItemAttributes(
            rawAddr, myPkt->GetScope(), myPkt->GetUid(), myPkt->GetUidCounter(), myPkt->GetAttributes(), label);
    } else {
        SendBrowseRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_ITEM_ATTRIBUTES);
    }
}

void AvrcTgProfile::SendGetTotalNumberOfItemsRsp(
    const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems, uint8_t label, int status)
{
    HILOGI("rawAddr:%{public}s, uidCounter:%{public}d, numOfItems:%{public}u, label:%{public}d, status:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), uidCounter, numOfItems, label, status);
    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    if (status == AVRC_ES_CODE_NO_ERROR) {
        cnManager->SetUidCounter(rawAddr, uidCounter);
    }

    std::shared_ptr<AvrcTgBrowsePacket> packet =
        std::make_shared<AvrcTgGtnoiPacket>(status, uidCounter, numOfItems, label);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    SendBrowseRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_TOTAL_NUMBER_OF_ITEMS);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::ReceiveGetTotalNumberOfItemsCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    std::shared_ptr<AvrcTgBrowsePacket> packet = std::make_shared<AvrcTgGtnoiPacket>(pkt, label);

    if (packet->IsValid()) {
        AvrcTgGtnoiPacket *myPkt = static_cast<AvrcTgGtnoiPacket *>(packet.get());
        myObserver_->getTotalNumberOfItems(rawAddr, myPkt->GetScope(), label);
    } else {
        SendBrowseRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GET_TOTAL_NUMBER_OF_ITEMS);
    }
}

void AvrcTgProfile::ReceiveBrowseCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt)
{
    HILOGI("rawAddr:%{public}s, label:%{public}d", GET_ENCRYPT_AVRCP_ADDR(rawAddr), label);

    uint8_t pduId = AvrcTgPacket::GetBrowsePdu(pkt);
    switch (pduId) {
        case AVRC_TG_PDU_ID_SET_BROWSED_PLAYER:
            ReceiveSetBrowsedPlayerCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_PDU_ID_CHANGE_PATH:
            ReceiveChangePathCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_PDU_ID_GET_FOLDER_ITEMS:
            ReceiveGetFolderItemsCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_PDU_ID_GET_ITEM_ATTRIBUTES:
            ReceiveGetItemAttributesCmd(rawAddr, label, pkt);
            break;
        case AVRC_TG_PDU_ID_GET_TOTAL_NUMBER_OF_ITEMS:
            ReceiveGetTotalNumberOfItemsCmd(rawAddr, label, pkt);
            break;
        default:
            std::shared_ptr<AvrcTgBrowsePacket> packet = std::make_shared<AvrcTgBrowsePacket>(
                AVRC_TG_PDU_ID_GENERAL_REJECT, AVRC_ES_CODE_INVALID_COMMAND, label);
            SendBrowseRsp(rawAddr, packet, AVRC_TG_SM_EVENT_GENERAL_REJECT);
            HILOGI("The PDU ID is wrong! Address: %{public}s, pduId: %{public}x",
                GET_ENCRYPT_AVRCP_ADDR(rawAddr), pduId);
            break;
    }
}

void AvrcTgProfile::ProcessChannelEvent(
    const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context)
{
    HILOGI("enter");
    switch (event) {
        case AVCT_CONNECT_IND_EVT:
            HILOGI("Receive [AVCT_CONNECT_IND_EVT] - Result[%{public}x] - Address[%{public}s]",
                result, GET_ENCRYPT_AVRCP_ADDR(rawAddr));
            ProcessChannelEventConnectIndEvt(rawAddr, connectId, event, result, context);
            break;
        case AVCT_DISCONNECT_IND_EVT:
            HILOGI("Receive [AVCT_DISCONNECT_IND_EVT] - Result[%{public}x] - Address[%{public}s]",
                result, GET_ENCRYPT_AVRCP_ADDR(rawAddr));
            ProcessChannelEventDisconnectIndEvt(rawAddr, connectId, event, result, context);
            break;
        case AVCT_DISCONNECT_CFM_EVT:
            HILOGI("Receive [AVCT_DISCONNECT_CFM_EVT] - Result[%{public}x] - Address[%{public}s]",
                result, GET_ENCRYPT_AVRCP_ADDR(rawAddr));
            ProcessChannelEventDisconnectCfmEvt(rawAddr, connectId, event, result, context);
            break;
        case AVCT_BR_CONNECT_IND_EVT:
            HILOGI("Receive [AVCT_BR_CONNECT_IND_EVT] Address:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));
            ProcessChannelEventBrConnectIndEvt(rawAddr, connectId, event, result, context);
            break;
        case AVCT_BR_CONNECT_CFM_EVT:
            HILOGI("Receive [AVCT_BR_CONNECT_CFM_EVT] - Result[%{public}x] - Address[%{public}s]",
                result, GET_ENCRYPT_AVRCP_ADDR(rawAddr));
            ProcessChannelEventBrConnectCfmEvt(rawAddr, connectId, event, result, context);
            break;
        case AVCT_BR_DISCONNECT_IND_EVT:
            HILOGI("Receive [AVCT_BR_DISCONNECT_IND_EVT] - Result[%{public}x] - Address[%{public}s]",
                result, GET_ENCRYPT_AVRCP_ADDR(rawAddr));

            DeleteBrowseStateMachine(rawAddr);
            break;
        case AVCT_BR_DISCONNECT_CFM_EVT:
            HILOGI("Receive [AVCT_BR_DISCONNECT_CFM_EVT] - Result[%{public}x] - Address[%{public}s]",
                result, GET_ENCRYPT_AVRCP_ADDR(rawAddr));

            DeleteBrowseStateMachine(rawAddr);
            break;
        case AVCT_CONNECT_CFM_EVT:
            /// The AVCTP does not return this message when an passive connection is created.
            HILOGI("Receive [AVCT_CONNECT_CFM_EVT] - Result[%{public}x] - Address[%{public}s]",
                result, GET_ENCRYPT_AVRCP_ADDR(rawAddr));
            ProcessChannelEventConnectCfmEvt(rawAddr, connectId, event, result, context);
            break;
        default:
            HILOGI("Unknown [%{public}x]! - Address[%{public}s]", event, GET_ENCRYPT_AVRCP_ADDR(rawAddr));
            break;
    };
}

void AvrcTgProfile::ProcessChannelEventConnectIndEvt(
    const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context)
{
    HILOGI("rawAddr:%{public}s, connectId:%{public}d, event:%{public}d, result:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), connectId, event, result);

    myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::CONNECTING));

    result = ExpainAvctResult(result);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    AvrcTgStateMachineManager *smManager = AvrcTgStateMachineManager::GetInstance();
    utility::Message msg(AVRC_TG_SM_EVENT_INVALID);

    if (result == RET_NO_ERROR) {
        result |= cnManager->Add(
            rawAddr, connectId, AVCT_ACPT, controlMtu_, browseMtu_, companyId_, 0x0000, eventCallback_, msgCallback_);
        result |= smManager->AddControlStateMachine(rawAddr);
        if (result == RET_NO_ERROR) {
            cnManager->SetControlMtu(rawAddr, AVCT_GetPeerMtu(connectId));
            AcceptPassiveConnect(rawAddr);
            if (cnManager->GetActiveDevice().compare(AVRC_TG_DEFAULT_BLUETOOTH_ADDRESS) == 0x00) {
                cnManager->SetActiveDevice(rawAddr.GetAddress());
                myObserver_->setActiveDevice(rawAddr);
            }
            cnManager->DeleteDisconnectedDevice(rawAddr.GetAddress());
            if (!IsSupportedBrowsing()) {
                myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::CONNECTED));
                IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_ON, PROFILE_NAME_AVRCP_TG, rawAddr);
            }
        }
    }
}

void AvrcTgProfile::ProcessChannelEventDisconnectIndEvt(
    const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context)
{
    HILOGI("rawAddr:%{public}s, connectId:%{public}d, event:%{public}d, result:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), connectId, event, result);
    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    AvrcTgStateMachineManager *smManager = AvrcTgStateMachineManager::GetInstance();

    if (smManager->IsControlDisableState(rawAddr)) {
        DeleteResource(rawAddr);
        if (cnManager->IsConnectInfoEmpty()) {
            if (cnManager->GetActiveDevice().compare(rawAddr.GetAddress()) == 0x00) {
                myObserver_->setActiveDevice(RawAddress(AVRC_TG_DEFAULT_BLUETOOTH_ADDRESS));
            }
            AvrcpTgSafeDelete(cnManager);
            smManager->ShutDown();
            myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::DISCONNECTED));
            IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_OFF, PROFILE_NAME_AVRCP_TG, rawAddr);
            myObserver_->onDisabled();
        }
    } else {
        myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::DISCONNECTING));
        DeleteResource(rawAddr);
        if (cnManager->GetActiveDevice().compare(rawAddr.GetAddress()) == 0x00) {
            cnManager->SetActiveDevice(AVRC_TG_DEFAULT_BLUETOOTH_ADDRESS);
            myObserver_->setActiveDevice(RawAddress(AVRC_TG_DEFAULT_BLUETOOTH_ADDRESS));
        }
        cnManager->AddDisconnectedDevice(rawAddr.GetAddress());
        myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::DISCONNECTED));
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_OFF, PROFILE_NAME_AVRCP_TG, rawAddr);
    }
}

void AvrcTgProfile::ProcessChannelEventDisconnectCfmEvt(
    const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context)
{
    HILOGI("rawAddr:%{public}s, connectId:%{public}d, event:%{public}d, result:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), connectId, event, result);
    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    AvrcTgStateMachineManager *smManager = AvrcTgStateMachineManager::GetInstance();
    utility::Message msg(AVRC_TG_SM_EVENT_INVALID);

    if (smManager->IsControlDisableState(rawAddr)) {
        DeleteResource(rawAddr);
        if (cnManager->IsConnectInfoEmpty()) {
            if (cnManager->GetActiveDevice().compare(rawAddr.GetAddress()) == 0x00) {
                myObserver_->setActiveDevice(RawAddress(AVRC_TG_DEFAULT_BLUETOOTH_ADDRESS));
            }
            AvrcpTgSafeDelete(cnManager);
            smManager->ShutDown();
            myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::DISCONNECTED));
            IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_OFF, PROFILE_NAME_AVRCP_TG, rawAddr);
            myObserver_->onDisabled();
        }
    } else {
        DeleteResource(rawAddr);
        if (cnManager->GetActiveDevice().compare(rawAddr.GetAddress()) == 0x00) {
            cnManager->SetActiveDevice(AVRC_TG_DEFAULT_BLUETOOTH_ADDRESS);
            myObserver_->setActiveDevice(RawAddress(AVRC_TG_DEFAULT_BLUETOOTH_ADDRESS));
        }
        cnManager->AddDisconnectedDevice(rawAddr.GetAddress());
        myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::DISCONNECTED));
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_OFF, PROFILE_NAME_AVRCP_TG, rawAddr);
    }
}

void AvrcTgProfile::ProcessChannelEventBrConnectIndEvt(
    const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context)
{
    HILOGI("rawAddr:%{public}s, connectId:%{public}d, event:%{public}d, result:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), connectId, event, result);
    AvrcTgStateMachineManager *smManager = AvrcTgStateMachineManager::GetInstance();
    utility::Message msg(AVRC_TG_SM_EVENT_INVALID);

    if (result == RET_NO_ERROR) {
        result |= smManager->AddBrowseStateMachine(rawAddr);
        if (result == RET_NO_ERROR) {
            msg.what_ = AVRC_TG_SM_EVENT_TO_CONNECTED_STATE;
            smManager->SendMessageToBrowseStateMachine(rawAddr, msg);
        }
    }
    if (IsSupportedBrowsing()) {
        myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::CONNECTED));
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_ON, PROFILE_NAME_AVRCP_TG, rawAddr);
    }
}

void AvrcTgProfile::ProcessChannelEventBrConnectCfmEvt(
    const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context)
{
    HILOGI("rawAddr:%{public}s, connectId:%{public}d, event:%{public}d, result:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), connectId, event, result);
    result = ExpainAvctResult(result);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    AvrcTgStateMachineManager *smManager = AvrcTgStateMachineManager::GetInstance();
    utility::Message msg(AVRC_TG_SM_EVENT_INVALID);

    if (result == RET_NO_ERROR) {
        cnManager->SetBrowseMtu(rawAddr, AVCT_BrGetPeerMtu(connectId));
        smManager->AddBrowseStateMachine(rawAddr);
        msg.what_ = AVRC_TG_SM_EVENT_TO_CONNECTED_STATE;
        smManager->SendMessageToBrowseStateMachine(rawAddr, msg);
    }
    myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::CONNECTED));
}

void AvrcTgProfile::ProcessChannelEventConnectCfmEvt(
    const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context)
{
    HILOGI("rawAddr:%{public}s, connectId:%{public}d, event:%{public}d, result:%{public}d",
        GET_ENCRYPT_AVRCP_ADDR(rawAddr), connectId, event, result);
    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();
    AvrcTgStateMachineManager *smManager = AvrcTgStateMachineManager::GetInstance();
    utility::Message msg(AVRC_TG_SM_EVENT_INVALID);

    if (result == RET_NO_ERROR) {
        cnManager->SetConnectId(rawAddr, connectId);
        cnManager->SetActiveDevice(rawAddr.GetAddress());
        msg.what_ = AVRC_TG_SM_EVENT_TO_CONNECTED_STATE;
        smManager->SendMessageToControlStateMachine(rawAddr, msg);

        if (IsSupportedBrowsing()) {
            ConnectBr(rawAddr);
        } else {
            myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::CONNECTED));
        }
        cnManager->DeleteDisconnectedDevice(rawAddr.GetAddress());
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_ON, PROFILE_NAME_AVRCP_TG, rawAddr);
    } else {
        DeleteResource(rawAddr);
        myObserver_->onConnectionStateChanged(rawAddr, static_cast<int>(BTConnectState::DISCONNECTED));
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_OFF, PROFILE_NAME_AVRCP_TG, rawAddr);
    }
}

void AvrcTgProfile::ProcessChannelMessage(
    uint8_t connectId, uint8_t label, uint8_t crType, uint8_t chType, Packet *pkt, void *context)
{
    HILOGI("connectId: %{public}u, label: %{public}u, crType: %{public}u, chType: %{public}u",
        connectId, label, crType, chType);

    AvrcTgConnectManager *cnManager = AvrcTgConnectManager::GetInstance();

    RawAddress rawAddr = AvrcTgConnectManager::GetInstance()->GetRawAddress(connectId);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_AVRCP_TG, rawAddr);
    std::string addr = rawAddr.GetAddress();
    transform(addr.begin(), addr.end(), addr.begin(), ::toupper);
    if (cnManager->GetActiveDevice().compare(addr) == 0x00) {
        if (chType == AVCT_DATA_CTRL) {
            uint8_t opCode = AvrcTgPacket::GetOpCode(pkt);
            HILOGI("opCode: %{public}x", opCode);
            switch (opCode) {
                case AVRC_TG_OP_CODE_PASS_THROUGH:
                    ReceivePassCmd(rawAddr, label, pkt);
                    break;
                case AVRC_TG_OP_CODE_UNIT_INFO:
                    ReceiveUnitCmd(rawAddr, label, pkt);
                    break;
                case AVRC_TG_OP_CODE_SUB_UNIT_INFO:
                    ReceiveSubUnitCmd(rawAddr, label, pkt);
                    break;
                case AVRC_TG_OP_CODE_VENDOR:
                    ReceiveVendorCmd(rawAddr, label, pkt);
                    break;
                default:
                    HILOGI("opCode: %{public}x is wrong! - ConnectId: %{public}x", opCode, connectId);
                    break;
            }
        } else if (chType == AVCT_DATA_BR) {
            ReceiveBrowseCmd(rawAddr, label, pkt);
        } else {
            HILOGI("chType: %{public}x is wrong! ConnectId: %{public}x", chType, connectId);
        }
    } else {
        HILOGI("Active device: %{public}s Request device: %{public}s!",
            cnManager->GetActiveDevice().c_str(), GET_ENCRYPT_AVRCP_ADDR(rawAddr));
    }

    PacketFree(pkt);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_AVRCP_TG, rawAddr);
}

void AvrcTgProfile::DeleteResource(const RawAddress &rawAddr)
{
    HILOGI("rawAddr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcTgConnectManager::GetInstance()->Delete(rawAddr);
    AvrcTgStateMachineManager::GetInstance()->DeletePairOfStateMachine(rawAddr);
}

void AvrcTgProfile::DeleteBrowseStateMachine(const RawAddress &rawAddr)
{
    HILOGI("rawAddr:%{public}s", GET_ENCRYPT_AVRCP_ADDR(rawAddr));

    AvrcTgStateMachineManager::GetInstance()->DeleteBrowseStateMachine(rawAddr);
}

int AvrcTgProfile::ExpainAvctResult(uint16_t avctRet)
{
    HILOGI("avctRet:%{public}d", avctRet);

    int result = RET_BAD_STATUS;

    switch (avctRet) {
        case AVCT_SUCCESS:
            result = RET_NO_ERROR;
            break;
        case AVCT_FAILED:
        default:
            break;
    }

    return result;
}

uint8_t AvrcTgProfile::ExplainResultToPassCrCode(int result)
{
    HILOGI("result:%{public}d", result);

    uint8_t crCode = AVRC_TG_RSP_CODE_STABLE;

    switch (result) {
        case RET_NO_ERROR:
            crCode = AVRC_TG_RSP_CODE_ACCEPTED;
            break;
        case RET_NO_SUPPORT:
            crCode = AVRC_TG_RSP_CODE_NOT_IMPLEMENTED;
            break;
        case RET_BAD_STATUS:
            /// FALL THROUGH
        default:
            crCode = AVRC_TG_RSP_CODE_REJECTED;
            break;
    }

    return crCode;
}

uint8_t AvrcTgProfile::ExplainResultToStatusCrCode(int result)
{
    HILOGI("result:%{public}d", result);

    uint8_t crCode = AVRC_TG_RSP_CODE_STABLE;

    switch (result) {
        case RET_NO_ERROR:
            crCode = AVRC_TG_RSP_CODE_STABLE;
            break;
        case RET_NO_SUPPORT:
            crCode = AVRC_TG_RSP_CODE_NOT_IMPLEMENTED;
            break;
        case RET_BAD_STATUS:
            /// FALL THROUGH
        default:
            crCode = AVRC_TG_RSP_CODE_REJECTED;
            break;
    }

    return crCode;
}

uint8_t AvrcTgProfile::ExplainResultToControlCrCode(int result)
{
    HILOGI("result:%{public}d", result);

    uint8_t crCode = AVRC_TG_RSP_CODE_STABLE;

    switch (result) {
        case RET_NO_ERROR:
            crCode = AVRC_TG_RSP_CODE_ACCEPTED;
            break;
        case RET_NO_SUPPORT:
            crCode = AVRC_TG_RSP_CODE_NOT_IMPLEMENTED;
            break;
        case RET_BAD_STATUS:
            /// FALL THROUGH
        default:
            crCode = AVRC_TG_RSP_CODE_REJECTED;
            break;
    }

    return crCode;
}

std::pair<bool, uint8_t> AvrcTgProfile::GetNotificationLabel(uint8_t event)
{
    HILOGI("event:%{public}d", event);
    switch (event) {
        case AVRC_TG_EVENT_ID_PLAYBACK_STATUS_CHANGED:
            return playStatusChanged;
        case AVRC_TG_EVENT_ID_TRACK_CHANGED:
            return trackChanged;
        case AVRC_TG_EVENT_ID_TRACK_REACHED_END:
        case AVRC_TG_EVENT_ID_TRACK_REACHED_START:
        case AVRC_TG_EVENT_ID_PLAYBACK_POS_CHANGED:
        case AVRC_TG_EVENT_ID_PLAYER_APPLICATION_SETTING_CHANGED:
        case AVRC_TG_EVENT_ID_NOW_PLAYING_CONTENT_CHANGED:
        case AVRC_TG_EVENT_ID_AVAILABLE_PLAYERS_CHANGED:
        case AVRC_TG_EVENT_ID_ADDRESSED_PLAYER_CHANGED:
        case AVRC_TG_EVENT_ID_UIDS_CHANGED:
        case AVRC_TG_EVENT_ID_VOLUME_CHANGED:
        default:
            return std::make_pair(false, 0);
    }
}

void AvrcTgProfile::SetNotificationLabel(uint8_t event, uint8_t label)
{
    HILOGI("event:%{public}d, label:%{public}d", event, label);
    switch (event) {
        case AVRC_TG_EVENT_ID_PLAYBACK_STATUS_CHANGED:
            playStatusChanged = std::make_pair(true, label);
            break;
        case AVRC_TG_EVENT_ID_TRACK_CHANGED:
            trackChanged = std::make_pair(true, label);
            break;
        case AVRC_TG_EVENT_ID_TRACK_REACHED_END:
        case AVRC_TG_EVENT_ID_TRACK_REACHED_START:
        case AVRC_TG_EVENT_ID_PLAYBACK_POS_CHANGED:
        case AVRC_TG_EVENT_ID_PLAYER_APPLICATION_SETTING_CHANGED:
        case AVRC_TG_EVENT_ID_NOW_PLAYING_CONTENT_CHANGED:
        case AVRC_TG_EVENT_ID_AVAILABLE_PLAYERS_CHANGED:
        case AVRC_TG_EVENT_ID_ADDRESSED_PLAYER_CHANGED:
        case AVRC_TG_EVENT_ID_UIDS_CHANGED:
        case AVRC_TG_EVENT_ID_VOLUME_CHANGED:
        default:
            break;
    }
}
}  // namespace bluetooth
}  // namespace OHOS
