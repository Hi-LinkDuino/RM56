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
 * @brief Defines map client instance object.
 *
 */

/**
 * @file map_mce_instance_client.cpp
 *
 * @brief map instance client source file .
 *
 */
#include "map_mce_instance_client.h"
#include "btm.h"
#include "log.h"
#include "map_mce_instance_stm.h"
#include "map_mce_mns_server.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
// client uuid
const uint8_t MapMceInstanceClient::mceClientUuidTbl_[MAX_OF_MASCLIENT_OBEX_UUID_TBL] = {
    0xbb, 0x58, 0x2b, 0x40, 0x42, 0x0c, 0x11, 0xdb, 0xb0, 0xde, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66
};

// call back
void MapMceRequestSecurityCb(uint16_t result, GapServiceSecurityInfo security, void *context)
{
    LOG_INFO("%{public}s enter, result = %{public}d", __PRETTY_FUNCTION__, int(result));
    auto pThis = static_cast<MapMceInstanceClient *>(context);
    if (pThis != nullptr) {
        pThis->OnGapRequestSecurityCb(result);
    }
}

MapMceInstanceClient::MapMceInstanceClient(utility::StateMachine &mapStm, utility::Dispatcher &dispacher,
    MapMceObserverManager &observeMgr, const MasInstanceConfig &config, std::recursive_mutex &mutex)
    : instanceStm_(mapStm), mceDispacher_(dispacher), observerMgrPtr_(observeMgr), stmMutex_(mutex)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    // same statemachine
    obexClientIns_ = nullptr;
    insDefaultConfig_ = config;
    currentRequestPtr_ = nullptr;
    masRequestQue_.clear();
    // response data init;
    resMasInstanceInfo_.isValid = false;
    resMasInstanceInfo_.masInstanceInfoUtf8 = "";
    resMasInstanceInfo_.ownerUciUtf8 = "";
    masObexObserver_ = nullptr;
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
};

MapMceInstanceClient::~MapMceInstanceClient()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    if (currentRequestPtr_ != nullptr) {
        LOG_INFO("%{public}s currentRequestPtr_ is not null", __PRETTY_FUNCTION__);
        currentRequestPtr_ = nullptr;
    }
    // clear the request list
    ClientDeleteAllSavedRequestInternal();
}

// call by device ctrl
void MapMceInstanceClient::SetDefaultSdpParam(const MasSdpParam &sdpSave)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(stmMutex_);

    masSdpParamSave_ = sdpSave;
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

// call by device ctrl
MasSdpParam MapMceInstanceClient::GetMasSdpParam()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(stmMutex_);

    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return masSdpParamSave_;
}

// call by client internal
void MapMceInstanceClient::SetObexConfig()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    BtAddr btAddr;
    BtUuid btUuid;
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;
    btUuid.type = BT_PUBLIC_DEVICE_ADDRESS;
    (void)memcpy_s(btUuid.uuid128, MAX_OF_MASCLIENT_OBEX_UUID_TBL,
        mceClientUuidTbl_, MAX_OF_MASCLIENT_OBEX_UUID_TBL);
    RawAddress rawAddr(((MapMceInstanceStm &)instanceStm_).GetBtDevice());
    rawAddr.ConvertToUint8(btAddr.addr, sizeof(btAddr.addr));

    // set obex config value
    masClientConfig_.addr_ = btAddr;
    masClientConfig_.isGoepL2capPSM_ = masSdpParamSave_.isGoepL2capPSM;  // rfcomm is false or l2cap is true
    masClientConfig_.mtu_ = insDefaultConfig_.rfcommMtu;
#ifdef MCE_DISABLE_L2CAP
    // GOEP 2.0 srm set true
    masClientConfig_.isSupportSrm_ = insDefaultConfig_.isSupportSrm;
#else
    // GOEP 2.0 srm set true
    if (masClientConfig_.isGoepL2capPSM_) {
        masClientConfig_.isSupportSrm_ = true;
        masClientConfig_.mtu_ = insDefaultConfig_.l2capMtu;
    }
#endif
    masClientConfig_.isSupportReliableSession_ = false;
    masClientConfig_.scn_ = masSdpParamSave_.scn;
    masClientConfig_.serviceUUID_ = btUuid;
    masClientConfig_.lpsm_ = MAP_MCE_GOEP_L2CAP_PSM_VALUE;
}

// call by stm
int MapMceInstanceClient::StartClientConnect()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret;
    BtAddr addr = ((MapMceInstanceStm &)instanceStm_).GetBtAddress();
    // prepare obex config
    SetObexConfig();
    // set obex config data , set obex observer
    if (masObexObserver_ == nullptr) {
        masObexObserver_ = std::make_unique<MasObexClientObserver>(instanceStm_);
    }
    if (obexClientIns_ == nullptr) {
        obexClientIns_ =
            std::make_unique<ObexMpClient>((const ObexClientConfig)masClientConfig_, *masObexObserver_, mceDispacher_);
        LOG_INFO("%{public}s,new ObexMpClient, rawAddr=%{public}s,isL2cap=%{public}d,scn=0x%x,lpsm=0x%x,instId=%{public}d",
            __PRETTY_FUNCTION__,
            ((MapMceInstanceStm &)instanceStm_).GetBtDevice().c_str(),
            masClientConfig_.isGoepL2capPSM_,
            masClientConfig_.scn_,
            masClientConfig_.lpsm_,
            int(masSdpParamSave_.instanceId));
    }
    // register gap
    ret = RegisterServiceSecurity(securityInfo_);
    if (ret == BT_NO_ERROR) {
        if (masClientConfig_.isGoepL2capPSM_ != true) {
            // base on rfcomm， request gap is doing in rfcomm
            ret = ExcuteObexConnectInternal();
            BTM_AddRemoteRfcommScnForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, masClientConfig_.scn_, &addr);
        } else {
            // base on l2cap, mce need request gap self
            ret = RequestSecurity(securityInfo_);
            BTM_AddRemoteL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, masClientConfig_.scn_, &addr);
        }
    } else {
        LOG_ERROR("%{public}s Register gap error", __PRETTY_FUNCTION__);
    }
    return ret;
}

int MapMceInstanceClient::ReconnectAfterTransportFailed()
{
    LOG_ERROR("%{public}s, execute", __PRETTY_FUNCTION__);
    return RequestSecurity(securityInfo_);
}

// call by stm
int MapMceInstanceClient::ExcuteObexConnect()
{
    LOG_INFO("%{public}s, obex->Connect() execute", __PRETTY_FUNCTION__);
    return ExcuteObexConnectInternal();
}

int MapMceInstanceClient::ExcuteObexConnectInternal()
{
    LOG_INFO("%{public}s, obex->Connect() execute", __PRETTY_FUNCTION__);
    // set obex param value
    TlvTriplet supportedFeatures(MCE_MAP_SUPPORTED_FEATURES, MAP_MCE_SUPPORTED_FEATURES_V14);
    ObexTlvParamters params;
    if (CheckSupportedFeatures(MAP_SUPPORTED_FEATURES_MAPSUPPORTED_FEATURES)) {
        params.AppendTlvtriplet(supportedFeatures);
    }
    // obex connect execute
    ObexConnectParams obexParam;
    obexParam.appParams_ = &params;
    int ret = obexClientIns_->Connect(obexParam);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s, obex->Connect() execute error, ret=%{public}d", __PRETTY_FUNCTION__, ret);
    }
    return ret;
}

// internal
int MapMceInstanceClient::RegisterServiceSecurity(GapServiceSecurityInfo &info)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    info.direction = OUTGOING;
    info.serviceId = masSdpParamSave_.gapNumber;
    // init
    info.channelId.l2capPsm = 0;
    info.channelId.rfcommChannel = 0;

    // set gap param
    if (masSdpParamSave_.isGoepL2capPSM) {
        // l2cap
        info.protocolId = SEC_PROTOCOL_L2CAP;  // if l2cap ,then  true
        info.channelId.l2capPsm = masSdpParamSave_.scn;
    } else {
        // rfcomm
        info.protocolId = SEC_PROTOCOL_RFCOMM;  // if not l2cap ,then  false
        info.channelId.rfcommChannel = masSdpParamSave_.scn;
    }
    // register gap
    uint16_t securityMode =
        GAP_SEC_IN_AUTHENTICATION | GAP_SEC_IN_ENCRYPTION | GAP_SEC_OUT_AUTHENTICATION | GAP_SEC_OUT_ENCRYPTION;
    LOG_INFO("%{public}s gap execute,dir=%{public}d,servic=%{public}d,protocol=%{public}d,rfchannel=%{public}d,l2psm=0x%x,securityMode=0x%x",
        __PRETTY_FUNCTION__,
        info.direction,
        info.serviceId,
        info.protocolId,
        info.channelId.rfcommChannel,
        info.channelId.l2capPsm,
        securityMode);
    // bluetooth device address
    BtAddr addr = ((MapMceInstanceStm &)instanceStm_).GetBtAddress();
    int ret = GAPIF_RegisterServiceSecurity(&addr, &info, securityMode);
    LOG_INFO("%{public}s end,ret=%{public}d", __PRETTY_FUNCTION__, ret);
    return ret;
}

// internal
int MapMceInstanceClient::RequestSecurity(GapServiceSecurityInfo info)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    GapRequestSecurityParam requestParam;
    requestParam.info = info;
    requestParam.callback = MapMceRequestSecurityCb;
    requestParam.context = this;

    BtAddr addr = ((MapMceInstanceStm &)instanceStm_).GetBtAddress();
    int ret = GAPIF_RequestSecurity(&addr, &requestParam);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s GAPIF_RequestSecurity execute error", __PRETTY_FUNCTION__);
    }
    return ret;
}

// call back
void MapMceInstanceClient::OnGapRequestSecurityCb(uint16_t result)
{
    // Request Security callback
    LOG_INFO("%{public}s enter, result = %{public}d", __PRETTY_FUNCTION__, int(result));
    if (result == BT_NO_ERROR) {
        utility::Message msg(MSG_MASSTM_GAP_REQUEST_FINISH);
        ((MapMceInstanceStm &)instanceStm_).PostMessage(msg);
    } else {  // gap request security failure
        utility::Message msg(MSG_MASSTM_GAP_REQUEST_FAILED);
        ((MapMceInstanceStm &)instanceStm_).PostMessage(msg);
        LOG_ERROR("%{public}s gap failed", __PRETTY_FUNCTION__);
    }
}

// call by stm
int MapMceInstanceClient::StartClientDisConnect()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret;
    if (obexClientIns_ == nullptr) {
        LOG_ERROR("%{public}s obexClientIns_ null", __PRETTY_FUNCTION__);
        return BT_OPERATION_FAILED;
    }
    // obex callback will send msg Start Client DisConnect to mce stm
    ret = obexClientIns_->Disconnect();
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

// call by stm
int MapMceInstanceClient::ObexConnectFailedDisConnect()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret;
    if (obexClientIns_ == nullptr) {
        LOG_ERROR("%{public}s obexClientIns_ null", __PRETTY_FUNCTION__);
        return BT_OPERATION_FAILED;
    }
    // obex callback will send msg Start Client DisConnect to mce stm
    ret = obexClientIns_->Disconnect(false);
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

// call by stm
void MapMceInstanceClient::ProcessDisConnectFinish()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceSendRequestMutex_);

    BtAddr addr = ((MapMceInstanceStm &)instanceStm_).GetBtAddress();
    // unregister service security
    int ret = GAPIF_DeregisterServiceSecurity(&addr, &securityInfo_);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s error,error=%{public}d", __PRETTY_FUNCTION__, ret);
    }
    if (masClientConfig_.isGoepL2capPSM_) {
        BTM_RemoveRemoteL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, masClientConfig_.scn_, &addr);
    } else {
        BTM_RemoveRemoteRfcommScnChannelForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, masClientConfig_.scn_, &addr);
    }

    // clear current request
    currentRequestPtr_ = nullptr;

    resMasInstanceInfo_.isValid = false;
    resMasInstanceInfo_.masInstanceInfoUtf8 = "";
    resMasInstanceInfo_.ownerUciUtf8 = "";
    // delete all the left request
    ClientDeleteAllSavedRequestInternal();
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

// call by stm
MceRequestType MapMceInstanceClient::GetCurrentRequest()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceSendRequestMutex_);
    MceRequestType ret;
    if (currentRequestPtr_ != nullptr) {
        ret = currentRequestPtr_->GetRequestType();
    } else {
        ret = MCE_REQUEST_TYPE_IDLE;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

// call by stm
int MapMceInstanceClient::ClientSendRequest(std::unique_ptr<MapMceInstanceRequest> &req)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceSendRequestMutex_);

    int ret = req->SendRequest(*obexClientIns_);
    if (ret == BT_NO_ERROR) {
        currentRequestPtr_ = std::move(req);
        currentRequestPtr_->SaveReq();
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

// call by stm
void MapMceInstanceClient::ClientSaveRequest(std::unique_ptr<MapMceInstanceRequest> &req)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceSendRequestMutex_);

    req->SaveReq();
    masRequestQue_.push_back(std::move(req));
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

// call by device ctrl
int MapMceInstanceClient::ClientSendReqSetNotificationRegistration(bool value)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(stmMutex_);

    int ret = BT_NO_ERROR;
    if (obexClientIns_ == nullptr) {
        return BT_OPERATION_FAILED;
    }
    // api only valid in connected state, if not valid return error at now
    utility::Message outMsg(MSG_MASSTM_REQ_SEND_REQUEST_SELF);
    std::unique_ptr<MapMceInstanceRequest> reqPtr =
        std::make_unique<MapMceRequestSetNotificationRegistration>(value);
    ((MapMceInstanceStm &)instanceStm_).MceProcessMessageWithRequestInternal(outMsg, reqPtr);
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

// call by stm
int MapMceInstanceClient::ClientSendReqSetPath(
    const uint8_t flags, const std::u16string &paths, std::vector<std::u16string> &pathList)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret = BT_NO_ERROR;
    if (obexClientIns_ == nullptr) {
        return BT_OPERATION_FAILED;
    }
    // api only valid in connected state, if not valid return error at now
    utility::Message outMsg(MSG_MASSTM_REQ_SEND_REQUEST_SELF);
    std::unique_ptr<MapMceInstanceRequest> reqPtr = std::make_unique<MapMceRequestSetPath>(flags, paths, pathList);
    ((MapMceInstanceStm &)instanceStm_).MceProcessMessageWithRequestInternal(outMsg, reqPtr);

    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

// call by stm
int MapMceInstanceClient::ClientSendReqGetMasInstanceInformation()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    // api only valid in connected state, if not valid return error at now
    utility::Message outMsg(MSG_MASSTM_REQ_SEND_REQUEST_SELF);
    std::unique_ptr<MapMceInstanceRequest> reqPtr =
        std::make_unique<MapMceRequestGetMasInstanceInformation>(masSdpParamSave_.instanceId);
    ((MapMceInstanceStm &)instanceStm_).MceProcessMessageWithRequestInternal(outMsg, reqPtr);

    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return BT_NO_ERROR;
}

// call by stm
int MapMceInstanceClient::ClientProcResponseCommonProcess(const ObexHeader &resp)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceSendRequestMutex_);

    MapExecuteStatus resCode = MapExecuteStatus::NOT_SUPPORT;
    int ret = MCE_RESPONSE_FINISH;
    if (currentRequestPtr_ == nullptr) {
        LOG_ERROR("%{public}s lost the current request pointer", __PRETTY_FUNCTION__);
        return ret;
    }
    // process response
    MapRequestResponseAction retAction;
    ret = currentRequestPtr_->ProcessResponse(*this, resp, retAction);
    uint8_t obexCode = resp.GetFieldCode();
    LOG_INFO("%{public}s obexCode=%{public}d", __PRETTY_FUNCTION__, int(obexCode));
    if ((obexCode == uint8_t(ObexRspCode::SUCCESS)) || (obexCode == uint8_t(ObexRspCode::CONTINUE))) {
        if (ret == MCE_RESPONSE_FINISH_NG) {
            resCode = MapExecuteStatus::NOT_SUPPORT;
        } else if (ret == MCE_RESPONSE_CONTINUE_WITH_CALLBACK) {
            resCode = MapExecuteStatus::CONTINUE;
        } else {
            resCode = MapExecuteStatus::SUCCEED;
        }
    }
    if ((ret == MCE_RESPONSE_FINISH) || (ret == MCE_RESPONSE_FINISH_NG) ||
        (ret == MCE_RESPONSE_CONTINUE_WITH_CALLBACK)) {  // FINISH OR FINISH NOCALLBACK
        // execute callback to framework
        ExcuteCallbackToFramework(retAction, resCode);
    }
    if ((ret == MCE_RESPONSE_FINISH) || (ret == MCE_RESPONSE_FINISH_NG) ||
        (ret == MCE_RESPONSE_FINISH_NO_CALLBACK)) {  // FINISH OR FINISH NOCALLBACK
        // destry request
        currentRequestPtr_ = nullptr;
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

// call by device ctrl
int MapMceInstanceClient::ClientCountSendingRequest(MceRequestType requestType)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceSendRequestMutex_);

    int sum = 0;
    MapMceInstanceRequest *requestPtr = nullptr;
    // no request sending
    if (currentRequestPtr_ == nullptr) {
        return sum;
    }
    // count all
    if (requestType == MCE_REQUEST_TYPE_ALL) {
        // need add current
        sum = 1 + masRequestQue_.size();
        return sum;
    }
    // normal request
    if (currentRequestPtr_->GetRequestType() == requestType) {
        sum++;
    }
    for (auto it = masRequestQue_.begin(); it != masRequestQue_.end(); it++) {
        requestPtr = it->get();
        if (requestPtr != nullptr) {
            if (requestPtr->GetRequestType() == requestType) {
                sum++;
            }
        }
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return sum;
}

// call by stm
int MapMceInstanceClient::ClientSendSavedRequest()
{
    LOG_INFO("%{public}s enter,request.size=%{public}d", __PRETTY_FUNCTION__, int(masRequestQue_.size()));
    std::lock_guard<std::recursive_mutex> lock(mceSendRequestMutex_);

    int ret = BT_OPERATION_FAILED;
    currentRequestPtr_ = nullptr;
    std::unique_ptr<MapMceInstanceRequest> nextReq = nullptr;
    while (masRequestQue_.size() != 0) {
        // send request continue
        nextReq = std::move(masRequestQue_.front());
        masRequestQue_.pop_front();
        ret = nextReq->SendRequest(*obexClientIns_);
        if (ret == BT_NO_ERROR) {
            currentRequestPtr_ = std::move(nextReq);
            break;
        } else {
            currentRequestPtr_ = nullptr;
            nextReq = nullptr;
            LOG_ERROR("%{public}s obex send request error,ret=%{public}d", __PRETTY_FUNCTION__, ret);
        }
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

// call by stm
void MapMceInstanceClient::ClientDeleteAllSavedRequest()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(mceSendRequestMutex_);

    ClientDeleteAllSavedRequestInternal();
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceInstanceClient::ClientDeleteAllSavedRequestInternal()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    while (masRequestQue_.size() != 0) {
        masRequestQue_.pop_front();
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

// call by request, by stm
void MapMceInstanceClient::SaveMasInstanceInformation(const MasInstanceInformation &instInfo)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    resMasInstanceInfo_ = instInfo;
}

// call by device ctrl
MasInstanceInformation MapMceInstanceClient::GetMasInstanceInformation()
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(stmMutex_);

    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return resMasInstanceInfo_;
}

// internal
bool MapMceInstanceClient::CheckSupportedFeatures(int mask) const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    if (masSdpParamSave_.supportedFeatrue & mask) {
        return true;
    } else {
        return false;
    }
}

// internal
void MapMceInstanceClient::ExcuteCallbackToFramework(MapRequestResponseAction &retAction, MapExecuteStatus resCode)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    if ((retAction.action_ == MapActionType::GET_MESSAGE) ||
        (retAction.action_ == MapActionType::GET_UNREAD_MESSAGES)) {
        IProfileBMessage bmsg;
        bmsg.bMessageParam_ = retAction.bmessage_.GetBMessageData();
        bmsg.bMessageStringObject_ = retAction.bmessage_.GetBmessageObject();
        bmsg.FractionDeliver = retAction.bmessage_.GetFractionDeliver();
        observerMgrPtr_.ExcuteObserverOnBmessageCompleted(
            ((MapMceInstanceStm &)instanceStm_).GetBtDevice(), bmsg, resCode);
    } else if (retAction.action_ == MapActionType::GET_MESSAGES_LISTING) {
        IProfileMessagesListing msgListing;
        msgListing.messageOutlineList_ = retAction.messageList_.GetList();
        msgListing.messagesListingParam_ = retAction.messageList_.GetParam();
        msgListing.messagesListingStringObject_ = retAction.messageList_.GetStringObject();
        observerMgrPtr_.ExcuteObserverOnMessagesListingCompleted(
            ((MapMceInstanceStm &)instanceStm_).GetBtDevice(), msgListing, resCode);
    } else if (retAction.action_ == MapActionType::GET_CONVERSATION_LISTING) {
        IProfileConversationListing convListing;
        convListing.conversationOutlineList_ = retAction.conversationList_.GetList();
        convListing.conversationListingParam_ = retAction.conversationList_.GetParam();
        convListing.conversationListingStringObject_ = retAction.conversationList_.GetStringObject();
        observerMgrPtr_.ExcuteObserverOnConversationListingCompleted(
            ((MapMceInstanceStm &)instanceStm_).GetBtDevice(), convListing, resCode);
    } else {
        IProfileMapAction prifileAction;
        prifileAction.action_ = retAction.action_;
        prifileAction.ownerStatus_ = retAction.ownerStatus_;
        prifileAction.supportedFeatures_ = retAction.supportedFeatures;
        observerMgrPtr_.ExcuteObserverOnMapActionCompleted(
            ((MapMceInstanceStm &)instanceStm_).GetBtDevice(), prifileAction, resCode);
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

MapMceInstanceClient::MasObexClientObserver::~MasObexClientObserver()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

void MapMceInstanceClient::MasObexClientObserver::OnTransportFailed(ObexClient &client, int errCd)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    utility::Message msg = MSG_MASSTM_OBEX_TRANSPORT_FAILED;
    msg.arg1_ = errCd;
    // post msg to mns state machine instance. execute in mns service thread
    ((MapMceInstanceStm &)obexObserverInstStm_).PostMessage(msg);
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceInstanceClient::MasObexClientObserver::OnConnected(ObexClient &client, const ObexHeader &resp)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    utility::Message msg = MSG_MASSTM_OBEX_CONNECTED;
    // post msg to mns state machine instance. execute in mns service thread
    ((MapMceInstanceStm &)obexObserverInstStm_).PostMessage(msg);
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceInstanceClient::MasObexClientObserver::OnConnectFailed(ObexClient &client, const ObexHeader &resp)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    ObexHeader *tempPtr = const_cast<ObexHeader *>(&resp);
    utility::Message msg(MSG_MASSTM_OBEX_CONNECTED_FAILED);
    // save error code
    msg.arg1_ = int(tempPtr->GetFieldCode());
    // post msg to mns state machine instance. execute in mns service thread
    ((MapMceInstanceStm &)obexObserverInstStm_).PostMessage(msg);
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceInstanceClient::MasObexClientObserver::OnDisconnected(ObexClient &client)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    utility::Message msg = MSG_MASSTM_OBEX_DISCONNECTED;
    // post msg to mns state machine instance. execute in mns service thread
    ((MapMceInstanceStm &)obexObserverInstStm_).PostMessage(msg);
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceInstanceClient::MasObexClientObserver::OnActionCompleted(ObexClient &client, const ObexHeader &resp)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    // action finish
    utility::Message msg(MSG_MASSTM_RECEIVE_REQUEST_COMPLETED);
    ((MapMceInstanceStm &)obexObserverInstStm_).PostMessageWithObexHeader(msg, resp);
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void MapMceInstanceClient::MasObexClientObserver::OnBusy(ObexClient &client, bool isBusy)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    utility::Message msg = MSG_MASSTM_OBEX_INFO_CHG_CONTINUEBUSY_STATUS;
    msg.arg1_ = int(isBusy);
    ((MapMceInstanceStm &)obexObserverInstStm_).PostMessage(msg);
}
}  // namespace bluetooth
}  // namespace OHOS
