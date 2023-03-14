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

#include "hfp_hf_rfcomm_connection_server.h"

#include "hfp_hf_defines.h"
#include "hfp_hf_gap_server.h"

namespace OHOS {
namespace bluetooth {
int HfpHfRfcommConnectionServer::RegisterServer(RFCOMM_EventCallback fn) const
{
    int ret = HfpHfGapServer::RegisterServiceSecurity(localScn_);
    HFP_HF_RETURN_IF_FAIL(ret);

    ret = RFCOMM_RegisterServer(localScn_, HFP_HF_COMMAND_MTU, HFP_RFCOMM_CONNECTION_SERVER_EVENTS, fn, nullptr);
    HFP_HF_RETURN_IF_FAIL(ret);
    return BT_NO_ERROR;
}

int HfpHfRfcommConnectionServer::RemoveServer()
{
    int ret = HfpHfGapServer::DeregisterServiceSecurity();
    HFP_HF_RETURN_IF_FAIL(ret);

    ret = RFCOMM_DeregisterServer(localScn_, true);
    HFP_HF_RETURN_IF_FAIL(ret);

    ret = RFCOMM_FreeServerNum(localScn_);
    HFP_HF_RETURN_IF_FAIL(ret);

    localScn_ = 0;
    return BT_NO_ERROR;
}

int HfpHfRfcommConnectionServer::AcceptConnection(uint16_t handle)
{
    int ret = RFCOMM_AcceptConnection(handle);
    HFP_HF_RETURN_IF_FAIL(ret);
    return BT_NO_ERROR;
}

int HfpHfRfcommConnectionServer::RejectConnection(uint16_t handle)
{
    int ret = RFCOMM_RejectConnection(handle);
    HFP_HF_RETURN_IF_FAIL(ret);
    return BT_NO_ERROR;
}

uint8_t HfpHfRfcommConnectionServer::AssignLocalScn()
{
    localScn_ = RFCOMM_AssignServerNum();
    return localScn_;
}

uint8_t HfpHfRfcommConnectionServer::GetLocalScn() const
{
    return localScn_;
}
}  // namespace bluetooth
}  // namespace OHOS