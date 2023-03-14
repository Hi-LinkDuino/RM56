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

#ifndef OHOS_BUNLDE_DAEMON_CLIENT_H
#define OHOS_BUNLDE_DAEMON_CLIENT_H

#include <semaphore.h>

#include "ohos_errno.h"
#include "iproxy_client.h"
#include "liteipc_adapter.h"
#include "mutex_lock.h"
#include "nocopyable.h"

namespace OHOS {
class BundleDaemonClient : public NoCopyable {
public:
    static BundleDaemonClient &GetInstance()
    {
        static BundleDaemonClient instance;
        return instance;
    }
    bool Initialize();
    int32_t ExtractHap(const char *hapFile, const char *codePath);
    int32_t RenameFile(const char *oldFile, const char *newFile);
    int32_t CreatePermissionDir();
    int32_t CreateDataDirectory(const char *dataPath, int32_t uid, int32_t gid, bool isChown);
    int32_t StoreContentToFile(const char *file, const void *buffer, uint32_t size);
    int32_t MoveFile(const char *oldFile, const char *newFile);
    int32_t RemoveFile(const char *file);
    int32_t RemoveInstallDirectory(const char *codePath, const char *dataPath, bool keepData);
    int32_t CallClientInvoke(int32_t funcId, const char *firstPath, const char *secondPath);
    static int32_t BundleDaemonCallback(const IpcContext* context, void *ipcMsg, IpcIo *io, void *arg);
    static int32_t DeathCallback(const IpcContext* context, void* ipcMsg, IpcIo* data, void* arg);
    static int Notify(IOwner owner, int code, IpcIo *reply);

private:
    BundleDaemonClient() = default;
    ~BundleDaemonClient();

    IClientProxy *bdsClient_ { nullptr };
    SvcIdentity svcIdentity_ {};
    SvcIdentity bdsSvcIdentity_ {};
    uint32_t cbid_ = INVALID_INDEX;
    sem_t sem_;
    Mutex mutex_;
    int32_t result_ = EC_FAILURE;
    bool initialized_ = false;

    static void *RegisterDeathCallback(void *);
    int32_t RegisterCallback();
    int32_t WaitResultSync(int32_t result);
};
} // namespace OHOS
#endif // OHOS_BUNLDE_DAEMON_CLIENT_H
