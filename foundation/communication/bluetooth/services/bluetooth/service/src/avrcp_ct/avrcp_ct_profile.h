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

#ifndef AVRCP_CT_PROFILE_H
#define AVRCP_CT_PROFILE_H

#include "avrcp_ct_internal.h"
#include "avrcp_ct_notification.h"
#include "avrcp_ct_state_machine.h"
#include "dispatcher.h"
/**
 * @brief The bluetooth subsystem.
 */
namespace OHOS {
namespace bluetooth {
/**
 * @brief This class provides a set of methods related to the role of the controller described in the Audio/Video Remote
 * Control profile.
 */
class AvrcCtProfile {
public:
    /**
     * @brief This struct provides a set of pointers for informing the state change.
     */
    struct Observer {
    public:
        /**
         * @brief Informs that the AVRCP TG profile is disabled.
         */
        std::function<void(void)> onDisabled;

        /**
         * @brief Informs that the connection state is changed.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] state   The connection state. Refer to <b>BTConnectState</b>.
         */
        std::function<void(const RawAddress &rawAddr, int state)> onConnectionStateChanged;

        /**
         * @brief Responds the data of the <b>PressButton</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] button  The code of the button released.
         * @param[in] result  The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t button, int result)> onButtonPressed;

        /**
         * @brief Responds the data of the <b>ReleaseButton</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] button  The code of the button released.
         * @param[in] result  The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t button, int result)> onButtonReleased;

        /**
         * @brief Responds the data of the <b>GetCapabilities</b>.
         *
         * @param[in] rawAddr      The address of the bluetooth device.
         * @param[in] companies    The list of CompanyId.
         * @param[in] events       The list of EventId.
         * @param[in] result    The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         */
        std::function<void(const RawAddress &rawAddr, const std::vector<uint32_t> &companies,
            const std::vector<uint8_t> &events, int result)>
            onGetCapabilities;

        /**
         * @brief Responds the data of the <b>ListPlayerApplicationSettingAttributes</b>.
         *
         * @param[in] rawAddr   The address of the bluetooth device.
         * @param[in] attribute The attribute of the player application setting. Refer to <b>AvrcPlayerAttribute</b>.
         * @param[in] result    The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         */
        std::function<void(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes, int result)>
            onListPlayerApplicationSettingAttributes;

        /**
         * @brief Responds the data of the <b>ListPlayerApplicationSettingValues</b>.
         *
         * @param[in] rawAddr   The address of the bluetooth device.
         * @param[in] attribute The attribute of the player application setting. Refer to <b>AvrcPlayerAttribute</b>.
         * @param[in] values    The values associated witch the attribute.
         * @param[in] result    The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         */
        std::function<void(
            const RawAddress &rawAddr, uint8_t attribute, const std::vector<uint8_t> &values, int result)>
            onListPlayerApplicationSettingValues;

        /**
         * @brief Responds the data of the <b>GetCurrentPlayerApplicationSettingValue</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] attributes The attribute of the player application settings. Refer to <b>AvrcPlayerAttribute</b>.
         * @param[in] values     The values associated witch the attribute.
         * @param[in] result     The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         */
        std::function<void(const RawAddress &rawAddr, const std::vector<uint8_t> &attribute,
            const std::vector<uint8_t> &values, int result)>
            onGetCurrentPlayerApplicationSettingValue;

        /**
         * @brief Responds the data of the <b>SetPlayerApplicationSettingValue</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] result  The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         */
        std::function<void(const RawAddress &rawAddr, int result)> onSetPlayerApplicationSettingValue;

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
         */
        std::function<void(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes,
            const std::vector<std::string> &attrStr, int result)>
            onGetPlayerApplicationSettingAttributeText;

        /**
         * @brief Responds the data of the <b>GetPlayerApplicationSettingValueText</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] values     Player application setting value ID.
         * @param[in] valueStr   Specifies the value string of player application setting value.
         * @param[in] result     The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         */
        std::function<void(const RawAddress &rawAddr, const std::vector<uint8_t> &values,
            const std::vector<std::string> &valueStr, int result)>
            onGetPlayerApplicationSettingValueText;

        /**
         * @brief Responds the data of the <b>GetElementAttributes</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] attributes The attribute of the player application setting.
         * @param[in] values     The values associated witch the attribute.
         * @param[in] result     The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         */
        std::function<void(const RawAddress &rawAddr, const std::vector<uint32_t> &attributes,
            const std::vector<std::string> &values, int result)>
            onGetElementAttributes;

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
         */
        std::function<void(
            const RawAddress &rawAddr, uint32_t songLength, uint32_t songPosition, uint8_t playStatus, int status)>
            onGetPlayStatus;

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
         */
        std::function<void(const RawAddress &rawAddr, int result, int detail)> onSetAddressedPlayer;

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
         */
        std::function<void(const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems,
            const std::vector<std::string> &folderNames, int result, int detail)>
            onSetBrowsedPlayer;

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
         */
        std::function<void(const RawAddress &rawAddr, uint32_t numOfItems, int result, int detail)> onChangePath;

        /**
         * @brief Responds the data of the <b>GetFolderItems</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] scope      The scope in which media content navigation may take place. Refer to
         * <b>AvrcMediaScope</b>.
         * @param[in] uidCounter The UID Counter.
         * @param[in] mpItems    The list of <b>AvrcCtMpItem</b> class.
         * @param[in] meItems    The list of <b>AvrcCtMeItem</b> class.
         * @param[in] result     The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         * @param[in] detail     The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
         *            @a AVRC_ES_CODE_NO_ERROR<br>
         *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
         *            @a AVRC_ES_CODE_UID_CHANGED<br>
         *            @a AVRC_ES_CODE_INVALID_SCOPE<br>
         *            @a AVRC_ES_CODE_RANGE_OUT_OF_BOUNDS<br>
         *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
         */
        std::function<void(const RawAddress &rawAddr, uint8_t scope, uint16_t uidCounter,
            const std::vector<AvrcMpItem> &mpItems, const std::vector<AvrcMeItem> &meItems, int result, int detail)>
            onGetFolderItems;

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
         */
        std::function<void(const RawAddress &rawAddr, const std::vector<uint32_t> &attributes,
            const std::vector<std::string> &values, int result, int detail)>
            onGetItemAttributes;

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
         */
        std::function<void(const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems, int result, int detail)>
            onGetTotalNumberOfItems;

        /**
         * @brief Informs that receive the command of the <b>PlayItem</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] status  The current status of playing. Refer to <b>PlayItem</b>
         * @param[in] result  The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         * @param[in] detail     The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
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
         */
        std::function<void(const RawAddress &rawAddr, int result, int detail)> onPlayItem;

        /**
         * @brief Responds the data of the <b>AddToNowPlaying</b>.
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
         */
        std::function<void(const RawAddress &rawAddr, int result, int detail)> onAddToNowPlaying;

        /**
         * @brief Responds the data of the <b>SetAbsoluteVolume</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] volume  The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
         * @param[in] result  The result of the execution.<br>
         *            @a RET_NO_ERROR   : Execute success.<br>
         *            @a RET_NO_SUPPORT : Not support.<br>
         *            @a RET_BAD_STATUS : Execute failure.
         */
        std::function<void(const RawAddress &rawAddr, uint8_t volume, int result)> onSetAbsoluteVolume;

        /**
         * @brief Responds the data of the notification <b>AVRC_CT_EVENT_ID_PLAYBACK_STATUS_CHANGED</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] playStatus The current status of playing. Refer to <b>AvrcPlayStatus</b>.
         * @param[in] result  The result of the execution.<br>
         *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
         *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
         */
        std::function<void(const RawAddress &rawAddr, uint8_t playStatus, int result)> onPlaybackStatusChanged;

        /**
         * @brief Responds the data of the notification <b>AVRC_CT_EVENT_ID_TRACK_CHANGED</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] uid     The unique ID of media item.
         * @param[in] result  The result of the execution.<br>
         *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
         *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
         */
        std::function<void(const RawAddress &rawAddr, uint64_t uid, int result)> onTrackChanged;

        /**
         * @brief Responds the data of the notification <b>AVRC_CT_EVENT_ID_TRACK_REACHED_END</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] result  The result of the execution.<br>
         *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
         *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
         */
        std::function<void(const RawAddress &rawAddr, int result)> onTrackReachedEnd;

        /**
         * @brief Responds the data of the notification <b>AVRC_CT_EVENT_ID_TRACK_REACHED_START</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] result  The result of the execution.<br>
         *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
         *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
         */
        std::function<void(const RawAddress &rawAddr, int result)> onTrackReachedStart;

        /**
         * @brief Responds the data of the notification <b>AVRC_CT_EVENT_ID_PLAYBACK_POS_CHANGED</b>.
         *
         * @param[in] rawAddr     The address of the bluetooth device.
         * @param[in] playbackPos Current playback position in millisecond.
         * @param[in] result  The result of the execution.<br>
         *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
         *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
         */
        std::function<void(const RawAddress &rawAddr, uint32_t playbackPos, int result)> onPlaybackPosChanged;

        /**
         * @brief Responds the data of the notification <b>AVRC_CT_EVENT_ID_PLAYER_APPLICATION_SETTING_CHANGED</b>.
         *
         * @param[in] rawAddr   The address of the bluetooth device.
         * @param[in] attribute The attribute of the player application setting. Refer to <b>AvrcPlayerAttribute</b>.
         * @param[in] values    The values associated witch the attribute.
         * @param[in] result  The result of the execution.<br>
         *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
         *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
         */
        std::function<void(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes,
            const std::vector<uint8_t> &values, int result)>
            onPlayerApplicationSettingChanged;

        /**
         * @brief Responds the data of the notification <b>AVRC_CT_EVENT_ID_NOW_PLAYING_CONTENT_CHANGED</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] result  The result of the execution.<br>
         *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
         *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
         */
        std::function<void(const RawAddress &rawAddr, int result)> onNowPlayingContentChanged;

        /**
         * @brief Responds the data of the notification <b>AVRC_CT_EVENT_ID_AVAILABLE_PLAYERS_CHANGED</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] result  The result of the execution.<br>
         *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
         *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
         */
        std::function<void(const RawAddress &rawAddr, int result)> onAvailablePlayersChanged;

        /**
         * @brief Responds the data of the notification <b>AVRC_CT_EVENT_ID_ADDRESSED_PLAYER_CHANGED</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] playerId   The unique media player id.
         * @param[in] uidCounter The UID counter shall be incremented every time the TG makes an update.
         * @param[in] result  The result of the execution.<br>
         *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
         *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
         */
        std::function<void(const RawAddress &rawAddr, uint16_t playerId, uint16_t uidCounter, int result)>
            onAddressedPlayerChanged;

        /**
         * @brief Responds the data of the notification <b>AVRC_CT_EVENT_ID_UIDS_CHANGED</b>.
         *
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] uidCounter The UID counter shall be incremented every time the TG makes an update.
         * @param[in] result  The result of the execution.<br>
         *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
         *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
         */
        std::function<void(const RawAddress &rawAddr, uint16_t uidCounter, int result)> onUidChanged;

        /**
         * @brief Responds the data of the notification <b>AVRC_CT_EVENT_ID_VOLUME_CHANGED</b>.
         *
         * @param[in] rawAddr The address of the bluetooth device.
         * @param[in] volume  The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
         * @param[in] result  The result of the execution.<br>
         *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
         *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
         */
        std::function<void(const RawAddress &rawAddr, uint8_t volume, int result)> onVolumeChanged;
    };

    /**
     * @brief A constructor used to create an <b>AvrcCtProfile</b> instance.
     */
    AvrcCtProfile(uint16_t features, uint32_t companyId, uint16_t controlMtu, uint16_t browseMtu, uint8_t maxFragments,
        utility::Dispatcher *dispatcher, AvctChannelEventCallback eventCallback, AvctMsgCallback msgCallback);

    /**
     * @brief A destructor used to delete the <b>AvrcCtProfile</b> instance.
     */
    ~AvrcCtProfile();

    /******************************************************************
     * REGISTER / UNREGISTER OBSERVER                                 *
     ******************************************************************/

    /**
     * @brief Registers the observer.
     *
     * @param observer The pointer to the instance of the <b>AvrcCtProfile::Observer</b> struct.
     */
    void RegisterObserver(AvrcCtProfile::Observer *observer);

    /**
     * @brief Unregisters the observer.
     */
    void UnregisterObserver(void);

    /******************************************************************
     * ENABLE / DISABLE                                               *
     ******************************************************************/

    /**
     * @brief Enables the AVRCP CT profile.
     *
     * @param[in] isTgEnabled The config of the TG is true or not.
     *
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int Enable(bool isTgEnabled);

    /**
     * @brief Disables the AVRCP CT profile.
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
     * @brief Disconnects the connection of the control channel.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int Disconnect(const RawAddress &rawAddr) const;

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

    /******************************************************************
     * PASS THROUGH COMMAND                                           *
     ******************************************************************/

    /**
     * @brief Sends the press command of the <b>PASS THROUGH</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] button  The value of the key operation. Refer to <b>AvrcKeyOperation</b>.
     */
    void SendPressButtonCmd(const RawAddress &rawAddr, uint8_t button);

    /**
     * @brief Sends the release command of the <b>PASS THROUGH</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] button  The value of the key operation. Refer to <b>AvrcKeyOperation</b>.
     */
    void SendReleaseButtonCmd(const RawAddress &rawAddr, uint8_t button);

    /**
     * @brief Gets the size of the <b>PASS THROUGH</b> command queue.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval true  The command queue is full.
     * @retval false The command queue is not full.
     */
    bool IsPassQueueFull(const RawAddress &rawAddr);

    /******************************************************************
     * UNIT INFO / SUB UNIT INFO COMMAND                              *
     ******************************************************************/

    /**
     * @brief Sends the command of the <b>UNIT INFO</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void SendUnitCmd(const RawAddress &rawAddr);

    /**
     * @brief Sends the command of the <b>SUB UNIT INFO</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void SendSubUnitCmd(const RawAddress &rawAddr);

    /**
     * @brief Gets the size of the <b>UNIT INFO</b> and <b>SUB UNIT INFO</b> command queue.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval true  The command queue is full.
     * @retval false The command queue is not full.
     */
    static bool IsUnitQueueFull(const RawAddress &rawAddr);

    /******************************************************************
     * VENDOR COMMAND                                                 *
     ******************************************************************/

    /**
     * @brief Sends the command of the <b>GetCapabilities</b>.
     *
     * @details This is sent by CT to get the capabilities of the peer device
     *
     * @param[in] rawAddr      The address of the bluetooth device.
     * @param[in] capabilityId Specific capability requested
     */
    void SendGetCapabilitiesCmd(const RawAddress &rawAddr, uint8_t capabilityId);

    /**
     * @brief Sends the command of the <b>ListPlayerApplicationSettingAttributes</b>.
     *
     * @param[in] rawAddr   The address of the bluetooth device.
     */
    void SendListPlayerApplicationSettingAttributesCmd(const RawAddress &rawAddr);

    /**
     * @brief Sends the command of the <b>ListPlayerApplicationSettingValues</b>.
     *
     * @param[in] rawAddr   The address of the bluetooth device.
     * @param[in] attribute The attribute of the player application setting.
     */
    void SendListPlayerApplicationSettingValuesCmd(const RawAddress &rawAddr, uint8_t attribute);

    /**
     * @brief Sends the command of the <b>GetCurrentPlayerApplicationSettingValue</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings.
     */
    void SendGetCurrentPlayerApplicationSettingValueCmd(
        const RawAddress &rawAddr, const std::vector<uint8_t> &attributes);

    /**
     * @brief Sends the command of the <b>SetPlayerApplicationSettingValue</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings.
     * @param[in] values     The value of the player application setting attributes.
     */
    void SendSetPlayerApplicationSettingValueCmd(
        const RawAddress &rawAddr, const std::vector<uint8_t> &attributes, const std::vector<uint8_t> &values);

    /**
     * @brief Sends the command of the <b>GetPlayerApplicationSettingAttributeText</b>.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings.
     */
    void SendGetPlayerApplicationSettingAttributeTextCmd(
        const RawAddress &rawAddr, const std::vector<uint8_t> &attributes);

    /**
     * @brief Sends the command of the <b>GetPlayerApplicationSettingValueText</b>.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr     The address of the bluetooth device.
     * @param[in] attributeId Player application setting attribute ID.
     * @param[in] values      Player application setting value ID.
     */
    void SendGetPlayerApplicationSettingValueTextCmd(
        const RawAddress &rawAddr, uint8_t attributeId, const std::vector<uint8_t> &values);

    /**
     * @brief Sends the command of the <b>GetElementAttributes</b>.
     *
     * @details Switch to the thread of the AVRCP CT service in this function.
     * @param[in] rawAddr    The address of the attribute.
     * @param[in] identifier Unique identifier to identify an element on TG
     * @param[in] attributes Specifies the attribute ID for the attributes to be retrieved
     */
    void SendGetElementAttributesCmd(
        const RawAddress &rawAddr, uint64_t identifier, const std::vector<uint32_t> &attributes);

    /**
     * @brief Sends the command of the <b>GetPlayStatus</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void SendGetPlayStatusCmd(const RawAddress &rawAddr);

    /**
     * @brief Sends the command of the <b>RequestContinuingResponse</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pduId   The PDU ID which wants to request.
     */
    void SendRequestContinuingResponseCmd(const RawAddress &rawAddr, uint8_t pduId);

    /**
     * @brief Sends the command of the <b>AbortContinuingResponse</b>.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pduId   The PDU ID which wants to abort.
     */
    void SendAbortContinuingResponseCmd(const RawAddress &rawAddr, uint8_t pduId);

    /**
     * @brief Sends the command of the <b>SetAddressedPlayer</b>.
     *
     * @param[in] rawAddr  The address of the bluetooth device.
     * @param[in] playerId The unique media player id.
     */
    void SendSetAddressedPlayerCmd(const RawAddress &rawAddr, uint16_t playerId);

    /**
     * @brief Sends the command of the <b>PlayItem</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] scope      The scope in which media content navigation may take place. Refer to <b>AvrcMediaScope</b>.
     * @param[in] uid        The unique ID of media item.
     * @param[in] uidCounter The UID counter shall be incremented every time the TG makes an update.
     */
    void SendPlayItemCmd(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter);

    /**
     * @brief Sends the command of the <b>AddToNowPlaying</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] scope      The scope in which media content navigation may take place. Refer to <b>AvrcMediaScope</b>.
     * @param[in] uid        The UID of the media element item or folder item.
     * @param[in] uidCounter The UID Counter.
     */
    void SendAddToNowPlayingCmd(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter);

    /**
     * @brief Sends the command of the <b>SetAbsoluteVolume</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] volume  The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
     */
    void SendSetAbsoluteVolumeCmd(const RawAddress &rawAddr, uint8_t volume);

    /**
     * @brief Sends the command of the <b>RegisterNotification</b>.
     *
     * @param[in] rawAddr  The address of the bluetooth device.
     * @param[in] eventId  The event for which the requires notification. Refer to <b>events</b>.
     * @param[in] interval The specifies the time interval (in seconds) at which the change in playback position will be
     * notified.
     */
    void EnableNotification(const RawAddress &rawAddr, const std::vector<uint8_t> &events,
        uint32_t interval = AVRC_PLAYBACK_INTERVAL_1_SEC);

    /**
     * @brief Sends the command of the <b>RegisterNotification</b>.
     *
     * @param[in] rawAddr  The address of the bluetooth device.
     * @param[in] events   The event for which the requires notification. Refer to <b>events</b>.
     */
    static void DisableNotification(const RawAddress &rawAddr, const std::vector<uint8_t> &events);

    /**
     * @brief Gets the size of the <b>VENDOR DEPENDENT</b> command queue.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval true  The command queue is full.
     * @retval false The command queue is not full.
     */
    bool IsVendorQueueFull(const RawAddress &rawAddr);

    /******************************************************************
     * BROWSING COMMAND                                               *
     ******************************************************************/

    /**
     * @brief Sends the command of the <b>SetBrowsedPlayer</b>.
     *
     * @param[in] rawAddr  The address of the bluetooth device.
     * @param[in] playerId The unique media player id.
     */
    void SendSetBrowsedPlayerCmd(const RawAddress &rawAddr, uint16_t playerId);

    /**
     * @brief Sends the command of the <b>ChangePath</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] uidCounter The value of the uid counter.
     * @param[in] direction  The flag of the navigation. Refer to <b>AvrcFolderDirection</b>.
     * @param[in] folderUid  The UID of the folder to navigate to. This may be retrieved via a GetFolderItems command.
     * If the navigation command is Folder Up this field is reserved.
     */
    void SendChangePathCmd(const RawAddress &rawAddr, uint16_t uidCounter, uint8_t direction, uint64_t folderUid);

    /**
     * @brief Sends the command of the <b>GetFolderItems</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] scope      The scope in which media content navigation may take place. Refer to <b>AvrcMediaScope</b>.
     * @param[in] startItem  The offset within the listing of the item, which should be the first returned item. The
     * first element in the listing is at offset 0.
     * @param[in] endItem    The offset within the listing of the item which should be the final returned item. If this
     * is set to a value beyond what is available, the TG shall return items from the provided Start Item index to the
     * index of the final item. If the End Item index is smaller than the Start Item index, the TG shall return an
     * error. If CT requests too many items, TG can respond with a sub-set of the requested items.
     * @param[in] attributes The list of media attributes.
     */
    void SendGetFolderItemsCmd(const RawAddress &rawAddr, uint8_t scope, uint32_t startItem, uint32_t endItem,
        const std::vector<uint32_t> &attributes);

    /**
     * @brief Sends the command of the <b>GetItemAttributes</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] scope      The scope in which media content navigation may take place. Refer to <b>AvrcMediaScope</b>.
     * @param[in] uid        The UID of the media element item or folder item.
     * @param[in] uidCounter The UID Counter.
     * @param[in] attributes The list of media attributes.
     */
    void SendGetItemAttributesCmd(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter,
        const std::vector<uint32_t> &attributes);

    /**
     * @brief Sends the command of the <b>GetTotalNumberOfItems</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] scope   The scope in which media content navigation may take place. Refer to <b>AvrcMediaScope</b>.
     */
    void SendGetTotalNumberOfItemsCmd(const RawAddress &rawAddr, uint8_t scope);

    /**
     * @brief Gets the size of the <b>BROWSING</b> command queue.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval true  The command queue is full.
     * @retval false The command queue is not full.
     */
    bool IsBrowseQueueFull(const RawAddress &rawAddr);

    /**
     * @brief Whether peerAddr support absolute volume command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of compatibility.
     */
    bool IsDisableAbsoluteVolume(const RawAddress &rawAddr);

    /**
     * @brief Whether the browse channel connected.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result.
     */
    bool IsBrowsingConnected(const RawAddress &rawAddr);

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
        const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context) const;

    void ProcessChannelEventConnectIndEvt(
        const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context) const;
    void ProcessChannelEventConnectCfmEvt(
        const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context) const;
    void ProcessChannelEventDisconnectIndEvt(
        const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context) const;
    void ProcessChannelEventDisconnectCfmEvt(
        const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context) const;

    void ProcessChannelEventBrConnectIndEvt(
        const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context) const;
    void ProcessChannelEventBrConnectCfmEvt(
        const RawAddress &rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context) const;

    /**
     * @brief Processes messages received from the AVCTP.
     *
     * @param[in] connectId The ID of the connection.
     * @param[in] crType    The type used to distinguish between the command frame and the response frame.
     * @param[in] chType    The type used to distinguish the message returned from the control channel or the browse
     * channel.
     * @param[in] pkt       The message sent by the peer bluetooth device.
     * @param[in] context   The context is used to send the message in the callback.
     */
    void ProcessChannelMessage(
        uint8_t connectId, uint8_t label, uint8_t crType, uint8_t chType, Packet *pkt, void *context);

private:
    /// The flag is used to indicate that the AVRCP CT profile is enabled or not.
    static bool g_isEnabled;
    /// The features supported by the AVRCP CT profile.
    uint32_t features_;
    /// The SIG company ID.
    uint32_t companyId_;
    /// The max MTU size of the control channel.
    uint16_t controlMtu_;
    /// The max MTU size of the browse channel.
    uint16_t browseMtu_;
    /// The max fragments of the control channel.
    uint8_t maxFragments_;
    /// The dispatcher that is used to switch to the thread of the AVRCP CT service.
    utility::Dispatcher *dispatcher_ {nullptr};
    /// The connect id of the passive connection.
    uint8_t connectId_;
    /// The pointer to the observer of the <b>AvrcCtProfile</b> class.
    AvrcCtProfile::Observer *myObserver_ {nullptr};
    /// The callback function, which registers into the AVCTP for receiving the events.
    AvctChannelEventCallback eventCallback_ {nullptr};
    /// The callback function, which registers into the AVCTP for receiving the messages.
    AvctMsgCallback msgCallback_ {nullptr};
    // Locks the local variable in a multi-threaded environment.
    std::recursive_mutex mutex_ {};

    /**
     * @brief A deleted default constructor.
     */
    AvrcCtProfile() = delete;

    /******************************************************************
     * ENABLE / DISABLE                                               *
     ******************************************************************/

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
     * @brief Sends the command of the <b>PASS THROUGH</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The reference of the shared pointer to the instance of the <b>AvrcCtPassPacket</b> class.
     */
    void SendPassCmd(const RawAddress &rawAddr, const std::shared_ptr<AvrcCtPassPacket> &pkt);

    /**
     * @brief Sends the next command of the <b>PASS THROUGH</b> in the cache queue.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void SendNextPassCmd(const RawAddress &rawAddr);

    /**
     * @brief Receives the response of the <b>PASS THROUGH</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceivePassRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Informs that revive the button operation.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] button  The value of the key operation.
     * @param[in] state   The value of the key state.
     * @param[in] result  The result of the method execution.
     *            @a RET_NO_ERROR   : Execute success.
     *            @a RET_NO_SUPPORT : Not support.
     *            @a RET_BAD_STATUS : Execute failure.
     */
    void InformPassRsp(const RawAddress &rawAddr, uint8_t button, uint8_t state, int result) const;

    /**
     * @brief Processes the timeout of command of the <b>PASS THROUGH</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void ProcessPassTimeout(RawAddress rawAddr, uint8_t key, uint8_t state);

    /**
     * @brief The callback function, which registers into the <b>utility::Timer</b>.
     *
     * @details This function switches to the thread of the AVRCP TG service firstly, then processes the timeout.
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void PassTimeoutCallback(const RawAddress &rawAddr, uint8_t key, uint8_t state);

    /******************************************************************
     * UNIT INFO / SUB UNIT INFO COMMAND                              *
     ******************************************************************/

    /**
     * @brief Sends the command of the <b>UNIT INFO</b> or <b>SUB UNIT INFO</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void SendNextUnitCmd(const RawAddress &rawAddr);

    /**
     * @brief Receives the response of the <b>UNIT INFO</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveUnitRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Receives the response of the <b>SUB UNIT INFO</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveSubUnitRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Processes the timeout of command of the <b>UNIT INFO</b> or <b>SUB UNIT INFO</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void ProcessUnitTimeout(RawAddress rawAddr);

    /**
     * @brief The callback function, which registers into the <b>utility::Timer</b>.
     *
     * @details This function switches to the thread of the AVRCP TG service firstly, then processes the timeout.
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void UnitTimeoutCallback(const RawAddress &rawAddr);

    /******************************************************************
     * VENDOR COMMAND                                                 *
     ******************************************************************/

    /**
     * @brief Sends the command of the <b>VENDOR DEPENDENT</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void SendVendorCmd(const RawAddress &rawAddr, const std::shared_ptr<AvrcCtVendorPacket> &pkt, AvrcCtSmEvent event);

    /**
     * @brief Sends the next command of the <b>VENDOR DEPENDENT</b> in the cache queue.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void SendNextVendorCmd(const RawAddress &rawAddr);

    /**
     * @brief Sends the command of the <b>RequestContinuingResponse</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pduId   The unique ID of the command.
     */
    void SendVendorContinueCmd(const RawAddress &rawAddr, uint8_t pduId);

    /**
     * @brief Sends the command of the <b>AbortContinuingResponse</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pduId   The unique ID of the command.
     */
    void SendVendorAbortCmd(const RawAddress &rawAddr, uint8_t pduId) const;

    /**
     * @brief Receives the response of the <b>GetCapabilities</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveGetCapabilitiesRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Receives the response of the <b>ListPlayerApplicationSettingAttributes</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveListPlayerApplicationSettingAttributesRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Receives the response of the <b>ListPlayerApplicationSettingValues</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveListPlayerApplicationSettingValuesRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Receives the response of the <b>GetCurrentPlayerApplicationSettingValue</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveGetCurrentPlayerApplicationSettingValueRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Receives the response of the <b>SetPlayerApplicationSettingValue</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveSetPlayerApplicationSettingValueRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Receives the response of the <b>GetPlayerApplicationSettingAttributeText</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveGetPlayerApplicationSettingAttributeTextRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Receives the response of the <b>GetPlayerApplicationSettingValueText</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveGetPlayerApplicationSettingValueTextRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Receives the response of the <b>GetElementAttributes</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The packet of the frame.
     */
    void ReceiveGetElementAttributesRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Receives the response of the <b>GetPlayStatus</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The packet of the frame.
     */
    void ReceiveGetPlayStatusRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Receives the response of the <b>AbortContinuingResponse</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveAbortContinuingResponseRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Receives the response of the <b>SetAddressedPlayer</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The packet of the frame.
     */
    void ReceiveSetAddressedPlayerRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Receives the response of the <b>PlayItem</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceivePlayItemRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Receives the response of the <b>AddToNowPlaying</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The packet of the frame.
     */
    void ReceiveAddToNowPlayingRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Receives the response of the <b>SetAbsoluteVolume</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The packet of the frame.
     */
    void ReceiveSetAbsoluteVolumeRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Receives the response of the <b>RegisterNotification</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The packet of the frame.
     */
    void ReceiveRegisterNotificationRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Inform the notification changed.
     *
     * @param[in] event The event of the notification.
     */
    void InformNotificationChanged(
        const RawAddress &rawAddr, const std::shared_ptr<AvrcCtNotifyPacket> &notifyPkt, int result);

    /**
     * @brief Inform the notification changed.
     *
     * @param[in] event The event of the notification.
     */
    void InformPlayerApplicationSettingChanged(
        const RawAddress &rawAddr, const std::shared_ptr<AvrcCtNotifyPacket> &notifyPkt, int result);

    /**
     * @brief Receives the response of the <b>VENDOR DEPENDENT AV/C Status</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveVendorRspAvcStatus(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Receives the response of the <b>VENDOR DEPENDENT AV/C Control</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveVendorRspAvcControl(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Receives the response of the <b>VENDOR DEPENDENT</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveVendorRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Processes the timeout of notification command of the <b>VENDOR DEPENDENT</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void ProcessVendorNotificationTimeout(RawAddress rawAddr);

    /**
     * @brief Processes the timeout of command of the <b>VENDOR DEPENDENT AV/C Control</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] packet The packet of the AV/C Control
     */
    void ProcessVendorAvcControlTimeout(RawAddress rawAddr, const std::shared_ptr<AvrcCtVendorPacket> &packet);

    /**
     * @brief Processes the timeout of command of the <b>VENDOR DEPENDENT AV/C Status</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] packet The packet of the AV/C Status1
     */
    void ProcessVendorAvcStatus1Timeout(RawAddress rawAddr, const std::shared_ptr<AvrcCtVendorPacket> &packet);

    /**
     * @brief Processes the timeout of command of the <b>VENDOR DEPENDENT AV/C Status</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] packet The packet of the AV/C Status2
     */
    void ProcessVendorAvcStatus2Timeout(RawAddress rawAddr, const std::shared_ptr<AvrcCtVendorPacket> &packet);

    /**
     * @brief Processes the timeout of command of the <b>VENDOR DEPENDENT</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void ProcessVendorTimeout(RawAddress rawAddr);

    /**
     * @brief The callback function, which registers into the <b>utility::Timer</b>.
     *
     * @details This function switches to the thread of the AVRCP CT service firstly, then processes the timeout.
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void VendorTimeoutCallback(const RawAddress &rawAddr);

    /******************************************************************
     * BROWSING COMMAND                                               *
     ******************************************************************/

    /**
     * @brief Sends the BROWSING command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void SendBrowseCmd(const RawAddress &rawAddr, const std::shared_ptr<AvrcCtBrowsePacket> &pkt, AvrcCtSmEvent event);

    /**
     * @brief Sends the next BROWSING command saved in the cache queue.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void SendNextBrowseCmd(const RawAddress &rawAddr);

    /**
     * @brief Receives the response of the "SetBrowsedPlayer".
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveSetBrowsedPlayerRsp(const RawAddress &rawAddr, Packet *pkt) const;

    /**
     * @brief Receives the response of the "ChangePath".
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveChangePathRsp(const RawAddress &rawAddr, Packet *pkt) const;

    /**
     * @brief Receives the response of the "GetFolderItems".
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveGetFolderItemsRsp(const RawAddress &rawAddr, Packet *pkt) const;

    /**
     * @brief Receives the response of the "GetItemAttributes".
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveGetItemAttributesRsp(const RawAddress &rawAddr, Packet *pkt) const;

    /**
     * @brief Receives the response of the "GetTotalNumberOfItems".
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveGetTotalNumberOfItemsRsp(const RawAddress &rawAddr, Packet *pkt) const;

    /**
     * @brief Receives the response of the <b>BROWSING</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void ReceiveBrowseRsp(const RawAddress &rawAddr, Packet *pkt);

    /**
     * @brief Processes the timeout of command of the <b>BROWSING</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void ProcessBrowseTimeout(RawAddress rawAddr);

    /**
     * @brief The callback function, which registers into the <b>utility::Timer</b>.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void BrowseTimeoutCallback(const RawAddress &rawAddr);

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
        return ((features_ & AVRC_CT_FEATURE_BROWSING) == AVRC_CT_FEATURE_BROWSING);
    }

    /**
     * @brief Explains the response of the <b>AVCTP</b> function to the result.
     *
     * @param avctRet The response code.
     * @return The requested result.
     */
    static int ExpainAvctResult(uint16_t avctRet);

    /**
     * @brief Explains the response of the <b>PASS THROUGH</b> command to the result.
     *
     * @param code The response code.
     * @return The requested result.
     */
    static int ExpainPassCrCodeToResult(uint8_t code);

    /**
     * @briefs Explains the "crCode" of the <b>STATUS</b> command to the response.
     *
     * @return The value of the "response".
     */
    static int ExplainCrCodeToResult(uint8_t crCode);

    /**
     * @briefs Get the name of the event
     *
     * @param event The event
     * @return The name of the event
     */
    static std::string GetEventName(uint8_t event);

    BT_DISALLOW_COPY_AND_ASSIGN(AvrcCtProfile);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_CT_PROFILE_H
