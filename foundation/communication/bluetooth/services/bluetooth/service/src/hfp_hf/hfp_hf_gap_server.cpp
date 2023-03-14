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

#include "hfp_hf_gap_server.h"

#include "hfp_hf_defines.h"

namespace OHOS {
namespace bluetooth {
GapServiceSecurityInfo HfpHfGapServer::g_securityInfo;

int HfpHfGapServer::RegisterServiceSecurity(uint8_t scn)
{
    GapSecChannel secChan {scn};
    g_securityInfo.direction = INCOMING;
    g_securityInfo.protocolId = SEC_PROTOCOL_RFCOMM;
    g_securityInfo.channelId = secChan;
    g_securityInfo.serviceId = HFP_HF;

    int ret = GAPIF_RegisterServiceSecurity(nullptr,
        &g_securityInfo,
        GAP_SEC_IN_ENCRYPTION | GAP_SEC_IN_AUTHENTICATION | GAP_SEC_OUT_ENCRYPTION | GAP_SEC_OUT_AUTHENTICATION);
    HFP_HF_RETURN_IF_FAIL(ret);
    return BT_NO_ERROR;
}

int HfpHfGapServer::DeregisterServiceSecurity()
{
    int ret = GAPIF_DeregisterServiceSecurity(nullptr, &g_securityInfo);
    HFP_HF_RETURN_IF_FAIL(ret);
    return BT_NO_ERROR;
}
}  // namespace bluetooth
}  // namespace OHOS