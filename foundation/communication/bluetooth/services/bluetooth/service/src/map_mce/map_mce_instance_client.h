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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines map client service instance client object.
 *
 */

/**
 * @file map_mce_instance_client.h
 *
 * @brief map client service instance client header file .
 *
 */

#ifndef MAP_MCE_INSTANCE_CLIENT_H
#define MAP_MCE_INSTANCE_CLIENT_H

#include <cstdint>
#include <list>
#include <mutex>
#include <vector>
#include "../obex/obex_headers.h"
#include "../obex/obex_mp_client.h"
#include "bt_def.h"
#include "gap_if.h"
#include "map_mce_instance_request.h"
#include "map_mce_service.h"
#include "raw_address.h"
#include "state_machine.h"

namespace OHOS {
namespace bluetooth {
// SDP Record for the Message Access Service on the MAP Device
static const uint16_t MAP_MCE_SERVICE_CLASS_UUID = 0x1132;
static const uint16_t MAP_MCE_GOEP_L2CAP_PSM_VALUE = 0x101F;
static const uint16_t MAP_MCE_SUPPORTED_MESSAGE_TYPE_ATTRIBUTE_ID = 0x0316;
// GoepL2capPsm (MAP v1.2 and later)
static const uint16_t MAP_MCE_GOEP_L2CAP_PSM_ATTRIBUTE_ID = 0x0200;
// MASInstanceID
static const uint16_t MAP_MCE_INSTANCE_ATTRIBUTE_ID = 0x0315;

// Map Supported Features
static const uint16_t MAP_MCE_SUPPORTED_FEATURES_ATTRIBUTE_ID = 0x0317;
static const uint32_t MAP_MCE_SUPPORTED_FEATURES_V14 = 0x007EFFFF;  // only bit 16 (pbap contact cross) is 0

/**
 * @brief mas mce sdp param
 */
struct MasSdpParam {
    uint8_t instanceId = 1;
    uint8_t messageType = 0;
    std::string serviceName = "";
    bool isGoepL2capPSM = false;    // default is rfcomm
    uint16_t scn = 0;               // rfcomm or l2cap
    uint32_t supportedFeatrue = 0;  // rfcomm
    GAP_Service gapNumber = MAP_CLIENT_ID_START;
};

/**
 * @brief sdp msg param
 */
struct MapSdpMsgArgPrt {
    BtAddr address {};
    std::vector<MasSdpParam> masSdpParamListPrt {};
};

/**
 * @brief Mce InstanceClient
 */
class MapMceInstanceClient {
public:
    /**
     * @brief  copy operator
     */
    BT_DISALLOW_COPY_AND_ASSIGN(MapMceInstanceClient);
    /**
     * @brief Construct a new Mce Instance Client object
     * @param  mapStm    the pointer of the instance state machine
     */
    MapMceInstanceClient(utility::StateMachine &mapStm, utility::Dispatcher &dispacher,
        MapMceObserverManager &observeMgr, const MasInstanceConfig &config, std::recursive_mutex &mutex);
    /**
     * @brief Destroy the Mce Instance Client object
     */
    ~MapMceInstanceClient();
    /**
     * @brief Set the Sdp Param
     * @param  sdpSave     sdp Param
     */
    void SetDefaultSdpParam(const MasSdpParam &sdpSave);
    /**
     * @brief Set the Default Config object
     * @param  configSave       save data
     */
    void SetDefaultConfig(MasInstanceConfig configSave);
    /**
     * @brief Get the Mas Sdp Param
     * @return MasSdpParam*
     */
    MasSdpParam GetMasSdpParam();
    /**
     * @brief  start the instance client connecting by obex client
     * @return int  success or failed
     */
    int StartClientConnect();
    /**
     * @brief  start the instance client disconnecting
     * @return int   success or failed
     */
    int StartClientDisConnect();
    /**
     * @brief  Execute ObexConnect
     * @return int
     */
    int ExcuteObexConnect();
    /**
     * @brief  Process DisConnect Finish
     * @return int   success or failed
     */
    void ProcessDisConnectFinish();
    /**
     * @brief Get the Current Request object
     * @return MceRequestType
     */
    MceRequestType GetCurrentRequest();
    /**
     * @brief  sending request
     * @param  req
     * @return int
     */
    int ClientSendRequest(std::unique_ptr<MapMceInstanceRequest> &req);
    /**
     * @brief  save request
     * @param  req
     * @return int
     */
    void ClientSaveRequest(std::unique_ptr<MapMceInstanceRequest> &req);

    /**
     * @brief  send set patch request
     * @param  flags            request path type
     * @param  paths            path string
     * @param  pathList         path string list
     * @return int
     */
    int ClientSendReqSetPath(const uint8_t flags, const std::u16string &paths, std::vector<std::u16string> &pathList);
    /**
     * @brief Send Request Get MasInstanceInformation
     * @return int
     */
    int ClientSendReqGetMasInstanceInformation();
    /**
     * @brief  Send Request SetNotificationRegistration
     * @param  value
     * @return int
     */
    int ClientSendReqSetNotificationRegistration(bool value);
    /**
     * @brief  Client Process ResponseCommonProcess
     * @param  resp
     * @return int
     */
    int ClientProcResponseCommonProcess(const ObexHeader &resp);
    /**
     * @brief  Client Count SendingRequest
     * @param  requestType
     * @return int
     */
    int ClientCountSendingRequest(MceRequestType requestType);
    /**
     * @brief  ClientSend SavedRequest
     * @return int
     */
    int ClientSendSavedRequest();
    /**
     * @brief Client Delete AllSavedRequest
     */
    void ClientDeleteAllSavedRequest();
    /**
     * @brief save Mas InstanceInformation
     * @param  instInfo
     */
    void SaveMasInstanceInformation(const MasInstanceInformation &instInfo);
    /**
     * @brief Get the Mas Instance Information object
     * @return MasInstanceInformation
     */
    MasInstanceInformation GetMasInstanceInformation();
    /**
     * @brief  On GapRequestSecurity callback
     * @param  result           stack error code
     */
    void OnGapRequestSecurityCb(uint16_t result);
    /**
     * @brief  Obex Connect Failed DisConnect
     * @param  result           stack error code
     */
    int ObexConnectFailedDisConnect();
    /**
     * @brief  Obex Connect Fail Connecting retry
     * @param  result           stack error code
     */
    int ReconnectAfterTransportFailed();
    /**
     * @brief Observer set to Obex
     */
    class MasObexClientObserver : public ObexClientObserver {
    public:
        /**
         * @brief  copy operator
         */
        explicit MasObexClientObserver(utility::StateMachine &stateMachine) : obexObserverInstStm_(stateMachine) {}
        /**
         * @brief Destroy the Mas Obex Client Observer object
         */
        ~MasObexClientObserver() override;
        /**
         * @brief  OnTransportFailed observer
         * @param  client
         * @param  errCd
         */
        void OnTransportFailed(ObexClient &client, int errCd) override;
        /**
         * @brief  OnConnected observer
         * @param  client
         * @param  resp
         */
        void OnConnected(ObexClient &client, const ObexHeader &resp) override;
        /**
         * @brief  OnConnectFailed observer
         * @param  client
         * @param  resp
         */
        void OnConnectFailed(ObexClient &client, const ObexHeader &resp) override;
        /**
         * @brief  OnDisconnected observer
         * @param  client
         */
        void OnDisconnected(ObexClient &client) override;
        /**
         * @brief  OnActionCompleted observer
         * @param  client
         * @param  resp
         */
        void OnActionCompleted(ObexClient &client, const ObexHeader &resp) override;
        /**
         * @brief  data sending continue status
         * @param  client
         * @param  isBusy
         */
        void OnBusy(ObexClient &client, bool isBusy) override;

    private:
        // mce instance stm
        utility::StateMachine &obexObserverInstStm_;
    };

private:
    /**
     * @brief  Check SupportedFeatures
     * @param  mask
     * @return true : ready
     * @return false : not ready
     */
    bool CheckSupportedFeatures(int mask) const;
    /**
     * @brief ClientDeleteAllSavedRequest Internal
     */
    void ClientDeleteAllSavedRequestInternal();
    /**
     * @brief Register Service Security
     * @return int
     */
    int RegisterServiceSecurity(GapServiceSecurityInfo &info);
    /**
     * @brief Register Service Security
     * @return int
     */
    int RequestSecurity(GapServiceSecurityInfo info);
    /**
     * @brief  Execute Callback To Framework api
     * @param  retAction
     * @param  resCode
     */
    void ExcuteCallbackToFramework(MapRequestResponseAction &retAction, MapExecuteStatus resCode);
    /**
     * @brief Set the Obex Config object
     */
    void SetObexConfig();
    /**
     * @brief Execute Obex Connect Internal
     * @return int
     */
    int ExcuteObexConnectInternal();
    // mce client uuid table
    static const uint8_t mceClientUuidTbl_[MAX_OF_MASCLIENT_OBEX_UUID_TBL];
    // instanceInfo
    MasInstanceInformation resMasInstanceInfo_ {};
    // mas instance statemachine pointer
    utility::StateMachine &instanceStm_;
    // mas obex observer
    std::unique_ptr<MasObexClientObserver> masObexObserver_ = nullptr;
    // send request
    std::unique_ptr<ObexMpClient> obexClientIns_ = nullptr;
    // new config
    ObexClientConfig masClientConfig_ {};
    // default config
    MasInstanceConfig insDefaultConfig_ {};
    // sdp param
    MasSdpParam masSdpParamSave_ {};
    // current request ptr
    std::unique_ptr<MapMceInstanceRequest> currentRequestPtr_ = nullptr;
    // request que
    std::list<std::unique_ptr<MapMceInstanceRequest>> masRequestQue_ {};
    // service dispatch
    utility::Dispatcher &mceDispacher_;
    // observer manager
    MapMceObserverManager &observerMgrPtr_;
    // sending mutex
    std::recursive_mutex mceSendRequestMutex_ {};
    // stm mutex
    std::recursive_mutex &stmMutex_;
    // security Info
    GapServiceSecurityInfo securityInfo_ {};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // MAP_MCE_INSTANCE_CLIENT_H
