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

#include "hfp_ag_system_interface.h"
#include <string.h>
#include "hfp_ag_service.h"
#include "log.h"
#include "stub/telephone_service.h"
#include "telephony_observer_client.h"
#include "bluetooth_call_client.h"
#include "call_manager_client.h"
#include "core_service_client.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "system_ability_status_change_stub.h"

using namespace OHOS;
using namespace OHOS::Telephony;
namespace OHOS {
namespace bluetooth {
constexpr int32_t INVALID_SLOT_ID = -1;

HfpAgSystemInterface::HfpAgSystemInterface()
{}

HfpAgSystemInterface &HfpAgSystemInterface::GetInstance()
{
    static HfpAgSystemInterface hfpAgSystemInterface;
    return hfpAgSystemInterface;
}

void HfpAgSystemInterface::Start()
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    QueryAgIndicator();
    RegisterObserver();
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        LOG_ERROR("[HFP AG] failed to get samgrProxy");
        return;
    }
    statusChangeListener_ = new SystemAbilityStatusChange();
    int32_t ret = samgrProxy->SubscribeSystemAbility(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID, statusChangeListener_);
    if (ret != ERR_OK) {
        LOG_ERROR("[HFP AG] subscribe systemAbilityId: telephone failed!");
        return;
    }
    return;
}

void HfpAgSystemInterface::Stop()
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    UnregisterObserver();
    DelayedSingleton<BluetoothCallClient>::GetInstance()->UnInit();
    slotId_ = 0;
    serviceState_ = 0;
    signalStrength_ = 0;
    roamState_ = 0;
    batteryLevel_ = 0;
    activeNum_ = 0;
    heldNum_ = 0;
    subscriberNumber_ = "";
    operatorName_ = "";
    callState_ = HFP_AG_CALL_STATE_DISCONNECTED;
    return;
}

void HfpAgSystemInterface::RegisterObserver()
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    if (observer_ == nullptr) {
        observer_ = new (std::nothrow) AgTelephonyObserver(*this);
    }
    CoreServiceClient::GetInstance().GetPrimarySlotId(slotId_);
    if (slotId_ < 0) {
        LOG_INFO("[HFP AG]%{public}s(): slotId_ is invalid",  __FUNCTION__);
        return;
    }
    TelephonyObserverClient::GetInstance().AddStateObserver(observer_, slotId_,
        TelephonyObserverBroker::OBSERVER_MASK_NETWORK_STATE | TelephonyObserverBroker::OBSERVER_MASK_SIGNAL_STRENGTHS,
        true);
}

void HfpAgSystemInterface::UnregisterObserver()
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    if (observer_ == nullptr) {
        return;
    }
    CoreServiceClient::GetInstance().GetPrimarySlotId(slotId_);
    if (slotId_ < 0) {
        LOG_INFO("[HFP AG]%{public}s(): slotId_ is invalid",  __FUNCTION__);
        return;
    }
    TelephonyObserverClient::GetInstance().RemoveStateObserver(slotId_,
        TelephonyObserverBroker::OBSERVER_MASK_NETWORK_STATE | TelephonyObserverBroker::OBSERVER_MASK_SIGNAL_STRENGTHS);
}

void HfpAgSystemInterface::RejectCall(const std::string &address) const
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    DelayedSingleton<BluetoothCallClient>::GetInstance()->RejectCall();
}

void HfpAgSystemInterface::DialOutCall(const std::string &address, const std::string &number) const
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    AppExecFwk::PacMap extras;
    std::u16string u16number = Str8ToStr16(number);
    DelayedSingleton<CallManagerClient>::GetInstance()->DialCall(u16number, extras);
}

void HfpAgSystemInterface::HangupCall(const std::string &address) const
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    HfpAgService *service = HfpAgService::GetService();
    if (service->GetMockState() == HFP_AG_MOCK) {
        HfpAgMessage evt(HFP_AG_SEND_CALL_STATE_EVT, HFP_AG_CALL_INACTIVE);
        SendMockEvent(evt);
        return;
    }

    std::vector<CallAttributeInfo> callList;
    int32_t slotId = INVALID_SLOT_ID;
    CoreServiceClient::GetInstance().GetPrimarySlotId(slotId);
    bool isActiveCallHangUp = false;
    if (slotId < 0) {
        LOG_ERROR("[HFP AG]%{public}s():slotId_ is invalid",  __FUNCTION__);
        return;
    }
    callList = DelayedSingleton<BluetoothCallClient>::GetInstance()->GetCurrentCallList(slotId);
    for (auto call : callList) {
        std::string number = call.accountNumber;
        LOG_INFO("[HFP AG]%{public}s():HangupCallnumbre = %{public}s, state = %{public}d id = %{public}d",
            __FUNCTION__, number.c_str(), (int)call.callState, (int)call.callId);
        if ((int)TelCallState::CALL_STATUS_ACTIVE == (int)call.callState) {
            // Should use hangup(index)
            DelayedSingleton<BluetoothCallClient>::GetInstance()->HangUpCall();
            isActiveCallHangUp = true;
        }
    }

    if (!isActiveCallHangUp) {
        DelayedSingleton<BluetoothCallClient>::GetInstance()->HangUpCall();
        LOG_INFO(", HangUpCall ");
    }
    isActiveCallHangUp = false;
}

void HfpAgSystemInterface::AnswerCall(const std::string &address) const
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    DelayedSingleton<BluetoothCallClient>::GetInstance()->AnswerCall();
}

bool HfpAgSystemInterface::SendDtmf(int dtmf, const std::string &address) const
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    char str = dtmf + '0';
    if (!DelayedSingleton<BluetoothCallClient>::GetInstance()->StartDtmf(str)) {
        return false;
    }
    DelayedSingleton<BluetoothCallClient>::GetInstance()->StopDtmf();
    return true;
}

bool HfpAgSystemInterface::HoldCall(int chld) const
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    return HandleChld(chld);
}

std::string HfpAgSystemInterface::GetNetworkOperator()
{
    CoreServiceClient::GetInstance().GetPrimarySlotId(slotId_);
    if (slotId_ < 0) {
        LOG_INFO("[HFP AG]%{public}s(): slotId_ is invalid",  __FUNCTION__);
    } else {
        sptr<NetworkState> networkState = nullptr;
        CoreServiceClient::GetInstance().GetNetworkState(slotId_, networkState);
        if (networkState != nullptr) {
            operatorName_ = networkState->GetLongOperatorName();
            LOG_INFO("[HFP AG]%{public}s(): operatorName_ is %{public}s", __FUNCTION__, operatorName_.c_str());
        } else {
            LOG_INFO("[HFP AG]%{public}s(): networkState is nullptr", __FUNCTION__);
        }
    }
    return operatorName_;
}

std::string HfpAgSystemInterface::GetSubscriberNumber()
{
    CoreServiceClient::GetInstance().GetPrimarySlotId(slotId_);
    if (slotId_ < 0) {
        LOG_INFO("[HFP AG]%{public}s(): slotId_ is invalid",  __FUNCTION__);
    } else {
        std::u16string telephoneNumber;
        CoreServiceClient::GetInstance().GetSimTelephoneNumber(slotId_, telephoneNumber);
        subscriberNumber_ = Str16ToStr8(telephoneNumber);
        LOG_INFO("[HFP AG]%{public}s(): subscriberNumber_ is %{public}s",  __FUNCTION__, subscriberNumber_.c_str());
    }
    return subscriberNumber_;
}

bool HfpAgSystemInterface::QueryCurrentCallsList(const std::string &address)
{
    LOG_INFO("[HFP AG]%{public}s(): enter",  __FUNCTION__);
    std::vector<CallAttributeInfo> callList;
    HfpAgService *service = HfpAgService::GetService();
    if (service == nullptr) {
        LOG_INFO("[HFP AG]%{public}s():no service",  __FUNCTION__);
        return false;
    }
    if (service->GetMockState() == HFP_AG_MOCK) {
        return HandleClccMock(address);
    }
    CoreServiceClient::GetInstance().GetPrimarySlotId(slotId_);
    if (slotId_ < 0) {
        LOG_ERROR("[HFP AG]%{public}s():slotId_ is invalid",  __FUNCTION__);
        service->ResponesOK(address);
        return false;
    }
    callList = DelayedSingleton<BluetoothCallClient>::GetInstance()->GetCurrentCallList(slotId_);
    int callIndex = 0;
    for (auto call : callList) {
        callIndex++;
        std::string number = call.accountNumber;
        bool conferenceState = false;
        if (call.conferenceState != TelConferenceState::TEL_CONFERENCE_IDLE) {
            conferenceState = true;
        }
        int dire = -1;
        switch ((int)call.callDirection) {
            case (int)CallDirection::CALL_DIRECTION_IN:
                dire = BT_CALL_DIRECTION_IN;
                break;
            case (int)CallDirection::CALL_DIRECTION_OUT:
                dire = BT_CALL_DIRECTION_OUT;
                break;
            default:
                dire = BT_CALL_DIRECTION_UNKNOW;
                break;
        }
        int calltype = -1;
        if ((int)call.callType == (int)CallType::TYPE_CS) {
            calltype = CALL_TYPE_DEFAULT;
        } else {
            calltype = (int)call.callType;
        }

        service->ClccResponse(callIndex, dire, (int)call.callState, 0, conferenceState, number, calltype);
            LOG_INFO("[HFP AG]%{public}s(): for number = %{public}s id = %{public}d state = %{public}d",
                __FUNCTION__, number.c_str(), callIndex, (int)call.callState);
    }
    callIndex = 0;
    service->ResponesOK(address);
    return true;
}

void HfpAgSystemInterface::QueryPhoneState() const
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    DelayedSingleton<BluetoothCallClient>::GetInstance()->GetCallState();
    return;
}

bool HfpAgSystemInterface::StartVoiceRecognition(const std::string &address) const
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    return stub::TelephoneService::GetInstance()->StartVoiceRecognition(address);
}

bool HfpAgSystemInterface::StopVoiceRecogition(const std::string &address) const
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    return stub::TelephoneService::GetInstance()->StopVoiceRecogition(address);
}

bool HfpAgSystemInterface::SetAudioParameters(const std::string &parameters) const
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    return stub::TelephoneService::GetInstance()->SetAudioParameters(parameters);
}

void HfpAgSystemInterface::SetStreamVolume(int streamType, int volume, int flag) const
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    stub::TelephoneService::GetInstance()->SetStreamVolume(streamType, volume, flag);
}

std::string HfpAgSystemInterface::GetLastDialNumber()
{
    LOG_DEBUG("[HFP AG]%{public}s():enter",  __FUNCTION__);
    std::string number = stub::TelephoneService::GetInstance()->GetLastDialNumber();
    return number;
}

int HfpAgSystemInterface::GetServiceState()
{
    CoreServiceClient::GetInstance().GetPrimarySlotId(slotId_);
    LOG_INFO("[HFP AG]%{public}s(): slotId_ is %{public}d",  __FUNCTION__, slotId_);
    if (slotId_ < 0) {
        LOG_INFO("[HFP AG]%{public}s(): slotId_ is invalid",  __FUNCTION__);
    } else {
        sptr<NetworkState> networkState = nullptr;
        CoreServiceClient::GetInstance().GetNetworkState(slotId_, networkState);
        if (networkState != nullptr) {
            serviceState_ = static_cast<std::underlying_type<RegServiceState>::type>(networkState->GetRegStatus());
            LOG_DEBUG("[HFP AG]%{public}s(): serviceState_ is %{public}d", __FUNCTION__, serviceState_);
        }
    }
    return serviceState_;
}

int HfpAgSystemInterface::GetSignalStrength()
{
    LOG_INFO("[HFP AG]%{public}s(): slotId_ is %{public}d",  __FUNCTION__, slotId_);
    if (slotId_ < 0) {
        LOG_INFO("[HFP AG]%{public}s(): slotId_ is invalid",  __FUNCTION__);
    } else {
        std::vector<sptr<SignalInformation>> signals;
        if (CoreServiceClient::GetInstance().GetSignalInfoList(slotId_, signals) == 0 && !signals.empty()) {
            signalStrength_ = signals[0]->GetSignalLevel();
        } else {
            signalStrength_ = 0;
            LOG_INFO("[HFP AG]%{public}s(): GetSignalInfoList is empty",  __FUNCTION__);
        }
        LOG_INFO("[HFP AG]%{public}s(): signalStrength_ is %{public}d",  __FUNCTION__, signalStrength_);
    }
    return signalStrength_;
}

int HfpAgSystemInterface::GetRoamState()
{
    CoreServiceClient::GetInstance().GetPrimarySlotId(slotId_);
    LOG_INFO("[HFP AG]%{public}s(): slotId_ is %{public}d",  __FUNCTION__, slotId_);
    if (slotId_ < 0) {
        LOG_INFO("[HFP AG]%{public}s(): slotId_ is invalid",  __FUNCTION__);
    } else {
        sptr<NetworkState> networkState = nullptr;
        CoreServiceClient::GetInstance().GetNetworkState(slotId_, networkState);
        if (networkState != nullptr) {
            roamState_ = networkState->IsRoaming();
            LOG_INFO("[HFP AG]%{public}s(): roamState_ is %{public}d", __FUNCTION__, roamState_);
        }
    }
    return roamState_;
}

int HfpAgSystemInterface::GetBatteryLevel()
{
    return batteryLevel_;
}

void HfpAgSystemInterface::QueryAgIndicator()
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    // No interface for querying battery level
    batteryLevel_ = 0;
    // query ag indicator for the other module.
    serviceState_ = 0;
    roamState_ = 0;
    operatorName_ = "";
    signalStrength_ = 0;
    subscriberNumber_ = "";
    CoreServiceClient::GetInstance().GetPrimarySlotId(slotId_);
    LOG_INFO("[HFP AG]%{public}s(): slotId_ is %{public}d",  __FUNCTION__, slotId_);
    if (slotId_ < 0) {
        LOG_INFO("[HFP AG]%{public}s(): slotId_ is invalid",  __FUNCTION__);
    } else {
        sptr<NetworkState> networkState = nullptr;
        CoreServiceClient::GetInstance().GetNetworkState(slotId_, networkState);
        if (networkState != nullptr) {
            serviceState_ = static_cast<std::underlying_type<RegServiceState>::type>(networkState->GetRegStatus());
            LOG_INFO("[HFP AG]%{public}s(): serviceState_ is %{public}d", __FUNCTION__, serviceState_);
            roamState_ = networkState->IsRoaming();
            LOG_INFO("[HFP AG]%{public}s(): roamState_ is %{public}d", __FUNCTION__, roamState_);
            operatorName_ = networkState->GetLongOperatorName();
            LOG_INFO("[HFP AG]%{public}s(): operatorName_ is %{public}s", __FUNCTION__, operatorName_.c_str());
        } else {
            LOG_INFO("[HFP AG]%{public}s(): networkState is nullptr", __FUNCTION__);
        }
        LOG_DEBUG("[HFP AG]%{public}s(): serviceState_ is %{public}d", __FUNCTION__, serviceState_);
        std::vector<sptr<SignalInformation>> signals;
        if (CoreServiceClient::GetInstance().GetSignalInfoList(slotId_, signals) == 0 && !signals.empty()) {
            signalStrength_ = signals[0]->GetSignalLevel();
        } else {
            signalStrength_ = 0;
            LOG_INFO("[HFP AG]%{public}s(): GetSignalInfoList is empty",  __FUNCTION__);
        }
        LOG_INFO("[HFP AG]%{public}s(): signalStrength_ is %{public}d",  __FUNCTION__, signalStrength_);

        std::u16string telephoneNumber;
        CoreServiceClient::GetInstance().GetSimTelephoneNumber(slotId_, telephoneNumber);
        subscriberNumber_ = Str16ToStr8(telephoneNumber);
        LOG_INFO("[HFP AG]%{public}s(): subscriberNumber_ is %{public}s",  __FUNCTION__, subscriberNumber_.c_str());
    }
    SendServiceStateToService();
    SendRoamStateToService();
    SendSignalStrengthToService();
    SendBatteryLevelToService();

    return;
}

void HfpAgSystemInterface::UpdateAgIndicator() const
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    for (auto call : bluetoothCallList_) {
        std::string number = call.number;
        LOG_INFO("[HFP AG]%{public}s():for number = %{public}s state = %{public}d",
            __FUNCTION__, number.c_str(), call.callstate);
        if ((int)TelCallState::CALL_STATUS_WAITING == call.callstate && bluetoothCallList_.size() > 1) {
            SendIncomingCallToService(number, CALL_TYPE_DEFAULT);
            SendCallSetupToService((int)HfpCallSetupStatus::HFP_AG_CALLSETUP_INCOMING);
        } else if ((int)TelCallState::CALL_STATUS_HOLDING == call.callstate) {
            SendCallHeldToService(HFP_AG_CALLHELD_ACTIVE);
        } else if ((int)TelCallState::CALL_STATUS_ACTIVE == call.callstate) {
            if (call.precallstate == (int)TelCallState::CALL_STATUS_WAITING ||
                call.precallstate == (int)TelCallState::CALL_STATUS_INCOMING) {
                SendCallStateToService(HFP_AG_CALL_ACTIVE);
            }
        }
    }
}

int HfpAgSystemInterface::GetCallWithState(int state) const
{
    int callid = -1;
    for (auto call : bluetoothCallList_) {
        if (call.callstate == state) {
            callid = call.callid;
        }
    }
    return callid;
}

void HfpAgSystemInterface::UpdateCallList()
{
    std::vector<CallAttributeInfo> callList;
    int slotId = INVALID_SLOT_ID;
    CoreServiceClient::GetInstance().GetPrimarySlotId(slotId);
    if (slotId < 0) {
        LOG_ERROR("[HFP AG]%{public}s():slotId_ is invalid",  __FUNCTION__);
    }
    callList = DelayedSingleton<BluetoothCallClient>::GetInstance()->GetCurrentCallList(slotId);
    if (callList.size() == 0) {
        bluetoothCallList_.clear();
        return;
    }
    for (auto call : callList) {
        int sameindex = -1;
        int prestate = -1;
        std::string number = call.accountNumber;
        for (int i = 0; i < bluetoothCallList_.size(); i++) {
            if (strcmp(bluetoothCallList_[i].number.c_str(), number.c_str()) == 0) {
                sameindex = i;
            }
        }
        if (bluetoothCallList_.size() > callList.size()) {
            prestate = bluetoothCallList_[sameindex].callstate;
            bluetoothCallList_.clear();
            sameindex = -1;
        }
        if (sameindex != -1) {
            bluetoothCallList_[sameindex].precallstate = bluetoothCallList_[sameindex].callstate;
            bluetoothCallList_[sameindex].callstate = (int)call.callState;
        } else {
            BluetoothCall btcall;
            btcall.callstate = (int)call.callState;
            btcall.number = call.accountNumber;
            btcall.callid = call.callId;
            btcall.precallstate = prestate;
            bluetoothCallList_.push_back(btcall);
        }
    }
}

void HfpAgSystemInterface::GetVoiceNumber()
{
    HfpAgService *service = HfpAgService::GetService();
    if (service == nullptr) {
        LOG_ERROR("[HFP AG]%{public}s():service is nullptr", __FUNCTION__);
    }
    std::string number = "1234567";
    // NEED TO GET VOICE TAG NUMBER, 1234567 JUST FOR TEST, INTERFACE NOT AVAIABLE NOW!
    service->SendBinpNumber(number);
}

void HfpAgSystemInterface::GetResponseHoldState(std::string address)
{
    LOG_INFO("[HFP AG]%{public}s(): enter",  __FUNCTION__);
    std::vector<CallAttributeInfo> callList;
    HfpAgService *service = HfpAgService::GetService();
    if (service == nullptr) {
        LOG_ERROR("[HFP AG]%{public}s():no service",  __FUNCTION__);
    }
    int slotId = INVALID_SLOT_ID;
    CoreServiceClient::GetInstance().GetPrimarySlotId(slotId);
    if (slotId < 0) {
        LOG_ERROR("[HFP AG]%{public}s():slotId_ is invalid",  __FUNCTION__);
    }
    callList = DelayedSingleton<BluetoothCallClient>::GetInstance()->GetCurrentCallList(slotId);
    bool hasHoldCall = false;
    for (auto call : callList) {
        std::string number = call.accountNumber;
        if ((int)TelCallState::CALL_STATUS_HOLDING == (int)call.callState) {
            hasHoldCall = true;
        }
    }

    if (hasHoldCall) {
        SendResponseHoldStateToService(HFP_AG_RESPONSE_HOLD);
    }
    hasHoldCall = false;
    service->ResponesOK(address);
}

void HfpAgSystemInterface::SetResponseHoldState(std::string address, int btrh)
{
    LOG_INFO("enter %{publis}s", __FUNCTION__);
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    std::vector<CallAttributeInfo> callList;
    HfpAgService *service = HfpAgService::GetService();
    if (service == nullptr) {
        LOG_ERROR("[HFP AG]%{public}s():no service",  __FUNCTION__);
    }
    int slotId = INVALID_SLOT_ID;
    CoreServiceClient::GetInstance().GetPrimarySlotId(slotId);
    if (slotId < 0) {
        LOG_ERROR("[HFP AG]%{public}s():slotId_ is invalid",  __FUNCTION__);
    }
    callList = DelayedSingleton<BluetoothCallClient>::GetInstance()->GetCurrentCallList(slotId);
    for (auto call : callList) {
        std::string number = call.accountNumber;
        if (btrh == 1) {
            if ((int)TelCallState::CALL_STATUS_INCOMING == (int)call.callState) {
                DelayedSingleton<BluetoothCallClient>::GetInstance()->HoldCall();
                SendResponseHoldStateToService(HFP_AG_RESPONSE_HOLD_ACCEPT);
            }
        }
    }
    service->ResponesOK(address);
}

void HfpAgSystemInterface::HandlePhoneStateMock(std::string number, int state, int type)
{
    HfpAgService *service = HfpAgService::GetService();
    callList_ = service->GetCallList();
    int activenum = 0;
    int holdnum = 0;
    int endnum = 0;
    int endcallindex = -1;
    for (int j = 0; j < callList_.size(); j++) {
        if (callList_[j].callstate == (int)TelCallState::CALL_STATUS_INCOMING) {
            HfpAgMessage evt1(HFP_AG_SEND_CALL_SETUP_EVT, (int)HfpCallSetupStatus::HFP_AG_CALLSETUP_INCOMING);
            SendMockEvent(evt1);
            HfpAgMessage evt(HFP_AG_MOCK_RING);
            SendMockEvent(evt);
            SendClip(callList_[j].number);
        } else if (callList_[j].callstate == (int)TelCallState::CALL_STATUS_WAITING) {
            bool hasendcall = false;
            for (auto call:callList_) {
                if (call.callstate == (int)TelCallState::CALL_STATUS_DISCONNECTING ||
                    call.callstate == (int)TelCallState::CALL_STATUS_DISCONNECTED) {
                    hasendcall = true;
                }
            }
            if (!hasendcall) {
                HfpAgMessage evt(HFP_AG_SEND_INCOMING_EVT, CALL_TYPE_DEFAULT);
                evt.str_ = callList_[j].number;
                SendMockEvent(evt);
                HfpAgMessage evt1(HFP_AG_SEND_CALL_SETUP_EVT, (int)HfpCallSetupStatus::HFP_AG_CALLSETUP_INCOMING);
                SendMockEvent(evt1);
            }
        } else if (callList_[j].callstate == (int)TelCallState::CALL_STATUS_ACTIVE) {
            bool hasoutgoingcall = false;
            for (auto call : callList_) {
                if (call.callstate == (int)TelCallState::CALL_STATUS_ALERTING ||
                    call.callstate == (int)TelCallState::CALL_STATUS_DIALING) {
                    hasoutgoingcall = true;
                }
            }
            if (!hasoutgoingcall) {
                HfpAgMessage evt(HFP_AG_SEND_CALL_STATE_EVT, HFP_AG_CALL_ACTIVE);
                SendMockEvent(evt);
                HfpAgMessage evt1(HFP_AG_SEND_CALL_SETUP_EVT, (int)HfpCallSetupStatus::HFP_AG_CALLSETUP_NONE);
                SendMockEvent(evt1);
            }
            activenum++;
        } else if (callList_[j].callstate == (int)TelCallState::CALL_STATUS_DISCONNECTED ||
                callList_[j].callstate == (int)TelCallState::CALL_STATUS_DISCONNECTING) {
            endcallindex = j;
            endnum ++;
            if (endnum < HFP_AG_CALL_NUM_TWO) {
                HfpAgMessage evt(HFP_AG_SEND_CALL_STATE_EVT, HFP_AG_CALL_INACTIVE);
                SendMockEvent(evt);
            }
        } else if (callList_[j].callstate == (int)TelCallState::CALL_STATUS_HOLDING) {
            HfpAgMessage evt(HFP_AG_SEND_CALL_HELD_EVT, 1);
            SendMockEvent(evt);
            holdnum++;
        } else if (callList_[j].callstate == (int)TelCallState::CALL_STATUS_DIALING) {
            HfpAgMessage evt(HFP_AG_SEND_CALL_SETUP_EVT, HFP_AG_CALLSETUP_OUTGOING);
            SendMockEvent(evt);
        } else if (callList_[j].callstate == (int)TelCallState::CALL_STATUS_ALERTING) {
            HfpAgMessage evt(HFP_AG_SEND_CALL_SETUP_EVT, HFP_AG_CALLSETUP_ALERTING);
            SendMockEvent(evt);
        }
    }
    if (endcallindex != -1) {
        callList_.erase(callList_.begin() + endcallindex);
        endcallindex = -1;
    }
    if (activenum > 1) {
        HfpAgMessage evt(HFP_AG_SEND_CALL_HELD_EVT, 0);
        SendMockEvent(evt);
        LOG_INFO("HandlePhoneStateMock activenum = %{public}d", activenum);
    } else if (activenum == 0 && holdnum == 0 && callList_.size() == 0) {
        HfpAgMessage evt(HFP_AG_SEND_CALL_STATE_EVT, HFP_AG_CALL_INACTIVE);
        SendMockEvent(evt);
    }
    activenum = 0;
    holdnum = 0;
    endnum = 0;
}

bool HfpAgSystemInterface::HandleChldMock(int chld) const
{
    HfpAgMessage evtok(HFP_AG_RESPONE_OK_EVT);
    SendMockEvent(evtok);
    if (chld == ATCHLD_RELEASE_HOLD_ACCPET_OTHER) {
        HfpAgMessage evt(HFP_AG_SEND_CALL_HELD_EVT, HFP_AG_CALL_NUM_TWO);
        SendMockEvent(evt);
        HfpAgMessage evt1(HFP_AG_SEND_CALL_STATE_EVT, HFP_AG_CALL_ACTIVE);
        SendMockEvent(evt1);
        HfpAgMessage evt2(HFP_AG_SEND_CALL_SETUP_EVT, 0);
        SendMockEvent(evt2);
        evt.arg1_ = 1;
        SendMockEvent(evt);
    } else if (chld == ATCHLD_ADD_CALL_TO_CONVERSATION) {
        HfpAgMessage evt(HFP_AG_SEND_CALL_HELD_EVT, 0);
        SendMockEvent(evt);
        HfpAgMessage evt1(HFP_AG_SEND_CALL_STATE_EVT, HFP_AG_CALL_ACTIVE);
        SendMockEvent(evt1);
        HfpAgMessage evt2(HFP_AG_SEND_CALL_SETUP_EVT, 0);
        SendMockEvent(evt2);
    }
    return true;
}


bool HfpAgSystemInterface::HandleClccMock(std::string address) const
{
    HfpAgService *service = HfpAgService::GetService();
    int callindex = 0;
    for (auto mockcall : callList_) {
        callindex++;
        std::string number = mockcall.number;
        bool conferenceState = false;
        if (callList_.size() >1) {
            conferenceState = true;
        }
        service->ClccResponse(callindex, BT_CALL_DIRECTION_IN, mockcall.callstate,
            0, conferenceState, number, CALL_TYPE_DEFAULT);
    }
    callindex = 0;
    service->ResponesOK(address);
    return true;
}

void HfpAgSystemInterface::SendHfIndicator(const std::string &address, int indId, int indValue) const
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    RawAddress device(address);
    HfpAgService *service = HfpAgService::GetService();
    if (service == nullptr) {
        LOG_ERROR("[HFP AG]%{public}s():service is nullptr", __FUNCTION__);
    }
    if (indId == HFP_AG_HF_INDICATOR_ENHANCED_DRIVER_SAFETY_ID) {
        service->NotifyHfEnhancedDriverSafety(device, indValue);
        stub::TelephoneService::GetInstance()->NotifyHfEnhancedDriverSafety(indValue);
    } else if (indId == HFP_AG_HF_INDICATOR_BATTERY_LEVEL_ID) {
        service->NotifyHfBatteryLevel(device, indValue);
        stub::TelephoneService::GetInstance()->NotifyHfBatteryLevel(indValue);
    } else {
        LOG_INFO("[HFP AG]%{public}s():HF Indicator id:%{public}d", __FUNCTION__, indId);
    }
    return;
}

void HfpAgSystemInterface::SetActiveCallNumber(int number)
{
    activeNum_ = number;
}

int HfpAgSystemInterface::GetActiveCallNumber() const
{
    return activeNum_;
}

void HfpAgSystemInterface::SetHeldCallNumber(int number)
{
    heldNum_ = number;
}

int HfpAgSystemInterface::GetHeldCallNumber() const
{
    return heldNum_;
}

void HfpAgSystemInterface::SetCallState(int state)
{
    callState_ = state;
}

int HfpAgSystemInterface::GetCallState() const
{
    return callState_;
}

void HfpAgSystemInterface::SendServiceStateToService() const
{
    LOG_DEBUG("[HFP AG]%{public}s():enter",  __FUNCTION__);
    HfpAgService *service = HfpAgService::GetService();
    if (service != nullptr) {
        service->NotifyAgIndicatorStateChanged(HFP_AG_NOTIFY_SERVICE_STATE, serviceState_);
    } else {
        LOG_ERROR("[HFP AG]%{public}s():service is null!", __FUNCTION__);
    }
}


void HfpAgSystemInterface::SendIncomingCallToService(std::string number, int type) const
{
    LOG_DEBUG("[HFP AG]%{public}s():enter",  __FUNCTION__);
    HfpAgService *service = HfpAgService::GetService();
    if (service != nullptr) {
        service->NotifyAgIncomingStateChanged(HFP_AG_SEND_INCOMING_EVT, number, type);
    } else {
        LOG_ERROR("[HFP AG]%{public}s():service is null!", __FUNCTION__);
    }
}

void HfpAgSystemInterface::SendMockEvent(HfpAgMessage evt) const
{
    LOG_INFO("[HFP AG]%{public}s():",  __FUNCTION__);
    HfpAgService *service = HfpAgService::GetService();
    if (service != nullptr) {
        service->SendMockCmd(evt);
    } else {
        LOG_ERROR("[HFP AG]%{public}s():service is null!", __FUNCTION__);
    }
}

void HfpAgSystemInterface::SendClip(std::string number) const
{
    LOG_INFO("[HFP AG]%{public}s():",  __FUNCTION__);
    HfpAgService *service = HfpAgService::GetService();
    HfpAgMessage evt(HFP_AG_MOCK_CLIP);
    evt.str_ = number;
    if (service != nullptr) {
        service->SendMockCmd(evt);
    } else {
        LOG_ERROR("[HFP AG]%{public}s():service is null!", __FUNCTION__);
    }
}

void HfpAgSystemInterface::SendCallSetupToService(int state) const
{
    LOG_INFO("[HFP AG]%{public}s():",  __FUNCTION__);
    HfpAgService *service = HfpAgService::GetService();
    if (service != nullptr) {
        service->NotifyAgIndicatorStateChanged(HFP_AG_SEND_CALL_SETUP_EVT, state);
    } else {
        LOG_ERROR("[HFP AG]%{public}s():service is null!", __FUNCTION__);
    }
}

void HfpAgSystemInterface::SendCallHeldToService(int state) const
{
    LOG_INFO("[HFP AG]%{public}s():",  __FUNCTION__);
    HfpAgService *service = HfpAgService::GetService();
    if (service != nullptr) {
        service->NotifyAgIndicatorStateChanged(HFP_AG_SEND_CALL_HELD_EVT, state);
    } else {
        LOG_ERROR("[HFP AG]%{public}s():service is null!", __FUNCTION__);
    }
}

void HfpAgSystemInterface::SendCallStateToService(int state) const
{
    HfpAgService *service = HfpAgService::GetService();
    if (service != nullptr) {
        service->NotifyAgIndicatorStateChanged(HFP_AG_SEND_CALL_STATE_EVT, state);
    } else {
        LOG_ERROR("[HFP AG]%{public}s():service is null!", __FUNCTION__);
    }
}

void HfpAgSystemInterface::SendResponseHoldStateToService(int state) const
{
    LOG_INFO("[HFP AG]%{public}s():",  __FUNCTION__);
    HfpAgService *service = HfpAgService::GetService();
    if (service != nullptr) {
        service->NotifyAgResponseHoldStateChanged(HFP_AG_SEND_RESPONSE_HOLD_STATE, state);
    } else {
        LOG_ERROR("[HFP AG]%{public}s():service is null!", __FUNCTION__);
    }
}

void HfpAgSystemInterface::SendRoamStateToService() const
{
    LOG_INFO("[HFP AG]%{public}s():",  __FUNCTION__);
    HfpAgService *service = HfpAgService::GetService();
    if (service != nullptr) {
        service->NotifyAgIndicatorStateChanged(HFP_AG_NOTIFY_ROAM_STATE, roamState_);
    } else {
        LOG_ERROR("[HFP AG]%{public}s():service is null!", __FUNCTION__);
    }
}

void HfpAgSystemInterface::SendSignalStrengthToService() const
{
    LOG_DEBUG("[HFP AG]%{public}s():enter",  __FUNCTION__);
    HfpAgService *service = HfpAgService::GetService();
    if (service != nullptr) {
        service->NotifyAgIndicatorStateChanged(HFP_AG_NOTIFY_SIGNAL_STRENGTH, signalStrength_);
    } else {
        LOG_ERROR("[HFP AG]%{public}s():service is null!", __FUNCTION__);
    }
}

void HfpAgSystemInterface::SendBatteryLevelToService() const
{
    LOG_DEBUG("[HFP AG]%{public}s():enter",  __FUNCTION__);
    HfpAgService *service = HfpAgService::GetService();
    if (service != nullptr) {
        service->NotifyAgIndicatorStateChanged(HFP_AG_NOTIFY_BATTERY_LEVEL, batteryLevel_);
    } else {
        LOG_ERROR("[HFP AG]%{public}s():service is null!", __FUNCTION__);
    }
}

void HfpAgSystemInterface::OnSubscriptionStateChanged(int state)
{
    LOG_DEBUG("[HFP AG]%{public}s():enter",  __FUNCTION__);
    int preService = serviceState_;
    serviceState_ = state;
    if (preService != serviceState_) {
        SendServiceStateToService();
    }
}

void HfpAgSystemInterface::OnSignalStrengthChanged(int signal)
{
    LOG_DEBUG("[HFP AG]%{public}s():enter",  __FUNCTION__);
    int preSignal = signalStrength_;
    signalStrength_ = signal;
    if (preSignal != signalStrength_) {
        SendSignalStrengthToService();
    }
}

void HfpAgSystemInterface::OnRoamStateChanged(int state)
{
    LOG_DEBUG("[HFP AG]%{public}s():enter",  __FUNCTION__);
    int preRoam = roamState_;
    roamState_ = state;
    if (preRoam != roamState_) {
        SendRoamStateToService();
    }
}

void HfpAgSystemInterface::OnBatteryLevel(int level)
{
    LOG_DEBUG("[HFP AG]%{public}s():enter",  __FUNCTION__);
    int preBattery = batteryLevel_;
    batteryLevel_ = level;
    if (preBattery != batteryLevel_) {
        SendBatteryLevelToService();
    }
}

bool HfpAgSystemInterface::IsInCall() const
{
    LOG_DEBUG("[HFP AG]%{public}s():enter",  __FUNCTION__);
    return ((activeNum_ > 0) || (heldNum_ > 0) ||
            ((callState_ != HFP_AG_CALL_STATE_IDLE) && (callState_ != HFP_AG_CALL_STATE_INCOMING) &&
                (callState_ != HFP_AG_CALL_STATE_DISCONNECTED) && (callState_ != HFP_AG_CALL_STATE_DISCONNECTING)));
}

bool HfpAgSystemInterface::IsRinging() const
{
    LOG_DEBUG("[HFP AG]%{public}s():enter",  __FUNCTION__);
    return (callState_ == HFP_AG_CALL_STATE_INCOMING);
}

bool HfpAgSystemInterface::HandleChld(int chld) const
{
    HfpAgService *service = HfpAgService::GetService();
    if (service->GetMockState() == HFP_AG_MOCK) {
        return HandleChldMock(chld);
    }
    std::vector<CallAttributeInfo> callList;
    int slotId = INVALID_SLOT_ID;
    CoreServiceClient::GetInstance().GetPrimarySlotId(slotId);
    if (slotId < 0) {
        LOG_ERROR("[HFP AG]%{public}s():slotId_ is invalid",  __FUNCTION__);
    }
    callList = DelayedSingleton<BluetoothCallClient>::GetInstance()->GetCurrentCallList(slotId);
    for (auto call : callList) {
        std::string number = call.accountNumber;
        LOG_INFO("[HFP AG]%{public}s():HandleChld for number = %{public}s state = %{public}d",
            __FUNCTION__, number.c_str(), (int)call.callState);
            if (chld == ATCHLD_RELEASE_ALL_HELD_CALLS) {
                if ((int)TelCallState::CALL_STATUS_HOLDING == (int)call.callState ||
                    (int)TelCallState::CALL_STATUS_WAITING == (int)call.callState) {
                    DelayedSingleton<BluetoothCallClient>::GetInstance()->HangUpCall();
                }
            } else if (chld == ATCHLD_RELEASE_ACTIVE_ACCPET_OTHER) {
                if ((int)TelCallState::CALL_STATUS_WAITING == (int)call.callState &&
                    (int)call.callDirection == (int)CallDirection::CALL_DIRECTION_IN) {
                    DelayedSingleton<BluetoothCallClient>::GetInstance()->AnswerCall();
                } else if ((int)TelCallState::CALL_STATUS_ACTIVE == (int)call.callState) {
                    DelayedSingleton<BluetoothCallClient>::GetInstance()->HangUpCall();
                } else if ((int)TelCallState::CALL_STATUS_HOLDING == (int)call.callState) {
                    DelayedSingleton<BluetoothCallClient>::GetInstance()->HoldCall();
                }
            } else if (chld == ATCHLD_RELEASE_HOLD_ACCPET_OTHER) {
                if ((int)TelCallState::CALL_STATUS_WAITING == (int)call.callState) {
                    DelayedSingleton<BluetoothCallClient>::GetInstance()->AnswerCall();
                } else if ((int)TelCallState::CALL_STATUS_ACTIVE == (int)call.callState) {
                    DelayedSingleton<BluetoothCallClient>::GetInstance()->HoldCall();
                }
            } else if (chld == ATCHLD_ADD_CALL_TO_CONVERSATION) {
                if ((int)TelCallState::CALL_STATUS_WAITING == (int)call.callState) {
                    DelayedSingleton<BluetoothCallClient>::GetInstance()->AnswerCall();
                    DelayedSingleton<BluetoothCallClient>::GetInstance()->CombineConference();
                }
            } else if (chld == ATCHLD_CONNECT_TWO_CALL) {
                DelayedSingleton<BluetoothCallClient>::GetInstance()->CombineConference();
                DelayedSingleton<BluetoothCallClient>::GetInstance()->HangUpCall();
            }
    }
    return true;
}

void HfpAgSystemInterface::AgTelephonyObserver::OnNetworkStateUpdated(int32_t slotId,
    const sptr<NetworkState> &networkState)
{
    HILOGI("[HFP AG] slotId: %{public}d", slotId);
    if (networkState == nullptr) {
        HILOGE("[HFP AG] networkState is null");
        return;
    }
    interface_.slotId_ = slotId;
    interface_.operatorName_ = networkState->GetLongOperatorName();
    interface_.OnSubscriptionStateChanged(static_cast<std::underlying_type<RegServiceState>::type>(
        networkState->GetRegStatus()));
    interface_.OnRoamStateChanged(networkState->IsRoaming());
}

void HfpAgSystemInterface::AgTelephonyObserver::OnSignalInfoUpdated(int32_t slotId,
    const std::vector<sptr<SignalInformation>> &vec)
{
    LOG_DEBUG("[HFP AG]%{public}s():enter",  __FUNCTION__);
    interface_.slotId_ = slotId;
    if (!vec.empty()) {
        interface_.OnSignalStrengthChanged(vec[0]->GetSignalLevel());
    }
}

inline std::u16string HfpAgSystemInterface::Str8ToStr16(const std::string& str) const
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::u16string result = convert.from_bytes(str);
    return result;
}

inline std::string HfpAgSystemInterface::Str16ToStr8(const std::u16string& str) const
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::string result = convert.to_bytes(str);
    return result;
}

void SystemAbilityStatusChange::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    HILOGI("OnAddSystemAbility systemAbilityId:%{public}d", systemAbilityId);
    switch (systemAbilityId) {
        case TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID:
            HILOGI("OnAddSystemAbility input service start");
            DelayedSingleton<BluetoothCallClient>::GetInstance()->Init();
            break;
        default:
            HILOGI("OnAddSystemAbility unhandled sysabilityId:%{public}d", systemAbilityId);
            break;
    }
    return;
}

void SystemAbilityStatusChange::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    HILOGI("OnRemoveSystemAbility: start!");
    return;
}
}  // namespace bluetooth
}  // namespace OHOS
