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

#ifndef AVRCP_TG_CONNECTION_H
#define AVRCP_TG_CONNECTION_H

#include "avrcp_tg_browse.h"
#include "avrcp_tg_internal.h"
#include "avrcp_tg_pass_through.h"
#include "avrcp_tg_sub_unit_info.h"
#include "avrcp_tg_unit_info.h"
#include "avrcp_tg_vendor.h"
#include "packet.h"
#include "time.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This class provides a set of attributes for saving the connection information.
 */
class AvrcTgConnectInfo {
public:
    /**
     * @brief A constructor used to create an <b>AvrcTgConnectInfo</b> instance.
     */
    AvrcTgConnectInfo(const std::string &btAddr, uint8_t connectId, uint8_t role, uint16_t controlMtu,
        uint16_t browseMtu, uint32_t companyId, uint16_t uidCounter, AvctChannelEventCallback eventCallback,
        AvctMsgCallback msgCallback);

    /**
     * @brief A destructor used to delete the <b>AvrcTgConnectInfo</b> instance.
     */
    ~AvrcTgConnectInfo();

    /**
     * @brief A deleted default constructor.
     */
    AvrcTgConnectInfo() = delete;

    // The pointer to a callback function for receiving the events, which has been registered to the AVCTP.
    AvctChannelEventCallback eventCallback_ {nullptr};
    // The pointer to a callback function for receiving the messages, which has been registered to the AVCTP.
    AvctMsgCallback msgCallback_ {nullptr};
    uint8_t connectId_;                // The id of the connection.
    uint8_t role_;                     // The role of the connection.(Initiator or Acceptor)
    uint16_t controlMtu_;              // The the maximum transmission unit of the control channel.
    uint16_t browseMtu_;               // The the maximum transmission unit of the browse channel.
    uint32_t companyId_;               // The SIG company ID of the bluetooth device.
    uint16_t uidCounter_;              // The UID counter.
    std::string btAddr_;               // The address of the bluetooth device.
    std::map<uint8_t, bool> notes_ {};  // The notifications are enabled or not.

    /**
     * @brief This struct provides a set of attributes for saving the data of the <b>PASS THROUGH</b> command.
     */
    struct PassInfo {
        // The shared pointer to the instance of the <b>utility::Timer</b>
        std::shared_ptr<utility::Timer> timer_ {nullptr};
        // The shared pointer to the instance of the <b>AvrcTgPassPacket</b>
        std::shared_ptr<AvrcTgPassPacket> pkt_ {nullptr};
    } ptInfo_ {};

    /**
     * @brief This struct provides a set of attributes for saving the data of the <b>UNIT INFO</b> and <b>SUB UNIT
     * INFO</b> command.
     */
    struct UnitInfo {
        // The shared pointer to the instance of the <b>utility::Timer</b>.
        std::shared_ptr<utility::Timer> timer_ {nullptr};
        // The shared pointer to the instance of the <b>AvrcTgUnitPacket</b>.
        std::shared_ptr<AvrcTgUnitPacket> pkt_ {nullptr};
    } unInfo_ {};

    /**
     * @brief This struct provides the attributes for saving the data of the <b>VENDOR DEPENDENT</b> command.
     */
    struct VendorInfo {
        // The shared pointer to the instance of the <b>utility::Timer</b>.
        std::shared_ptr<utility::Timer> timer_ {nullptr};
        // The shared pointer to the instance of the <b>AvrcTgVendorPacket</b>.
        std::shared_ptr<AvrcTgVendorPacket> pkt_ {nullptr};
    } vdInfo_ {};

    /**
     * @brief This struct provides the attributes for saving the data of the <b>BROWSING</b> command.
     */
    struct BrowseInfo {
        // The shared pointer to the instance of the <b>utility::Timer</b>.
        std::shared_ptr<utility::Timer> timer_ {nullptr};
        // The shared pointer to the instance of the <b>AvrcTgBrowsePacket</b>.
        std::shared_ptr<AvrcTgBrowsePacket> pkt_ {nullptr};
    } brInfo_ {};
};

/**
 * @brief This class provides a set of methods for adding/deleting/getting/setting attributes of the connection
 * information.
 *
 * @detail This class implements the singleton pattern.
 */
class AvrcTgConnectManager {
public:
    /**
     * @brief A destructor used to delete the <b>AvrcTgConnectManager</b> instance.
     */
    ~AvrcTgConnectManager();

    /**
     * @brief Gets the instance.
     *
     * @return The instance of the AvrcCtConnectManager.
     */
    static AvrcTgConnectManager *GetInstance(void);

    /**
     * @brief Adds some properties of the connection information of the specified bluetooth address.
     *
     * @param[in] rawAddr      The address of the bluetooth device.
     * @param[in] connectId    The ID of the connection.
     * @param[in] role         The role of the connection.(Initiator of the connection or Acceptor of the connection)
     * @param[in] controlMtu   The MTU size of the control channel.
     * @param[in] browseMtu    The MTU size of the browse channel.
     * @param[in] companyId    The SIG company ID of the bluetooth device.
     * @param[in] uidCounter   The uid counter.
     * @param[in] eventCallback The callback function for receiving the channel event.
     * @param[in] msgCallback  The callback function for receiving the message.
     * @return @c RET_NO_ERROR  : The connection information was added successfully.
     *         @C RET_BAD_PARAM : The connection information already exists.
     */
    int Add(const RawAddress &rawAddr, uint8_t connectId, uint8_t role, uint16_t controlMtu, uint16_t browseMtu,
        uint32_t companyId, uint16_t uidCounter, AvctChannelEventCallback eventCallback, AvctMsgCallback msgCallback);

    /**
     * @brief Deletes the whole connection information of the specified bluetooth device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void Delete(const RawAddress &rawAddr);

    /**
     * @brief Gets the whole connection information of the specified bluetooth device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The read only pointer to the instance of the <b>AvrcTgConnectInfo</b> class.
     */
    const AvrcTgConnectInfo *GetConnectInfo(const RawAddress &rawAddr);

    /**
     * @brief Gets the raw address of the specified connection id.
     *
     * @param[in] connectId The connection id.
     * @return The raw address of the bluetooth device.
     */
    RawAddress GetRawAddress(uint8_t connectId) const;

    /**
     * @brief Gets the address of the connected devices.
     *
     * @return The list of the address.
     */
    std::list<std::string> GetDeviceAddresses(void);

    /**
     * @brief Sets the address of the active bluetooth device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void SetActiveDevice(const std::string addr);

    /**
     * @brief Gets the address of the active bluetooth device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    const std::string &GetActiveDevice(void);

    /**
     * @brief Adds the address of the disconnected bluetooth device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void AddDisconnectedDevice(const std::string addr);

    /**
     * @brief Deletes the address of the disconnected bluetooth device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void DeleteDisconnectedDevice(const std::string addr);

    /**
     * @brief Get all of the address of the disconnected bluetooth devices.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    const std::list<std::string> &GetAllDisconnectedDevices(void);

    /**
     * @brief Checks the connection information of the specified bluetooth device is empty or not.
     *
     * @return The result of the method execution.
     * @retval true  The connection information is empty.
     * @retval false The connection information is not empty.
     */
    bool IsConnectInfoEmpty(void) const;

    /**
     * @brief Gets the connection id of the specified bluetooth device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The connection id.
     */
    uint8_t GetConnectId(const RawAddress &rawAddr);

    /**
     * @brief Sets the connection id of the specified bluetooth device.
     *
     * @param[in] rawAddr   The address of the bluetooth device.
     * @param[in] connectId The connection id.
     */
    void SetConnectId(const RawAddress &rawAddr, uint8_t connectId);

    /**
     * @brief Gets the vendor mtu of the specified bluetooth device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The vendor mtu.
     */
    uint16_t GetControlMtu(const RawAddress &rawAddr);

    /**
     * @brief Sets the vendor mtu of the specified bluetooth device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] mtu     The vendor mtu.
     */
    void SetControlMtu(const RawAddress &rawAddr, uint16_t mtu);

    /**
     * @brief Gets the browse mtu of the specified bluetooth device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The browse mtu.
     */
    uint16_t GetBrowseMtu(const RawAddress &rawAddr);

    /**
     * @brief Sets the browse mtu of the specified bluetooth device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] mtu     The browse mtu.
     */
    void SetBrowseMtu(const RawAddress &rawAddr, uint16_t mtu);

    /**
     * @brief Enables the notification of the specified bluetooth device.
     *
     * @param[in] rawAddr     The address of the bluetooth device.
     * @param[in] notifyState The specified notify state.
     */
    void EnableNotifyState(const RawAddress &rawAddr, uint8_t notification);

    /**
     * @brief Disables the notification of the specified bluetooth device.
     *
     * @param[in] rawAddr      The address of the bluetooth device.
     * @param[in] notification The specified notify state.
     */
    void DisableNotifyState(const RawAddress &rawAddr, uint8_t notification);

    /**
     * @brief Checks the notification of the specified bluetooth device is enabled or not.
     *
     * @param[in] rawAddr      The address of the bluetooth device.
     * @param[in] notification The specified notify state.
     * @return The result of the method execution.
     * @retval true  The notification is enabled.
     * @retval false The notification is disabled.
     */
    bool IsNotifyStateEnabled(const RawAddress &rawAddr, uint8_t notification);

    /**
     * @brief Gets company id of the specified bluetooth device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The company id.
     */
    uint32_t GetCompanyId(const RawAddress &rawAddr);

    /**
     * @brief Gets the uid counter of the specified bluetooth device.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The uid counter.
     */
    uint16_t GetUidCounter(const RawAddress &rawAddr);

    /**
     * @brief Sets the uid counter of the specified bluetooth device.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] UidCounter The uid counter.
     */
    void SetUidCounter(const RawAddress &rawAddr, uint16_t uidCounter);

    /******************************************************************
     * PASS THROUGH COMMAND                                           *
     ******************************************************************/

    /**
     * @brief Gets the frame packet related to the <b>PASS THROUGH</b> command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The shared pointer of the frame packet.
     */
    std::shared_ptr<AvrcTgPassPacket> GetPassPacket(const RawAddress &rawAddr);

    /**
     * @brief Sets the frame packet related to the <b>PASS THROUGH</b> command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The shared pointer of the frame packet.
     */
    void SetPassPacket(const RawAddress &rawAddr, const std::shared_ptr<AvrcTgPassPacket> &pkt);

    /**
     * @brief Checks the timer related to the <b>PASS THROUGH</b> command is empty or not.
     *
     * @return The result of the method execution.
     * @retval true  The timer is empty.
     * @retval false The timer is not empty.
     */
    bool IsPassTimerEmpty(const RawAddress &rawAddr);

    /**
     * @brief Sets the timer related to the <b>PASS THROUGH</b> command.
     *
     * @param[in] rawAddr    The address of the bluetooth device.
     * @param[in] callback   The callback for processing timeout.
     * @param[in] ms         The number of milliseconds used to count.
     * @param[in] isPeriodic The timer is periodic or not.
     */
    void SetPassTimer(const RawAddress &rawAddr, std::function<void()> callback, int ms, bool isPeriodic = false);

    /**
     * @brief Clears the frame packet related to the <b>PASS THROUGH</b> command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void ClearPassPacket(const RawAddress &rawAddr);

    /**
     * @brief Clears the timer related to the <b>PASS THROUGH</b> command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void ClearPassTimer(const RawAddress &rawAddr);

    /**
     * @brief Clears all saved data related to the <b>PASS THROUGH</b> command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void ClearPassInfo(const RawAddress &rawAddr);

    /******************************************************************
     * UNIT INFO / SUB UNIT INFO COMMAND                              *
     ******************************************************************/

    /**
     * @brief Gets the frame packet related to the <b>UNIT INFO</b> and <b>SUB UNIT INFO</b> command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The shared pointer of the frame packet.
     */
    std::shared_ptr<AvrcTgUnitPacket> GetUnitPacket(const RawAddress &rawAddr);

    /**
     * @brief Sets the frame packet related to the <b>UNIT INFO</b> and <b>SUB UNIT INFO</b> command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void SetUnitPacket(const RawAddress &rawAddr, const std::shared_ptr<AvrcTgUnitPacket> &pkt);

    /**
     * @brief Clears the frame packet related to the <b>UNIT INFO</b> and <b>SUB UNIT INFO</b> command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void ClearUnitPacket(const RawAddress &rawAddr);

    /**
     * @brief Clears all saved data related to the <b>UNIT INFO</b> and <b>SUB UNIT INFO</b> command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void ClearUnitInfo(const RawAddress &rawAddr);

    /******************************************************************
     * VENDOR DEPENDENT COMMAND                                       *
     ******************************************************************/

    /**
     * @brief Gets the frame packet related to the <b>VENDOR DEPENDENT</b> command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The shared pointer of the frame packet.
     */
    std::shared_ptr<AvrcTgVendorPacket> GetVendorPacket(const RawAddress &rawAddr);

    /**
     * @brief Sets the frame packet related to the <b>VENDOR DEPENDENT</b> command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void SetVendorPacket(const RawAddress &rawAddr, const std::shared_ptr<AvrcTgVendorPacket> &pkt);

    /**
     * @brief Clears the frame packet related to the <b>VENDOR DEPENDENT</b> command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void ClearVendorPacket(const RawAddress &rawAddr);

    /**
     * @brief Clears all saved data related to the <b>VENDOR DEPENDENT</b> command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void ClearVendorInfo(const RawAddress &rawAddr);

    /******************************************************************
     * BROWSING COMMAND                                               *
     ******************************************************************/

    /**
     * @brief Gets the frame packet related to the <b>BROWSING</b> command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The shared pointer of the frame packet.
     */
    std::shared_ptr<AvrcTgBrowsePacket> GetBrowsePacket(const RawAddress &rawAddr);

    /**
     * @brief Sets the frame packet related to the <b>BROWSING</b> command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] pkt     The frame packet.
     */
    void SetBrowsePacket(const RawAddress &rawAddr, const std::shared_ptr<AvrcTgBrowsePacket> &pkt);

    /**
     * @brief Clears the frame packet related to the <b>BROWSING</b> command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void ClearBrowsePacket(const RawAddress &rawAddr);

    /**
     * @brief Clears all saved data related to the <b>BROWSING</b> command.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void ClearBrowseInfo(const RawAddress &rawAddr);

private:
    // Locks the local variable in a multi-threaded environment.
    std::mutex mutex_ {};
    // The active address of the bluetooth device.
    std::string activeAddr_ {AVRC_TG_DEFAULT_BLUETOOTH_ADDRESS};
    // The disconnected address of the bluetooth devices.
    std::list<std::string> disconnectedDevices_ {};
    // Saves the connection information according to the address of the bluetooth device.
    std::map<std::string, AvrcTgConnectInfo> infos_ {};
    // The static pointer to the instance of the <b>AvrcTgConnectManager</b> class.
    static AvrcTgConnectManager *g_instance;

    /**
     * @brief A constructor used to create an <b>AvrcTgConnectManager</b> instance.
     */
    AvrcTgConnectManager() = default;

    /**
     * @brief Gets the connection information of the specified bluetooth address.
     *
     * @param[in] btAddr The address of the bluetooth device.
     * @return The pointer to the instance of the <b>AvrcTgConnectInfo</b> class.
     */
    AvrcTgConnectInfo *GetConnectInfo(const std::string &btAddr);

    BT_DISALLOW_COPY_AND_ASSIGN(AvrcTgConnectManager);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_TG_CONNECTION_H
