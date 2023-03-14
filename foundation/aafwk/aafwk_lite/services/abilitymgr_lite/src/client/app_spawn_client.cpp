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

#include "client/app_spawn_client.h"

#include "appspawn_service.h"
#include "cJSON.h"
#include "ohos_errno.h"
#include "liteipc_adapter.h"
#include "samgr_lite.h"
#include "securec.h"
#include "util/abilityms_log.h"

namespace OHOS {
const unsigned long SLEEP_TIMES = 200000;
const int RETRY_TIMES_MAX = 30;

static int Notify(IOwner owner, int code, IpcIo *reply)
{
    if (reply == nullptr || owner == nullptr) {
        return EC_INVALID;
    }
    int64_t *result = reinterpret_cast<int64_t *>(owner);
    *result = IpcIoPopInt64(reply);
    return EC_SUCCESS;
}

AbilityMsStatus AppSpawnClient::Initialize()
{
    while (spawnClient_ == nullptr) {
        IUnknown *iUnknown = SAMGR_GetInstance()->GetDefaultFeatureApi(APPSPAWN_SERVICE_NAME);
        if (iUnknown == nullptr) {
            PRINTW("AppSpawnClient", "get default feature api fail, again try");
            usleep(SLEEP_TIMES); // sleep 200ms
            continue;
        }
        int result = iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, (void **)(&spawnClient_));
        if (result != EC_SUCCESS || spawnClient_ == nullptr) {
            PRINTW("AppSpawnClient", "get app spawn client fail");
            usleep(SLEEP_TIMES); // sleep 200ms
            continue;
        }
    }
    return AbilityMsStatus::Ok();
}

static void  InnerFreeDataBuff(void *ptr)
{
    if (ptr != nullptr) {
        cJSON_free(ptr);
    }
}

AbilityMsStatus AppSpawnClient::CallingInnerSpawnProcess(char *spawnMessage, AppRecord &appRecord)
{
    if (spawnMessage == nullptr) {
        return AbilityMsStatus::ProcessStatus("CallingInnerSpawnProcess spawnMessage is nullptr");
    }
    if (spawnClient_ == nullptr) {
        AbilityMsStatus status = Initialize();
        if (!status.IsOk()) {
            cJSON_free(spawnMessage);
            return AbilityMsStatus::ProcessStatus("CallingInnerSpawnProcess Initialize unsuccessfully");
        }
    }

    IpcIo request;
    char data[IPC_IO_DATA_MAX];
#ifdef __LINUX__
    IpcIoInit(&request, data, IPC_IO_DATA_MAX, 0);
    IpcIoPushString(&request, spawnMessage);
#else
    IpcIoInit(&request, data, IPC_IO_DATA_MAX, 1);
    BuffPtr dataBuff = {
        .buffSz = strlen(spawnMessage) + 1, // include \0
        .buff = spawnMessage,
    };
    IpcIoPushDataBuffWithFree(&request, &dataBuff, InnerFreeDataBuff);
#endif
    pid_t pid = -1;
    int result = spawnClient_->Invoke(spawnClient_, ID_CALL_CREATE_SERVICE, &request, &pid, Notify);
    int retry = 0;
    while (result != EC_SUCCESS && retry < RETRY_TIMES_MAX) {
        ++retry;
        PRINTI("AppManager", "invoke fail: %{public}d, retry times: %{public}d", result, retry);
        usleep(SLEEP_TIMES); // sleep 200ms if invoke fail.
        result = spawnClient_->Invoke(spawnClient_, ID_CALL_CREATE_SERVICE, &request, &pid, Notify);
    }
    if (result != EC_SUCCESS) {
        return AbilityMsStatus::ProcessStatus("spawn process fail");
    }
    appRecord.SetPid(pid);
    return AbilityMsStatus::Ok();
}

AbilityMsStatus AppSpawnClient::SpawnProcess(AppRecord &appRecord)
{
    char *innerBundleName = appRecord.GetBundleInfo().bundleName;
    if (innerBundleName == nullptr) {
        return AbilityMsStatus::ProcessStatus("invalid argument");
    }

    cJSON *root = cJSON_CreateObject();
    if (root == nullptr) {
        return AbilityMsStatus::ProcessStatus("SpawnProcess create fail");
    }
    cJSON_AddStringToObject(root, "bundleName", innerBundleName);
    std::string identityId = std::to_string(appRecord.GetIdentityId());
    cJSON_AddStringToObject(root, "identityID", identityId.c_str());
    cJSON_AddNumberToObject(root, "uID", appRecord.GetBundleInfo().uid);
    cJSON_AddNumberToObject(root, "gID", appRecord.GetBundleInfo().gid);

    cJSON *caps = cJSON_AddArrayToObject(root, "capability");
    if (caps == nullptr) {
        cJSON_Delete(root);
        return AbilityMsStatus::ProcessStatus("SpawnProcess create array unsuccessfully");
    }

    uint32_t *capabilities = nullptr;
    uint32_t capNums = 0;
    AbilityMsStatus status = appRecord.QueryAppCapability(innerBundleName, &capabilities, &capNums);
    if (!status.IsOk()) {
        cJSON_Delete(root);
        return AbilityMsStatus::ProcessStatus("SpawnProcess QueryAppCapability unsuccessfully");
    }
    if ((capabilities != nullptr) && (capNums != 0)) {
        for (int32_t i = 0; i < capNums; ++i) {
            cJSON *item = cJSON_CreateNumber(capabilities[i]);
            if ((item == nullptr) || !cJSON_AddItemToArray(caps, item)) {
                free(capabilities);
                capabilities = nullptr;
                cJSON_Delete(item);
                cJSON_Delete(root);
                return AbilityMsStatus::ProcessStatus("SpawnProcess add item to array unsuccessfully");
            }
        }
        free(capabilities);
        capabilities = nullptr;
    }
    char *spawnMessage = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    return CallingInnerSpawnProcess(spawnMessage, appRecord);
}
} // namespace OHOS
