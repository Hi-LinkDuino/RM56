/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ipc_dev_auth_proxy.h"

#include "common_defs.h"
#include "hc_log.h"
#include "ipc_adapt.h"
#include "ipc_sdk.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
ProxyDevAuth::ProxyDevAuth(const sptr<IRemoteObject> &impl) : IRemoteProxy<IMethodsIpcCall>(impl)
{}

ProxyDevAuth::~ProxyDevAuth()
{}

int32_t ProxyDevAuth::DoCallRequest(MessageParcel &dataParcel, MessageParcel &replyParcel, bool withSync)
{
    int32_t ret;
    sptr<IRemoteObject> remote = nullptr;
    MessageOption option = { MessageOption::TF_SYNC };

    LOGI("ProxyDevAuth, SendRequest...");
    remote = Remote();
    if (remote == nullptr) {
        LOGE("Proxy DoCallRequest Remote() is null");
        return HC_ERR_IPC_INTERNAL_FAILED;
    }

    if (withSync == false) {
        option = { MessageOption::TF_ASYNC };
    }
    ret = remote->SendRequest(DEV_AUTH_CALL_REQUEST, dataParcel, replyParcel, option);
    LOGI("SendRequest done, ret %d", ret);
    (ret == ERR_NONE) ? replyParcel.ReadInt32(ret) : (ret = HC_ERR_IPC_INTERNAL_FAILED);
    return ret;
}

bool ProxyDevAuth::ServiceRunning(void)
{
    auto saMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saMgr == nullptr) {
        return false;
    }
    auto daSa = saMgr->GetSystemAbility(DEVICE_AUTH_SERVICE_ID);
    if (daSa == nullptr) {
        return false;
    }
    return true;
}

int32_t ProxyDevAuthData::EncodeCallRequest(int32_t type, const uint8_t *param, int32_t paramSz)
{
    LOGI("type %d, paramSz %d", type, paramSz);
    if (tmpDataParcel.WriteInt32(type) && tmpDataParcel.WriteInt32(paramSz) &&
        tmpDataParcel.WriteBuffer(reinterpret_cast<const void *>(param), static_cast<size_t>(paramSz))) {
        paramCnt++;
        return HC_SUCCESS;
    }
    return HC_ERROR;
}

int32_t ProxyDevAuthData::FinalCallRequest(int32_t methodId)
{
    int32_t dataLen;
    const uint8_t *dataPtr = nullptr;

    dataLen = static_cast<int32_t>(tmpDataParcel.GetDataSize());
    dataPtr = const_cast<const uint8_t *>(reinterpret_cast<uint8_t *>(tmpDataParcel.GetData()));
    if ((dataLen <= 0) || (dataPtr == nullptr)) {
        LOGE("data invalid");
        return HC_ERROR;
    }
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        LOGE("get proxy failed");
        return HC_ERR_IPC_GET_PROXY;
    }
    if (!dataParcel.WriteInterfaceToken(proxy->GetDescriptor())) {
        LOGE("Failed to write interface token!");
        return HC_ERROR;
    }
    LOGI("method id %d, param num %d, data length %d", methodId, paramCnt, dataLen);
    /* request data length = number of params + params information */
    if (!dataParcel.WriteInt32(methodId) || !dataParcel.WriteInt32(dataLen + sizeof(int32_t)) ||
        !dataParcel.WriteInt32(paramCnt)) {
        return HC_ERROR;
    }
    if (!dataParcel.WriteBuffer(reinterpret_cast<const void *>(dataPtr), static_cast<size_t>(dataLen))) {
        return HC_ERROR;
    }
    if (withCallback) {
        if (!dataParcel.WriteInt32(PARAM_TYPE_CB_OBJECT) || !dataParcel.WriteRemoteObject(cbStub)) {
            return HC_ERROR;
        }
        LOGI("type %d, cbStub %s", PARAM_TYPE_CB_OBJECT, (cbStub != nullptr) ? "true" : "false");
    }
    cbStub = nullptr;
    withCallback = false;
    return HC_SUCCESS;
}

sptr<ProxyDevAuth> ProxyDevAuthData::GetProxy() const
{
    auto saMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saMgr == nullptr) {
        LOGE("GetSystemAbilityManager failed");
        return nullptr;
    }
    auto daSa = saMgr->GetSystemAbility(DEVICE_AUTH_SERVICE_ID);
    if (daSa == nullptr) {
        LOGE("GetSystemAbility failed");
        return nullptr;
    }

    return iface_cast<ProxyDevAuth>(daSa);
}

int32_t ProxyDevAuthData::ActCall(bool withSync)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        LOGE("proxy failed");
        return HC_ERR_IPC_GET_PROXY;
    }
    return proxy->DoCallRequest(dataParcel, replyParcel, withSync);
}

MessageParcel *ProxyDevAuthData::GetReplyParcel(void)
{
    return &replyParcel;
}

void ProxyDevAuthData::SetCallbackStub(sptr<IRemoteObject> cbRemote)
{
    if (cbRemote != nullptr) {
        this->cbStub = cbRemote;
        withCallback = true;
    }
    return;
}
}
