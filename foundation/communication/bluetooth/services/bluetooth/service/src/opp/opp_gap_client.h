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

#ifndef OPP_GAP_CLIENT_H
#define OPP_GAP_CLIENT_H
#include <cstdint>
#include <memory>
#include "gap_if.h"
#include "log.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief opp gap client
 * opp gap client
 */
class OppGapClient {
public:
    /**
     * @brief constructor
     * @details constructor
     * @param address remote device
     * @param channel rfcomm channel or l2cap psm
     * @param rfcommOrPsm true:l2cap false:rfcomm
     * @return
     */
    OppGapClient(const std::string &address, GapSecChannel channel, bool rfcommOrPsm);

    /**
     * @brief deconstructor
     * @details deconstructor
     * @return
     */
    virtual ~OppGapClient();

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
    std::string address_;              // remote device address
    GapSecChannel gapSecChannel_ {};   // rfcomm channel or l2cap psm
    bool l2capOrRfcomm_ = false;       // true:l2cap false:rfcomm
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // OPP_GAP_CLIENT_H
