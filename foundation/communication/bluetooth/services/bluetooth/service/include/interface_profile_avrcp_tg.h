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
 * @file interface_profile_avrcp_tg.h
 *
 * @brief Declares the basic class of the AVRCP service, including attributes, and methods.
 *
 * @since 6
 */

#ifndef INTERFACE_PROFILE_AVRCP_TG_H
#define INTERFACE_PROFILE_AVRCP_TG_H

#include <deque>
#include <vector>
#include "bt_def.h"
#include "avrcp_media.h"
#include "interface_profile.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This class provides data interaction for asynchronous actions.
 *
 * @since 6
 */
class IProfileAvrcpTg : public IProfile {
public:
    /**
     * @brief This abstract class declares a set of methods for observing the class of the <b>IProfileAvrcpTg</b>.
     *
     * @since 6
     */
    class IObserver {
    public:
        /**
         * @brief A constructor used to create an <b>IProfileAvrcpTg::IObserver</b> instance.
         *
         * @since 6
         */
        IObserver() = default;

        /**
         * @brief A destructor used to delete the <b>IProfileAvrcpTg::IObserver</b> instance.
         *
         * @since 6
         */
        virtual ~IObserver() = default;

        /**
         * @brief Informs the state of the connection.
         *
         * @param[in] device The bluetooth device.
         * @param[in] state  The connection state. Refer to <b>BTConnectState</b>.
         *
         * @since 6
         */
        virtual void OnConnectionStateChanged(const RawAddress &rawAddr, int state) = 0;
    };

    /******************************************************************
     * REGISTER / UNREGISTER OBSERVER                                 *
     ******************************************************************/

    /**
     * @brief Registers the observer into the instance of the class <b>AvrcpController</b>.
     *
     * @param[in] observer The observer which wants to register.
     * @since 6
     */
    virtual void RegisterObserver(IObserver *observer) = 0;

    /**
     * @brief Unregisters the observer from the instance of the class <b>AvrcpController</b>.
     *
     * @param[in] observer The observer which already registered.
     *
     * @since 6
     */
    virtual void UnregisterObserver(void) = 0;

    /******************************************************************
     * ENABLE / DISABLE                                               *
     ******************************************************************/

    /**
     * @brief Checks whether the AVRCP TG service is enabled.
     *
     * @return The result of the method execution.
     * @retval true  The service is enabled.
     * @retval false The service is disabled.
     *
     * @since 6
     */
    virtual bool IsEnabled(void) = 0;

    /******************************************************************
     * CONNECTION                                                     *
     ******************************************************************/

    /**
     * @brief Sets the active device.
     *
     * @detail Only one CT can interact witch TG.
     *
     * @since 6
     */
    virtual void SetActiveDevice(const RawAddress &rawAddr) = 0;

    /**
     * @brief Gets the devices of the specified states.
     *
     * @param[in] states The connection states. Refer to <b>BTConnectState</b>.
     * @return The list of the instance of the <b>RawAddress</b> class.
     *
     * @since 6
     */
    virtual std::vector<RawAddress> GetConnectedDevices(void) = 0;

    /**
     * @brief Gets the devices of the specified states.
     *
     * @param[in] states The connection states. Refer to <b>BTConnectState</b>.
     * @return The list of the instance of the <b>RawAddress</b> class.
     *
     * @since 6
     */
    virtual std::vector<RawAddress> GetDevicesByStates(const std::vector<int> &states) = 0;

    /**
     * @brief Gets the connection state of the specified bluetooth device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The connection state. Refer to <b>BTConnectState</b>.
     *
     * @since 6
     */
    virtual int GetDeviceState(const RawAddress &rawAddr) = 0;

    /******************************************************************
     * Media Player Selection                                         *
     ******************************************************************/

    /**
     * @brief Responds the data of the <b>SetAddressedPlayer</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] status  The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_INVALID_PLAYER_ID<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     *
     * @since 6
     */
    virtual void OnSetAddressedPlayer(const RawAddress &rawAddr, uint8_t label, int status) = 0;

    /**
     * @brief Responds the data of the <b>SetBrowsedPlayer</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr     The address of the bluetooth device.
     * @param[in] uidCounter  The value of the uid counter.
     * @param[in] numOfItems  The number of items in the directory.
     * @param[in] folderNames The current browsed path of the player. The first element in folderNames is the parent
     * folder. The root folder has no name. The names are in UTF-8.
     * @param[in] label       The label which is used to distinguish different call.
     * @param[in] detail      The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_INVALID_PLAYER_ID<br>
     *            @a AVRC_ES_CODE_PLAYER_NOT_BROWSABLE<br>
     *            @a AVRC_ES_CODE_PLAYER_NOT_ADDRESSED<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     *
     * @since 6
     */
    virtual void OnSetBrowsedPlayer(const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems,
        const std::vector<std::string> &folderNames, uint8_t label, int status) = 0;

    /******************************************************************
     * PLAYER APPLICATION SETTINGS                                    *
     ******************************************************************/

    /**
     * @brief Responds the data of the <b>GetPlayerAppSettingAttributes</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr       The address of the bluetooth device.
     * @param[in] attributes    The attribute of player application settings.
     * @param[in] label         The label which is used to distinguish different call.
     *
     * @since 6
     */
    virtual void OnGetPlayerAppSettingAttributes(
        const RawAddress &rawAddr, const std::deque<uint8_t> &attributes, uint8_t label) = 0;

    /**
     * @brief Responds the data of the <b>GetPlayerAppSettingValues</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr   The address of the bluetooth device.
     * @param[in] values    The values associated witch the attribute.
     * @param[in] label     The label which is used to distinguish different call.
     *
     * @since 6
     */
    virtual void OnGetPlayerAppSettingValues(
        const RawAddress &rawAddr, const std::deque<uint8_t> &values, uint8_t label) = 0;

    /**
     * @brief Responds the data of the <b>GetPlayerAppSettingCurrentValue</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings. Refer to <b>AvrcPlayerAttribute</b>.
     * @param[in] values     The values associated witch the attribute.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] context    The context which is used to distinguish different purpose.
     *
     * @since 6
     */
    virtual void OnGetPlayerAppSettingCurrentValue(const RawAddress &rawAddr, const std::deque<uint8_t> &attributes,
        const std::deque<uint8_t> &values, uint8_t label, uint8_t context) = 0;

    /**
     * @brief Responds the data of the <b>SetPlayerAppSettingCurrentValue</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     *
     * @since 6
     */
    virtual void OnSetPlayerAppSettingCurrentValue(const RawAddress &rawAddr, uint8_t label) = 0;

    /**
     * @brief Responds the data of the <b>GetPlayerAppSettingAttributeTest</b>.
     *
     * @param[in] addr       The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings.
     * @param[in] attrStr    The values associated witch the attribute.
     * @param[in] label      The label which is used to distinguish different call.
     *
     * @since 6
     */
    virtual void OnGetPlayerAppSettingAttributeText(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes,
        const std::vector<std::string> &attrStr, uint8_t label) = 0;

    /**
     * @brief Responds the data of the <b>GetPlayerAppSettingValueTest</b>.
     *
     * @param[in] addr       The address of the bluetooth device.
     * @param[in] values     The attribute of the player application settings.
     * @param[in] valueStr   The values associated witch the attribute.
     * @param[in] label      The label which is used to distinguish different call.
     *
     * @since 6
     */
    virtual void OnGetPlayerAppSettingValueText(const RawAddress &rawAddr, const std::vector<uint8_t> &values,
        const std::vector<std::string> &valueStr, uint8_t label) = 0;

    /******************************************************************
     * MEDIA INFORMATION PDUS                                         *
     ******************************************************************/

    /**
     * @brief Responds the data of the <b>GetElementAttributes</b>.
     *
     * @details Switch to the thread of the AVRCP CT service in this function.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes  Specifies the attribute ID forthe attributes to be retrieved.
     * @param[in] values     The list of the value of this attribute.
     * @param[in] label      The label which is used to distinguish different call.
     *
     * @since 6
     */
    virtual void OnGetElementAttributes(const RawAddress &rawAddr, const std::vector<uint32_t> &attribtues,
        const std::vector<std::string> &values, uint8_t label) = 0;

    /******************************************************************
     * PLAY                                                           *
     ******************************************************************/

    /**
     * @brief Responds the data of the <b>GetPlayStatus</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr      The address of the bluetooth device.
     * @param[in] songLength   The total length of the playing song in milliseconds.
     * @param[in] songPosition The current position of the playing in milliseconds elapsed.
     * @param[in] playStatus   The current status of playing. Refer to <b>AvrcPlayStatus</b>.
     * @param[in] label        The label which is used to distinguish different call.
     * @param[in] context      The context which is used to distinguish different purpose.
     *
     * @since 6
     */
    virtual void OnGetPlayStatus(const RawAddress &rawAddr, uint32_t songLength, uint32_t songPosition,
        uint8_t playStatus, uint8_t label, uint8_t context) = 0;

    /**
     * @brief Responds the data of the <b>PlayItem</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] status  The current status of media.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] result  The result of the action.
     *            @c RET_NO_ERROR   : The request was accepted.
     *            @c RET_NO_SUPPORT : The request was not support.
     *            @c RET_BAD_STATUS : The request was rejected.
     */
    virtual void OnPlayItem(const RawAddress &rawAddr, uint8_t label, int status) = 0;

    /**
     * @brief Responds the data of the <b>AddToNowPlaying</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
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
     *
     * @since 6
     */
    virtual void OnAddToNowPlaying(const RawAddress &rawAddr, uint8_t label, int status) = 0;

    /******************************************************************
     * OPERATE THE VIRTUAL FILE SYSTEM                                *
     ******************************************************************/

    /**
     * @brief Responds the data of the <b>ChangePath</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
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
     *
     * @since 6
     */
    virtual void OnChangePath(const RawAddress &rawAddr, uint32_t numOfItems, uint8_t label, int status) = 0;

    /**
     * @brief Responds the data of the <b>GetFolderItems</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] uidCounter The value of the uid counter.
     * @param[in] items      The list of media player.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] status     The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_DOES_NOT_EXIST<br>
     *            @a AVRC_ES_CODE_INVALID_SCOPE<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     *
     * @since 6
     */
    virtual void OnGetMediaPlayers(const RawAddress &rawAddr, uint16_t uidCounter, const std::vector<AvrcMpItem> &items,
        uint8_t label, int status) = 0;

    /**
     * @brief Responds the data of the <b>GetFolderItems</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] uidCounter The value of the uid counter.
     * @param[in] items      The list of media items.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] status     The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_DOES_NOT_EXIST<br>
     *            @a AVRC_ES_CODE_INVALID_SCOPE<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     *
     * @since 6
     */
    virtual void OnGetFolderItems(const RawAddress &rawAddr, uint16_t uidCounter, const std::vector<AvrcMeItem> &items,
        uint8_t label, int status) = 0;

    /**
     * @brief Responds the data of the <b>GetItemAttributes</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
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
     *
     * @since 6
     */
    virtual void OnGetItemAttributes(const RawAddress &rawAddr, const std::vector<uint32_t> &attributes,
        const std::vector<std::string> &values, uint8_t label, int status) = 0;

    /**
     * @brief Responds the data of the <b>GetTotalNumberOfItems</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] uidCounter The value of the uid counter.
     * @param[in] numOfItems The number of items in the directory.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] status     The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     *
     * @since 6
     */
    virtual void OnGetTotalNumberOfItems(
        const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems, uint8_t label, int status) = 0;

    /******************************************************************
     * ABSOLUTE VOLUME                                                *
     ******************************************************************/

    /**
     * @brief Responds the data of the <b>SetAbsoluteVolume</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] volume  The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
     * @param[in] label   The label which is used to distinguish different call.
     *
     * @since 6
     */
    virtual void OnSetAbsoluteVolume(const RawAddress &rawAddr, uint8_t volume, uint8_t label) = 0;

    /******************************************************************
     * NOTIFICATION                                                   *
     ******************************************************************/

    virtual void OnGetSelectedTrack(const RawAddress &rawAddr, uint64_t uid, uint8_t label) = 0;

    virtual void OnGetAddressedPlayer(
        const RawAddress &rawAddr, uint16_t playerId, uint16_t uidCounter, uint8_t label) = 0;

    virtual void OnGetUidCounter(const RawAddress &rawAddr, uint16_t uidCounter, uint8_t label) = 0;

    virtual void OnGetCurrentAbsoluteVolume(const RawAddress &rawAddr, uint8_t volume, uint8_t label) = 0;

    /**
     * @brief Notifies the playback status is changed.
     *
     * @param[in] playStatus  The current status of playing. Refer to <b>AvrcPlayStatus</b>.
     * @param[in] playbackPos Current playback position in millisecond.
     * @param[in] label       The label which is used to distinguish different call.
     *
     * @since 6
     */
    virtual void NotifyPlaybackStatusChanged(
        uint8_t playStatus, uint32_t playbackPos, uint8_t label = AVRC_DEFAULT_LABEL) = 0;

    /**
     * @brief Notifies the track reached end is changed.
     *
     * @param[in] uid         The unique ID of media item.
     * @param[in] playbackPos Current playback position in millisecond.
     * @param[in] label       The label which is used to distinguish different call.
     *
     * @since 6
     */
    virtual void NotifyTrackChanged(uint64_t uid, uint32_t playbackPos, uint8_t label = AVRC_DEFAULT_LABEL) = 0;

    /**
     * @brief Notifies the track reached end is changed.
     *
     * @param[in] playbackPos Current playback position in millisecond.
     * @param[in] label       The label which is used to distinguish different call.
     *
     * @since 6
     */
    virtual void NotifyTrackReachedEnd(uint32_t playbackPos, uint8_t label = AVRC_DEFAULT_LABEL) = 0;

    /**
     * @brief Notifies the track reached start is changed.
     *
     * @param[in] playbackPos Current playback position in millisecond.
     * @param[in] label       The label which is used to distinguish different call.
     *
     * @since 6
     */
    virtual void NotifyTrackReachedStart(uint32_t playbackPos, uint8_t label = AVRC_DEFAULT_LABEL) = 0;

    /**
     * @brief Notifies the player application setting is changed.
     *
     * @param[in] playbackPos Current playback position in millisecond.
     * @param[in] label       The label which is used to distinguish different call.
     *
     * @since 6
     */
    virtual void NotifyPlaybackPosChanged(uint32_t playbackPos, uint8_t label = AVRC_DEFAULT_LABEL) = 0;

    /**
     * @brief Notifies the player application setting is changed.
     *
     * @param[in] attribute The attribute of the player application setting. Refer to <b>AvrcPlayerAttribute</b>.
     * @param[in] values    The values associated witch the attribute.
     * @param[in] label     The label which is used to distinguish different call.
     *
     * @since 6
     */
    virtual void NotifyPlayerAppSettingChanged(const std::deque<uint8_t> &attributes, const std::deque<uint8_t> &values,
        uint8_t label = AVRC_DEFAULT_LABEL) = 0;

    /**
     * @brief Notifies the now playing content is changed.
     *
     * @param[in] label The label which is used to distinguish different call.
     *
     * @since 6
     */
    virtual void NotifyNowPlayingContentChanged(uint8_t label = AVRC_DEFAULT_LABEL) = 0;

    /**
     * @brief Notifies the available player is changed.
     *
     * @param[in] label The label which is used to distinguish different call.
     *
     * @since 6
     */
    virtual void NotifyAvailablePlayersChanged(uint8_t label = AVRC_DEFAULT_LABEL) = 0;

    /**
     * @brief Notifies the addressed player is changed.
     *
     * @param[in] playerId   The unique media player id.
     * @param[in] uidCounter The UID counter shall be incremented every time the TG makes an update.
     * @param[in] label      The label which is used to distinguish different call.
     *
     * @since 6
     */
    virtual void NotifyAddressedPlayerChanged(
        uint16_t playerId, uint16_t uidCounter, uint8_t label = AVRC_DEFAULT_LABEL) = 0;

    /**
     * @brief Notifies the uids is changed.
     *
     * @param[in] uidCounter The UID counter shall be incremented every time the TG makes an update.
     * @param[in] label      The label which is used to distinguish different call.
     *
     * @since 6
     */
    virtual void NotifyUidChanged(uint16_t uidCounter, uint8_t label = AVRC_DEFAULT_LABEL) = 0;

    /**
     * @brief Notifies the absolute volume is changed.
     *
     * @param[in] volume The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
     * @param[in] label  The label which is used to distinguish different call.
     *
     * @since 6
     */
    virtual void NotifyVolumeChanged(uint8_t volume, uint8_t label = AVRC_DEFAULT_LABEL) = 0;

    /**
     * @brief Notifies the AVSession is created.
     */
    virtual void OnSessionCreate(std::string sessionId) = 0;

    /**
     * @brief Notifies the AVSession is release.
     */
    virtual void OnSessionRelease(std::string sessionId) = 0;

    /**
     * @brief Notifies the TopAVSession is changed.
     */

    virtual void OnTopSessionChange(std::string sessionId) = 0;
    /**
     * @brief Notifies the playback state is changed.
     */
    virtual void OnPlaybackStateChange(int32_t state) = 0;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // INTERFACE_PROFILE_AVRCP_TG_H
