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

#ifndef AVRCP_TG_PROFILE_H
#define AVRCP_TG_PROFILE_H

#include <deque>
#include "avrcp_media.h"
#include "avrcp_tg_internal.h"
#include "avrcp_tg_state_machine.h"
#include "base_def.h"
#include "dispatcher.h"
#include "packet.h"
#include "raw_address.h"

/**
 * @brief The bluetooth subsystem.
 */
namespace OHOS {
namespace bluetooth {
/**
 * @brief This class provides a set of methods related to the role of the target described in the Audio/Video Remote
 * Control profile.
 */
class AvrcTgProfile {
public:
    /**
     * @brief This struct provides a set of pointers for informing the state change.
     */
    struct Observer {
    public:
        /**
         * @brief Informs that the AVRCP TG profile is disabled.
         */
        std::function<void()> onDisabled;

        /**
         * @brief Informs that the connection state is changed.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] state   The connection state. Refer to <b>BTConnectState</b>.
         */
        std::function<void(const RawAddress &rawAddr, int state)> onConnectionStateChanged;

        /**
         * @brief Informs that want to find the AVRCP CT record of the specified device.
         *
         * @param[in] rawAddr The address of the peer bluetooth device.
         */
        std::function<void(const RawAddress &rawAddr)> findCtService;

        /**
         * @brief Informs that the button is pressed.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] button  The value of the key operation.
         * @param[in] label   The label which is used to distinguish different call.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t button, uint8_t label)> onButtonPressed;

        /**
         * @brief Informs that the button is released.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] button  The value of the key operation.
         * @param[in] label   The label which is used to distinguish different call.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t button, uint8_t label)> onButtonReleased;

        /**
         * @brief Informs that the button is held.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] button  The value of the key operation.
         * @param[in] label   The label which is used to distinguish different call.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t button, uint8_t label)> onButtonHeld;

        /**
         * @brief Responds the command of the <b>GetCapabilities</b>.
         *
         * @param[in] rawAddr      The address of the bluetooth device.
         * @param[in] label        The label which is used to distinguish different call.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t label)> getCapabilities;

        /**
         * @brief Responds the command of the <b>ListPlayerApplicationSettingAttributes</b>.
         *
         * @param[in] rawAddr   The address of the bluetooth device.
         * @param[in] label     The label which is used to distinguish different call.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t label)> listPlayerApplicationSettingAttributes;

        /**
         * @brief Responds the command of the <b>ListPlayerApplicationSettingValues</b>.
         *
         * @param[in] rawAddr   The address of the bluetooth device.
         * @param[in] attribute The attribute of the player application setting. Refer to <b>AvrcPlayerAttribute</b>.
         * @param[in] label     The label which is used to distinguish different call.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t attribute, uint8_t label)>
            listPlayerApplicationSettingValues;

        /**
         * @brief Responds the command of the <b>ListPlayerApplicationSettingValues</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] attributes The attribute of the player application settings. Refer to <b>AvrcPlayerAttribute</b>.
         * @param[in] label      The label which is used to distinguish different call.
         * @param[in] context    The context which is used to distinguish different purpose.
         */
        std::function<void(
            const RawAddress &rawAddr, const std::deque<uint8_t> &attributes, uint8_t label, uint8_t context)>
            getCurrentPlayerApplicationSettingValue;

        /**
         * @brief Responds the command of the <b>SetPlayerApplicationSettingValue</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] attributes The attribute of the player application settings. Refer to <b>AvrcPlayerAttribute</b>.
         * @param[in] values     The value of the player application setting attributes.
         * @param[in] label      The label which is used to distinguish different call.
         */
        std::function<void(const RawAddress &rawAddr, const std::deque<uint8_t> &attributes,
            const std::deque<uint8_t> &values, uint8_t label)>
            setPlayerApplicationSettingValue;

        /**
         * @brief Responds the command of the <b>GetPlayerApplicationSettingAttributeText</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] attributes The attribute of the player application settings.
         * @param[in] label      The label which is used to distinguish different call.
         */
        std::function<void(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes, uint8_t label)>
            getPlayerApplicationSettingAttributeText;

        /**
         * @brief Responds the command of the <b>GetPlayerApplicationSettingValueText</b>.
         *
         * @param[in] rawAddr     The address of the bluetooth device.
         * @param[in] attributeId Player application setting attribute ID.
         * @param[in] values      Player application setting value ID.
         * @param[in] label       The label which is used to distinguish different call.
         */
        std::function<void(
            const RawAddress &rawAddr, uint8_t attributeId, const std::vector<uint8_t> &values, uint8_t label)>
            getPlayerApplicationSettingValueText;

        /**
         * @brief Responds the command of the <b>GetElementAttribtues</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] identifier Unique identifier to identify an element on TG.
         * @param[in] attributes  Specifies the attribute ID for the attributes to be retrieved.
         * @param[in] label      The label which is used to distinguish different call.
         */
        std::function<void(
            const RawAddress &rawAddr, uint64_t identifier, const std::vector<uint32_t> &attributes, uint8_t label)>
            getElementAttributes;

        /**
         * @brief Responds the command of the <b>GetPlayStatus</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] label   The label which is used to distinguish different call.
         * @param[in] context The context which is used to distinguish different purpose.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t label, uint8_t context)> getPlayStatus;

        /**
         * @brief Responds the command of the <b>SetAddressedPlayer</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] playerId   The unique media player id.
         * @param[in] uidCounter The value of the uid counter.
         * @param[in] label      The label which is used to distinguish different call.
         */
        std::function<void(const RawAddress &rawAddr, uint16_t playerId, uint16_t uidCounter, uint8_t label)>
            setAddressedPlayer;

        /**
         * @brief Responds the command of the <b>SetBrowsedPlayer</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] playerId   The unique media player id.
         * @param[in] uidCounter The value of the uid counter.
         * @param[in] label      The label which is used to distinguish different call.
         */
        std::function<void(const RawAddress &rawAddr, uint16_t playerId, uint16_t uidCounter, uint8_t label)>
            setBrowsedPlayer;

        /**
         * @brief Responds the command of the <b>ChangePath</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] uidCounter The value of the uid counter.
         * @param[in] direction  The flag of the navigation. Refer to <b>bluetooth::AvrcFolderDirection</b>.
         * @param[in] folderUid  The UID of the folder to navigate to. This may be retrieved via a GetFolderItems
         * command. If the navigation command is Folder Up this field is reserved.
         * @param[in] label      The label which is used to distinguish different call.
         */
        std::function<void(
            const RawAddress &rawAddr, uint16_t uidCounter, uint8_t direction, uint64_t folderUid, uint8_t label)>
            changePath;

        /**
         * @brief Responds the command of the <b>GetFolderItems</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] scope      The scope in which media content navigation may take place. Refer to
         * <b>AvrcMediaScope</b>.
         * @param[in] startItem  The offset within the listing of the item, which should be the first returned item. The
         * first element in the listing is at offset 0.
         * @param[in] endItem    The offset within the listing of the item which should be the final returned item. If
         * this is set to a value beyond what is available, the TG shall return items from the provided Start Item index
         * to the index of the final item. If the End Item index is smaller than the Start Item index, the TG shall
         * return an error. If CT requests too many items, TG can respond with a sub-set of the requested items.
         * @param[in] attributes The list of media attributes.
         * @param[in] label      The label which is used to distinguish different call.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t scope, uint32_t startItem, uint32_t endItem,
            const std::vector<uint32_t> &attributes, uint8_t label)>
            getFolderItems;

        /**
         * @brief Responds the command of the <b>GetItemAttributes</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] scope      The scope in which media content navigation may take place. Refer to
         * <b>AvrcMediaScope</b>.
         * @param[in] uid        The UID of the media element item or folder item.
         * @param[in] uidCounter The UID Counter.
         * @param[in] attributes The list of media attributes.
         * @param[in] label      The label which is used to distinguish different call.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter,
            const std::vector<uint32_t> &attributes, uint8_t label)>
            getGetItemAttributes;

        /**
         * @brief Responds the command of the <b>GetTotalNumberOfItems</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] scope      The scope in which media content navigation may take place. Refer to
         * <b>AvrcMediaScope</b>.
         * @param[in] label      The label which is used to distinguish different call.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t scope, uint8_t label)> getTotalNumberOfItems;

        /**
         * @brief Informs that receive the command of the <b>PlayItem</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] scope      The scope in which media content navigation may take place. Refer to
         * <b>AvrcMediaScope</b>.
         * @param[in] uid        The UID of the media element item or folder item.
         * @param[in] uidCounter The UID Counter.
         * @param[in] label      The label which is used to distinguish different call.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter, uint8_t label)>
            playItem;

        /**
         * @brief Responds the command of the <b>AddToNowPlaying</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] scope      The scope in which media content navigation may take place. Refer to
         * <b>AvrcMediaScope</b>.
         * @param[in] uid        The UID of the media element item or folder item.
         * @param[in] uidCounter The UID Counter.
         * @param[in] label      The label which is used to distinguish different call.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter, uint8_t label)>
            addToNowPlaying;

        /**
         * @brief Responds the command of the <b>SetAbsoluteVolume</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] volume  The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
         * @param[in] label   The label which is used to distinguish different call.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t volume, uint8_t label)> setAbsoluteVolume;

        /**
         * @brief Responds the command of the <b>EVENT_TRACK_CHANGED</b>.
         *
         * @param[in] rawAddr  The address of the bluetooth device.
         * @return The id of the addressed player.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t label)> getSelectedTrack;

        /**
         * @brief Responds the command of the <b>EVENT_ADDRESSED_PLAYER_CHANGED</b>.
         *
         * @param[in] rawAddr  The address of the bluetooth device.
         * @return The id of the addressed player.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t label)> getAddressedPlayer;

        /**
         * @brief Responds the command of the <b>EVENT_UIDS_CHANGED</b>.
         *
         * @param[in] rawAddr  The address of the bluetooth device.
         * @return The id of the current playing item.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t label)> getUidCounter;

        /**
         * @brief Responds the command of the <b>EVENT_ VOLUME_CHANGED</b>.
         *
         * @param[in] rawAddr  The address of the bluetooth device.
         * @return The id of the current playing item.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t label)> getCurrentAbsoluteVolume;

        /**
         * @brief Responds the command of the <b>RegisterNotification</b>.
         *
         * @param[in] rawAddr  The address of the bluetooth device.
         * @param[in] interval The specifies the time interval (in seconds) at which the change in playback position
         * will be notified. If the song is being forwarded / rewound, a notification will be received whenever the
         * playback position will change by this value.
         */
        std::function<void(const RawAddress &rawAddr, uint32_t interval)> setPlaybackInterval;

        /**
         * @brief Sets the address of the active bluetooth device.
         *
         * @param[in] rawAddr  The address of the bluetooth device.
         */
        std::function<void(const RawAddress &rawAddr)> setActiveDevice;
    };

    /**
     * @brief A constructor used to create an <b>AvrcTgProfile</b> instance.
     */
    AvrcTgProfile(uint16_t features, uint32_t companyId, uint16_t controlMtu, uint16_t browseMtu,
        utility::Dispatcher *dispatcher, AvctChannelEventCallback eventCallback, AvctMsgCallback msgCallback);

    /**
     * @brief A destructor used to delete the <b>AvrcTgProfile</b> instance.
     */
    ~AvrcTgProfile();

    /******************************************************************
     * REGISTER / UNREGISTER OBSERVER                                 *
     ******************************************************************/

    /**
     * @brief Registers the observer.
     *
     * @param observer The pointer to the instance of the <b>AvrcTgProfile::Observer</b> struct.
     */
    void RegisterObserver(AvrcTgProfile::Observer *observer);

    /**
     * @brief Unregisters the observer.
     */
    void UnregisterObserver(void);

    /******************************************************************
     * ENABLE / DISABLE                                               *
     ******************************************************************/

    /**
     * @brief Enables the AVRCP TG profile.
     *
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int Enable(void);

    /**
     * @brief Disables the AVRCP TG profile.
     *
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int Disable(void);

    /**
     * @brief Sets the flag of the profile enabled or not.
     *
     * @param[in] isEnabled The service is enabled or not.
     *            @a true  : The service is enabled.<br>
     *            @a false : The service is disabled.<br>
     */
    static void SetEnableFlag(bool isEnabled);

    /******************************************************************
     * CONNECTION                                                     *
     ******************************************************************/

    /**
     * @brief Sets the active device.
     *
     * @detail Only one CT can interact witch TG.
     */
    void SetActiveDevice(const RawAddress &rawAddr);

    /**
     * @brief Gets the connected devices.
     *
     * @return The list of the instance of the <b>RawAddress</b> class.
     */
    std::vector<RawAddress> GetConnectedDevices(void);

    /**
     * @brief Gets the devices of the specified states.
     *
     * @param[in] states The connection states. Refer to <b>BTConnectState</b>.
     * @return The list of the instance of the <b>RawAddress</b> class.
     */
    std::vector<RawAddress> GetDevicesByStates(const std::vector<int> &states);

    /**
     * @brief Gets the connection state of the specified bluetooth device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The connection state. Refer to <b>BTConnectState</b>.
     */
    int GetDeviceState(const RawAddress &rawAddr);

    /**
     * @brief Gets the maximum number of connections.
     *
     * @return The maximum number of connections.
     */
    int GetMaxConnectNum(void);

    /**
     * @brief Connects to the connection of the control channel.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int Connect(const RawAddress &rawAddr) const;

    /**
     * @brief Connects to the connection of the browse channel.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_BAD_STATUS Execute failure.
     */
    static int ConnectBr(const RawAddress &rawAddr);

    /**
     * @brief Disconnects the connection of the control channel.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int Disconnect(const RawAddress &rawAddr) const;

    /**
     * @brief Disconnects the connection of the browse channel.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_BAD_STATUS Execute failure.
     */
    static int DisconnectBr(const RawAddress &rawAddr);

    /**
     * @brief Gets the connection states.
     *
     * @return The connection states. The values are in bits.
     */
    int GetConnectState(void);

    /**
     * @brief Accepts the passive connection.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    static void AcceptPassiveConnect(const RawAddress &rawAddr);

    /**
     * @brief Rejects the passive connection.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void RejectPassiveConnect(const RawAddress &rawAddr);

    /******************************************************************
     * PASS THROUGH COMMAND                                           *
     ******************************************************************/

    /**
     * @brief Sends the press response of the <b>PASS THROUGH</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] button  The value of the key operation. Refer to <b>AvrcKeyOperation</b>.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] result  Whether accepted the key operation.
     *            @a true  : Accepted.
     *            @a false : Rejected.
     */
    void SendPressButtonRsp(const RawAddress &rawAddr, uint8_t button, uint8_t label, int result);

    /**
     * @brief Sends the release response of the <b>PASS THROUGH</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] button  The value of the key operation. Refer to <b>AvrcKeyOperation</b>.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] result  Whether accepted the key operation.
     *            @a true  : Accepted.
     *            @a false : Rejected.
     */
    void SendReleaseButtonRsp(const RawAddress &rawAddr, uint8_t button, uint8_t label, int result);

    /******************************************************************
     * VENDOR COMMAND                                                 *
     ******************************************************************/

    /**
     * @brief Sends the response of the <b>GetCapabilities</b>.
     *
     * @details This is sent by CT to get the capabilities of the peer device.
     * @param[in] rawAddr      The address of the bluetooth device.
     * @param[in] capabilityId Specific capability requested.
     * @param[in] companies    The list of CompanyId.
     * @param[in] events       The list of EventIds.
     * @param[in] label        The label which is used to distinguish different call.
     * @param[in] result       The result of calling the <b>ListPlayerApplicationSettingAttributes</b>.
     *            @a RET_NO_ERROR   : Accepted.
     *            @a RET_NO_SUPPORT : Not support.
     *            @a RET_BAD_STATUS : Rejected.
     */
    void SendGetCapabilitiesRsp(
        const RawAddress &rawAddr, const std::vector<uint32_t> &companies, uint8_t label, int result);
    void SendGetCapabilitiesRsp(
        const RawAddress &rawAddr, const std::vector<uint8_t> &events, uint8_t label, int result);

    /**
     * @brief Sends the response of the <b>ListPlayerApplicationSettingAttributes</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] result     The result of calling the <b>ListPlayerApplicationSettingAttributes</b>.
     *            @a RET_NO_ERROR   : Accepted.
     *            @a RET_NO_SUPPORT : Not support.
     *            @a RET_BAD_STATUS : Rejected.
     */
    void SendListPlayerApplicationSettingAttributesRsp(
        const RawAddress &rawAddr, std::deque<uint8_t> attributes, uint8_t label, int result);

    /**
     * @brief Sends the response of the <b>ListPlayerApplicationSettingValues</b>.

     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] values  The value of the player application setting attribute.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] result  The result of calling the <b>ListPlayerApplicationSettingValues</b>.
     *            @a RET_NO_ERROR   : Accepted.
     *            @a RET_NO_SUPPORT : Not support.
     *            @a RET_BAD_STATUS : Rejected.
     */
    void SendListPlayerApplicationSettingValuesRsp(
        const RawAddress &rawAddr, const std::deque<uint8_t> &values, uint8_t label, int result);

    /**
     * @brief Sends the response of the <b>GetCurrentPlayerApplicationSettingValue</b>.

     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings.
     * @param[in] values     The value of the player application setting attribute.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] result     The result of calling the <b>ListPlayerApplicationSettingValues</b>.
     *            @a RET_NO_ERROR   : Accepted.
     *            @a RET_NO_SUPPORT : Not support.
     *            @a RET_BAD_STATUS : Rejected.
     */
    void SendGetCurrentPlayerApplicationSettingValueRsp(const RawAddress &rawAddr,
        const std::deque<uint8_t> &attributes, const std::deque<uint8_t> &values, uint8_t label, int result);

    /**
     * @brief Sends the response of the <b>SetPlayerApplicationSettingValue</b>.

     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] result     The result of calling the <b>ListPlayerApplicationSettingValues</b>.
     *            @a RET_NO_ERROR   : Accepted.
     *            @a RET_NO_SUPPORT : Not support.
     *            @a RET_BAD_STATUS : Rejected.
     */
    void SendSetPlayerApplicationSettingValueRsp(const RawAddress &rawAddr, uint8_t label, int result);

    /**
     * @brief Sends the response of the <b>GetPlayerApplicationSettingAttributeText</b>.

     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings.
     * @param[in] attrStr    The values associated witch the attribute.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] result     The result of calling the <b>ListPlayerApplicationSettingValues</b>.
     *            @a RET_NO_ERROR   : Accepted.
     *            @a RET_NO_SUPPORT : Not support.
     *            @a RET_BAD_STATUS : Rejected.
     */
    void SendGetPlayerApplicationSettingAttributeTextRsp(const RawAddress &rawAddr,
        const std::vector<uint8_t> &attributes, const std::vector<std::string> &attrStr, uint8_t label, int result);

    /**
     * @brief Sends the response of the <b>GetPlayerApplicationSettingValueText</b>.

     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] values     Player application setting value ID for which the text is returned.
     * @param[in] valueStr   Specifies the player application setting value string.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] result     The result of calling the <b>ListPlayerApplicationSettingValues</b>.
     *            @a RET_NO_ERROR   : Accepted.
     *            @a RET_NO_SUPPORT : Not support.
     *            @a RET_BAD_STATUS : Rejected.
     */
    void SendGetPlayerApplicationSettingValueTextRsp(const RawAddress &rawAddr, const std::vector<uint8_t> &values,
        const std::vector<std::string> &valueStr, uint8_t label, int result);

    /**
     * @brief Responds the data of the <b>GetElementAttributes</b>.
     *
     * @details Switch to the thread of the AVRCP CT service in this function.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes Specifies the attribute ID for the attributes to be retrieved.
     * @param[in] values     The list of the value of this attribute.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] result     The result of calling the <b>ListPlayerApplicationSettingValues</b>.
     *            @a RET_NO_ERROR   : Accepted.
     *            @a RET_NO_SUPPORT : Not support.
     *            @a RET_BAD_STATUS : Rejected.
     */
    void SendGetElementAttributesRsp(const RawAddress &rawAddr, const std::vector<uint32_t> &attribtues,
        const std::vector<std::string> &values, uint8_t label, int result);

    /**
     * @brief Sends the response of the <b>GetPlayStatus</b>.
     *
     * @param[in] rawAddr      The address of the bluetooth device.
     * @param[in] songLength   The total length of the playing song in milliseconds.
     * @param[in] songPosition The current position of the playing in milliseconds elapsed.
     * @param[in] playStatus   The current status of playing. Refer to <b>AvrcPlayStatus</b>.
     * @param[in] label        The label which is used to distinguish different call.
     * @param[in] result       The result of calling the <b>GetPlayStatus</b>.
     *            @a RET_NO_ERROR   : Accepted.
     *            @a RET_NO_SUPPORT : Not support.
     *            @a RET_BAD_STATUS : Rejected.
     */
    void SendGetPlayStatusRsp(const RawAddress &rawAddr, uint32_t songLength, uint32_t songPosition, uint8_t playStatus,
        uint8_t label, int result);

    /**
     * @brief Sends the response of the <b>SetAddressedPlayer</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] status  The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_INVALID_PLAYER_ID<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] result  The result of calling the <b>SetAddressedPlayer</b>.
     *            @a RET_NO_ERROR   : Accepted.
     *            @a RET_NO_SUPPORT : Not support.
     *            @a RET_BAD_STATUS : Rejected.
     */
    void SendSetAddressedPlayerRsp(const RawAddress &rawAddr, int status, uint8_t label, int result);

    /**
     * @brief Sends the response of the <b>PlayItem</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] status  The current status of media. Refer to <b>AvrcEsCode</>.<br>
     *            @a AVRC_ES_CODE_INVALID_COMMAND<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_INVALID_PARAMETER<br>
     *            @a AVRC_ES_CODE_PARAMETER_CONTENT_ERROR<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_RESERVED<br>
     *            @a AVRC_ES_CODE_DOES_NOT_EXIST<br>
     *            @a AVRC_ES_CODE_INVALID_SCOPE
     *            @a AVRC_ES_CODE_FOLDER_ITEM_IS_NOT_PLAYABLE
     *            @a AVRC_ES_CODE_MEDIA_IN_USE
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     *            @a AVRC_ES_CODE_FOLDER_ITEM_IS_NOT_PLAYABLE
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] result  The result of calling the <b>PlayItem</b>.
     *            @a RET_NO_ERROR   : Accepted.
     *            @a RET_NO_SUPPORT : Not support.
     *            @a RET_BAD_STATUS : Rejected.
     */
    void SendPlayItemRsp(const RawAddress &rawAddr, int status, uint8_t label, int result);

    /**
     * @brief Sends the response of the <b>AddToNowPlaying</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] status  The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_DOES_NOT_EXIST<br>
     *            @a AVRC_ES_CODE_INVALID_SCOPE<br>
     *            @a AVRC_ES_CODE_FOLDER_ITEM_IS_NOT_PLAYABLE<br>
     *            @a AVRC_ES_CODE_MEDIA_IN_USE<br>
     *            @a AVRC_ES_CODE_NOW_PLAYING_LIST_FULL<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] result  The result of calling the <b>AddToNowPlaying</b>.
     *            @a RET_NO_ERROR   : Accepted.
     *            @a RET_NO_SUPPORT : Not support.
     *            @a RET_BAD_STATUS : Rejected.
     */
    void SendAddToNowPlayingRsp(const RawAddress &rawAddr, int status, uint8_t label, int result);

    /**
     * @brief Sends the response of the <b>SetAbsoluteVolume</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] volume  The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] result  The result of calling the <b>SetAbsoluteVolume</b>.
     *            @a RET_NO_ERROR   : Accepted.
     *            @a RET_NO_SUPPORT : Not support.
     *            @a RET_BAD_STATUS : Rejected.
     */
    void SendSetAbsoluteVolumeRsp(const RawAddress &rawAddr, uint8_t volume, uint8_t label, int result);

    /**
     * @brief Sends the response of the <b>EVENT_PLAYBACK_STATUS_CHANGED</b>.
     *
     * @param[in] playStatus Indicates the current status of playback.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] result  The result of calling the <b>SetAbsoluteVolume</b>.
     *            @a RET_NO_ERROR   : Accepted.
     *            @a RET_NO_SUPPORT : Not support.
     *            @a RET_BAD_STATUS : Rejected.
     */
    void SendPlaybackStatusChangedRsp(bool isInterim, uint8_t playStatus, uint8_t label, int result);

    /**
     * @brief Sends the response of the <b>EVENT_TRACK_CHANGED</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] Identifier Unique Identifier to identify an element on TG, as is used for GetElementAttribute command
     * in case Browsing is not supported and GetItemAttribute command in case Browsing is supported.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void SendTrackChangedRsp(bool isInterim, uint64_t identifier, uint8_t label, int result);

    void SendTrackReachedEndRsp(bool isInterim, uint8_t label, int result);
    void SendTrackReachedStartRsp(bool isInterim, uint8_t label, int result);

    /**
     * @brief Sends the response of the <b>EVENT_PLAYBACK_POS_CHANGED</b>.
     *
     * @param[in] rawAddr     The address of the bluetooth device.
     * @param[in] playbackPos Current playback position in millisecond.
     * @param[in] label       The label which is used to distinguish different call.
     */
    void SendPlaybackPosChangedRsp(bool isInterim, uint32_t playbackPos, uint8_t label, int result);

    /**
     * @brief Sends the response of the <b>EVENT_PLAYER_APPLICATION_SETTING_CHANGED</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes Player application setting attribute ID for which the value is returned.
     * @param[in] values     Currently set Player Application Setting value on the TG for the above attributes.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void SendPlayerApplicationSettingChangedRsp(bool isInterim, const std::deque<uint8_t> &attributes,
        const std::deque<uint8_t> &values, uint8_t label, int result);

    void SendNowPlayingContentChangedRsp(bool isInterim, uint8_t label, int result);

    void SendAvailablePlayersChangedRsp(bool isInterim, uint8_t label, int result);
    /**
     * @brief Sends the response of the <b>EVENT_ADDRESSED_PLAYER_CHANGED</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] playerId   The player id.
     * @param[in] uidCounter The uid counter.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void SendAddressedPlayerChangedRsp(
        bool isInterim, uint16_t playerId, uint16_t uidCounter, uint8_t label, int result);

    /**
     * @brief Sends the response of the <b>EVENT_UIDS_CHANGED</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] uid     The uid.
     */
    void SendUidsChangedRsp(bool isInterim, uint16_t uidCounter, uint8_t label, int result);

    /**
     * @brief Sends the response of the <b>EVENT_VOLUME_CHANGED</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] volume  The absolute volume.
     */
    void SendVolumeChangedRsp(bool isInterim, uint8_t volume, uint8_t label, int result);

    /******************************************************************
     * BROWSING COMMAND                                               *
     ******************************************************************/

    /**
     * @brief Sends the response of the <b>SetBrowsedPlayer</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] uidCounter The value of the uid counter.
     * @param[in] numOfItems The number of items in the directory.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] status     The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_INVALID_PLAYER_ID<br>
     *            @a AVRC_ES_CODE_PLAYER_NOT_BROWSABLE<br>
     *            @a AVRC_ES_CODE_PLAYER_NOT_ADDRESSED<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     */
    void SendSetBrowsedPlayerRsp(const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems,
        const std::vector<std::string> &folderNames, uint8_t label, int status);

    /**
     * @brief Sends the response of the <b>ChangePath</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] numOfItems The number of items in the directory.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] status     The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_INVALID_DIRECTION<br>
     *            @a AVRC_ES_CODE_NOT_A_DIRECTORY<br>
     *            @a AVRC_ES_CODE_DOES_NOT_EXIST<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     */
    void SendChangePathRsp(const RawAddress &rawAddr, uint32_t numOfItems, uint8_t label, int status);

    /**
     * @brief Sends the response of the <b>GetFolderItems</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] uidCounter The value of the uid counter.
     * @param[in] items      The list of <b>AvrcMpItem</b> class.
     * @param[in] values     The list of the value of this attribute.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] status     The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_DOES_NOT_EXIST<br>
     *            @a AVRC_ES_CODE_INVALID_SCOPE<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     */
    void SendGetFolderItemsRsp(const RawAddress &rawAddr, uint16_t uidCounter, const std::vector<AvrcMpItem> &items,
        uint8_t label, int status);

    /**
     * @brief Sends the response of the <b>GetFolderItems</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] uidCounter The value of the uid counter.
     * @param[in] items      The list of <b>AvrcMeItem</b> class.
     * @param[in] values     The list of the value of this attribute.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] status     The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_DOES_NOT_EXIST<br>
     *            @a AVRC_ES_CODE_INVALID_SCOPE<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     */
    void SendGetFolderItemsRsp(const RawAddress &rawAddr, uint16_t uidCounter, const std::vector<AvrcMeItem> &items,
        uint8_t label, int status);

    /**
     * @brief Sends the response of the <b>GetItemAttributes</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The list of media attributes.
     * @param[in] values     The list of the value of this attribute.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] status     The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_DOES_NOT_EXIST<br>
     *            @a AVRC_ES_CODE_INVALID_SCOPE<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     */
    void SendGetItemAttributesRsp(const RawAddress &rawAddr, const std::vector<uint32_t> &attributes,
        const std::vector<std::string> &values, uint8_t label, int status);

    /**
     * @brief Sends the response of the <b>GetTotalNumberOfItems</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] uidCounter The value of the uid counter.
     * @param[in] numOfItems The number of items in the directory.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] status     The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     */
    void SendGetTotalNumberOfItemsRsp(
        const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems, uint8_t label, int status);

    /**
     * @brief Processes events received from the AVCTP.
     *
     * @param[in] rawAddr   The address of the bluetooth device.
     * @param[in] connectId The ID of the connection.
     * @param[in] event     The event from the underlying module.
     * @param[in] result    The result of the event.
     * @param[in] context   The context is used to send the event in the callback.
     */
    void ProcessChannelEvent(
        const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context);

    void ProcessChannelEventConnectIndEvt(
        const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context);
    void ProcessChannelEventDisconnectIndEvt(
        const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context);
    void ProcessChannelEventDisconnectCfmEvt(
        const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context);

    void ProcessChannelEventBrConnectIndEvt(
        const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context);
    void ProcessChannelEventBrConnectCfmEvt(
        const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context);
    void ProcessChannelEventConnectCfmEvt(
        const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context);
    /**
     * @brief Processes messages received from the AVCTP.
     *
     * @param[in] connectId The ID of the connection.
     * @param[in] label     The label used to distinguish between different calls to the same function.
     * @param[in] crType    The type used to distinguish between the command frame and the response frame.
     * @param[in] chType    The type used to distinguish the message returned from the control channel or the browse
     * channel.
     * @param[in] pkt       The message sent by the peer bluetooth device.
     * @param[in] context   The context is used to send the message in the callback.
     */
    void ProcessChannelMessage(
        uint8_t connectId, uint8_t label, uint8_t crType, uint8_t chType, Packet *pkt, void *context);

    /**
     * @brief get the label for notification changed event.
     */
    std::pair<bool, uint8_t> GetNotificationLabel(uint8_t event);

    /**
     * @brief set the label for notification changed event.
     */
    void SetNotificationLabel(uint8_t event, uint8_t label);

private:
    /// The flag is used to indicate that the AVRCP TG profile is enabled or not.
    static bool g_isEnabled;

    /// The features supported by the AVRCP CT profile.
    uint32_t features_;
    /// The SIG company ID.
    uint32_t companyId_;
    /// The max MTU size of the control channel.
    uint16_t controlMtu_;
    /// The max MTU size of the browse channel.
    uint16_t browseMtu_;
    /// The dispatcher that is used to switch to the thread of the AVRCP CT service.
    utility::Dispatcher *dispatcher_ {nullptr};
    /// The connect id of the passive connection.
    uint8_t connectId_;

    /// The pointer to the observer of the <b>AvrcTgProfile</b> class.
    AvrcTgProfile::Observer *myObserver_ {nullptr};

    /// Notification label trackers
    std::pair<bool, uint8_t> playStatusChanged;
    std::pair<bool, uint8_t> trackChanged;

    /// The callback function, which registers into the AVCTP for receiving the events.
    AvctChannelEventCallback eventCallback_ {nullptr};
    /// The callback function, which registers into the AVCTP for receiving the messages.
    AvctMsgCallback msgCallback_ {nullptr};
    // Locks the local variable in a multi-threaded environment.
    std::recursive_mutex mutex_ {};
    /**
     * @brief A deleted default constructor.
     */
    AvrcTgProfile() = delete;

    /**
     * @brief Checks whether the AVRCP TG profile is enabled.
     *
     * @return The result of the method execution.
     * @retval true  The profile is enabled.
     * @retval false The profile is disabled.
     */
    static bool IsEnabled(void);

    /******************************************************************
     * PASS THROUGH COMMAND                                           *
     ******************************************************************/

    /**
     * @brief Checks whether the PASS THROUGH press command is same or not.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] button  The value of the key operation.
     * @return The result of the method execution.
     * @retval true  Same pass press command.
     * @retval false Different pass press command.
     */
    static bool IsSamePassPressCommand(const RawAddress &rawAddr, uint8_t button);

    /**
     * @brief Checks whether the PASS THROUGH release command is same or not.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] button  The value of the key operation.
     * @return The result of the method execution.
     * @retval true  Same pass release command.
     * @retval false Different release press command.
     */
    static bool IsIgnorePassReleaseCommand(const RawAddress &rawAddr, uint8_t button);

    /**
     * @brief Sends the response of the <b>PASS THROUGH</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The reference of the shared pointer to the instance of the <b>AvrcTgVendorPacket</b> class.
     */
    void SendPassRsp(const RawAddress &rawAddr, std::shared_ptr<AvrcTgPassPacket> &pkt);

    /**
     * @brief Receives the command of the <b>PASS THROUGH</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceivePassCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Processes the timeout of command of the <b>PASS THROUGH</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] button  The value of the key operation. Refer to <b>AvrcKeyOperation</b>.
     * @param[in] state   The value of the key state. Refer to <b>AvrcTgKeyState</b>.
     * @param[in] label   The label which is used to distinguish different call.
     */
    void ProcessPassTimeout(RawAddress rawAddr, uint8_t button, uint8_t state, uint8_t label);

    /**
     * @brief The callback function, which registers into the <b>utility::Timer</b>.
     *
     * @details This function switches to the thread of the AVRCP TG service firstly, then processes the timeout.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] button  The value of the key operation. Refer to <b>AvrcKeyOperation</b>.
     * @param[in] state   The value of the key state. Refer to <b>AvrcTgKeyState</b>.
     * @param[in] label   The label which is used to distinguish different call.
     */
    void PassTimeoutCallback(const RawAddress &rawAddr, uint8_t button, uint8_t state, uint8_t label);

    /******************************************************************
     * UNIT INFO / SUB UNIT INFO COMMAND                              *
     ******************************************************************/

    /**
     * @brief Sends the response of the <b>UNIT INFO</b> or the <b>SUB UNIT INFO</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The reference of the shared pointer to the instance of the <b>AvrcTgUnitPacket</b> class.
     */
    static void SendUnitRsp(const RawAddress &rawAddr, std::shared_ptr<AvrcTgUnitPacket> &pkt, AvrcTgSmEvent event);

    /**
     * @brief Receives the command of the <b>UNIT INFO</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveUnitCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>SUB UNIT INFO</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveSubUnitCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /******************************************************************
     * VENDOR COMMAND                                                 *
     ******************************************************************/

    /**
     * @brief Sends the response of the <b>VENDOR DEPENDENT</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The reference of the shared pointer to the instance of the <b>AvrcTgVendorPacket</b> class.
     * @param[in] event   The event which is responded by the state machine.
     */
    static void SendVendorRsp(const RawAddress &rawAddr, std::shared_ptr<AvrcTgVendorPacket> &pkt, AvrcTgSmEvent event);

    /**
     * @brief Receives the command of the <b>GetCapabilities</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveGetCapabilitiesCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>ListPlayerApplicationSettingAttributes</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveListPlayerApplicationSettingAttributesCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>ListPlayerApplicationSettingValues</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveListPlayerApplicationSettingValuesCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>GetCurrentPlayerApplicationSettingValue</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveGetCurrentPlayerApplicationSettingValueCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>SetPlayerApplicationSettingValue</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveSetPlayerApplicationSettingValueCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>GetPlayerApplicationSettingAttributeText</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveGetPlayerApplicationSettingAttributeTextCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>GetPlayerApplicationSettingValueText</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveGetPlayerApplicationSettingValueTextCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>GetElementAttributes</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveGetElementAttributesCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>GetPlayStatus</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveGetPlayStatusCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>RequestContinuingResponse</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveRequestContinuingResponseCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the "AbortContinuingResponse".
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveAbortContinuingResponseCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>SetAddressedPlayer</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveSetAddressedPlayerCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>PlayItem</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceivePlayItemCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>AddToNowPlaying</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveAddToNowPlayingCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>SetAbsoluteVolume</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveSetAbsoluteVolumeCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>RegisterNotification</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveRegisterNotificationCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>VENDOR DEPENDENT</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveVendorCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /******************************************************************
     * BROWSING COMMAND                                               *
     ******************************************************************/

    /**
     * @brief Sends the response of the <b>BROWSING</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    static void SendBrowseRsp(const RawAddress &rawAddr, std::shared_ptr<AvrcTgBrowsePacket> &pkt, AvrcTgSmEvent event);

    /**
     * @brief Receives the command of the <b>SetBrowsedPlayer</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveSetBrowsedPlayerCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>ChangePath</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveChangePathCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>GetFolderItems</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveGetFolderItemsCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>GetItemAttributes</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveGetItemAttributesCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>GetTotalNumberOfItems</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveGetTotalNumberOfItemsCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Receives the command of the <b>BROWSING</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveBrowseCmd(const RawAddress &rawAddr, uint8_t label, Packet *pkt);

    /**
     * @brief Deletes all the data of the specified device, include the connection information and the state machines.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    static void DeleteResource(const RawAddress &rawAddr);

    /**
     * @brief Deletes the browse state machine.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    static void DeleteBrowseStateMachine(const RawAddress &rawAddr);

    /**
     * @briefs Checks the browsing channel is supported or not.
     *
     * @return The result of the method execution.
     * @retval true  Supported.
     * @retval false Unsupported.
     */
    bool IsSupportedBrowsing(void) const
    {
        return ((features_ & AVRC_TG_FEATURE_BROWSING) == AVRC_TG_FEATURE_BROWSING);
    }

    /**
     * @brief Explains the response of the <b>AVCTP</b> function to the result.
     *
     * @param avctRet The response code.
     * @return The requested result.
     */
    static int ExpainAvctResult(uint16_t avctRet);

    /**
     * @briefs Explains the result to the response of the <b>PASS THROUGH</b> command.
     *
     * @return The value of the "response".
     */
    static uint8_t ExplainResultToPassCrCode(int result);

    /**
     * @briefs Explains the result to the "crCode" of the <b>STATUS</b> command.
     *
     * @return The value of the "crCode".
     */
    static uint8_t ExplainResultToStatusCrCode(int result);

    /**
     * @briefs Explains the result to the "crCode" of the <b>CONTROL</b> command.
     *
     * @return The value of the "crCode".
     */
    static uint8_t ExplainResultToControlCrCode(int result);

    BT_DISALLOW_COPY_AND_ASSIGN(AvrcTgProfile);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_TG_PROFILE_H
