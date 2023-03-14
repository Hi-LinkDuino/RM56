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

#include "socket_gap_server.h"
#include "log.h"

namespace OHOS {
namespace bluetooth {
int SocketGapServer::RegisterServiceSecurity(
    uint8_t scn, int securityFlag, GAP_Service serviceId)
{
    LOG_INFO("[sock]%{public}s", __func__);

    uint16_t securityLevel = 0;
    GapSecChannel secChan {scn};
    GAP_ServiceConnectDirection connectDirection = INCOMING;
    GapServiceSecurityInfo serviceInfo;
    serviceInfo.channelId = secChan;
    serviceInfo.direction = connectDirection;
    serviceInfo.protocolId = SEC_PROTOCOL_RFCOMM;
    serviceInfo.serviceId = serviceId;

    if (securityFlag != 0) {
        securityLevel =
            GAP_SEC_IN_ENCRYPTION | GAP_SEC_OUT_ENCRYPTION | GAP_SEC_IN_AUTHENTICATION | GAP_SEC_OUT_AUTHENTICATION;
    }

    LOG_INFO("[sock]%{public}s serviceId:%{public}d securityLevel:%hu", __func__, serviceId, securityLevel);
    return GAPIF_RegisterServiceSecurity(nullptr, &serviceInfo, securityLevel);
}

int SocketGapServer::UnregisterSecurity(const BtAddr addr, uint8_t scn, GAP_Service serviceId)
{
    LOG_INFO("[sock]%{public}s", __func__);
    GAP_ServiceConnectDirection connectDirection;
    GapSecChannel secChan {scn};
    connectDirection = INCOMING;
    GapServiceSecurityInfo serviceInfo;
    serviceInfo.channelId = secChan;
    serviceInfo.direction = connectDirection;
    serviceInfo.protocolId = SEC_PROTOCOL_RFCOMM;
    serviceInfo.serviceId = serviceId;

    LOG_INFO("[sock]%{public}s serviceId:%{public}d ", __func__, serviceId);

    return GAPIF_DeregisterServiceSecurity(&addr, &serviceInfo);
}
}  // namespace bluetooth
}  // namespace OHOS