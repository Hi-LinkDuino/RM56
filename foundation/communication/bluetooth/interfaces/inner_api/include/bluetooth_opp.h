/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines a bluetooth system that provides basic bluetooth connection and profile functions,
 *        including A2DP, AVRCP, BLE, GATT, HFP, MAP, PBAP, and SPP, etc.
 *
 */

/**
 * @file bluetooth_opp.h
 *
 * @brief Declares OPP role framework functions, including basic and observer functions.
 *
 */
#ifndef BLUETOOTH_OPP_H
#define BLUETOOTH_OPP_H

#include <string>
#include <vector>
#include <memory>

#include "bluetooth_def.h"
#include "bluetooth_remote_device.h"
#include "bluetooth_types.h"
namespace OHOS {
namespace Bluetooth {
/**
 * @brief Bluetooth Opp Transfer Information.
 *
 * @since 9
 */
class BLUETOOTH_API BluetoothOppTransferInformation {
public:
    /**
     * @brief A constructor used to create a <b>BluetoothOppTransferInformation</b> instance.
     *
     * @since 6
     */
    BluetoothOppTransferInformation();

    /**
     * @brief A destructor used to delete the <b>BluetoothOppTransferInformation</b> instance.
     *
     * @since 6
     */
    ~BluetoothOppTransferInformation();

    /**
     * @brief Get Id.
     *
     * @return Returns Id.
     * @since 6
     */
    int GetId() const;

    /**
     * @brief Get File Name.
     *
     * @return File Name.
     * @since 6
     */
    std::string GetFileName() const;

    /**
     * @brief Get File Path.
     *
     * @return Returns File Path.
     * @since 6
     */
    std::string GetFilePath() const;

    /**
     * @brief Get Mime Type.
     *
     * @since 6
     */
    std::string GetMimeType() const;

    /**
     * @brief Get Device Name.
     *
     * @return Returns Device Name.
     * @since 6
     */
    std::string GetDeviceName() const;

    /**
     * @brief Get Device Address.
     *
     * @return Returns Device Address.
     * @since 6
     */
    std::string GetDeviceAddress() const;

    /**
     * @brief Get Direction.
     *
     * @return Returns Direction.
     * @since 6
     */
    int GetDirection() const;

    /**
     * @brief Get Status.
     *
     * @return Returns Status.
     * @since 6
     */
    int GetStatus() const;

    /**
     * @brief Get Failed Reason.
     *
     * @return Returns Failed Reason.
     * @since 6
     */
    int GetFailedReason() const;

    /**
     * @brief Get Time Stamp.
     *
     * @return TimeStamp.
     * @since 6
     */
    uint64_t GetTimeStamp() const;

    /**
     * @brief Get Current Bytes.
     *
     * @return Returns Current Bytes.
     * @since 6
     */
    uint64_t GetCurrentBytes() const;

    /**
     * @brief Get Total Bytes.
     *
     * @return Returns TotalBytes.
     * @since 6
     */
    uint64_t GetTotalBytes() const;

    /**
     * @brief Set Id.
     *
     * @param interval Id.
     * @since 6
     */
    void SetId(int id);

    /**
     * @brief Set File Name.
     *
     * @param interval File Name.
     * @since 6
     */
    void SetFileName(std::string fileName);

    /**
     * @brief Set File Path.
     *
     * @param interval File Path.
     * @since 6
     */
    void SetFilePath(std::string filePath);

    /**
     * @brief Set Mime Type.
     *
     * @param interval Mime Type.
     * @since 6
     */
    void SetMimeType(std::string mimeType);

    /**
     * @brief Set Device Name.
     *
     * @param interval Device Name.
     * @since 6
     */
    void SetDeviceName(std::string deviceName);

    /**
     * @brief Set Device Address.
     *
     * @param interval Device Address.
     * @since 6
     */
    void SetDeviceAddress(std::string deviceAddress);

    /**
     * @brief Set Direction.
     *
     * @param interval Direction.
     * @since 6
     */
    void SetDirection(int direction);

    /**
     * @brief Set Status.
     *
     * @param interval Status.
     * @since 6
     */
    void SetStatus(int status);

    /**
     * @brief Set Failed Reason.
     *
     * @param interval Failed Reason.
     * @since 6
     */
    void SetFailedReason(int failedReason);

    /**
     * @brief Set Time Stamp.
     *
     * @param interval Time Stamp.
     * @since 6
     */
    void SetTimeStamp(uint64_t timeStamp);

    /**
     * @brief Set Current Bytes.
     *
     * @param interval Current Bytes.
     * @since 6
     */
    void SetCurrentBytes(uint64_t currentBytes);

    /**
     * @brief Set Total Bytes.
     *
     * @param interval Total Bytes.
     * @since 6
     */
    void SetTotalBytes(uint64_t totalBytes);

private:
    int id_ = -1;
    std::string fileName_;
    std::string filePath_;
    std::string mimeType_;
    std::string deviceName_;
    std::string deviceAddress_;
    int direction_ = 0;
    int status_ = 0;
    int failedReason_ = 0;
    uint64_t timeStamp_ = 0;
    uint64_t currentBytes_ = 0;
    uint64_t totalBytes_ = 0;
};

/**
 * @brief Class for Opp Host observer functions.
 *
 */
class OppObserver {
public:
    /**
     * @brief The observer function to notify receive incoming file changed.
     *
     * @param transferInformation transfer Information.
     */
    virtual void OnReceiveIncomingFileChanged(const BluetoothOppTransferInformation &transferInformation)
    {}

   /**
     * @brief The observer function to notify transfer state changed.
     *
     * @param transferInformation transfer Information.
     */
    virtual void OnTransferStateChanged(const BluetoothOppTransferInformation &transferInformation)
    {}

    /**
     * @brief Destroy the OppObserver object.
     *
     */
    virtual ~OppObserver()
    {}
};

/**
 * @brief Class for Opp API.
 *
 */
class BLUETOOTH_API Opp {
public:
    /**
     * @brief Get the instance of Opp object.
     *
     * @return Returns the pointer to the Opp instance.
     */
    static Opp *GetProfile();

    /**
     * @brief Send File.
     *
     * @return Return a bool value to confirm whether the file is successfully sent.
     */
    bool SendFile(std::string device, std::vector<std::string>filePaths, std::vector<std::string>mimeTypes);

    /**
     * @brief Set Incoming File Confirmation.
     *
     * @return Return a bool value to confirm whether the setting is successful.
     */
    bool SetIncomingFileConfirmation(bool accept);

    /**
     * @brief Get Current Transfer Information.
     *
     * @return Return OppTransferInformation Object.
     */
    BluetoothOppTransferInformation GetCurrentTransferInformation();

     /**
     * @brief Cancel Transfer.
     *
     * @return Return a bool value to confirm whether the cancellation is set successfully.
     */
    bool CancelTransfer();

    /**
     * @brief Register Opp observer instance.
     *
     * @param observer Opp observer instance.
     */
    void RegisterObserver(OppObserver *observer);

    /**
     * @brief Deregister Opp observer instance.
     *
     * @param observer Opp observer instance.
     */
    void DeregisterObserver(OppObserver *observer);

    /**
     * @brief Get remote Opp device list which are in the specified states.
     *
     * @param states List of remote device states.
     * @return Returns the list of devices.
     */
    std::vector<BluetoothRemoteDevice> GetDevicesByStates(std::vector<int> states);

     /**
     * @brief Get the connection state of the specified remote Opp device.
     *
     * @param device Remote device object.
     * @return Returns the connection state of the remote device.
     */
    int GetDeviceState(const BluetoothRemoteDevice &device);

    /**
     * @brief Static Opp observer instance.
     *
     */
    static OppObserver *instance_;

private:
    Opp();
    ~Opp();
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(Opp);
    BLUETOOTH_DECLARE_IMPL();
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // BLUETOOTH_OPP_H