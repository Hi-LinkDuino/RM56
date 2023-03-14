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
 * @brief Defines map client service object.
 *
 */

/**
 * @file map_mce_service.h
 *
 * @brief map client service header file .
 *
 */

#ifndef MAP_MCE_SERVICE_H
#define MAP_MCE_SERVICE_H

#include <atomic>
#include <cstdint>
#include <list>
#include <map>
#include <mutex>
#include <string>
#include <vector>
#include "../obex/obex_mp_server.h"
#include "bt_def.h"
#include "context.h"
#include "interface_profile.h"
#include "interface_profile_map_mce.h"
#include "map_mce_instance_request.h"
#include "map_mce_observer_manager.h"
#include "map_mce_parameter.h"
#include "map_mce_types.h"
#include "message.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
// external class and c function declare
class MapMceMnsServer;
class MapMceDeviceCtrl;

enum MceServiceEnumType {
    MCE_MAX_OF_SENDING_REQUEST = 30,
    MCE_MAX_LENGTH_10 = 10
};

enum MapMceServiceStateType {
    MAP_MCE_STATE_STARTUP,
    MAP_MCE_STATE_STARTUPING,
    MAP_MCE_STATE_SHUTDOWN,
    MAP_MCE_STATE_SHUTDOWNING
};

enum MapMceServiceMessage {
    // service message
    MSG_MCESERVICE_START = 0x0000,  // don't modify it
    // service message
    MSG_MCESERVICE_DEVICE_CONNECTED = MSG_MCESERVICE_START,
    MSG_MCESERVICE_DEVICE_DISCONNECTED,
    MSG_MCESERVICE_DEVICE_CONNECT_FAILED,
    MSG_MCESERVICE_DEVICE_TRANSPORT_ERRO,
    MSG_MCESERVICE_GET_SURPORT_FEATURES_FINISH,
    MSG_MCESERVICE_GET_SURPORT_FEATURES_FAILED,
};
/**
 * @brief map mce service
 */
class MapMceService : public IProfileMapMce, public utility::Context {
public:
    /**
     * @brief  copy operator
     */
    BT_DISALLOW_COPY_AND_ASSIGN(MapMceService);
    /**
     * @brief  service name
     * @return const std::string
     */
    const std::string Name()
    {
        return PROFILE_NAME_MAP_MCE;
    }
    /**
     * @brief Construct a new Map Mce Service object
     */
    MapMceService();
    /**
     * @brief Destroy the Map Mce Service object
     */
    ~MapMceService();
    /**
     * @brief GetContext
     */
    utility::Context *GetContext() override;
    /**
     * @brief  register observer
     * @param  observer         function pointer
     */
    void RegisterObserver(IProfileMapMceObserver &observer) override;
    /**
     * @brief  deregister observer
     * @param  observer         function pointer
     */
    void DeregisterObserver(IProfileMapMceObserver &observer) override;
    /**
     * @brief check connect status
     * @param  device           Bluetooth Device address
     * @return true   success
     * @return false  failed
     */
    bool IsConnected(const RawAddress &device) override;
    /**
     * @brief Get the Devices By States object
     * @param  states           device state
     * @return std::vector<RawAddress>   Bluetooth Device list
     */
    std::vector<RawAddress> GetDevicesByStates(const std::vector<int> &statusList) override;
    /**
     * @brief Set the Connection Strategy object
     * @param  device           Bluetooth Device address
     * @param  strategy         strategy
     * @param  masInstanceId    Instance Id
     * @return int
     */
    int SetConnectionStrategy(const RawAddress &device, int strategy) override;
    /**
     * @brief Get the Connection Strategy object
     * @param  device           Bluetooth Device address
     * @return int
     */
    int GetConnectionStrategy(const RawAddress &device) const override;
    /**
     * @brief Get the Unread Messages object
     * @param  device           Bluetooth Device address
     * @param  type             message type
     * @param  maxListCount     iterm number
     * @return int
     */
    int GetUnreadMessages(const RawAddress &device, MapMessageType type, uint8_t max) override;
    /**
     * @brief Get the Supported Features object
     * @param  device           Bluetooth Device address
     * @return int
     */
    int GetSupportedFeatures(const RawAddress &device) override;
    /**
     * @brief  send mas request , send new messange
     * @param  device           Bluetooth Device address
     * @param  msg              message parameters
     * @return int
     */
    int SendMessage(const RawAddress &device, const IProfileSendMessageParameters &msg) override;
    /**
     * @brief Set the Notification Filter object
     * @param  device           Bluetooth Device address
     * @param  on
     * @return int
     */
    int SetNotificationFilter(const RawAddress &device, const int mask) override;
    /**
     * @brief Set the Notification on off
     * @param  device           Bluetooth Device address
     * @param  on
     * @return int
     */
    int SetNotificationRegistration(const RawAddress &device, const bool on) override;
    /**
     * @brief Get the Messages Listing object
     * @param  device           Bluetooth Device address
     * @param  para
     * @return int
     */
    int GetMessagesListing(const RawAddress &device, const IProfileGetMessagesListingParameters &para) override;
    /**
     * @brief Get the Message object
     * @param  device           Bluetooth Device address
     * @param  msgHandle
     * @param  para             message parameters
     * @return int
     */
    int GetMessage(const RawAddress &device, MapMessageType type, const std::u16string &msgHandle,
        const IProfileGetMessageParameters &para) override;
    /**
     * @brief  Get the Message object
     * @param  device           Bluetooth Device address
     * @param  para             message parameters
     * @param  cfg              config data
     * @return int
     */
    int GetMessageWithReqCfg(const RawAddress &device, MapMessageType type, const std::u16string &msgHandle,
        const IProfileGetMessageParameters &para, const MapMceRequestConfig &cfg);
    /**
     * @brief  update inbox
     * @param  device           Bluetooth Device address
     * @return int
     */
    int UpdateInbox(const RawAddress &device, MapMessageType type) override;
    /**
     * @brief Get the Conversation Listing object
     * @param  device           Bluetooth Device address
     * @param  para
     * @return int
     */
    int GetConversationListing(const RawAddress &device, const IProfileGetConversationListingParameters &para) override;
    /**
     * @brief Set the Message Status object
     * @param  device
     * @param  type
     * @param  msgStatus
     * @return int
     */
    int SetMessageStatus(const RawAddress &device, MapMessageType type,
        const IProfileSetMessageStatus &msgStatus) override;
    /**
     * @brief Set the Owner Status object
     * @param  device           Bluetooth Device address
     * @param  para
     * @return int
     */
    int SetOwnerStatus(const RawAddress &device, const IProfileSetOwnerStatusParameters &para) override;
    /**
     * @brief Get the Owner Status object
     * @param  device           Bluetooth Device address
     * @param  conversationId  conversation Id
     * @return int
     */
    int GetOwnerStatus(const RawAddress &device, const std::string &conversationId) override;
    /**
     * @brief  connect one bt device and all instance with map profile
     * @param  device
     * @return int
     */
    int Connect(const RawAddress &device) override;
    /**
     * @brief  disconnect one bt device and all instance in the device with map profile
     * @param  device           Bluetooth Device address
     * @return int
     */
    int Disconnect(const RawAddress &device) override;
    /**
     * @brief system request , user app do not use
     * @return int ProfileState mask type
     */
    int GetConnectState() override;
    /**
     * @brief Get the Connect State object
     * @return int FrameworkStatus type
     */
    int GetDeviceConnectState(const RawAddress &device) override;
    /**
     * @brief Get folder listing
     *
     * @param  device           bluetooth address
     * @param  maxOfListCount   max of list count
     * @param  startOffset      start offset
     * @return int
     */
    int GetFolderListing(const RawAddress &device, uint16_t maxOfListCount, uint16_t startOffset) override;
    /**
     * @brief set folder path
     *
     * @param  device           bluetooth address
     * @param  flags            flags control
     * @param  folder           folder name
     * @param  folderList       folder list
     * @return int
     */
    int SetPath(const RawAddress &device, const uint8_t flags, const std::u16string &folder,
        std::vector<std::u16string> &folderList) override;
    /**
     * @brief Get the Mas Instance Info object
     * @param  device          Bluetooth Device address
     * @return IProfileMasInstanceInfoList instance info list
     */
    IProfileMasInstanceInfoList GetMasInstanceInfo(const RawAddress &device) override;
    /**
     * @brief startup for adapter manager
     * @return int
     */
    void Enable() override;
    /**
     * @brief shutdown for adapter manager
     * @return int
     */
    void Disable() override;
    /**
     * @brief Get the Max Connect Num object
     * @return int
     */
    int GetMaxConnectNum() override;
    /**
     * @brief Get the Connect Devices object
     * @return std::list<RawAddress>
     */
    std::list<RawAddress> GetConnectDevices() override;
    /**
     * @brief  Connect single instance
     * @param  device          bluetooth device address
     * @param  instanceId       instance id
     * @return int
     */
    int ConnectInstance(const RawAddress &device, int instanceId);
    /**
     * @brief  check device status
     * @param  device           Bluetooth Device address
     * @return int
     */
    int CheckDeviceConnectStatus(const RawAddress &device);
    /**
     * @brief  check device status
     * @param  device           Bluetooth Device address
     * @return int
     */
    int CheckDeviceDisconnectStatus(const RawAddress &device);
    /**
     * @brief  Send Request To Connected Device
     * @param  device           Bluetooth Device address
     * @param  msg           request pointer
     * @return int
     */
    int SendRequestToConnectedDevice(
        const RawAddress &device, utility::Message msg, std::unique_ptr<MapMceInstanceRequest> &req);
    /**
     * @brief  Count Sending Request num
     * @param  device           Bluetooth Device address
     * @param  requestType      request type
     * @return int
     */
    int CountSendingRequest(const RawAddress &device, MceRequestType requestType);
    /**
     * @brief  connect one bt device and all instance in the device
     * @param  device           Bluetooth Device address
     * @return int
     */
    int ConnectInternal(const RawAddress &device, bool sInsMode, int sInsId);
    /**
     * @brief  start one instance of device to disconnecting
     * @param  device           Bluetooth Device address
     * @return int
     */
    int DisconnectInternal(const RawAddress &device);
    /**
     * @brief start up internal
     * @return int
     */
    int StartUpInternal();
    /**
     * @brief shutdown for adapter manager
     * @return int
     */
    int ShutDownInternal();
    /**
     * @brief  CheckSdp ForGetSupportedFeatures
     * @param  device           Bluetooth Device address
     * @return int
     */
    int CheckSdpForGetSupportedFeatures(const RawAddress &device);
    /**
     * @brief  post message to map mce thread for call back function send back message
     * @param  msg  message param
     */
    void PostMessage(utility::Message msg);
    /**
     * @brief Get the Service States object
     * @return MapMceServiceStateType
     */
    MapMceServiceStateType GetServiceStates();
    /**
     * @brief Set Default Config
     * @return void
     */
    void SetDefaultConfig(const MasInstanceConfig &configSave);
    /**
     * @brief  Mns Post Message
     * @param  msg              message
     * @param  btAddr           bluetooth address
     */
    void MnsPostMessage(const utility::Message &msg, std::string btAddr);
    /**
     * @brief  Mns Post Message With obex Header
     * @param  msg              message
     * @param  btAddr           bluetooth address
     */
    void MnsPostMessageWithHeader(const utility::Message &msg, std::string btAddr, const ObexHeader &req);

protected:
private:
    /**
     * @brief  shutdone internal common process
     * @return int
     */
    int ShutDownInternalCommon();
    /**
     * @brief process message
     * @param  msg  message param
     */
    void ProcessMessage(const utility::Message &msg);
    /**
     * @brief process message from mns server
     * @param  msg  message param
     */
    void MnsProcessMessage(const utility::Message msg, std::string btAddr);
    /**
     * @brief process message from mns server
     * @param  msg  message param
     */
    void MnsProcessMessageWithHeader(utility::Message msg, std::string btAddr, ObexHeader req);
    /**
     * @brief  sdp result dispatch th instance state machine
     * @param  msg  message param
     */
    void SdpDispatchToDeviceInstMgr(const utility::Message &msg);
    /**
     * @brief Process GetSupportedFeatures Respones
     * @param  msg
     */
    void ProcessGetSupportedFeaturesRespones(const utility::Message &msg);
    /**
     * @brief remove no use devcie and instance
     */
    void RemoveNoUseDeviceAndInstance();
    /**
     * @brief remove all Bluetooth Device
     */
    void RemoveAllDevice();
    /**
     * @brief Get the Config From Xml object
     */
    void GetConfigFromXml();
    /**
     * @brief  type change MapMessageType To IprofileMask int
     * @param  type     messange type
     * @return int
     */
    int MapMessageTypeToIprofileMask(MapMessageType type);
    // servcie thread dispatcher
    utility::Dispatcher *serviceDispatcher_ = nullptr;
    // service state
    std::atomic_int serviceState_ {MapMceServiceStateType::MAP_MCE_STATE_SHUTDOWN};
    // bt device instance magr's map
    std::map<const std::string, std::unique_ptr<MapMceDeviceCtrl>> serviceBtDeviceInstMgrMap_ {};
    // mns server pointer
    std::unique_ptr<MapMceMnsServer> mnsServer_ = nullptr;
    // notification Registration default value
    std::atomic_bool notificationRegistration_ {true};
    // default config
    MasInstanceConfig insDefaultConfig_ {};
    // service rpc callback manager
    MapMceObserverManager serviceRpcCallbackMgr_ {};
    // mutex
    std::recursive_mutex mceDeviceMapMutex_ {};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // MAP_MCE_SERVICE_H
