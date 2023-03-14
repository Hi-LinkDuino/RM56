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

#include "hfp_ag_rfcomm_connection_server.h"

#include "hfp_ag_defines.h"
#include "hfp_ag_gap_server.h"

namespace OHOS {
namespace bluetooth {
int HfpAgRfcommConnectionServer::RegisterServer(RFCOMM_EventCallback fn) const
{
    int ret = HfpAgGapServer::RegisterServiceSecurity(localScn_);
    HFP_AG_RETURN_IF_FAIL(ret);

    ret = RFCOMM_RegisterServer(localScn_, HFP_AG_COMMAND_MTU, HFP_RFCOMM_CONNECTION_SERVER_EVENTS, fn, nullptr);
    HFP_AG_RETURN_IF_FAIL(ret);
    return BT_NO_ERROR;
}

int HfpAgRfcommConnectionServer::RemoveServer()
{
    int ret = HfpAgGapServer::DeregisterServiceSecurity();
    HFP_AG_RETURN_IF_FAIL(ret);

    ret = RFCOMM_DeregisterServer(localScn_, true);
    HFP_AG_RETURN_IF_FAIL(ret);

    ret = RFCOMM_FreeServerNum(localScn_);
    HFP_AG_RETURN_IF_FAIL(ret);

    localScn_ = 0;
    return BT_NO_ERROR;
}

int HfpAgRfcommConnectionServer::AcceptConnection(uint16_t handle)
{
    int ret = RFCOMM_AcceptConnection(handle);
    HFP_AG_RETURN_IF_FAIL(ret);
    return BT_NO_ERROR;
}

int HfpAgRfcommConnectionServer::RejectConnection(uint16_t handle)
{
    int ret = RFCOMM_RejectConnection(handle);
    HFP_AG_RETURN_IF_FAIL(ret);
    return BT_NO_ERROR;
}

uint8_t HfpAgRfcommConnectionServer::AssignLocalScn()
{
    localScn_ = RFCOMM_AssignServerNum();
    return localScn_;
}

uint8_t HfpAgRfcommConnectionServer::GetLocalScn() const
{
    return localScn_;
}
}  // namespace bluetooth
}  // namespace OHOS