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
 * @file bluetooth_avrcp_tg.h
 *
 * @brief Declares the class of the AVRCP target framework, including attributes and methods.
 *
 * @since 6
 */

#ifndef BLUETOOTH_AVRCP_TG_H
#define BLUETOOTH_AVRCP_TG_H

#include <vector>

#include "bluetooth_def.h"
#include "bluetooth_types.h"
#include "bluetooth_remote_device.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief This class provides a set of methods for operating the AVRCP target.
 *
 * @since 6
 */
class BLUETOOTH_API AvrcpTarget {
public:
    /**
     * @brief This abstract class declares a set of methods for observing the <b>AvrcpTarget::IObserver</b> class.
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
    };

    /**
     * @brief Gets the static instance of the <b>AvrcpTarget</b> class.
     *
     * @return The static instance.
     *
     * @since 6
     */
    static AvrcpTarget *GetProfile(void);

    /******************************************************************
     * REGISTER / UNREGISTER OBSERVER                                 *
     ******************************************************************/

    /**
     * @brief Registers the observer.
     *
     * @param[in] observer The pointer to the <b>AvrcpTarget::IObserver</b>.
     * @since 6
     */
    void RegisterObserver(AvrcpTarget::IObserver *observer);

    /**
     * @brief Unregisters the observer.
     *
     * @param[in] observer The pointer to the <b>AvrcpTarget::IObserver</b>.
     * @since 6
     */
    void UnregisterObserver(AvrcpTarget::IObserver *observer);

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
    void SetActiveDevice(const BluetoothRemoteDevice &device);

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
    std::vector<BluetoothRemoteDevice> GetDevicesByStates(std::vector<int> states);

    /**
     * @brief Gets the connection state of the specified bluetooth device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The connection state. Refer to <b>BTConnectState</b>.
     *
     * @since 6
     */
    int GetDeviceState(const BluetoothRemoteDevice &device);

        /**
     * @brief Connects to the AVRCP CT service.
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
     * @brief Disconnects from the AVRCP CT service.
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
     * NOTIFICATION                                                   *
     ******************************************************************/

    /**
     * @brief Notifies the playback status is changed.
     *
     * @param[in] uid         The unique ID of media item.
     * @param[in] playbackPos Current playback position in millisecond.
     *
     * @since 6
     */
    void NotifyPlaybackStatusChanged(uint8_t playStatus, uint32_t playbackPos);

    /**
     * @brief Notifies the track is changed.
     *
     * @param[in] uid         The unique ID of media item.
     * @param[in] playbackPos Current playback position in millisecond.
     *
     * @since 6
     */
    void NotifyTrackChanged(uint64_t uid, uint32_t playbackPos);

    /**
     * @brief Notifies the track reached end is changed.
     *
     * @param[in] playbackPos Current playback position in millisecond.
     *
     * @since 6
     */
    void NotifyTrackReachedEnd(uint32_t playbackPos);

    /**
     * @brief Notifies the track reached start is changed.
     *
     * @param[in] playbackPos Current playback position in millisecond.
     *
     * @since 6
     */
    void NotifyTrackReachedStart(uint32_t playbackPos);

    /**
     * @brief Notifies the player application setting is changed.
     *
     * @param[in] playbackPos Current playback position in millisecond.
     *
     * @since 6
     */
    void NotifyPlaybackPosChanged(uint32_t playbackPos);

    /**
     * @brief Notifies the player application setting is changed.
     *
     * @since 6
     */
    void NotifyPlayerAppSettingChanged(const std::vector<uint8_t> &attributes, const std::vector<uint8_t> &values);

    /**
     * @brief Notifies the addressed player is changed.
     *
     * @since 6
     */
    void NotifyNowPlayingContentChanged(void);

    /**
     * @brief Notifies that has the available player.
     *
     * @since 6
     */
    void NotifyAvailablePlayersChanged(void);

    /**
     * @brief Notifies the addressed player is changed.
     *
     * @param[in] playerId   The unique media player id.
     * @param[in] uidCounter The UID counter shall be incremented every time the TG makes an update.
     *
     * @since 6
     */
    void NotifyAddressedPlayerChanged(uint16_t playerId, uint16_t uidCounter);

    /**
     * @brief Notifies the uids is changed.
     *
     * @param[in] uidCounter The UID counter shall be incremented every time the TG makes an update.
     *
     * @since 6
     */
    void NotifyUidChanged(uint16_t uidCounter);

    /**
     * @brief Notifies the absolute volume is changed.
     *
     * @param[in] volume The percentage of the absolute volume. Refer to <b>AvrcAbsoluteVolume</b>.
     *
     * @since 6
     */
    void NotifyVolumeChanged(uint8_t volume);

private:
    /**
     * @brief A constructor used to create an <b>AvrcpTarget</b> instance.
     *
     * @since 6
     */
    AvrcpTarget();

    /**
     * @brief A destructor used to delete the <b>AvrcpTarget</b> instance.
     *
     * @since 6
     */
    ~AvrcpTarget();

    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(AvrcpTarget);
    BLUETOOTH_DECLARE_IMPL();
};
}  // namespace Bluetooth
}  // namespace OHOS

#endif  // !BLUETOOTH_AVRCP_TG_H
