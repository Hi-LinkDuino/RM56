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

#ifndef CLASSIC_ADAPTER_H
#define CLASSIC_ADAPTER_H

#include <map>
#include <vector>

#include "base_def.h"
#include "bt_uuid.h"
#include "btm.h"
#include "classic_adapter_properties.h"
#include "classic_battery_observer.h"
#include "classic_bluetooth_data.h"
#include "classic_remote_device.h"
#include "context.h"
#include "gap_if.h"
#include "interface_adapter_classic.h"
#include "log.h"
#include "log_util.h"
#include "raw_address.h"
#include "sdp.h"

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines adapter classic common functions.
 *
 */

/**
 * @file classic_adapter.h
 *
 * @brief Adapter classic.
 *
 */

namespace OHOS {
namespace bluetooth {
/**
 * @brief Represents classic adapter.
 *
 */
class ClassicAdapter : public IAdapterClassic, public utility::Context {
public:
    /**
     * @brief A constructor used to create a <b>ClassicAdapter</b> instance.
     *
     */
    ClassicAdapter();

    /**
     * @brief A destructor used to delete the <b>ClassicAdapter</b> instance.
     *
     */
    ~ClassicAdapter();

    /**
     * @brief Get local device address.
     *
     * @return Returns local device address.
     */
    std::string GetLocalAddress() const override;

    /**
     * @brief Get local device name.
     *
     * @return Returns local device name.
     */
    std::string GetLocalName() const override;

    /**
     * @brief Set local device name.
     *
     * @param name Device name.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetLocalName(const std::string &name) const override;

    /**
     * @brief Set bondable mode.
     *
     * @param mode Bondable mode.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetBondableMode(int mode) const override;

    /**
     * @brief Get bondable mode.
     *
     * @return Returns bondable mode.
     */
    int GetBondableMode() const override;

    /**
     * @brief Get remote device name.
     *
     * @param device Remote device.
     * @return Returns device name.
     */
    std::string GetDeviceName(const RawAddress &device) const override;

    /**
     * @brief Get remote device uuids.
     *
     * @param device Remote device.
     * @return Returns device uuids.
     */
    std::vector<Uuid> GetDeviceUuids(const RawAddress &device) const override;

    /**
     * @brief Get paired devices.
     *
     * @return Returns device list.
     */
    std::vector<RawAddress> GetPairedDevices() const override;

    /**
     * @brief Local device start pair.
     *
     * @param device Remote device.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool StartPair(const RawAddress &device) override;

    /**
     * @brief Check if remote device was bonded from local.
     *
     * @param device Remote device.
     * @return Returns <b>true</b> if device was bonded from local;
     *         returns <b>false</b> if device was not bonded from local.
     */
    bool IsBondedFromLocal(const RawAddress &device) const override;

    /**
     * @brief Cancel pair operation.
     *
     * @param device Remote device.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool CancelPairing(const RawAddress &device) override;

    /**
     * @brief Remove pair.
     *
     * @param device Remote device.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool RemovePair(const RawAddress &device) override;

    /**
     * @brief Get device pair state.
     *
     * @param device Remote device.
     * @return Returns device pair state.
     */
    int GetPairState(const RawAddress &device) const override;

    /**
     * @brief Set device pairing confirmation.
     *
     * @param device Remote device.
     * @param accept Set gap accept flag.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetDevicePairingConfirmation(const RawAddress &device, bool accept) const override;

    /**
     * @brief Set device pair passkey.
     *
     * @param device Remote device.
     * @param passkey Device passkey.
     * @param accept Set gap accept flag.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetDevicePasskey(const RawAddress &device, int passkey, bool accept) const override;

    /**
     * @brief Check remote device pair request reply.
     *
     * @param device Remote device.
     * @param accept Set gap accept flag.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool PairRequestReply(const RawAddress &device, bool accept) const override;

    /**
     * @brief Check if acl connected with remote device.
     *
     * @param device Remote device.
     * @return Returns <b>true</b> if device acl connected;
     *         returns <b>false</b> if device does not acl connect.
     */
    bool IsAclConnected(const RawAddress &device) const override;

    /**
     * @brief Check if remote device acl Encrypted.
     *
     * @return Returns <b>true</b> if device acl Encrypted;
     *         returns <b>false</b> if device does not acl Encrypt.
     */
    bool IsAclEncrypted(const RawAddress &device) const override;

    /**
     * @brief Get the instance of the ClassicAdapter.
     *
     * @return Returns the instance of the ClassicAdapter.
     */
    utility::Context *GetContext() override;

    /**
     * @brief Enable classic service.
     *
     */
    void Enable() override;

    /**
     * @brief Disable classic service.
     *
     */
    void Disable() override;

    /**
     * @brief PostEnable classic service.
     *
     */
    void PostEnable() override;

    /**
     * @brief Get remote device type.
     *
     * @param device Remote device.
     * @return Returns remote device type.
     */
    int GetDeviceType(const RawAddress &device) const override;

    /**
     * @brief Get remote device class.
     *
     * @param device Remote device.
     * @return Returns remote device class.
     */
    int GetDeviceClass(const RawAddress &device) const override;

    /**
     * @brief Get remote device alias name.
     *
     * @param device Remote device
     * @return Returns remote device alias name.
     */
    std::string GetAliasName(const RawAddress &device) const override;

    /**
     * @brief Set remote device alias name.
     *
     * @param device Remote device which setted alias name.
     * @param name Alias name.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetAliasName(const RawAddress &device, const std::string &name) const override;

    /**
     * @brief Register remote device observer.
     *
     * @param observer Class IClassicRemoteDeviceObserver pointer to register observer.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool RegisterRemoteDeviceObserver(IClassicRemoteDeviceObserver &observer) const override;

    /**
     * @brief Deregister remote device observer.
     *
     * @param observer Class IClassicRemoteDeviceObserver pointer to register observer.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool DeregisterRemoteDeviceObserver(IClassicRemoteDeviceObserver &observer) const override;

    /**
     * @brief Register classic adapter observer.
     *
     * @param observer Class IAdapterClassicObserver pointer to register observer.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool RegisterClassicAdapterObserver(IAdapterClassicObserver &observer) const override;

    /**
     * @brief Deregister classic adapter observer.
     *
     * @param observer Class IAdapterClassicObserver pointer to register observer.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool DeregisterClassicAdapterObserver(IAdapterClassicObserver &observer) const override;

    /**
     * @brief Remove all pairs.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool RemoveAllPairs() override;

    /**
     * @brief Set device pair pin.
     *
     * @param device Remote device address.
     * @param pin Pin code.
     * @param length Pin code length.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetDevicePin(const RawAddress &device, const std::string &pinCode) const override;

    /**
     * @brief Set device scan mode.
     *
     * @param mode Scan mode.
     * @param duration Scan time.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetBtScanMode(int mode, int duration) override;

    /**
     * @brief Get device scan mode.
     *
     * @return Returns bluetooth scan mode.
     */
    int GetBtScanMode() const override;

    /**
     * @brief Get local device class.
     *
     * @return Returns local device class.
     */
    int GetLocalDeviceClass() const override;

    /**
     * @brief Set local device class.
     *
     * @param deviceClass Device class.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetLocalDeviceClass(int deviceClass) const override;

    /**
     * @brief Get device address.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool StartBtDiscovery() override;

    /**
     * @brief Cancel device discovery.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool CancelBtDiscovery() override;

    /**
     * @brief Check if device is discovering.
     *
     * @return Returns <b>true</b> if device is discovering;
     *         returns <b>false</b> if device is not discovering.
     */
    bool IsBtDiscovering() const override;

    /**
     * @brief Get device discovery end time.
     *
     * @return Returns device discovery end time.
     */
    long GetBtDiscoveryEndMillis() const override;
    int GetDeviceBatteryLevel(const RawAddress &device) const override;
    void SetDeviceBatteryLevel(const RawAddress &device, int batteryLevel) const override;

private:
    /**
     * @brief Inquiry result callback.
     *
     * @param addr Device address.
     * @param classOfDevice Device class.
     * @param context Classic adapter object.
     */
    static void InquiryResultCallback(const BtAddr *addr, uint32_t classOfDevice, void *context);

    /**
     * @brief Inquiry result rssi callback.
     *
     * @param addr Device address.
     * @param classOfDevice Device class.
     * @param rssi Device rssi.
     * @param context Classic adapter object.
     */
    static void InquiryResultRssiCallback(const BtAddr *addr, uint32_t classOfDevice, int8_t rssi, void *context);

    /**
     * @brief Extended inquiry result callback.
     *
     * @param addr Device address.
     * @param classOfDevice Device class.
     * @param rssi Device rssi.
     * @param eir Eir data.
     * @param context Classic adapter object.
     */
    static void ExtendedInquiryResultCallback(const BtAddr *addr, uint32_t classOfDevice, int8_t rssi,
        const uint8_t eir[MAX_EXTEND_INQUIRY_RESPONSE_LEN], void *context);

    /**
     * @brief Inquiry complete callback.
     *
     * @param status Inquire status.
     * @param context Classic adapter object.
     */
    static void InquiryCompleteCallback(uint8_t status, void *context);

    /**
     * @brief Remote device name callback.
     *
     * @param status Inquire status.
     * @param addr Device address.
     * @param name Device name.
     * @param context Classic adapter object.
     */
    static void RemoteNameCallback(
        uint8_t status, const BtAddr *addr, const uint8_t name[MAX_LOC_BT_NAME_LEN], void *context);
    /**
     * @brief User confirm request callback.
     *
     * @param addr Device address.
     * @param number Pass key.
     * @param context Classic adapter object.
     */
    static void UserConfirmReqCallback(const BtAddr *addr, uint32_t number, void *context);

    /**
     * @brief User pass key request callback.
     *
     * @param addr Device address.
     * @param context Classic adapter object.
     */
    static void UserPasskeyReqCallback(const BtAddr *addr, void *context);

    /**
     * @brief User pass key notification callback.
     *
     * @param addr Device address.
     * @param number Pass key.
     * @param context Classic adapter object.
     */
    static void UserPasskeyNotificationCallback(const BtAddr *addr, uint32_t number, void *context);

    /**
     * @brief Remote oob key request callback.
     *
     * @param addr Device address.
     * @param context Classic adapter object.
     */
    static void RemoteOobReqCallback(const BtAddr *addr, void *context);

    /**
     * @brief Pin code request callback.
     *
     * @param addr Device address.
     * @param context Classic adapter object.
     */
    static void PinCodeReqCallback(const BtAddr *addr, void *context);

    /**
     * @brief Link key request callback.
     *
     * @param addr Device address.
     * @param context Classic adapter object.
     */
    static void LinkKeyReqCallback(const BtAddr *addr, void *context);

    /**
     * @brief Io capability request callback.
     *
     * @param addr Device address.
     * @param context Classic adapter object.
     */
    static void IoCapabilityReqCallback(const BtAddr *addr, void *context);

    /**
     * @brief Io capability response callback.
     *
     * @param addr Device address.
     * @param ioCapability Device iocapability.
     * @param context Classic adapter object.
     */
    static void IoCapabilityRspCallback(const BtAddr *addr, uint8_t ioCapability, void *context);

    /**
     * @brief Link key notification.
     *
     * @param addr Device address.
     * @param linkkey Link key.
     * @param keyType Link key type.
     * @param context Classic adapter object.
     */
    static void LinkKeyNotification(
        const BtAddr *addr, const uint8_t linkKey[GAP_LINKKEY_SIZE], uint8_t keyType, void *context);

    /**
     * @brief Simple pair complete callback.
     *
     * @param addr Device address.
     * @param status Pair status.
     * @param context Classic adapter object.
     */
    static void SimplePairCompleteCallback(const BtAddr *addr, uint8_t status, void *context);

    /**
     * @brief Authentication complete callback.
     *
     * @param addr Device address.
     * @param status Authentication status.
     * @param context Classic adapter object.
     */
    static void AuthenticationCompleteCallback(const BtAddr *addr, uint8_t status, void *context);

    /**
     * @brief Encryption change complete callback.
     *
     * @param addr Device address.
     * @param status Encryption status.
     * @param context Classic adapter object.
     */
    static void EncryptionChangeCallback(const BtAddr *addr, uint8_t status, void *context);

    /**
     * @brief Authorize ind callback.
     *
     * @param addr Device address.
     * @param service Gap service.
     * @param context Classic adapter object.
     */
    static void AuthorizeIndCallback(const BtAddr *addr, GAP_Service service, void *context);

    /// BTM ACL Change status callback.
    /**
     * @brief Acl connection complete callback.
     *
     * @param status Connection status.
     * @param connectionHandle Connection handle.
     * @param addr Device address.
     * @param encyptionEnabled Encyption enable status.
     * @param context Classic adapter object.
     */
    static void ConnectionComplete(const BtmAclConnectCompleteParam *param, void *context);

    /**
     * @brief Acl disconnection complete callback.
     *
     * @param status Connection status.
     * @param connectionHandle Connection handle.
     * @param reason Disconnection reason.
     * @param context Classic adapter object.
     */
    static void DisconnectionComplete(uint8_t status, uint16_t connectionHandle, uint8_t reason, void *context);

    /// SDP search remote device supported uuid callback.
    /**
     * @brief Service search attribute callback.
     *
     * @param addr Device address.
     * @param serviceArray Service array.
     * @param serviceNum Service number.
     * @param context Classic adapter object.
     */
    static void ServiceSearchAttributeCb(
        const BtAddr *addr, const SdpService *serviceArray, uint16_t serviceNum, void *context);

    /**
     * @brief Handle inquiry event.
     *
     * @param event inquiry callback event.
     * @param param Gap callback paramer.
     */
    void HandleInquiryEvent(GAP_CB_EVENT event, const GapCallbackParam &param);

    /**
     * @brief Handle security event.
     *
     * @param event security callback event.
     * @param param Gap callback paramer.
     */
    void HandleSecurityEvent(GAP_CB_EVENT event, const GapCallbackParam &param);

    /**
     * @brief ClassicAdater start up. Load config file and enable stack.
     *
     */
    void StartUp();

    /**
     * @brief ClassicAdater shut down. Save config file and disable stack.
     *
     */
    void ShutDown();

    /**
     * @brief Process post enable.
     *
     */
    void ProcessPostEnable();

    /**
     * @brief Set scan mode.
     *
     * @param mode Scan mode.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetScanMode(int mode);

    /**
     * @brief Set scan mode result callback.
     *
     * @param status Scan status.
     * @param context ClassicAdapterProperties object.
     */
    static void SetScanModeResultCallback(uint8_t status, void *context);

    /**
     * @brief Receive SetScanMode callback.
     *
     * @param status SetScanMode result.
     */
    void ReceiveSetScanModeCallback(uint8_t status);

    /**
     * @brief ScanMode timeout.
     *
     */
    void ScanModeTimeout();

    /**
     * @brief Hw(Controller) process timeout.
     *
     */
    void HwProcessTimeout();
    void HwTimeout();

    /**
     * @brief Register callback.
     *
     */
    bool RegisterCallback();

    /**
     * @brief Deregister callback.
     *
     */
    bool DeregisterCallback() const;

    /**
     * @brief Get remote device name.
     *
     * @param addr Device address.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool GetRemoteName(const BtAddr &addr) const;

    /**
     * @brief Set link key.
     *
     * @param addr Device address.
     */
    void SetLinkKey(const BtAddr &addr);

    /**
     * @brief Set Io capability.
     *
     * @param addr Device address.
     */
    void SetIoCapability(const BtAddr &addr);

    /**
     * @brief Parser eir data.
     *
     * @param remote Remote device.
     * @param data Eir data.
     */
    void ParserEirData(std::shared_ptr<ClassicRemoteDevice> remote, const std::vector<uint8_t> &data) const;

    /**
     * @brief Load paired device info from config file.
     *
     */
    void LoadPairedDeviceInfo();

    /**
     * @brief Save paired devices info.
     *
     */
    void SavePairedDevices() const;

    /**
     * @brief Get service uuid from device uuid.
     *
     * @param uuid Device uuid.
     * @return Returns service uuid.
     */
    Uuid GetUuidFromBtUuid(const BtUuid &inUuid) const;

    /**
     * @brief Get local support uuids then update the uuids to EIR data.
     *
     */
    void UpdateSupportedUuids() const;

    /**
     * @brief Save remote device Io capability.
     *
     * @param addr Remote device address.
     * @param ioCapability Device Io capability
     */
    void SaveRemoteIoCapability(const BtAddr &addr, uint8_t ioCapability);

    /**
     * @brief Send scan mode changed.
     *
     * @param mode ScamMode.
     */
    void SendScanModeChanged(int mode) const;

    /**
     * @brief Send discovery state changed.
     *
     */
    void SendDiscoveryStateChanged(int discoveryState) const;

    /**
     * @brief Send discovery result.
     *
     * @param device Remote device.
     */
    void SendDiscoveryResult(const RawAddress &device) const;

    /**
     * @brief Send remote device cod changed.
     *
     * @param device Remote device.
     * @param cod Remote device cod.
     */
    void SendRemoteCodChanged(const RawAddress &device, int cod) const;

    /**
     * @brief Send remote device battery level changed.
     *
     * @param device Remote device.
     * @param batteryLevel Remote device battery level.
     */
    void SendRemoteBatteryLevelChanged(const RawAddress &device, int batteryLevel) const;

    /**
     * @brief Send remote device name changed.
     *
     * @param device Remote device.
     * @param deviceName Remote device name.
     */
    void SendRemoteNameChanged(const RawAddress &device, const std::string &deviceName) const;

    /**
     * @brief Send pair status changed.
     *
     * @param transport Remote device transport.
     * @param device Remote device.
     * @param status Pair status.
     */
    void SendPairStatusChanged(const BTTransport transport, const RawAddress &device, int status) const;

    /**
     * @brief Send pair request.
     *
     * @param transport Remote device transport.
     * @param device Remote device.
     */
    void SendPairRequested(const BTTransport transport, const RawAddress &device) const;

    /**
     * @brief Send pair confirmed.
     *
     * @param device Remote device.
     * @param reqType confirmed type (SSP/Pin Code).
     * @param number confirmed number.
     */
    void SendPairConfirmed(const RawAddress &device, int reqType, int number) const;

    /**
     * @brief Send remote device uuid changed.
     *
     * @param device Remote device.
     * @param uuids Device uuid.
     */
    void SendRemoteUuidChanged(const RawAddress &device, const std::vector<Uuid> &uuids) const;

    /**
     * @brief Send remote alias name changed.
     *
     * @param device Remote device.
     * @param aliasName alias name.
     */
    void SendRemoteAliasChanged(const RawAddress &device, const std::string &aliasName) const;

    /**
     * @brief Handle inquire result.
     *
     * @param addr Device address.
     * @param classOfDevice Device class.
     * @param rssi Device rssi.
     * @param eir Eir data.
     */
    void HandleInquiryResult(const BtAddr &addr, uint32_t classOfDevice, std::vector<uint8_t> eir, int8_t rssi = 0);

    /**
     * @brief Handle inquire complete.
     *
     * @param status Inquire status.
     */
    void HandleInquiryComplete(uint8_t status);

    /**
     * @brief Receive remote device name.
     *
     * @param status Receive status.
     * @param addr Remote device address.
     * @param name  Remote device name.
     */
    void ReceiveRemoteName(uint8_t status, const BtAddr &addr, const uint8_t name[MAX_LOC_BT_NAME_LEN]);

    /**
     * @brief SSP confirm request.
     *
     * @param addr Remote device address.
     * @param reqType  Request type.
     * @param number Pass key.
     */
    void SSPConfirmReq(const BtAddr &addr, int reqType, int number);

    /**
     * @brief Set authorize response.
     *
     * @param addr Remote device address.
     * @param service Gap service.
     */
    void SetAuthorizeRes(const BtAddr &addr, GAP_Service service) const;

    /**
     * @brief Receive link key notification.
     *
     * @param addr Remote device address.
     * @param linkKey Link key.
     * @param keyType Link key type.
     */
    void ReceiveLinkKeyNotification(const BtAddr &addr, const uint8_t linkKey[GAP_LINKKEY_SIZE], uint8_t keyType);

    /**
     * @brief Receive simple pair complete.
     *
     * @param addr Remote device address.
     * @param status Pair status.
     */
    void ReceiveSimplePairComplete(const BtAddr &addr, uint8_t status);

    /**
     * @brief Receive authentication complete.
     *
     * @param addr Remote device address.
     * @param status Authentication status.
     */
    void ReceiveAuthenticationComplete(const BtAddr &addr, uint8_t status);

    /**
     * @brief Receive encryption change.
     *
     * @param addr Remote device address.
     * @param status Encryption status.
     */
    void ReceiveEncryptionChange(const BtAddr &addr, uint8_t status);

    /**
     * @brief Receive acl connection complete.
     *
     * @param status Connection status.
     * @param connectionHandle Connection handle.
     * @param remoteAddr Device address.
     * @param encyptionEnabled Encyption enable status.
     */
    void ReceiveConnectionComplete(uint8_t status, uint16_t connectionHandle, const RawAddress &device,
        uint32_t classOfDevice, bool encyptionEnabled);

    /**
     * @brief Receive acl disconnection complete.
     *
     * @param status Connection status.
     * @param connectionHandle Connection handle.
     * @param reason Disconnection reason.
     */
    void ReceiveDisconnectionComplete(uint8_t status, uint16_t connectionHandle, uint8_t reason);

    /**
     * @brief User confirm auto reply.
     *
     * @param device Remote device.
     * @param reqType Request type.
     * @param accept Request accept or not.
     */
    void UserConfirmAutoReply(const RawAddress &device, int reqType, bool accept) const;

    /**
     * @brief Set pin code.
     *
     * @param device Remote device.
     * @param pin Pin code.
     * @param accept Request accept or not.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool SetPinCode(const RawAddress &device, const std::vector<uint8_t> &pin, bool accept) const;

    /**
     * @brief Check if auto reply.
     *
     * @param remoteIo Remote device Io capability.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool CheckAutoReply(int remoteIo) const;

    /**
     * @brief Check ssp confirm type.
     *
     * @param remoteIo Remote device Io capability.
     * @param type Confirm type.
     * @return Returns confirm type.
     */
    int CheckSspConfirmType(int remoteIo, int type) const;

    /**
     * @brief Find remote device.
     *
     * @param remoteAddr Remote device address.
     * @return Returns remote device.
     */
    std::shared_ptr<ClassicRemoteDevice> FindRemoteDevice(const RawAddress &device);

    /**
     * @brief Parser remote name from eir data.
     *
     * @param remote Remote device.
     * @param type type of Eir data structure.
     * @param value Eir data of Eir data structure.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool ParserRemoteNameFromEir(
        std::shared_ptr<ClassicRemoteDevice> remote, int type, const std::vector<uint8_t> &value) const;

    /**
     * @brief Parser service uuid from eir data.
     *
     * @param type type of Eir data structure.
     * @param value Eir data of Eir data structure.
     * @return Returns uuids parser from eir data.
     */
    std::vector<Uuid> ParserUuidFromEir(int type, const std::vector<uint8_t> &value) const;

    /**
     * @brief Parser 16 bit service uuid from eir data.
     *
     * @param value Eir data of Eir data structure.
     * @return Returns 16 Bit uuids parser from eir data.
     */
    std::vector<Uuid> Parser16BitUuidFromEir(const std::vector<uint8_t> &value) const;

    /**
     * @brief Parser 32 bit service uuid from eir data.
     *
     * @param value Eir data of Eir data structure.
     * @return Returns 32 Bit uuids parses from eir data.
     */
    std::vector<Uuid> Parser32BitUuidFromEir(const std::vector<uint8_t> &value) const;

    /**
     * @brief Parser 128 bit service uuid from eir data.
     *
     * @param value Eir data of Eir data structure.
     * @return Returns 128 Bit uuids parser from eir data.
     */
    std::vector<Uuid> Parser128BitUuidFromEir(const std::vector<uint8_t> &value) const;

    /**
     * @brief Save remote device uuids.
     *        If uuids is changed, send OnRemoteUuidChanged to client.
     *
     * @param remote Remote device.
     * @param uuids Remote device uuids.
     */
    void SaveRemoteDeviceUuids(std::shared_ptr<ClassicRemoteDevice> remote, const std::vector<Uuid> &uuids) const;

    /**
     * @brief Check uuids1 is equal with uuids2.
     *
     * @param uuids1 uuids.
     * @param uuids2 uuids.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool IsUuidsEqual(const std::vector<Uuid> &uuids1, const std::vector<Uuid> &uuids2) const;

    void InitMode();
    void FreeMemory();
    void DisableBTM();
    bool DiscoverRemoteName();
    bool CancelGetRemoteName() const;
    void SearchRemoteUuids(const RawAddress &device, uint16_t uuid);
    void ResetScanMode();
    void DeleteLinkKey(std::shared_ptr<ClassicRemoteDevice> remoteDevice) const;
    BtAddr ConvertToBtAddr(const RawAddress &device) const;
    void DisablePairProcess();
    void SearchAttributeEnd(const RawAddress &device, const std::vector<Uuid> &uuids);
    void PinCodeReq(const BtAddr &addr);
    ClassicAdapterProperties &adapterProperties_;
    std::unique_ptr<utility::Timer> timer_ {};
    std::unique_ptr<utility::Timer> hwTimer_ {};
    int discoveryState_ {};
    int scanMode_ {};
    long discoveryEndMs_ {};
    bool pinMode_ {};
    bool btmEnableSuccess_ {};
    bool isDisable_ {};
    bool receiveInquiryComplete_ {};
    bool cancelDiscovery_ {};
    bool waitPairResult_ {};
    uint16_t searchUuid_ {};
    std::vector<Uuid> uuids_ {};
    std::string remoteNameAddr_ {INVALID_MAC_ADDRESS};
    std::map<std::string, std::shared_ptr<ClassicRemoteDevice>> devices_ {};
    BtmAclCallbacks btmAclCbs_ {};
    ClassicBluetoothData eirData_ {};
    std::unique_ptr<ClassicBatteryObserverHf> batteryObserverHf_ {};
    IProfileHfpHf *hfService_ {nullptr};
    std::unique_ptr<ClassicBatteryObserverAg> batteryObserverAg_ {};
    IProfileHfpAg *agService_ {nullptr};

    DECLARE_IMPL();
    BT_DISALLOW_COPY_AND_ASSIGN(ClassicAdapter);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // CLASSIC_ADAPTER_H