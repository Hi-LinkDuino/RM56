/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "camera_client.h"
#include "media_log.h"
#include "samgr_lite.h"
#include "camera_type.h"
#include "liteipc_adapter.h"

#include <cstdio>

extern "C" void __attribute__((weak)) HOS_SystemInit(void)
{
    SAMGR_Bootstrap();
}

using namespace std;
namespace OHOS {
namespace Media {
CameraClient *CameraClient::GetInstance()
{
    static CameraClient client;
    return &client;
}

bool CameraClient::InitCameraClient()
{
    HOS_SystemInit();
    if (proxy_ == nullptr) {
        IUnknown *iUnknown = SAMGR_GetInstance()->GetDefaultFeatureApi(SERVICE_NAME);
        if (iUnknown == nullptr) {
            MEDIA_ERR_LOG("Camera server connecting failed.");
            return false;
        }
        (void)iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, (void **)&proxy_);
        if (proxy_ == nullptr) {
            MEDIA_ERR_LOG("QueryInterface failed.");
            return false;
        }
    }
    return true;
}

IClientProxy *CameraClient::GetIClientProxy()
{
    return proxy_;
}

void CameraClient::ClearIpcMsg(void *ipcMsg)
{
    if (ipcMsg == nullptr) {
        return;
    }
    uint32_t flag = 0;
    GetFlag(ipcMsg, &flag);
    if (flag == LITEIPC_FLAG_DEFAULT) {
        // send reply to client, the second param is reply
        SendReply(nullptr, ipcMsg, nullptr);
    } else {
        FreeBuffer(nullptr, ipcMsg);
    }
}
} // namespace Media
} // namespace OHOS
