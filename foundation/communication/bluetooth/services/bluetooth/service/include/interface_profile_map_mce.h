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
 * @brief Defines map client service interface, including observer and api functions.
 *
 * @since 6
 */

/**
 * @file interface_profile_map_mce.h
 *
 * @brief map client interface.
 *
 * @since 6
 */

#ifndef INTERFACE_PROFILE_MAP_MCE_H
#define INTERFACE_PROFILE_MAP_MCE_H

#include "interface_profile.h"
#include "map_mce_parameter.h"
#include <vector>

namespace OHOS {
namespace bluetooth {
/**
 * @brief map mce observer for framework api
 *
 * @since 6
 */
class IProfileMapMceObserver {
public:
    /**
     * @brief Destroy the IProfileMapMceObserver object
     *
     * @since 6
     */
    virtual ~IProfileMapMceObserver() {};
    /**
     * @brief  action completed observer
     *
     * @param  deviceAddress     bluetooth address
     * @param  action
     * @param  status
     * @since 6
     */
    virtual void OnMapActionCompleted(
        const RawAddress &deviceAddress, const IProfileMapAction &action, MapExecuteStatus status) {};
    /**
     * @brief  event report observer
     *
     * @param  deviceAddress     bluetooth address
     * @param  report
     * @since 6
     */
    virtual void OnMapEventReported(const RawAddress &deviceAddress, const IProfileMapEventReport &report) {};
    /**
     * @brief  ConnectionState Changed
     *
     * @param  deviceAddress     bluetooth address
     * @param  state            changed status
     * @since 6
     */
    virtual void OnConnectionStateChanged(const RawAddress &deviceAddress, int state) {};
    /**
     * @brief  Bmessage completed observer
     *
     * @param  deviceAddress     bluetooth address
     * @param  bmsg
     * @param  status
     * @since 6
     */
    virtual void OnBmessageCompleted(
        const RawAddress &deviceAddress, const IProfileBMessage &bmsg, MapExecuteStatus status) {};
    /**
     * @brief  MessagesListing completed observer
     *
     * @param  deviceAddress     bluetooth address
     * @param  bmsg
     * @param  status
     * @since 6
     */
    virtual void OnMessagesListingCompleted(
        const RawAddress &deviceAddress, const IProfileMessagesListing &listing, MapExecuteStatus status) {};
    /**
     * @brief  ConversationListing completed observer
     *
     * @param  deviceAddress     bluetooth address
     * @param  listing
     * @param  status
     * @since 6
     */
    virtual void OnConversationListingCompleted(
        const RawAddress &deviceAddress, const IProfileConversationListing &listing, MapExecuteStatus status) {};
};

class IProfileMapMce : public IProfile {
public:
    /**
     * @brief  register observer
     *
     * @param  observer         function pointer
     * @since 6
     */
    virtual void RegisterObserver(IProfileMapMceObserver &observer) = 0;
    /**
     * @brief  deregister observer
     *
     * @param  observer         function pointer
     * @since 6
     */
    virtual void DeregisterObserver(IProfileMapMceObserver &observer) = 0;
    /**
     * @brief check connect status
     *
     * @param  device           Bluetooth Device address
     * @return true   success
     * @return false  failed
     * @since 6
     */
    virtual bool IsConnected(const RawAddress &device) = 0;
    /**
     * @brief Get the Devices By States object
     *
     * @param  states           device state
     * @return std::vector<RawAddress>   Bluetooth Device list
     * @since 6
     */
    virtual std::vector<RawAddress> GetDevicesByStates(const std::vector<int> &statusList) = 0;
    /**
     * @brief Set the Connection Strategy object
     *
     * @param  device           Bluetooth Device address
     * @param  strategy         strategy
     * @return int
     * @since 6
     */
    virtual int SetConnectionStrategy(const RawAddress &device, int strategy) = 0;
    /**
     * @brief Get the Connection Strategy object
     *
     * @param  device           Bluetooth Device address
     * @return int
     * @since 6
     */
    virtual int GetConnectionStrategy(const RawAddress &device) const = 0;
    /**
     * @brief Get the Unread Messages object
     * @param  device           Bluetooth Device address
     * @param  type             message type
     * @param  maxListCount     iterm number
     * @return int
     * @since 6
     */
    virtual int GetUnreadMessages(const RawAddress &device, MapMessageType type, uint8_t max) = 0;
    /**
     * @brief Get the Supported Features object request
     *
     * @param  device           Bluetooth Device address
     * @return int
     * @since 6
     */
    virtual int GetSupportedFeatures(const RawAddress &device) = 0;
    /**
     * @brief  send new messange request
     *
     * @param  device           Bluetooth Device address
     * @param  msg
     * @return int
     * @since 6
     */
    virtual int SendMessage(const RawAddress &device, const IProfileSendMessageParameters &msg) = 0;
    /**
     * @brief Set the Notification Filter object request
     *
     * @param  device           Bluetooth Device address
     * @param  on
     * @return int
     * @since 6
     */
    virtual int SetNotificationFilter(const RawAddress &device, const int mask) = 0;
    /**
     * @brief Set the Notification Filter object request
     *
     * @param  device           Bluetooth Device address
     * @param  on
     * @return int
     * @since 6
     */
    virtual int SetNotificationRegistration(const RawAddress &device, const bool on) = 0;
    /**
     * @brief Get the Messages Listing object request
     *
     * @param  device           Bluetooth Device address
     * @param  para
     * @return int
     * @since 6
     */
    virtual int GetMessagesListing(const RawAddress &device, const IProfileGetMessagesListingParameters &para) = 0;
    /**
     * @brief Get the Message object request
     *
     * @param  device           Bluetooth Device address
     * @param  msgHandle
     * @param  para
     * @return int
     * @since 6
     */
    virtual int GetMessage(const RawAddress &device, MapMessageType type, const std::u16string &msgHandle,
        const IProfileGetMessageParameters &para) = 0;
    /**
     * @brief  update inbox request
     *
     * @param  device           Bluetooth Device address
     * @return int
     * @since 6
     */
    virtual int UpdateInbox(const RawAddress &device, MapMessageType type) = 0;
    /**
     * @brief Get the Conversation Listing object
     *
     * @param  device           Bluetooth Device address
     * @param  para
     * @return int
     * @since 6
     */
    virtual int GetConversationListing(
        const RawAddress &device, const IProfileGetConversationListingParameters &para) = 0;
    /**
     * @brief Set the Message Status object
     * @param  device
     * @param  type
     * @param  msgStatus
     * @return int
     */
    virtual int SetMessageStatus(const RawAddress &device, MapMessageType type,
        const IProfileSetMessageStatus &msgStatus) = 0;
    /**
     * @brief Set the Owner Status object
     *
     * @param  device           Bluetooth Device address
     * @param  para
     * @return int
     * @since 6
     */
    virtual int SetOwnerStatus(const RawAddress &device, const IProfileSetOwnerStatusParameters &para) = 0;
    /**
     * @brief Get the Owner Status object
     *
     * @param  device           Bluetooth Device address
     * @param  conversationId
     * @return int
     * @since 6
     */
    virtual int GetOwnerStatus(const RawAddress &device, const std::string &conversationId) = 0;
    /**
     * @brief Get the Connect State object
     *
     * @return int
     * @since 6
     */
    virtual int GetDeviceConnectState(const RawAddress &device) = 0;
    /**
     * @brief Get the Mas Instance Info object
     *
     * @param  device           bluetooth address
     * @return int
     * @since 6
     */
    virtual IProfileMasInstanceInfoList GetMasInstanceInfo(const RawAddress &device) = 0;
    /**
     * @brief Get folder listing
     *
     * @param  device           bluetooth address
     * @param  maxOfListCount   max of list count
     * @param  startOffset      start offset
     * @return int
     * @since 6
     */
    virtual int GetFolderListing(const RawAddress &device, uint16_t maxOfListCount, uint16_t startOffset) = 0;
    /**
     * @brief set folder path
     *
     * @param  device           bluetooth address
     * @param  flags            flags control
     *                              0x02: Go down 1 level
     *                              0x02: Go back to root , if choice this , don't input the folder and the folder list
     *                              0x03: Go up 1 level
     *                              0x04: path name, folder name list valid
     * @param  folder           folder name, only valid in flags 0x02 or 0x03
     * @param  folderList       folder list, only valid in flags 0x04
     * @return int
     * @since 6
     */
    virtual int SetPath(const RawAddress &device, const uint8_t flags, const std::u16string &folder,
        std::vector<std::u16string> &folderList) = 0;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // INTERFACE_PROFILE_MAP_MCE_H
