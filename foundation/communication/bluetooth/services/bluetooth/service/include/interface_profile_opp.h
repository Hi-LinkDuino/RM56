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
 * @brief Defines opp service interface, including observer and api functions.
 *
 */

/**
 * @file interface_profile_opp.h
 *
 * @brief opp interface.
 *
 */

#ifndef INTERFACE_PROFILE_OPP_H
#define INTERFACE_PROFILE_OPP_H

#include "interface_profile.h"
#include "opp_transfer_information.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief opp observer for framework api
 *
 */
class IOppObserver {
public:
    /**
     * @brief Destroy the IOppObserver object
     *
     */
    virtual ~IOppObserver() = default;
    /**
     * @brief  Receive a incoming file request.
     *
     * @param  info file information
     */
    virtual void OnReceiveIncomingFile(const IOppTransferInformation &info) {}
    /**
     * @brief  Notify the transfer's state changed.
     *
     * @param  info file information
     */
    virtual void OnTransferStateChange(const IOppTransferInformation &info) {}
};

class IProfileOpp : public IProfile {
public:
    /**
     * @brief  register observer
     *
     * @param  observer  function pointer
     */
    virtual void RegisterObserver(IOppObserver &observer) = 0;
    /**
     * @brief  deregister observer
     *
     * @param  observer  function pointer
     */
    virtual void DeregisterObserver(IOppObserver &observer) = 0;
    /**
     * @brief Get remote device list which are in the specified states.
     *
     * @param states  List of remote device states.
     * @return Returns the list of devices.
     */
    virtual std::vector<RawAddress> GetDevicesByStates(std::vector<int> states) = 0;
    /**
     * @brief Get the connection state of the specified remote device.
     *
     * @param device  Remote device object.
     * @return Returns the connection state of the remote device.
     */
    virtual int GetDeviceState(const RawAddress &device) = 0;

    virtual int SendFile(const RawAddress &device, const std::vector<std::string> filePaths,
        const std::vector<std::string> mimeTypes) = 0;
    virtual int SetIncomingFileConfirmation(const bool accept) = 0;
    virtual IOppTransferInformation GetCurrentTransferInformation() = 0;
    virtual int CancelTransfer() = 0;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // INTERFACE_PROFILE_OPP_H