/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "ability_self_callback.h"

#include "adapter.h"
#include "ability_callback_utils.h"
#include "ability_manager.h"
#include "iproxy_client.h"
#include "log.h"
#include "samgr_lite.h"

namespace OHOS {
AbilitySelfCallback::~AbilitySelfCallback()
{
    if (svcIdentity_ != nullptr) {
        (void) UnregisterIpcCallback(*svcIdentity_);
        AdapterFree(svcIdentity_);
    }
}

int32_t InnerCallback(const char *resultMessage, uint8_t resultCode, const IAbilityStartCallback &iAbilityStartCallback)
{
    if (resultMessage == nullptr || iAbilityStartCallback == nullptr) {
        return PARAM_NULL_ERROR;
    }
    if (resultCode == ERR_OK) {
        iAbilityStartCallback(resultCode, resultMessage);
    } else {
        iAbilityStartCallback(resultCode, ObtainErrorMessage(resultCode).c_str());
    }
    return ERR_OK;
}

int32_t AbilitySelfCallback::Callback(const IpcContext *context, void *ipcMsg, IpcIo *io, void *arg)
{
    if (ipcMsg == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "AbilitySelfCallback ipcMsg is nullptr");
        return PARAM_NULL_ERROR;
    }

    if (io == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "AbilitySelfCallback io is nullptr");
        FreeBuffer(NULL, ipcMsg);
        return PARAM_NULL_ERROR;
    }
    IAbilityStartCallback iAbilityStartCallback = GetInstance().GetCallback();
    if (iAbilityStartCallback == nullptr) {
        FreeBuffer(NULL, ipcMsg);
        return PARAM_NULL_ERROR;
    }
    uint32_t callbackType = 0;
    int32_t ret = GetCode(ipcMsg, &callbackType);
    if (ret != LITEIPC_OK) {
        FreeBuffer(NULL, ipcMsg);
        HILOG_ERROR(HILOG_MODULE_APP, "AbilitySelfCallbck get callback type failed");
        return GET_CALLBACK_TYPE_ERROR;
    }
    auto resultCode = static_cast<uint8_t>(IpcIoPopInt32(io));
    FreeBuffer(NULL, ipcMsg);
    if (callbackType == START_ABILITY_CALLBACK) {
        return InnerCallback(START_ABILITY_SUCCESS, resultCode, iAbilityStartCallback);
    }
    HILOG_ERROR(HILOG_MODULE_APP, "AbilitySelfCallback get error callback type");
    return GET_CALLBACK_TYPE_ERROR;
}

int32 AbilitySelfCallback::GenerateLocalServiceId()
{
    svcIdentity_ = reinterpret_cast<SvcIdentity *>(AdapterMalloc(sizeof(SvcIdentity)));
    if (svcIdentity_ == nullptr) {
        return CALLBACK_GENERATE_LOCAL_SERVICEID_FAILED;
    }

    int32_t ret = RegisterIpcCallback(Callback, 0, IPC_WAIT_FOREVER, svcIdentity_, NULL);
    if (ret != LITEIPC_OK) {
        AdapterFree(svcIdentity_);
        svcIdentity_ = nullptr;
        return CALLBACK_GENERATE_LOCAL_SERVICEID_FAILED;
    }
    return ERR_OK;
}

const SvcIdentity *AbilitySelfCallback::RegisterAbilitySelfCallback(IAbilityStartCallback &iAbilityStartCallback)
{
    if (iAbilityStartCallback == nullptr) {
        return nullptr;
    }
    if (svcIdentity_ == nullptr) {
        int32 ret = GenerateLocalServiceId();
        if (ret != ERR_OK) {
            return nullptr;
        }
    }
    iAbilityStartCallback_ = iAbilityStartCallback;
    return svcIdentity_;
}

const IAbilityStartCallback AbilitySelfCallback::GetCallback()
{
    return iAbilityStartCallback_;
}
} // namespace