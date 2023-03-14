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

#include "avrcp_ct_gap.h"
#include "avrcp_ct_internal.h"
#include "gap_if.h"

namespace OHOS {
namespace bluetooth {
AvrcCtGapManager::AvrcCtGapManager()
{
    HILOGI("enter");
}

AvrcCtGapManager::~AvrcCtGapManager()
{
    HILOGI("enter");
}

int AvrcCtGapManager::RegisterSecurity(void)
{
    HILOGI("enter");

    int result = RET_NO_ERROR;

    GapServiceSecurityInfo avctInfo = {INCOMING, AVRCP_CT, SEC_PROTOCOL_L2CAP, {AVCT_PSM}};
    result |= GAPIF_RegisterServiceSecurity(nullptr, &avctInfo, GAP_SEC_IN_AUTHENTICATION | GAP_SEC_OUT_AUTHENTICATION);
    avctInfo.direction = OUTGOING;
    result |= GAPIF_RegisterServiceSecurity(nullptr, &avctInfo, GAP_SEC_IN_AUTHENTICATION | GAP_SEC_OUT_AUTHENTICATION);

    GapServiceSecurityInfo avctBrinfo = {INCOMING, AVRCP_CT_BROWSING, SEC_PROTOCOL_L2CAP, {AVCT_BR_PSM}};
    result |=
        GAPIF_RegisterServiceSecurity(nullptr, &avctBrinfo, GAP_SEC_IN_AUTHENTICATION | GAP_SEC_OUT_AUTHENTICATION);
    avctBrinfo.direction = OUTGOING;
    result |=
        GAPIF_RegisterServiceSecurity(nullptr, &avctBrinfo, GAP_SEC_IN_AUTHENTICATION | GAP_SEC_OUT_AUTHENTICATION);

    (result == BT_NO_ERROR) ? (result = RET_NO_ERROR) : (result = RET_BAD_STATUS);

    return result;
}

int AvrcCtGapManager::UnregisterSecurity(void)
{
    HILOGI("enter");

    int result = RET_NO_ERROR;

    GapServiceSecurityInfo avctInfo = {INCOMING, AVRCP_CT, SEC_PROTOCOL_L2CAP, {AVCT_PSM}};
    result |= GAPIF_DeregisterServiceSecurity(nullptr, &avctInfo);
    avctInfo.direction = OUTGOING;
    result |= GAPIF_DeregisterServiceSecurity(nullptr, &avctInfo);

    GapServiceSecurityInfo avctBrinfo = {INCOMING, AVRCP_CT_BROWSING, SEC_PROTOCOL_L2CAP, {AVCT_BR_PSM}};
    result |= GAPIF_DeregisterServiceSecurity(nullptr, &avctBrinfo);
    avctBrinfo.direction = OUTGOING;
    result |= GAPIF_DeregisterServiceSecurity(nullptr, &avctBrinfo);

    (result == BT_NO_ERROR) ? (result = RET_NO_ERROR) : (result = RET_BAD_STATUS);

    return result;
}
}  // namespace bluetooth
}  // namespace OHOS