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
 * @version 1.0
 */

/**
 * @file bluetooth_hf_calls.h
 *
 * @brief Declares HFP HandsFree Unit calls data class.
 *
 * @since 6
 * @version 1.0
 */

#ifndef HANDS_FREE_UNIT_CALLS_H
#define HANDS_FREE_UNIT_CALLS_H

#include <string>
#include "bt_uuid.h"
#include "iosfwd"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Phone call state enum.
 *
 * @since 6
 * @version 1.0
 */
enum HfpCallState {
    HFP_CALL_STATE_ACTIVE = 0,
    HFP_CALL_STATE_HELD,
    HFP_CALL_STATE_DIALING,
    HFP_CALL_STATE_ALERTING,
    HFP_CALL_STATE_INCOMING,
    HFP_CALL_STATE_WAITING,
    HFP_CALL_STATE_RESPONSE_HELD,
    HFP_CALL_STATE_FINISHED
};

/**
 * @brief Class for HandsFree Unit calls.
 *
 * @since 6
 * @version 1.0
 */
class HandsFreeUnitCalls {
public:
    /**
     * @brief Construct a new HandsFreeUnitCalls object.
     *
     * @param device Remote device address.
     * @param id Call index.
     * @param state Call state @see HfpCallState.
     * @param number Call number.
     * @param multiParty Is multiparty flag.
     * @param outgoing Incoming/outing flag.
     * @param inBandRing Is inband-ring flag.
     * @since 6
     * @version 1.0
     */
    HandsFreeUnitCalls(const std::string &device, int id, int state, const std::string &number,
                       bool multiParty, bool outgoing, bool inBandRing);

    /**
     * @brief Construct a new HandsFreeUnitCalls object.
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
     * @version 1.0
     */
    HandsFreeUnitCalls(const std::string &device, int id, int state, const std::string &number, Uuid &uuid,
                       bool multiParty, bool outgoing, bool inBandRing, long creationTime);

    /**
     * @brief Construct a new HandsFreeUnitCall object as default.
     *
     * @since 6
     * @version 1.0
     */
    HandsFreeUnitCalls() = default;

    /**
     * @brief Construct a new HandsFreeUnitCalls object as default.
     *
     * @param other Other HandsFreeUnitCalls object.
     * @since 6
     * @version 1.0
     */
    HandsFreeUnitCalls(const HandsFreeUnitCalls& other) = default;

    /**
     * @brief Destroy the HandsFreeUnitCall object.
     *
     * @since 6
     * @version 1.0
     */
    ~HandsFreeUnitCalls() = default;

    /**
     * @brief Get the remote device address.
     *
     * @return Returns remote device address.
     * @since 6
     * @version 1.0
     */
    const std::string &GetRemoteDevice() const;

    /**
     * @brief Get the call index.
     *
     * @return Returns the call index.
     * @since 6
     * @version 1.0
     */
    int GetId() const;

    /**
     * @brief Get the uuid object.
     *
     * @return Returns the call uuid.
     * @since 6
     * @version 1.0
     */
    Uuid GetUuid() const;

    /**
     * @brief Get the call state.
     *
     * @return Returns the call state.
     * @since 6
     * @version 1.0
     */
    int GetState() const;

    /**
     * @brief Get the call number.
     *
     * @return Returns the call number.
     * @since 6
     * @version 1.0
     */
    const std::string &GetNumber() const;

    /**
     * @brief Get the call's creation time.
     *
     * @return Returns the call's creation time.
     * @since 6
     * @version 1.0
     */
    long GetCreationTime() const;

    /**
     * @brief Get the IsMultiParty flag.
     *
     * @return Returns the IsMultiParty flag.
     * @since 6
     * @version 1.0
     */
    bool IsMultiParty() const;

    /**
     * @brief Get the IsOutgoing flag.
     *
     * @return Returns the IsOutgoing flag.
     * @since 6
     * @version 1.0
     */
    bool IsOutgoing() const;

    /**
     * @brief Get the IsInBandRing flag.
     *
     * @return Returns the IsInBandRing flag.
     * @since 6
     * @version 1.0
     */
    bool IsInBandRing() const;

    /**
     * @brief Set the call state.
     *
     * @param state Call state.
     * @since 6
     * @version 1.0
     */
    void SetState(int state);

    /**
     * @brief Set the call number.
     *
     * @param number Call number.
     * @since 6
     * @version 1.0
     */
    void SetNumber(const std::string &number);

    /**
     * @brief Set the multiParty flag.
     *
     * @param multiParty The multiParty flag
     * @since 6
     * @version 1.0
     */
    void SetMultiParty(bool multiParty);

protected:
    std::string device_ {""};
    int id_ {0};
    int state_ {-1};
    std::string number_ {""};
    Uuid uuid_ {};
    bool multiParty_ {false};
    bool outgoing_ {false};
    bool inBandRing_ {false};
    long creationTime_ {0};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HANDS_FREE_UNIT_CALLS_H