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

#ifndef A2DP_SDP_H
#define A2DP_SDP_H

#include <cstdint>
#include "a2dp_def.h"
#include "btstack.h"
#include "sdp.h"

/**
 * @brief The Bluetooth subsystem.
 */
namespace OHOS {
namespace bluetooth {
#define A2DP_SDP_ATTRIBUTE_NUM 3
/**
 * @brief This class provides a set of methods used to interact with the SDP protocol.
 */
class A2dpSdpManager {
public:
    /**
     * @brief Constructor.
     */
    A2dpSdpManager(){};

    /**
     * @brief Destructor.
     */
    ~A2dpSdpManager() = default;

    /**
     * @brief The function is set the role of the profile.
     * @param[in] The role of local profile
     */
    void SetProfileRole(uint8_t role);

    /**
     * @brief Registers a record of the A2DP target service into the SDP protocol.
     * @return @c BT_NO_ERROR            : The function is executed successfully.
     *         Other than @c BT_NO_ERROR : The function is not executed successfully.
     */
    int RegisterService();

    /**
     * @brief Unregisters a record of the A2DP target service from the SDP protocol.
     */
    int UnregisterService(void) const;

    /**
     * @brief Finds a record of controller service from the SDP protocol.
     * @param[in] addr     The address of the peer device.
     * @param[in] a2dpInstance The instance of profile
     * @param[in] callback The callback function that receives the search result.
     * @return @c BT_NO_ERROR            : The function is executed successfully.
     *         Other than @c BT_NO_ERROR : The function is not executed successfully.
     */
    int FindSnkService(const BtAddr &addr, void *a2dpInstance, void (*callback)
        (const BtAddr *addr, const SdpService *serviceArray, uint16_t serviceNum, void *context)) const;

private:
    uint8_t role_ = A2DP_ROLE_SOURCE;
    uint32_t sdpHandle_ = 0;  // A handle to the SDP service record got from the SDP protocol.
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // A2DP_SDP_H
