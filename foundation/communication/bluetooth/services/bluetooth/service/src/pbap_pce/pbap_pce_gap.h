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

#ifndef PBAP_PCE_GAP_H
#define PBAP_PCE_GAP_H
#include <cstdint>
#include <memory>
#include "gap_if.h"
#include "interface_profile_pbap_pce.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
class PbapPceService;
/**
 * @brief pbap pce gap service
 * pbap pce gap service
 */
class PbapPceGap {
public:
    /**
     * @brief constructor
     * @details constructor
     * @param service PbapPceService
     * @param device remote device
     * @param channel rfcomm channel or l2cap psm
     * @param rfcommOrPsm true:l2cap false:rfcomm
     * @return
     */
    PbapPceGap(const IProfilePbapPce &service, const RawAddress &device, GapSecChannel channel, bool rfcommOrPsm);

    /**
     * @brief deconstructor
     * @details deconstructor
     * @return
     */
    virtual ~PbapPceGap();

    /**
     * @brief register gap
     * @details register gap service
     * @return int @c 0 success
     *             @c not 0 failure
     */
    int Register();

    /**
     * @brief deregister gap
     * @details deregister gap service
     * @return int @c 0 success
     *             @c not 0 failure
     */
    int Deregister();

    /**
     * @brief RequestSecurity
     * @details RequestSecurity
     * @return int @c 0 success
     *             @c not 0 failure
     */
    int RequestSecurity();

private:
    void RequestSecurityCallback(uint16_t result) const;
    static void RequestSecurityCallback(uint16_t result, GapServiceSecurityInfo security, void *context);

private:
    bool registered_ = false;          // registered or not
    const IProfilePbapPce &service_;   // PbapPceService ref
    RawAddress device_ {};             // remote device address
    GapSecChannel gapSecChannel_ {};   // rfcomm channel or l2cap psm
    bool l2capOrRfcomm_ = false;       // true:l2cap false:rfcomm
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PBAP_PCE_GAP_H
