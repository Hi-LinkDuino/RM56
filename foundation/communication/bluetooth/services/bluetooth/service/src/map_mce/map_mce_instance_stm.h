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
 * @brief Defines map client service instance state machine object.
 *
 */

/**
 * @file map_mce_instance_stm.h
 *
 * @brief map client service instance state machine header file .
 *
 */

#ifndef MAP_MCE_INSTANCE_STM_H
#define MAP_MCE_INSTANCE_STM_H

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include "../obex/obex_headers.h"
#include "btstack.h"
#include "dispatcher.h"
#include "map_mce_device_ctrl.h"
#include "map_mce_instance_client.h"
#include "map_mce_instance_request.h"
#include "map_mce_types.h"
#include "message.h"
#include "state_machine.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief MceInstanceState Type
 */
enum MceInstanceStateType {
    MCE_INSTANCE_STATE_CONNECTED,
    MCE_INSTANCE_STATE_CONNECTING,
    MCE_INSTANCE_STATE_DISCONNECTED,
    MCE_INSTANCE_STATE_DISCONNECTING
};
/**
 * @brief state machine message define
 */
enum MapMceStmMessage {
    // instance stm message
    MSG_MASSTM_START = 0x2000,
    // client stm interface api msg
    MSG_MASSTM_REQ_CONNECT,
    MSG_MASSTM_REQ_DISCONNECT,
    MSG_MASSTM_REQ_GET_FOLDER_LISTING,
    MSG_MASSTM_REQ_SET_NOTIFICATION_REGIST,
    MSG_MASSTM_REQ_SET_PATH,
    MSG_MASSTM_REQ_GET_INSTANCE,
    // send request
    MSG_MASSTM_REQ_SEND_REQUEST,
    MSG_MASSTM_REQ_SEND_REQUEST_SELF,
    // device msg
    MSG_MASSTM_GAP_REQUEST_FINISH = MSG_MASSTM_START + 0x100,
    MSG_MASSTM_GAP_REQUEST_FAILED,
    MSG_MASSTM_OBEX_CONNECTED,
    MSG_MASSTM_OBEX_CONNECTED_FAILED,
    MSG_MASSTM_OBEX_TRANSPORT_FAILED,
    MSG_MASSTM_OBEX_DISCONNECTED,
    MSG_MASSTM_OBEX_INFO_CHG_CONTINUEBUSY_STATUS,
    // normal request response
    MSG_MASSTM_RECEIVE_REQUEST_COMPLETED,
};

const std::string MCE_DISCONNECTED_STATE = "MceDisconnected";
const std::string MCE_CONNECTING_STATE = "MceConnecting";
const std::string MCE_CONNECTED_STATE = "MceConnected";
const std::string MCE_DISCONNECTING_STATE = "MceDisconnecting";
const std::string MCE_CONNECTED_STATE_S_REQSENDING = "MceConnectedReqSending";

class MapMceInstanceStm : public utility::StateMachine {
public:
    /**
     * @brief  copy operator
     */
    BT_DISALLOW_COPY_AND_ASSIGN(MapMceInstanceStm);
    /**
     * @brief Construct a new Map Mce Instance Stm object
     * @param  deviceMgr
     * @param  dispatcher
     * @param  instanceId
     */
    MapMceInstanceStm(
        MapMceDeviceCtrl &deviceMgr, utility::Dispatcher &dispatcher, int instanceId, const MasInstanceConfig &config);
    /**
     * @brief Destroy the Mce Mas Instance Stm object
     */
    ~MapMceInstanceStm();
    /**
     * @brief Create a Stm object
     */
    void CreateStm();
    /**
     * @brief  Post Message to map mce service task
     * @param  msg
     */
    void PostMessage(utility::Message msg);
    /**
     * @brief  message process
     * @param  msg
     */
    void MceProcessMessage(utility::Message msg);
    /**
     * @brief Get the Target State object
     * @return MceInstanceStateType
     */
    MceInstanceStateType GetTargetState();
    /**
     * @brief Get the current State object
     * @return MceInstanceStateType
     */
    std::string GetCurrentMceStmStateName();
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
     * @brief  Tans client TargetState
     * @param  state
     */
    void TansTargetState(MceInstanceStateType state);
    /**
     * @brief  Post Message to map mce service task
     * @param  msg message
     * @param  head obex header
     */
    void PostMessageWithObexHeader(utility::Message msg, ObexHeader head);
    /**
     * @brief  message process
     * @param  msg message
     * @param  head obex header
     */
    void MceProcessMessageWithObexHeader(utility::Message msg, ObexHeader head);
    /**
     * @brief  Post Message to map mce service task
     * @param  msg message
     * @param  req request
     * @return int
     */
    void PostMessageWithRequest(utility::Message msg, std::unique_ptr<MapMceInstanceRequest> &req);
    /**
     * @brief  process Message to map mce service task
     * @param  msg message
     * @param  req request
     * @return int
     */
    void MceProcessMessageWithRequest(utility::Message msg, std::unique_ptr<MapMceInstanceRequest> &req);
    /**
     * @brief  process Message to map mce service task
     * @param  msg message
     * @param  req request
     * @return int
     */
    void MceProcessMessageWithRequestInternal(utility::Message msg, std::unique_ptr<MapMceInstanceRequest> &req);
    /**
     * @brief GetBusyStatus
     * @return bool
     */
    bool GetBusyStatus();
    /**
     * @brief  Change Busy Status
     * @param  status
     */
    void ChangeStmBusyStatus(bool status);
    /**
     * @brief Get the Client object
     * @return MapMceInstanceClient&
     */
    MapMceInstanceClient &GetMasClient();

private:
    // mas client
    std::unique_ptr<MapMceInstanceClient> masClient_ = nullptr;
    // mce device mgr
    MapMceDeviceCtrl &mceDeviceMgr_;
    // obex header
    std::unique_ptr<ObexHeader> stmObexHeader_ = nullptr;
    // request ptr
    std::unique_ptr<MapMceInstanceRequest> stmRequestPtr_ = nullptr;
    // sending flag
    std::atomic_bool stmSendFlag {false};

    /**
     * @brief  Change Busy Status
     * @param  status
     */
    void ChangeContinueBusyStatus(bool status);
    // bluetooth device address ,string
    std::string btDevice_ = "";
    // bluetooth device address ,array
    BtAddr btAddress_ {};
    // instance id
    int masInstanceId_ = 0;
    // target state
    MceInstanceStateType targetState_ = MceInstanceStateType::MCE_INSTANCE_STATE_DISCONNECTED;
    // dispacher
    utility::Dispatcher &mceStmDispacher_;
    // default config
    MasInstanceConfig insDefaultConfig_ {};
    // stm mutex
    std::recursive_mutex mceInstanceStmMutex_ {};
    // stm Data busy Flag
    std::atomic_bool busyFlag_ {false};
    // stm ata Continue Flag
    std::atomic_bool stmDataContinueFlag_ {false};
    // stm Data busy Flag
    std::atomic_bool stmDataStmBusyFlag_ {false};
    /**
     * @brief map state machine state
     */
    class MapMceState : public utility::StateMachine::State {
    public:
        /**
         * @brief Construct a new Map Mce State object
         * @param  name
         * @param  stm
         * @param  parent
         */
        explicit MapMceState(const std::string &name, utility::StateMachine &stm) : State(name, stm){};
        /**
         * @brief Construct a new Map Mce State object
         * @param  name
         * @param  stm
         * @param  parent
         */
        explicit MapMceState(const std::string &name, utility::StateMachine &stm, State &parent)
            : State(name, stm, parent){};
        /**
         * @brief Destroy the Map Mce State object
         */
        virtual ~MapMceState() = default;
    };
    /**
     * @brief map state machine state
     */
    class MapMceDisconnectedState : public MapMceState {
    public:
        /**
         * @brief Construct a new Map Mce Disconnected State object
         * @param  stm
         */
        explicit MapMceDisconnectedState(utility::StateMachine &stm)
            : MapMceState(MCE_DISCONNECTED_STATE, stm), mceInstanceStm_((MapMceInstanceStm &)stm){};
        /**
         * @brief Destroy the Map Mce Disconnected State object
         */
        virtual ~MapMceDisconnectedState() = default;
        /**
         * @brief  state entry
         */
        virtual void Entry();
        /**
         * @brief  state exit
         */
        virtual void Exit();
        /**
         * @brief  dispatch
         * @param  msg
         * @return true
         * @return false
         */
        virtual bool Dispatch(const utility::Message &msg);

    private:
        // instance stm
        MapMceInstanceStm &mceInstanceStm_;
    };
    /**
     * @brief map state machine state
     */
    class MapMceConnectingState : public MapMceState {
    public:
        /**
         * @brief Construct a new Map Mce Connecting State object
         * @param  stm
         */
        explicit MapMceConnectingState(utility::StateMachine &stm)
            : MapMceState(MCE_CONNECTING_STATE, stm), mceInstanceStm_((MapMceInstanceStm &)stm){};
        /**
         * @brief Destroy the Map Mce Connecting State object
         */
        virtual ~MapMceConnectingState() = default;
        /**
         * @brief  state entry
         */
        virtual void Entry();
        /**
         * @brief  state exit
         */
        virtual void Exit();
        /**
         * @brief  msg dispatch
         * @param  msg
         * @return true
         * @return false
         */
        virtual bool Dispatch(const utility::Message &msg);

    private:
        /**
         * @brief Connected dispatch
         */
        void DispatchProcObexConnected();
        /**
         * @brief disconnected dispatch
         */
        void DispatchProcObexDisconnected();
        /**
         * @brief transport failed dispatch
         */
        void DispatchProcTransportFailed(const utility::Message &msg);
        /**
         * @brief connected failed dispatch
         */
        void DispatchProcObexConnectedFailed();
        /**
         * @brief connected failed dispatch
         */
        void DispatchProcConnectedFailed();
        // mce instance stm
        MapMceInstanceStm &mceInstanceStm_;
    };
    /**
     * @brief map state machine state
     */
    class MapMceConnectedState : public MapMceState {
    public:
        /**
         * @brief Construct a new Map Mce Connected State object
         * @param  stm
         */
        explicit MapMceConnectedState(utility::StateMachine &stm)
            : MapMceState(MCE_CONNECTED_STATE, stm), mceInstanceStm_((MapMceInstanceStm &)stm){};
        /**
         * @brief Destroy the Map Mce Connected State object
         */
        virtual ~MapMceConnectedState() = default;
        /**
         * @brief  state entry
         */
        virtual void Entry();
        /**
         * @brief  state exit
         */
        virtual void Exit();
        /**
         * @brief  msg dispatch
         * @param  msg
         * @return true
         * @return false
         */
        virtual bool Dispatch(const utility::Message &msg);

    private:
        // instance stm
        MapMceInstanceStm &mceInstanceStm_;
    };

    /**
     * @brief map state machine state
     */
    class MapMceConnectedStateSubReqSending : public MapMceState {
    public:
        /**
         * @brief Construct a new Map Mce Connected State Sub Req Sending object
         * @param  stm
         * @param  parent
         */
        MapMceConnectedStateSubReqSending(utility::StateMachine &stm, State &parent)
            : MapMceState(MCE_CONNECTED_STATE_S_REQSENDING, stm, parent), mceInstanceStm_((MapMceInstanceStm &)stm){};
        /**
         * @brief Destroy the Map Mce Connected State Sub Req Sending object
         */
        virtual ~MapMceConnectedStateSubReqSending() = default;
        /**
         * @brief state entry
         */
        virtual void Entry();
        /**
         * @brief  state exit
         */
        virtual void Exit();
        /**
         * @brief  state dispatch
         * @param  msg
         * @return true
         * @return false
         */
        virtual bool Dispatch(const utility::Message &msg);

    private:
        /**
         * @brief  process Receive Request Complite
         * @param  msg              My Param doc
         */
        void ProcReceiveRequestComplite(const utility::Message &msg);
        // mce instance stm
        MapMceInstanceStm &mceInstanceStm_;
    };
    /**
     * @brief map state machine state
     */
    class MapMceDisconnectingState : public MapMceState {
    public:
        /**
         * @brief Construct a new Map Mce Disconnecting State object
         * @param  stm
         */
        explicit MapMceDisconnectingState(utility::StateMachine &stm)
            : MapMceState(MCE_DISCONNECTING_STATE, stm), mceInstanceStm_((MapMceInstanceStm &)stm){};
        /**
         * @brief Destroy the Map Mce Disconnecting State object
         */
        virtual ~MapMceDisconnectingState() = default;
        /**
         * @brief  state entry
         */
        virtual void Entry();
        /**
         * @brief  state exit
         */
        virtual void Exit();
        /**
         * @brief  msg dispatch
         */
        virtual bool Dispatch(const utility::Message &msg);

    private:
        // instance stm
        MapMceInstanceStm &mceInstanceStm_;
    };
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // MAP_MCE_INSTANCE_STM_H
