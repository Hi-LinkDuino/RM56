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
 * @file interface_profile_avrcp_ct.h
 *
 * @brief Declares the basic class of the AVRCP service, including attributes, and methods.
 *
 * @since 6
 */

#ifndef INTERFACE_PROFILE_AVRCP_CT_H
#define INTERFACE_PROFILE_AVRCP_CT_H

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
class IProfileAvrcpCt : public IProfile {
public:
    /**
     * @brief This abstract class declares a set of methods for observing the class of the <b>IProfileAvrcpCt</b>.
     *
     * @since 6
     */
    class IObserver {
    public:
        /**
         * @brief A constructor used to create an <b>IProfileAvrcpCt::IObserver</b> instance.
         *
         * @since 6
         */
        IObserver() = default;

        /**
         * @brief A destructor used to delete the <b>IProfileAvrcpCt::IObserver</b> instance.
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

        /**
         * @brief Responds the data of the <b>PressButton</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] button  The value of the key operation. Refer to <b>AvrcKeyOperation</b>.
         * @param[in] result  The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         *
         * @since 6
         */
        virtual void OnPressButton(const RawAddress &rawAddr, uint8_t button, int result) = 0;

        /**
         * @brief Responds the data of the <b>ReleaseButton</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] button  The value of the key operation. Refer to <b>AvrcKeyOperation</b>.
         * @param[in] result  The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         *
         * @since 6
         */
        virtual void OnReleaseButton(const RawAddress &rawAddr, uint8_t button, int result) = 0;

        /**
         * @brief Responds the data of the <b>SetAddressedPlayer</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] result  The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         * @param[in] detail  The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
         *            @a AVRC_ES_CODE_NO_ERROR<br>
         *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
         *            @a AVRC_ES_CODE_UID_CHANGED<br>
         *            @a AVRC_ES_CODE_INVALID_PLAYER_ID<br>
         *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
         *
         * @since 6
         */
        virtual void OnSetAddressedPlayer(const RawAddress &rawAddr, int result, int detail) = 0;

        /**
         * @brief Responds the data of the <b>SetBrowsedPlayer</b>.
         *
         * @param[in] rawAddr     The address of the bluetooth device.
         * @param[in] uidCounter  The value of the uid counter.
         * @param[in] numOfItems  The number of items in the directory.
         * @param[in] folderNames The current browsed path of the player. The first element in folderNames is the parent
         * folder. The root folder has no name. The names are in UTF-8.
         * @param[in] result      The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
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
            const std::vector<std::string> &folderNames, int result, int detail) = 0;

        /**
         * @brief Responds the data of the <b>GetCapabilities</b>.
         *
         * @param[in] addr         The address of the bluetooth device.
         * @param[in] companies    The list of CompanyId.
         * @param[in] events       The list of Events.
         * @param[in] result      The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         *
         * @since 6
         */
        virtual void OnGetCapabilities(const RawAddress &rawAddr, const std::vector<uint32_t> &companies,
            const std::vector<uint8_t> &events, int result) = 0;

        /**
         * @brief Responds the data of the <b>GetPlayerAppSettingAttributes</b>.
         *
         * @param[in] rawAddr   The address of the bluetooth device.
         * @param[in] attributes The attribute of the player application settings.
         * @param[in] result    The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         *
         * @since 6
         */
        virtual void OnGetPlayerAppSettingAttributes(
            const RawAddress &rawAddr, std::vector<uint8_t> attributes, int result) = 0;

        /**
         * @brief Responds the data of the <b>GetPlayerAppSettingValues</b>.
         *
         * @param[in] rawAddr   The address of the bluetooth device.
         * @param[in] attribute The attribute of the player application setting. Refer to <b>AvrcPlayerAttribute</b>.
         * @param[in] values    The values associated witch the attribute.
         * @param[in] result    The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         *
         * @since 6
         */
        virtual void OnGetPlayerAppSettingValues(
            const RawAddress &rawAddr, uint8_t attribute, const std::vector<uint8_t> &values, int result) = 0;

        /**
         * @brief Responds the data of the <b>GetPlayerAppSettingCurrentValue</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] attributes The attribute of the player application settings. Refer to <b>AvrcPlayerAttribute</b>.
         * @param[in] values     The values associated witch the attribute.
         * @param[in] result     The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         *
         * @since 6
         */
        virtual void OnGetPlayerAppSettingCurrentValue(const RawAddress &rawAddr,
            const std::vector<uint8_t> &attributes, const std::vector<uint8_t> &values, int result) = 0;

        /**
         * @brief Responds the data of the <b>SetPlayerAppSettingCurrentValue</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] result  The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         *
         * @since 6
         */
        virtual void OnSetPlayerAppSettingCurrentValue(const RawAddress &rawAddr, int result) = 0;

        /**
         * @brief Responds the data of the <b>GetPlayerApplicationSettingAttributeText</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] attributes The attribute of the player application settings.
         * @param[in] attrStr    The values associated witch the attribute.
         * @param[in] result     The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         * @since 6
         */
        virtual void OnGetPlayerAppSettingAttributeText(const RawAddress &rawAddr,
            const std::vector<uint8_t> &attributes, const std::vector<std::string> &attrStr, int result) = 0;

        /**
         * @brief Responds the data of the <b>GetPlayerAppSettingValueText</b>.
         *
         * @param[in] addr       The address of the bluetooth device.
         * @param[in] values     Player application setting value ID.
         * @param[in] valueStr   Specifies the value string of player application setting value.
         * @param[in] result     The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         * @since 6
         */
        virtual void OnGetPlayerAppSettingValueText(const RawAddress &rawAddr, const std::vector<uint8_t> &values,
            const std::vector<std::string> &valueStr, int result) = 0;

        /**
         * @brief Responds the data of the <b>GetElementAttribtues</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] attributes The attributes of the player application setting. Refer to <b>AvrcPlayerAttribute</b>.
         * @param[in] values     The values associated witch the attribute.Refer to <b>GetElementAttribtues</b>.
         * @param[in] result     The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         *
         * @since 6
         */

        virtual void OnGetElementAttributes(const RawAddress &rawAddr, const std::vector<uint32_t> &attributes,
            const std::vector<std::string> &values, int result) = 0;

        /**
         * @brief Responds the data of the <b>GetPlayStatus</b>.
         *
         * @param[in] rawAddr      The address of the bluetooth device.
         * @param[in] songLength   The total length of the playing song in milliseconds.
         * @param[in] songPosition The current position of the playing in milliseconds elapsed.
         * @param[in] playStatus   The current status of playing. Refer to <b>AvrcPlayStatus</b>.
         * @param[in] result       The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         *
         * @since 6
         */
        virtual void OnGetPlayStatus(
            const RawAddress &rawAddr, uint32_t songLength, uint32_t songPosition, uint8_t playStatus, int result) = 0;

        /**
         * @brief Responds the data of the <b>AddToNowPlaying</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] result  The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         * @param[in] detail     The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
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
        virtual void OnAddToNowPlaying(const RawAddress &rawAddr, int result, int detail) = 0;

        /**
         * @brief Responds the data of the <b>PlayItem</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] result  The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         * @param[in] detail  The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
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
        virtual void OnPlayItem(const RawAddress &rawAddr, int result, int detail) = 0;

        /**
         * @brief Responds the data of the <b>ChangePath</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] numOfItems The number of items in the directory.
         * @param[in] result     The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         * @param[in] detail     The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
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
        virtual void OnChangePath(const RawAddress &rawAddr, uint32_t numOfItems, int result, int detail) = 0;

        /**
         * @brief Responds the data of the <b>GetFolderItems</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] uidCounter The value of the uid counter.
         * @param[in] items      The list of media player.
         * @param[in] result  The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         * @param[in] detail  The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
         *            @a AVRC_ES_CODE_NO_ERROR<br>
         *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
         *            @a AVRC_ES_CODE_UID_CHANGED<br>
         *            @a AVRC_ES_CODE_DOES_NOT_EXIST<br>
         *            @a AVRC_ES_CODE_INVALID_SCOPE<br>
         *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
         *
         * @since 6
         */
        virtual void OnGetMediaPlayers(const RawAddress &rawAddr, uint16_t uidCounter,
            const std::vector<AvrcMpItem> &items, int result, int detail) = 0;

        /**
         * @brief Responds the data of the <b>GetFolderItems</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] uidCounter The value of the uid counter.
         * @param[in] items      The list of media items.
         * @param[in] result  The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         * @param[in] detail  The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
         *            @a AVRC_ES_CODE_NO_ERROR<br>
         *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
         *            @a AVRC_ES_CODE_UID_CHANGED<br>
         *            @a AVRC_ES_CODE_DOES_NOT_EXIST<br>
         *            @a AVRC_ES_CODE_INVALID_SCOPE<br>
         *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
         *
         * @since 6
         */
        virtual void OnGetFolderItems(const RawAddress &rawAddr, uint16_t uidCounter,
            const std::vector<AvrcMeItem> &items, int result, int detail) = 0;

        /**
         * @brief Responds the data of the <b>GetItemAttributes</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] attributes The list of media attributes.
         * @param[in] values     The list of the value of this attribute.
         * @param[in] result     The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         * @param[in] detail     The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
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
            const std::vector<std::string> &values, int result, int detail) = 0;

        /**
         * @brief Responds the data of the <b>GetTotalNumberOfItems</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] uidCounter The value of the uid counter.
         * @param[in] numOfItems The number of items in the directory.
         * @param[in] result     The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         * @param[in] detail     The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
         *            @a AVRC_ES_CODE_NO_ERROR<br>
         *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
         *            @a AVRC_ES_CODE_UID_CHANGED<br>
         *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
         *
         * @since 6
         */
        virtual void OnGetTotalNumberOfItems(
            const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems, int result, int detail) = 0;

        /**
         * @brief Responds the data of the <b>SetAbsoluteVolume</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] volume  The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
         * @param[in] result  The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         *
         * @since 6
         */
        virtual void OnSetAbsoluteVolume(const RawAddress &rawAddr, uint8_t volume, int result) = 0;

        /**
         * @brief Responds the data of the notification <b>EVENT_PLAYBACK_STATUS_CHANGED</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] playStatus The current status of playing. Refer to <b>AvrcPlayStatus</b>.
         *
         * @since 6
         */
        virtual void OnPlaybackStatusChanged(const RawAddress &rawAddr, uint8_t playStatus, int result) = 0;

        /**
         * @brief Responds the data of the notification <b>EVENT_TRACK_CHANGED</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] uid     The unique ID of media item.
         *
         * @since 6
         */
        virtual void OnTrackChanged(const RawAddress &rawAddr, uint64_t uid, int result) = 0;

        /**
         * @brief Responds the data of the notification <b>EVENT_TRACK_REACHED_END</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         *
         * @since 6
         */
        virtual void OnTrackReachedEnd(const RawAddress &rawAddr, int result) = 0;

        /**
         * @brief Responds the data of the notification <b>EVENT_TRACK_REACHED_START</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         *
         * @since 6
         */
        virtual void OnTrackReachedStart(const RawAddress &rawAddr, int result) = 0;

        /**
         * @brief Responds the data of the notification <b>EVENT_PLAYBACK_POS_CHANGED</b>.
         *
         * @param[in] rawAddr     The address of the bluetooth device.
         * @param[in] playbackPos Current playback position in millisecond.
         *
         * @since 6
         */
        virtual void OnPlaybackPosChanged(const RawAddress &rawAddr, uint32_t playbackPos, int result) = 0;

        /**
         * @brief Responds the data of the notification <b>EVENT_PLAYER_APPLICATION_SETTING_CHANGED</b>.
         *
         * @param[in] rawAddr   The address of the bluetooth device.
         * @param[in] attribute The attribute of the player application setting. Refer to <b>AvrcPlayerAttribute</b>.
         * @param[in] values    The values associated witch the attribute.
         *
         * @since 6
         */
        virtual void OnPlayerAppSettingChanged(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes,
            const std::vector<uint8_t> &values, int result) = 0;

        /**
         * @brief Responds the data of the notification <b>EVENT_NOW_PLAYING_CONTENT_CHANGED</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         *
         * @since 6
         */
        virtual void OnNowPlayingContentChanged(const RawAddress &rawAddr, int result) = 0;

        /**
         * @brief Responds the data of the notification <b>EVENT_AVAILABLE_PLAYERS_CHANGED</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         *
         * @since 6
         */
        virtual void OnAvailablePlayersChanged(const RawAddress &rawAddr, int result) = 0;

        /**
         * @brief Responds the data of the notification <b>EVENT_ADDRESSED_PLAYER_CHANGED</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] playerId   The unique media player id.
         * @param[in] uidCounter The UID counter shall be incremented every time the TG makes an update.
         *
         * @since 6
         */
        virtual void OnAddressedPlayerChanged(
            const RawAddress &rawAddr, uint16_t playerId, uint16_t uidCounter, int result) = 0;

        /**
         * @brief Responds the data of the notification <b>EVENT_UIDS_CHANGED</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] uidCounter The UID counter shall be incremented every time the TG makes an update.
         *
         * @since 6
         */
        virtual void OnUidChanged(const RawAddress &rawAddr, uint16_t uidCounter, int result) = 0;

        /**
         * @brief Responds the data of the notification <b>EVENT_VOLUME_CHANGED</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] volume  The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
         *
         * @since 6
         */
        virtual void OnVolumeChanged(const RawAddress &rawAddr, uint8_t volume, int result) = 0;
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
     * @since 6
     */
    virtual void UnregisterObserver(void) = 0;

    /******************************************************************
     * ENABLE / DISABLE                                               *
     ******************************************************************/

    /**
     * @brief Checks whether the AVRCP CT service is enabled.
     *
     * @return The result of the method execution.
     * @retval true  The service is enabled.
     * @retval false The service is disabled.
     */
    virtual bool IsEnabled(void) = 0;

    /******************************************************************
     * CONNECTION                                                     *
     ******************************************************************/

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
     * BUTTON OPERATION                                               *
     ******************************************************************/

    /**
     * @brief Presses the button.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] button  The value of the key operation. Refer to <b>AvrcKeyOperation</b>
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int PressButton(const RawAddress &rawAddr, uint8_t button) = 0;

    /**
     * @brief Releases the button.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] button  The value of the key operation. Refer to <b>AvrcKeyOperation</b>
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int ReleaseButton(const RawAddress &rawAddr, uint8_t button) = 0;

    /******************************************************************
     * UNIT INFO / SUB UNIT INFO                                      *
     ******************************************************************/

    /**
     * @brief Gets the unit information.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int GetUnitInfo(const RawAddress &rawAddr) = 0;

    /**
     * @brief Gets the sub unit information.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int GetSubUnitInfo(const RawAddress &rawAddr) = 0;

    /******************************************************************
     * Media Player Selection                                         *
     ******************************************************************/

    /**
     * @brief Informs to which media player wishes to control.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr  The address of the bluetooth device.
     * @param[in] playerId The unique media player id.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int SetAddressedPlayer(const RawAddress &rawAddr, uint16_t playerId) = 0;

    /**
     * @brief Informs to which player browsing commands should be routed.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr  The address of the bluetooth device.
     * @param[in] playerId The unique media player id.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int SetBrowsedPlayer(const RawAddress &rawAddr, uint16_t playerId) = 0;

    /******************************************************************
     * Capabilities                                                   *
     ******************************************************************/

    /**
     * @brief Get the supported companies by remote device.
     *
     * @details This is sent by CT to get the capabilities of the peer device
     * @param[in] rawAddr      The address of the bluetooth device.
     * @param[in] capabilityID Specific capability requested
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int GetSupportedCompanies(const RawAddress &rawAddr) = 0;

    /**
     * @brief Get the supported events by remote device.
     *
     * @details This is sent by CT to get the capabilities of the peer device
     * @param[in] rawAddr      The address of the bluetooth device.
     * @param[in] capabilityID Specific capability requested
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int GetSupportedEvents(const RawAddress &rawAddr) = 0;
    /******************************************************************
     * PLAYER APPLICATION SETTINGS                                    *
     ******************************************************************/

    /**
     * @brief Gets the attribute of the player application.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr   The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int GetPlayerAppSettingAttributes(const RawAddress &rawAddr) = 0;

    /**
     * @brief Gets the values of the specified attribute of the player application.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr   The address of the bluetooth device.
     * @param[in] attribute The attribute of the player application setting. Refer to <b>AvrcPlayerAttribute</b>.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int GetPlayerAppSettingValues(const RawAddress &rawAddr, uint8_t attribute) = 0;

    /**
     * @brief Gets the current set values on the target for the provided player application setting attributes list.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings. Refer to <b>AvrcPlayerAttribute</b>.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int GetPlayerAppSettingCurrentValue(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes) = 0;

    /**
     * @brief Sets the player application setting list of player application setting values on the target device for the
     * corresponding defined list of AvrcPlayerAttribute.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings. Refer to <b>AvrcPlayerAttribute</b>.
     * @param[in] values     The value of the player application setting attribute.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int SetPlayerAppSettingCurrentValue(
        const RawAddress &rawAddr, const std::vector<uint8_t> &attributes, const std::vector<uint8_t> &values) = 0;

    /**
     * @brief  provide supported player application setting attribute displayable text.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int GetPlayerAppSettingAttributeText(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes) = 0;

    /**
     * @brief  request the target device to provide target supported player application setting value displayable text
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr     The address of the bluetooth device.
     * @param[in] attributeId Player application setting attribute ID.
     * @param[in] values      Player application setting value ID.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int GetPlayerAppSettingValueText(
        const RawAddress &rawAddr, uint8_t attributeId, const std::vector<uint8_t> &values) = 0;

    /******************************************************************
     * MEDIA INFORMATION                                              *
     ******************************************************************/

    /**
     * @brief Requests the TG to provide the attributes of the element specified in the parameter.
     *
     * @details Switch to the thread of the AVRCP CT service in this function.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] identifier Unique identifier to identify an element on TG
     * @param[in] attributes  Specifies the attributeID for the attributes to be retrieved
     *            @c RET_NO_ERROR   : The action is successful.
     *            @c RET_NO_SUPPORT : The action is not supported.
     *            @c RET_BAD_STATUS : The action is failed.
     */
    virtual int GetElementAttributes(
        const RawAddress &rawAddr, uint64_t identifier, const std::vector<uint32_t> &attributes) = 0;

    /******************************************************************
     * PLAY                                                           *
     ******************************************************************/

    /**
     * @brief Gets the play status.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int GetPlayStatus(const RawAddress &rawAddr) = 0;

    /**
     * @brief Starts playing an item indicated by the UID.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] scope      The scope in which media content navigation may take place. Refer to <b>AvrcMediaScope</b>.
     * @param[in] uid        The unique ID of media item.
     * @param[in] uidCounter The UID counter shall be incremented every time the TG makes an update.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int PlayItem(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter) = 0;

    /**
     * @brief Adds an item indicated by the UID to the Now Playing queue.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] scope      The scope in which media content navigation may take place. Refer to <b>AvrcMediaScope</b>.
     * @param[in] uid        The UID of the media element item or folder item.
     * @param[in] uidCounter The UID Counter.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int AddToNowPlaying(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter) = 0;

    /******************************************************************
     * CONTINUING RESPONSE / ABORT CONTINUING RESPONSE                *
     ******************************************************************/

    /**
     * @brief Requests continuing result.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pduId   The PDU ID which wants to request.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int RequestContinuingResponse(const RawAddress &rawAddr, uint8_t pduId) = 0;

    /**
     * @brief Aborts continuing result.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pduId   The PDU ID which wants to abort.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int AbortContinuingResponse(const RawAddress &rawAddr, uint8_t pduId) = 0;

    /******************************************************************
     * OPERATE THE VIRTUAL FILE SYSTEM                                *
     ******************************************************************/

    /**
     * @brief Navigates one level up or down in the virtual file system.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] uidCounter The value of the uid counter.
     * @param[in] direction  The flag of the navigation. Refer to <b>AvrcFolderDirection</b>.
     * @param[in] folderUid  The UID of the folder to navigate to. This may be retrieved via a GetFolderItems command.
     * If the navigation command is Folder Up this field is reserved.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int ChangePath(const RawAddress &rawAddr, uint16_t uidCounter, uint8_t direction, uint64_t folderUid) = 0;

    /**
     * @brief Retrieves a listing of the contents of a folder.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] scope      The scope in which media content navigation may take place. Refer to <b>AvrcMediaScope</b>.
     * @param[in] startItem  The offset within the listing of the item, which should be the first returned item. The
     * first element in the listing is at offset 0.
     * @param[in] endItem    The offset within the listing of the item which should be the final returned item. If this
     * is set to a value beyond what is available, the TG shall return items from the provided Start Item index to the
     * index of the final item. If the End Item index is smaller than the Start Item index, the TG shall return an
     * error. If CT requests too many items, TG can respond with a sub-set of the requested items.
     * @param[in] attributes The list of media attributes.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int GetFolderItems(const RawAddress &rawAddr, uint8_t scope, uint32_t startItem, uint32_t endItem,
        const std::vector<uint32_t> &attributes) = 0;

    /**
     * @brief Retrieves the metadata attributes for a particular media element item or folder item.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] scope      The scope in which media content navigation may take place. Refer to <b>AvrcMediaScope</b>.
     * @param[in] uid        The UID of the media element item or folder item.
     * @param[in] uidCounter The UID Counter.
     * @param[in] attributes The list of media attributes.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int GetItemAttributes(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter,
        const std::vector<uint32_t> &attributes) = 0;

    /**
     * @brief Gets the number of items in the selected folder at the selected scope.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] scope   The scope in which media content navigation may take place. Refer to <b>AvrcMediaScope</b>.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int GetTotalNumberOfItems(const RawAddress &rawAddr, uint8_t scope) = 0;

    /******************************************************************
     * ABSOLUTE VOLUME                                                *
     ******************************************************************/

    /**
     * @brief Sets an absolute volume to be used by the rendering device.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] volume  The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int SetAbsoluteVolume(const RawAddress &rawAddr, uint8_t volume) = 0;

    /******************************************************************
     * NOTIFICATION                                                   *
     ******************************************************************/

    /**
     * @brief Enables for receiving notifications asynchronously based on specific events occurring.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr  The address of the bluetooth device.
     * @param[in] events   The event for which the requires notification. Refer to <b>AvrcEventId</b>.
     * @param[in] interval The specifies the time interval (in seconds) at which the change in playback position will be
     * notified.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int EnableNotification(const RawAddress &rawAddr, const std::vector<uint8_t> &events, uint8_t interval) = 0;

    /**
     * @brief Disables for receiving notifications asynchronously based on specific events occurring.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] events  The event for which the requires notification. Refer to <b>AvrcEventId</b>.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    virtual int DisableNotification(const RawAddress &rawAddr, const std::vector<uint8_t> &events) = 0;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // INTERFACE_PROFILE_AVRCP_CT_H
