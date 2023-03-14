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

#include <ohos_init.h>
#include <ohos_errno.h>

#include "media_log.h"
#include "iproxy_client.h"
#include "iproxy_server.h"
#include "iunknown.h"
#include "samgr_lite.h"
#include "feature.h"
#include "service.h"
#include "player_type.h"

#include "player_server.h"

namespace OHOS {
struct DefaultFeatureApi {
    INHERIT_SERVER_IPROXY;
};

struct PlayerService {
    INHERIT_SERVICE;
    INHERIT_IUNKNOWNENTRY(DefaultFeatureApi);
    Identity identity;
};

static const char* GetName(Service* service)
{
    (void)service;
    return SERVICE_NAME;
}

static BOOL Initialize(Service* service, Identity identity)
{
    PlayerService* example = (PlayerService*)service;
    example->identity = identity;
    MEDIA_INFO_LOG("Initialize(%s)! Identity<%d, %d, %p>\n", SERVICE_NAME,
        identity.serviceId, identity.featureId, identity.queueId);
    return TRUE;
}

static BOOL MessageHandle(Service* service, Request* msg)
{
    MEDIA_INFO_LOG("MessageHandle(%s)! Request<%d, %d, %s>\n",
        service->GetName(service), msg->msgId, msg->msgValue, msg->data);
    return FALSE;
}

static TaskConfig GetTaskConfig(Service* service)
{
    (void)service;
    TaskConfig config = {LEVEL_HIGH, PRI_BELOW_NORMAL, 0x800, 20, SHARED_TASK};
    return config;
}

static int32 Invoke(IServerProxy* iProxy, int funcId, void* origin, IpcIo* req, IpcIo* reply)
{
    Media::PlayerServer::PlayerServerRequestHandle(funcId, origin, req, reply);
    return EC_SUCCESS;
}

static PlayerService g_example = {
    .GetName = GetName,
    .Initialize = Initialize,
    .MessageHandle = MessageHandle,
    .GetTaskConfig = GetTaskConfig,
    SERVER_IPROXY_IMPL_BEGIN,
    .Invoke = Invoke,
    IPROXY_END,
};

static void Init(void)
{
    SAMGR_GetInstance()->RegisterService((Service*)&g_example);
    SAMGR_GetInstance()->RegisterDefaultFeatureApi(SERVICE_NAME, GET_IUNKNOWN(g_example));
}

SYSEX_SERVICE_INIT(Init);
} // namespace OHOS
