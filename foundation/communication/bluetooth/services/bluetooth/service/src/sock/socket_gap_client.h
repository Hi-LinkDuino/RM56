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

#ifndef SOCKET_GAP_CLIENT_H
#define SOCKET_GAP_CLIENT_H

#include <stdint.h>
#include "base_def.h"
#include "gap_if.h"

namespace OHOS {
namespace bluetooth {
class SocketGapClient {
public:
    /**
     * @brief Constructor.
     */
    SocketGapClient() = default;

    /**
     * @brief Destructor.
     */
    virtual ~SocketGapClient()
    {}

    /**
     * @brief Register Service Security
     *
     * @param scn server channel number
     * @param isServer is server or not
     * @param securityFlag require the connection to be encrypted and authenticated.
     * @return int
     */
    static int RegisterServiceSecurity(
        const BtAddr addr, uint8_t scn, int securityFlag, GAP_Service serviceId);

    /**
     * @brief UnRegister Service Security
     *
     * @param serviceId service id
     * @return int
     */
    static int UnregisterSecurity(const BtAddr addr, uint8_t scn, GAP_Service serviceId);

private:
    static uint8_t AssignServiceId();
    BT_DISALLOW_COPY_AND_ASSIGN(SocketGapClient);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // SOCKET_GAP_CLIENT_H
