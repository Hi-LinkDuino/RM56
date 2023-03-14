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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines map client stm object.
 *
 */

/**
 * @file map_mce_instance_stm.cpp
 *
 * @brief map instance client stm source file .
 *
 */

#include "map_mce_instance_stm.h"
#include "log.h"
#include "map_mce_mns_server.h"

namespace OHOS {
namespace bluetooth {
void MapMceInstanceStm::CreateStm()
{
    btDevice_ = mceDeviceMgr_.GetBtDevice();
    btAddress_ = mceDeviceMgr_.GetBtAddress();

    // create mas client
    MapMceObserverManager &observer = mceDeviceMgr_.GetDeviceCallbackMgr();
    masClient_ = std::make_unique<MapMceInstanceClient>(
        *this, mceStmDispacher_, observer, insDefaultConfig_, mceInstanceStmMutex_);
    // main state create
    std::unique_ptr<State> disconnectedState = std::make_unique<MapMceDisconnectedState>(*this);
    std::unique_ptr<State> connectingState = std::make_unique<MapMceConnectingState>(*this);
    std::unique_ptr<State> connectedState = std::make_unique<MapMceConnectedState>(*this);
    std::unique_ptr<State> disconnectingState = std::make_unique<MapMceDisconnectingState>(*this);
    // connected sub state
    std::unique_ptr<State> sendingState = std::make_unique<MapMceConnectedStateSubReqSending>(*this, *connectedState);

    // add main state
    Move(disconnectedState);
    Move(connectingState);
    Move(connectedState);
    Move(disconnectingState);
    // add connected sub state
    Move(sendingState);
    // init status
    InitState(MCE_DISCONNECTED_STATE);
}

MapMceInstanceStm::MapMceInstanceStm(
    MapMceDeviceCtrl &deviceMgr, utility::Dispatcher &dispatcher, int instanceId, const MasInstanceConfig &config)
    : mceDeviceMgr_(deviceMgr), mceStmDispacher_(dispatcher)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    masInstanceId_ = instanceId;
    targetState_ = MCE_INSTANCE_STATE_DISCONNECTED;
    masClient_ = nullptr;
    insDefaultConfig_ = config;
    stmSendFlag = false;
    stmDataContinueFlag_ = false;
    stmDataStmBusyFlag_ = false;
    busyFlag_ = false;
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
};

MapMceInstanceStm::~MapMceInstanceStm()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

void MapMceInstanceStm::PostMessage(utility::Message msg)
{
    LOG_INFO("%{public}s execute, msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    mceStmDispacher_.PostTask(std::bind(&MapMceInstanceStm::MceProcessMessage, this, msg));
}

void MapMceInstanceStm::PostMessageWithRequest(utility::Message msg, std::unique_ptr<MapMceInstanceRequest> &req)
{
    LOG_INFO("%{public}s enter, msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    std::lock_guard<std::recursive_mutex> lock(mceInstanceStmMutex_);

    if (!stmSendFlag) {
        stmSendFlag = true;
        stmRequestPtr_ = std::move(req);
        mceStmDispacher_.PostTask(std::bind(&MapMceInstanceStm::MceProcessMessage, this, msg));
    } else {
        masClient_->ClientSaveRequest(req);
    }
    req = nullptr;
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceInstanceStm::MceProcessMessageWithRequest(utility::Message msg, std::unique_ptr<MapMceInstanceRequest> &req)
{
    LOG_INFO("%{public}s enter, msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    std::lock_guard<std::recursive_mutex> lock(mceInstanceStmMutex_);

    MceProcessMessageWithRequestInternal(msg, req);
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceInstanceStm::MceProcessMessageWithRequestInternal(
    utility::Message msg, std::unique_ptr<MapMceInstanceRequest> &req)
{
    LOG_INFO("%{public}s execute, msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    if (!stmSendFlag) {
        stmSendFlag = true;
        stmRequestPtr_ = std::move(req);
        ProcessMessage(msg);
        if ((msg.what_ == MSG_MASSTM_REQ_SEND_REQUEST_SELF) || (msg.what_ == MSG_MASSTM_REQ_SEND_REQUEST)) {
            stmSendFlag = false;
        }
    } else {
        masClient_->ClientSaveRequest(req);
    }
    req = nullptr;
}

bool MapMceInstanceStm::GetBusyStatus()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    return busyFlag_;
}

void MapMceInstanceStm::ChangeContinueBusyStatus(bool status)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    bool current = false;

    stmDataContinueFlag_ = status;
    if (stmDataContinueFlag_ || stmDataStmBusyFlag_) {
        current = true;
    }
    if (busyFlag_ != current) {
        busyFlag_ = current;
        // send to dev ctrl
        utility::Message outMsg(MSG_MCEDEVICE_INSCLIENT_INFO_BUSY_STATUS_CHANGE);
        outMsg.arg1_ = int(busyFlag_);
        mceDeviceMgr_.PostMessage(outMsg);
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceInstanceStm::ChangeStmBusyStatus(bool status)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    bool current = false;

    stmDataStmBusyFlag_ = status;
    if (stmDataContinueFlag_ || stmDataStmBusyFlag_) {
        current = true;
    }
    if (busyFlag_ != current) {
        busyFlag_ = current;
        // send to dev ctrl
        utility::Message outMsg(MSG_MCEDEVICE_INSCLIENT_INFO_BUSY_STATUS_CHANGE);
        outMsg.arg1_ = int(busyFlag_);
        mceDeviceMgr_.PostMessage(outMsg);
    }
}

MapMceInstanceClient &MapMceInstanceStm::GetMasClient()
{
    return *masClient_;
}

void MapMceInstanceStm::PostMessageWithObexHeader(utility::Message msg, ObexHeader head)
{
    LOG_INFO("%{public}s execute, msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    mceStmDispacher_.PostTask(std::bind(&MapMceInstanceStm::MceProcessMessageWithObexHeader, this, msg, head));
}

void MapMceInstanceStm::MceProcessMessageWithObexHeader(utility::Message msg, ObexHeader head)
{
    stmObexHeader_ = std::make_unique<ObexHeader>(head);
    MceProcessMessage(msg);
}

void MapMceInstanceStm::MceProcessMessage(utility::Message msg)
{
    LOG_INFO("%{public}s enter,msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    std::lock_guard<std::recursive_mutex> lock(mceInstanceStmMutex_);

    ProcessMessage(msg);

    switch (msg.what_) {
        case MSG_MASSTM_REQ_SEND_REQUEST_SELF:
        case MSG_MASSTM_REQ_SEND_REQUEST:
            stmSendFlag = false;
            break;
        case MSG_MASSTM_OBEX_INFO_CHG_CONTINUEBUSY_STATUS:
            ChangeContinueBusyStatus((bool)msg.arg1_);
            break;
        default:
            break;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

MceInstanceStateType MapMceInstanceStm::GetTargetState()
{
    return targetState_;
}

std::string MapMceInstanceStm::GetCurrentMceStmStateName()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceInstanceStmMutex_);

    std::string state = GetState()->Name();
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return state;
}

std::string MapMceInstanceStm::GetBtDevice()
{
    return btDevice_;
}

BtAddr MapMceInstanceStm::GetBtAddress()
{
    return btAddress_;
}

void MapMceInstanceStm::TansTargetState(MceInstanceStateType state)
{
    LOG_INFO("%{public}s execute, state=%{public}d", __PRETTY_FUNCTION__, state);
    targetState_ = state;
}

// MCE_DISCONNECTED_STATE
void MapMceInstanceStm::MapMceDisconnectedState::Entry()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

void MapMceInstanceStm::MapMceDisconnectedState::Exit()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

bool MapMceInstanceStm::MapMceDisconnectedState::Dispatch(const utility::Message &msg)
{
    LOG_INFO("%{public}s enter, msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    bool ret = true;
    switch (msg.what_) {
        case MSG_MASSTM_REQ_CONNECT:
            mceInstanceStm_.TansTargetState(MCE_INSTANCE_STATE_CONNECTED);
            Transition(MCE_CONNECTING_STATE);
            break;
        case MSG_MASSTM_REQ_DISCONNECT:
            // set target status disconnect
            mceInstanceStm_.TansTargetState(MCE_INSTANCE_STATE_DISCONNECTED);
            break;
        default:
            ret = false;
            break;
    }
    LOG_INFO("%{public}s exit", __PRETTY_FUNCTION__);
    return ret;
}

// MCE_DISCONNECTING_STATE
void MapMceInstanceStm::MapMceDisconnectingState::Entry()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);

    // obex client execute disconnect
    if (mceInstanceStm_.masClient_ != nullptr) {
        int retVal = mceInstanceStm_.masClient_->StartClientDisConnect();
        if (retVal != BT_NO_ERROR) {
            // error occur
            // post msg to mns state machine instance. execute in mns service thread
            utility::Message outMsg = MSG_MASSTM_OBEX_DISCONNECTED;
            mceInstanceStm_.PostMessage(outMsg);
            LOG_INFO("%{public}s start obex error", __PRETTY_FUNCTION__);
        }
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceInstanceStm::MapMceDisconnectingState::Exit()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

bool MapMceInstanceStm::MapMceDisconnectingState::Dispatch(const utility::Message &msg)
{
    LOG_INFO("%{public}s enter, msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    bool ret = true;
    utility::Message outMsg(MSG_MCEDEVICE_INSCLIENT_TRANSPORT_ERRO);
    switch (msg.what_) {
        case MSG_MASSTM_OBEX_TRANSPORT_FAILED:
            Transition(MCE_DISCONNECTED_STATE);
            mceInstanceStm_.TansTargetState(MCE_INSTANCE_STATE_DISCONNECTED);
            mceInstanceStm_.masClient_->ProcessDisConnectFinish();
            // post message to instance stm
            outMsg.what_ = MSG_MCEDEVICE_INSCLIENT_TRANSPORT_ERRO;
            mceInstanceStm_.mceDeviceMgr_.PostMessage(outMsg);
            break;
        case MSG_MASSTM_OBEX_DISCONNECTED:
            // disconnected finish
            mceInstanceStm_.masClient_->ProcessDisConnectFinish();
            // send disconnected to device mgr
            outMsg.what_ = MSG_MCEDEVICE_INSCLIENT_DISCONNECTED;
            mceInstanceStm_.mceDeviceMgr_.PostMessage(outMsg);
            // check target state
            if (mceInstanceStm_.GetTargetState() == MCE_INSTANCE_STATE_CONNECTED) {
                // connecting again
                Transition(MCE_CONNECTING_STATE);
            } else {
                // change to disconnected
                Transition(MCE_DISCONNECTED_STATE);
            }
            break;
        case MSG_MASSTM_REQ_CONNECT:
            // set target mode connect
            mceInstanceStm_.TansTargetState(MCE_INSTANCE_STATE_CONNECTED);
            break;
        case MSG_MASSTM_REQ_DISCONNECT:
            // set target mode disconnect
            mceInstanceStm_.TansTargetState(MCE_INSTANCE_STATE_DISCONNECTED);
            break;
        default:
            ret = false;
            // default ret
            break;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

// MCE_CONNECTING_STATE
void MapMceInstanceStm::MapMceConnectingState::Entry()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    // start connecting.
    if (mceInstanceStm_.masClient_->StartClientConnect() != BT_NO_ERROR) {
        // error process
        utility::Message outMsg(MSG_MASSTM_OBEX_DISCONNECTED);
        mceInstanceStm_.PostMessage(outMsg);
        LOG_ERROR("%{public}s StartClientConnect execute error", __PRETTY_FUNCTION__);
    }
}

void MapMceInstanceStm::MapMceConnectingState::Exit()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

void MapMceInstanceStm::MapMceConnectingState::DispatchProcObexConnected()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    // check target state
    if (mceInstanceStm_.GetTargetState() == MCE_INSTANCE_STATE_DISCONNECTED) {
        // start disconnecting
        Transition(MCE_DISCONNECTING_STATE);
    } else {
        // change to connected state
        Transition(MCE_CONNECTED_STATE);
        mceInstanceStm_.masClient_->ClientSendReqGetMasInstanceInformation();
        std::u16string setpathPaths = u"";
        std::vector<std::u16string> setpathPathList;
        setpathPathList.push_back(MCE_FOLDER_TELECOM);
        setpathPathList.push_back(MCE_FOLDER_MSG);
        mceInstanceStm_.masClient_->ClientSendReqSetPath(int(MASSTM_SETPATH_LIST), setpathPaths, setpathPathList);
    }
}

void MapMceInstanceStm::MapMceConnectingState::DispatchProcObexDisconnected()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    // receive the obex disconnected error response
    Transition(MCE_DISCONNECTED_STATE);
    mceInstanceStm_.TansTargetState(MCE_INSTANCE_STATE_DISCONNECTED);
    mceInstanceStm_.masClient_->ProcessDisConnectFinish();
    // send disconnect to divece mgr
    utility::Message outMsg(MSG_MCEDEVICE_INSCLIENT_DISCONNECTED);
    mceInstanceStm_.mceDeviceMgr_.PostMessage(outMsg);
}

void MapMceInstanceStm::MapMceConnectingState::DispatchProcTransportFailed(const utility::Message &msg)
{
    LOG_ERROR("%{public}s enter", __PRETTY_FUNCTION__);
    if (msg.arg1_ == CONNECT_COLLISION) {  // transport's micro define
        // retry connecting for l2cap error
        mceInstanceStm_.masClient_->ReconnectAfterTransportFailed();
    } else {
        Transition(MCE_DISCONNECTED_STATE);
        mceInstanceStm_.TansTargetState(MCE_INSTANCE_STATE_DISCONNECTED);
        mceInstanceStm_.masClient_->ProcessDisConnectFinish();
        // send transport error to divece mgr
        utility::Message outMsg(MSG_MCEDEVICE_INSCLIENT_TRANSPORT_ERRO);
        mceInstanceStm_.mceDeviceMgr_.PostMessage(outMsg);
    }
}

void MapMceInstanceStm::MapMceConnectingState::DispatchProcObexConnectedFailed()
{
    LOG_ERROR("%{public}s enter", __PRETTY_FUNCTION__);
    // receive the obex connected failed response
    mceInstanceStm_.TansTargetState(MCE_INSTANCE_STATE_DISCONNECTED);
    int ret = mceInstanceStm_.masClient_->ObexConnectFailedDisConnect();
    if (ret == BT_NO_ERROR) {
        Transition(MCE_DISCONNECTING_STATE);
    } else {
        Transition(MCE_DISCONNECTED_STATE);
        mceInstanceStm_.masClient_->ProcessDisConnectFinish();
        utility::Message outMsg(MSG_MCEDEVICE_INSCLIENT_CONNECT_FAILED);
        mceInstanceStm_.mceDeviceMgr_.PostMessage(outMsg);
        LOG_ERROR("%{public}s obex disconnect error", __PRETTY_FUNCTION__);
    }
}

void MapMceInstanceStm::MapMceConnectingState::DispatchProcConnectedFailed()
{
    LOG_ERROR("%{public}s enter", __PRETTY_FUNCTION__);
    // receive the obex connected failed response
    mceInstanceStm_.TansTargetState(MCE_INSTANCE_STATE_DISCONNECTED);
    Transition(MCE_DISCONNECTED_STATE);
    mceInstanceStm_.masClient_->ProcessDisConnectFinish();
    // send connect failed to divece mgr
    utility::Message outMsg(MSG_MCEDEVICE_INSCLIENT_CONNECT_FAILED);
    mceInstanceStm_.mceDeviceMgr_.PostMessage(outMsg);
}

bool MapMceInstanceStm::MapMceConnectingState::Dispatch(const utility::Message &msg)
{
    LOG_INFO("%{public}s enter, msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    bool ret = true;
    int btRet = BT_NO_ERROR;
    switch (msg.what_) {
        case MSG_MASSTM_OBEX_CONNECTED:
            DispatchProcObexConnected();
            break;
        case MSG_MASSTM_OBEX_DISCONNECTED:  // error process
            DispatchProcObexDisconnected();
            break;
        case MSG_MASSTM_OBEX_TRANSPORT_FAILED:
            DispatchProcTransportFailed(msg);
            break;
        case MSG_MASSTM_OBEX_CONNECTED_FAILED:
            DispatchProcObexConnectedFailed();
            break;
        case MSG_MASSTM_REQ_CONNECT:
            // set target mod connect
            mceInstanceStm_.TansTargetState(MCE_INSTANCE_STATE_CONNECTED);
            break;
        case MSG_MASSTM_REQ_DISCONNECT:
            // set target mod disconnect
            mceInstanceStm_.TansTargetState(MCE_INSTANCE_STATE_DISCONNECTED);
            break;
        case MSG_MASSTM_GAP_REQUEST_FINISH:
            btRet = mceInstanceStm_.masClient_->ExcuteObexConnect();
            if (btRet != BT_NO_ERROR) {
                DispatchProcConnectedFailed();
                LOG_ERROR("%{public}s executeObexConnect error after GAP", __PRETTY_FUNCTION__);
            }
            break;
        case MSG_MASSTM_GAP_REQUEST_FAILED:
            DispatchProcConnectedFailed();
            break;
        default:
            ret = false;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

// MCE_CONNECTED_STATE
void MapMceInstanceStm::MapMceConnectedState::Entry()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);

    utility::Message outMsg(MSG_MCEDEVICE_INSCLIENT_CONNECTED);
    mceInstanceStm_.mceDeviceMgr_.PostMessage(outMsg);
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

/**
 * @brief exit
 */
void MapMceInstanceStm::MapMceConnectedState::Exit()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

/**
 * @brief dispatch the state msg
 * @param  msg              message to process
 * @return true     this msg has been process ok
 * @return false    do not process , the parant can be process again
 */
bool MapMceInstanceStm::MapMceConnectedState::Dispatch(const utility::Message &msg)
{
    LOG_INFO("%{public}s enter, msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    bool ret = true;
    utility::Message outMsg(MSG_MCEDEVICE_INSCLIENT_DISCONNECTED);
    switch (msg.what_) {
        case MSG_MASSTM_REQ_CONNECT:
            mceInstanceStm_.TansTargetState(MCE_INSTANCE_STATE_CONNECTED);
            break;
        case MSG_MASSTM_REQ_DISCONNECT:
            // if sub state is not running, start disconnect
            mceInstanceStm_.TansTargetState(MCE_INSTANCE_STATE_DISCONNECTED);
            // start disconnect sequence
            Transition(MCE_DISCONNECTING_STATE);
            break;
        case MSG_MASSTM_OBEX_DISCONNECTED:
            // clear state and var, then trans to disconncted
            Transition(MCE_DISCONNECTED_STATE);
            mceInstanceStm_.TansTargetState(MCE_INSTANCE_STATE_DISCONNECTED);
            mceInstanceStm_.masClient_->ProcessDisConnectFinish();
            // send to device mgr
            outMsg.what_ = MSG_MCEDEVICE_INSCLIENT_DISCONNECTED;
            mceInstanceStm_.mceDeviceMgr_.PostMessage(outMsg);
            break;
        case MSG_MASSTM_OBEX_TRANSPORT_FAILED:
            Transition(MCE_DISCONNECTED_STATE);
            mceInstanceStm_.TansTargetState(MCE_INSTANCE_STATE_DISCONNECTED);
            mceInstanceStm_.masClient_->ProcessDisConnectFinish();
            // send to device mgr
            outMsg.what_ = MSG_MCEDEVICE_INSCLIENT_TRANSPORT_ERRO;
            mceInstanceStm_.mceDeviceMgr_.PostMessage(outMsg);
            break;
        case MSG_MASSTM_REQ_SEND_REQUEST:       // msg from service api
        case MSG_MASSTM_REQ_SEND_REQUEST_SELF:  // msg from instance client
            // process sending reqest
            if (mceInstanceStm_.stmRequestPtr_ != nullptr) {
                int sendRet = mceInstanceStm_.masClient_->ClientSendRequest(mceInstanceStm_.stmRequestPtr_);
                if (sendRet == BT_NO_ERROR) {
                    // trans to request sending
                    Transition(MCE_CONNECTED_STATE_S_REQSENDING);
                } else {
                    LOG_ERROR("%{public}s obex send request error", __PRETTY_FUNCTION__);
                }
            } else {
                LOG_ERROR("%{public}s stmRequestPtr_ null", __PRETTY_FUNCTION__);
            }
            break;
        default:
            ret = false;
            break;
    }
    return ret;
}

// MCE_CONNECTED_STATE_S_REQSENDING
void MapMceInstanceStm::MapMceConnectedStateSubReqSending::Entry()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    mceInstanceStm_.ChangeStmBusyStatus(true);
}

void MapMceInstanceStm::MapMceConnectedStateSubReqSending::Exit()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    mceInstanceStm_.ChangeStmBusyStatus(false);
}

void MapMceInstanceStm::MapMceConnectedStateSubReqSending::ProcReceiveRequestComplite(const utility::Message &msg)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    if (mceInstanceStm_.stmObexHeader_ == nullptr) {
        LOG_ERROR("%{public}s stmObexHeader_ NULL", __PRETTY_FUNCTION__);
        return;
    }
    // process response
    int responseRet = mceInstanceStm_.masClient_->ClientProcResponseCommonProcess(*mceInstanceStm_.stmObexHeader_);
    if ((responseRet == MCE_RESPONSE_FINISH) || (responseRet == MCE_RESPONSE_FINISH_NG) ||
        (responseRet == MCE_RESPONSE_FINISH_NO_CALLBACK)) {  // current request process finished
        // check target state
        if (mceInstanceStm_.GetTargetState() == MCE_INSTANCE_STATE_DISCONNECTED) {
            // release memory
            mceInstanceStm_.masClient_->ClientDeleteAllSavedRequest();
            // start disconnecting
            Transition(MCE_DISCONNECTING_STATE);
        } else {
            mceInstanceStm_.masClient_->ClientSendSavedRequest();
            // no another request execute, then trans to idle connected state
            if (mceInstanceStm_.masClient_->GetCurrentRequest() == MCE_REQUEST_TYPE_IDLE) {
                Transition(MCE_CONNECTED_STATE);
            }
        }
    } else {
        // last request continue execute
    }
}

bool MapMceInstanceStm::MapMceConnectedStateSubReqSending::Dispatch(const utility::Message &msg)
{
    LOG_INFO("%{public}s enter, msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
    bool ret = true;
    switch (msg.what_) {
        case MSG_MASSTM_RECEIVE_REQUEST_COMPLETED:
            ProcReceiveRequestComplite(msg);
            break;
        case MSG_MASSTM_REQ_CONNECT:
            // set target state
            mceInstanceStm_.TansTargetState(MCE_INSTANCE_STATE_CONNECTED);
            break;
        case MSG_MASSTM_REQ_DISCONNECT:
            // set target state
            mceInstanceStm_.TansTargetState(MCE_INSTANCE_STATE_DISCONNECTED);
            break;
        case MSG_MASSTM_REQ_SEND_REQUEST:
        case MSG_MASSTM_REQ_SEND_REQUEST_SELF:
            // when sending , another send request coming
            // added the request to the sending que
            if (mceInstanceStm_.stmRequestPtr_ != nullptr) {
                mceInstanceStm_.masClient_->ClientSaveRequest(mceInstanceStm_.stmRequestPtr_);
            } else {
                LOG_ERROR("%{public}s stmRequestPtr_ null", __PRETTY_FUNCTION__);
            }
            break;
        default:
            ret = false;
            LOG_INFO("%{public}s ret false, msg=0x%x", __PRETTY_FUNCTION__, msg.what_);
            break;
    }
    LOG_INFO("%{public}s end,ret=%{public}d", __PRETTY_FUNCTION__, ret);
    return ret;
}
}  // namespace bluetooth
}  // namespace OHOS
