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
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines a bluetooth system that provides basic blurtooth connection and profile functions,
 *        including A2DP, AVRCP, BLE, GATT, HFP, MAP, PBAP, and SPP, etc.
 *
 * @since 6
 */

/**
 * @file bluetooth_hf_call.h
 *
 * @brief Declares HFP HandsFree Unit call data class.
 *
 * @since 6
 */

#ifndef BLUETOOTH_HF_CALL_H
#define BLUETOOTH_HF_CALL_H

#include <ctime>
#include "bluetooth_host.h"
#include "bluetooth_types.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief Phone call state enum.
 *
 * @since 6
 */
enum CallState {
    HF_CALL_STATE_ACTIVE = 0,
    HF_CALL_STATE_HELD,
    HF_CALL_STATE_DIALING,
    HF_CALL_STATE_ALERTING,
    HF_CALL_STATE_INCOMING,
    HF_CALL_STATE_WAITING,
    HF_CALL_STATE_RESPONSE_HELD,
    HF_CALL_STATE_FINISHED
};

/**
 * @brief Class for HandsFree Unit call.
 *
 * @since 6
 */
class HandsFreeUnitCall {
public:
    /**
     * @brief Construct a new HandsFreeUnitCall object.
     *
     * @param device Remote device address.
     * @param id Call index.
     * @param state Call state @see HfpCallState.
     * @param number Call number.
     * @param multiParty Is multiparty flag.
     * @param outgoing Incoming/outing flag.
     * @param inBandRing Is inband-ring flag.
     * @since 6
     */
    HandsFreeUnitCall(
        std::string device, int id, int state, std::string number, bool multiParty, bool outgoing, bool inBandRing)
        : device_(device),
          id_(id),
          state_(state),
          number_(number),
          multiParty_(multiParty),
          outgoing_(outgoing),
          inBandRing_(inBandRing)
    {
        uuid_ = UUID::RandomUUID();
        creationTime_ = clock();
    }

    /**
     * @brief Construct a new HandsFreeUnitCall object.
     *
     * @param device Remote device address.
     * @param id Call index.
     * @param state Call state @see HfpCallState.
     * @param number Call number.
     * @param uuid Call uuid.
     * @param multiParty Is multiparty flag.
     * @param outgoing Incoming/outing flag.
     * @param inBandRing Is inband-ring flag.
     * @param creationTime Call's creation time.
     * @since 6
     */
    HandsFreeUnitCall(std::string device, int id, int state, std::string number, UUID uuid, bool multiParty,
        bool outgoing, bool inBandRing, long creationTime)
        : device_(device),
          id_(id),
          state_(state),
          number_(number),
          uuid_(uuid),
          multiParty_(multiParty),
          outgoing_(outgoing),
          inBandRing_(inBandRing),
          creationTime_(creationTime)
    {}

    /**
     * @brief Construct a new HandsFreeUnitCall object as default.
     *
     * @since 6
     */
    HandsFreeUnitCall() = default;

    /**
     * @brief Destroy the HandsFreeUnitCall object.
     *
     * @since 6
     */
    ~HandsFreeUnitCall() = default;

    /**
     * @brief Get the remote device address.
     *
     * @return Returns remote device address.
     * @since 6
     */
    std::string GetRemoteDevice() const
    {
        return device_;
    }

    /**
     * @brief Get the call index.
     *
     * @return Returns the call index.
     * @since 6
     */
    int GetId() const
    {
        return id_;
    }

    /**
     * @brief Get the uuid object.
     *
     * @return Returns the call uuid.
     * @since 6
     */
    UUID GetUuid() const
    {
        return uuid_;
    }

    /**
     * @brief Get the call state.
     *
     * @return Returns the call state.
     * @since 6
     */
    int GetState() const
    {
        return state_;
    }

    /**
     * @brief Get the call number.
     *
     * @return Returns the call number.
     * @since 6
     */
    std::string GetNumber() const
    {
        return number_;
    }

    /**
     * @brief Get the call's creation time.
     *
     * @return Returns the call's creation time.
     * @since 6
     */
    long GetCreationTime() const
    {
        return creationTime_;
    }

    /**
     * @brief Get the IsMultiParty flag.
     *
     * @return Returns the IsMultiParty flag.
     * @since 6
     */
    bool IsMultiParty() const
    {
        return multiParty_;
    }

    /**
     * @brief Get the IsOutgoing flag.
     *
     * @return Returns the IsOutgoing flag.
     * @since 6
     */
    bool IsOutgoing() const
    {
        return outgoing_;
    }

    /**
     * @brief Get the IsInBandRing flag.
     *
     * @return Returns the IsInBandRing flag.
     * @since 6
     */
    bool IsInBandRing() const
    {
        return inBandRing_;
    }

    /**
     * @brief Set the call state.
     *
     * @param state Call state.
     * @since 6
     */
    void SetState(int state)
    {
        state_ = state;
    }

    /**
     * @brief Set the call number.
     *
     * @param number Call number.
     * @since 6
     */
    void SetNumber(std::string number)
    {
        number_ = number;
    }

    /**
     * @brief Set the multiParty flag.
     *
     * @param multiParty The multiParty flag
     * @since 6
     */
    void SetMultiParty(bool multiParty)
    {
        multiParty_ = multiParty;
    }

protected:
    std::string device_ {""};
    int id_ {0};
    int state_ {-1};
    std::string number_ {""};
    UUID uuid_;
    bool multiParty_ {false};
    bool outgoing_ {false};
    bool inBandRing_ {false};
    long creationTime_ {0};
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // BLUETOOTH_HF_CALL_H