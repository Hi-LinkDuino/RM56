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

#ifndef AVRCP_CT_SERVICE_H
#define AVRCP_CT_SERVICE_H

#include <atomic>

#include "bt_def.h"
#include "context.h"
#include "interface_profile_avrcp_ct.h"

#include "avrcp_ct_gap.h"
#include "avrcp_ct_internal.h"
#include "avrcp_ct_profile.h"
#include "avrcp_ct_sdp.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This enumeration declares the states of the AVRCP TG service.
 */
enum AvrcCtServiceState : uint8_t {
    AVRC_CT_SERVICE_STATE_INVALID,
    AVRC_CT_SERVICE_STATE_ENABLING,
    AVRC_CT_SERVICE_STATE_ENABLED,
    AVRC_CT_SERVICE_STATE_DISABLING,
    AVRC_CT_SERVICE_STATE_DISABLED,
};

/**
 * @brief This class provides a set of methods related to the role of the controller described in the Audio/Video Remote
 * Control profile.
 */
class AvrcpCtService : public IProfileAvrcpCt, public utility::Context {
public:
    /**
     * @brief A constructor used to create an <b>AvrcpCtService</b> instance.
     */
    AvrcpCtService();

    /**
     * @brief A destructor used to delete the <b>AvrcpCtService</b> instance.
     */
    ~AvrcpCtService();

    utility::Context *GetContext() override;

    /******************************************************************
     * REGISTER / UNREGISTER OBSERVER                                 *
     ******************************************************************/

    /**
     * @brief Registers the observer.
     *
     * @param observer The instance of the struct <b>IProfileAvrcpCt::Observer</b>.
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
     * @brief Enables the AVRCP CT service.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @return The result of the method execution.
     * @retval true  Execute success.
     * @retval false Execute failure.
     */
    void Enable(void) override;

    /**
     * @brief Disables the AVRCP CT service.
     *
     * @detail Switch to the thread of the AVRCP CT service in this method.
     * @return The result of the method execution.
     * @retval true  Execute success.
     * @retval false Execute failure.
     */
    void Disable(void) override;

    /**
     * @brief Checks whether the AVRCP CT service is enabled.
     *
     * @return The result of the method execution.
     * @retval true  The service is enabled.
     * @retval false The service is disabled.
     */
    bool IsEnabled(void) override;

    /******************************************************************
     * CONNECTION                                                     *
     ******************************************************************/

    /**
     * @brief Useless.
     */
    std::list<RawAddress> GetConnectDevices(void) override
    {
        std::list<RawAddress> rawAddrs;
        return rawAddrs;
    }

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
     * @brief Gets the max number of the connection.
     *
     * @return The max number of the connection.
     */
    int GetMaxConnectNum(void) override;

    /**
     * @brief Connects to the AVRCP TG service.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int Connect(const RawAddress &rawAddr) override;

    /**
     * @brief Disconnects from the AVRCP TG service.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int Disconnect(const RawAddress &rawAddr) override;

    /**
     * @brief Gets the connect states.
     *
     * @return The connect states.
     */
    int GetConnectState(void) override;

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
     */
    int PressButton(const RawAddress &rawAddr, uint8_t button) override;

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
     */
    int ReleaseButton(const RawAddress &rawAddr, uint8_t button) override;

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
     */
    int GetUnitInfo(const RawAddress &rawAddr) override;

    /**
     * @brief Gets the sub unit information.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int GetSubUnitInfo(const RawAddress &rawAddr) override;

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
     */
    int SetAddressedPlayer(const RawAddress &rawAddr, uint16_t playerId) override;

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
     */
    int SetBrowsedPlayer(const RawAddress &rawAddr, uint16_t playerId) override;

    /******************************************************************
     * Capabilities                                                   *
     ******************************************************************/

    /**
     * @brief Get the supported companies by remote device.
     *
     * @details This is sent by CT to get the capabilities of the peer device.
     * @param[in] rawAddr      The address of the bluetooth device.
     * @param[in] capabilityId Specific capability requested.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int GetSupportedCompanies(const RawAddress &rawAddr) override;

    /**
     * @brief Get the supported events by remote device.
     *
     * @details This is sent by CT to get the capabilities of the peer device.
     * @param[in] rawAddr      The address of the bluetooth device.
     * @param[in] capabilityId Specific capability requested.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int GetSupportedEvents(const RawAddress &rawAddr) override;

    /******************************************************************
     * PLAYER APPLICATION SETTINGS                                    *
     ******************************************************************/

    /**
     * @brief Gets the attribute of the player application.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int GetPlayerAppSettingAttributes(const RawAddress &rawAddr) override;

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
     */
    int GetPlayerAppSettingValues(const RawAddress &rawAddr, uint8_t attribute) override;

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
     */
    int GetPlayerAppSettingCurrentValue(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes) override;

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
     */
    int SetPlayerAppSettingCurrentValue(
        const RawAddress &rawAddr, const std::vector<uint8_t> &attributes, const std::vector<uint8_t> &values) override;

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
     */
    int GetPlayerAppSettingAttributeText(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes) override;

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
     */
    int GetPlayerAppSettingValueText(
        const RawAddress &rawAddr, uint8_t attributeId, const std::vector<uint8_t> &values) override;

    /******************************************************************
     * MEDIA INFORMATION PDUS                                         *
     ******************************************************************/

    /**
     * @brief Requests the TG to provide the attributes of the element specified in the parameter.
     *
     * @details Switch to the thread of the AVRCP CT service in this function.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] identifier Unique identifier to identify an element on TG
     * @param[in] attributes Specifies the attribute ID for the attributes to be retrieved
     *            @c RET_NO_ERROR   : The action is successful.
     *            @c RET_NO_SUPPORT : The action is not supported.
     *            @c RET_BAD_STATUS : The action is failed.
     */
    int GetElementAttributes(
        const RawAddress &rawAddr, uint64_t identifier, const std::vector<uint32_t> &attributes) override;

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
     */
    int GetPlayStatus(const RawAddress &rawAddr) override;

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
     */
    int PlayItem(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter) override;

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
     */
    int AddToNowPlaying(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter) override;

    /******************************************************************
     * CONTINUING RESPONSE / ABORT CONTINUING RESPONSE                *
     ******************************************************************/

    /**
     * @brief Requests continuing response.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pduId   The PDU ID which wants to request.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int RequestContinuingResponse(const RawAddress &rawAddr, uint8_t pduId) override;

    /**
     * @brief Aborts continuing response.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pduId   The PDU ID which wants to abort.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int AbortContinuingResponse(const RawAddress &rawAddr, uint8_t pduId) override;

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
     */
    int ChangePath(const RawAddress &rawAddr, uint16_t uidCounter, uint8_t direction, uint64_t folderUid) override;

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
     */
    int GetFolderItems(const RawAddress &rawAddr, uint8_t scope, uint32_t startItem, uint32_t endItem,
        const std::vector<uint32_t> &attributes) override;

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
     */
    int GetItemAttributes(const RawAddress &rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter,
        const std::vector<uint32_t> &attributes) override;

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
     */
    int GetTotalNumberOfItems(const RawAddress &rawAddr, uint8_t scope) override;

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
     */
    int SetAbsoluteVolume(const RawAddress &rawAddr, uint8_t volume) override;

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
     */
    int EnableNotification(const RawAddress &rawAddr, const std::vector<uint8_t> &events,
        uint8_t interval = AVRC_PLAYBACK_INTERVAL_1_SEC) override;

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
     */
    int DisableNotification(const RawAddress &rawAddr, const std::vector<uint8_t> &events) override;

private:
    /// using a local lock_guard to lock mutex guarantees unlocking on destruction / exception:
    std::mutex mutex_ {};
    /// The maximum connection of device.
    int maxConnection_ {AVRC_CT_DEFAULT_MAX_OF_CONN};
    /// current connection num.
    int currentConn_ = 0;
    /// The flag is used to indicate that the state of the AVRCP CT service.
    std::atomic_uint8_t state_ {AVRC_CT_SERVICE_STATE_DISABLED};
    /// The features supported by the AVRCP CT service.
    uint16_t features_ {AVRC_CT_FEATURE_INVALID_FEATURE};

    /// The observer registered by the AVRCP CT framework.
    /// @see AvrcCtProfile::Observer
    IObserver *myObserver_ {nullptr};
    /// The shared pointer to the observer registered into the AVRCP CT profile.
    /// @see AvrcCtProfile::Observer
    std::unique_ptr<AvrcCtProfile::Observer> pfObserver_ {nullptr};

    /// The unique pointer to an object of the AvrcCtGapManager class.
    /// @see AvrcCtGapManager
    std::unique_ptr<AvrcCtGapManager> gapManager_ {nullptr};
    /// A unique pointer to an object of the AvrcCtSdpManager class.
    /// @see AvrcCtSdpManager
    std::unique_ptr<AvrcCtSdpManager> sdpManager_ {nullptr};
    /// The unique pointer to an object of the AvrcCtProfile class.
    /// @see AvrcCtProfile
    std::unique_ptr<AvrcCtProfile> profile_ {nullptr};
    /******************************************************************
     * ENABLE / DISABLE                                               *
     ******************************************************************/

    /**
     * @brief Enables the AVRCP CT service.
     */
    void EnableNative(void);

    /**
     * @brief Disables the AVRCP CT service.
     */
    void DisableNative(void);

    /**
     * @brief Enables the AVRCP CT profile.
     *
     * @return The result of the method execution.
     * @retval true  Execute success.
     * @retval false Execute failure.
     */
    int EnableProfile(void);

    /**
     * @brief Disables the AVRCP CT profile.
     *
     * @return The result of the method execution.
     * @retval true  Execute success.
     * @retval false Execute failure.
     */
    int DisableProfile(void) const;

    /**
     * @brief Informs the profile is disabled.
     *
     * @param[in] result The result of the disable.
     */
    void OnProfileDisabled(int result = RET_NO_ERROR);

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
     * @brief Registers the service record into the SDP.
     *
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int RegisterService(void);

    /**
     * @brief Unregisters the service record from the SDP.
     */
    int UnregisterService(void);

    /**
     * @brief Checks whether the AVRCP CT service is disabled.
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
     * @brief Connects to the AVRCP TG service.
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
     * @brief Informs the connection state was changed.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] state   The connection state. Refer to <b>BTConnectState</b>.
     */
    void OnConnectionStateChanged(const RawAddress &rawAddr, int state) const;

    /**
     * @brief Accepts the active connection.
     *
     * @param[in] rawAddr The address of the peer bluetooth device.
     */
    void AcceptActiveConnect(const RawAddress &rawAddr);

    /**
     * @brief Rejects the active connection.
     *
     * @param[in] rawAddr The address of the peer bluetooth device.
     */
    void RejectActiveConnect(const RawAddress &rawAddr) const;

    /**
     * @brief Finds the service record from the SDP.
     *
     * @param[in] rawAddr  The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_BAD_STATUS Execute failure.
     */
    int FindTgService(const RawAddress &rawAddr) const;

    /**
     * @brief The callback function that receives the search result return from the SDP.
     *
     * @param[in] btAddr      The address of the peer Bluetooth device.
     * @param[in] handleArray The list of handle to a qualifying service.
     * @param[in] handleCount The number of handle to a qualifying service.
     * @param[in] context     The context is used to send the event in the callback.
     */
    static void FindTgServiceCallback(
        const BtAddr *btAddr, const uint32_t *handleArray, uint16_t handleCount, void *context);

    /******************************************************************
     * BUTTON OPERATION                                               *
     ******************************************************************/

    /**
     * @brief Presses the button.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] button  The code of the button pressed.
     */
    void PressButtonNative(RawAddress rawAddr, uint8_t button);

    /**
     * @brief Releases the button.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] button  The code of the button released.
     */
    void ReleaseButtonNative(RawAddress rawAddr, uint8_t button);

    /**
     * @brief Informs to the result of pressing button that receives from the peer device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] button  The code  of the key operation.
     * @param[in] result  The result of the execution.<br>
     *            @a RET_NO_ERROR   : Execute success.<br>
     *            @a RET_NO_SUPPORT : Not support.<br>
     *            @a RET_BAD_STATUS : Execute failure.
     */
    void OnButtonPressed(const RawAddress &rawAddr, uint8_t button, int result) const;

    /**
     * @brief Informs to the result of releasing button that receives from the peer device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] button  The code of the button released.
     * @param[in] result  The result of the execution.<br>
     *            @a RET_NO_ERROR   : Execute success.<br>
     *            @a RET_NO_SUPPORT : Not support.<br>
     *            @a RET_BAD_STATUS : Execute failure.
     */
    void OnButtonReleased(const RawAddress &rawAddr, uint8_t button, int result) const;

    /******************************************************************
     * UNIT INFO / SUB UNIT INFO                                      *
     ******************************************************************/

    /**
     * @brief Gets the unit information.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void UnitInfoNative(RawAddress rawAddr);

    /**
     * @brief Gets the sub unit information.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void SubUnitInfoNative(RawAddress rawAddr);

    /******************************************************************
     * Media Player Selection                                         *
     ******************************************************************/

    /**
     * @brief Informs which media player wishes to control.
     *
     * @param[in] rawAddr  The address of the bluetooth device.
     * @param[in] playerId The unique media player id.
     */
    void SetAddressedPlayerNative(RawAddress rawAddr, uint16_t playerId);

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
     *            @a AVRC_ES_CODE_INVALID_PLAYER_ID
     *            @a AVRC_ES_CODE_NO_AVAILABLE_PLAYERS
     */
    void OnSetAddressedPlayer(const RawAddress &rawAddr, int result, int detail) const;

    /**
     * @brief Informs to which player browsing commands should be routed.
     *
     * @param[in] rawAddr  The address of the bluetooth device.
     * @param[in] playerId The unique media player id.
     */
    void SetBrowsedPlayerNative(RawAddress rawAddr, uint16_t playerId);

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
    void OnSetBrowsedPlayer(const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems,
        const std::vector<std::string> &folderNames, int result, int detail) const;

    /******************************************************************
     * Capabilities                                                   *
     ******************************************************************/

    /**
     * @brief Get the capabilities supported by remote device.
     *
     * @details This is sent by CT to get the capabilities of the peer device
     * @param[in] rawAddr      The address of the bluetooth device.
     * @param[in] capabilityId Specific capability requested
     */
    void GetCapabilitiesNative(RawAddress rawAddr, uint8_t capabilityId);

    /**
     * @brief Responds the data of the <b>GetCapabilities</b>.
     *
     * @param[in] addr         The address of the bluetooth device.
     * @param[in] companies    The list of CompanyID.
     * @param[in] events       The list of EventIDs
     * @param[in] result       The result of the execution.<br>
     *            @a RET_NO_ERROR   : Execute success.<br>
     *            @a RET_NO_SUPPORT : Not support.<br>
     *            @a RET_BAD_STATUS : Execute failure.
     */
    void OnGetCapabilities(const RawAddress &rawAddr, const std::vector<uint32_t> &companies,
        const std::vector<uint8_t> &events, int result) const;
    /******************************************************************
     * PLAYER APPLICATION SETTINGS                                    *
     ******************************************************************/

    /**
     * @brief Gets the attributes of the player application.
     *
     * @param[in] rawAddr   The address of the bluetooth device.
     */
    void GetPlayerAppSettingAttributesNative(RawAddress rawAddr);

    /**
     * @brief Responds the data of the <b>GetPlayerAppSettingAttributes</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of player application.
     * @param[in] result     The result of the execution.<br>
     *            @a RET_NO_ERROR   : Execute success.<br>
     *            @a RET_NO_SUPPORT : Not support.<br>
     *            @a RET_BAD_STATUS : Execute failure.
     */

    void OnGetPlayerAppSettingAttribtues(
        const RawAddress &rawAddr, const std::vector<uint8_t> &attributes, int result) const;

    /**
     * @brief Gets the values of the specified attribute of the player application.
     *
     * @param[in] rawAddr   The address of the bluetooth device.
     * @param[in] attribute The attribute of the player application setting. Refer to <b>AvrcPlayerAttribute</b>.
     */
    void GetPlayerAppSettingValuesNative(RawAddress rawAddr, uint8_t attribute);

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
     */
    void OnGetPlayerAppSettingValues(
        const RawAddress &rawAddr, uint8_t attribute, const std::vector<uint8_t> &values, int result) const;

    /**
     * @brief Gets the current set values on the target for the provided player application setting attributes list.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings. Refer to <b>AvrcPlayerAttribute</b>.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not Support.
     * @retval RET_BAD_STATUS Execute failure.
     */
    void GetPlayerAppSettingCurrentValueNative(RawAddress rawAddr, std::vector<uint8_t> attributes);

    /**
     * @brief Responds the data of the <b>GetCurrentPlayerAppSettingValue</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attributes of the player application setting. Refer to <b>AvrcPlayerAttribute</b>.
     * @param[in] values     The values associated witch the attribute.
     * @param[in] result     The result of the execution.<br>
     *            @a RET_NO_ERROR   : Execute success.<br>
     *            @a RET_NO_SUPPORT : Not support.<br>
     *            @a RET_BAD_STATUS : Execute failure.
     */
    void OnGetPlayerAppSettingCurrentValue(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes,
        const std::vector<uint8_t> &values, int result) const;

    /**
     * @brief Sets the player application setting list of player application setting values on the target device for the
     * corresponding defined list of AvrcPlayerAttribute.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings. Refer to <b>AvrcPlayerAttribute</b>.
     * @param[in] values     The value of the player application setting attribute.
     */
    void SetPlayerAppSettingCurrentValueNative(
        RawAddress rawAddr, std::vector<uint8_t> attributes, std::vector<uint8_t> values);

    /**
     * @brief Responds the data of the <b>SetPlayerAppSettingCurrentValue</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] result     The result of the execution.<br>
     *            @a RET_NO_ERROR   : Execute success.<br>
     *            @a RET_NO_SUPPORT : Not support.<br>
     *            @a RET_BAD_STATUS : Execute failure.
     */
    void OnSetPlayerAppSettingCurrentValue(const RawAddress &rawAddr, int result) const;

    /**
     * @brief  provide supported player application setting attribute displayable text.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] attributes The attribute of the player application settings.
     */
    void GetPlayerAppSettingAttributeTextNative(RawAddress rawAddr, std::vector<uint8_t> attributes);

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
    void OnGetPlayerAppSettingAttributeText(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes,
        const std::vector<std::string> &attrStr, int result) const;

    /**
     * @brief  request the target device to provide target supported player application setting value displayable text
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr     The address of the bluetooth device.
     * @param[in] attributeId Player application setting attribute ID.
     * @param[in] values      Player application setting value ID.
     */
    void GetPlayerAppSettingValueTextNative(RawAddress rawAddr, uint8_t attributeId, std::vector<uint8_t> values);

    /**
     * @brief Responds the data of the <b>GetPlayerApplicationSettingValueText</b>.
     *
     * @param[in] rawAddr  The address of the bluetooth device.
     * @param[in] values   Player application setting value ID.
     * @param[in] valueStr Specifies the value string of player application setting value.
     * @param[in] result   The result of the execution.<br>
     *            @a RET_NO_ERROR   : Execute success.<br>
     *            @a RET_NO_SUPPORT : Not support.<br>
     *            @a RET_BAD_STATUS : Execute failure.
     */
    void OnGetPlayerAppSettingValueText(const RawAddress &rawAddr, const std::vector<uint8_t> &values,
        const std::vector<std::string> &valueStr, int result) const;

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
     */
    void GetElementAttributesNative(RawAddress rawAddr, uint64_t identifier, std::vector<uint32_t> attributes);

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
     */
    void OnGetElementAttributes(const RawAddress &rawAddr, const std::vector<uint32_t> &attributes,
        const std::vector<std::string> &values, int result) const;

    /******************************************************************
     * PLAY                                                           *
     ******************************************************************/

    /**
     * @brief Gets the play status.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void GetPlayStatusNative(RawAddress rawAddr);

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
    void OnGetPlayStatus(
        const RawAddress &rawAddr, uint32_t songLength, uint32_t songPosition, uint8_t playStatus, int result) const;

    /**
     * @brief Starts playing an item indicated by the UID.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] scope     The scope in which media content navigation may take place. Refer to <b>AvrcMediaScope</b>.
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] uid        The unique ID of media item.
     * @param[in] uidCounter The UID counter shall be incremented every time the TG makes an update.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     */
    void PlayItemNative(RawAddress rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter);

    /**
     * @brief Responds the data of the <b>PlayItem</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] result  The result of the execution.<br>
     *            @a RET_NO_ERROR   : Execute success.<br>
     *            @a RET_NO_SUPPORT : Not support.<br>
     *            @a RET_BAD_STATUS : Execute failure.
     * @param[in] detail  The detail result of the execution. Refer to <b>AvrcEsCode</b>.<br>
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
    void OnPlayItem(const RawAddress &rawAddr, int result, int detail) const;

    /**
     * @brief Adds an item indicated by the UID to the Now Playing queue.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] scope      The scope in which media content navigation may take place. Refer to <b>AvrcMediaScope</b>.
     * @param[in] uid        The UID of the media element item or folder item.
     * @param[in] uidCounter The UID Counter.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_NO_SUPPORT Not support.
     * @retval RET_BAD_STATUS Execute failure.
     */
    void AddToNowPlayingNative(RawAddress rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter);

    /**
     * @brief Responds the data of the <b>AddToNowPlaying</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
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
     */
    void OnAddToNowPlaying(const RawAddress &rawAddr, int result, int detail) const;

    /******************************************************************
     * CONTINUING RESPONSE / ABORT CONTINUING RESPONSE                *
     ******************************************************************/

    /**
     * @brief Requests continuing response.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pduId   The PDU ID which wants to request.
     */
    void RequestContinuingResponseNative(RawAddress rawAddr, uint8_t pduId);

    /**
     * @brief Aborts continuing response.
     *
     * @details Switch to the thread of the AVRCP CT service in this method.
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pduId   The PDU ID which wants to abort.
     */
    void AbortContinuingResponseNative(RawAddress rawAddr, uint8_t pduId);

    /******************************************************************
     * OPERATE THE VIRTUAL FILE SYSTEM                                *
     ******************************************************************/

    /**
     * @brief Navigates one level up or down in the virtual file system.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] uidCounter The value of the uid counter.
     * @param[in] direction  The flag of the navigation. Refer to <b>AvrcFolderDirection</b>.
     * @param[in] folderUid  The UID of the folder to navigate to. This may be retrieved via a GetFolderItems command.
     * If the navigation command is Folder Up this field is reserved.
     */
    void ChangePathNative(RawAddress rawAddr, uint16_t uidCounter, uint8_t direction, uint64_t folderUid);

    /**
     * @brief Responds the data of the <b>ChangePath</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] numOfItems The number of items in the directory.
     * @param[in] result  The result of the execution.<br>
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
    void OnChangePath(const RawAddress &rawAddr, uint32_t numOfItems, int result, int detail) const;

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
     */
    void GetFolderItemsNative(
        RawAddress rawAddr, uint8_t scope, uint32_t startItem, uint32_t endItem, std::vector<uint32_t> attributes);

    /**
     * @brief Responds the data of the <b>GetFolderItems</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] scope      The scope in which media content navigation may take place. Refer to
     * <b>AvrcMediaScope</b>.
     * @param[in] uidCounter The UID Counter.
     * @param[in] mpItems    The list of <b>AvrcCtMpItem</b> class.
     * @param[in] mpItems    The list of <b>AvrcCtMeItem</b> class.
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
    void OnGetFolderItems(const RawAddress &rawAddr, uint8_t scope, uint16_t uidCounter,
        const std::vector<AvrcMpItem> &mpItems, const std::vector<AvrcMeItem> &meItems, int result, int detail) const;

    /**
     * @brief Retrieves the metadata attributes for a particular media element item or folder item.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] scope      The scope in which media content navigation may take place. Refer to <b>AvrcMediaScope</b>.
     * @param[in] uid        The UID of the media element item or folder item.
     * @param[in] uidCounter The UID Counter.
     * @param[in] attributes The list of media attributes.
     */
    void GetItemAttributesNative(
        RawAddress rawAddr, uint8_t scope, uint64_t uid, uint16_t uidCounter, std::vector<uint32_t> attributes);

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
    void OnGetItemAttributes(const RawAddress &rawAddr, const std::vector<uint32_t> &attributes,
        const std::vector<std::string> &values, int result, int detail) const;

    /**
     * @brief Gets the Number of Items in the selected folder at the selected scope.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] scope   The scope in which media content navigation may take place. Refer to <b>AvrcMediaScope</b>.
     */
    void GetTotalNumberOfItemsNative(RawAddress rawAddr, uint8_t scope);

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
    void OnGetTotalNumberOfItems(
        const RawAddress &rawAddr, uint16_t uidCounter, uint32_t numOfItems, int result, int detail) const;

    /******************************************************************
     * ABSOLUTE VOLUME                                                *
     ******************************************************************/

    /**
     * @brief Sets an absolute volume to be used by the rendering device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] volume  The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
     */
    void SetAbsoluteVolumeNative(RawAddress rawAddr, uint8_t volume);

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
    void OnSetAbsoluteVolume(const RawAddress &rawAddr, uint8_t volume, int result) const;

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
     */
    void EnableNotificationNative(RawAddress rawAddr, std::vector<uint8_t> events, uint8_t interval);

    /**
     * @brief Disables for receiving notifications asynchronously based on specific events occurring.
     *
     * @param[in] rawAddr The address of the bluetooth device.d
     * @param[in] events  The event for which the requires notification. Refer to <b>AvrcEventId</b>.
     */
    void DisableNotificationNative(RawAddress rawAddr, std::vector<uint8_t> events);

    /**
     * @brief Responds the data of the notification <b>EVENT_PLAYBACK_STATUS_CHANGED</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] playStatus The current status of playing. Refer to <b>AvrcPlayStatus</b>.
     * @param[in] result  The result of the execution.<br>
     *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
     *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
     */
    void OnPlaybackStatusChanged(const RawAddress &rawAddr, uint8_t playStatus, int result) const;

    /**
     * @brief Responds the data of the notification <b>EVENT_TRACK_CHANGED</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] uid     The unique ID of media item.
     * @param[in] result  The result of the execution.<br>
     *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
     *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
     */
    void OnTrackChanged(const RawAddress &rawAddr, uint64_t uid, int result) const;

    /**
     * @brief Responds the data of the notification <b>EVENT_TRACK_REACHED_END</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] result  The result of the execution.<br>
     *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
     *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
     */
    void OnTrackReachedEnd(const RawAddress &rawAddr, int result) const;

    /**
     * @brief Responds the data of the notification <b>EVENT_TRACK_REACHED_START</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] result  The result of the execution.<br>
     *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
     *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
     */
    void OnTrackReachedStart(const RawAddress &rawAddr, int result) const;

    /**
     * @brief Responds the data of the notification <b>EVENT_PLAYBACK_POS_CHANGED</b>.
     *
     * @param[in] rawAddr     The address of the bluetooth device.
     * @param[in] playbackPos Current playback position in millisecond.
     * @param[in] result  The result of the execution.<br>
     *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
     *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
     */
    void OnPlaybackPosChanged(const RawAddress &rawAddr, uint32_t playbackPos, int result) const;

    /**
     * @brief Responds the data of the notification <b>EVENT_PLAYER_APPLICATION_SETTING_CHANGED</b>.
     *
     * @param[in] rawAddr   The address of the bluetooth device.
     * @param[in] attribute The attribute of the player application setting. Refer to <b>AvrcPlayerAttribute</b>.
     * @param[in] values    The values associated witch the attribute.
     * @param[in] result  The result of the execution.<br>
     *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
     *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
     */
    void OnPlayerApplicationSettingChanged(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes,
        const std::vector<uint8_t> &values, int result) const;

    /**
     * @brief Responds the data of the notification <b>EVENT_NOW_PLAYING_CONTENT_CHANGED</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] result  The result of the execution.<br>
     *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
     *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
     */
    void OnNowPlayingContentChanged(const RawAddress &rawAddr, int result) const;

    /**
     * @brief Responds the data of the notification <b>EVENT_AVAILABLE_PLAYERS_CHANGED</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] result  The result of the execution.<br>
     *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
     *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
     */
    void OnAvailablePlayersChanged(const RawAddress &rawAddr, int result) const;

    /**
     * @brief Responds the data of the notification <b>EVENT_ADDRESSED_PLAYER_CHANGED</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] playerId   The unique media player id.
     * @param[in] uidCounter The UID counter shall be incremented every time the TG makes an update.
     * @param[in] result  The result of the execution.<br>
     *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
     *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
     */
    void OnAddressedPlayerChanged(const RawAddress &rawAddr, uint16_t playerId, uint16_t uidCounter, int result) const;

    /**
     * @brief Responds the data of the notification <b>EVENT_UIDS_CHANGED</b>.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] uidCounter The UID counter shall be incremented every time the TG makes an update.
     * @param[in] result  The result of the execution.<br>
     *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
     *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
     */
    void OnUidChanged(const RawAddress &rawAddr, uint16_t uidCounter, int result) const;

    /**
     * @brief Responds the data of the notification <b>EVENT_VOLUME_CHANGED</b>.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] volume  The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
     * @param[in] result  The result of the execution.<br>
     *            @a AVRC_ES_CODE_NO_ERROR             : Execute success.<br>
     *            @a AVRC_ES_CODE_NOTIFICATION_CHANGED : Registered notification changed<br>
     */
    void OnVolumeChanged(const RawAddress &rawAddr, uint8_t volume, int result) const;

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
     * @brief Receives events from the AVCT protocol.
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
     * @brief The callback function is registered into the AVCTP.
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

    BT_DISALLOW_COPY_AND_ASSIGN(AvrcpCtService);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_TG_SERVICE_H
