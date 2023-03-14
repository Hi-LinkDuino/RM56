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
 * @file bluetooth_avrcp_ct.h
 *
 * @brief Declares the class of the AVRCP controller framework, including attributes and methods.
 *
 * @since 6
 */

#ifndef BLUETOOTH_AVRCP_CT_H
#define BLUETOOTH_AVRCP_CT_H

#include <vector>

#include "bluetooth_def.h"
#include "bluetooth_types.h"
#include "bluetooth_remote_device.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief This class provides the attribute of the result of the actions.
 *
 * @since 6
 */
class AvrcpCtResponse {
public:
    /**
     * @brief This struct declares the attributes of the button.
     *
     * @since 6
     */
    class Button {
    public:
        Button(uint8_t code) : code_(code)
        {};
        ~Button()
        {};
        uint8_t code_;  // The value of the button.

    private:
        Button() = delete;
    };

    /**
     * @brief This struct declares a set of capabilities supported by TG.
     *
     * @since 6
     */
    class Capabilities {
    public:
        Capabilities(std::vector<uint32_t> companies) : companies_(companies)
        {};
        Capabilities(std::vector<uint8_t> events) : events_(events)
        {};
        ~Capabilities()
        {
            events_.clear();
            companies_.clear();
        }
        std::vector<uint32_t> companies_;
        std::vector<uint8_t> events_;

    private:
        Capabilities() = delete;
    };

    /**
     * @brief This struct declares a set of attributes of the player application setting.
     *
     * @since 6
     */
    class PlayerSettingAttributes {
    public:
        PlayerSettingAttributes(std::vector<uint8_t> attributes) : attributes_(attributes)
        {};
        ~PlayerSettingAttributes()
        {
            attributes_.clear();
        };
        std::vector<uint8_t> attributes_;  // The attribute of the player application setting.

    private:
        PlayerSettingAttributes() = delete;
    };

    /**
     * @brief This struct declares a set of attributes of the player application setting values.
     *
     * @since 6
     */
    class PlayerSettingValues {
    public:
        PlayerSettingValues(uint8_t attribute, const std::vector<uint8_t> &values)
            : attribute_(attribute), values_(values)
            {};
        ~PlayerSettingValues()
        {
            values_.clear();
        };
        uint8_t attribute_;            // The attribute of the player application setting.
        std::vector<uint8_t> values_;  // The values of the specified attribute of the player application setting.

    private:
        PlayerSettingValues() = delete;
    };

    /**
     * @brief This struct declares a set of attributes of the current player application setting value.
     *
     * @since 6
     */
    class PlayerSettingCurrentValue {
    public:
        PlayerSettingCurrentValue(const std::vector<uint8_t> &attributes, const std::vector<uint8_t> &values)
            : attributes_(attributes), values_(values)
            {};
        ~PlayerSettingCurrentValue()
        {
            attributes_.clear();
            values_.clear();
        };
        std::vector<uint8_t> attributes_;  // The attribute of the player application settings.
        std::vector<uint8_t> values_;      // The value of the specified attribute of the player application settings.

    private:
        PlayerSettingCurrentValue() = delete;
    };

    /**
     * @brief This struct declares a set of attributes of the player application getting attribtue text.
     *
     * @since 6
     */
    class PlayerGettingText {
    public:
        PlayerGettingText(const std::vector<uint8_t> &attributes, const std::vector<std::string> &attrStr)
            : attributes_(attributes), attrStr_(attrStr)
            {};
        ~PlayerGettingText()
        {
            attributes_.clear();
            attrStr_.clear();
        };
        std::vector<uint8_t> attributes_;   // The attribute of the player application settings.
        std::vector<std::string> attrStr_;  // The values associated witch the attribute.

    private:
        PlayerGettingText() = delete;
    };

    /**
     * @brief This struct declares a set of element attribute.
     *
     * @since 6
     */
    class ElementAttributes {
    public:
        ElementAttributes(const std::vector<uint32_t> &attributes, const std::vector<std::string> &values)
            : attributes_(attributes), values_(values)
            {};
        ~ElementAttributes()
        {
            attributes_.clear();
            values_.clear();
        };
        std::vector<uint32_t> attributes_;  // The attribute of the player application settings.
        std::vector<std::string> values_;   // The value of the specified attribute of the player application settings.

    private:
        ElementAttributes() = delete;
    };

    /**
     * @brief This struct declares the attributes of the player status.
     *
     * @since 6
     */
    class PlayStatus {
    public:
        PlayStatus(uint32_t songLength, uint32_t songPosition, uint8_t playStatus)
            : songLength_(songLength), songPosition_(songPosition), playStatus_(playStatus)
            {};
        ~PlayStatus()
        {};
        uint32_t songLength_;    // The total length of the playing song in milliseconds.
        uint32_t songPosition_;  // The current position of the playing in milliseconds elapsed.
        uint8_t playStatus_;     // The current status of playing. Refer to <b>AvrcPlayStatus</b>.

    private:
        PlayStatus() = delete;
    };

    /**
     * @brief This struct declares the attributes of the media item.
     *
     * @since 6

     */
    class MediaItems {
    public:
        struct MediaItem {
            // The value of the "Folder Type" and the "Media Type". Refer to <b>AvrcMediaFolderType</b> and
            // <b>AvrcMediaElementType</b>.
            uint8_t type_ {AVRC_MEDIA_ELEMENT_TYPE_RESERVED};
            // The value of the "Is Playable". Refer to <b>AvrcMediaFolderPlayable</b>.
            uint8_t playable_ {AVRC_MEDIA_FOLDER_PLAYABLE_RESERVED};
            uint64_t uid_ {0xFFFFFFFFFFFFFFFF};
            // The value of the "Displayable Name".
            std::string name_;
            // The list of the "Attribute ID".  Refer to <b>AvrcMediaAttribute</b>.
            std::vector<uint32_t> attributes_;
            // The list of the "Attribute Value".
            std::vector<std::string> values_;
        };
        MediaItems(uint16_t uidCounter, const std::vector<MediaItem> &mediaItems)
            : uidCounter_(uidCounter), mediaItems_(mediaItems)
            {};
        ~MediaItems()
        {};

        uint16_t uidCounter_ {0xFFFF};
        std::vector<MediaItem> mediaItems_;

    private:
        MediaItems() = delete;
    };

        /**
     * @brief This struct declares the the media player list.
     *
     * @since 6
     */
    class MediaPlayers {
    public:
        struct MediaPlayer {
            /// The value of the "Folder Type" and the "Media Type". Refer to <b>AvrcMediaFolderType</b> and
            /// <b>AvrcMediaElementType</b>.
            uint8_t itemType_ {AVRC_MEDIA_ELEMENT_TYPE_RESERVED};
            /// The value of the "Is Playable". Refer to <b>AvrcMediaFolderPlayable</b>.
            uint16_t playerId_ {AVRC_MEDIA_FOLDER_PLAYABLE_RESERVED};
            ///< The value of the "Major Player Type". Refer to <b>AvrcMediaMajorPlayerType</b>.
            uint8_t majorType_ = 0x00;
            ///< The value of the "Player Sub Type". Refer to <b>AvrcMediaPlayerSubType</b>.
            uint32_t subType_ = 0x00;
            ///< The value of the "Play Status". Refer to <b>AvrcPlayStatus</b>.
            uint8_t playStatus_ = 0x00;
            ///< The value of the "Feature Bit Mask".
            std::vector<uint8_t> features_;
            ///< The value of the "Displayable Name".
            std::string name_ = "\0";
        };
        MediaPlayers(uint16_t uidCounter, const std::vector<MediaPlayer> &mediaPlayers)
            : uidCounter_(uidCounter), mediaPlayers_(mediaPlayers)
            {};
        ~MediaPlayers()
        {};

        uint16_t uidCounter_ {0xFFFF};
        std::vector<MediaPlayer> mediaPlayers_;

    private:
        MediaPlayers() = delete;
    };

    /**
     * @brief This struct declares the attributes of the media item attribute.
     *
     * @since 6
     */
    class ItemAttributes {
    public:
        struct ItemAttribute {
            uint32_t attribute_;
            std::string value_;
        };
        ItemAttributes(const std::vector<ItemAttribute> &itemAttrs) : itemAttrs_(itemAttrs)
        {};
        ~ItemAttributes()
        {};
        std::vector<ItemAttribute> itemAttrs_;

    private:
        ItemAttributes() = delete;
    };

    /**
     * @brief This struct declares a set of attributes of the player application setting values.
     *
     * @since 6
     */
    class TotalNumberOfItems {
    public:
        TotalNumberOfItems(uint16_t uidCounter, uint32_t numOfItems)
            : uidCounter_(uidCounter), numOfItems_(numOfItems)
            {};
        ~TotalNumberOfItems()
        {};
        uint16_t uidCounter_;  // The value of the uid counter.
        uint32_t numOfItems_;  // The number of items in the directory.

    private:
        TotalNumberOfItems() = delete;
    };

    /**
     * @brief This struct declares a set of attributes of the player application setting values.
     *
     * @since 6
     */
    class AbsoluteVolume {
    public:
        AbsoluteVolume(uint8_t volume) : volume_(volume)
        {};
        ~AbsoluteVolume()
        {};
        uint8_t volume_;  // The percentage of the absolute volume.Refer to <b> AvrcAbsoluteVolume</ b>.

    private:
        AbsoluteVolume() = delete;
    };

    /**
     * @brief This struct declares a set of attributes of the notifications.
     *
     * @since 6
     */
    class Notification {
    public:
        Notification(uint8_t playStatus, uint8_t volume) : playStatus_(playStatus), volume_(volume)
        {};
        Notification(uint64_t uid) : uid_(uid)
        {};
        Notification(uint32_t playbackPos) : playbackPos_(playbackPos)
        {};
        Notification(const std::vector<uint8_t> &attributes, const std::vector<uint8_t> &values)
            : attributes_(attributes), values_(values)
            {};
        Notification(uint16_t playerId, uint16_t uidCounter) : playerId_(playerId), uidCounter_(uidCounter)
        {};
        Notification(uint16_t uidCounter) : uidCounter_(uidCounter)
        {};
        ~Notification()
        {};
        uint8_t playStatus_ {AVRC_PLAY_STATUS_ERROR};    // The value of the "PlayStatus".
        uint64_t uid_ {0xFFFFFFFFFFFFFFFF};              // The value of the "Identifier".
        uint32_t playbackPos_ {0x00000000};              // The value of the "Playback position".
        std::vector<uint8_t> attributes_;             // The value of the "PlayerApplicationSettingAttributeID".
        std::vector<uint8_t> values_;                 // The value of the "PlayerApplicationSettingValueID".
        uint16_t playerId_ {0xFFFF};                     // The value of the "Player Id".
        uint16_t uidCounter_ {0xFFFF};                   // The value of the "UID Counter".
        uint8_t volume_ {AVRC_ABSOLUTE_VOLUME_INVALID};  // The value of the "Absolute Volume".
    private:
        Notification() = delete;
    };

    AvrcpCtResponse(uint8_t type, int resp);
    ~AvrcpCtResponse();

    uint8_t type_;  // The type of the action.
    int resp_;      // The result of the called action.
    // The unique pointer to the <b>AvrcpCtResponse::Button</b> class.
    std::unique_ptr<Button> button_ {nullptr};
    // The unique pointer to the <b>AvrcpCtResponse::GetCapabilities</b> class.
    std::unique_ptr<Capabilities> capabilities_ {nullptr};
    // The unique pointer to the <b>AvrcpCtResponse::PlayerSettingAttributes</b> class.
    std::unique_ptr<PlayerSettingAttributes> playerAttrs_ {nullptr};
    // The unique pointer to the <b>AvrcpCtResponse::PlayerSettingValues</b> class.
    std::unique_ptr<PlayerSettingValues> playerVals_ {nullptr};
    // The unique pointer to the <b>AvrcpCtResponse::PlayerSettingCurrentValue</b> class.
    std::unique_ptr<PlayerSettingCurrentValue> playerCurVal_ {nullptr};
    // The unique pointer to the <b>AvrcpCtResponse::PlayerGettingAttribtueText</b> class.
    std::unique_ptr<PlayerGettingText> playerText_ {nullptr};
    // The unique pointer to the <b>AvrcpCtResponse::GetElementAttributes</b> class.
    std::unique_ptr<ElementAttributes> eleSts_ {nullptr};
    // The unique pointer to the <b>AvrcpCtResponse::PlayStatus</b> class.
    std::unique_ptr<PlayStatus> playSts_ {nullptr};
    // The unique pointer to the <b>AvrcpCtResponse::MediaItem</b> class.
    std::unique_ptr<MediaItems> mediaItems_ {nullptr};
    ///< The unique pointer to the <b>AvrcpCtResponse::MediaPlayer</b> class.
    std::unique_ptr<MediaPlayers> mediaPlayers_ {nullptr};
    ///< The unique pointer to the <b>AvrcpCtResponse::ItemAttributes</b> class.
    std::unique_ptr<ItemAttributes> itemAttrs_ {nullptr};
    // The unique pointer to the <b>AvrcpCtResponse::TotalNumberOfItems</b> class.
    std::unique_ptr<TotalNumberOfItems> totalItems_ {nullptr};
    // The unique pointer to the <b>AvrcpCtResponse::AbsoluteVolume</b> class.
    std::unique_ptr<AbsoluteVolume> absVolume_ {nullptr};
    // The unique pointer to the <b>AvrcpCtResponse::Notification</b> class.
    std::unique_ptr<Notification> notify_ {nullptr};

private:
    AvrcpCtResponse() = delete;
};

/**
 * @brief This class provides a set of methods for operating the AVRCP controller.
 *
 * @since 6
 */
class BLUETOOTH_API AvrcpController {
public:
    /**
     * @brief This abstract class declares a set of methods for observing the <b>AvrcpController::IObserver</b> class.
     *
     * @since 6
     */
    class IObserver {
    public:
        /**
         * @brief A constructor used to create an <b>AvrcpController::IObserver</b> instance.
         *
         * @since 6
         */
        IObserver() = default;

        /**
         * @brief A destructor used to delete the <b>AvrcpController::IObserver</b> instance.
         *
         * @since 6
         */
        virtual ~IObserver() = default;

        /**
         * @brief Observes the state of the connection.
         *
         * @param[in] device The bluetooth device.
         * @param[in] state  The connection state. Refer to <b>BTConnectState</b>.
         *
         * @since 6
         */
        virtual void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state) = 0;

        /**
         * @brief Observes the status of the other actions.
         *
         * @param[in] device The bluetooth device.
         * @param[in] resp   The result of the called action.
         *
         * @since 6
         */
        virtual void OnActionCompleted(const BluetoothRemoteDevice &device, const AvrcpCtResponse &resp) = 0;
    };

    /**
     * @brief Gets the static instance of the <b>AvrcpController</b> class.
     *
     * @return The static instance.
     *
     * @since 6
     */
    static AvrcpController *GetProfile(void);

    /******************************************************************
     * REGISTER / UNREGISTER OBSERVER                                 *
     ******************************************************************/

    /**
     * @brief Registers the observer.
     *
     * @param[in] observer The pointer to the <b>AvrcpController::IObserver</b>.
     * @since 6
     */
    void RegisterObserver(AvrcpController::IObserver *observer);

    /**
     * @brief Unregisters the observer.
     *
     * @param[in] observer The pointer to the <b>AvrcpController::IObserver</b>.
     * @since 6
     */
    void UnregisterObserver(AvrcpController::IObserver *observer);

    /******************************************************************
     * CONNECTION                                                     *
     ******************************************************************/

    /**
     * @brief Gets the connected devices.
     *
     * @return The list of the instance of the <b>BluetoothRemoteDevice</b> class.
     *
     * @since 6
     */
    std::vector<BluetoothRemoteDevice> GetConnectedDevices(void);

    /**
     * @brief Gets the devices of the specified states.
     *
     * @param[in] states The connection states. Refer to <b>BTConnectState</b>.
     * @return The list of the instance of the <b>BluetoothRemoteDevice</b> class.
     *
     * @since 6
     */
    std::vector<BluetoothRemoteDevice> GetDevicesByStates(const std::vector<int> &states);

    /**
     * @brief Gets the connection state of the specified bluetooth device.
     *
     * @param[in] device The bluetooth device.
     * @return The connection state. Refer to <b>BTConnectState</b>.
     *
     * @since 6
     */
    int GetDeviceState(const BluetoothRemoteDevice &device);

    /**
     * @brief Connects to the AVRCP TG service.
     *
     * @param[in] device The bluetooth device.
     * @return The result of the method execution.
     * @retval true  command send success.
     * @retval false command seend failed.
     *
     * @since 6
     */
    bool Connect(const BluetoothRemoteDevice &device);

    /**
     * @brief Disconnects from the AVRCP TG service.
     *
     * @param[in] device The bluetooth device.
     * @return The result of the method execution.
     * @retval true  command send success.
     * @retval false command seend failed.
     *
     * @since 6
     */
    bool Disconnect(const BluetoothRemoteDevice &device);

    /******************************************************************
     * BUTTON OPERATION                                               *
     ******************************************************************/

    /**
     * @brief Presses the button.
     *
     * @param[in] device The bluetooth device.
     * @param[in] button The value of the key operation. Refer to <b>AvrcKeyOperation</b>
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_PARAM  Bad parameters.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    int PressButton(const BluetoothRemoteDevice &device, uint8_t button);

    /**
     * @brief Releases the button.
     *
     * @param[in] device The bluetooth device.
     * @param[in] button The value of the key operation. Refer to <b>AvrcKeyOperation</b>
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_PARAM  Bad parameters.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    int ReleaseButton(const BluetoothRemoteDevice &device, uint8_t button);

    /******************************************************************
     * UNIT INFO / SUB UNIT INFO                                      *
     ******************************************************************/

    // TEMP
    /**
     * @brief Gets the unit information.
     *
     * @param[in] device The bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    int GetUnitInfo(const BluetoothRemoteDevice &device);

    // TEMP
    /**
     * @brief Gets the sub unit information.
     *
     * @param[in] device The bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    int GetSubUnitInfo(const BluetoothRemoteDevice &device);

    /******************************************************************
     * Media Player Selection                                         *
     ******************************************************************/

    // TEMP
    /**
     * @brief Informs which media player wishes to control.
     *
     * @param[in] device   The bluetooth device.
     * @param[in] playerId The unique media player id.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    int SetAddressedPlayer(const BluetoothRemoteDevice &device, uint16_t playerId);

    /**
     * @brief Informs to which player browsing commands should be routed.
     *
     * @param[in] device   The bluetooth device.
     * @param[in] playerId The unique media player id.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    int SetBrowsedPlayer(const BluetoothRemoteDevice &device, uint16_t playerId);

    /******************************************************************
     * Capabilities                                                   *
     ******************************************************************/

    /**
     * @brief Get the supported companies by remote device.
     *
     * @details This is sent by CT to get the capabilities of the peer device.
     * @param[in] rawAddr      The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    int GetSupportedCompanies(const BluetoothRemoteDevice &device);

    /**
     * @brief Get the supported events by remote device.
     *
     * @details This is sent by CT to get the capabilities of the peer device.
     * @param[in] rawAddr      The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    int GetSupportedEvents(const BluetoothRemoteDevice &device);
    /******************************************************************
     * PLAYER APPLICATION SETTINGS                                    *
     ******************************************************************/

    /**
     * @brief Gets the attribute of the player application.
     *
     * @param[in] device The bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_PARAM  Bad parameters.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    int GetPlayerAppSettingAttributes(const BluetoothRemoteDevice &device);

    /**
     * @brief Gets the values of the specified attribute of the player application.
     *
     * @param[in] device    The bluetooth device.
     * @param[in] attribute The attribute of the player application setting. Refer to <b>AvrcPlayerAttribute</b>.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_PARAM  Bad parameters.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    int GetPlayerAppSettingValues(const BluetoothRemoteDevice &device, uint8_t attribute);

    /**
     * @brief Gets the current set values on the target for the provided player application setting attributes list.
     *
     * @param[in] device     The bluetooth device.
     * @param[in] attributes The attribute of the player application settings. Refer to <b>AvrcPlayerAttribute</b>.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    int GetPlayerAppSettingCurrentValue(const BluetoothRemoteDevice &device, const std::vector<uint8_t> &attributes);

    /**
     * @brief Sets the player application setting list of player application setting values on the target device for the
     * corresponding defined list of AvrcPlayerAttribute.
     *
     * @param[in] device     The bluetooth device.
     * @param[in] attributes The attribute of the player application settings. Refer to <b>AvrcPlayerAttribute</b>.
     * @param[in] values     The value of the player application setting attribute.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    int SetPlayerAppSettingCurrentValue(const BluetoothRemoteDevice &device, const std::vector<uint8_t> &attributes,
        const std::vector<uint8_t> &values);

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
    int GetPlayerApplicationSettingAttributeText(
        const BluetoothRemoteDevice &device, const std::vector<uint8_t> &attributes);

    /**
     * @brief  request the target device to provide target supported player application setting value displayable text.
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
    int GetPlayerApplicationSettingValueText(
        const BluetoothRemoteDevice &device, uint8_t attributeId, const std::vector<uint8_t> &values);

    /******************************************************************
     * MEDIA INFORMATION PDUS                                         *
     ******************************************************************/

    /**
     * @brief Requests the TG to provide the attributes of the element specified in the parameter.
     *
     * @details Switch to the thread of the AVRCP CT service in this function.
     * @param[in] device     The bluetooth device.
     * @param[in] attributes Specifies the attribute ID for the attributes to be retrieved
     *            @c RET_NO_ERROR   : The action is successful.
     *            @c RET_NO_SUPPORT : The action is not supported.
     *            @c RET_BAD_STATUS : The action is failed.
     */

    int GetElementAttributes(const BluetoothRemoteDevice &device, const std::vector<uint32_t> &attributes);

    /******************************************************************
     * PLAY                                                           *
     ******************************************************************/

    /**
     * @brief Gets the play status.
     *
     * @param[in] device The bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    int GetPlayStatus(const BluetoothRemoteDevice &device);

    /**
     * @brief Starts playing an item indicated by the UID.
     *
     * @param[in] device     The bluetooth device.
     * @param[in] scope      The scope in which media content navigation may take place. Refer to <b>AvrcMediaScope</b>.
     * @param[in] uid        The unique ID of media item.
     * @param[in] uidCounter The UID counter shall be incremented every time the TG makes an update.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    int PlayItem(const BluetoothRemoteDevice &device, uint64_t uid, uint16_t uidCounter);

    // TEMP
    /**
     * @brief Adds an item indicated by the UID to the Now Playing queue.
     *
     * @param[in] device     The bluetooth device.
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
    int AddToNowPlaying(const BluetoothRemoteDevice &device, uint64_t uid, uint16_t uidCounter);

    /******************************************************************
     * CONTINUING RESPONSE / ABORT CONTINUING RESPONSE                *
     ******************************************************************/

    /**
     * @brief Requests continuing response.
     *
     * @param[in] device  The bluetooth device.
     * @param[in] pduId   The PDU ID which wants to request.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int RequestContinuingResponse(const BluetoothRemoteDevice &device, uint8_t pduId);

    /**
     * @brief Aborts continuing response.
     *
     * @param[in] device  The bluetooth device.
     * @param[in] pduId   The PDU ID which wants to abort.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6.0
     */
    int AbortContinuingResponse(const BluetoothRemoteDevice &device, uint8_t pduId);

    /******************************************************************
     * OPERATE THE VIRTUAL FILE SYSTEM                                *
     ******************************************************************/

    /**
     * @brief Navigates one level up or down in the virtual file system.
     *
     * @param[in] device     The bluetooth device.
     * @param[in] uidCounter The value of the uid counter.
     * @param[in] direction  The flag of the navigation. Refer to <b>bluetooth::AvrcFolderDirection</b>.
     * @param[in] folderUid  The UID of the folder to navigate to. This may be retrieved via a GetFolderItems command.
     * If the navigation command is Folder Up this field is reserved.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_PARAM  Bad parameters.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    int ChangePath(const BluetoothRemoteDevice &device, uint16_t uidCounter, uint16_t direction, uint64_t folderUid);

    /**
     * @brief Retrieves a listing of the contents of a folder.
     *
     * @param[in] device     The bluetooth device.
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
    int GetFolderItems(const BluetoothRemoteDevice &device, uint32_t startItem, uint32_t endItem,
        const std::vector<uint32_t> &attributes);

        /**
     * @brief Retrieves a listing of the contents of a folder.
     *
     * @param[in] device     The bluetooth device.
     * @param[in] startItem  The offset within the listing of the item, which should be the first returned item. The
     * first element in the listing is at offset 0.
     * @param[in] endItem    The offset within the listing of the item which should be the final returned item. If this
     * is set to a value beyond what is available, the TG shall return items from the provided Start Item index to the
     * index of the final item. If the End Item index is smaller than the Start Item index, the TG shall return an
     * error. If CT requests too many items, TG can respond with a sub-set of the requested items.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    int GetMeidaPlayerList(const BluetoothRemoteDevice &device, uint32_t startItem, uint32_t endItem);

    /**
     * @brief Retrieves the metadata attributes for a particular media element item or folder item.
     *
     * @param[in] device     The bluetooth device.
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
    int GetItemAttributes(const BluetoothRemoteDevice &device, uint64_t uid, uint16_t uidCounter,
        const std::vector<uint32_t> &attributes);

    /**
     * @brief Gets the number of items in the now playing scope.
     *
     * @param[in] device The bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    int GetTotalNumberOfItems(const BluetoothRemoteDevice &device);

    /******************************************************************
     * ABSOLUTE VOLUME                                                *
     ******************************************************************/

    /**
     * @brief Sets an absolute volume to be used by the rendering device.
     *
     * @param[in] device The bluetooth device.
     * @param[in] volume The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     *
     * @since 6
     */
    int SetAbsoluteVolume(const BluetoothRemoteDevice &device, uint8_t volume);

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
    int EnableNotification(const BluetoothRemoteDevice &device, const std::vector<uint8_t> &events,
        uint32_t interval = AVRC_PLAYBACK_INTERVAL_1_SEC);

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
    int DisableNotification(const BluetoothRemoteDevice &device, const std::vector<uint8_t> &events);

private:
    /**
     * @brief A constructor used to create an <b>AvrcpController</b> instance.
     *
     * @since 6
     */
    AvrcpController();

    /**
     * @brief A destructor used to delete the <b>AvrcpController</b> instance.
     *
     * @since 6
     */
    virtual ~AvrcpController();

    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(AvrcpController);
    BLUETOOTH_DECLARE_IMPL();
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // !BLUETOOTH_AVRCP_CT_H
