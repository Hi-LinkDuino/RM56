/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ipc_client_server_proxy.h"

#include "device_manager_notify.h"
#include "dm_constants.h"
#include "dm_log.h"
#include "ipc_cmd_register.h"
#include "ipc_def.h"

namespace OHOS {
namespace DistributedHardware {
const int32_t INVALID_CB_ID = 0xFF;
static uint32_t g_deathCbId = INVALID_CB_ID;
static SvcIdentity g_svcIdentity;
static std::shared_ptr<IpcRsp> pCurRsp;

void __attribute__((weak)) HOS_SystemInit(void)
{
    SAMGR_Bootstrap();
    return;
}

int32_t DmDeathCallback(const IpcContext *ctx, void *ipcMsg, IpcIo *data, void *arg)
{
    (void)ctx;
    (void)ipcMsg;
    (void)data;
    (void)arg;
    LOGI("ATTENTION  SERVICE (%s) DEAD !!!\n", DEVICE_MANAGER_SERVICE_NAME);
    UnregisterDeathCallback(g_svcIdentity, g_deathCbId);
    g_deathCbId = INVALID_CB_ID;
    g_svcIdentity.handle = 0;
    g_svcIdentity.token = 0;
    g_svcIdentity.cookie = 0;
    DeviceManagerNotify::GetInstance().OnRemoteDied();
    return DM_OK;
}

static int32_t SendCmdResultCb(IOwner owner, int32_t code, IpcIo *reply)
{
    (void)code;
    int32_t cmdCode = *(int32_t *)owner;
    LOGI("SendCmdResultCb code:%d", cmdCode);
    (void)IpcCmdRegister::GetInstance().ReadResponse(cmdCode, *reply, pCurRsp);
    return DM_OK;
}

IClientProxy *IpcClientServerProxy::GetServerProxy(void)
{
    IClientProxy *clientProxy = nullptr;
    IUnknown *iUnknown = nullptr;

    LOGI("start get client proxy");
    iUnknown = SAMGR_GetInstance()->GetDefaultFeatureApi(DEVICE_MANAGER_SERVICE_NAME);
    if (iUnknown == nullptr) {
        return nullptr;
    }
    if (iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, (void **)&clientProxy) != DM_OK ||
        clientProxy == nullptr) {
        LOGE("QueryInterface failed");
    }
    return clientProxy;
}

int IpcClientServerProxy::RegisterServerDeathCb(void)
{
    g_svcIdentity = SAMGR_GetRemoteIdentity(DEVICE_MANAGER_SERVICE_NAME, nullptr);
    g_deathCbId = INVALID_CB_ID;
    if (RegisterDeathCallback(nullptr, g_svcIdentity, DmDeathCallback, nullptr, &g_deathCbId) != EC_SUCCESS) {
        LOGE("reg death callback failed");
        return DM_FAILED;
    }
    return DM_OK;
}

int32_t IpcClientServerProxy::SendCmd(int32_t cmdCode, std::shared_ptr<IpcReq> req, std::shared_ptr<IpcRsp> rsp)
{
    LOGI("IpcClientServerProxy::SendCmd:%d", cmdCode);
    uint8_t data[MAX_DM_IPC_LEN] = {0};
    IpcIo request;

    if (IpcCmdRegister::GetInstance().SetRequest(cmdCode, req, request, data, MAX_DM_IPC_LEN) != DM_OK) {
        return DM_FAILED;
    }
    {
        std::lock_guard<std::mutex> autoLock(lock_);
        pCurRsp = rsp;
        if (serviceProxy_ != nullptr &&
            serviceProxy_->Invoke(serviceProxy_, cmdCode, &request, &cmdCode, SendCmdResultCb) != 0) {
            LOGE("serviceProxy_ invoke failed.");
            return DM_FAILED;
        }
    }
    LOGI("IpcClientServerProxy::SendCmd:%d, end", cmdCode);
    return DM_OK;
}

int32_t IpcClientServerProxy::Init(void)
{
    if (serviceProxy_ != nullptr) {
        LOGI("ServerProxy already Init");
        return DM_OK;
    }
    HOS_SystemInit();
    serviceProxy_ = GetServerProxy();
    if (serviceProxy_ == nullptr) {
        LOGE("get ipc client proxy failed");
        return DM_FAILED;
    }
    if (RegisterServerDeathCb() != DM_OK) {
        LOGE("register server death cb failed");
        return DM_FAILED;
    }
    LOGI("ServerProxyInit ok");
    return DM_OK;
}
} // namespace DistributedHardware
} // namespace OHOS
