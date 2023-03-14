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

#ifndef AVRCP_TG_SERVICE_H
#define AVRCP_TG_SERVICE_H

#include <atomic>
#include <deque>
#include "avrcp_tg_gap.h"
#include "avrcp_tg_internal.h"
#include "avrcp_tg_profile.h"
#include "avrcp_tg_sdp.h"
#include "bt_def.h"
#include "context.h"
#include "interface_profile_avrcp_tg.h"
#include "raw_address.h"
#include "stub/media_service.h"
#ifdef AVRCP_AVSESSION
#include "avsession_manager.h"
#include "avsession_errors.h"
#endif
namespace OHOS {
namespace bluetooth {
/**
 * @brief This enumeration declares the states of the AVRCP TG service.
 */
enum AvrcTgServiceState : uint8_t {
    AVRC_TG_SERVICE_STATE_INVALID = 0x00,
    AVRC_TG_SERVICE_STATE_ENABLING,
    AVRC_TG_SERVICE_STATE_ENABLED,
    AVRC_TG_SERVICE_STATE_DISABLING,
    AVRC_TG_SERVICE_STATE_DISABLED,
};

/**
 * @brief This class provides a set of methods related to the role of the target described in the Audio/Video Remote
 * Control profile.
 */
class AvrcpTgService : public IProfileAvrcpTg, public utility::Context {
public:
    /**
     * @brief This class implement the <b>stub::MediaService::IObserver</b> interface for observing the state change.
     */
    class ObserverImpl : public stub::MediaService::IObserver {
    public:
        /**
         * @brief A constructor used to create an <b>ObserverImpl</b> instance.
         */
        ObserverImpl() = default;

        /**
         * @brief A destructor used to delete the <b>ObserverImpl</b> instance.
         */
        ~ObserverImpl() = default;

        void OnConnectionStateChanged(const std::string &addr, int state) override;
        void OnPressButton(const std::string &addr, uint8_t button) override;
        void OnReleaseButton(const std::string &addr, uint8_t button) override;

        /**
         * @brief Responds the data of the <b>SetAddressedPlayer</b>.
         *
         * @param[in] addr    The address of the bluetooth device.
         * @param[in] label   The label which is used to distinguish different call.
         * @param[in] status  The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
         *            @a AVRC_ES_CODE_NO_ERROR<br>
         *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
         *            @a AVRC_ES_CODE_UID_CHANGED<br>
         *            @a AVRC_ES_CODE_INVALID_PLAYER_ID<br>
         *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
         */
        void OnSetAddressedPlayer(const std::string &addr, uint8_t label, int status) override;

        /**
         * @brief Responds the data of the <b>SetBrowsedPlayer</b>.
         *
         * @param[in] addr        The address of the bluetooth device.
         * @param[in] uidCounter  The value of the uid counter.
         * @param[in] numOfItems  The number of items in the directory.
         * @param[in] folderNames The current browsed path of the player. The first element in folderNames is the parent
         * folder. The root folder has no name. The names are in UTF-8.
         * @param[in] label       The label which is used to distinguish different call.
         * @param[in] status      The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
         *            @a AVRC_ES_CODE_NO_ERROR<br>
         *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
         *            @a AVRC_ES_CODE_UID_CHANGED<br>
         *            @a AVRC_ES_CODE_INVALID_PLAYER_ID<br>
         *            @a AVRC_ES_CODE_PLAYER_NOT_BROWSABLE<br>
         *            @a AVRC_ES_CODE_PLAYER_NOT_ADDRESSED<br>
         *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
         */
        void OnSetBrowsedPlayer(const std::string &addr, uint16_t uidCounter, uint32_t numOfItems,
            const std::vector<std::string> &folderNames, uint8_t label, int status) override;

        /**
         * @brief Responds the data of the <b>OnGetPlayerAppSettingAttributes</b>.
         *
         * @param[in] addr   The address of the bluetooth device.
         * @param[in] The attribute of the player application settings.
         * @param[in] label  The label which is used to distinguish different call.
         */
        void OnGetPlayerAppSettingAttributes(
            const std::string &addr, const std::deque<uint8_t> &attributes, uint8_t label) override;

        /**
         * @brief Responds the data of the <b>GetPlayerAppSettingValues</b>.
         *
         * @param[in] addr   The address of the bluetooth device.
         * @param[in] values The values associated witch the attribute.
         * @param[in] label  The label which is used to distinguish different call.
         */
        void OnGetPlayerAppSettingValues(
            const std::string &addr, const std::deque<uint8_t> &values, uint8_t label) override;

        /**
         * @brief Responds the data of the <b>GetCurrentPlayerAppSettingValue</b>.
         *
         * @param[in] addr       The address of the bluetooth device.
         * @param[in] attributes The attribute of the player application settings. Refer to <b>AvrcPlayerAttribute</b>.
         * @param[in] values     The values associated witch the attribute.
         * @param[in] label      The label which is used to distinguish different call.
         * @param[in] context    The context which is used to distinguish different purpose.
         */
        void OnGetPlayerAppSettingCurrentValue(const std::string &addr, const std::deque<uint8_t> &attributes,
            const std::deque<uint8_t> &values, uint8_t label, uint8_t context) override;

        /**
         * @brief Responds the data of the <b>SetSetPlayerAppSettingCurrentValue</b>.
         *
         * @param[in] addr  The address of the bluetooth device.
         * @param[in] label The label which is used to distinguish different call.
         */
        void OnSetPlayerAppSettingCurrentValue(const std::string &addr, uint8_t label) override;

        /**
         * @brief Responds the data of the <b>GetPlayerAppSettingAttributeTest</b>.
         *
         * @param[in] addr       The address of the bluetooth device.
         * @param[in] attributes The attribute of the player application settings.
         * @param[in] attrStr    The values associated witch the attribute.
         * @param[in] label      The label which is used to distinguish different call.
         */
        void OnGetPlayerAppSettingAttributeText(const std::string &addr, const std::vector<uint8_t> &attributes,
            const std::vector<std::string> &attrStr, uint8_t label) override;

        /**
         * @brief Responds the data of the <b>GetPlayerAppSettingValueText</b>.
         *
         * @param[in] addr       The address of the bluetooth device.
         * @param[in] values     Player application setting value ID.
         * @param[in] valueStr   Specifies the value string of player application setting value.
         * @param[in] label      The label which is used to distinguish different call.
         */
        void OnGetPlayerAppSettingValueText(const std::string &addr, const std::vector<uint8_t> &values,
            const std::vector<std::string> &valueStr, uint8_t label) override;

        /**
         * @brief Responds the data of the <b>GetElementAttributes</b>.
         *
         * @details Switch to the thread of the AVRCP CT service in this function.
         * @param[in] rawAddr    The address of the bluetooth device.
         * @param[in] attributes  Specifies the attribute ID for the attributes to be retrieved.
         * @param[in] values     The list of the value of this attribute.
         * @param[in] label      The label which is used to distinguish different call.
         */
        void OnGetElementAttributes(const std::string &addr, const std::vector<uint32_t> &attribtues,
            const std::vector<std::string> &values, uint8_t label) override;

        /**
         * @brief Responds the data of the <b>GetPlayStatus</b>.
         *
         * @param[in] addr         The address of the bluetooth device.
         * @param[in] songLength   The total length of the playing song in milliseconds.
         * @param[in] songPosition The current position of the playing in milliseconds elapsed.
         * @param[in] playStatus   The current status of playing. Refer to <b>bluetooth::AvrcPlayStatus</b>.
         * @param[in] label        The label which is used to distinguish different call.
         * @param[in] context      The context which is used to distinguish different purpose.
         */
        void OnGetPlayStatus(const std::string &addr, uint32_t songLength, uint32_t songPosition, uint8_t playStatus,
            uint8_t label, uint8_t context) override;

        /**
         * @brief Responds the data of the <b>PlayItem</b>.
         *
         * @param[in] addr    The address of the bluetooth device.
         * @param[in] label   The label which is used to distinguish different call.
         * @param[in] status  The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
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
        void OnPlayItem(const std::string &addr, uint8_t label, int status) override;

        /**
         * @brief Responds the data of the <b>AddToNowPlaying</b>.
         *
         * @param[in] addr    The address of the bluetooth device.
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
         */
        void OnAddToNowPlaying(const std::string &addr, uint8_t label, int status) override;

        /**
         * @brief Responds the data of the <b>ChangePath</b>.
         *
         * @param[in] addr       The address of the bluetooth device.
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
        void OnChangePath(const std::string &addr, uint32_t numOfItems, uint8_t label, int status) override;

        /**
         * @brief Responds the data of the <b>GetFolderItems</b>.
         *
         * @param[in] addr       The address of the bluetooth device.
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
         */
        void OnGetMediaPlayers(const std::string &addr, uint16_t uidCounter,
            const std::vector<stub::MediaService::MediaPlayer> &items, uint8_t label, int status) override;

        /**
         * @brief Responds the data of the <b>GetFolderItems</b>.
         *
         * @param[in] addr       The address of the bluetooth device.
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
         */
        void OnGetFolderItems(const std::string &addr, uint16_t uidCounter,
            const std::vector<stub::MediaService::MediaItem> &items, uint8_t label, int status) override;

        /**
         * @brief Responds the data of the <b>GetItemAttributes</b>.
         *
         * @param[in] addr       The address of the bluetooth device.
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
        void OnGetItemAttributes(const std::string &addr, const std::vector<uint32_t> &attributes,
            const std::vector<std::string> &values, uint8_t label, int status) override;

        /**
         * @brief Responds the data of the <b>GetTotalNumberOfItems</b>.
         *
         * @param[in] addr       The address of the bluetooth device.
         * @param[in] uidCounter The value of the uid counter.
         * @param[in] numOfItems The number of items in the directory.
         * @param[in] label      The label which is used to distinguish different call.
         * @param[in] status     The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
         *            @a AVRC_ES_CODE_NO_ERROR<br>
         *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
         *            @a AVRC_ES_CODE_UID_CHANGED<br>
         *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
         */
        void OnGetTotalNumberOfItems(
            const std::string &addr, uint16_t uidCounter, uint32_t numOfItems, uint8_t label, int status) override;

        /**
         * @brief Responds the data of the <b>SetAbsoluteVolume</b>.
         *
         * @param[in] addr   The address of the bluetooth device.
         * @param[in] volume The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
         * @param[in] label  The label which is used to distinguish different call.
         */
        void OnSetAbsoluteVolume(const std::string &addr, uint8_t volume, uint8_t label) override;

        void OnGetSelectedTrack(const std::string &addr, uint64_t uid, uint8_t label) override;

        void OnGetAddressedPlayer(
            const std::string &addr, uint16_t playerId, uint16_t uidCounter, uint8_t label) override;

        void OnGetUidCounter(const std::string &addr, uint16_t uidCounter, uint8_t label) override;

        void OnGetCurrentAbsoluteVolume(const std::string &addr, uint8_t volume, uint8_t label) override;

    private:
        IProfileAvrcpTg *GetService(void);
    };
#ifdef AVRCP_AVSESSION
     /**
     * @brief This class implement the <b>SessionListender</b> interface for observing the state change.
     */
    class AVSessionObserverImpl : public OHOS::AVSession::SessionListener {
    public:
        /**
         * @brief A constructor used to create an <b>AVSessionObserverImpl</b> instance.
         */
        AVSessionObserverImpl() = default;

        /**
         * @brief A destructor used to delete the <b>AVSessionObserverImpl</b> instance.
         */
        ~AVSessionObserverImpl() = default;

        void OnSessionCreate(const OHOS::AVSession::AVSessionDescriptor& descriptor) override;
        void OnSessionRelease(const OHOS::AVSession::AVSessionDescriptor& descriptor) override;
        void OnTopSessionChange(const OHOS::AVSession::AVSessionDescriptor& descriptor) override;
    private:
        IProfileAvrcpTg *GetService(void);
    };

     /**
     * @brief This class implement the <b>SessionListender</b> interface for observing the state change.
     */
    class AVControllerObserverImpl : public OHOS::AVSession::AVControllerCallback {
    public:
        /**
         * @brief A constructor used to create an <b>AVControllerObserverImpl</b> instance.
         */
        AVControllerObserverImpl() = default;

        /**
         * @brief A destructor used to delete the <b>AVControllerObserverImpl</b> instance.
         */
        ~AVControllerObserverImpl() = default;

        void OnSessionDestroy() override;
        void OnPlaybackStateChange(const OHOS::AVSession::AVPlaybackState &state) override;
        void OnMetaDataChange(const OHOS::AVSession::AVMetaData &data) override;
        void OnActiveStateChange(bool isActive) override;
        void OnValidCommandChange(const std::vector<int32_t> &cmds) override;
        void OnOutputDeviceChange(const OHOS::AVSession::OutputDeviceInfo &outputDeviceInfo) override {};
    private:
        IProfileAvrcpTg *GetService(void);
    };
#endif
    /**
     * @brief A constructor used to create an <b>AvrcpTgService</b> instance.
     */
    AvrcpTgService();

    /**
     * @brief A destructor used to delete the <b>AvrcpTgService</b> instance.
     */
    ~AvrcpTgService();

    utility::Context *GetContext() override;

    /******************************************************************
     * REGISTER / UNREGISTER OBSERVER                                 *
     ******************************************************************/

    /**
     * @brief Registers the observer.
     *
     * @param[in] observer The pointer to the instance of the <b>AvrcTgProfile::IObserver</b>.
     */
    void RegisterObserver(IObserver *observer) override;

    /**
     * @brief Unregisters the observer.
     */
    void UnregisterObserver(void) override;

    /******************************************************************
     * ENABLE / DISABLE                                               *
     ******************************************************************/

    /**
     * @brief Enables the AVRCP TG service.
     *
     * @details Switch to the thread of the AVRCP TG service in this method.
     * @return The result of the method execution.
     * @retval true  Execute success.
     * @retval false Execute failure.
     */
    void Enable(void) override;

    /**
     * @brief Disables the AVRCP TG service.
     *
     * @detail Switch to the thread of the AVRCP TG service in this method.
     * @return The result of the method execution.
     * @retval true  Execute success.
     * @retval false Execute failure.
     */
    void Disable(void) override;

    /**
     * @brief Checks whether the AVRCP TG service is enabled.
     *
     * @return The result of the method execution.
     * @retval true  The service is enabled.
     * @retval false The service is not enabled.
     */
    bool IsEnabled(void) override;

    /******************************************************************
     * CONNECTION                                                     *
     ******************************************************************/

    /**
     * @brief Sets the active device.
     *
     * @detail Only one CT can interact witch TG.
     */
    void SetActiveDevice(const RawAddress &rawAddr) override;

    /**
     * @brief Gets the devices of the specified states.
     *
     * @param[in] states The connection states. Refer to <b>BTConnectState</b>.
     * @return The list of the instance of the <b>RawAddress</b> class.
     */
    std::list<RawAddress> GetConnectDevices(void) override
    {
        std::list<RawAddress> rawAddrs;
        return rawAddrs;
    };

    /**
     * @brief Gets the connected devices.
     *
     * @return The list of the instance of the <b>RawAddress</b> class.
     */
    std::vector<RawAddress> GetConnectedDevices(void) override;

    /**
     * @brief Gets the devices of the specified states.
     *
     * @param[in] states The connection states. Refer to <b>BTConnectState</b>.
     * @return The list of the instance of the <b>RawAddress</b> class.
     */
    std::vector<RawAddress> GetDevicesByStates(const std::vector<int> &states) override;

    /**
     * @brief Gets the connection state of the specified bluetooth device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The connection state. Refer to <b>BTConnectState</b>.
     */
    int GetDeviceState(const RawAddress &rawAddr) override;

    /**
     * @brief Gets the maximum number of connections.
     *
     * @return The maximum number of connections.
     */
    int GetMaxConnectNum(void) override;

    /**
     * @brief Connects to the AVRCP TG service.
     *
     * @details Switch to the thread of the AVRCP TG service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT The method is not supported.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int Connect(const RawAddress &rawAddr) override;

    /**
     * @brief Disconnects from the AVRCP TG service.
     *
     * @details Switch to the thread of the AVRCP TG service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT The method is not supported.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int Disconnect(const RawAddress &rawAddr) override;

    /**
     * @brief Gets the connection states.
     *
     * @return The connection states. The values are in bits.
     */
    int GetConnectState(void) override;

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
     */
    void OnSetAddressedPlayer(const RawAddress &rawAddr, uint8_t label, int status) override;

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
     * @param[in] status      The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_INVALID_PLAYER_ID<br>
     *            @a AVRC_ES_CODE_PLAYER_NOT_BROWSABLE<br>
     *            @a AVRC_ES_CODE_PLAYER_NOT_ADDRESSED<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     */
    void OnSetBrowsedPlayer(const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems,
        const std::vector<std::string> &folderNames, uint8_t label, int status) override;

    /******************************************************************
     * PLAYER APPLICATION SETTINGS                                    *
     ******************************************************************/

    /**
     * @brief Responds the data of the <b>GetPlayerAppSettingAttributes</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of player application settings.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void OnGetPlayerAppSettingAttributes(
        const RawAddress &rawAddr, const std::deque<uint8_t> &attributes, uint8_t label) override;

    /**
     * @brief Responds the data of the <b>GetPlayerAppSettingValues</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr   The address of the bluetooth device.
     * @param[in] values    The values associated witch the attribute.
     * @param[in] label     The label which is used to distinguish different call.
     */
    void OnGetPlayerAppSettingValues(
        const RawAddress &rawAddr, const std::deque<uint8_t> &values, uint8_t label) override;

    /**
     * @brief Responds the data of the <b>GetCurrentPlayerAppSettingValue</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings. Refer to <b>AvrcPlayerAttribute</b>.
     * @param[in] values     The values associated witch the attribute.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] context    The context which is used to distinguish different purpose.
     */
    void OnGetPlayerAppSettingCurrentValue(const RawAddress &rawAddr, const std::deque<uint8_t> &attributes,
        const std::deque<uint8_t> &values, uint8_t label, uint8_t context) override;

    /**
     * @brief Responds the data of the <b>SetPlayerAppSettingCurrentValue</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     */
    void OnSetPlayerAppSettingCurrentValue(const RawAddress &rawAddr, uint8_t label) override;

    /**
     * @brief Responds the data of the <b>GetPlayerAppSettingAttributeTest</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings.
     * @param[in] attrStr    The values associated witch the attribute.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void OnGetPlayerAppSettingAttributeText(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes,
        const std::vector<std::string> &attrStr, uint8_t label) override;

    /**
     * @brief Responds the data of the <b>GetPlayerAppSettingValueText</b>.
     *
     * @param[in] addr       The address of the bluetooth device.
     * @param[in] values     Player application setting value ID.
     * @param[in] valueStr   Specifies the value string of player application setting value.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void OnGetPlayerAppSettingValueText(const RawAddress &rawAddr, const std::vector<uint8_t> &values,
        const std::vector<std::string> &valueStr, uint8_t label) override;

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
     */
    void OnGetElementAttributes(const RawAddress &rawAddr, const std::vector<uint32_t> &attribtues,
        const std::vector<std::string> &values, uint8_t label) override;

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
     * @param[in] context      The context which is used to distinguish different purpose.     * @param[in] context The
     * context which is used to distinguish different purpose.     * @param[in] context      The context which is used
     * to distinguish different purpose.
     */
    void OnGetPlayStatus(const RawAddress &rawAddr, uint32_t songLength, uint32_t songPosition, uint8_t playStatus,
        uint8_t label, uint8_t context) override;

    /**
     * @brief Sends the response of the <b>PlayItem</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] status  The current status of media.
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
    void OnPlayItem(const RawAddress &rawAddr, uint8_t label, int status) override;

    /**
     * @brief Responds the data of the <b>AddToNowPlaying</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] status  The status of the method execution. Refer to <b>AvrcEsCode</b>.<br>
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
    void OnAddToNowPlaying(const RawAddress &rawAddr, uint8_t label, int status) override;

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
     * @param[in] status     The status of the method execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_INVALID_DIRECTION<br>
     *            @a AVRC_ES_CODE_NOT_A_DIRECTORY<br>
     *            @a AVRC_ES_CODE_DOES_NOT_EXIST<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     */
    void OnChangePath(const RawAddress &rawAddr, uint32_t numOfItems, uint8_t label, int status) override;

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
     */
    void OnGetMediaPlayers(const RawAddress &rawAddr, uint16_t uidCounter, const std::vector<AvrcMpItem> &items,
        uint8_t label, int status) override;

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
     */
    void OnGetFolderItems(const RawAddress &rawAddr, uint16_t uidCounter, const std::vector<AvrcMeItem> &items,
        uint8_t label, int status) override;

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
     */
    void OnGetItemAttributes(const RawAddress &rawAddr, const std::vector<uint32_t> &attributes,
        const std::vector<std::string> &values, uint8_t label, int status) override;

    /**
     * @brief Responds the data of the <b>GetTotalNumberOfItems</b>.
     *
     * @details Switch to the thread of the AVRCP TG service in this function.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] uidCounter The value of the uid counter.
     * @param[in] numOfItems The number of items in the directory.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] status     The status of the method execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     */
    void OnGetTotalNumberOfItems(
        const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems, uint8_t label, int status) override;

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
     */
    void OnSetAbsoluteVolume(const RawAddress &rawAddr, uint8_t volume, uint8_t label) override;

    /******************************************************************
     * NOTIFICATION                                                   *
     ******************************************************************/

    void OnGetSelectedTrack(const RawAddress &rawAddr, uint64_t uid, uint8_t label) override;

    void OnGetAddressedPlayer(
        const RawAddress &rawAddr, uint16_t playerId, uint16_t uidCounter, uint8_t label) override;

    void OnGetUidCounter(const RawAddress &rawAddr, uint16_t uidCounter, uint8_t label) override;

    void OnGetCurrentAbsoluteVolume(const RawAddress &rawAddr, uint8_t volume, uint8_t label) override;

    /**
     * @brief Notifies the playback status is changed.
     *
     * @param[in] playStatus  The current status of playing. Refer to <b>AvrcPlayStatus</b>.
     * @param[in] playbackPos Current playback position in millisecond.
     * @param[in] label       The label which is used to distinguish different call.
     */
    void NotifyPlaybackStatusChanged(
        uint8_t playStatus, uint32_t playbackPos, uint8_t label = AVRC_DEFAULT_LABEL) override;

    /**
     * @brief Notifies the track reached end is changed.
     *
     * @param[in] uid         The unique ID of media item.
     * @param[in] playbackPos Current playback position in millisecond.
     * @param[in] label       The label which is used to distinguish different call.
     */
    void NotifyTrackChanged(uint64_t uid, uint32_t playbackPos, uint8_t label = AVRC_DEFAULT_LABEL) override;

    /**
     * @brief Notifies the track reached end is changed.
     *
     * @param[in] playbackPos Current playback position in millisecond.
     * @param[in] label       The label which is used to distinguish different call.
     */
    void NotifyTrackReachedEnd(uint32_t playbackPos, uint8_t label = AVRC_DEFAULT_LABEL) override;

    /**
     * @brief Notifies the track reached start is changed.
     *
     * @param[in] playbackPos Current playback position in millisecond.
     * @param[in] label       The label which is used to distinguish different call.
     */
    void NotifyTrackReachedStart(uint32_t playbackPos, uint8_t label = AVRC_DEFAULT_LABEL) override;

    /**
     * @brief Notifies the player application setting is changed.
     *
     * @param[in] playbackPos Current playback position in millisecond.
     * @param[in] label       The label which is used to distinguish different call.
     */
    void NotifyPlaybackPosChanged(uint32_t playbackPos, uint8_t label = AVRC_DEFAULT_LABEL) override;

    /**
     * @brief Notifies the player application setting is changed.
     *
     * @param[in] attribute The attribute of the player application setting. Refer to <b>AvrcPlayerAttribute</b>.
     * @param[in] values    The values associated witch the attribute.
     * @param[in] label     The label which is used to distinguish different call.
     */
    void NotifyPlayerAppSettingChanged(const std::deque<uint8_t> &attributes, const std::deque<uint8_t> &values,
        uint8_t label = AVRC_DEFAULT_LABEL) override;

    /**
     * @brief Notifies the now playing content is changed.
     *
     * @param[in] label The label which is used to distinguish different call.
     */
    void NotifyNowPlayingContentChanged(uint8_t label = AVRC_DEFAULT_LABEL) override;

    /**
     * @brief Notifies the addressed player is changed.
     *
     * @param[in] label The label which is used to distinguish different call.
     */
    void NotifyAvailablePlayersChanged(uint8_t label = AVRC_DEFAULT_LABEL) override;

    /**
     * @brief Notifies the addressed player is changed.
     *
     * @param[in] playerId   The unique media player id.
     * @param[in] uidCounter The UID counter shall be incremented every time the TG makes an update.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void NotifyAddressedPlayerChanged(
        uint16_t playerId, uint16_t uidCounter, uint8_t label = AVRC_DEFAULT_LABEL) override;

    /**
     * @brief Notifies the uids is changed.
     *
     * @param[in] uidCounter The UID counter shall be incremented every time the TG makes an update.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void NotifyUidChanged(uint16_t uidCounter, uint8_t label = AVRC_DEFAULT_LABEL) override;

    /**
     * @brief Notifies the absolute volume is changed.
     *
     * @param[in] volume The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
     * @param[in] label  The label which is used to distinguish different call.
     */
    void NotifyVolumeChanged(uint8_t volume, uint8_t label = AVRC_DEFAULT_LABEL) override;

    /**
     * @brief Notifies the AVSession is created.
     */
    void OnSessionCreate(std::string sessionId) override;

    /**
     * @brief Notifies the AVSession is released.
     */
    void OnSessionRelease(std::string sessionId) override;

    /**
     * @brief Notifies the TopAVSession is changed.
     */
    void OnTopSessionChange(std::string sessionId) override;

    /**
     * @brief Notifies the playback state is changed.
     */
    void OnPlaybackStateChange(const int32_t state) override;

private:
    /// The flag is used to indicate that the state of the AVRCP TG service.
    std::atomic_uint8_t state_ {AVRC_TG_SERVICE_STATE_DISABLED};

    /// The maximum connection of device.
    int maxConnection_ {AVRC_TG_DEFAULT_MAX_OF_CONN};
    /// current connection num.
    int currentConn_ = 0;

    /// The features supported by the AVRCP TG service.
    uint16_t features_ {AVRC_TG_FEATURE_INVALID_FEATURE};

    /// using a local lock_guard to lock mutex guarantees unlocking on destruction / exception:
    std::recursive_mutex mutex_ {};
#ifdef AVRCP_AVSESSION
    /// The pointer to the instance of the <b>SessionListener</b> struct.
    std::shared_ptr<OHOS::AVSession::SessionListener> avSessionObserver_ {nullptr};

    /// The pointer to the instance of the <b>AVControllerCallback</b> struct.
    std::shared_ptr<OHOS::AVSession::AVControllerCallback> avControllerObserver_ {nullptr};

    /// The instance of the <b>AVSessionDescriptor</b>.
    std::vector<OHOS::AVSession::AVSessionDescriptor> avSessionDescriptor_ {};

    /// The pointer to the instance of the <b>AVSessionController</b> class.
    std::shared_ptr<OHOS::AVSession::AVSessionController> avSessionController_ {nullptr};
#endif
    /// The unique pointer to the instance of the <b>stub::MedisService::IObserver</b> struct.
    std::unique_ptr<stub::MediaService::IObserver> mdObserver_ {nullptr};

    /// The pointer to the instance of the <b>AvrcpTgService::IObserver</b> class.
    IObserver *myObserver_ {nullptr};
    /// The unique pointer to the instance of the <b>AvrcTgProfile::Observer</b> struct.
    std::unique_ptr<AvrcTgProfile::Observer> pfObserver_ {nullptr};

    /// The unique pointer to the instance of the <b>AvrcTgGapManager</b> class.
    std::unique_ptr<AvrcTgGapManager> gapManager_ {nullptr};
    /// The unique pointer to the instance of the <b>AvrcTgSdpManager</b> class.
    std::unique_ptr<AvrcTgSdpManager> sdpManager_ {nullptr};
    /// The unique pointer to the instance of the <b>AvrcTgProfile</b> class.
    std::unique_ptr<AvrcTgProfile> profile_ {nullptr};

    /******************************************************************
     * ENABLE / DISABLE                                               *
     ******************************************************************/

    /**
     * @brief Enables the AVRCP TG service.
     *
     * @return The result of the method execution.
     * @retval true  Execute success.
     * @retval false Execute failure.
     */
    void EnableNative(void);

    /**
     * @brief Disables the AVRCP TG service.
     *
     * @return The result of the method execution.
     * @retval true  Execute success.
     * @retval false Execute failure.
     */
    void DisableNative(void);

#ifdef AVRCP_AVSESSION
    /**
     * @brief Registers AVSessionControl.
     *
     * @return void.
     */
    void RegisterAvSessionControl(void);
#endif

    /**
     * @brief Registers security requirements into the GAP.
     *
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int RegisterSecurity(void);

    /**
     * @brief Unregisters security requirements from the GAP.
     *
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int UnregisterSecurity(void);

    /**
     * @brief Register a record of the AVRCP TG service into the SDP.
     *
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int RegisterService(void);

    /**
     * @brief Unregister a record of the AVRCP TG service from the SDP.
     *
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int UnregisterService(void);

    /**
     * @brief Enables the AVRCP TG profile.
     *
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int EnableProfile(void);

    /**
     * @brief Disables the AVRCP TG profile.
     *
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int DisableProfile(void) const;

    /**
     * @brief Informs that the AVRCP TG profile is disabled.
     *
     * @param[in] result The result of the method execution.
     */
    void OnProfileDisabled(int result = RET_NO_ERROR);

    /**
     * @brief Checks whether the AVRCP TG service is disabled.
     *
     * @return The result of the method execution.
     * @retval true  The service is disabled.
     * @retval false The service is not disabled.
     */
    bool IsDisabled(void);

    /**
     * @brief Sets the flag of the service enabled or not.
     *
     * @param[in] state The state of the service.
     */
    void SetServiceState(uint8_t state);

    void InitFeatures();

    /******************************************************************
     * CONNECTION                                                     *
     ******************************************************************/
    /**
     * @brief Connects to the AVRCP CT service.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void ConnectNative(RawAddress rawAddr);

    /**
     * @brief Disconnects from the AVRCP TG service.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void DisconnectNative(RawAddress rawAddr);

    /**
     * @brief Informs that the connection state is changed.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] state   The connection state. Refer to <b>BTConnectState</b>.
     */
    void OnConnectionStateChanged(const RawAddress &rawAddr, int state);

    /**
     * @brief Accepts the active connection.
     *
     * @param[in] rawAddr The address of the peer bluetooth device.
     */
    void AcceptActiveConnect(const RawAddress &rawAddr);

    /**
     * @brief Accepts the passive connection.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void AcceptPassiveConnect(const RawAddress &rawAddr);

    /**
     * @brief Rejects the passive connection.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void RejectPassiveConnect(const RawAddress &rawAddr);

    /**
     * @brief Finds the AVRCP TG record of the specified device.
     *
     * @param[in] rawAddr The address of the peer bluetooth device.
     */
    void FindCtService(const RawAddress &rawAddr);

    /**
     * @brief The callback function, which register into the SDP for receiving the search result.
     *
     * @param[in] btAddr      The address of the peer Bluetooth device.
     * @param[in] handleArray The list of handle to a qualifying service.
     * @param[in] handleCount The number of handle to a qualifying service.
     * @param[in] context     The context is used to send the event in the callback.
     */
    static void FindCtServiceCallback(
        const BtAddr *btAddr, const uint32_t *handleArray, uint16_t handleCount, void *context);

    /******************************************************************
     * PASS THROUGH COMMAND                                           *
     ******************************************************************/

    /**
     * @brief Informs that the button is pressed.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] button  The value of the key operation.
     * @param[in] label   The label which is used to distinguish different call.
     */
    void OnButtonPressed(const RawAddress &rawAddr, uint8_t button, uint8_t label) const;

    /**
     * @brief Informs that the button is released.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] button  The value of the key operation.
     * @param[in] label   The label which is used to distinguish different call.
     */
    void OnButtonReleased(const RawAddress &rawAddr, uint8_t button, uint8_t label) const;

    /**
     * @brief Informs that the button is held.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] button  The value of the key operation.
     * @param[in] label   The label which is used to distinguish different call.
     */
    void HoldButton(const RawAddress &rawAddr, uint8_t button, uint8_t label) const;

    /******************************************************************
     * Media Player Selection                                         *
     ******************************************************************/

    /**
     * @brief Responds the data of the <b>SetAddressedPlayer</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] playerId   The unique media player id.
     * @param[in] uidCounter The value of the uid counter.
     * @param[in] label      The label which is used to distinguish different call.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     */
    void SetAddressedPlayer(const RawAddress &rawAddr, uint16_t playerId, uint16_t uidCounter, uint8_t label) const;

    /**
     * @brief Responds the data of the <b>SetAddressedPlayer</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] status  The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_INVALID_PLAYER_ID<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     */
    void OnSetAddressedPlayerNative(RawAddress rawAddr, uint8_t label, int status);

    /**
     * @brief Responds the data of the <b>SetBrowsedPlayer</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] playerId   The unique media player id.
     * @param[in] uidCounter The value of the uid counter.
     * @param[in] label      The label which is used to distinguish different call.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     */
    void SetBrowsedPlayer(const RawAddress &rawAddr, uint16_t playerId, uint16_t uidCounter, uint8_t label) const;

    /**
     * @brief Responds the data of the <b>SetBrowsedPlayer</b>.
     *
     * @param[in] rawAddr     The address of the bluetooth device.
     * @param[in] uidCounter  The value of the uid counter.
     * @param[in] numOfItems  The number of items in the directory.
     * @param[in] folderNames The current browsed path of the player. The first element in folderNames is the parent
     * folder. The root folder has no name. The names are in UTF-8.
     * @param[in] label       The label which is used to distinguish different call.
     * @param[in] status      The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_NO_ERROR<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR<br>
     *            @a AVRC_ES_CODE_UID_CHANGED<br>
     *            @a AVRC_ES_CODE_INVALID_PLAYER_ID<br>
     *            @a AVRC_ES_CODE_PLAYER_NOT_BROWSABLE<br>
     *            @a AVRC_ES_CODE_PLAYER_NOT_ADDRESSED<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     */
    void OnSetBrowsedPlayerNative(RawAddress rawAddr, uint16_t uidCounter, uint32_t numOfItems,
        std::vector<std::string> folderNames, uint8_t label, int status);

    /******************************************************************
     * Capabilities                                                   *
     ******************************************************************/

    /**
     * @brief Get the capabilities supported by remote device.
     *
     * @details This is sent by CT to get the capabilities of the peer device.
     * @param[in] rawAddr      The address of the bluetooth device.
     * @param[in] label        The label which is used to distinguish different call.
     */
    void GetCapabilities(const RawAddress &rawAddr, uint8_t label);

    /******************************************************************
     * PLAYER APPLICATION SETTINGS                                    *
     ******************************************************************/

    /**
     * @brief Gets the attribute of the player application.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application setting. Refer to <b>AvrcPlayerAttributes</b>.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void GetPlayerAppSettingAttributes(const RawAddress &rawAddr, uint8_t label) const;

    /**
     * @brief Responds the data of the <b>GetPlayerAppSettingAttributes</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void OnGetPlayerAppSettingAttributesNative(RawAddress rawAddr, std::deque<uint8_t> attributes, uint8_t label);

    /**
     * @brief Gets the values of the specified attribute of the player application.
     *
     * @param[in] rawAddr   The address of the bluetooth device.
     * @param[in] attribute The attribute of the player application setting. Refer to <b>AvrcPlayerAttribute</b>.
     * @param[in] label     The label which is used to distinguish different call.
     */
    void GetPlayerAppSettingValues(const RawAddress &rawAddr, uint8_t attribute, uint8_t label) const;

    /**
     * @brief Responds the data of the <b>GetPlayerAppSettingValues</b>.
     *
     * @param[in] rawAddr   The address of the bluetooth device.
     * @param[in] values    The values associated witch the attribute.
     * @param[in] label     The label which is used to distinguish different call.
     */
    void OnGetPlayerAppSettingValuesNative(RawAddress rawAddr, std::deque<uint8_t> values, uint8_t label);

    /**
     * @brief Gets the current set values on the target for the provided player application setting attributes list.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings. Refer to <b>AvrcPlayerAttribute</b>.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] context    The context which is used to distinguish different purpose.
     */
    void GetPlayerAppSettingCurrentValue(
        const RawAddress &rawAddr, const std::deque<uint8_t> &attributes, uint8_t label, uint8_t context) const;

    /**
     * @brief Responds the data of the <b>GetCurrentPlayerAppSettingValue</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings. Refer to <b>AvrcPlayerAttribute</b>.
     * @param[in] values     The values associated witch the attribute.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] context    The context which is used to distinguish different purpose.
     */
    void OnGetCurrentPlayerAppSettingValueNative(
        RawAddress rawAddr, std::deque<uint8_t> attributes, std::deque<uint8_t> values, uint8_t label, uint8_t context);

    /**
     * @brief Sets the player application setting list of player application setting values on the target device for the
     * corresponding defined list of AvrcPlayerAttribute.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings. Refer to <b>AvrcPlayerAttribute</b>.
     * @param[in] values     The value of the player application setting attribute.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void SetPlayerAppSettingCurrentValue(const RawAddress &rawAddr, const std::deque<uint8_t> &attributes,
        const std::deque<uint8_t> &values, uint8_t label) const;

    /**
     * @brief Responds the data of the <b>SetPlayerAppSettingCurrentValue</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     */
    void OnSetPlayerAppSettingCurrentValueNative(RawAddress rawAddr, uint8_t label);

    /**
     * @@brief  provide supported player application setting attribute displayable text.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void GetPlayerAppSettingAttributeText(
        const RawAddress &rawAddr, const std::vector<uint8_t> &attributes, uint8_t label) const;

    /**
     * @brief Responds the data of the <b>GetPlayerAppSettingAttributeText</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings.
     * @param[in] attrStr    The values associated witch the attribute.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void OnGetPlayerAppSettingAttributeTextNative(
        RawAddress rawAddr, std::vector<uint8_t> attributes, std::vector<std::string> attrStr, uint8_t label);

    /**
     * @@brief  provide supported player application setting attribute displayable text.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void GetPlayerAppSettingValueText(
        const RawAddress &rawAddr, uint8_t attributeId, const std::vector<uint8_t> &values, uint8_t label) const;

    /**
     * @brief Responds the data of the <b>GetPlayerAppSettingValueText</b>.
     *
     * @param[in] addr       The address of the bluetooth device.
     * @param[in] values     Player application setting value ID.
     * @param[in] valueStr   Specifies the value string of player application setting value.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void OnGetPlayerAppSettingValueTextNative(
        RawAddress rawAddr, std::vector<uint8_t> values, std::vector<std::string> valueStr, uint8_t label);

    /******************************************************************
     * MEDIA INFORMATION PDUS                                         *
     ******************************************************************/

    /**
     * @brief Requests the TG to provide the attributes of the element specified in the parameter.
     *
     * @details Switch to the thread of the AVRCP CT service in this function.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] identifier Unique identifier to identify an element on TG
     * @param[in] attributes  Specifies the attribute ID for the attributes to be retrieved
     * @param[in] label      The label which is used to distinguish different call.
     */
    void GetElementAttributes(
        const RawAddress &rawAddr, uint64_t identifier, const std::vector<uint32_t> &attributes, uint8_t label) const;

    /**
     * @brief Responds the data of the <b>GetElementAttributes</b>.
     *
     * @details Switch to the thread of the AVRCP CT service in this function.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes  Specifies the attribute ID for the attributes to be retrieved.
     * @param[in] values     The list of the value of this attribute.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void OnGetElementAttributesNative(
        RawAddress rawAddr, std::vector<uint32_t> attributes, std::vector<std::string> values, uint8_t label);

    /******************************************************************
     * PLAY                                                           *
     ******************************************************************/

    /**
     * @brief Gets the play status.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] context The context which is used to distinguish different purpose.
     */
    void GetPlayStatus(const RawAddress &rawAddr, uint8_t label, uint8_t context) const;

    /**
     * @brief Responds the data of the <b>GetPlayStatus</b>.
     *
     * @param[in] rawAddr      The address of the bluetooth device.
     * @param[in] songLength   The total length of the playing song in milliseconds.
     * @param[in] songPosition The current position of the playing in milliseconds elapsed.
     * @param[in] playStatus   The current status of playing. Refer to <b>AvrcPlayStatus</b>.
     * @param[in] label        The label which is used to distinguish different call.
     * @param[in] context      The context which is used to distinguish different purpose.
     */
    void OnGetPlayStatusNative(RawAddress rawAddr, uint32_t songLength, uint32_t songPosition, uint8_t playStatus,
        uint8_t label, uint8_t context);

    /**
     * @brief Informs that receive the command of the <b>PlayItem</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] scope      The scope in which media content navigation may take place. Refer to <b>AvrcMediaScope</b>.
     * @param[in] uid        The UID of the media element item or folder item.
     * @param[in] uidCounter The UID Counter.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void PlayItem(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter, uint8_t label) const;

    /**
     * @brief Sends the response of the <b>PlayItem</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] status  The current status of media.
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
    void OnPlayItemNative(RawAddress rawAddr, uint8_t label, int status);

    /**
     * @brief Adds an item indicated by the UID to the Now Playing queue.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] scope      The scope in which media content navigation may take place. Refer to <b>AvrcMediaScope</b>.
     * @param[in] uid        The UID of the media element item or folder item.
     * @param[in] uidCounter The UID Counter.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void AddToNowPlaying(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter,
        uint8_t label) const;

    /**
     * @brief Responds the data of the <b>AddToNowPlaying</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     * @param[in] status  The status of the method execution. Refer to <b>AvrcEsCode</b>.<br>
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
    void OnAddToNowPlayingNative(RawAddress rawAddr, uint8_t label, int status);

    /******************************************************************
     * OPERATE THE VIRTUAL FILE SYSTEM                                *
     ******************************************************************/

    /**
     * @brief Navigates one level up or down in the virtual file system.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] uidCounter The value of the uid counter.
     * @param[in] direction  The flag of the navigation. Refer to <b>bluetooth::AvrcFolderDirection</b>.
     * @param[in] folderUid  The UID of the folder to navigate to. This may be retrieved via a GetFolderItems command.
     * If the navigation command is Folder Up this field is reserved.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void ChangePath(
        const RawAddress &rawAddr, uint16_t uidCounter, uint8_t direction, uint64_t folderUid, uint8_t label) const;

    /**
     * @brief Responds the data of the <b>ChangePath</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] numOfItems The number of items in the directory.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] status     The status of the method execution. Refer to <b>AvrcEsCode</b>.<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR       : Internal Error.<br>
     *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
     *            @a AVRC_ES_CODE_UID_CHANGED          : The UID is changed<br>
     *            @a AVRC_ES_CODE_INVALID_DIRECTION    : The Direction parameter is invalid.<br>
     *            @a AVRC_ES_CODE_NOT_A_DIRECTORY      : The UID provided does not refer to a folder item.<br>
     *            @a AVRC_ES_CODE_DOES_NOT_EXIST       : The UID provided does not refer to any currently valid
     * item.<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS : No available players.
     */
    void OnChangePathNative(RawAddress rawAddr, uint32_t numOfItems, uint8_t label, int status);

    /**
     * @brief Retrieves a listing of the contents of a folder.
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
     * @param[in] label      The label which is used to distinguish different call.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     */
    void GetFolderItems(const RawAddress &rawAddr, uint8_t scope, uint32_t startItem, uint32_t endItem,
        const std::vector<uint32_t> &attributes, uint8_t label) const;

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
     */
    void OnGetMediaPlayersNative(
        RawAddress rawAddr, uint16_t uidCounter, std::vector<AvrcMpItem> items, uint8_t label, int status);

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
     */
    void OnGetFolderItemsNative(
        RawAddress rawAddr, uint16_t uidCounter, std::vector<AvrcMeItem> items, uint8_t label, int status);

    /**
     * @brief Retrieves the metadata attributes for a particular media element item or folder item.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] scope      The scope in which media content navigation may take place. Refer to <b>AvrcMediaScope</b>.
     * @param[in] uid        The UID of the media element item or folder item.
     * @param[in] uidCounter The UID Counter.
     * @param[in] attributes The list of media attributes.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void GetItemAttributes(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter,
        std::vector<uint32_t> attributes, uint8_t label) const;

    /**
     * @brief Responds the data of the <b>GetItemAttributes</b>.
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
    void OnGetItemAttributesNative(RawAddress rawAddr, std::vector<uint32_t> attributes,
        std::vector<std::string> values, uint8_t label, int status);

    /**
     * @brief Gets the number of items in the selected folder at the selected scope.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] scope      The scope in which media content navigation may take place. Refer to
     * <b>AvrcMediaScope</b>.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void GetTotalNumberOfItems(const RawAddress &rawAddr, uint8_t scope, uint8_t label) const;

    /**
     * @brief Responds the data of the <b>GetTotalNumberOfItems</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] uidCounter The value of the uid counter.
     * @param[in] numOfItems The number of items in the directory.
     * @param[in] label      The label which is used to distinguish different call.
     * @param[in] response   The response of calling the <b>ChangePath</b>.<br>
     *            @a AVRC_ES_CODE_INTERNAL_ERROR       : Internal Error.<br>
     *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
     *            @a AVRC_ES_CODE_UID_CHANGED          : The UID is changed<br>
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS : No available players.
     */
    void OnGetTotalNumberOfItemsNative(
        RawAddress rawAddr, uint16_t uidCounter, uint32_t numOfItems, uint8_t label, int status);

    /******************************************************************
     * ABSOLUTE VOLUME                                                *
     ******************************************************************/

    /**
     * @brief Sets an absolute volume to be used by the rendering device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] volume  The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
     * @param[in] label   The label which is used to distinguish different call.
     */
    void SetAbsoluteVolume(const RawAddress &rawAddr, uint8_t volume, uint8_t label) const;

    /**
     * @brief Responds the data of the <b>SetAbsoluteVolume</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] volume  The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
     * @param[in] label   The label which is used to distinguish different call.
     */
    void OnSetAbsoluteVolumeNative(RawAddress rawAddr, uint8_t volume, uint8_t label);

    /******************************************************************
     * NOTIFICATION                                                   *
     ******************************************************************/

    /**
     * @brief Responds the command of the <b>EVENT_TRACK_CHANGED</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     */
    void GetSelectedTrack(const RawAddress &rawAddr, uint8_t label) const;

    void OnGetSelectedTrackNative(RawAddress rawAddr, uint64_t uid, uint8_t label);

    /**
     * @brief Responds the command of the <b>EVENT_ADDRESSED_PLAYER_CHANGED</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] label   The label which is used to distinguish different call.
     */
    void GetAddressedPlayer(const RawAddress &rawAddr, uint8_t label) const;

    void OnGetAddressedPlayerNative(RawAddress rawAddr, uint16_t playerId, uint16_t uidCounter, uint8_t label);

    void GetUidCounter(const RawAddress &rawAddr, uint8_t label) const;

    void OnGetUidCounterNative(const RawAddress &rawAddr, uint16_t uidCounter, uint8_t label);

    void GetCurrentAbsoluteVolume(const RawAddress &rawAddr, uint8_t label)const;

    void OnGetCurrentAbsoluteVolumeNative(const RawAddress &rawAddr, uint8_t volume, uint8_t label);

    /**
     * AVSession
     */
    void OnPlaybackStateChangeNative(int32_t state);

    /**
     * @brief Notifies the playback status is changed.
     *
     * @param[in] playStatus  The current status of playing. Refer to <b>AvrcPlayStatus</b>.
     * @param[in] playbackPos Current playback position in millisecond.
     * @param[in] label       The label which is used to distinguish different call.
     */
    void NotifyPlaybackStatusChangedNative(uint8_t playStatus, uint32_t playbackPos, uint8_t label);

    /**
     * @brief Notifies the track reached end is changed.
     *
     * @param[in] uid         The unique ID of media item.
     * @param[in] playbackPos Current playback position in millisecond.
     * @param[in] label       The label which is used to distinguish different call.
     */
    void NotifyTrackChangedNative(uint64_t uid, uint32_t playbackPos, uint8_t label);

    /**
     * @brief Notifies the track reached end is changed.
     *
     * @param[in] playbackPos Current playback position in millisecond.
     * @param[in] label       The label which is used to distinguish different call.
     */
    void NotifyTrackReachedEndNative(uint32_t playbackPos, uint8_t label);

    /**
     * @brief Notifies the track reached start is changed.
     *
     * @param[in] playbackPos Current playback position in millisecond.
     * @param[in] label       The label which is used to distinguish different call.
     */
    void NotifyTrackReachedStartNative(uint32_t playbackPos, uint8_t label);

    /**
     * @brief Notifies the player application setting is changed.
     *
     * @param[in] playbackPos Current playback position in millisecond.
     * @param[in] label       The label which is used to distinguish different call.
     */
    void NotifyPlaybackPosChangedNative(uint32_t playbackPos, uint8_t label);

    /**
     * @brief Notifies the player application setting is changed.
     *
     * @param[in] attribute The attribute of the player application setting. Refer to <b>AvrcPlayerAttribute</b>.
     * @param[in] values    The values associated witch the attribute.
     * @param[in] label     The label which is used to distinguish different call.
     */
    void NotifyPlayerAppSettingChangedNative(std::deque<uint8_t> attributes, std::deque<uint8_t> values, uint8_t label);

    /**
     * @brief Notifies the now playing content is changed.
     *
     * @param[in] label The label which is used to distinguish different call.
     */
    void NotifyNowPlayingContentChangedNative(uint8_t label);

    /**
     * @brief Notifies the addressed player is changed.
     *
     * @param[in] label The label which is used to distinguish different call.
     */
    void NotifyAvailablePlayersChangedNative(uint8_t label);

    /**
     * @brief Notifies the addressed player is changed.
     *
     * @param[in] playerId   The unique media player id.
     * @param[in] uidCounter The UID counter shall be incremented every time the TG makes an update.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void NotifyAddressedPlayerChangedNative(uint16_t playerId, uint16_t uidCounter, uint8_t label);

    /**
     * @brief Notifies the uids is changed.
     *
     * @param[in] uidCounter The UID counter shall be incremented every time the TG makes an update.
     * @param[in] label      The label which is used to distinguish different call.
     */
    void NotifyUidChangedNative(uint16_t uidCounter, uint8_t label);

    /**
     * @brief Notifies the absolute volume is changed.
     *
     * @param[in] volume The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
     * @param[in] label  The label which is used to distinguish different call.
     */
    void NotifyVolumeChangedNative(uint8_t volume, uint8_t label);

    /**
     * @brief Sets the specified the time interval(in seconds).
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] interval The specifies the time interval (in seconds) at which the change in playback position
     * will be notified. If the song is being forwarded / rewound, a notification will be received whenever the
     * playback position will change by this value.
     */
    void SetPlaybackInterval(const RawAddress &rawAddr, uint32_t interval) const;

    /**
     * @brief Processes events received from the AVCTP.
     *
     * @param[in] rawAddr   The address of the bluetooth device.
     * @param[in] connectId The ID of the connection.
     * @param[in] event     The event from the underlying module.
     * @param[in] result    The result of the event.
     * @param[in] context   The context is used to send the event in the callback.
     */
    void ProcessChannelEvent(RawAddress rawAddr, uint8_t connectId, uint8_t event, uint16_t result, void *context);

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
     * @brief The callback function for receiving the events, which register into the AVCTP.
     *
     * @detail This function is the callback function registered into the AVCTP.
     * @param[in] connectId The ID of the connection.
     * @param[in] event     The event from the underlying module.
     * @param[in] result    The result of the event.
     * @param[in] btAddr    The address of the bluetooth device.
     * @param[in] context   The context is used to send the event in the callback.
     */
    static void ChannelEventCallback(
        uint8_t connectId, uint8_t event, uint16_t result, const BtAddr *btAddr, void *context);

    /**
     * @brief The callback function for receiving the messages, which register into the AVCTP.
     *
     * @param[in] connectId The ID of the connection.
     * @param[in] label     The label used to distinguish between different calls to the same function.
     * @param[in] crType    The type used to distinguish between the command frame and the response frame.
     * @param[in] chType    The type used to distinguish the message returned from the control channel or the browse
     * channel.
     * @param[in] pkt       The message sent by the peer bluetooth device.
     * @param[in] context   The context is used to send the message in the callback.
     */
    static void ChannelMessageCallback(
        uint8_t connectId, uint8_t label, uint8_t crType, uint8_t chType, Packet *pkt, void *context);

    bool CheckConnectionNum();

    inline void DecConnectionNum()
    {
        if (currentConn_-- < 0) {
            currentConn_ = 0;
        }
    }

    uint8_t ConvertPlayState(const int32_t state) const;

    BT_DISALLOW_COPY_AND_ASSIGN(AvrcpTgService);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_TG_SERVICE_H
