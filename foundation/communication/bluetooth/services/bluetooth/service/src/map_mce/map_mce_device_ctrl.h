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
 * @brief Defines map client service device control.
 *
 */

/**
 * @file map_mce_device_ctrl.h
 *
 * @brief map client service device control.
 *
 */

#ifndef MAP_MCE_DEVICE_CTRL_H
#define MAP_MCE_DEVICE_CTRL_H

#include <atomic>
#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <vector>
#include "../obex/obex_headers.h"
#include "../obex/obex_session.h"
#include "bt_def.h"
#include "map_mce_instance_request.h"
#include "map_mce_service.h"
#include "map_mce_types.h"
#include "message.h"
#include "state_machine.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief map mce device state type
 */
enum MapMceDeviceStateType {
    MAP_MCE_DEV_STATE_CONNECTED = int(BTConnectState::CONNECTED),
    MAP_MCE_DEV_STATE_CONNECTING = int(BTConnectState::CONNECTING),
    MAP_MCE_DEV_STATE_DISCONNECTED = int(BTConnectState::DISCONNECTED),
    MAP_MCE_DEV_STATE_DISCONNECTING = int(BTConnectState::DISCONNECTING)
};
/**
 * @brief map mce device busy state type
 */
enum MapMcePowerBusyState {
    MAP_MCE_PWBUSY_STATUS_DISCONNECT = 0,
    MAP_MCE_PWBUSY_STATUS_CONNECT = 1,
    MAP_MCE_PWBUSY_STATUS_LOW = 2,
    MAP_MCE_PWBUSY_STATUS_HIGH = 3
};
/**
 * @brief map mce device state type
 */
enum MapMceDeviceMessage {
    MSG_MCEDEVICE_START = 0x1000,  // don't modify it
    // device msg
    MSG_MCEDEVICE_SDP_GET_INSTANCE_FINISH,
    MSG_MCEDEVICE_SDP_GET_INSTANCE_FAILED,
    // MAS client msg
    MSG_MCEDEVICE_INSCLIENT_DISCONNECTED,
    MSG_MCEDEVICE_INSCLIENT_CONNECTED,
    MSG_MCEDEVICE_INSCLIENT_CONNECT_FAILED,
    MSG_MCEDEVICE_INSCLIENT_TRANSPORT_ERRO,
    MSG_MCEDEVICE_REQ_DEVICE_DISCONNECT,
    MSG_MCEDEVICE_REQ_DEVICE_CONNECT,
    MSG_MCEDEVICE_REQ_SEND_REQUEST,
    MSG_MCEDEVICE_INSCLIENT_INFO_BUSY_STATUS_CHANGE,
    // MNS server msg
    MSG_MCEDEVICE_MNS_INFO_TRANSPORT_INCOMING,
    MSG_MCEDEVICE_MNS_INFO_CONNECT,
    MSG_MCEDEVICE_MNS_INFO_DISCONNECT,
    MSG_MCEDEVICE_MNS_INFO_TRANSPORT_ERROR,
    MSG_MCEDEVICE_MNS_INFO_TRANSPORT_DISCONNECTED,
    MSG_MCEDEVICE_MNS_INFO_PUT,
    MSG_MCEDEVICE_MNS_DEVICE_DISCONNECTED_FINISH,
    MSG_MCEDEVICE_MNS_INFO_BUSY_STATUS_CHANGE
};
/**
 * @brief mce device client control, manager the isntances list.
 */
class MapMceDeviceCtrl {
public:
    /**
     * @brief  copy operator
     */
    BT_DISALLOW_COPY_AND_ASSIGN(MapMceDeviceCtrl);
    /**
     * @brief Construct a new Mce Device Instance Mgr object
     * @param  device
     * @param  service
     * @param  rpcId
     */
    MapMceDeviceCtrl(const std::string &device, MapMceService &service, bool notifySet, MasInstanceConfig config,
        MapMceObserverManager &callback);
    /**
     * @brief Destroy the Mce Device Instance Mgr object
     */
    ~MapMceDeviceCtrl();
    /**
     * @brief post message to service thread device module
     * @param  msg
     */
    void PostMessage(utility::Message msg);
    /**
     * @brief  Post Message With Request
     * @param  msg              message
     * @param  req              request
     * @return int
     */
    int PostMessageWithRequest(utility::Message msg, std::unique_ptr<MapMceInstanceRequest> &req);
    /**
     * @brief Get the Current Device State object
     * @return MapMceDeviceStateType
     */
    MapMceDeviceStateType GetCurrentDeviceState();
    /**
     * @brief Get the Current Device State object
     * @return MapMceDeviceStateType
     */
    MapMceDeviceStateType GetTargetDeviceState() const;
    /**
     * @brief
     * @param  deviceState
     * @return int
     */
    static int DeviceStateConvertToProfileState(MapMceDeviceStateType deviceState);
    /**
     * @brief process message
     * @param  msg
     */
    void ProcessMessage(utility::Message msg);
    /**
     * @brief process MnsObexObserver message
     * @param  msg
     */
    void ProcessMnsObexObserverMessage(const ObexHeader &req, utility::Message msg);
    /**
     * @brief Get the Bt Device object
     * @return std::string
     */
    std::string GetBtDevice();
    /**
     * @brief Get the Bt Address object
     * @return BtAddr
     */
    BtAddr GetBtAddress();
    /**
     * @brief Get the Device Rpc Callback Mgr object
     * @return MapMceObserverManager*
     */
    MapMceObserverManager &GetDeviceCallbackMgr() const;
    /**
     * @brief Get the Device Instance Stm Map size
     * @return map size
     */
    int GetDeviceInstanseStmMapSize();
    /**
     * @brief Get MasInstanceInfo
     * @param  device           bluetooth device address
     * @return std::vector<IProfileMasInstanceInfo>
     */
    std::vector<IProfileMasInstanceInfo> GetMasInstanceInfo(const RawAddress &device);
    /**
     * @brief Get the Mns State object
     * @return MapMceDeviceStateType
     */
    MapMceDeviceStateType GetMnsState() const;
    /**
     * @brief Count Sending Request
     * @param  requestType           request Type
     * @return int
     */
    int CountSendingRequest(MceRequestType requestType);
    /**
     * @brief Se tDeviceCtl Config
     * @param  config           config value
     */
    void SetDeviceCtlConfig(const MasInstanceConfig &config);

private:
    /**
     * @brief trans state machine
     * @param  state
     */
    void TransClientState(MapMceDeviceStateType state);
    /**
     * @brief state connected process massage
     * @param  msg
     */
    void ClientConnectedProcMsg(utility::Message msg);
    /**
     * @brief state connecting process massage
     * @param  msg
     */
    void ClientConnectingProcMsg(utility::Message msg);
    /**
     * @brief state disconnected process massage
     * @param  msg
     */
    void ClientDisconnectedProcMsg(utility::Message msg);
    /**
     * @brief state disconnecting process massage
     * @param  msg
     */
    void ClientDisConnectingProcMsg(utility::Message msg);
    /**
     * @brief start connecting
     * @return int
     */
    int StartConnecting();
    /**
     * @brief All Instances start connecting
     */
    void AllInstancesStartConnecting();
    /**
     * @brief Get the Supported Mas Instances object
     * @param  device
     * @return int
     */
    static int GetSupportedMasInstances(const RawAddress &device);
    /**
     * @brief process the sdp coming infomachine
     * @param  msg
     */
    int ProcessGetInstance(const utility::Message &msg);
    /**
     * @brief All Instances start disconnecting
     * @return true
     * @return false
     */
    bool AllInstancesStartDisConnecting();
    /**
     * @brief process obex coming connect info
     */
    void ProcessConnected();
    /**
     * @brief rocess obex coming disconnect info
     */
    void ProcessDisconnected();
    /**
     * @brief  send request
     * @param  req              req data
     */
    void SendRequest(utility::Message msg);
    /**
     * @brief  MessageState msg Process
     * @param  msg
     */
    void ProcessMessageStateProcess(utility::Message msg);
    /**
     * @brief  connected process message
     * @param  msg
     */
    void MnsConnectedProcMsg(utility::Message msg);
    /**
     * @brief  connected process message
     * @param  msg
     */
    void MnsConnectingProcMsg(utility::Message msg);
    /**
     * @brief  disconnected process message
     * @param  msg
     */
    void MnsDisconnectedProcMsg(utility::Message msg);
    /**
     * @brief  disconnecting process message
     * @param  msg
     */
    void MnsDisconnectingProcMsg(utility::Message msg);
    /**
     * @brief  trans mns state
     * @param  state
     */
    void TransMnsState(MapMceDeviceStateType state);
    /**
     * @brief  process obex put
     * @param  msg
     */
    void MnsProcObexPut(utility::Message msg);
    /**
     * @brief  mns process accept
     * @param  msg
     */
    void MnsProcConnectAccept(utility::Message msg);
    /**
     * @brief  mns process incoming accept
     * @param  msg
     */
    void MnsProcIncomingAccept(utility::Message msg);
    /**
     * @brief disconnected massage process
     */
    void MnsProcDisconnected();
    /**
     * @brief  Mns server Process Connect Refuse
     * @param  msg
     */
    void MnsProcConnectRefuse(utility::Message msg);
    /**
     * @brief  Mns server Process Disconnect Accept
     * @param  msg
     * @return int
     */
    int MnsProcDisconnectAccept(utility::Message msg);
    /**
     * @brief Mns Process Request Disconnect
     */
    void MnsProcRequestDisconnect();
    /**
     * @brief  Mns save ObexHeader when receive observer
     * @param  req
     */
    void MnsChangeSessionObexHeader(const ObexHeader &req);
    /**
     * @brief  Process Error Observer
     * @param  request
     */
    void ProcessErrorObserver(MapMceInstanceRequest &request);
    /**
     * @brief  Change Request Type To Iprofile Type
     * @param  type
     * @return MapActionType
     */
    static MapActionType ChangeRequestTypeToIprofileType(MceRequestType type);
    /**
     * @brief  Mns server Process Check TargetId when Connect Accept
     * @return int
     */
    int MnsProcConnectAcceptCheckTargetId();
    /**
     * @brief Check PowerBusyState Change
     * @param  state
     */
    void CheckPowerBusyStateChange();
    /**
     * @brief trans Power Busy State
     * @param  state
     */
    void TransPowerBusyState(MapMcePowerBusyState state);
    // uuid
    static const uint8_t mnsTargetUuidTbl_[MAX_OF_MASCLIENT_OBEX_UUID_TBL];
    // device address string type
    std::string btDevice_ = "";
    // device state
    std::atomic_int btDeviceState_ {MapMceDeviceStateType::MAP_MCE_DEV_STATE_DISCONNECTED};
    // target device state
    std::atomic_int btDeviceTargetState_ {MapMceDeviceStateType::MAP_MCE_DEV_STATE_DISCONNECTED};
    // service pointer
    MapMceService &devService_;
    // mce mns server status
    std::atomic_int mnsServerStatus_ {MapMceDeviceStateType::MAP_MCE_DEV_STATE_DISCONNECTED};
    // mce mns server session
    ObexServerSession *mnsObexSession_ = nullptr;
    // mce mns incoming
    ObexIncomingConnect *incomingPtr = nullptr;
    // mce mns obex header
    std::unique_ptr<ObexHeader> mnsSessionObexheader_ = nullptr;
    // notification Registration default value
    bool notificationRegistration_ = false;
    // device supported Features_
    uint32_t supportedFeatures_ = 0;
    // default config
    MasInstanceConfig insDefaultConfig_ {};
    // connect Id
    int connectId_ = 0;
    // instance id map
    std::map<const int, std::unique_ptr<utility::StateMachine>> deviceInstanceStmMap_ {};
    // call back mgr pointer
    MapMceObserverManager &deviceRpcCallbackMgr_;
    // mutex
    std::recursive_mutex mceInstanceMapMutex_ {};
    // request ptr
    std::unique_ptr<MapMceInstanceRequest> ctrlRequestPtr_ = nullptr;
    // send flag
    std::atomic_bool ctrlSendFlag_ {false};
    // busy flag
    MapMcePowerBusyState ctrlBusyStatus_ = MAP_MCE_PWBUSY_STATUS_DISCONNECT;
    // mns Continue Flag
    std::atomic_bool mnsContinueBusyFlag_ {false};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // MAP_MCE_DEVICE_CTRL_H
