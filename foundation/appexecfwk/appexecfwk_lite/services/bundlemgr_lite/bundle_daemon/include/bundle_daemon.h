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

#ifndef OHOS_BUNDLE_DAEMON_H
#define OHOS_BUNDLE_DAEMON_H

#include "bundle_daemon_handler.h"
#include "bundle_daemon_interface.h"
#include "bundlems_client.h"
#include "nocopyable.h"
#include "service.h"

namespace OHOS {
typedef int32_t (*InvokeFunc)(IpcIo *req);
class BundleDaemon : public Service, NoCopyable {
public:
    static BundleDaemon &GetInstance()
    {
        static BundleDaemon instance;
        return instance;
    }
    static int32_t Invoke(IServerProxy *iProxy, int32_t funcId, void *origin, IpcIo *req, IpcIo *reply);
private:
    static const char *GetServiceName(Service *service);
    static BOOL ServiceInitialize(Service *service, Identity identity);
    static TaskConfig GetServiceTaskConfig(Service *service);
    static BOOL ServiceMessageHandle(Service *service, Request *request);
private:
    BundleDaemon();
    ~BundleDaemon();
    Identity identity_ {};
    BundleDaemonHandler handler_ {};
    BundleMsClient *bundleMsClient_ { nullptr };

    static int32_t RegisterCallbackInvoke(IpcIo *req);
    static int32_t ExtractHapInvoke(IpcIo *req);
    static int32_t RenameFileInvoke(IpcIo *req);
    static int32_t CreatePermissionInvoke(IpcIo *req);
    static int32_t CreateDataDirectoryInvoke(IpcIo *req);
    static int32_t StoreContentToFileInvoke(IpcIo *req);
    static int32_t MoveFileInvoke(IpcIo *req);
    static int32_t RemoveFileInvoke(IpcIo *req);
    static int32_t RemoveInstallDirectoryInvoke(IpcIo *req);
    static constexpr InvokeFunc invokeFuncs[BDS_CMD_END] {
        BundleDaemon::ExtractHapInvoke,
        BundleDaemon::RenameFileInvoke,
        BundleDaemon::CreatePermissionInvoke,
        BundleDaemon::CreateDataDirectoryInvoke,
        BundleDaemon::StoreContentToFileInvoke,
        BundleDaemon::MoveFileInvoke,
        BundleDaemon::RemoveFileInvoke,
        BundleDaemon::RemoveInstallDirectoryInvoke,
    };
};

struct DefaultFeatureApi {
    INHERIT_SERVER_IPROXY;
};

typedef struct {
    INHERIT_IUNKNOWNENTRY(DefaultFeatureApi);
    Identity identity;
} BundleDaemonFeature;
} // OHOS
#endif // OHOS_BUNDLE_DAEMON_H
