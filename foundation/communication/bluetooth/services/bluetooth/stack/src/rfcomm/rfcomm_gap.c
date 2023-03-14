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

#include "rfcomm_defs.h"

void RfcommRecvChannelSecurityResult(uint16_t result, GapServiceSecurityInfo serviceInfo, void *context)
{
    LOG_INFO("%{public}s result:%hu", __func__, result);

    bool isChannelValid = RfcommIsChannelValid((RfcommChannelInfo *)context);
    if (!isChannelValid) {
        LOG_ERROR("%{public}s:DLC is closed.", __func__);
        return;
    }

    RfcommChannelEvtFsm((RfcommChannelInfo *)context, EV_CHANNEL_RECV_SECURITY_RESULT, (void *)&result);
}

void RfcommRecvSessionSecurityResult(uint16_t result, GapServiceSecurityInfo serviceInfo, void *context)
{
    LOG_INFO("%{public}s result:%hu, direction:%{public}d.", __func__, result, serviceInfo.direction);

    RfcommSessionSecurityRslt securityRslt;
    securityRslt.result = result;
    securityRslt.direction = serviceInfo.direction;

    bool isSessionValid = RfcommIsSessionValid((RfcommSessionInfo *)context);
    if (!isSessionValid) {
        LOG_ERROR("%{public}s:Session is closed.", __func__);
        return;
    }

    RfcommSessionEvtFsm((RfcommSessionInfo *)context, EV_SESSION_RECV_SECURITY_RESULT, (void *)&securityRslt);
}